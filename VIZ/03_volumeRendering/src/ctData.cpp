#include "ctData.h"

#define LIGHT_DIR			(-rayIn->dir)
#define ALPHAACCUM_TRESHOLD 0.01


CTdata::CTdata(ColorMap *colorMap, Shader *shader){
	data			= NULL;
	stepX			= 1;
	stepY			= 1;
	stepZ			= 1;
	sampleDistance	= 1;
	sampleCount		= 0;
	pColorMap		= colorMap;
	pShader			= shader;
}

CTdata::~CTdata(){
	SAFE_DELETE_ARRAY_PTR(data)	
	SAFE_DELETE_PTR(pColorMap)
	SAFE_DELETE_PTR(pShader)

	vertexMap.clear();
}

v3 CTdata::getCenterPoint(){
	return center;//v3(scX*float(dimX)*0.5, scY*float(dimY)*0.5,scZ*float(dimZ)*0.5);
}

void CTdata::loadSphere(int _szX, int _szY, int _szZ, float minV, float maxV)
{
	scX = scY = scZ = 1;
	dimX = _szX;
	dimY = _szY;
	dimZ = _szZ;
	szX = dimX * scX;
	szY = dimY * scY;
	szZ = dimZ * scZ;
	data = new float[dimX*dimY*dimZ];
	center = v3 (float(dimX)/2.0, float(dimY)/2.0,float(dimZ)/2.0);
	v3 p;
	v3 diff;
	int x,y,z;
	for (z=0; z<(dimZ); z+=1){
		for (y=0; y<(dimY); y+=1){
			for (x=0; x<(dimX); x+=1){
				p = v3(x,y,z);
				diff = center-p;
				data[z*dimX*dimY + y*dimX +x] = 100.0/diff.length();
			}
		}
	}
	center.x = szX/2.0;
	center.y = szY/2.0;
	center.z = szZ/2.0;
}
	
bool CTdata::loadFromFiles(const char * filename, int cnt, int scaleX, int scaleY, int scaleZ){
	int divide = 2;
	int chars = 0;
	scX = scaleX;
	scY = scaleY;
	scZ = scaleZ/divide;
	char filen [50];
	unsigned char * rawData;
	PNG ctfile;
	int width=0, height=0, width2, height2; 
	int x,y,z;
	float val;
	for(z=0; z<cnt; z++){
		sprintf(filen, filename, z+1);
		ctfile.clear();
		if (!ctfile.load(filen)){
			return false;
		}
		
		rawData		= ctfile.getData();
		height		= ctfile.height;
		width		= ctfile.width;
		width2		= width/float(divide);
		height2		= height/float(divide);
		if (data==NULL){
			data = new float[cnt*width2*height2]; //data = new float[cnt*width*height];
		}
		for (y=0; y<height2; y++){ //height
			for (x=0; x<width2; x++){ //width
				// 2x 8-bit to 1x 16-bit
				val = 256*rawData[(2*y*width + 2*x)*divide]+rawData[(2*y*width + 2*x)*divide + 1]; //val = 256*rawData[2*y*width + 2*x]+rawData[2*y*width + 2*x + 1];
				// save in array
				data[z*height2*width2 + y*width2 +(width2-1-x)] = val; //data[z*width*height + y*width +x] = val;
			}
		}
		BACKSPACE(chars);
		chars = printf(" progress: %03i %%",((z+1)*100)/cnt);
	
	}

	dimX = width2;
    dimY = height2;
	dimZ = cnt;
	

	szX = dimX * scX;
	szY = dimY * scY;
	szZ = dimZ * scZ;
	center.x = szX/2.0;
	center.y = szY/2.0;
	center.z = szZ/2.0;

	// bounding box
	box.bounds[0] = center-v3(szX,szY,szZ)/2.0;// min corner
	box.bounds[1] = center+v3(szX,szY,szZ)/2.0;// max corner

	// backspace...
	BACKSPACE(chars);
	//printf("LOADING CT images (%i) DONE\n", cnt);
	return true;
}

Vertex CTdata::interpolate (Vertex v1, Vertex v2, float t) {
	Vertex v;
	v.position	= v1.position*(1-t) + v2.position * t;
	v.normal	= v1.normal*(1-t)   + v2.normal   * t;
	v.value		= v1.value*(1-t)    + v2.value    * t;
	return v;
}

