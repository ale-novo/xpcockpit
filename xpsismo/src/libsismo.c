/* This is the libsismo.c code which contains all functions to interact with
   the SISMO SOLUCIONES hardware

   Copyright (C) 2020 Reto Stockli

   Additions for analog axes treatment by Hans Jansen 2011
   Also several cosmetic changes and changes for Linux compilation
   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <sys/types.h>

#include "common.h"
#include "libsismo.h"
#include "handleudp.h"

int verbose;

char xpserver_ip[30];
int xpserver_port;
char clientname[100];

char sismoserver_ip[30];
int sismoserver_port;

unsigned char recvBuffer[RECVMSGLEN];
unsigned char sendBuffer[SENDMSGLEN];

int ncards;
sismo_struct sismo[MAXCARDS];


/*Function to return the digit of n-th position of num. Position starts from 0*/
int get_digit(int num, int n)
{
    int r;
    r = num / pow(10, n);
    r = r % 10; 
    return r;
}

/* Function to set a specific bit (0-7) of a byte with a value 0 or 1 */
void set_bit(unsigned char *byte, int bit, int val)
{
  unsigned char mask = 1 << bit; // make sure bit n is 1
  if (val == 0) {
    mask ^= 0xff; // xor mask (only set bit n to 0
    *byte = *byte & mask;
  } else {
    *byte = *byte | mask;
  }
}

/* Function to get value of bit i (0-7) from byte */
int get_bit(unsigned char byte, int bit)
{
  return ((byte >> bit) & 0x01);
}

/* Function to set the 7 segments of a display where
   the segments are ordered in 8 bits of a byte and 
   the last bit is the decimal point */
void set_7segment(unsigned char *byte, int val, int dp)
{
  switch(val) {
  case 0:
    *byte = 0x7E;
    break;
  case 1:
    *byte = 0x30;
    break;
  case 2:
    *byte = 0x6D;
    break;
  case 3:
    *byte = 0x79;
    break;
  case 4:
    *byte = 0x33;
    break;
  case 5:
    *byte = 0x5B;
    break;
  case 6:
    *byte = 0x5F;
    break;
  case 7:
    *byte = 0x70;
    break;
  case 8:
    *byte = 0x7F;
    break;
  case 9:
    *byte = 0x7B;
    break;
  case -1:
    *byte = 0x01; /* minus sign */
    break;
  default:
    *byte = 0x00; /* all other: display off */
  }
  if (dp == 1) *byte = *byte | 0x80;

}

