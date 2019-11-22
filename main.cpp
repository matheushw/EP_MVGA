#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <list>
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <time.h>
//#include <functional>

#include "of.h"
#include "ofOffPointsReader.h"
#include "Handler.hpp" 
#include "GL_Interactor.h"
#include "ColorRGBA.hpp"
#include "Cores.h"
#include "Point.hpp"
#include "printof.hpp"


#include "CommandComponent.hpp"
#include "MyCommands.hpp"

#include "ofVertexStarIteratorSurfaceVertex.h"


// #include "scrInteractor.h"

clock_t start_insert;
clock_t end_insert;
clock_t start_print;
clock_t end_print;



using namespace std;
using namespace of;

//Define o tamanho da tela.
scrInteractor *Interactor = new scrInteractor(900, 650);

//Define a malha a ser usada.
typedef of::MyofDefault2D TTraits;
typedef of::ofMesh<TTraits> TMesh;
TMesh *malha;
Handler<TMesh> meshHandler;

typedef PrintOf<TTraits> TPrintOf;

TPrintOf *Print;

typedef MyCommands<TPrintOf> TMyCommands;
typedef CommandComponent TAllCommands;

ofVtkWriter<TTraits> writer;
TAllCommands *allCommands;

int type = 3;
double coords[3];
int atual;
int p;

void RenderScene(void){ 
	allCommands->Execute();
	Print->Vertices(malha,blue,3);
	Print->FacesWireframe(malha,grey,3);
	//////////////////////////////////////////
	double primeiro[2];
	double segundo[2];
	double terceiro[2];
	double d, dx, dy, dz;
	atual = 0; // mudar para mudar o triangulo inicial
	do {
		Print->Face(atual, red);
		// primeiro vertice
		primeiro[0] = malha->getVertex(malha->getCell(atual)->getVertexId(0))->getCoord(0);
		primeiro[1] = malha->getVertex(malha->getCell(atual)->getVertexId(0))->getCoord(1);

		// segundo vertice
		segundo[0] = malha->getVertex(malha->getCell(atual)->getVertexId(1))->getCoord(0);
		segundo[1] = malha->getVertex(malha->getCell(atual)->getVertexId(1))->getCoord(1);

		// terceiro vertice
		terceiro[0] = malha->getVertex(malha->getCell(atual)->getVertexId(2))->getCoord(0);
		terceiro[1] = malha->getVertex(malha->getCell(atual)->getVertexId(2))->getCoord(1);

		d = (segundo[0]*terceiro[1]) + (terceiro[0]*primeiro[1]) + (primeiro[0]*segundo[1]) - (primeiro[1]*segundo[0]) - (segundo[1]*terceiro[0]) - (terceiro[1]*primeiro[0]);

		dx = (segundo[0]*terceiro[1]) + (terceiro[0]*coords[1]) + (coords[0]*segundo[1]) - (coords[1]*segundo[0]) - (segundo[1]*terceiro[0]) - (terceiro[1]*coords[0]);

		dy = (coords[0]*terceiro[1]) + (terceiro[0]*primeiro[1]) + (primeiro[0]*coords[1]) - (primeiro[1]*coords[0]) - (coords[1]*terceiro[0]) - (terceiro[1]*primeiro[0]);

		dz =  (segundo[0]*coords[1]) + (coords[0]*primeiro[1]) + (primeiro[0]*segundo[1]) - (primeiro[1]*segundo[0]) - (segundo[1]*coords[0]) - (coords[1]*primeiro[0]);

		int x = dx/d;
		int y = dy/d;
		int z = dz/d;

		if (x>0 && y>0 && z>0) break;

		if (x<y && x<z) {
			atual = malha->getCell(atual)->getMateId(0);
			continue;
		}
		if(y<x && y<z) {
			atual = malha->getCell(atual)->getMateId(1);
			continue;
		}
		if(z<x && z<y) {
			atual = malha->getCell(atual)->getMateId(2);
			continue;
		}
		if (x==y || x==z) {
			atual = malha->getCell(atual)->getMateId(0);
			continue;
		}
		atual = malha->getCell(atual)->getMateId(1);

		if(atual == -1) break;

	} while(true);

	glFinish();
	glutSwapBuffers();
}

void HandleKeyboard(unsigned char key, int x, int y){	
	char *xs[10];
	allCommands->Keyboard(key);
	
	switch (key) {
		case 'e':
			exit(1);
		break;
		case 'v':
			coords[0]=x;
			coords[1]=y;
			coords[2]=0.0;
			malha->delVertex(p);
			p = malha->addVertex(coords);
			break;
		case 'd':
		break;
	}
	
	Interactor->Refresh_List();
	glutPostRedisplay();

}

using namespace std;

int main(int argc, char **argv)
{

  ofRuppert2D<MyofDefault2D> ruppert;
  ofPoints2DReader<MyofDefault2D> reader;
  ofVtkWriter<MyofDefault2D> writer;
  Interactor->setDraw(RenderScene);
	meshHandler.Set(new TMesh());
      char *fileBrasil = "./Brasil.off";

     
    reader.readOffFile(fileBrasil);
    
    ruppert.execute2D(reader.getLv(),reader.getLids(),true);
  
  meshHandler = ruppert.getMesh();
  malha = ruppert.getMesh();
  
  
  Print = new TPrintOf(meshHandler);

	allCommands = new TMyCommands(Print, Interactor);

	double a,x1,x2,y1,y2,z1,z2; 

	of::ofVerticesIterator<TTraits> iv(&meshHandler);

	iv.initialize();
	x1 = x2 = iv->getCoord(0);
	y1 = y2 = iv->getCoord(1);
	z1 = z2 = iv->getCoord(2);

	for(iv.initialize(); iv.notFinish(); ++iv){
		if(iv->getCoord(0) < x1) x1 = a = iv->getCoord(0);
		if(iv->getCoord(0) > x2) x2 = a = iv->getCoord(0);
		if(iv->getCoord(1) < y1) y1 = a = iv->getCoord(1);
		if(iv->getCoord(1) > y2) y2 = a = iv->getCoord(1);
		if(iv->getCoord(2) < z1) z1 = a = iv->getCoord(2);
		if(iv->getCoord(2) > z2) z2 = a = iv->getCoord(2);
	}

	double maxdim;
	maxdim = fabs(x2 - x1);
	if(maxdim < fabs(y2 - y1)) maxdim = fabs(y2 - y1);
	if(maxdim < fabs(z2 - z1)) maxdim = fabs(z2 - z1);

	maxdim *= 0.6;
	
	Point center((x1+x2)/2.0, (y1+y2)/2.0, (y1+y2)/2.0 );
	Interactor->Init(center[0]-maxdim, center[0]+maxdim,
					center[1]-maxdim, center[1]+maxdim,
					center[2]-maxdim, center[2]+maxdim,&argc,argv);

	
	
	AddKeyboard(HandleKeyboard);

	allCommands->Help(std::cout);
	std::cout<< std::endl<< "Press \"?\" key for help"<<std::endl<<std::endl;
	double t;
	
	Init_Interactor();

  
  return EXIT_SUCCESS;
}
