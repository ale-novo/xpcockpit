/* This is the b737_pedestal.c code which connects the Boeing 737 center pedestal via the SISMO Ethernet Card
   to X-Plane

   Copyright (C) 2023 Reto Stockli

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
#include "serverdata.h"
#include "b737_pedestal.h"

#define max(A,B) ((A)>(B) ? (A) : (B)) 
#define min(A,B) ((A)<(B) ? (A) : (B)) 

/* Common Variables */
int adf1_tfr_button;
int adf1_ant_switch;
int adf1_tone_switch;

int nav1_tfr_button;
int nav1_test_button;

int com1_tfr_button;
int com1_test_button;

int xpndr_sel_switch;
int xpndr_src_switch;
int xpndr_mode_select;

int cargofire_test;
int cargofire_select_fwd_a;
int cargofire_select_fwd_b;
int cargofire_select_aft_a;
int cargofire_select_aft_b;
int cargofire_arm_fwd;
int cargofire_arm_aft;
int cargofire_disch;

int cargofire_fault;
int cargofire_test_fwd;
int cargofire_test_aft;

int wxr_mode_test;
int wxr_mode_wx;
int wxr_mode_turb;

int stab_trim_ovrd;
int flt_deck_door_unlock;
int flt_deck_door_auto;
int flt_deck_door_deny;
int flt_deck_door_pos;

