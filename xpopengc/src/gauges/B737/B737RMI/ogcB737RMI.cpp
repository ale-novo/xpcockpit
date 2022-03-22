/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Copyright (C) 2022  by:
  Reto Stockli
  
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

#include "ogcB737RMI.h"
#include "ogcCircleEvaluator.h"

namespace OpenGC
{

  B737RMI::B737RMI()
  {
    printf("B737RMI constructed\n");
 
    m_PhysicalSize.x = 100;
    m_PhysicalSize.y = 100;
  
    // We need a font to draw the ILS display
    m_Font = m_pFontManager->LoadDefaultFont();
  
    // We want to draw an outline
    this->SetGaugeOutline(true);

  }

  B737RMI::~B737RMI()
  {
    // Destruction handled by base class
  }

  void B737RMI::Render()
  {
  
    //  int *avionics_on = link_dataref_int("sim/cockpit/electrical/avionics_on");
    // if (*avionics_on == 1) {
    Gauge::Render();

    float tickWidth = 1.5;
    float fontHeight = 6;
    float fontWidth = 4.5;
    float lineWidth = 3.0;
	
    float R = m_PhysicalSize.x / 2.0 * 0.9;
    float center_x = m_PhysicalSize.x / 2.0;
    float center_y = m_PhysicalSize.y / 2.0;
    
    float value = 45; // = m_pDataSource->GetAirframe()->GetMag_Heading();
	
    bool adfValid = true; // = m_pDataSource->GetAirframe()->GetAdf1_Valid();
    float adfValue = 120.0; // = m_pDataSource->GetAirframe()->GetAdf1_Heading();
	
    bool nav1Valid = true; // = (m_pDataSource->GetAirframe()->GetNav1_Valid() != 0 ? true : false);
    float nav1DirValue = 180.0; // = m_pDataSource->GetAirframe()->GetNav1_Bearing();
    //	bool nav1HasDME = m_pDataSource->GetAirframe()->GetNav1_Has_DME();
    //	float nav1DMEValue = m_pDataSource->GetAirframe()->GetNav1_DME();

    bool nav2Valid = true; // (m_pDataSource->GetAirframe()->GetNav2_Valid() != 0 ? true : false);
    float nav2DirValue = 180.0; // = m_pDataSource->GetAirframe()->GetNav2_Bearing();
    //	bool nav2HasDME = m_pDataSource->GetAirframe()->GetNav2_Has_DME();
    //	float nav2DMEValue = m_pDataSource->GetAirframe()->GetNav2_DME();

    int i;
    char buf[10];


    // Draw the background rectangle
    // Draw in dark gray
    glColor3ub(COLOR_GRAY35);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    glVertex2f(0.0, 0.0);
    glVertex2f(0.0, m_PhysicalSize.y);
    glVertex2f(m_PhysicalSize.x, m_PhysicalSize.y);
    glVertex2f(m_PhysicalSize.x, 0.0);
    glVertex2f(0.0, 0.0);
    glEnd();

    // Set up the circles
    CircleEvaluator aCircle;
    aCircle.SetRadius(R);
    aCircle.SetArcStartEnd(0,360);
    aCircle.SetDegreesPerPoint(10);
    CircleEvaluator bCircle;
    bCircle.SetRadius(R/8);
    bCircle.SetArcStartEnd(0,360);
    bCircle.SetDegreesPerPoint(20);
        
    glColor3ub(COLOR_BLACK);
    aCircle.SetOrigin(center_x, center_y);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    aCircle.Evaluate();
    glEnd();
 
    // First, store the "root" position of the gauge component
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(center_x, center_y, 0); // move to center
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    // outer circle markers on -90, -45, 0, 45, 90
    for (i = 0; i < 360; i += 45) {
      glPushMatrix();
      glRotated(i, 0, 0, 1);
      glLineWidth(2.0);
      glColor3ub(COLOR_WHITE);
      glBegin(GL_POLYGON);
      glVertex2f(0, R);
      glVertex2f(2, R+3);
      glVertex2f(-2, R+3);
      glEnd();
      glPopMatrix();
    }

    glPushMatrix();
    glRotated(value, 0, 0, 1);

    // draw marker lines & text
    m_pFontManager->SetSize(m_Font, fontWidth, fontHeight);

    float linelength;
    float linestart;
    float linewidth;
    for (i = 0; i > -360; i -= 5) {
      if (i % 10 == 0) {
	linelength = 6;
	linestart = 0;
	linewidth = 3;
      } else {
	linelength = 3;
	linestart = 3;
	linewidth = 2;
      }

      glPushMatrix();

      glRotated(i, 0, 0, 1);
      glLineWidth(linewidth);
      glColor3ub(COLOR_WHITE);
      glBegin(GL_LINE_STRIP);
      glVertex2f(0, R-linestart);
      glVertex2f(0, R-linestart-linelength);
      glEnd();

      if (i % 30 == 0) {
	snprintf(buf, sizeof(buf), "%d", -i/10);
	if (i < -90)
	  m_pFontManager->Print(-fontWidth*0.75, R-13, buf, m_Font);
	else
	  m_pFontManager->Print(-fontWidth*0.75/2, R-13, buf, m_Font);
      }

      glPopMatrix();
    }

    glPopMatrix();

    // draw VOR/ADF arrows
    if (nav1Valid) {
      glPushMatrix();
      glRotated(-nav1DirValue, 0, 0, 1);
      glColor3ub(COLOR_WHITE);
      glBegin(GL_POLYGON);
      glVertex2f(0, R-7);
      glVertex2f(-3, R-16);
      glVertex2f(3, R-16);
      glEnd();

      glLineWidth(lineWidth*1.5);
      glEnable(GL_LINE_STIPPLE);
      glLineStipple( 4, 0x3F3F );
      glBegin(GL_LINES);
      glVertex2f(0.0,R-15);
      glVertex2f(0.0,-R+6);
      glEnd();
      glDisable(GL_LINE_STIPPLE);

      /*
      bool draw = true;
      glLineWidth(4.0);
      for (i = R-7; i >= -R+9; i -= 2*(R-9)/14) {
	if (draw) {
	  glBegin(GL_LINE_STRIP);
	  glVertex2f(0, i);
	  glVertex2f(0, i-(2*(R-9)/14));
	  glEnd();
	}
	draw = !draw;
      }
      */
      glPopMatrix();
    }
    if (nav2Valid) {
      glPushMatrix();
      glRotated(-nav2DirValue, 0, 0, 1);
      glLineWidth(lineWidth);
      glColor3ub(COLOR_WHITE);
      glBegin(GL_LINE_LOOP);
      glVertex2f(0, R-5);
      glVertex2f(-4, R-17);
      glVertex2f(4, R-17);
      glEnd();
      glBegin(GL_LINE_LOOP);
      glVertex2f(-1.5, R-17);
      glVertex2f(1.5, R-17);
      glVertex2f(1.5, -R+17);
      glVertex2f(0.0, -R+9);
      glVertex2f(-1.5, -R+17);
      glEnd();
      glPopMatrix();
    }    
  
    /* Small Circle in the middle holding needles */
    glPushMatrix();
    glColor3ub(COLOR_GRAY15);
    aCircle.SetOrigin(center_x, center_y);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);
    bCircle.Evaluate();
    glEnd();
    glPopMatrix();

    glPopMatrix();
  
    //}
  }

} // end namespace OpenGC

