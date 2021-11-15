/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcNavaidList.h,v $

  Last modification:
    Date:      $Date: 2004/10/14 19:28:13 $
    Version:   $Revision: 1.1.1.1 $
    Author:    $Author: damion $
  
  Copyright (c) 2001-2003 Damion Shelton
  All rights reserved.
  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef ogcTerrainData_h
#define ogcTerrainData_h

#include <list>
#include <string>
#include "ogcGeographicObjectList.h"

namespace OpenGC
{

class TerrainData : public GeographicObjectList
{

public:

  TerrainData();
  virtual ~TerrainData();

  void SetPathToDEM(string pathToDEM) { m_PathToDEM = pathToDEM;}
  
  /** Check if DEM Files are there, returns true if successful, else false */
  bool CheckFiles();

  void SetCurrentLonLat(double lon, double lat);

protected:
  string m_PathToDEM;

  int m_nlon; // number of longitudes to store. odd number: e.g. 5: will be current lon +/- 2 lon
  int m_nlat; // number of latitudes to store. odd number: e.g. 5: will be current lat +/- 2 lat
  int m_lonmin;
  int m_lonmax;
  int m_latmin;
  int m_latmax;
  int m_curlon;
  int m_curlat;

};

} // end namespace OpenGC

#endif
