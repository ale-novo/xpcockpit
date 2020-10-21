/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737SpeedTape.cpp,v $

  Copyright (C) 2001-2015 by:
  Original author:
  Damion Shelton
  Contributors (in alphabetical order):
  Reto Stockli

  Last modification:
  Date:      $Date: 2015/11/26 $
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
#include "ogcB737PFD.h"
#include "ogcB737SpeedTape.h"

namespace OpenGC
{

  double indent_x; // defines region of speed tape

  B737SpeedTape::B737SpeedTape()
  {
    printf("B737SpeedTape constructed\n");

    m_Font = m_pFontManager->LoadDefaultFont();

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 34; // make the clip region larger to handle speed bug
    m_PhysicalSize.y = 136;
    indent_x = m_PhysicalSize.x - 10;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  B737SpeedTape::~B737SpeedTape()
  {

  }

  void B737SpeedTape::Render()
  {
    // Call base class to setup viewport and projection
    GaugeComponent::Render();

    int acf_type = m_pDataSource->GetAcfType();
    
    // indicated air speed (knots)
    float *speed_knots = link_dataref_flt("sim/flightmodel/position/indicated_airspeed",-1);

    // change of airspeed (knots / second)
    float *speed_acceleration = link_dataref_flt("sim/cockpit2/gauges/indicators/airspeed_acceleration_kts_sec_pilot",-1);
    
    // Autopilot speed (knots)
    float *ap_speed;
    int *ap_speed_is_mach;
    if (acf_type == 1) {
      ap_speed = link_dataref_flt("x737/systems/athr/MCPSPD_spd",0);
      ap_speed_is_mach = link_dataref_int("x737/systems/athr/MCPSPD_ismach");  
    } else {
      ap_speed = link_dataref_flt("sim/cockpit/autopilot/airspeed",0);
      ap_speed_is_mach = link_dataref_int("sim/cockpit/autopilot/airspeed_is_mach");
    }

    // minimum and maximum speeds
    float *vfe;
    float *vle;
    float *vne;
    float *min_speed;
    float *min_speed_show;
    float *max_speed;
    float *min_maneuver_speed;
    float *min_maneuver_speed_show;
    float *max_maneuver_speed;
    float *max_maneuver_speed_show;

    float *vflap0;
    float *vflap1;
    float *vflap2;
    float *vflap5;
    float *vflap10;
    float *vflap15;
    float *vflap25;
    int *sflap0;
    int *sflap1;
    int *sflap2;
    int *sflap5;
    int *sflap10;
    int *sflap15;
    int *sflap25;
    if ((acf_type == 2) || (acf_type ==3)) {
      min_speed = link_dataref_flt("laminar/B738/pfd/min_speed",0);
      min_speed_show = link_dataref_flt("laminar/B738/pfd/min_speed_show",0);
      max_speed = link_dataref_flt("laminar/B738/pfd/max_speed",0);
      min_maneuver_speed = link_dataref_flt("laminar/B738/pfd/min_maneuver_speed",0);
      min_maneuver_speed_show = link_dataref_flt("laminar/B738/pfd/min_maneuver_speed_show",0);
      max_maneuver_speed = link_dataref_flt("laminar/B738/pfd/max_maneuver_speed",0);
      max_maneuver_speed_show = link_dataref_flt("laminar/B738/pfd/max_maneuver_speed_show",0);

      vflap0 = link_dataref_flt("laminar/B738/pfd/flaps_up",0);
      vflap1 = link_dataref_flt("laminar/B738/pfd/flaps_1",0);
      vflap2 = link_dataref_flt("laminar/B738/pfd/flaps_2",0);
      vflap5 = link_dataref_flt("laminar/B738/pfd/flaps_5",0);
      vflap10 = link_dataref_flt("laminar/B738/pfd/flaps_10",0);
      vflap15 = link_dataref_flt("laminar/B738/pfd/flaps_15",0);
      vflap25 = link_dataref_flt("laminar/B738/pfd/flaps_25",0);
      sflap0 = link_dataref_int("laminar/B738/pfd/flaps_up_show");
      sflap1 = link_dataref_int("laminar/B738/pfd/flaps_1_show");
      sflap2 = link_dataref_int("laminar/B738/pfd/flaps_2_show");
      sflap5 = link_dataref_int("laminar/B738/pfd/flaps_5_show");
      sflap10 = link_dataref_int("laminar/B738/pfd/flaps_10_show");
      sflap15 = link_dataref_int("laminar/B738/pfd/flaps_15_show");
      sflap25 = link_dataref_int("laminar/B738/pfd/flaps_25_show");
      
    } else {
      //    float *vs = link_dataref_flt("sim/aircraft/view/acf_Vs",0);
      vfe = link_dataref_flt("sim/aircraft/view/acf_Vfe",0);
      vle = link_dataref_flt("sim/aircraft/overflow/acf_Vle",0);
      //    float *vno = link_dataref_flt("sim/aircraft/view/acf_Vno",0);
      vne = link_dataref_flt("sim/aircraft/view/acf_Vne",0);
    }
    
    // other speeds (only currently for x737)
    float *v1;
    float *v2;
    float *v2_15;
    float *vr;
    float *vref;
    if ((acf_type == 2) || (acf_type ==3)) {
      v1 = link_dataref_flt("laminar/B738/FMS/v1",0);
      v2 = link_dataref_flt("laminar/B738/FMS/v2",0);
      v2_15 = link_dataref_flt("laminar/B738/FMS/v2_15",0);
      vr = link_dataref_flt("laminar/B738/FMS/vr",0);
      vref = link_dataref_flt("laminar/B738/FMS/vref",0);
    } else if (acf_type == 1) {
      v1 = link_dataref_flt("x737/systems/FMC/SPDREF_v1",0);
      v2 = link_dataref_flt("x737/systems/FMC/SPDREF_v2",0);
      vr = link_dataref_flt("x737/systems/FMC/SPDREF_vr",0);
      vref = link_dataref_flt("x737/systems/FMC/SPDREF_vref",0);
    } else {
      v1 = link_dataref_flt("xpserver/v1",0);
      v2 = link_dataref_flt("xpserver/v2",0);
      vr = link_dataref_flt("xpserver/vr",0);
      vref = link_dataref_flt("xpserver/vref",0);
    }
      
    int *gear_handle = link_dataref_int("sim/cockpit2/controls/gear_handle_down");
    float *flap_ratio = link_dataref_flt("sim/flightmodel/controls/flaprat",-2);
    
    //printf("%f %f %f %f \n",*v1,*v2,*vr,*vref);
    
    if (*speed_knots != FLT_MISS) {

      // Speed for integer calculations
      int ias_int = (int) *speed_knots;
      float ias_flt = *speed_knots;
     
      // The speed tape doesn't display speeds < 30 or > 999
      /* if(ias_flt < 10.0)
	 {
	 ias_flt = 10.0;
	 ias_int = 10;
	 } */
  
      if(ias_flt > 999.0)
	{
	  ias_flt = 999.0;
	  ias_int = 999;
	}

      // Save matrix
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      // Draw in gray-blue
      glColor3ub(COLOR_GRAYBLUE);

      // Draw the background rectangle
      glBegin(GL_POLYGON);
      glVertex2f(0.0, 0.0);
      glVertex2f(0.0, m_PhysicalSize. y);
      glVertex2f(indent_x, m_PhysicalSize.y);
      glVertex2f(indent_x, 0.0);
      glVertex2f(0.0, 0.0);
      glEnd();
  
      // Tick marks are space every 10 kph vertically
      // The tick spacing represents how far apart they are in physical
      // units
      float tickSpacing = 11.3;
      float tickWidth = 3.7;
      float fontHeight = 5;
      float fontWidth = 5;
      float fontIndent = 6;

      m_pFontManager->SetSize(m_Font, fontHeight, fontWidth);

      char buffer[10];
      char buffer2[10];
      memset(buffer,0,sizeof(buffer));
      memset(buffer2,0,sizeof(buffer));

      int nextHighestIAS = (ias_int/10)*10;

      if (nextHighestIAS < ias_int)
	nextHighestIAS += 10;

      // The vertical offset is how high in physical units the next highest 10's
      // airspeed is above the arrow. For airspeeds divisible by 10, this is 0. I.e. 120, 130
      // etc. are all aligned with the arrow
      float vertOffset = (float) nextHighestIAS - ias_flt;
  
      // Vertical location of the tick mark
      float tickLocation = 0;

      glColor3ub(255,255,255);
      glLineWidth(2.0);

      float i = 0; // counter
      int tickSpeed; // speed represented by tick mark
      int charSpd; // speed for computing text digits

      // Draw ticks up from the center
      for (i = 0; i <= ((m_PhysicalSize.y/2) / tickSpacing) + 1; i += 1.0)
	{
	  tickSpeed = nextHighestIAS + (int)(i*10.0);
	  tickLocation = (m_PhysicalSize.y/2) + i*tickSpacing+vertOffset;
    
	  // do not draw negative tick speeds
	  if (tickSpeed >= 0) {
    
	    glLineWidth(2.0);
	    glBegin(GL_LINES);
	    glVertex2f(indent_x - tickWidth, tickLocation);
	    glVertex2f(indent_x, tickLocation);
	    glEnd();

	    if( (tickSpeed % 20)==0 )
	      {
		float texty = tickLocation - fontHeight / 2;

		charSpd = tickSpeed;

		if(charSpd >= 100)
		  {
		    // 100's
		    //_itoa( charSpd / 100, buffer, 10);
		    snprintf( buffer, sizeof(buffer), "%i", charSpd/100);
		    m_pFontManager->Print(fontIndent, texty, &buffer[0], m_Font);
		    charSpd = charSpd-100*(int)(charSpd/100);
	    
		    // draw second digit if tickSpeed is multiplier of 100
		    if (charSpd == 0) {
		      snprintf( buffer, sizeof(buffer), "%i", charSpd/10);
		      m_pFontManager->Print(fontIndent + fontWidth, texty, &buffer[0], m_Font);
		    }
		  }

		if(charSpd >= 10)
		  {
		    // 10's
		    //_itoa( charSpd / 10, buffer, 10);
		    snprintf(buffer, sizeof(buffer), "%i", charSpd/10);
		    m_pFontManager->Print(fontIndent + fontWidth, texty, &buffer[0], m_Font);
		    charSpd = charSpd-10*(int)(charSpd/10);
		  }
	
		snprintf(buffer, sizeof(buffer), "%i", 0);
		m_pFontManager->Print(fontIndent + fontWidth*2, texty, &buffer[0], m_Font);
	      }
    
	  }

	}

      // Draw ticks down from the center
      for (i = 1; i <= ((m_PhysicalSize.y/2) / tickSpacing) + 1; i += 1.0)
	{
	  tickSpeed = (int) nextHighestIAS - i*10;
	  
	  // Only draw ticks if they are positive
	  if( tickSpeed >= 0)
	    {
	      tickLocation = (m_PhysicalSize.y/2) - ( (i-1) * tickSpacing) - (10 - vertOffset);

	      glLineWidth(2.0);
	      glBegin(GL_LINES);
	      glVertex2f(indent_x - tickWidth, tickLocation);
	      glVertex2f(indent_x, tickLocation);
	      glEnd();

	      if( (tickSpeed % 20)==0 )
		{
		  float texty = tickLocation - fontHeight / 2;

		  charSpd = tickSpeed;

		  if(charSpd >= 100)
		    {
		      // 100's
		      //_itoa( charSpd / 100, buffer, 10);
		      snprintf( buffer, sizeof(buffer), "%i", charSpd/100);
		      m_pFontManager->Print(fontIndent, texty, &buffer[0], m_Font);
		      charSpd = charSpd-100*(int)(charSpd/100);

		      // draw second digit if tickSpeed is multiplier of 100
		      if (charSpd == 0) {
			snprintf( buffer, sizeof(buffer), "%i", charSpd/10);
			m_pFontManager->Print(fontIndent + fontWidth, texty, &buffer[0], m_Font);
		      }
		    }

		  if (charSpd >=10)
		    {
		      // 10's
		      //_itoa( charSpd / 10, buffer, 10);
		      snprintf( buffer, sizeof(buffer), "%i", charSpd/10);
		      m_pFontManager->Print(fontIndent + fontWidth, texty, &buffer[0], m_Font);
		      charSpd = charSpd-10*(int)(charSpd/10);
		    }

		  snprintf( buffer, sizeof(buffer), "%i", 0);
		  m_pFontManager->Print(fontIndent + fontWidth*2, texty, &buffer[0], m_Font);
		}
	    }
	}
      
 	
      // draw red minimum and maximum speed rectangles

      if ((acf_type == 2) || (acf_type == 3)) {
	if ((*min_speed != FLT_MISS) && (*min_speed_show == 1.0)) {
	  float minspdLocation = float(*min_speed - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;

	  int nbox = (int) (minspdLocation / tickSpacing*0.30 * 5.0 + 3.0);

	  if (nbox > 0) {
	    glColor3ub( 255, 0,  0 );
	    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	    for (i=0;i<nbox;i++) {
	      glBegin(GL_POLYGON);
	      glVertex2f(indent_x+tickSpacing*0.35, minspdLocation-tickSpacing*0.30*i*2);
	      glVertex2f(indent_x+tickSpacing*0.35, minspdLocation-tickSpacing*0.30*(i*2+1));
	      glVertex2f(indent_x,                  minspdLocation-tickSpacing*0.30*(i*2+1));
	      glVertex2f(indent_x,                  minspdLocation-tickSpacing*0.30*i*2);
	      glEnd();
	    }
	  }

	  if ((*min_maneuver_speed != FLT_MISS) && (*min_maneuver_speed_show == 1.0)) {
	    float minmaneuverspdLocation = float(*min_maneuver_speed - ias_flt) *
	      tickSpacing / 10.0 + m_PhysicalSize.y/2;
	    glColor3ub( 255, 140,  0 );
	    glLineWidth(2.0);
	    glBegin(GL_LINE_STRIP);
	    glVertex2f(indent_x+tickSpacing*0.175, minspdLocation);
	    glVertex2f(indent_x+tickSpacing*0.175, minmaneuverspdLocation);
	    glVertex2f(indent_x, minmaneuverspdLocation);
	    glEnd();	
	  }
	}
      }
      
      if ((acf_type == 2) || (acf_type == 3)) {
	if (*max_speed != FLT_MISS) {
	  float maxspdLocation = float(*max_speed - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;

	  int nbox = (int) ((m_PhysicalSize.y - maxspdLocation) / tickSpacing*0.30 * 5.0 + 3.0);

	  if (nbox > 0) {
	    glColor3ub( 255, 0,  0 );
	    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	    for (i=0;i<nbox;i++) {
	      glBegin(GL_POLYGON);
	      glVertex2f(indent_x+tickSpacing*0.35, maxspdLocation+tickSpacing*0.30*i*2);
	      glVertex2f(indent_x+tickSpacing*0.35, maxspdLocation+tickSpacing*0.30*(i*2+1));
	      glVertex2f(indent_x,                  maxspdLocation+tickSpacing*0.30*(i*2+1));
	      glVertex2f(indent_x,                  maxspdLocation+tickSpacing*0.30*i*2);
	      glEnd();
	    }
	  }
	  
	  if ((*max_maneuver_speed != FLT_MISS) && (*max_maneuver_speed_show == 1.0)) {
	    float maxmaneuverspdLocation = float(*max_maneuver_speed - ias_flt) *
	      tickSpacing / 10.0 + m_PhysicalSize.y/2;
	    glColor3ub( 255, 140,  0 );
	    glLineWidth(2.0);
	    glBegin(GL_LINE_STRIP);
	    glVertex2f(indent_x+tickSpacing*0.175, maxspdLocation);
	    glVertex2f(indent_x+tickSpacing*0.175, maxmaneuverspdLocation);
	    glVertex2f(indent_x, maxmaneuverspdLocation);
	    glEnd();	
	  }
	}
      }

      if (*ap_speed != FLT_MISS) {
	// draw MCP dialed speed if within the speed tape range (PLEASE ADAPT FOR AP MACH SPEEED)
	float mcpspdLocation;
	if (*ap_speed_is_mach == 1) {
	  // approximate knots with standard formula (20degC / dry air)
	  mcpspdLocation = float(*ap_speed * 661.0 - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;
	} else {
	  mcpspdLocation = float(*ap_speed - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;
	}
	  
	// keep MCP speed within bounds of tape
	mcpspdLocation = fmin(fmax(0.0,mcpspdLocation),m_PhysicalSize.y);

	// draw a magenta MCP altitude indicator
	glLineWidth(3.0);
	glColor3ub( 210, 5,  210 );
	glBegin(GL_LINE_LOOP);
	glVertex2f(indent_x-5.0, mcpspdLocation);
	glVertex2f(indent_x, mcpspdLocation+tickSpacing*0.3);
	glVertex2f(m_PhysicalSize.x-2.0, mcpspdLocation+tickSpacing*0.3);
	glVertex2f(m_PhysicalSize.x-2.0, mcpspdLocation-tickSpacing*0.3);
	glVertex2f(indent_x, mcpspdLocation-tickSpacing*0.3);
	glEnd();
    
      }

      // draw reference speeds
      if ((*vref != FLT_MISS) && (*vref != 0.0)) {
	float vLocation = float(*vref - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;
    
	glColor3ub( 0, 179, 0);
	strcpy(buffer2, "REF");
	m_pFontManager->SetSize(m_Font, 0.7*fontHeight, 0.7*fontWidth);
	m_pFontManager->Print(indent_x+1.5, vLocation-0.35*fontHeight, &buffer2[0], m_Font);

	glBegin(GL_LINES);
	glVertex2f(indent_x - 0.5*tickWidth, vLocation);
	glVertex2f(indent_x + 1.0, vLocation);
	glEnd();    
      }
      if ((*v1 != FLT_MISS) && (*v1 != 0.0)) {
	float vLocation = float(*v1 - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;
    
	glColor3ub( 0, 179, 0);
	strcpy(buffer2, "V1");
	m_pFontManager->SetSize(m_Font, 0.7*fontHeight, 0.7*fontWidth);
	m_pFontManager->Print(indent_x+1.5, vLocation-0.35*fontHeight, &buffer2[0], m_Font);

	glBegin(GL_LINES);
	glVertex2f(indent_x - 0.5*tickWidth, vLocation);
	glVertex2f(indent_x + 1.0, vLocation);
	glEnd();
    
      }
      if ((*v2 != FLT_MISS) && (*v2 != 0.0)) {
	float vLocation = float(*v2 - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;
    
	glColor3ub( 0, 179, 0);
	strcpy(buffer2, "V2");
	m_pFontManager->SetSize(m_Font, 0.7*fontHeight, 0.7*fontWidth);
	m_pFontManager->Print(indent_x+1.5, vLocation-0.35*fontHeight, &buffer2[0], m_Font);

	glBegin(GL_LINES);
	glVertex2f(indent_x - 0.5*tickWidth, vLocation);
	glVertex2f(indent_x + 1.0, vLocation);
	glEnd();
    
      }
      if ((*vr != FLT_MISS) && (*vr != 0.0)) {
	float vLocation = float(*vr - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;
    
	glColor3ub( 0, 179, 0);
	strcpy(buffer2, "VR");
	m_pFontManager->SetSize(m_Font, 0.7*fontHeight, 0.7*fontWidth);
	m_pFontManager->Print(indent_x+1.5, vLocation-0.35*fontHeight, &buffer2[0], m_Font);

	glBegin(GL_LINES);
	glVertex2f(indent_x - 0.5*tickWidth, vLocation);
	glVertex2f(indent_x + 1.0, vLocation);
	glEnd();
      }
      /* Speeds only specific to ZIBO */
      if ((acf_type == 2) || (acf_type == 3)) {
	if ((*v2_15 != FLT_MISS) && (*v2_15 != 0.0)) {
	  float vLocation = float(*v2_15 - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;
	  
	  glColor3ub( 255, 255, 255);
	  glLineWidth(1.0);

	  glBegin(GL_LINE_LOOP);
	  glVertex2f(indent_x-3.0, vLocation);
	  glVertex2f(indent_x, vLocation+tickSpacing*0.15);
	  glVertex2f(m_PhysicalSize.x-6.0, vLocation+tickSpacing*0.15);
	  glVertex2f(m_PhysicalSize.x-6.0, vLocation-tickSpacing*0.15);
	  glVertex2f(indent_x, vLocation-tickSpacing*0.15);
	  glEnd();
 	  
	}
	
	if ((*vflap0 != FLT_MISS) && (*sflap0 == 1)) {
	  float vLocation = float(*vflap0 - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;
	  
	  glColor3ub( 0, 179, 0);
	  strcpy(buffer2, "UP");
	  m_pFontManager->SetSize(m_Font, 0.7*fontHeight, 0.7*fontWidth);
	  m_pFontManager->Print(indent_x+1.5, vLocation-0.35*fontHeight, &buffer2[0], m_Font);
	  
	  glBegin(GL_LINES);
	  glVertex2f(indent_x - 0.5*tickWidth, vLocation);
	  glVertex2f(indent_x + 1.0, vLocation);
	  glEnd();
	}
	if ((*vflap1 != FLT_MISS) && (*sflap1 == 1)) {
	  float vLocation = float(*vflap1 - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;
	  
	  glColor3ub( 0, 179, 0);
	  strcpy(buffer2, "1");
	  m_pFontManager->SetSize(m_Font, 0.7*fontHeight, 0.7*fontWidth);
	  m_pFontManager->Print(indent_x+1.5, vLocation-0.35*fontHeight, &buffer2[0], m_Font);
	  
	  glBegin(GL_LINES);
	  glVertex2f(indent_x - 0.5*tickWidth, vLocation);
	  glVertex2f(indent_x + 1.0, vLocation);
	  glEnd();
	}
	if ((*vflap2 != FLT_MISS) && (*sflap2 == 1)) {
	  float vLocation = float(*vflap2 - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;
	  
	  glColor3ub( 0, 179, 0);
	  strcpy(buffer2, "2");
	  m_pFontManager->SetSize(m_Font, 0.7*fontHeight, 0.7*fontWidth);
	  m_pFontManager->Print(indent_x+1.5, vLocation-0.35*fontHeight, &buffer2[0], m_Font);
	  
	  glBegin(GL_LINES);
	  glVertex2f(indent_x - 0.5*tickWidth, vLocation);
	  glVertex2f(indent_x + 1.0, vLocation);
	  glEnd();
	}
	if ((*vflap5 != FLT_MISS) && (*sflap5 == 1)) {
	  float vLocation = float(*vflap5 - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;
	  
	  glColor3ub( 0, 179, 0);
	  strcpy(buffer2, "5");
	  m_pFontManager->SetSize(m_Font, 0.7*fontHeight, 0.7*fontWidth);
	  m_pFontManager->Print(indent_x+1.5, vLocation-0.35*fontHeight, &buffer2[0], m_Font);
	  
	  glBegin(GL_LINES);
	  glVertex2f(indent_x - 0.5*tickWidth, vLocation);
	  glVertex2f(indent_x + 1.0, vLocation);
	  glEnd();
	}
	if ((*vflap10 != FLT_MISS) && (*sflap10 == 1)) {
	  float vLocation = float(*vflap10 - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;
	  
	  glColor3ub( 0, 179, 0);
	  strcpy(buffer2, "10");
	  m_pFontManager->SetSize(m_Font, 0.7*fontHeight, 0.7*fontWidth);
	  m_pFontManager->Print(indent_x+1.5, vLocation-0.35*fontHeight, &buffer2[0], m_Font);
	  
	  glBegin(GL_LINES);
	  glVertex2f(indent_x - 0.5*tickWidth, vLocation);
	  glVertex2f(indent_x + 1.0, vLocation);
	  glEnd();
	}
	if ((*vflap15 != FLT_MISS) && (*sflap15 == 1)) {
	  float vLocation = float(*vflap15 - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;
	  
	  glColor3ub( 0, 179, 0);
	  strcpy(buffer2, "15");
	  m_pFontManager->SetSize(m_Font, 0.7*fontHeight, 0.7*fontWidth);
	  m_pFontManager->Print(indent_x+1.5, vLocation-0.35*fontHeight, &buffer2[0], m_Font);
	  
	  glBegin(GL_LINES);
	  glVertex2f(indent_x - 0.5*tickWidth, vLocation);
	  glVertex2f(indent_x + 1.0, vLocation);
	  glEnd();
	}
	if ((*vflap25 != FLT_MISS) && (*sflap25 == 1)) {
	  float vLocation = float(*vflap25 - ias_flt) * tickSpacing / 10.0 + m_PhysicalSize.y/2;
	  
	  glColor3ub( 0, 179, 0);
	  strcpy(buffer2, "25");
	  m_pFontManager->SetSize(m_Font, 0.7*fontHeight, 0.7*fontWidth);
	  m_pFontManager->Print(indent_x+1.5, vLocation-0.35*fontHeight, &buffer2[0], m_Font);
	  
	  glBegin(GL_LINES);
	  glVertex2f(indent_x - 0.5*tickWidth, vLocation);
	  glVertex2f(indent_x + 1.0, vLocation);
	  glEnd();
	}
	
      }

      if ((*speed_acceleration != FLT_MISS) && (fabs(*speed_acceleration) > 0.1)) {
	float vLocation = float(*speed_acceleration*10.0) * tickSpacing / 10.0 + m_PhysicalSize.y/2;

	glColor3ub( 0, 179, 0);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glVertex2f(indent_x - tickWidth, m_PhysicalSize.y/2);
	glVertex2f(indent_x - tickWidth/2, m_PhysicalSize.y/2);
	glEnd();	
	glBegin(GL_LINES);
	glVertex2f(indent_x - tickWidth/2, m_PhysicalSize.y/2);
	glVertex2f(indent_x - tickWidth/2, vLocation);
	glEnd();
	if (*speed_acceleration > 0.0) {
	  glBegin(GL_LINES);
	  glVertex2f(indent_x - tickWidth/2, vLocation);
	  glVertex2f(indent_x - tickWidth/2 - tickWidth/3, vLocation - tickWidth/2);
	  glEnd();	
	  glBegin(GL_LINES);
	  glVertex2f(indent_x - tickWidth/2, vLocation);
	  glVertex2f(indent_x - tickWidth/2 + tickWidth/3, vLocation - tickWidth/2);
	  glEnd();
	} else {
	  glBegin(GL_LINES);
	  glVertex2f(indent_x - tickWidth/2, vLocation);
	  glVertex2f(indent_x - tickWidth/2 - tickWidth/3, vLocation + tickWidth/2);
	  glEnd();	
	  glBegin(GL_LINES);
	  glVertex2f(indent_x - tickWidth/2, vLocation);
	  glVertex2f(indent_x - tickWidth/2 + tickWidth/3, vLocation + tickWidth/2);
	  glEnd();
	}
      }
      
    }

    glPopMatrix();
  }
  
} // end namespace OpenGC