int read_sismo() {
  
  int card;
  int i,b,s;
  int val;
  int input;
  int any;
  
  while (udpReadLeft >= RECVMSGLEN) {
    
    card = -1;
    
    /* empty UDP receive buffer instead of directly accessing the device */

    pthread_mutex_lock(&exit_cond_lock);    
    /* read from start of receive buffer */
    memcpy(recvBuffer,&udpRecvBuffer[0],RECVMSGLEN);
    /* shift remaining read buffer to the left */
    memmove(&udpRecvBuffer[0],&udpRecvBuffer[RECVMSGLEN],udpReadLeft-RECVMSGLEN);    
    /* decrease read buffer position and counter */
    udpReadLeft -= RECVMSGLEN;
    pthread_mutex_unlock(&exit_cond_lock);

    /* decode message */

    /* check init string */
    if ((recvBuffer[0] == 0x53) && (recvBuffer[1] == 0x43)) {
      /* check MAC Address and Port of Card which did send the message */
      
      for (i=0;i<MAXCARDS;i++) {
	if ((sismo[i].mac[0] == recvBuffer[2]) && (sismo[i].mac[1] == recvBuffer[3]) &&
	    (sismo[i].port == recvBuffer[6] + 256 * recvBuffer[7])) card = i;
      }
      
      /* card found */
      if (card >= 0) {
	/* set # of inputs/outputs depending on activated daughters */
	sismo[card].noutputs = 64 + get_bit(recvBuffer[5],0)*64 + get_bit(recvBuffer[5],1)*64;
	sismo[card].nservos = get_bit(recvBuffer[5],2)*14;
	sismo[card].ndisplays = 32 + get_bit(recvBuffer[5],3)*32 + get_bit(recvBuffer[5],6)*32;
	sismo[card].naxes = 5 + get_bit(recvBuffer[5],4)*10;
	// sismo[card].analogoutputs = get_bit(recvBuffer[5],5)*XXX; /* Planned, but not available */
	  
	/* check type of input */
	if (recvBuffer[4] == 0x00) {
	  /* Input from Master */

	  /* Digital Inputs are ordered in 8 bytes with 8 bits each = 64 inputs */
	  /* Located in Bytes 8-15 */

	  /* check if any input has changed */
	  any = 0;
	  for (b=0;b<8;b++) {
	    for (i=0;i<8;i++) {
	      input = b*8+i;
	      val = get_bit(recvBuffer[8+b],i);
	      if (val != sismo[card].inputs[input][0]) any = 1;
	    }
	  }
	  /* if any input changed make sure we capture the whole history of changed
	     inputs, since during a single reception we can have several state changes */
	  if (any) {
	    for (b=0;b<8;b++) {
	      for (i=0;i<8;i++) {
		input = b*8+i;
		/* shift all inputs in history array by one */
		for (s=MAXSAVE-2;s>=0;s--) {
		  sismo[card].inputs[input][s+1] = sismo[card].inputs[input][s];
		}
		val = get_bit(recvBuffer[8+b],i);
		/* update changed flag for changed inputs */
		if (val != sismo[card].inputs[input][0]) {
		  sismo[card].inputs_changed[input] += 1;
		  if (verbose > 0) printf("Card %i Input %i Changed from %i to: %i \n",card,
					  input,sismo[card].inputs[input][1],sismo[card].inputs[input][0]);
		}
		/* update all inputs */
		sismo[card].inputs[input][0] = val; 
	      }
	    }
	  }

	  /* Analog Inputs are ordered in 10 bytes with 2 bytes per input */
	  /* Located in bytes 16-25 */
	  for (i=0;i<5;i++) {
	    val = recvBuffer[16+i*2] + 256 * recvBuffer[17+i*2];
	    if (val != sismo[card].axes[i]) {
	      sismo[card].axes[i] = val;
	      sismo[card].axes_changed[i] += 1;
	      if (verbose > 2) printf("Card %i Analog Input %i Changed to %i \n",card,i,sismo[card].axes[i]);
	    }				      
	  }
	  
	} else if (recvBuffer[4] == 0x01) {
	  /* Input from Daughter Digital Inputs 1 */
	} else if (recvBuffer[4] == 0x02) {
	  /* Input from Daughter Digital Inputs 2 */
	} else if (recvBuffer[4] == 0x03) {
	  /* Inputs from Daughter Analog Inputs */
	} else {
	  printf("Unknown Inputs Type: %02x \n",recvBuffer[4]);
	}
	  
	
	//printf("Left to Read: %i \n",udpReadLeft);
	
      } else {
	printf("Card with MAC %02x:%02x or Port %i is not defined in ini file \n",
	       recvBuffer[2],recvBuffer[3],recvBuffer[6] + 256 * recvBuffer[7]);
      }
    } else {
      printf("Received wrong Init String: %02x %02x \n",recvBuffer[0],recvBuffer[1]);
    }

  } /* while UDP data present in receive buffer */

  return 0;
}

int write_sismo() {

  return 0;
}

void test() {
  int i,b;
  int val;
  int ret;
  
  unsigned char initstring[2] = "SC";
  unsigned char sendto_mb = 0x00; // Mother Board
  unsigned char outstype_out = 0x00; // Digital Outputs
  unsigned char outstype_dis = 0x01; // 7 Segment Displays
  int num_ana = 5;
  int num_inp = 64;
  int num_out = 64;
  int num_dis = 32;
  int ana[num_ana];
  int inp[num_inp];
  int out[num_out];
  int dis[num_dis];
  
  struct timeval tval_before, tval_after, tval_result;

  /* Send Outputs */
  memset(sendBuffer,0,SENDMSGLEN);

  sendBuffer[0] = initstring[0];
  sendBuffer[1] = initstring[1];
  sendBuffer[2] = sendto_mb;
  sendBuffer[3] = outstype_out;


  i=2; // set output number
  val=1; // set output value
  //  sendBuffer[4+(i-1)/8] = 0xff;
  set_bit(&sendBuffer[4+(i-1)/8],(i-1)%8,val);
  //  ret = send_udp();

  printf("Sent %i bytes \n", ret);

  gettimeofday(&tval_before, NULL);
  

    
    /* Send Displays */
    
    memset(sendBuffer,0,SENDMSGLEN);
    
    sendBuffer[0] = initstring[0];
    sendBuffer[1] = initstring[1];
    sendBuffer[2] = sendto_mb;
    sendBuffer[3] = outstype_dis;      
    sendBuffer[4] = 1; // Display Group 1-4 (each display group has 8 displays)
    sendBuffer[13] = 0x0f; // brightness (0-15 or 0x00 - 0x0f)
    for (i=0;i<5;i++) {
      set_7segment(&sendBuffer[5+i%8],get_digit(ana[0],i),0);
    }
    ret = send_udp(sismo[0].ip,sismo[0].port,sendBuffer,SENDMSGLEN);
    
    printf("Sent %i bytes \n", ret);
    
    usleep(1000);
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    printf("Time elapsed: %ld.%06ld\n",
	   (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
    //}

}
