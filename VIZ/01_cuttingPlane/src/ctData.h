#ifndef _CTDATA_H
#define _CTDATA_H

#include <string>
#include "png.h"
#include "utils.h"
#include "settings.h"
//#include "../../include/GLEE/GLee.h" 

using namespace std;



class CTdata{
public:
	CTdata();
	~CTdata();
	// load CT 3d grid from files... filename in format like in printf with one %i
	bool	loadFromFiles(const char * filename, int cnt, int scaleX, int scaleY, int scaleZ);

	// get value at real position
	inline float	getValueAt(const float x, const float y, const float z){
		// get nearby coordinates
		int xc, yc, zc;
		float xt, yt,zt;
		float val1, val2, val3, val4, val5, val6, val7, val8;
		getCoordsForInterpolation(x, int(scX), xc, xt);
		getCoordsForInterpolation(y, int(scY), yc, yt);
		getCoordsForInterpolation(z, int(scZ), zc, zt); 

		// get values
		bool in = false;
		val1 = getValueAt(xc  , yc  , zc  , in);
		val2 = getValueAt(xc  , yc+1, zc  , in);
		val3 = getValueAt(xc  , yc  , zc+1, in);
		val4 = getValueAt(xc  , yc+1, zc+1, in);
		val5 = getValueAt(xc+1, yc  , zc+1, in);
		val6 = getValueAt(xc+1, yc+1, zc+1, in);
		val7 = getValueAt(xc+1, yc  , zc  , in);
		val8 = getValueAt(xc+1, yc+1, zc  , in);
		float val = 0.0f;
		if (in){
			// at least one point is inside 3D grid... 
			// interpolate then
			val = interpolate3Dlin(val1, val2, val3, val4, val5, val6, val7, val8, xt, yt, zt);
		} else {
			return BACKGROUND_VALUE;
		}

		return val/3272;
	}
	inline float	getValueAt(const v3 position){
		return getValueAt(position.x, position.y, position.z);
	}

	v3		getCenterPoint();

	void	create3DIsosurrface(float isovalue, int step_x, int step_y, int step_z );
	float	szX, szY, szZ;
private:
	float *	data;

	int dimX, dimY, dimZ;
	
	float scX, scY, scZ;
	float max;

	// get value at position in grid [not scaled]
	inline float getValueAt(int x, int y, int z, bool &inFlag){
		if (x<0 || y<0 || z<0 || x>=dimX || y>=dimY || z>=dimZ){
			// out of grid
			return 0.0f;
		}
		inFlag = true;
		return data[ z*dimX*dimY + y*dimX + x ];
	}
};


#endif