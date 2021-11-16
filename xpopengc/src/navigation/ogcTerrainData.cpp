/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org
  
  Module:  $RCSfile: ogcTerrainData.cpp,v $

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

  =========================================================================*/

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>    // std::min,max
#include "ogcTerrainData.h"

namespace OpenGC
{

  using namespace std;

  TerrainData
  ::TerrainData()
  {
    m_nlon = 7; // make nlon dynamic depending on latitude (we need more lons at higher lats)
    m_nlat = 5;

    m_pplon = 120;
    m_pplat = 120;
   
  }

  TerrainData
  ::~TerrainData()
  {

  }

  bool
  TerrainData
  ::CheckFiles()
  {

    bool success = true;
    char filename[100];
    ifstream ifile;

    for (int f=0;f<16;f++) {
      sprintf(filename,"%s/%c%s",m_PathToDEM.c_str(),f+97,"10g");
      ifile.open(filename);
      if(ifile) {
	//printf("DEM File exists: %s \n",filename);
	ifile.close();
      } else {
	printf("DEM File doesn't exist: %s \n",filename);
	success = false;
      }
    }

    if (!success) printf("DEM Dataset incomplete. Please Check\n");
    
    return success;
  }

  void
  TerrainData
  ::SetCurrentLonLat(double lon, double lat)
  {
    /* limit to 179/89 because lon = 179 will read lons 179-180 and lat 89 will read lats 89-90 */
    m_curlon = min((int) lon,179);
    m_curlat = min((int) lat,89);

    /* TODO: HANDLE Dateline and Poles */
    m_lonmin = max(min(m_curlon - (m_nlon-1)/2,179),-180);
    m_lonmax = max(min(m_curlon + (m_nlon-1)/2,179),-180);
    m_latmin = max(min(m_curlat - (m_nlat-1)/2,89),-90);
    m_latmax = max(min(m_curlat + (m_nlat-1)/2,89),-90);
   
    printf("Current lon/lat: %i %i \n",m_curlon,m_curlat);
    printf("Lonmin/Lonmax: %i %i \n",m_lonmin,m_lonmax);
    printf("Latmin/Latmax: %i %i \n",m_latmin,m_latmax);
    
  }

  bool
  TerrainData
  ::ReadDEMLonLat(int lon, int lat)
  {
    // 4 j tiles are arranged from N->S by 40, 50, 50, 40 degrees latitude
    // 4 i tiles are arranged from W->E by 90 degrees each

    int i,j,f,x,y,l;
    int latmax, lat1;
    ifstream ifile;
    short buffer[m_pplon];
    char filename[100];

    lat1=lat+1; // We need UL corner
    
    if (lat1>50) {
      j=0;
      latmax = 90;
    } else if (lat1>0) {
      j=1;
      latmax = 50;
    } else if (lat1>-50) {
      j=2;
      latmax = 0;
    } else {
      j=3;
      latmax = -50;
    }
    i = min((lon+180)/90,3);

    f = i + (j*4);

    x = (lon - (i*90-180))*m_pplon;
    y = (latmax - lat1)*m_pplat;

    printf("%i %i \n",x,y);
    
    sprintf(filename,"%s/%c%s",m_PathToDEM.c_str(),f+97,"10g");
    ifile.open (filename, ios::in | ios::binary);
    for (l=0;l<m_pplat;l++) {
      ifile.seekg (2*(x+(y+l)*90*m_pplon));
      ifile.read ((char*)&buffer, sizeof(buffer));
      printf("%i %i %i %i \n",buffer[0],buffer[1],buffer[2],buffer[3]);
    }
    ifile.close();

    return true;
  }

} // end namespace OpenGC
