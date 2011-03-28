#include "ctData.h"

CTdata::CTdata(){
	data				= NULL;
	
	elementBufferID		= 0;
	vertexBufferID		= 0;
	shaderProgramID		= 0;
	
	color				= v3(1.f,1.f,1.f);
	
	pVertexBufferData	= NULL;
	pNormalBufferData	= NULL;
	pElementBufferData	= NULL;

	numIndices	= 0;
	numVertices	= 0;

	stepX = 1;
	stepY = 1;
	stepZ = 1;

}

CTdata::~CTdata(){
	if (data){
		delete [] data;
	}
	vertexMap.clear();
}

v3 CTdata::getCenterPoint(){
	return v3(scX*float(dimX)*0.5, scY*float(dimY)*0.5,scZ*float(dimZ)*0.5);
}

bool CTdata::loadFromFiles(const char * filename, int cnt, int scaleX, int scaleY, int scaleZ){
	int chars = 0;
	scX = scaleX;
	scY = scaleY;
	scZ = scaleZ;
	char filen [50];
	unsigned char * rawData;
	PNG ctfile;
	int width=0, height=0; 
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
		if (data==NULL){
			data = new float[cnt*width*height];
		}
		for (y=0; y<height; y++){
			for (x=0; x<width; x++){
				// 2x 8-bit to 1x 16-bit
				val = 256*rawData[2*y*width + 2*x]+rawData[2*y*width + 2*x + 1];
				// save in array
				data[z*width*height + y*width +x] = val;
			}
		}
		BACKSPACE(chars);
		chars = printf("LOADING CT images: %03i %%",((z+1)*100)/cnt);
	
	}
	dimX = width;
	dimY = height;
	dimZ = cnt;

	szX = dimX * scX;
	szY = dimY * scY;
	szZ = dimZ * scZ;
	// backspace...
	BACKSPACE(chars);
	printf("LOADING CT images (%i) DONE\n", cnt);
	return true;
}

void CTdata::create3dIsosurface(float isovalue, int _stepX, int _stepY, int _stepZ)
{
	// prepare structures
	stepX = _stepX;
	stepY = _stepY;
	stepZ = _stepZ;
	

	// find cell with isosurface inside
	int x,y,z;
	float minV, maxV;
	for (z=0; z<(dimZ-1); z+=stepZ){
		for (y=0; y<(dimY-1); y+=stepY){
			for (x=0; x<(dimX-1); x+=stepX){
				if (!getMinMaxForCell(x,y,z, &maxV, &minV)){
					printf("Error: Out of bounds!\n");
					return;
				}
				if (maxV>=isovalue && minV<=isovalue){
					// the cell of interest... :D
					triangulateCell5(x,y,z, isovalue);
				}
			}
		}
	}

	// vertices, indices and normals MUST be filled in NOW!

	// initialization of buffers
	initBuffers();
}

/*	
*	Use 5 tetraherons to triangulate isosurface in cell
*	Fill vertices, indices and normals
*/
void CTdata::triangulateCell5(int x, int y, int z, float isovalue)
{
	const int * tetrahedraIds;
	// which type of tetrahedra split...
	if ((((x/stepX)%2)*2-1)*(((y/stepY)%2)*2-1)*(((z/stepZ)%2)*2-1)<0){
		// type A
		tetrahedraIds = tetrahedron5a;
	} else {
		// type B
		tetrahedraIds = tetrahedron5b;
	}
	// cell points
	Vertex* cellVertices[8];
	cellVertices[0] = & getVertexAt(x  , y  , z  );
	cellVertices[1] = & getVertexAt(x+1, y  , z  );
	cellVertices[2] = & getVertexAt(x+1, y+1, z  );
	cellVertices[3] = & getVertexAt(x  , y+1, z  );
	cellVertices[4] = & getVertexAt(x  , y  , z+1);
	cellVertices[5] = & getVertexAt(x  , y  , z+1);
	cellVertices[6] = & getVertexAt(x  , y+1, z+1);
	cellVertices[7] = & getVertexAt(x  , y+1, z+1);
	int i, j;

	// select tetrahedra vertices
	Vertex* tetrahedraVertices[4];
	for (i=0; i<5; i++){
		for (j=0; j<4; j++){
			tetrahedraVertices[j] = cellVertices[tetrahedraIds[i*4+j]];
		}
		// do some stuff with tetrahedraVertices...

	}

	
}

