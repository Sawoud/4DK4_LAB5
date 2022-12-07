
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

#include "main.h"

/*******************************************************************************/

#ifndef _PACKET_TRANSMISSION_H_
#define _PACKET_TRANSMISSION_H_

/*******************************************************************************/

/*
 * Function prototypes
 */

void
transmission_start_event(Simulation_Run_Ptr, void *);

long int
schedule_transmission_start_event(Simulation_Run_Ptr, Time, void *);

void
transmission_end_event(Simulation_Run_Ptr, void *);

long int
schedule_transmission_end_event(Simulation_Run_Ptr, Time, void *);

/* Cloud Server */

void
start_processing_on_cloud_server(Simulation_Run_Ptr, Packet_Ptr, Server_Ptr);

void
end_packet_processing_event(Simulation_Run_Ptr, void*);

long int
schedule_end_packet_processing_event(Simulation_Run_Ptr, Time, Server_Ptr);

/*******************************************************************************/

#endif /* packet_transmission.h */




