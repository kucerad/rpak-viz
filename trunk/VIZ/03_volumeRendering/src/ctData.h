#ifndef _CTDATA_H
#define _CTDATA_H

#include <string>
#include "png.h"
#include "utils.h"
#include "settings.h"
#include <map>
#include "Ray.h"
#include "Vertex.h"
#include "colorMap.h"
#include "BBox.h"
#include "Shader.h"
//#define BACKGROUND_COLOR v3(0.f, 0.f, 0.f)

using namespace std;

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

class CTdata{
public:
	CTdata(ColorMap *colorMap, Shader *shader);
	~CTdata();
	// load CT 3d grid from files... filename in format like in printf with one %i
	bool	loadFromFiles(const char * filename, int cnt, int scaleX, int scaleY, int scaleZ);
	void	loadSphere(int _szX, int _szY, int _szZ, float minV, float maxV);
	
	unsigned int	sampleCount;
	float			sampleDistance;

	Vertex interpolate (Vertex v1, Vertex v2, float isovalue);
	Vertex getVertexAt(float x, float y, float z);
	Vertex* getVertexAt(const int x, const int y, const int z);
	float	getValueAt(const int x, const int y, const int z);
	v3		getCenterPoint();

	void setCm(ColorMap *colorMap);

	void	colorizeRay(Ray * rayIn);
	Shader*		pShader;
	ColorMap*	pColorMap;
	float		szX, szY, szZ;
	v3			center;
private:
	float *	data;

	int dimX, dimY, dimZ;
	int stepX, stepY, stepZ;
	float scX, scY, scZ;

	
	BBox box;
	// cached attributes
	map<Position3i, Vertex, p3iComparator> vertexMap;
};


#endif