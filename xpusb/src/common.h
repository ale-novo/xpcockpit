/* This is the common.h header to the usbiocards code

   Copyright (C) 2014  Reto Stockli

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. 
*/

#define INTERVAL 1  /* usbiocards loop interval in milliseconds, keep at 1 ms or below in order not to have
		     a buffer overflow for devices which report at 4 ms (BU836X) - 10 ms (iocards) or even less */

/* verbosity of usbiocards (0-4) */
int verbose;

int acf_type; /* aircraft type: 
		 0: any X-Plane aircraft
		 1: x737 by Benedikt Stratmann
		 2: Laminar B737-800
		 3: ZIBO MOD of Laminar B737-800
	      */
