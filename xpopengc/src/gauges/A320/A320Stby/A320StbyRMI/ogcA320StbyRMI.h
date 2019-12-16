/*=============================================================================

  This is the ogcA320StbyRMI.h header to ogcA320StbyRMI.cpp

  Created:
    Date:   2011-11-14
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
 * Airbus A320 style Standby Radio Magnetic Indicator
 */

#ifndef OGCA320StbyRMI_H
#define OGCA320StbyRMI_H

#include "ogcGauge.h"

namespace OpenGC
{

class A320StbyRMI : public Gauge  
{
public:

  A320StbyRMI();
  virtual ~A320StbyRMI();

  void Render();

};

} // end namespace OpenGC

#endif
