/*=============================================================================

  This is the ogcA320PFDMiscInfo.h header to ogcA320PFDMiscInfo.cpp

  Created:
    Date:   2015-05-15
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
 * Airbus A320 style Primary Flight Display: Miscellaneous Info
 */

#ifndef A320PFDMISCINFO_H
#define A320PFDMISCINFO_H

#include "ogcGaugeComponent.h"
#include "../ogcAirbusColors.h"

extern int verbosity;

namespace OpenGC
{
  class A320PFDMiscInfo : public GaugeComponent  
  {
  public:
    A320PFDMiscInfo();
    virtual ~A320PFDMiscInfo();

    void Render();

  protected:
    int m_Font;
  };
}
#endif

