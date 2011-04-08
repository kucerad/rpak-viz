#include "ctData.h"

#define BUFFER_OFFSET(i)	((char*) NULL + (i))
#define LIGHT_DIR			(-rayIn->dir)
#define ALPHAACCUM_TRESHOLD 0.01

v3 ambientColor = v3(1.f, 0.f, 0.f);
v3 specularColor = v3(1.f, 1.f, 1.f);;
float ka = 1;
float kd = 1;
float ks = 1;
float sh = 2;




CTdata::CTdata(){
	data				= NULL;
	stepX = 1;
	stepY = 1;
	stepZ = 1;
	sampleDistance = 1;
	sampleCount = 0;
}

CTdata::~CTdata(){
	if (data){
		delete [] data;
	}
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
		chars = printf("LOADING CT images: %03i %%",((z+1)*100)/cnt);
	
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
	printf("LOADING CT images (%i) DONE\n", cnt);
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
	x=max2f(min2f(float(dimX), x),0.f);
	y=max2f(min2f(float(dimY), y),0.f); 
	z=max2f(min2f(float(dimZ), z),0.f);
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
		v->position = v3(x*scX, y*scY, z*scZ)-center;
		//printf("POS[ %f , %f , %f ]\n", v.position.x, v.position.y, v.position.z);
		// normal
		int i;
		float nx, ny, nz;
		nx = getValueAt(x-i, y, z) - getValueAt(x+i, y, z);
		ny = getValueAt(x, y-i, z) - getValueAt(x, y+i, z);
		nz = getValueAt(x, y, z-i) - getValueAt(x, y, z+i);
		v->normal = v3(nx, ny, nz);
		
		// value
		v->value	= getValueAt(x,y,z);

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

void  CTdata::colorizeRay(Ray * rayIn, ColorMap *colorMap, Shader &shader)
{
	// get first intersection with data cube [min, max setting]
	box.intersect(( *rayIn ) );

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
		
		// get value and normal at this position
		v = getVertexAt(pos.x, pos.y, pos.z);
		
		// get color from transfer function
		col = colorMap->mapValueToColor(v.value);
		
		// use phong shading model to shade color
		colDiffuse = col.xyz();
		
		shader.apply(&colShaded, &ambientColor, &colDiffuse, &specularColor, ka, kd, ks, sh, &(v.normal), &(LIGHT_DIR), &(-rayIn->dir));

		// apply this sample to final color
		alpha = 1.0 - col.a; // recalc to opacity
		rayIn->color = rayIn->color + colShaded * (alpha * alphaAccum);
		alphaAccum *= ( 1.0 - alpha );
		
		// Is it reasonable to continue? Will the sample be visible?
		if (alphaAccum<ALPHAACCUM_TRESHOLD){
			// following samples do not participate... skip them!!!
			break;
		}
	} // END for each sample on ray
	// sampling ended

}

