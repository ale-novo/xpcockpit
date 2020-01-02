/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcB737PFDSpeedIndicator.cpp,v $

  Copyright (C) 2002 by:
    Original author:
      Michael DeFeyter
    Contributors (in alphabetical order):

  Last modification:
    Date:      $Date: 2003/04/18 06:00:35 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
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

#include <math.h>
#include "ogcCircleEvaluator.h"
#include "B737/B737PFDSA/ogcB737PFDSpeedIndicator.h"

namespace OpenGC
{

B737PFDSpeedIndicator::B737PFDSpeedIndicator()
{
  printf("B737PFDSpeedIndicator constructed\n");

  m_Font = m_pFontManager->LoadDefaultFont();

  m_PhysicalPosition.x = 0;
  m_PhysicalPosition.y = 0;

  m_PhysicalSize.x = 80;
  m_PhysicalSize.y = 80;

  m_Scale.x = 1.0;
  m_Scale.y = 1.0;

}

B737PFDSpeedIndicator::~B737PFDSpeedIndicator()
{

}

//////////////////////////////////////////////////////////////////////

void B737PFDSpeedIndicator::Render()
{
	// Call base class to setup viewport and projection
	GaugeComponent::Render();

	/*
	
	float value = m_pDataSource->GetAirframe()->GetIAS();
	float avalue = m_pDataSource->GetAirframe()->GetGround_Speed_K();

	// For drawing circles
	CircleEvaluator aCircle;

	// First, store the "root" position of the gauge component
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	char buf[10];
	GLUquadric *qobj;
	float R = 39.5;

	// gray background
	glColor3ub(93, 116, 150);

	aCircle.SetOrigin(40,40);
	aCircle.SetRadius(R);
	aCircle.SetDegreesPerPoint(5);

	aCircle.SetArcStartEnd(270.0,360.0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0,80);
	glVertex2f(0,40);
	aCircle.Evaluate();
	glVertex2f(40,80);
	glEnd();

	aCircle.SetArcStartEnd(0.0,90.0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(80,80);
	glVertex2f(40,80);
	aCircle.Evaluate();
	glVertex2f(80,40);
	glEnd();

	aCircle.SetArcStartEnd(90.0,180.0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(80,0);
	glVertex2f(80,40);
	aCircle.Evaluate();
	glVertex2f(40,0);
	glEnd();

	aCircle.SetArcStartEnd(180.0,270.0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0,0);
	glVertex2f(40,0);
	aCircle.Evaluate();
	glVertex2f(0,40);
	glEnd();


	// cut out corners
	glColor3ub(0,0,0);
	glBegin(GL_POLYGON);
	glVertex2f(0,74);
	glVertex2f(0,80);
	glVertex2f(6,80);
	glVertex2f(0,74);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(74,80);
	glVertex2f(80,80);
	glVertex2f(80,74);
	glVertex2f(74,80);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(80,6);
	glVertex2f(80,0);
	glVertex2f(74,0);
	glVertex2f(80,6);
	glEnd();
	
	glBegin(GL_POLYGON);
	glVertex2f(6,0);
	glVertex2f(0,0);
	glVertex2f(0,6);
	glVertex2f(6,0);
	glEnd();


	// marker lines
	m_pFontManager->SetSize(m_Font, 4, 4);
	glTranslatef(40, 40, 0);

	float min = 60;
	float max = 250;
	int step = 5;
	int bigstep = 10;
	int textstep = 20;
	float minDegrees = 0;
	float maxDegrees = 245;
	float percentage, degree, radians, xcircle, ycircle, linecut, tX, tY, xs;
	for (xs = min; xs <= max; xs += step) {
		percentage = (xs - min) / (max - min) ;
		degree = minDegrees + ((maxDegrees - minDegrees) * percentage);
		radians = degree * atan2(0.0, -1.0) / 180;
		if ((int)xs % bigstep == 0)
			linecut = 0;
		else
			linecut = 2.5;
		glLineWidth(2.0);
		glColor3ub(255, 255, 255);
		glBegin(GL_LINE_STRIP);
		xcircle = (R - linecut) * sin(radians);
		ycircle = (R - linecut) * cos(radians);
		glVertex2f(xcircle, ycircle);
		xcircle = (R - 5) * sin(radians);
		ycircle = (R - 5) * cos(radians);
		glVertex2f(xcircle, ycircle);
		glEnd();
		// text in unitmarkers
		if (((int)xs % textstep == 0) || (xs == max)) {
			if ((xs == 60) || (xs == 80) || (xs == 200) || (xs == 220)) {
				tX = -4;
				tY = -2;
			}
			else if ((xs >= 100) && (xs <= 180)) {
				tX = -6.5;
				tY = -2;
			}
			else if (xs == 240) {
				tX = -2;
				tY = -2;
			}
			else if (xs == 250) {
				tX = -2;
				tY = -1.5;
			}
			glTranslatef(tX, tY, 0);
			snprintf(buf,  sizeof(buf), "%.0f", xs);
			xcircle = (R - 9) * sin(radians);
			ycircle = (R - 9) * cos(radians);
			m_pFontManager->Print(xcircle, ycircle, buf, m_Font);
			glTranslatef(-tX, -tY, 0);
		}
	}
	min = 250;
	max = 300;
	step = 10;
	bigstep = 50;
	textstep = 50;
	minDegrees = 245;
	maxDegrees = 270;
	for (xs = min; xs <= max; xs += step) {
		if (xs == min)
			continue;
		percentage = (xs - min) / (max - min) ;
		degree = minDegrees + ((maxDegrees - minDegrees) * percentage);
		radians = degree * atan2(0.0, -1.0) / 180;
		if ((int)xs % bigstep == 0)
			linecut = 0;
		else
			linecut = 2.5;
		glLineWidth(2.0);
		glColor3ub(255, 255, 255);
		glBegin(GL_LINE_STRIP);
		xcircle = (R - linecut) * sin(radians);
		ycircle = (R - linecut) * cos(radians);
		glVertex2f(xcircle, ycircle);
		xcircle = (R - 5) * sin(radians);
		ycircle = (R - 5) * cos(radians);
		glVertex2f(xcircle, ycircle);
		glEnd();
		// text in unitmarkers
		if (((int)xs % textstep == 0) || (xs == max)) {
			tX = -2.5;
			tY = -1.5;
			glTranslatef(tX, tY, 0);
			snprintf(buf,  sizeof(buf), "%.0f", xs);
			xcircle = (R - 9) * sin(radians);
			ycircle = (R - 9) * cos(radians);
			m_pFontManager->Print(xcircle, ycircle, buf, m_Font);
			glTranslatef(-tX, -tY, 0);
		}
	}
	min = 300;
	max = 450;
	step = 10;
	bigstep = 50;
	textstep = 50;
	minDegrees = 270;
	maxDegrees = 330;
	float overspeedValue = 320;
	float percentageE, degreeE;
	bool red = true;
	for (xs = min; xs <= max; xs += step) {
		if (xs == min)
			continue;
		percentage = (xs - min) / (max - min) ;
		degree = minDegrees + ((maxDegrees - minDegrees) * percentage);
		radians = degree * atan2(0.0, -1.0) / 180;
		if ((int)xs % bigstep == 0)
			linecut = 0;
		else
			linecut = 2.5;
		glLineWidth(2.0);
		glColor3ub(255, 255, 255);
		glBegin(GL_LINE_STRIP);
		xcircle = (R - linecut) * sin(radians);
		ycircle = (R - linecut) * cos(radians);
		glVertex2f(xcircle, ycircle);
		xcircle = (R - 5) * sin(radians);
		ycircle = (R - 5) * cos(radians);
		glVertex2f(xcircle, ycircle);
		glEnd();
		// text in unitmarkers
		if (((int)xs % textstep == 0) && (xs != max)) {
			tX = -2.5;
			tY = -1.5;
			glTranslatef(tX, tY, 0);
			snprintf(buf, sizeof(buf), "%.0f", xs);
			xcircle = (R - 9) * sin(radians);
			ycircle = (R - 9) * cos(radians);
			m_pFontManager->Print(xcircle, ycircle, buf, m_Font);
			glTranslatef(-tX, -tY, 0);
		}

		// overspeed markers
		if ((xs >= overspeedValue) && ((int)xs % 20 == 0)) {
			glLineWidth(2.0);
			if (red)
				glColor3ub(255, 0, 0);
			else
				glColor3ub(255, 255, 255);

			if (xs == overspeedValue) {
				glBegin(GL_LINE_STRIP);
				xcircle = (R + 1) * sin(radians);
				ycircle = (R + 1) * cos(radians);
				glVertex2f(xcircle, ycircle);
				xcircle = (R - 5) * sin(radians);
				ycircle = (R - 5) * cos(radians);
				glVertex2f(xcircle, ycircle);
				glEnd();
			}

			if (xs >= max-20)
				percentageE = (xs+10 - min) / (max - min) ;
			else
				percentageE = (xs+20 - min) / (max - min) ;
			degreeE = minDegrees + ((maxDegrees - minDegrees) * percentageE);

			qobj = gluNewQuadric();
			gluPartialDisk(qobj, R, R+1, 50, 1, degree, degreeE-degree);
      gluDeleteQuadric(qobj);

			red = !red;
		}


	}


	// rectangle containg text
	glLineWidth(2.0);
	glColor3ub(255, 255, 255);
	glBegin(GL_LINE_LOOP);
	glVertex2f( -10, -5 );
	glVertex2f( 10, -5 );
	glVertex2f( 10, -20 );
	glVertex2f( -10, -20 );
	glEnd();


	// text
	m_pFontManager->SetSize(m_Font, 7, 8);
	glColor3ub(255, 255, 255);
	snprintf(buf, sizeof(buf), "%.0f", value);
	float textX;
	if (value < 10)
		textX = 3;
	else if (value < 100)
		textX = -2.25;
	else
		textX = -7.5;
	m_pFontManager->Print(textX, -16, buf, m_Font);


	// circle in the middle of the gauge
	glLineWidth(2.0);
	glColor3ub(255, 255, 255);

	qobj = gluNewQuadric();
	gluPartialDisk(qobj, 1, 2, 50, 1, 0, 360);
  gluDeleteQuadric(qobj);


	// gauge
	float vGauge;
	if (value < 60)
		vGauge = 60;
	else
		vGauge = value;

	if (value <= 250) {
		min = 60;
		max = 250;
		minDegrees = 0;
		maxDegrees = 245;
	}
	else if (value <= 300) {
		min = 250;
		max = 300;
		minDegrees = 245;
		maxDegrees = 270;
	}
	else {
		min = 300;
		max = 450;
		minDegrees = 270;
		maxDegrees = 330;
	}

	glPushMatrix();

	glLineWidth(3.0);
	glColor3ub(255, 255, 255);

	percentage = (vGauge - min) / (max-min) ;
	degree = minDegrees + ((maxDegrees - minDegrees) * percentage);
	glRotated(-degree, 0, 0, 1);

	glBegin(GL_LINE_STRIP);
	glVertex2f(0, 1.5);
	glVertex2f(0, R-5);
	glEnd();

	glLineWidth(1.0);
	glBegin(GL_POLYGON);
	glVertex2f(0, R-5);
	glVertex2f(-2.3, R-12.7);
	glVertex2f(-2.3, R-17);
	glVertex2f(0, R-19);
	glVertex2f(2.3, R-17);
	glVertex2f(2.3, R-12.7);
	glEnd();

	glPopMatrix();


	// autopilot ref
	if (avalue < 60)
		vGauge = 60;
	else
		vGauge = avalue;

	glPushMatrix();

	glLineWidth(1.0);
	glColor3ub(250, 10, 200);
	percentage = (vGauge - min) / (max-min) ;
	degree = minDegrees + ((maxDegrees - minDegrees) * percentage);
	glRotated(-degree, 0, 0, 1);

	glBegin(GL_POLYGON);
	glVertex2f(0, R-3.5);
	glVertex2f(-2.5, R-1);
	glVertex2f(-2.5, R+2);
	glVertex2f(-0.4, R+2);
	glVertex2f(-0.4, R);
	glVertex2f(0.4, R);
	glVertex2f(0.4, R+2);
	glVertex2f(2.5, R+2);
	glVertex2f(2.5, R-1);
	glEnd();
	
	glPopMatrix();


	glPopMatrix();

	*/
}

}