Vertex& CTdata::getVertexAt(int x, int y, int z)
{
	Position3i p;
	p.x = x;
	p.y = y;
	p.z = z;
	// is Vetrex allready precomputed in vertexMap?
	Vertex& v = vertexMap[p];
	if (!v.isValid){
		// not precomputed... compute now...
		printf("vertex not precomputed...\n");

		// position
		v.position = v3(x*scX, y*scY, z*scZ);

		// normal
		float nx = getValueAt2(x-1, y, z) - getValueAt2(x+1, y, z);
		float ny = getValueAt2(x, y-1, z) - getValueAt2(x, y+1, z);
		float nz = getValueAt2(x, y, z-1) - getValueAt2(x, y, z+1);
		v.normal	= v3(nx, ny, nz);

		// value
		v.value	= getValueAt(x,y,z);

		// validate
		v.isValid = true;
	} else {
		printf("Vertex PRECOMP!!\n");
	}

	return v;

}

bool CTdata::getMinMaxForCell(int x, int y, int z, float *maxV, float *minV)
{
	if ((x+1)>=dimX || (y+1)>=dimY || (z+1)>=dimZ){
		// bad index...
		return false;
	}
	*minV = FLT_MAX;
	*maxV = -FLT_MAX;
	int i,j,k;
	for (k=0; k<2; k++){
		for (j=0; j<2; j++){
			for (i=0; i<2; i++){
				*minV = min(*minV, getValueAt(x+i, y+j, z+k));
				*maxV = max(*maxV, getValueAt(x+i, y+j, z+k));
			}
		}
	}
	return true;

}

void CTdata::initBuffers(){
	// create buffers
	numIndices			= indices.size();
	numVertices			= vertices.size();
	pElementBufferData	= new GLuint	[ numIndices  ];
	pVertexBufferData	= new GLfloat	[ numVertices ];
	pNormalBufferData	= new GLfloat	[ numVertices ];
	int i;
	for (i=0; i<numIndices; i++){
		pElementBufferData[i]	= indices[i];
	}
	for (i=0; i<numVertices; i++){
		pVertexBufferData[i]	= vertices[i];
		pNormalBufferData[i]	= normals[i];
	}
	// element buffer
	glGenBuffers(1,&elementBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID); // vytvoreni indexoveho bufferu
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(GLubyte), pElementBufferData, GL_STATIC_DRAW); // prazdna inicializace
	glGenBuffers(1,&vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, 2*numVertices*3*sizeof(GLfloat), NULL, GL_STATIC_DRAW); // prazdna inicializace
		// do prvni pulky pole pozice
		glBufferSubData(GL_ARRAY_BUFFER,0, numVertices*3*sizeof(GLfloat), pVertexBufferData);
		// do druhe pulky normaly
		glBufferSubData(GL_ARRAY_BUFFER,numVertices*3*sizeof(GLfloat), numVertices*3*sizeof(GLfloat), pNormalBufferData);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	
}

void CTdata::draw3dIsosurface()
{
	glColor(color.r, color.g, color.b);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID); 
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0)); // posledni je BUFFER_OFFSET ve VBO
			glNormalPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(numVertices*3*sizeof(GLfloat)));
			glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CTdata::destroy3dIsosurface()
{
	delete [] pVertexBufferData;
	delete [] pNormalBufferData;
	delete [] pElementBufferData;
	pVertexBufferData	= NULL;
	pNormalBufferData	= NULL;
	pElementBufferData	= NULL;
	numIndices			= 0;
	numVertices			= 0;
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteBuffers(1, &elementBufferID);
}

