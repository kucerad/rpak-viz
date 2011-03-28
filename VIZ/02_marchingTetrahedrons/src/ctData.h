#ifndef _CTDATA_H
#define _CTDATA_H

#include <string>
#include "png.h"
#include "utils.h"
#include "settings.h"
#include <map>

using namespace std;
class Vertex{
public:
	Vertex(){
		isValid=false;
	}
	~Vertex(){}
	v3		normal;
	v3		position;
	float	value;
	bool	isValid;
};
union Position3i{
	int pos[3];
	struct{int x, y, z;};
};
struct p3iComparator {
  bool operator() (const Position3i& p1, const Position3i& p2) const
  {
	  if (p1.x<p2.x){
		return true;
	  } else if (p1.x==p2.x){
		if (p1.y<p2.y){
			return true;
		} else if (p1.y==p2.y){
			return (p1.z<p2.z);
		} 
	  }
	  return false;
  }
};
static const char powersOf2[6]	 = {1,2,4,8,16,32};
static const int tetrahedron5a[] = {
								0,2,5,7,
								0,1,2,5,
								0,2,3,7,
								0,4,5,7,
								2,5,6,7
							 };
static const int tetrahedron5b[] = {
								0,1,3,4,
								1,3,4,6,
								1,2,3,6,
								1,4,5,6,
								3,4,6,7
							 };
static const int tetrahedron6[] = {
								0,1,3,5,
								1,2,3,5,
								2,3,5,6,
								3,5,6,7,
								3,4,5,7,
								0,3,4,5
							 };

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
		val1 = getValueAt(xc  , yc  , zc  );
		val2 = getValueAt(xc  , yc+1, zc  );
		val3 = getValueAt(xc  , yc  , zc+1);
		val4 = getValueAt(xc  , yc+1, zc+1);
		val5 = getValueAt(xc+1, yc  , zc+1);
		val6 = getValueAt(xc+1, yc+1, zc+1);
		val7 = getValueAt(xc+1, yc  , zc  );
		val8 = getValueAt(xc+1, yc+1, zc  );
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

	void  create3dIsosurface(float isovalue, int stepX, int stepY, int stepZ);
	void    draw3dIsosurface();
	void destroy3dIsosurface();


	v3		getCenterPoint();
	float szX, szY, szZ;
private:
	float *	data;

	int dimX, dimY, dimZ;
	int stepX, stepY, stepZ;
	float scX, scY, scZ;
	float maxSz;

	// get value at position in grid [not scaled]
	inline float getValueAt(int x, int y, int z){
		if (x<0 || y<0 || z<0 || x>=dimX || y>=dimY || z>=dimZ){
			// out of grid
			return 0.0f;
		}
		return data[ z*dimX*dimY + y*dimX + x ];
	}

	inline float getValueAt2(int x, int y, int z){
		x = max(min(x,dimX-1), 0);
		y = max(min(y,dimY-1), 0);
		z = max(min(z,dimZ-1), 0);

		return data[ z*dimX*dimY + y*dimX + x ];
	}

	v3 **	normalArr;
	int		normalArrSize;
	inline bool getNormalAt(int x, int y, int z, v3* normal){
		if (x<0 || y<0 || z<0 || x>=dimX || y>=dimY || z>=dimZ){
			// out of grid
			return false;
		}
		int index = (z*dimX*dimY/stepZ) + (y*dimX/stepY) + (x/stepX);
		normal = normalArr[ index ];
		if (normal==NULL){
			// calc normal... Central Difference Gradient
			float nx = getValueAt2(x-1, y, z) - getValueAt2(x+1, y, z);
			float ny = getValueAt2(x, y-1, z) - getValueAt2(x, y+1, z);
			float nz = getValueAt2(x, y, z-1) - getValueAt2(x, y, z+1);
			normal = new v3(nx, ny, nz);
			normal->normalize();
			normalArr[ index ] = normal;
		}
		return true;
	}
	Vertex& getVertexAt(int x, int y, int z);
	

	// cached attributes
	map<Position3i, Vertex, p3iComparator> vertexMap;



	// OpenGL variables
	GLuint	elementBufferID;
	GLuint	vertexBufferID;
	GLuint	shaderProgramID;
	v3		color;
	GLfloat	*pVertexBufferData;
	GLfloat	*pNormalBufferData;
	GLuint	*pElementBufferData;

	int		numIndices;
	int		numVertices;

	// 3d surface data
	vector<GLfloat>	vertices;
	vector<GLuint>	indices;
	vector<GLfloat>	normals;

	void initBuffers();
	void triangulateCell5(int x, int y, int z, float isovalue);
	bool getMinMaxForCell(int x, int y, int z, float *max, float *min);
};


#endif