/*=============================================================================

  This is the ogcA320MCDUWidget.h header to ogcA320MCDUWidget.cpp

  Created:
    Date:   2011-11-28
    Author: Hans Jansen
    (see ogcSkeletonGauge.cpp for more details)

  Copyright (C) 2011-2016 Hans Jansen (hansjansen@users.sourceforge.net)
  and/or                  Reto Stöckli (stockli@users.sourceforge.net)

  This program is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>. 

=============================================================================*/

/**
 * Airbus A320 style Engine/Warning Display: the Widget
 */

#ifndef A320MCDUWidget_H
#define A320MCDUWidget_H

#include "ogcGaugeComponent.h"
#include "../ogcAirbusColors.h"

extern int verbosity;

namespace OpenGC
{
  class A320MCDUWidget : public GaugeComponent  
  {
  public:
    A320MCDUWidget();
    virtual ~A320MCDUWidget();

    void Render();

  protected:
    int m_Font;
    void drawSpec (int fontWidth, int fontHeight, int xPos, int ypos, char* line);

  };

}
#endif
