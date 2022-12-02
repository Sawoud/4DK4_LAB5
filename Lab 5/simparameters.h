
/*
 * Simulation of the ALOHA Protocol
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

#ifndef _SIMPARAMETERS_H_
#define _SIMPARAMETERS_H_

/*******************************************************************************/

#define NUMBER_OF_STATIONS 2
#define MEAN_PACKET_DURATION 2      /* normalized packet Tx time, Xr */
#define PACKET_ARRIVAL_RATE 2    /* packets per Tx time */
#define MEAN_BACKOFF_DURATION 10    /* in units of packet transmit time, Tx */
#define GUARD_TIME 0.01 /* reservation guard time */

#define MEAN_DATA_PACKET_DURATION 1      /* normalized data packet Tx time, X */
#define MEAN_UPLOAD_DURATION 1    /* in units of packet transmit time, Tx */


#define RUNLENGTH 700000
#define BLIPRATE 100000



/* Comma separated list of random seeds to run. */
#define RANDOM_SEED_LIST 400072132

/*******************************************************************************/

#endif /* simparameters.h */






