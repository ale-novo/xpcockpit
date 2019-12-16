/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737MachNumber.cpp,v $

  Copyright (C) 2001-2015 by:
  Original author:
  Damion Shelton
  Contributors (in alphabetical order):
  Reto Stockli

  Last modification:
  Date:      $Date: 2015/11/24 $
  Version:   $Revision: $
  Author:    $Author: stockli $
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

=========================================================================*/

#include <stdio.h>
#include <math.h>
#include "ogcGaugeComponent.h"
#include "ogcB737MachNumber.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC
{

B737MachNumber::B737MachNumber()
{
  printf("B737MachNumber constructed\n");

  m_Font = m_pFontManager->LoadDefaultFont();

  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;

  m_PhysicalSize.x = 200; // make the clip region larger to handle speed bug
  m_PhysicalSize.y = 220;

  m_Scale.x = 1.0;
  m_Scale.y = 1.0;
}

B737MachNumber::~B737MachNumber()
{

}

void B737MachNumber::Render()
{
  char buffer[10]; // temporary buffer for text
  float fontHeight = 5;
  float fontWidth = 5;
  float rawGlideslope;
  float rawLocalizer;

  // Call base class to setup viewport and projection
  GaugeComponent::Render();

  // Save matrix
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  // retrieve data
  bool is_captain = (this->GetArg() == 1);
  bool is_copilot = (this->GetArg() == 2);

  int acf_type = m_pDataSource->GetAcfType();

  float *ias = link_dataref_flt("sim/flightmodel/position/indicated_airspeed",-1);
  float *tat = link_dataref_flt("sim/weather/temperature_ambient_c",-1);
  float *ap_speed;
  int *ap_speed_is_mach;
  float *ap_altitude;

  if ((acf_type == 2) || (acf_type == 3)) {
    ap_speed = link_dataref_flt("sim/cockpit/autopilot/airspeed",0);
    ap_speed_is_mach = link_dataref_int("sim/cockpit/autopilot/airspeed_is_mach");  
    ap_altitude = link_dataref_flt("sim/cockpit/autopilot/altitude",0); 
  } else if (acf_type == 1) {
    ap_speed = link_dataref_flt("x737/systems/athr/MCPSPD_spd",0);
    ap_speed_is_mach = link_dataref_int("x737/systems/athr/MCPSPD_ismach");  
    ap_altitude = link_dataref_flt("x737/systems/afds/ALTHLD_baroalt",0); 
  } else {
    ap_speed = link_dataref_flt("sim/cockpit/autopilot/airspeed",0);
    ap_speed_is_mach = link_dataref_int("sim/cockpit/autopilot/airspeed_is_mach");  
    ap_altitude = link_dataref_flt("sim/cockpit/autopilot/altitude",0); 
  }
    
  float *altimeter_pressure = link_dataref_flt("sim/cockpit/misc/barometer_setting",-2);

  //  int *altimeter_pressure_unit = link_dataref_int("x737/systems/units/baroPressUnit");   
  int *altimeter_pressure_unit;
  if ((acf_type == 2) || (acf_type == 3)) {
    altimeter_pressure_unit = link_dataref_int("laminar/B738/EFIS_control/capt/baro_in_hpa");
  } else {
    altimeter_pressure_unit = link_dataref_int("xpserver/barometer_unit");
  }
					       
					      

  int *ap_athr_armed_rec;   
  int *ap_athr_armed;   
  int *ap_n1_mode;   
  int *ap_mcpspd_mode;   
  int *ap_fmcspd_mode;   
  int *ap_retard_mode;   
  int *ap_thrhld_mode;   
  int *ap_lnav_armed;   
  int *ap_vorloc_armed;   
  int *ap_pitchspd_mode;   
  int *ap_althld_mode;   
  int *ap_althld_mode_rec;   
  int *ap_vvi_mode;
  int *ap_vs_mode;   
  int *ap_vnavalt_mode;   
  int *ap_vnavpath_mode;   
  int *ap_vnavspd_mode;   
  int *ap_gs_armed;   
  int *ap_gs_mode;   
  int *ap_flare_armed;   
  int *ap_flare_mode;   
  int *ap_toga_mode;   
  int *ap_lnav_mode;   
  int *ap_hdg_mode;   
  int *ap_hdg_mode_rec;   
  int *ap_vorloc_mode;   
  int *app_single_ch;
  if ((acf_type == 2) || (acf_type == 3)) {
    ap_athr_armed = link_dataref_int("laminar/B738/autopilot/autothrottle_status");   
    ap_athr_armed_rec = link_dataref_int("laminar/B738/autopilot/rec_thr2_modes");   
    ap_n1_mode = link_dataref_int("laminar/B738/autopilot/n1_status");   
    ap_mcpspd_mode = link_dataref_int("laminar/B738/autopilot/speed_status1");   
    ap_fmcspd_mode = link_dataref_int("laminar/B738/autopilot/fmc_spd_pfd");   
    ap_retard_mode = link_dataref_int("laminar/B738/autopilot/retard_mode");   
    ap_thrhld_mode = link_dataref_int("laminar/B738/autopilot/thr_hld_pfd");   
    ap_lnav_armed = link_dataref_int("laminar/B738/autopilot/lnav_status");   
    ap_vorloc_armed = link_dataref_int("laminar/B738/autopilot/vorloc_status");   
    ap_pitchspd_mode = link_dataref_int("xpserver/PFD_PITCHSPD_mode_on");   
    ap_althld_mode = link_dataref_int("laminar/B738/autopilot/pfd_alt_mode");   
    ap_althld_mode_rec = link_dataref_int("laminar/B738/autopilot/rec_alt_modes");   
    ap_vvi_mode = link_dataref_int("laminar/B738/autopilot/vvi_status_pfd");
    ap_vs_mode = link_dataref_int("laminar/B738/autopilot/vs_status");   
    ap_vnavalt_mode = link_dataref_int("laminar/B738/autopilot/vnav_alt_pfd");   
    ap_vnavpath_mode = link_dataref_int("laminar/B738/autopilot/vnav_pth_pfd");   
    ap_vnavspd_mode = link_dataref_int("laminar/B738/autopilot/vnav_spd_pfd");   
    ap_gs_armed = link_dataref_int("laminar/B738/autopilot/gs_armed_pfd");   
    ap_gs_mode = link_dataref_int("xpserver/PFD_GS_mode_on");   
    ap_flare_armed = link_dataref_int("xpserver/PFD_FLAREARMED_mode_on");   
    ap_flare_mode = link_dataref_int("laminar/B738/autopilot/flare_status");   
    ap_toga_mode = link_dataref_int("xpserver/PFD_TOGA_mode_on");   
    ap_lnav_mode = link_dataref_int("laminar/B738/autopilot/pfd_vorloc_lnav");   
    ap_hdg_mode = link_dataref_int("laminar/B738/autopilot/pfd_hdg_mode");   
    ap_hdg_mode_rec = link_dataref_int("laminar/B738/autopilot/rec_hdg_modes");   
    ap_vorloc_mode = link_dataref_int("laminar/B738/autopilot/pfd_vorloc_lnav");   
    app_single_ch = link_dataref_int("laminar/B738/autopilot/single_ch_status");
  } else if (acf_type == 1) {
    ap_athr_armed = link_dataref_int("x737/systems/PFD/PFD_ATHR_ARMED_on");   
    ap_athr_armed_rec = link_dataref_int("xpserver/rec_thr2_modes");   
    ap_n1_mode = link_dataref_int("x737/systems/PFD/PFD_N1_mode_on");   
    ap_mcpspd_mode = link_dataref_int("x737/systems/PFD/PFD_MCPSPD_mode_on");   
    ap_fmcspd_mode = link_dataref_int("x737/systems/PFD/PFD_FMCSPD_mode_on");   
    ap_retard_mode = link_dataref_int("x737/systems/PFD/PFD_RETARD_mode_on");   
    ap_thrhld_mode = link_dataref_int("x737/systems/PFD/PFD_THRHLD_mode_on");   
    ap_lnav_armed = link_dataref_int("x737/systems/PFD/PFD_LNAVARMED_mode_on");   
    ap_vorloc_armed = link_dataref_int("x737/systems/PFD/PFD_VORLOCARMED_mode_on");   
    ap_pitchspd_mode = link_dataref_int("x737/systems/PFD/PFD_PITCHSPD_mode_on");   
    ap_althld_mode = link_dataref_int("x737/systems/PFD/PFD_ALTHLD_mode_on");   
    ap_althld_mode_rec = link_dataref_int("xpserver/rec_alt_modes");   
    ap_vvi_mode = link_dataref_int("x737/systems/PFD/PFD_VSARMED_mode_on");
    ap_vs_mode = link_dataref_int("x737/systems/PFD/PFD_VS_mode_on");   
    ap_vnavalt_mode = link_dataref_int("x737/systems/PFD/PFD_VNAVALT_mode_on");   
    ap_vnavpath_mode = link_dataref_int("x737/systems/PFD/PFD_VNAVPATH_mode_on");   
    ap_vnavspd_mode = link_dataref_int("x737/systems/PFD/PFD_VNAVSPD_mode_on");   
    ap_gs_armed = link_dataref_int("x737/systems/PFD/PFD_GSARMED_mode_on");   
    ap_gs_mode = link_dataref_int("x737/systems/PFD/PFD_GS_mode_on");   
    ap_flare_armed = link_dataref_int("x737/systems/PFD/PFD_FLAREARMED_mode_on");   
    ap_flare_mode = link_dataref_int("x737/systems/PFD/PFD_FLARE_mode_on");   
    ap_toga_mode = link_dataref_int("x737/systems/PFD/PFD_TOGA_mode_on");   
    ap_lnav_mode = link_dataref_int("x737/systems/PFD/PFD_LNAV_mode_on");   
    ap_hdg_mode = link_dataref_int("x737/systems/PFD/PFD_HDG_mode_on");   
    ap_hdg_mode_rec = link_dataref_int("xpserver/PFD_HDG_mode_on");   
    ap_vorloc_mode = link_dataref_int("x737/systems/PFD/PFD_VORLOC_mode_on");   
    app_single_ch = link_dataref_int("x737/systems/afds/SINGLE_CH_warning");
  } else {
    ap_athr_armed = link_dataref_int("xpserver/PFD_ATHR_ARMED_on");   
    ap_athr_armed_rec = link_dataref_int("xpserver/rec_thr2_modes");   
    ap_n1_mode = link_dataref_int("xpserver/PFD_N1_mode_on");   
    ap_mcpspd_mode = link_dataref_int("xpserver/PFD_MCPSPD_mode_on");   
    ap_fmcspd_mode = link_dataref_int("xpserver/PFD_FMCSPD_mode_on");   
    ap_retard_mode = link_dataref_int("xpserver/PFD_RETARD_mode_on");   
    ap_thrhld_mode = link_dataref_int("xpserver/PFD_THRHLD_mode_on");   
    ap_lnav_armed = link_dataref_int("xpserver/PFD_LNAVARMED_mode_on");   
    ap_vorloc_armed = link_dataref_int("xpserver/PFD_VORLOCARMED_mode_on");   
    ap_pitchspd_mode = link_dataref_int("xpserver/PFD_PITCHSPD_mode_on");   
    ap_althld_mode = link_dataref_int("xpserver/PFD_ALTHLD_mode_on");   
    ap_althld_mode_rec = link_dataref_int("xpserver/rec_alt_modes");   
    ap_vvi_mode = link_dataref_int("xpserver/PFD_VSARMED_mode_on");
    ap_vs_mode = link_dataref_int("xpserver/PFD_VS_mode_on");   
    ap_vnavalt_mode = link_dataref_int("xpserver/PFD_VNAVALT_mode_on");   
    ap_vnavpath_mode = link_dataref_int("xpserver/PFD_VNAVPATH_mode_on");   
    ap_vnavspd_mode = link_dataref_int("xpserver/PFD_VNAVSPD_mode_on");   
    ap_gs_armed = link_dataref_int("xpserver/PFD_GSARMED_mode_on");   
    ap_gs_mode = link_dataref_int("xpserver/PFD_GS_mode_on");   
    ap_flare_armed = link_dataref_int("xpserver/PFD_FLAREARMED_mode_on");   
    ap_flare_mode = link_dataref_int("xpserver/PFD_FLARE_mode_on");   
    ap_toga_mode = link_dataref_int("xpserver/PFD_TOGA_mode_on");   
    ap_lnav_mode = link_dataref_int("xpserver/PFD_LNAV_mode_on");   
    ap_hdg_mode = link_dataref_int("xpserver/PFD_HDG_mode_on");   
    ap_hdg_mode_rec = link_dataref_int("xpserver/PFD_HDG_mode_on");   
    ap_vorloc_mode = link_dataref_int("xpserver/PFD_VORLOC_mode_on");   
    app_single_ch = link_dataref_int("xpserver/SINGLE_CH_warning");
  }
  
  //  int *ap_vnav_armed = link_dataref_int("x737/systems/afds/VNAV");   
  //  int *ap_lvlchg_armed = link_dataref_int("x737/systems/afds/LVLCHG");   

  int *fd_a_status;
  int *fd_b_status;
  int *cmd_a_status;
  int *cmd_b_status;
  if ((acf_type == 2) || (acf_type == 3)) {
    fd_a_status = link_dataref_int("laminar/B738/autopilot/master_capt_status");   
    fd_b_status = link_dataref_int("laminar/B738/autopilot/master_fo_status");
    cmd_a_status = link_dataref_int("laminar/B738/autopilot/cmd_a_status");
    cmd_b_status = link_dataref_int("laminar/B738/autopilot/cmd_b_status");
  } else if (acf_type == 1) {
    fd_a_status = link_dataref_int("x737/systems/afds/fdA_status");   
    fd_b_status = link_dataref_int("x737/systems/afds/fdB_status");   
    cmd_a_status = link_dataref_int("x737/systems/afds/CMD_A");
    cmd_b_status = link_dataref_int("x737/systems/afds/CMD_B");
  } else {
    fd_a_status = link_dataref_int("sim/cockpit/autopilot/autopilot_mode");   
    fd_b_status = link_dataref_int("sim/cockpit/autopilot/autopilot_mode");
    cmd_a_status = link_dataref_int("xpserver/CMD_A");
    cmd_b_status = link_dataref_int("xpserver/CMD_B");
  }
    
  int *nav1_CDI = link_dataref_int("sim/cockpit/radios/nav1_CDI");   
  int *nav2_CDI = link_dataref_int("sim/cockpit/radios/nav2_CDI"); 
  int *nav1_horizontal = link_dataref_int("sim/cockpit2/radios/indicators/nav1_display_horizontal");
  int *nav2_horizontal = link_dataref_int("sim/cockpit2/radios/indicators/nav2_display_horizontal");
  int *nav1_vertical = link_dataref_int("sim/cockpit2/radios/indicators/nav1_display_vertical");
  int *nav2_vertical = link_dataref_int("sim/cockpit2/radios/indicators/nav2_display_vertical");
  float *nav1_hdef = link_dataref_flt("sim/cockpit/radios/nav1_hdef_dot",-2);   
  float *nav2_hdef = link_dataref_flt("sim/cockpit/radios/nav2_hdef_dot",-2);   
  float *nav1_vdef = link_dataref_flt("sim/cockpit/radios/nav1_vdef_dot",-2);   
  float *nav2_vdef = link_dataref_flt("sim/cockpit/radios/nav2_vdef_dot",-2);   

  int *gpws = link_dataref_int("sim/cockpit2/annunciators/GPWS");
  int *windshear = link_dataref_int("sim/operation/failures/rel_wind_shear");

  float *altimeter_minimum = link_dataref_flt("sim/cockpit/misc/radio_altimeter_minimum",0);
  
  if (*ias != FLT_MISS) {
    // 1. Plot Mach number below the speed tape
    // transform air speed (knots) to Mach number
    float mach = fabs( *ias / (38.967854 * sqrt(*tat+273.15)));
    
    glColor3ub(255,255,255);
    glLineWidth(2.0);
    
    m_pFontManager->SetSize(m_Font, fontHeight, fontWidth);
    
    sprintf(buffer, "%5.3f", mach);
    m_pFontManager->Print(10,25+0, &buffer[0], m_Font);
  }

  // 2. Plot Autopilot dialed speed above the speed tape

  glColor3ub( 210, 5,  210 );
  glLineWidth(2.0);

  // m_pFontManager->SetSize(m_Font, fontHeight, fontWidth);

  if (*ap_speed != FLT_MISS) {
    if (*ap_speed_is_mach == 1) {
      sprintf(buffer, "%3.2f", *ap_speed);
      m_pFontManager->Print(12,170+0, &buffer[0], m_Font);
    } else {
      sprintf(buffer, "%4.0f", *ap_speed);
      m_pFontManager->Print(10,170+0, &buffer[0], m_Font);
    }
  }

  // 3. Plot dialed barometric pressure in In Hg below the altitude tape

  glColor3ub(50,255,50);
  glLineWidth(2.0);

  if (*altimeter_pressure != FLT_MISS) {
    if (roundf(*altimeter_pressure*100) == 2992) {
      strcpy(buffer, "STD");
      m_pFontManager->Print(153,25+0, &buffer[0], m_Font);
    } else {
      if (*altimeter_pressure_unit == 1) {
	sprintf(buffer, "%5.1f hPa", *altimeter_pressure/0.029530);
	m_pFontManager->Print(150,25+0, &buffer[0], m_Font);
      } else {
	sprintf(buffer, "%4.2f In", *altimeter_pressure);
	m_pFontManager->Print(150,25+0, &buffer[0], m_Font);
      }
    }
  }

  // 4. Plot dialed Autopilot altitude in feet above the altitude tape

  glColor3ub( 210, 5,  210 );
  glLineWidth(2.0);

  if (*ap_altitude != FLT_MISS) {
    sprintf(buffer, "%5.0f", *ap_altitude);
    m_pFontManager->Print(150,170+0, &buffer[0], m_Font);
  }

  // 5. Plot MCP states above PFD 

  // Draw in gray
  glColor3ub(50,50,50);

  // Draw the background rectangle
  glBegin(GL_POLYGON);
  glVertex2f(40, 190+0);
  glVertex2f(40, 170+0);
  glVertex2f(140, 170+0);
  glVertex2f(140, 190+0);
  glEnd();  

  // Draw in black
  glColor3ub(0,0,0);
  glLineWidth(3.0);

  glBegin(GL_LINES);
  glVertex2f(40,180+0);
  glVertex2f(140,180+0);
  glEnd();
  glBegin(GL_LINES);
  glVertex2f(73,170+0);
  glVertex2f(73,190+0);
  glEnd();
  glBegin(GL_LINES);
  glVertex2f(106,170+0);
  glVertex2f(106,190+0);
  glEnd();

  // Draw Autopilot states 

  m_pFontManager->SetSize(m_Font, 4, 4);

  // top left
  if ((*ap_athr_armed >= 1) && (*ap_n1_mode == 0) && (*ap_mcpspd_mode == 0)) {
    
    // Green border around background
    if ((*ap_athr_armed == 2) || (*ap_athr_armed_rec == 1)) {
      glColor3ub(0,179,0);
      glLineWidth(2.0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(40,181+0);
      glVertex2f(72,181+0);
      glVertex2f(72,190+0);
      glVertex2f(40,190+0);
      glEnd();
    }
    
    // White Text on Gray
    glColor3ub(255,255,255);
    strcpy(buffer, "ARM");
    m_pFontManager->Print(49,183+0, &buffer[0], m_Font);
  }

  if (*ap_n1_mode >= 1) {
    glColor3ub(0,179,0);
    
    // Green border around background
    if (*ap_n1_mode == 2) {
      glLineWidth(2.0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(40,181+0);
      glVertex2f(72,181+0);
      glVertex2f(72,190+0);
      glVertex2f(40,190+0);
      glEnd();
    }
      
    strcpy(buffer, "N1");
    m_pFontManager->Print(51,183+0, &buffer[0], m_Font);
  }

  if ((*ap_mcpspd_mode >= 1) && (*ap_n1_mode == 0)) {
    glColor3ub(0,179,0);
    
    // Green border around background
    if (*ap_mcpspd_mode == 2) {
      glLineWidth(2.0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(40,181+0);
      glVertex2f(72,181+0);
      glVertex2f(72,190+0);
      glVertex2f(40,190+0);
      glEnd();
    }
      
    strcpy(buffer, "MCP SPD");
    m_pFontManager->Print(44,183+0, &buffer[0], m_Font);
  }

  // top center
  if (*ap_hdg_mode >= 1) {

    glColor3ub(0,179,0);

    if (((*ap_hdg_mode == 2) && (acf_type == 1)) || (*ap_hdg_mode_rec == 1)) {
      // Green border around background
      glLineWidth(2.0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(74,181+0);
      glVertex2f(105,181+0);
      glVertex2f(105,190+0);
      glVertex2f(74,190+0);
      glEnd();
    }

    // Green Text on Gray
    if ((acf_type == 2) || (acf_type == 3)) {
      if (*ap_hdg_mode == 2) {
	strcpy(buffer, "VOR LOC");
      } else {
	strcpy(buffer, "HDG SEL");
      }
    } else {
      strcpy(buffer, "HDG SEL");
    }
    m_pFontManager->Print(77,183+0, &buffer[0], m_Font);
  }
 
  //  printf("%i %i \n",*ap_lnav_armed,*ap_lnav_mode);
 
  if (*ap_lnav_mode >= 1) {

    glColor3ub(0,179,0);
 
    if (*ap_lnav_mode == 2) {
      // Green border around background
      glLineWidth(2.0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(74,181+0);
      glVertex2f(105,181+0);
      glVertex2f(105,190+0);
      glVertex2f(74,190+0);
      glEnd();
    }   
    // Green Text on Gray
    strcpy(buffer, "LNAV");
    m_pFontManager->Print(82,183+0, &buffer[0], m_Font);
  }

  // top right
  if ((*ap_gs_mode >= 1) || (*ap_althld_mode == 5)) {

    glColor3ub(0,179,0);

    // Green border around background
    if ((*ap_gs_mode == 2) || (*ap_althld_mode_rec == 1)) {
      glLineWidth(2.0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(107,181+0);
      glVertex2f(140,181+0);
      glVertex2f(140,190+0);
      glVertex2f(107,190+0);
      glEnd();
    }
    
    strcpy(buffer, "G/S");
    m_pFontManager->Print(119,183+0, &buffer[0], m_Font);
  }

  if (*ap_vs_mode >= 1) {

    glColor3ub(0,179,0);

    // Green border around background
    if ((*ap_vs_mode == 2) || (*ap_althld_mode_rec == 1)) {
      glLineWidth(2.0);      glBegin(GL_LINE_LOOP);
      glVertex2f(107,181+0);
      glVertex2f(140,181+0);
      glVertex2f(140,190+0);
      glVertex2f(107,190+0);
      glEnd();
    }

    // Green Text on Gray
    strcpy(buffer, "V/S");
    m_pFontManager->Print(119,183+0, &buffer[0], m_Font);
  }

  // printf("%i %i\n",*ap_althld_mode,*ap_vs_mode);

  if (((*ap_althld_mode == 1) || (*ap_althld_mode == 2) || (*ap_althld_mode == 4)) && (*ap_vs_mode == 0)) {

    glColor3ub(0,179,0);
    
    // Green border around background
    if ((*ap_althld_mode == 2) || (*ap_althld_mode == 4) || (*ap_althld_mode_rec == 1)) {
      glLineWidth(2.0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(107,181+0);
      glVertex2f(140,181+0);
      glVertex2f(140,190+0);
      glVertex2f(107,190+0);
      glEnd();
    }

    // Green Text on Gray
    strcpy(buffer, "ALT HLD");
    m_pFontManager->Print(111,183+0, &buffer[0], m_Font);
  }

  // bottom left
  
  // bottom center
  if (*ap_lnav_armed >= 1) {

    glColor3ub(255,255,255);

    // White border around background
    if (*ap_lnav_armed == 2) {
      glLineWidth(2.0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(74,170+0);
      glVertex2f(105,170+0);
      glVertex2f(105,179+0);
      glVertex2f(74,179+0);
      glEnd();
    }
    
    // White Text on Gray
    strcpy(buffer, "LNAV ARM");
    m_pFontManager->Print(76,172+0, &buffer[0], m_Font);
  }

  
  if (((*ap_vorloc_armed >= 1) || (*ap_vorloc_mode >= 1)) && (*ap_lnav_armed == 0)) {

    if (*ap_vorloc_armed >= 1) {
      glColor3ub(255,255,255);
    } else {
      glColor3ub(0,179,0);
    }
      
    // White border around background
    if ((*ap_vorloc_armed == 2) || (*ap_vorloc_mode == 2)) {
      glLineWidth(2.0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(74,170+0);
      glVertex2f(105,170+0);
      glVertex2f(105,179+0);
      glVertex2f(74,179+0);
      glEnd();
    }
    
    // White Text on Gray
    strcpy(buffer, "VOR LOC");
    m_pFontManager->Print(77,172+0, &buffer[0], m_Font);
  }

  // bottom right
  if (*ap_vvi_mode >= 1) {

    glColor3ub(255,255,255);

    // White border around background
    if (*ap_vvi_mode == 2)  {
      glLineWidth(2.0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(107,170+0);
      glVertex2f(140,170+0);
      glVertex2f(140,179+0);
      glVertex2f(107,179+0);
      glEnd();
    }

    // White Text on Gray
    strcpy(buffer, "VERT SPD");
    m_pFontManager->Print(110,172+0, &buffer[0], m_Font);
  }

  if ((*ap_flare_armed >= 1) || (*ap_flare_mode >= 1)) {
    
    if (*ap_flare_armed >= 1) {
      glColor3ub(255,255,255);
    } else {
      glColor3ub(0,179,0);
    }
 
    // White border around background
    if ((*ap_flare_armed == 2) || (*ap_flare_mode == 2)) {
      glLineWidth(2.0);
      glBegin(GL_LINE_LOOP);
      glVertex2f(107,170+0);
      glVertex2f(140,170+0);
      glVertex2f(140,179+0);
      glVertex2f(107,179+0);
      glEnd();
    }

    // White Text on Gray
    strcpy(buffer, "FLARE");
    m_pFontManager->Print(114,172+0, &buffer[0], m_Font);
  }
  
  // Print FD status
  if (((*fd_a_status >= 1) && is_captain) || ((*fd_b_status >= 1) && is_copilot)) {
    m_pFontManager->SetSize(m_Font, 5, 5);
    glColor3ub(0,179,0);

    
    if (*app_single_ch == 1) {
      glColor3ub(255,255,0);
      strcpy(buffer, "SINGLE CH");
      m_pFontManager->Print(70,155+0, &buffer[0], m_Font);
    } else {
      if ((((*cmd_a_status == 1) || (*cmd_b_status == 1)) && (acf_type >= 1)) ||
	  (((*fd_a_status == 2) || (*fd_b_status == 2)) && (acf_type == 0))) {
	strcpy(buffer, "CMD");
	m_pFontManager->Print(83,155+0, &buffer[0], m_Font);
      } else {
	strcpy(buffer, "FD");
	m_pFontManager->Print(85,155+0, &buffer[0], m_Font);
	
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(83,153+0);
	glVertex2f(97,153+0);
	glVertex2f(97,162+0);
	glVertex2f(83,162+0);
	glEnd();
      }
    }
  }

  // Draw the glideslope and localizer displays to the right and bottom of the ADI
  if ( ((*nav1_vertical == 1) && (*nav1_CDI == 1)) || 
       ((*nav2_vertical == 1) && (*nav2_CDI == 1)) ) 
  {
    if (*nav1_vertical == 1) {
      rawGlideslope = *nav1_vdef;
    } else {
      rawGlideslope = *nav2_vdef;
    }

    if (rawGlideslope < - 2.5) rawGlideslope = -2.5;
    if (rawGlideslope >   2.5) rawGlideslope = 2.5;

    // Where is the center of the ADI?
    float ADICenterX = (94/2) + 42;
    float ADICenterY = (98/2) + 52;
    
    // The height of the glideslope markers above and below center (+/- 1 and 2 degrees deflection)
    float glideslopeHeight = 20;
    
    // The vertical offset of the glideslope bug
    float glideslopePosition = ADICenterY - rawGlideslope * glideslopeHeight;
    
    // Horizontal position of the dots relative to the ADI center
    float dotsHoriz = ADICenterX + 55;
    
    // Horizontal center line
    glColor3ub( 255, 255,  255 );
    glBegin(GL_LINES);
    glVertex2f( dotsHoriz - 4, ADICenterY );
    glVertex2f( dotsHoriz + 4, ADICenterY );
    glEnd();
    
    // Draw the glideslope dots
    
    // Set up the circle
    CircleEvaluator aCircle;
    aCircle.SetRadius(2.0);
    aCircle.SetArcStartEnd(0,360);
    aCircle.SetDegreesPerPoint(10);
    
    glLineWidth(2.0);
    
    aCircle.SetOrigin(dotsHoriz, ADICenterY + glideslopeHeight * 2);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
    
    aCircle.SetOrigin(dotsHoriz, ADICenterY + glideslopeHeight);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
    
    aCircle.SetOrigin(dotsHoriz, ADICenterY + -1 * glideslopeHeight);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
    
    aCircle.SetOrigin(dotsHoriz, ADICenterY + -1 * glideslopeHeight * 2);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
		
    // This is the glideslope bug
    // Todo: fill the glideslope bug when we are in glide slope
    // color is magenta
    glColor3ub( 210, 5,  210 );
    if (fabs(rawGlideslope) < 2.0) {
      glBegin(GL_POLYGON);
    } else {
      glBegin(GL_LINE_LOOP);
    }
    glVertex2f( dotsHoriz - 2, glideslopePosition ); 	
    glVertex2f( dotsHoriz, glideslopePosition + 4 );
    glVertex2f( dotsHoriz + 2, glideslopePosition );
    glVertex2f( dotsHoriz, glideslopePosition - 4 );
    glEnd();	
  }

  // Draw the localizer
  if ( (*nav1_horizontal == 1) || (*nav2_horizontal == 1) ) 
  {
    if (*nav1_horizontal == 1) {
      rawLocalizer = *nav1_hdef;
    } else {
      rawLocalizer = *nav2_hdef;
    }
    
    // Where is the center of the ADI?
    float ADICenterX = (94/2) + 42;
    
    // Height of localizer center in the PFD
    float localizerHeight = 48.0;
    
    // Overall localizer width
    float localizerWidth = 20.0;
    
    // The horizontal offset of the localizer bug
    float localizerPosition = ADICenterX + rawLocalizer * localizerWidth;
		
    // Verticalal center line
    glColor3ub( 255, 255,  255 );
    glBegin(GL_LINES);
    glVertex2f( ADICenterX, localizerHeight + 4 );
    glVertex2f( ADICenterX, localizerHeight - 4 );
    glEnd();
    
    // Draw the localizer dots
    
    // Set up the circle
    CircleEvaluator aCircle;
    aCircle.SetRadius(2.0);
    aCircle.SetArcStartEnd(0,360);
    aCircle.SetDegreesPerPoint(10);
    
    glLineWidth(2.0);
    
    aCircle.SetOrigin(ADICenterX + localizerWidth * 2, localizerHeight);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
    
    aCircle.SetOrigin(ADICenterX + localizerWidth, localizerHeight);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
    
    aCircle.SetOrigin(ADICenterX + -1 * localizerWidth, localizerHeight);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
    
    aCircle.SetOrigin(ADICenterX + -1 * localizerWidth * 2, localizerHeight);
    glBegin(GL_LINE_LOOP);
    aCircle.Evaluate();
    glEnd();
		
    // This is the localizer bug
    // color is magenta
    glColor3ub( 210, 5,  210 );
    if (fabs(rawLocalizer) < 2.0) {
      glBegin(GL_POLYGON);
    } else {
      glBegin(GL_LINE_LOOP);
    }
    glVertex2f( localizerPosition - 4, localizerHeight ); 	
    glVertex2f( localizerPosition, localizerHeight + 2 );
    glVertex2f( localizerPosition + 4, localizerHeight );
    glVertex2f( localizerPosition, localizerHeight - 2 );
    glEnd();	

  }

  // draw GPWS in RED
  if (*gpws != INT_MISS) {
    if (*gpws == 1) {
      m_pFontManager->SetSize(m_Font, fontHeight*1.5, fontWidth*1.5);
      glColor3ub( 255, 0,  0 );
      strcpy(buffer, "PULL UP");
      m_pFontManager->Print(70,42, &buffer[0], m_Font);
    }
  }
  // draw Wind Shear Warning in RED
  if (*windshear != INT_MISS) {
    if (*windshear == 1) {
      m_pFontManager->SetSize(m_Font, fontHeight*1.5, fontWidth*1.5);
      glColor3ub( 255, 0,  0 );
      strcpy(buffer, "WINDSHEAR");
      m_pFontManager->Print(58,42, &buffer[0], m_Font);
    }
  }

  // draw minimum altitude (feet)
  if (*altimeter_minimum != FLT_MISS) {
      m_pFontManager->SetSize(m_Font, fontHeight, fontWidth);
      glColor3ub( 0, 255,  0 );
      strcpy(buffer, "RADIO");
      m_pFontManager->Print(117,35, &buffer[0], m_Font);
      sprintf(buffer, "%4.0f", *altimeter_minimum);
      m_pFontManager->Print(124,28, &buffer[0], m_Font);
  }
  
  glPopMatrix();
}

} // end namespace OpenGC
