/*=============================================================================

  This is the ogcA320StbyAlt.h header to ogcA320StbyAlt.c

  Created:
    Date:   2018-05-03
    Author: Hans Jansen
    last change: 2020-01-23
    (see ogcSkeletonGauge.c for more details)

  Copyright (C) 2018-2020 Hans Jansen (hansjansen@users.sourceforge.net)
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
 * Airbus A320 style Standby Altitude Indicator
 */

#ifndef ogcA320StbyAlt_h
#define ogcA320StbyAlt_h

#include "ogcGauge.h"

extern int verbosity;

namespace OpenGC {

  class A320StbyAlt : public Gauge {
  
    public:
      A320StbyAlt ();
      virtual ~A320StbyAlt ();
      void Render ();

  };

} // end namespace OpenGC

#endif