Vertex CTdata::getVertexAt(float x, float y, float z)
{
	x=max2f(min2f(float(szX), x),0.f);
	y=max2f(min2f(float(szY), y),0.f); 
	z=max2f(min2f(float(szZ), z),0.f);
	// get nearby coordinates
	int xc, yc, zc;
	float xt, yt,zt;
	Vertex* v1, *v2, *v3, *v4, *v5, *v6, *v7, *v8, outVertex;
	getCoordsForInterpolation(x, int(scX), xc, xt);
	getCoordsForInterpolation(y, int(scY), yc, yt);
	getCoordsForInterpolation(z, int(scZ), zc, zt); 

	// get values
	v1 = getVertexAt(xc  , yc  , zc  );
	v2 = getVertexAt(xc  , yc+1, zc  );
	v3 = getVertexAt(xc  , yc  , zc+1);
	v4 = getVertexAt(xc  , yc+1, zc+1);
	v5 = getVertexAt(xc+1, yc  , zc+1);
	v6 = getVertexAt(xc+1, yc+1, zc+1);
	v7 = getVertexAt(xc+1, yc  , zc  );
	v8 = getVertexAt(xc+1, yc+1, zc  );

	// interpolate them
	outVertex = interpolate3Dlin(v1, v2, v3, v4, v5, v6, v7, v8, xt, yt, zt);
	
	return outVertex;
}

Vertex* CTdata::getVertexAt(int x, int y, int z)
{
	Position3i p;
	p.x = x;
	p.y = y;
	p.z = z;
	// is Vetrex allready precomputed in vertexMap?
	Vertex* v = &(vertexMap[p]);
	if (!v->isValid){
		// not precomputed... compute now...
		// position
		v->position = v3(x*scX, y*scY, z*scZ);
		//printf("POS[ %f , %f , %f ]\n", v.position.x, v.position.y, v.position.z);
		// normal
		float nx, ny, nz;
		nx = getValueAt(x-1, y, z) - getValueAt(x+1, y, z);
		ny = getValueAt(x, y-1, z) - getValueAt(x, y+1, z);
		nz = getValueAt(x, y, z-1) - getValueAt(x, y, z+1);
		v->normal = v3(nx, ny, nz);
		
		// value
		v->value	= getValueAt(x,y,z);

		// color
		// get color from transfer function
		v->color = pColorMap->mapValueToColor(v->value);

		// validate
		v->isValid = true;
	}
	return v;

}

float CTdata::getValueAt(const int x, const int y, const int z)
{
	int i,j,k;
	i=max(min(dimX, x),0);
	j=max(min(dimY, y),0); 
	k=max(min(dimZ, z),0);
	return data[k*dimY*dimX + j*dimX +i];
}

void  CTdata::colorizeRay(Ray * rayIn)
{
	// get first intersection with data cube [min, max setting]
	if (!box.intersect( rayIn  )){
		rayIn->color = BACKGROUND_COLOR;
		return;
	}

	// sample ray and calc colors
	float i, alphaAccum=1.f, alpha;
	Vertex v;
	v3 pos, colDiffuse, colShaded;
	v4 col;
	for ( i = rayIn->min + sampleDistance*0.5;
		  i < rayIn->max;
		  i += sampleDistance )
	{ // for each sample on ray
		sampleCount++;

		// get sample position
		pos = rayIn->org + rayIn->dir * i;
		
		// get value, normal and color at this position
		v = getVertexAt(pos.x, pos.y, pos.z);
		
		// use phong shading model to shade color
		v3 colDiffuse = v.color.xyz();
		v3 outputColor;
		pShader->apply(&(outputColor), &(colDiffuse), &(v.normal), &(LIGHT_DIR), &(-rayIn->dir));
		
		v.color.setFromV3(outputColor);

		// apply this sample to final color
		alpha = 1.0 - v.color.a; // recalc to opacity
		rayIn->color = rayIn->color + v.color.xyz() * (alpha * alphaAccum);
		alphaAccum *= ( 1.0 - alpha );
		
		// Is it reasonable to continue? Will the sample be visible?
		if (alphaAccum<ALPHAACCUM_TRESHOLD){
			// following samples do not participate... skip them!!!
			break;
		}
	} // END for each sample on ray
	// sampling ended
}

