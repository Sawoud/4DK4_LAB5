
/*
 * Simulation_Run of the ALOHA Protocol
 * 
 * Copyright (C) 2014 Terence D. Todd Hamilton, Ontario, CANADA
 * todd@mcmaster.ca
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

/*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include "trace.h"
#include "output.h"
#include "channel.h"
#include "packet_transmission.h"

/****************************************************************************************************************
Transmission start event for transmitting packets from mobile device to base station
*/

long int
schedule_transmission_start_event(Simulation_Run_Ptr simulation_run,
				  Time event_time,
				  void * packet) 
{
  Event event;

  event.description = "Start Of Packet";
  event.function = transmission_start_event;
  event.attachment = packet;

  return simulation_run_schedule_event(simulation_run, event, event_time);
}

void
transmission_start_event(Simulation_Run_Ptr simulation_run, void * ptr)
{
  Packet_Ptr this_packet;
  Simulation_Run_Data_Ptr data;
  Channel_Ptr channel;

  this_packet = (Packet_Ptr) ptr;
  data = (Simulation_Run_Data_Ptr) simulation_run_data(simulation_run);
  channel = data->channel;

  /* This packet is starting to transmit. */
  increment_transmitting_stn_count(channel);
  this_packet->status = TRANSMITTING;

  if(get_channel_state(channel) != IDLE) {
    /* The channel is now colliding. */
    set_channel_state(channel, COLLISION);
  } else {
    /* The channel is successful, for now. */
    set_channel_state(channel, SUCCESS);
  }

  /* Schedule the end of packet transmission event (S-ALOHA). */
  schedule_transmission_end_event(simulation_run,
				  simulation_run_get_time(simulation_run) + 
				  this_packet->upload_time + GUARD_TIME,
				  (void *) this_packet);
}

/******************************************************************************
Transmission end event for transmitting packets from mobile device to base station
*/

long int
schedule_transmission_end_event(Simulation_Run_Ptr simulation_run,
				Time event_time,
				void * packet)
{
  Event event;

  event.description = "End of Packet";
  event.function = transmission_end_event;
  event.attachment = packet;

  return simulation_run_schedule_event(simulation_run, event, event_time);
}

void
transmission_end_event(Simulation_Run_Ptr simulation_run, void* packet)
{
    Packet_Ptr this_packet, next_packet;
    Buffer_Ptr buffer;
    Time backoff_duration, now;
    Simulation_Run_Data_Ptr data;
    Channel_Ptr channel;

    data = (Simulation_Run_Data_Ptr)simulation_run_data(simulation_run);
    channel = data->channel;

    now = simulation_run_get_time(simulation_run);

    this_packet = (Packet_Ptr)packet;
    buffer = (data->stations + this_packet->station_id)->buffer;

    /* This mobile device has stopped transmitting. */
    decrement_transmitting_stn_count(channel);

    /* Check if the packet was successful. */
    if (get_channel_state(channel) == SUCCESS) {
        /* Get packet from queue */
        this_packet = fifoqueue_get(buffer);

        /* Transmission was a success. The channel is now IDLE. */
        set_channel_state(channel, IDLE);

        TRACE(printf("Success.\n"););

        /* Output activity blip every so often. */
        output_blip_to_screen(simulation_run);

        /* Collect Statistics */
        double packet_delay = simulation_run_get_time(simulation_run) - this_packet->arrive_time;

        data->packets_transmitted++;
        data->number_of_collisions += this_packet->collision_count;

        (data->stations + this_packet->station_id)->packets_transmitted++;
        (data->stations + this_packet->station_id)->number_of_collisions += this_packet->collision_count;

        /* Push packet to FIFO queue at cloud server */

        Packet_Ptr new_packet;
        Fifoqueue_Ptr cloud_server_queue;
        Server_Ptr cloud_server;

        new_packet = this_packet;
        cloud_server = data->cloud_server;
        cloud_server_queue = data->cloud_server_queue;
        
        /* Start transmission if the data link is free. Otherwise put the packet into
         * the buffer.  */

        if (server_state(cloud_server) == BUSY) {
            fifoqueue_put(cloud_server_queue, (void*)new_packet);
        }
        else {
            start_processing_on_cloud_server(simulation_run, new_packet, cloud_server);
        }

        /* See if there is another packet at this mobile device. If so, enable
           it for transmission. We will transmit immediately. */
        if (fifoqueue_size(buffer) > 0) {
            next_packet = fifoqueue_see_front(buffer);

            schedule_transmission_start_event(simulation_run,
                now + GUARD_TIME,
                (void*)next_packet);
        }
    }
    else {

        /* The transmission was unsuccessful. Clean up the channel state,
           backoff, and try again. */

        this_packet->collision_count++;
        this_packet->status = WAITING;

        TRACE(printf("Collision. Collision count = %i\n",
            this_packet->collision_count););

        /* If the collision is over, free up the channel. */
        if (get_transmitting_stn_count(channel) == 0) {
            set_channel_state(channel, IDLE);
        }

        backoff_duration = 2.0 * uniform_generator() * MEAN_BACKOFF_DURATION;

        schedule_transmission_start_event(simulation_run,
            now + backoff_duration,
            (void*)this_packet);
    }
}


/*******************************************************************************************************************
Event start for processing packets on cloud server
*/
void
start_processing_on_cloud_server(Simulation_Run_Ptr simulation_run,
    Packet_Ptr this_packet,
    Server_Ptr link)
{
    TRACE(printf("Start Of Processing.\n");)

        server_put(link, (void*)this_packet);
    this_packet->status = TRANSMITTING;

    /* Schedule the end of packet processing event. */
    schedule_end_packet_processing_event(simulation_run,
        simulation_run_get_time(simulation_run) + this_packet->service_time,
        (void*)link);
}

/******************************************************************************
Event end for processing packets on cloud server
*/

long
schedule_end_packet_processing_event(Simulation_Run_Ptr simulation_run,
    double event_time,
    Server_Ptr link)
{
    Event event;

    event.description = "Packet Processing End";
    event.function = end_packet_processing_event;
    event.attachment = (void*)link;

    return simulation_run_schedule_event(simulation_run, event, event_time);
}

void
end_packet_processing_event(Simulation_Run_Ptr simulation_run, void* link)
{
    Simulation_Run_Data_Ptr data;
    Packet_Ptr this_packet, next_packet;

    TRACE(printf("End Of Packet.\n"););

    data = (Simulation_Run_Data_Ptr)simulation_run_data(simulation_run);

    /* Packet transmission is finished. Take the packet off the data link. */
    this_packet = (Packet_Ptr)server_get(link);

    /* Output activity blip every so often. */
    output_blip_to_screen(simulation_run);

    /* Collect statistics. */
    double packet_delay = simulation_run_get_time(simulation_run) - this_packet->arrive_time;

    data->packets_processed++;
    data->accumulated_delay += packet_delay;

    (data->stations + this_packet->station_id)->packets_processed++;
    (data->stations + this_packet->station_id)->accumulated_delay += packet_delay;

    /* This packet is done ... give the memory back. */
    free((void*)this_packet);

    /*
     * See if there is are packets waiting in the buffer. If so, take the next one
     * out and transmit it immediately.
    */

    if (fifoqueue_size(data->cloud_server_queue) > 0) {
        next_packet = (Packet_Ptr)fifoqueue_get(data->cloud_server_queue);
        start_processing_on_cloud_server(simulation_run, next_packet, link);
    }
}


















