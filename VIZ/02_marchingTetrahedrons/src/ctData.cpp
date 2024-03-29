#include "ctData.h"

#define BUFFER_OFFSET(i) ((char*) NULL + (i))

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
	//vertexMap.clear();
	destroy3dIsosurface();
}

v3 CTdata::getCenterPoint(){
	return v3(scX*float(dimX)*0.5, scY*float(dimY)*0.5,scZ*float(dimZ)*0.5);
}
void	CTdata::loadSphere(int _szX, int _szY, int _szZ, float minV, float maxV)
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

	// backspace...
	BACKSPACE(chars);
	printf("LOADING CT images (%i) DONE\n", cnt);
	return true;
}

void CTdata::create3dIsosurface(float isovalue, int _stepX, int _stepY, int _stepZ)
{
	destroy3dIsosurface();
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
	//printf("Triangles: %i\n", vertices.size()/9);


	// initialization of buffers
	initBuffers();
}

Vertex CTdata::interpolate (Vertex v1, Vertex v2, float isovalue) {
	Vertex v;
	float t = (v1.value-isovalue)/(v1.value-v2.value);
	v.position = (v1.position*(1-t) + v2.position*t);
	v.normal = (v1.normal*(1-t) + v2.normal*t);
	//v.normal.normalize();
	v.value = isovalue;
	return v;
}