void b737_pedestal(void)
{

  /* local variables */
  int card = 2; /* SISMO card according to configuration */
  
  int ret;
  int updn;
  int dp;
  int temp;
  int temp2;
  int integer; /* integer part of displays */
  int decimal; /* decimal part of displays */
  int i0; /* first input # per panel */
  int o0; /* first output # per panel */
  int d0; /* first display # per panel */
  int a0; /* first analog # per panel */

  /* parameters */
  /* X-Plane frequencies are scaled by 10 kHz except for adf and dme */
  int zero = 0;
  int one = 1;
  int ten = 10;
  int nav_min = 108; // MHz
  int nav_max = 117; // MHz
  int com_min = 118; // MHz
  int com_max = 136; // MHz
  int adf_min = 190; // kHz
  int adf_max = 1750; // kHz
  int transponder_min = 0;
  int transponder_max = 7;

  float trim_min = -1.0;
  float trim_max = 1.0;

  float flood_brightness;
  float panel_brightness;

  float fvalue;



  /* x-plane data */
  int *nav1_freq_active = link_dataref_int("sim/cockpit/radios/nav1_freq_hz");
  int *nav2_freq_active = link_dataref_int("sim/cockpit/radios/nav2_freq_hz");
  int *com1_freq_active = link_dataref_int("sim/cockpit2/radios/actuators/com1_frequency_hz_833");
  int *com2_freq_active = link_dataref_int("sim/cockpit2/radios/actuators/com2_frequency_hz_833");
  int *adf1_freq_active = link_dataref_int("sim/cockpit/radios/adf1_freq_hz");
  int *adf2_freq_active = link_dataref_int("sim/cockpit/radios/adf2_freq_hz");
  int *nav1_freq_stdby = link_dataref_int("sim/cockpit/radios/nav1_stdby_freq_hz");
  int *nav2_freq_stdby = link_dataref_int("sim/cockpit/radios/nav2_stdby_freq_hz");
  int *com1_freq_stdby = link_dataref_int("sim/cockpit2/radios/actuators/com1_standby_frequency_hz_833");
  int *com2_freq_stdby = link_dataref_int("sim/cockpit2/radios/actuators/com2_standby_frequency_hz_833");
  int *adf1_freq_stdby = link_dataref_int("sim/cockpit/radios/adf1_stdby_freq_hz");
  int *adf2_freq_stdby = link_dataref_int("sim/cockpit/radios/adf2_stdby_freq_hz"); 

  int *transponder_code = link_dataref_int("sim/cockpit/radios/transponder_code");
  int *transponder_fail = link_dataref_int("sim/operation/failures/rel_xpndr");

  int *transponder_ident;
  float *transponder_mode_f;
  int *transponder_mode_up;
  int *transponder_mode_dn;
  if ((acf_type == 2) || (acf_type == 3)) {
    transponder_mode_f = link_dataref_flt("laminar/B738/knob/transponder_pos",0);
    transponder_mode_up = link_dataref_cmd_once("laminar/B738/knob/transponder_mode_up");
    transponder_mode_dn = link_dataref_cmd_once("laminar/B738/knob/transponder_mode_dn");
    *transponder_mode_up = 0;
    *transponder_mode_dn = 0;
    transponder_ident = link_dataref_cmd_once("laminar/B738/push_button/transponder_ident_dn");
  } else {
    transponder_ident = link_dataref_cmd_once("sim/transponder/transponder_ident");
  }
  int *transponder_mode = link_dataref_int("sim/cockpit/radios/transponder_mode");
    
  float *rudder_trim = link_dataref_flt("sim/flightmodel/controls/rud_trim",-3);
  int *rudder_trim_left = link_dataref_cmd_hold("sim/flight_controls/rudder_trim_left");
  int *rudder_trim_right = link_dataref_cmd_hold("sim/flight_controls/rudder_trim_right");
  
  //  float *aileron_trim = link_dataref_flt("sim/flightmodel/controls/ail_trim",-4);
  int *aileron_trim_left = link_dataref_cmd_hold("sim/flight_controls/aileron_trim_left");
  int *aileron_trim_right = link_dataref_cmd_hold("sim/flight_controls/aileron_trim_right");

  /* FIRE SWITCHES ETC */
  int *fire_test; 
  int *fire_test_ovht; 
  int *fire_test_fault; 
  int *fire_eng1_ovht_det; 
  int *fire_eng2_ovht_det;
  int *fire_eng_ext_test;
  int *fire_eng_ext_test_left;
  int *fire_eng_ext_test_right;
  float *fire_eng1_ovht_ann;
  float *fire_eng2_ovht_ann;
  float *fire_wheelwell_ann;
  float *fire_fault_ann;
  float *fire_apu_det_inop_ann;
  float *fire_apu_bottle_disch_ann;
  float *fire_bottle_l_disch_ann;
  float *fire_bottle_r_disch_ann;
  float *fire_eng1_ann;
  float *fire_apu_ann;
  float *fire_eng2_ann;
  float *fire_eng1_test_ann;
  float *fire_apu_test_ann;
  float *fire_eng2_test_ann;
  int *fire_eng1_pulled;
  int *fire_eng1_rotate; 
  int *fire_eng1_rotate_l; 
  int *fire_eng1_rotate_r; 
  int *fire_apu_pulled;
  int *fire_apu_rotate; 
  int *fire_apu_rotate_l; 
  int *fire_apu_rotate_r; 
  int *fire_eng2_pulled;
  int *fire_eng2_rotate;
  int *fire_eng2_rotate_l;
  int *fire_eng2_rotate_r;
  float *fire_eng1_pulled_status;
  float *fire_eng1_rotate_status; 
  float *fire_apu_pulled_status;
  float *fire_apu_rotate_status; 
  float *fire_eng2_pulled_status;
  float *fire_eng2_rotate_status;
  if ((acf_type == 2) || (acf_type == 3)) {
    fire_test_fault = link_dataref_cmd_hold("laminar/B738/toggle_switch/fire_test_lft"); 
    fire_test_ovht = link_dataref_cmd_hold("laminar/B738/toggle_switch/fire_test_rgt"); 
    fire_eng_ext_test_left = link_dataref_cmd_hold("laminar/B738/toggle_switch/exting_test_lft"); 
    fire_eng_ext_test_right = link_dataref_cmd_hold("laminar/B738/toggle_switch/exting_test_rgt"); 

    fire_eng1_ovht_ann = link_dataref_flt("laminar/B738/annunciator/engine1_ovht",-1);
    fire_eng2_ovht_ann = link_dataref_flt("laminar/B738/annunciator/engine2_ovht",-1);
    fire_wheelwell_ann = link_dataref_flt("laminar/B738/annunciator/wheel_well_fire",-1);
    fire_fault_ann = link_dataref_flt("laminar/B738/annunciator/fire_fault_inop",-1);
    fire_apu_det_inop_ann = link_dataref_flt("laminar/B738/annunciator/fire_fault_inop",-1); // PLEASE REPLACE!!!
    fire_apu_bottle_disch_ann = link_dataref_flt("laminar/B738/annunciator/apu_bottle_discharge",-1);
    fire_bottle_l_disch_ann = link_dataref_flt("laminar/B738/annunciator/l_bottle_discharge",-1);
    fire_bottle_r_disch_ann = link_dataref_flt("laminar/B738/annunciator/r_bottle_discharge",-1);
    fire_eng1_ann = link_dataref_flt("laminar/B738/annunciator/engine1_fire",-1);
    fire_apu_ann = link_dataref_flt("laminar/B738/annunciator/apu_fire",-1);
    fire_eng2_ann = link_dataref_flt("laminar/B738/annunciator/engine2_fire",-1);
    fire_eng1_test_ann = link_dataref_flt("laminar/B738/annunciator/extinguisher_circuit_annun_left",-1);
    fire_apu_test_ann = link_dataref_flt("laminar/B738/annunciator/extinguisher_circuit_annun_apu",-1);
    fire_eng2_test_ann = link_dataref_flt("laminar/B738/annunciator/extinguisher_circuit_annun_right",-1);
   

    fire_eng1_pulled = link_dataref_cmd_once("laminar/B738/fire/engine01/ext_switch_arm"); 
    fire_eng1_rotate_l = link_dataref_cmd_once("laminar/B738/fire/engine01/ext_switch_L");
    fire_eng1_rotate_r = link_dataref_cmd_once("laminar/B738/fire/engine01/ext_switch_R");
    fire_apu_pulled = link_dataref_cmd_once("laminar/B738/fire/apu/ext_switch_arm"); 
    fire_apu_rotate_l = link_dataref_cmd_once("laminar/B738/fire/apu/ext_switch_L"); 
    fire_apu_rotate_r = link_dataref_cmd_once("laminar/B738/fire/apu/ext_switch_R"); 
    fire_eng2_pulled = link_dataref_cmd_once("laminar/B738/fire/engine02/ext_switch_arm"); 
    fire_eng2_rotate_l = link_dataref_cmd_once("laminar/B738/fire/engine02/ext_switch_L"); 
    fire_eng2_rotate_r = link_dataref_cmd_once("laminar/B738/fire/engine02/ext_switch_R"); 

    *fire_eng1_pulled = 0;
    *fire_eng1_rotate_l = 0;
    *fire_eng1_rotate_r = 0;
    *fire_apu_pulled = 0;
    *fire_apu_rotate_l = 0;
    *fire_apu_rotate_r = 0;
    *fire_eng2_pulled = 0;
    *fire_eng2_rotate_l = 0;
    *fire_eng2_rotate_r = 0;
    
    fire_eng1_pulled_status = link_dataref_flt("laminar/B738/fire/engine01/ext_switch/pos_arm",-2); 
    fire_eng1_rotate_status = link_dataref_flt("laminar/B738/fire/engine01/ext_switch/pos_disch",-2);
    fire_apu_pulled_status = link_dataref_flt("laminar/B738/fire/apu/ext_switch/pos_arm",-2); 
    fire_apu_rotate_status = link_dataref_flt("laminar/B738/fire/apu/ext_switch/pos_disch",-2);
    fire_eng2_pulled_status = link_dataref_flt("laminar/B738/fire/engine02/ext_switch/pos_arm",-2); 
    fire_eng2_rotate_status = link_dataref_flt("laminar/B738/fire/engine02/ext_switch/pos_disch",-2);
   
    /*
    fire_eng1_ovht_det = link_dataref_int("");
    fire_eng2_ovht_det = link_dataref_int("");
    */
  } else if (acf_type == 1) {
    fire_test = link_dataref_int("x737/cockpit/fireSupPanel/fireWarnTestSw_state"); // 0: fault, 1: off, 2: ovht
    fire_eng1_ovht_det = link_dataref_int("x737/cockpit/fireSupPanel/Eng1OvhtDetSw_state"); // 0: A, 1: NORMAL, 2: B
    fire_eng2_ovht_det = link_dataref_int("x737/cockpit/fireSupPanel/Eng2OvhtDetSw_state"); // 0: A, 1: NORMAL, 2: B
    fire_eng_ext_test = link_dataref_int("x737/cockpit/fireSupPanel/fireExtTestSw_state"); //  0: '1', 1: center, 2: '2'
    fire_eng1_pulled = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle1Pulled_state"); // 0: down, 1: pulled
    fire_eng1_rotate = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle1Rotate_state"); // -1: 'L', 0: center, 1: 'R'
    fire_apu_pulled = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle2Pulled_state"); // 0: down, 1: pulled
    fire_apu_rotate = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle2Rotate_state"); // -1: 'L', 0: center, 1: 'R'
    fire_eng2_pulled = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle3Pulled_state"); // 0: down, 1: pulled
    fire_eng2_rotate = link_dataref_int("x737/cockpit/fireSupPanel/fireHandle3Rotate_state"); // -1: 'L', 0: center, 1: 'R'
    
    fire_eng1_ovht_ann = link_dataref_flt("x737/cockpit/fireSupPanel/Eng1OvhtDet_annunc",-1);
    fire_eng2_ovht_ann = link_dataref_flt("x737/cockpit/fireSupPanel/Eng2OvhtDet_annunc",-1);
    fire_wheelwell_ann = link_dataref_flt("x737/cockpit/fireSupPanel/wheelWell_annunc",-1);
    fire_fault_ann = link_dataref_flt("x737/cockpit/fireSupPanel/fault_annunc",-1);
    fire_apu_det_inop_ann = link_dataref_flt("x737/cockpit/fireSupPanel/APUDetInop_annunc",-1);
    fire_apu_bottle_disch_ann = link_dataref_flt("x737/cockpit/fireSupPanel/APUBotDisch_annunc",-1);
    fire_bottle_l_disch_ann = link_dataref_flt("x737/cockpit/fireSupPanel/bottle1Disch_annunc",-1);
    fire_bottle_r_disch_ann = link_dataref_flt("x737/cockpit/fireSupPanel/bottle2Disch_annunc",-1);
    fire_eng1_ann = link_dataref_flt("x737/cockpit/fireSupPanel/fireHandle1_annunc",-1);
    fire_apu_ann = link_dataref_flt("x737/cockpit/fireSupPanel/fireHandle2_annunc",-1);
    fire_eng2_ann = link_dataref_flt("x737/cockpit/fireSupPanel/fireHandle3_annunc",-1);
    fire_eng1_test_ann = link_dataref_flt("x737/cockpit/fireSupPanel/fireExtTest1Btn_annunc",-1);
    fire_apu_test_ann = link_dataref_flt("x737/cockpit/fireSupPanel/fireExtTest2Btn_annunc",-1);
    fire_eng2_test_ann = link_dataref_flt("x737/cockpit/fireSupPanel/fireExtTest3Btn_annunc",-1);
  }
  //  int *engine_fires = link_dataref_int_arr("sim/cockpit2/annunciators/engine_fires",8,-1);



  /* TEMPORARY ASSIGNMENT TO BETTERPUSHBACK */
  int *vhf1 = link_dataref_cmd_once("BetterPushback/start");
  int *vhf2 = link_dataref_cmd_once("BetterPushback/stop");
  int *vhf3 = link_dataref_cmd_once("BetterPushback/disconnect");
  int *hf1 = link_dataref_cmd_once("BetterPushback/connect_first");
  int *hf2 = link_dataref_cmd_once("BetterPushback/cab_camera");

  /* WXR Gain and Tilt */
  float *wxr_gain = link_dataref_flt("xpserver/wxr_gain",-2);
  float *wxr_tilt = link_dataref_flt("xpserver/wxr_tilt",-2);
  
  float *flt_deck_door;
  int *flt_deck_door_left;
  int *flt_deck_door_right;
  float *lock_fail;
  float *auto_unlk;
  if ((acf_type == 2) || (acf_type == 3)) {
    flt_deck_door = link_dataref_flt("laminar/B738/toggle_switch/flt_dk_door",0);
    flt_deck_door_left = link_dataref_cmd_once("laminar/B738/toggle_switch/flt_dk_door_left");
    flt_deck_door_right = link_dataref_cmd_once("laminar/B738/toggle_switch/flt_dk_door_right");
    lock_fail = link_dataref_flt("laminar/B738/annunciator/door_lock_fail",0);
    auto_unlk = link_dataref_flt("laminar/B738/annunciator/door_auto_unlk",0);
  } else {
    flt_deck_door = link_dataref_flt("xpserver/flt_dk_door",0);
    flt_deck_door_left = link_dataref_int("xpserver/flt_dk_door_left");
    flt_deck_door_right = link_dataref_int("xpserver/flt_dk_door_right");    
    lock_fail = link_dataref_flt("xpserver/lock_fail",0);
    auto_unlk = link_dataref_flt("xpserver/auto_unlk",0);
  }
    
  float *radio_volume1;
  float *radio_volume2;
  if ((acf_type == 2) || (acf_type == 3)) {
    radio_volume1 = link_dataref_flt("laminar/B738/comm/audio_vol_com1",-1);
    radio_volume2 = link_dataref_flt("laminar/B738/comm/audio_vol_com2",-1);
  } else {
    radio_volume1 = link_dataref_flt("sim/cockpit2/radios/actuators/audio_volume_com1",-1);
    radio_volume2 = link_dataref_flt("sim/cockpit2/radios/actuators/audio_volume_com1",-1);
  }
  float *master_volume = link_dataref_flt("sim/operation/sound/master_volume_ratio",-1);

  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");

  /* blank displays if avionics are off */
  int blank = 0;
  if (*avionics_on != 1) blank = 1;

  if (0) {
  
  /*** ADF1 Panel ***/ 
  i0 = 0;
  d0 = 0;
  
  /* ADF1 tfr button */
  ret = digital_input(card,i0+0,&adf1_tfr_button,0);
  if (ret == 1) {
    printf("ADF1 TFR Button: %i \n",adf1_tfr_button);
    if (adf1_tfr_button == 1) {
      temp = *adf1_freq_active;
      *adf1_freq_active = *adf1_freq_stdby;
      *adf1_freq_stdby = temp;
    }
  }
  /* ADF1 ANT switch */
  ret = digital_input(card,i0+1,&adf1_ant_switch,0);
  if (ret == 1) {
    printf("ADF1 ANT SWITCH: %i \n",adf1_ant_switch);
  }
  /* ADF1 TONE switch */
  ret = digital_input(card,i0+2,&adf1_tone_switch,0);
  if (ret == 1) {
    printf("ADF1 TONE SWITCH: %i \n",adf1_tone_switch);
  }
  /* ADF1 Outer Encoder (100 kHz step) */
  updn = 0;
  ret = encoder_input(card,i0+3,i0+4,&updn,100,1);
  if (ret == 1) {
    if (*adf1_freq_stdby != INT_MISS) {
      *adf1_freq_stdby += updn;
      *adf1_freq_stdby = min(max(*adf1_freq_stdby,adf_min),adf_max);
      printf("ADF1 STDBY FREQ: %i \n",*adf1_freq_stdby);
    }
  }
  /* ADF1 Inner Encoder (1 kHz step) */
  updn = 0;
  ret = encoder_input(card,i0+5,i0+6,&updn,1,1);
  if (ret == 1) {
    if (*adf1_freq_stdby != INT_MISS) {
      integer = *adf1_freq_stdby / 100;
      decimal = *adf1_freq_stdby - integer * 100;
      decimal += updn;
      if (decimal < 0) decimal = 99;
      if (decimal > 99) decimal = 0;
      *adf1_freq_stdby = integer * 100 + decimal;
      *adf1_freq_stdby = min(max(*adf1_freq_stdby,adf_min),adf_max);
      printf("ADF1 STDBY FREQ: %i \n",*adf1_freq_stdby);
    }
  }
  /* ADF1 Displays */
  temp = *adf1_freq_active * 10;
  ret = display_output(card, d0+0, 5, &temp, 1, blank);
  temp = *adf1_freq_stdby * 10;
  ret = display_output(card, d0+8, 5, &temp, 1, blank);

  
  /*** NAV1 Panel ***/ 
  i0 = 0;
  d0 = 0;
  
  /* NAV1 tfr button */
  ret = digital_input(card,i0+0,&nav1_tfr_button,0);
  if (ret == 1) {
    printf("NAV1 TFR Button: %i \n",nav1_tfr_button);
    if (nav1_tfr_button == 1) {
      temp = *nav1_freq_active;
      *nav1_freq_active = *nav1_freq_stdby;
      *nav1_freq_stdby = temp;
    }
  }
  /* NAV1 Test Button */
  ret = digital_input(card,i0+1,&nav1_test_button,0);
  if (ret == 1) {
    printf("NAV1 TEST Button: %i \n",nav1_test_button);
  }
  /* NAV1 Outer Encoder (1 MHz step) */
  updn = 0;
  ret = encoder_input(card,i0+2,i0+3,&updn,1,1);
  if (ret == 1) {
    if (*nav1_freq_stdby != INT_MISS) {
      integer = *nav1_freq_stdby / 100;
      decimal = *nav1_freq_stdby - integer * 100;
      integer += updn;
      if (integer < nav_min) integer = nav_max;
      if (integer > nav_max) integer = nav_min;
      *nav1_freq_stdby = integer * 100 + decimal;
      printf("NAV1 STDBY FREQ: %i \n",*nav1_freq_stdby);
    }
  }
  /* NAV1 Inner Encoder (50 kHz step) */
  updn = 0;
  ret = encoder_input(card,i0+4,i0+5,&updn,5,1);
  if (ret == 1) {
    if (*nav1_freq_stdby != INT_MISS) {
      integer = *nav1_freq_stdby / 100;
      decimal = *nav1_freq_stdby - integer * 100;
      decimal += updn;
      if (decimal < 0) decimal = 95;
      if (decimal > 95) decimal = 0;
      *nav1_freq_stdby = integer * 100 + decimal;
      printf("NAV1 STDBY FREQ: %i \n",*nav1_freq_stdby);
    }
  }
  /* NAV1 Displays */
  ret = display_output(card, d0+0, 5, nav1_freq_active, 2, blank);
  ret = display_output(card, d0+8, 5, nav1_freq_stdby, 2, blank);

  }
 
  if (0) {
  
  /*** COM1 Panel ***/ 
  i0 = 0;
  d0 = 0;
  
  /* COM1 tfr button */
  ret = digital_input(card,i0+0,&com1_tfr_button,0);
  if (ret == 1) {
    printf("COM1 TFR Button: %i \n",com1_tfr_button);
    if (com1_tfr_button == 1) {
      temp = *com1_freq_active;
      *com1_freq_active = *com1_freq_stdby;
      *com1_freq_stdby = temp;
    }
  }
  /* COM1 Test Button */
  ret = digital_input(card,i0+1,&com1_test_button,0);
  if (ret == 1) {
    printf("COM1 TEST Button: %i \n",com1_test_button);
  }
  /* COM1 Outer Encoder (1 MHz step) */
  updn = 0;
  ret = encoder_input(card,i0+2,i0+3,&updn,1,1);
  if (ret == 1) {
    if (*com1_freq_stdby != INT_MISS) {
      integer = *com1_freq_stdby / 1000;
      decimal = *com1_freq_stdby - integer * 1000;
      integer += updn;
      if (integer < nav_min) integer = nav_max;
      if (integer > nav_max) integer = nav_min;
      *com1_freq_stdby = integer * 1000 + decimal;
      printf("COM1 STDBY FREQ: %i \n",*com1_freq_stdby);
    }
  }
  /* COM1 Inner Encoder (5 kHz step) */
  updn = 0;
  ret = encoder_input(card,i0+4,i0+5,&updn,5,1);
  if (ret == 1) {
    if (*com1_freq_stdby != INT_MISS) {
      integer = *com1_freq_stdby / 1000;
      decimal = *com1_freq_stdby - integer * 1000;
      decimal += updn;
      if (decimal < 0) decimal = 995;
      if (decimal > 995) decimal = 0;
      *com1_freq_stdby = integer * 1000 + decimal;
      printf("COM1 STDBY FREQ: %i \n",*com1_freq_stdby);
    }
  }
  /* COM1 Displays */
  ret = display_output(card, d0+0, 6, com1_freq_active, 3, blank);
  ret = display_output(card, d0+8, 6, com1_freq_stdby, 3, blank);

  }
  
  if (0) {

  /*** TRANSPONDER PANEL ***/
  i0 = 0;
  d0 = 0;

  /* XPNDR IDENT button */
  ret = digital_input(card,i0+8,transponder_ident,0);
  if (ret == 1) {
    printf("XPNDR IDENT BUTTON: %i \n",*transponder_ident);
  }
  /* XPNDR Selection switch */
  ret = digital_input(card,i0+9,&xpndr_sel_switch,0);
  if (ret == 1) {
    printf("XPNDR SEL SWITCH: %i \n",xpndr_sel_switch);
  }
  /* XPNDR Source switch */
  ret = digital_input(card,i0+10,&xpndr_src_switch,0);
  if (ret == 1) {
    printf("XPNDR SRC SWITCH: %i \n",xpndr_src_switch);
  }
  /* XPNDR Mode Select */
  temp = 0;
  ret = digital_input(card,i0+11,&temp,0);
  if ((ret == 1) && (xpndr_mode_select != 1)) {
    xpndr_mode_select = 1;
    printf("XPNDR MODE SELECT: %i \n",xpndr_mode_select);
  }
  temp = 0;
  ret = digital_input(card,i0+12,&temp,0);
  if ((ret == 1) && (xpndr_mode_select != 2)) {
    xpndr_mode_select = 2;
    printf("XPNDR MODE SELECT: %i \n",xpndr_mode_select);
  }
  temp = 0;
  ret = digital_input(card,i0+13,&temp,0);
  if ((ret == 1) && (xpndr_mode_select != 3)) {
    xpndr_mode_select = 3;
    printf("XPNDR MODE SELECT: %i \n",xpndr_mode_select);
  }
  temp = 0;
  ret = digital_input(card,i0+14,&temp,0);
  if ((ret == 1) && (xpndr_mode_select != 4)) {
    xpndr_mode_select = 4;
    printf("XPNDR MODE SELECT: %i \n",xpndr_mode_select);
  }
  temp = 0;
  ret = digital_input(card,i0+15,&temp,0);
  if ((ret == 1) && (xpndr_mode_select != 5)) {
    xpndr_mode_select = 5;
    printf("XPNDR MODE SELECT: %i \n",xpndr_mode_select);
  }

  if ((acf_type == 2) || (acf_type == 3)) {
    float xpndr_mode_select_f = (float) xpndr_mode_select;
    ret = set_state_updnf(&xpndr_mode_select_f,transponder_mode_f,transponder_mode_up,transponder_mode_dn);
    if (ret != 0) {
      printf("XPNDR MODE: %f %f %i %i \n",
	     xpndr_mode_select_f,*transponder_mode_f,*transponder_mode_up,*transponder_mode_dn);
    }
  } else {
    *transponder_mode = xpndr_mode_select;
  }

  
  if (*transponder_code != INT_MISS) {
    /* XPNDR Encoder Digits 1's */
    updn = 0;
    ret = encoder_input(card,i0+6,i0+7,&updn,1,1);
    if (ret == 1) {
      temp = *transponder_code - (*transponder_code / 10) * 10;
      *transponder_code -= temp;
      temp += updn;
      if (temp > transponder_max) temp = transponder_min;
      if (temp < transponder_min) temp = transponder_max;
      *transponder_code += temp; 
      printf("TRANSPONDER CODE: %i\n",*transponder_code);
    }
    /* XPNDR Encoder Digits 10's */
    updn = 0;
    ret = encoder_input(card,i0+4,i0+5,&updn,1,1);
    if (ret == 1) {
      temp = (*transponder_code - (*transponder_code / 100) * 100)/10;
      *transponder_code -= 10*temp;
      temp += updn;
      if (temp > transponder_max) temp = transponder_min;
      if (temp < transponder_min) temp = transponder_max;
      *transponder_code += 10*temp; 
      printf("TRANSPONDER CODE: %i\n",*transponder_code);
    }
    /* XPNDR Encoder Digits 100's */
    updn = 0;
    ret = encoder_input(card,i0+2,i0+3,&updn,1,1);
    if (ret == 1) {
      temp = (*transponder_code - (*transponder_code / 1000) * 1000)/100;
      *transponder_code -= 100*temp;
      temp += updn;
      if (temp > transponder_max) temp = transponder_min;
      if (temp < transponder_min) temp = transponder_max;
      *transponder_code += 100*temp; 
      printf("TRANSPONDER CODE: %i\n",*transponder_code);
    }
    /* XPNDR Encoder Digits 1000's */
    updn = 0;
    ret = encoder_input(card,i0+0,i0+1,&updn,1,1);
    if (ret == 1) {
      temp = *transponder_code / 1000;
      *transponder_code -= 1000*temp;
      temp += updn;
      if (temp > transponder_max) temp = transponder_min;
      if (temp < transponder_min) temp = transponder_max;
      *transponder_code += 1000*temp; 
      printf("TRANSPONDER CODE: %i\n",*transponder_code);
    }
    
    /* Digits 1's */
    temp = *transponder_code - (*transponder_code / 10) * 10;
    ret = display_output(card, d0+6, 1, &temp, -1, blank);
    /* Digits 10's */
    temp = *transponder_code / 10 - (*transponder_code / 100) * 10;
    ret = display_output(card, d0+7, 1, &temp, -1, blank);
    /* Digits 100's */
    temp = *transponder_code / 100 - (*transponder_code / 1000)*10;
    ret = display_output(card, d0+8+6, 1, &temp, -1, blank);
    /* Digits 1000's */
    if (*transponder_fail != INT_MISS) {
      dp = -1 + *transponder_fail; /* set major digit DP for transponder fail led */
    } else {
      dp = -1;
    }
    temp = *transponder_code / 1000;
    ret = display_output(card, d0+8+7, 1, &temp, dp, blank);

  } else {
    temp = 0;
    ret = display_output(card, d0+6, 2, &temp, -1, 1);
    ret = display_output(card, d0+8+6, 2, &temp, -1, 1);
  }

  }


  if (0) {
  
  /**** RUDDER TRIM PANEL ***/
  i0 = 0;
  o0 = 0;

  /* Aileron Trim Left switch 1 */
  ret = digital_input(card,i0+1,&temp,0);
  /* Aileron Trim Left switch 2 */
  ret = digital_input(card,i0+3,&temp2,0);
  /* Only activate Aileron Trim if both Switches are activated */
  if ((temp == 1) && (temp2 == 1)) {
    *aileron_trim_left = 1;
  } else {
    *aileron_trim_left = 0;
  }
  /* Aileron Trim Right switch 1 */
  ret = digital_input(card,i0+2,&temp,0);
  /* Aileron Trim Right switch 2 */
  ret = digital_input(card,i0+4,&temp2,0);
  /* Only activate Aileron Trim if both Switches are activated */
  if ((temp == 1) && (temp2 == 1)) {
    *aileron_trim_right = 1;
  } else {
    *aileron_trim_right = 0;
  }
  
  /* Rudder Trim Left switch */
  ret = digital_input(card,i0+5,rudder_trim_left,0);
  if (ret == 1) {
    printf("RUDDER TRIM LEFT SWITCH \n");
  }
  /* Rudder Trim Right switch */
  ret = digital_input(card,i0+6,rudder_trim_right,0);
  if (ret == 1) {
    printf("RUDDER TRIM RIGHT SWITCH \n");
  }

  /*  Rudder Trim Display Simulated by Servo 0 */
  float servoval = 0.0;
  if (*rudder_trim != FLT_MISS) {
    servoval = *rudder_trim * 0.82 - 0.28;
  }
  ret = servo_outputf(card,0,&servoval,trim_min,trim_max);

  
  /*** CARGO FIRE PANEL ***/
  i0 = 0;
  o0 = 0;

  /* TEST button */
  ret = digital_input(card,i0+0,&cargofire_test,0);
  if (ret == 1) {
    printf("CARGO FIRE TEST BUTTON: %i \n",cargofire_test);
  }  
  /* SELECT FWD rotary */
  ret = digital_input(card,i0+1,&cargofire_select_fwd_a,0);
  if (ret == 1) {
    printf("CARGO FIRE SELECT FWD A: %i \n",cargofire_select_fwd_a);
  }  
  ret = digital_input(card,i0+2,&cargofire_select_fwd_b,0);
  if (ret == 1) {
    printf("CARGO FIRE SELECT FWD B: %i \n",cargofire_select_fwd_b);
  }  
  /* SELECT AFT rotary */
  ret = digital_input(card,i0+3,&cargofire_select_aft_a,0);
  if (ret == 1) {
    printf("CARGO FIRE SELECT AFT A: %i \n",cargofire_select_aft_a);
  }  
  ret = digital_input(card,i0+4,&cargofire_select_aft_b,0);
  if (ret == 1) {
    printf("CARGO FIRE SELECT AFT B: %i \n",cargofire_select_aft_b);
  }
  /* ARM FWD button */
  ret = digital_input(card,i0+5,&cargofire_arm_fwd,1);
  if (ret == 1) {
    printf("CARGO FIRE ARM FWD: %i \n",cargofire_arm_fwd);
  }
  /* ARM AFT button */
  ret = digital_input(card,i0+6,&cargofire_arm_aft,1);
  if (ret == 1) {
    printf("CARGO FIRE ARM AFT: %i \n",cargofire_arm_aft);
  }
  /* DISCH button */
  ret = digital_input(card,i0+7,&cargofire_disch,0);
  if (ret == 1) {
    printf("CARGO FIRE DISCH: %i \n",cargofire_disch);
  }

  /* Annunciators */
  cargofire_fault = cargofire_test;
  cargofire_test_fwd = cargofire_test;
  cargofire_test_aft = cargofire_test;
  ret = digital_output(card,o0+0,&cargofire_fault);
  ret = digital_output(card,o0+1,&cargofire_test_fwd);
  ret = digital_output(card,o0+2,&cargofire_test_aft);
  ret = digital_output(card,o0+3,&cargofire_arm_fwd);
  ret = digital_output(card,o0+4,&cargofire_arm_fwd);
  ret = digital_output(card,o0+5,&cargofire_arm_aft);
  ret = digital_output(card,o0+6,&cargofire_arm_aft);
  ret = digital_output(card,o0+7,&cargofire_disch);


  /*** SELCAL PANEL ***/
  /* Part of WXR PANEL */
  i0 = 0;
  o0 = 0;

  ret = digital_input(card,i0+0,vhf1,0);
  if (ret == 1) {
    printf("SELCAL VHF1 BUTTON: %i \n",*vhf1);
  }    
  ret = digital_output(card,o0+0,vhf1);

  ret = digital_input(card,i0+1,vhf2,0);
  if (ret == 1) {
    printf("SELCAL VHF2 BUTTON: %i \n",*vhf2);
  }    
  ret = digital_output(card,o0+1,vhf2);
  
  ret = digital_input(card,i0+2,vhf3,0);
  if (ret == 1) {
    printf("SELCAL VHF3 BUTTON: %i \n",*vhf3);
  }    
  ret = digital_output(card,o0+2,vhf3);

  ret = digital_input(card,i0+3,hf1,0);
  if (ret == 1) {
    printf("SELCAL HF1 BUTTON: %i \n",*hf1);
  }    
  ret = digital_output(card,o0+3,hf1);
  
  ret = digital_input(card,i0+4,hf2,0);
  if (ret == 1) {
    printf("SELCAL HF2 BUTTON: %i \n",*hf2);
  }    
  ret = digital_output(card,o0+4,hf2);


  /*** WXR PANEL ***/
  /* Part of SELCAL PANEL */
  i0 = 0;
  a0 = 0;

  /* WXR Mode Selector */
  ret = digital_input(card,i0+5,&wxr_mode_test,0);
  if (ret == 1) {
    printf("WXR MODE TEST: %i \n",wxr_mode_test);
  }    
  ret = digital_input(card,i0+6,&wxr_mode_wx,0);
  if (ret == 1) {
    printf("WXR MODE WX: %i \n",wxr_mode_wx);
  }    
  ret = digital_input(card,i0+7,&wxr_mode_turb,0);
  if (ret == 1) {
    printf("WXR MODE WX/TURB: %i \n",wxr_mode_turb);
  }

  /* WXR Gain */
  ret = analog_input(card,a0+0,wxr_gain,0.0,1.0);
  if (ret == 1) {
    printf("WXR Gain: %f \n",*wxr_gain);
  }
  
  /* WXR Tilt */
  ret = analog_input(card,a0+1,wxr_tilt,0.0,1.0);
  if (ret == 1) {
    printf("WXR Tilt: %f \n",*wxr_tilt);
  }

  }

  /*** DOOR PANEL ***/
  i0 = 0;
  o0 = 0;

  ret = digital_input(card,i0+0,&stab_trim_ovrd,0);
  if (ret == 1) {
    printf("STAB TRIM OVRD SWITCH: %i \n",stab_trim_ovrd);
  }    

  ret = digital_input(card,i0+1,&flt_deck_door_unlock,0);
  if (ret == 1) {
    printf("FLT DECK DOOR UNLOCKED: %i \n",flt_deck_door_unlock);
  }    
  ret = digital_input(card,i0+2,&flt_deck_door_auto,0);
  if (ret == 1) {
    printf("FLT DECK DOOR AUTO: %i \n",flt_deck_door_auto);
  }    
  ret = digital_input(card,i0+3,&flt_deck_door_deny,0);
  if (ret == 1) {
    printf("FLT DECK DOOR DENY: %i \n",flt_deck_door_deny);
  }

  flt_deck_door_pos = -flt_deck_door_unlock + flt_deck_door_deny; 
  float flt_deck_door_pos_f = (float) flt_deck_door_pos;
  ret = set_state_updnf(&flt_deck_door_pos_f,flt_deck_door,flt_deck_door_right,flt_deck_door_left);
  if (ret != 0) {
    printf("FLT DECK DOOR: %f %f %i %i \n",
	   flt_deck_door_pos_f,*flt_deck_door,*flt_deck_door_right,*flt_deck_door_left);
  }
 
  ret = digital_outputf(card,o0+0,lock_fail);
  ret = digital_outputf(card,o0+1,auto_unlk);
  
  
}
