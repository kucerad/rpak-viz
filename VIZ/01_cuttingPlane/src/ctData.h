#ifndef _CTDATA_H
#define _CTDATA_H

#include <string>
#include "../common/png.h"
#include "../common/utils.h"
#include "settings.h"

using namespace std;



class CTdata{
public:
	CTdata();
	~CTdata();
	// load CT 3d grid from files... filename in format like in printf with one %i
	bool loadFromFiles(const char * filename, int cnt, int scaleX, int scaleY, int scaleZ);

	// get value at real position
	float getValueAt(const float x, const float y, const float z);

	// get value at position in grid [not scaled]
	float getValueAt(int x, int y, int z, bool &inFlag);

private:
	float *	data;
	int dimX, dimY, dimZ;
	float scX, scY, scZ;
	float max;

	void getCoordsForInterpolation(const float realCoord, const int axisScale, int &lowerCoord, float &t);
};

#endif