void CTdata::push(Vertex v1, Vertex v2, Vertex v3) {
	vertices.push_back(v1.position.x);
	vertices.push_back(v1.position.y);
	vertices.push_back(v1.position.z);
	vertices.push_back(v2.position.x);
	vertices.push_back(v2.position.y);
	vertices.push_back(v2.position.z);
	vertices.push_back(v3.position.x);
	vertices.push_back(v3.position.y);
	vertices.push_back(v3.position.z);

	normals.push_back(v1.normal.x);
	normals.push_back(v1.normal.y);
	normals.push_back(v1.normal.z);
	normals.push_back(v2.normal.x);
	normals.push_back(v2.normal.y);
	normals.push_back(v2.normal.z);
	normals.push_back(v3.normal.x);
	normals.push_back(v3.normal.y);
	normals.push_back(v3.normal.z);
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
	cellVertices[5] = & getVertexAt(x+1, y  , z+1);
	cellVertices[6] = & getVertexAt(x+1, y+1, z+1);
	cellVertices[7] = & getVertexAt(x  , y+1, z+1);
	int i, j;
	char type;
	
	// select tetrahedra vertices
	Vertex* tetrahedraVertices[4];
	for (i=0; i<5; i++){
		type = 0;
		for (j=0; j<4; j++){
			tetrahedraVertices[j] = cellVertices[tetrahedraIds[i*4+j]];
			if (tetrahedraVertices[j]->value>=isovalue){
				type+=powersOf2[j];
			}
		}
		
		// do some funny stuff with tetrahedraVertices...
		/*

		    _-0-_
		  _-  |  -_
		 3 - -|- - 1
		  \   |   /
		   \  |  /
		    \ | /
			 \|/
			  2
README:
	- dvojice p��pad� se li�� jen opa�nou orientac� roviny. Proto se
	  pro n�kter� nastavuje flipFlag na true - ty se mus� oto�it obr�cen�
    - je t�eba produkovat troj�heln�ky a to i pro p��pad, 
	  kdy se m� vytvo�it 4 �heln�k (tedy rozd�lit na dva troj�heln�ky)
	TODO : Takto produkovan� struktura by m�la redundantn� body. 
	       V�t�ina bod� je sd�len� v�ce troj�heln�ky. Lze s v�hodou vyu��t ELEMENT_DRAW.
		   proto je NUTN� p�ed�lat ulo�i�t� bod� na n�co jako MAP a nep�id�vat tam tedy bod,
		   kter� m� stejn� sou�adnice!!! 

		*/
		Vertex v1, v2, v3, v4;

		bool flipFlag = true;
		switch (type){
			case 0:
			case 15:
				// no intersection
				break;

			case 1:
				flipFlag = false;
			case 14:
				// intersection with 3 edges from pt 0
				v1 = interpolate(*tetrahedraVertices[0], *tetrahedraVertices[1], isovalue);
				v2 = interpolate(*tetrahedraVertices[0], *tetrahedraVertices[2], isovalue);
				v3 = interpolate(*tetrahedraVertices[0], *tetrahedraVertices[3], isovalue);
				if (flipFlag) {
					push(v3, v2, v1);
				} else {
					push(v1, v2, v3);
				}
				break;
			case 2:
				flipFlag = false;
			case 13:
				// intersection with 3 edges from pt 1
				v1 = interpolate(*tetrahedraVertices[1], *tetrahedraVertices[0], isovalue);
				v2 = interpolate(*tetrahedraVertices[1], *tetrahedraVertices[2], isovalue);
				v3 = interpolate(*tetrahedraVertices[1], *tetrahedraVertices[3], isovalue);
				if (flipFlag) {
					push(v1, v2, v3);
				} else {
					push(v1, v3, v2);
				}
				break;
			case 4:
				flipFlag = false;
			case 11:
				// intersection with 3 edges from pt 2
				v1 = interpolate(*tetrahedraVertices[2], *tetrahedraVertices[0], isovalue);
				v2 = interpolate(*tetrahedraVertices[2], *tetrahedraVertices[1], isovalue);
				v3 = interpolate(*tetrahedraVertices[2], *tetrahedraVertices[3], isovalue);
				if (flipFlag) {
					push(v1, v3, v2);
				} else {
					push(v1, v2, v3);
				}
				break;
			case 8:
				flipFlag = false;
			case 7:
				// intersection with 3 edges from pt 3
				v1 = interpolate(*tetrahedraVertices[3], *tetrahedraVertices[0], isovalue);
				v2 = interpolate(*tetrahedraVertices[3], *tetrahedraVertices[1], isovalue);
				v3 = interpolate(*tetrahedraVertices[3], *tetrahedraVertices[2], isovalue);
				if (flipFlag) {
					push(v1, v2, v3);
				} else {
					push(v1, v3, v2);
				}
				break;
			case 3:
				flipFlag = false;
			case 12:
				// intersection with 4 edges
				v1 = interpolate(*tetrahedraVertices[0], *tetrahedraVertices[2], isovalue);
				v2 = interpolate(*tetrahedraVertices[0], *tetrahedraVertices[3], isovalue);
				v3 = interpolate(*tetrahedraVertices[1], *tetrahedraVertices[3], isovalue);
				v4 = interpolate(*tetrahedraVertices[1], *tetrahedraVertices[2], isovalue);
				if (flipFlag) {
					push(v1, v3, v2);
					push(v1, v4, v3);					
				} else {
					push(v1, v2, v3);
					push(v1, v3, v4);					
				}
				break;

			case 5:
				flipFlag = false;
			case 10:
				// intersection with 4 edges
				v1 = interpolate(*tetrahedraVertices[0], *tetrahedraVertices[1], isovalue);
				v2 = interpolate(*tetrahedraVertices[0], *tetrahedraVertices[3], isovalue);
				v3 = interpolate(*tetrahedraVertices[2], *tetrahedraVertices[3], isovalue);
				v4 = interpolate(*tetrahedraVertices[2], *tetrahedraVertices[1], isovalue);
				if (flipFlag) {
					push(v1, v2, v3);
					push(v1, v3, v4);				
				} else {
					push(v1, v3, v2);
					push(v1, v4, v3);	
				}
				break;

			case 6:
				flipFlag = false;
			case 9:
				// intersection with 4 edges
				v1 = interpolate(*tetrahedraVertices[1], *tetrahedraVertices[0], isovalue);
				v2 = interpolate(*tetrahedraVertices[1], *tetrahedraVertices[3], isovalue);
				v3 = interpolate(*tetrahedraVertices[2], *tetrahedraVertices[3], isovalue);
				v4 = interpolate(*tetrahedraVertices[2], *tetrahedraVertices[0], isovalue);
				if (flipFlag) {
					push(v2, v4, v3);
					push(v4, v2, v1);
				} else {
					push(v4, v2, v3);
					push(v2, v4, v1);												
				}
				break;
		}
	} // next tetrahedra...

	
}
void CTdata::drawTetrahedrons(int id){
	if (id<0 || id>=tetrahedrons.size())
	{
		for (int i=0; i<tetrahedrons.size(); i++){
			tetrahedrons[i]->draw();
		}
		return;
	}
	tetrahedrons[id]->draw();

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
		//printf("vertex not precomputed...\n");

		// position
		v.position = v3(x*scX, y*scY, z*scZ)-center;
		//printf("POS[ %f , %f , %f ]\n", v.position.x, v.position.y, v.position.z);
		// normal
		int i;
		float nx, ny, nz;

		for (i=1; v.normal.length()<=100.0; i++){
			nx = getValueAt2(x-i, y, z) - getValueAt2(x+i, y, z);
			ny = getValueAt2(x, y-i, z) - getValueAt2(x, y+i, z);
			nz = getValueAt2(x, y, z-i) - getValueAt2(x, y, z+i);
			v.normal = v3(nx, ny, nz);
		}

		//v.normal.normalize();

		// value
		v.value	= getValueAt(x,y,z);

		// validate
		v.isValid = true;
	} else {
		//printf("Vertex PRECOMP!!\n");
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
	//numIndices			= indices.size();
	numVertices			= vertices.size();
	//pElementBufferData	= new GLuint	[ numIndices  ];
	pVertexBufferData	= new GLfloat	[ numVertices ];
	pNormalBufferData	= new GLfloat	[ numVertices ];
	int i;
	/*
	for (i=0; i<numIndices; i++){
		pElementBufferData[i]	= indices[i];
	}
	*/
	for (i=0; i<numVertices; i++){
		pVertexBufferData[i]	= vertices[i];
		pNormalBufferData[i]	= normals[i];
	}

	// element buffer
	/*
	glGenBuffers(1,&elementBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID); // vytvoreni indexoveho bufferu
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(GLubyte), pElementBufferData, GL_STATIC_DRAW); // prazdna inicializace
	*/
	glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, 2*numVertices*sizeof(GLfloat), NULL, GL_STATIC_DRAW); // prazdna inicializace
		// do prvni pulky pole pozice
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices*sizeof(GLfloat), pVertexBufferData);
		// do druhe pulky normaly
		glBufferSubData(GL_ARRAY_BUFFER, numVertices*sizeof(GLfloat), numVertices*sizeof(GLfloat), pNormalBufferData);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	
}

void CTdata::draw3dIsosurface()
{
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID); 
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0)); // posledni je BUFFER_OFFSET ve VBO
			glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(numVertices*sizeof(GLfloat)));
			glDrawArrays(GL_TRIANGLES, 0, numVertices/3);
			//glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CTdata::destroy3dIsosurface()
{
	delete [] pVertexBufferData;
	delete [] pNormalBufferData;
	//delete [] pElementBufferData;
	pVertexBufferData	= NULL;
	pNormalBufferData	= NULL;
	//pElementBufferData	= NULL;
	vertexMap.clear();
	numIndices			= 0;
	numVertices			= 0;
	glDeleteBuffers(1, &vertexBufferID);
	//glDeleteBuffers(1, &elementBufferID);
	vertices.clear();
	normals.clear();
}

