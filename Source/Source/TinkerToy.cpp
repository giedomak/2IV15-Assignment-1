// TinkerToy.cpp : Defines the entry point for the console application.
//

#include "Particle.h"
#include "SpringForce.h"
#include "Gravity.h"
#include "RodConstraint.h"
#include "CircularWireConstraint.h"
#include "imageio.h"
#include "IForce.h"
#include "MouseForce.h"
#include "Force.h"
#include <iostream>
#include <fstream>

using namespace std;

#include <vector>
#include <algorithm> 
#include <stdlib.h>
#include <stdio.h>
#include <glut.h>

/* macros */

/* external definitions (from solver) */
extern void simulation_step(std::vector<Particle*> pVector, std::vector<IForce*> forces, std::vector<IForce*> gravforces, std::vector<IForce*> constraints, float dt);

/* global variables */

static int N;
static float dt, d;
static int dsim;
static int grav;
static int dump_frames;
static int frame_number;

// static Particle *pList;
static std::vector<Particle*> pVector;

static int win_id;
static int win_x, win_y;
static int mouse_down[3];
static int mouse_release[3];
static int mouse_shiftclick[3];
static int omx, omy, mx, my;
static int hmx, hmy;

Vec2f MousePos;
int particleSelected = -1;

static std::vector<MouseForce*> mouses;
static std::vector<IForce*> forces;
static std::vector<IForce*> gravforces;
static std::vector<IForce*> gravforcesEmpty;
static std::vector<IForce*> constraints;

static SpringForce * delete_this_dummy_spring = NULL;
static RodConstraint * delete_this_dummy_rod = NULL;
static CircularWireConstraint * delete_this_dummy_wire = NULL;


/*
----------------------------------------------------------------------
free/clear/allocate simulation data
----------------------------------------------------------------------
*/

static void free_data ( void )
{
	pVector.clear();
	if (delete_this_dummy_rod) {
		delete delete_this_dummy_rod;
		delete_this_dummy_rod = NULL;
	}
	if (delete_this_dummy_spring) {
		delete delete_this_dummy_spring;
		delete_this_dummy_spring = NULL;
	}
	if (delete_this_dummy_wire) {
		delete delete_this_dummy_wire;
		delete_this_dummy_wire = NULL;
	}
}

static void clear_data ( void )
{
	int ii, size = pVector.size();

	for(ii=0; ii<size; ii++){
		pVector[ii]->reset();
	}
}

static void init_system(void)
{
	const double dist = 0.2;
	const Vec2f center(0.0, 0.0);
	const Vec2f offset(dist, 0.0);


	//creating a random particle for the lulz
	const Vec2f random(dist * 2, 0.0);

	// Create three particles, attach them to each other, then add a
	// circular wire constraint to the first.

	pVector.push_back(new Particle(center + offset));
	pVector.push_back(new Particle(center + offset + offset + Vec2f(0.1, 0.0)));
	pVector.push_back(new Particle(center + offset + offset + offset));

	pVector.push_back(new Particle(center - 3 * offset ));

	// You shoud replace these with a vector generalized forces and one of
	// constraints...
	
	int i, size = pVector.size();
	for (i = 0; i<size; i++)
	{
		mouses.push_back(new MouseForce(pVector[i], pVector[i]->m_Velocity, 0.5, 0.5));
	}


	forces.push_back( new SpringForce(pVector[0], pVector[1], dist, 0.3, 0.3));

	// Apply gravity on all particles

	/*for (int p = 0; p < pVector.size(); p++) {
		forces.push_back( new Gravity(pVector[p]));
	}*/

	//apply gravity on the 2nd particle
	gravforces.push_back(new Gravity(pVector[1]));
	

	//constraints.push_back(new RodConstraint(pVector[0], pVector[1], dist));

	//delete_this_dummy_spring = new SpringForce(pVector[0], pVector[1], dist, 1.0, 1.0);
	//delete_this_dummy_rod = new RodConstraint(pVector[1], pVector[2], dist);
	delete_this_dummy_wire = new CircularWireConstraint(pVector[0], center, dist);
}

/*
----------------------------------------------------------------------
OpenGL specific drawing routines
----------------------------------------------------------------------
*/

static void pre_display ( void )
{
	glViewport ( 0, 0, win_x, win_y );
	glMatrixMode ( GL_PROJECTION );
	glLoadIdentity ();
	gluOrtho2D ( -1.0, 1.0, -1.0, 1.0 );
	glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear ( GL_COLOR_BUFFER_BIT );
}

static void post_display ( void )
{
	// Write frames if necessary.
	if (dump_frames) {
		const int FRAME_INTERVAL = 4;
		if ((frame_number % FRAME_INTERVAL) == 0) {
			const unsigned int w = glutGet(GLUT_WINDOW_WIDTH);
			const unsigned int h = glutGet(GLUT_WINDOW_HEIGHT);
			unsigned char * buffer = (unsigned char *) malloc(w * h * 4 * sizeof(unsigned char));
			if (!buffer)
				exit(-1);
			// glRasterPos2i(0, 0);
			glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
			static char filename[80];
			sprintf_s(filename, "snapshots/img%.5i.png", frame_number / FRAME_INTERVAL);
			printf("Dumped %s.\n", filename);
			saveImageRGBA(filename, buffer, w, h);

			free(buffer);
		}
	}
	frame_number++;

	glutSwapBuffers ();
}

static void draw_particles ( void )
{
	int size = pVector.size();

	for(int ii=0; ii< size; ii++)
	{
		pVector[ii]->draw();
	}
}

static void draw_forces(void)
{

	for_each(forces.begin(), forces.end(), [](IForce* f)
	{
		f->draw();
	});
		for_each(gravforces.begin(), gravforces.end(), [](IForce* f)
		{
			f->draw();
		});

	for_each(mouses.begin(), mouses.end(), [](MouseForce* m)
	{
		m->draw();
	});




}

static void draw_constraints ( void )
{
	// change this to iteration over full set
	if (delete_this_dummy_rod)
		delete_this_dummy_rod->draw();
	if (delete_this_dummy_wire)
		delete_this_dummy_wire->draw();
}

/*
----------------------------------------------------------------------
relates mouse movements to tinker toy construction
----------------------------------------------------------------------
*/

static void get_from_UI ()
{
	int i, j;
	// int size, flag;
	int hi, hj;
	// float x, y;
	if ( !mouse_down[0] && !mouse_down[2] && !mouse_release[0] 
	&& !mouse_shiftclick[0] && !mouse_shiftclick[2] ) return;

	i = (int)((       mx /(float)win_x)*N);
	j = (int)(((win_y-my)/(float)win_y)*N);

	if ( i<1 || i>N || j<1 || j>N ) return;

	if ( mouse_down[0] ) {

	}

	if ( mouse_down[2] ) {
	}

	hi = (int)((       hmx /(float)win_x)*N);
	hj = (int)(((win_y-hmy)/(float)win_y)*N);

	if( mouse_release[0] ) {
	}

	omx = mx;
	omy = my;
}

static void remap_GUI()
{
	int ii, size = pVector.size();
	for(ii=0; ii<size; ii++)
	{
		pVector[ii]->m_Position[0] = pVector[ii]->m_ConstructPos[0];
		pVector[ii]->m_Position[1] = pVector[ii]->m_ConstructPos[1];
	}
}

/*
----------------------------------------------------------------------
GLUT callback routines
----------------------------------------------------------------------
*/

static void key_func ( unsigned char key, int x, int y )
{
	switch ( key )
	{
	case 'c':
	case 'C':
		clear_data ();
		break;

	case 'd':
	case 'D':
		dump_frames = !dump_frames;
		break;

	case 'q':
	case 'g':
	case 'G':
		grav = !grav;
		cout << "turned gravity to " << (grav? "TRUE" : "FALSE") << "\n";
		break;
	case 'Q':
		free_data ();
		exit ( 0 );
		break;

	case ' ':
		dsim = !dsim;
		//put vectors on 0 to restart the simulation
		//for_each(pVector.begin(), pVector.end(), [](Particle* v) {
		//	v->m_Velocity = 0;

		//});
		break;
	}
}





static void mouse_func ( int button, int state, int x, int y )
{
	omx = mx = x;
	omx = my = y;

	if(!mouse_down[0]){hmx=x; hmy=y;}
	if(mouse_down[button]) mouse_release[button] = state == GLUT_UP;
	if(mouse_down[button]) mouse_shiftclick[button] = glutGetModifiers()==GLUT_ACTIVE_SHIFT;
	mouse_down[button] = state == GLUT_DOWN;
}

static void motion_func ( int x, int y )
{
	mx = x;
	my = y;
}

static void reshape_func ( int width, int height )
{
	glutSetWindow ( win_id );
	glutReshapeWindow ( width, height );

	win_x = width;
	win_y = height;
}

/*
----------------------------------------------------------------------
relates mouse movements to tinker toy construction
----------------------------------------------------------------------
*/

static void get_mouse_pos(void)
{
	//screen is -1 to 1 in both x and y pos
	//mouse pos is from 0 to 64
	float x = 0;
	float y = 0;

	int i, j;
	i = (int)((mx / (float)win_x)*N);
	j = (int)(((win_y - my) / (float)win_y)*N);

	if (!mouse_down[0] && !mouse_down[2] && !mouse_release[0]
		&& !mouse_shiftclick[0] && !mouse_shiftclick[2]) return;

	if (mouse_down[0])
	{

		x = i - 32;
		x = (float)(x / 32);

		y = j - 32;
		y = (float)(y / 32);

		int i, size = pVector.size();

		for (i = 0; i<size; i++)
		{

			MousePos[0] = x;
			MousePos[1] = y;

			float xDis = pVector[i]->m_Position[0] - MousePos[0];
			float yDis = pVector[i]->m_Position[1] - MousePos[1];

			float distance = xDis*xDis + yDis*yDis;

			if (distance < 0.001)
			{
				particleSelected = i;
			}

			//particle is selected
			if (particleSelected == i)
			{
				mouses[i]->getMouse(MousePos);
				mouses[i]->setForce(true);
				mouses[i]->apply();
			}
			else
			{
				mouses[i]->getMouse(pVector[i]->m_Position);
				mouses[i]->setForce(false);
			}

		}
	}
	else
	{
		particleSelected = -1;
		int i, size = pVector.size();

		for (i = 0; i < size; i++)
		{
			mouses[i]->getMouse(pVector[i]->m_Position);
			mouses[i]->setForce(false);
		}
	}
}

static void idle_func ( void )
{

	if (dsim) simulation_step(pVector, forces, (grav ? gravforces : gravforcesEmpty), constraints, dt);
	else        { get_from_UI(); remap_GUI(); }


	get_mouse_pos();
	glutSetWindow ( win_id );
	glutPostRedisplay ();
}

static void display_func ( void )
{
	pre_display ();

	draw_forces();
	draw_constraints();
	draw_particles();

	post_display ();
}




/*
----------------------------------------------------------------------
open_glut_window --- open a glut compatible window and set callbacks
----------------------------------------------------------------------
*/

static void open_glut_window ( void )
{
	glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE );

	glutInitWindowPosition ( 0, 0 );
	glutInitWindowSize ( win_x, win_y );
	win_id = glutCreateWindow ( "Tinkertoys!" );

	glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear ( GL_COLOR_BUFFER_BIT );
	glutSwapBuffers ();
	glClear ( GL_COLOR_BUFFER_BIT );
	glutSwapBuffers ();

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);

	pre_display ();

	glutKeyboardFunc ( key_func );
	glutMouseFunc ( mouse_func );
	glutMotionFunc ( motion_func );
	glutReshapeFunc ( reshape_func );
	glutIdleFunc ( idle_func );
	glutDisplayFunc ( display_func );
}


/*
----------------------------------------------------------------------
main --- main routine
----------------------------------------------------------------------
*/

int main ( int argc, char ** argv )
{
	glutInit ( &argc, argv );

	if ( argc == 1 ) {
		N = 64;
		dt = 0.1f;
		d = 5.f;
		grav = true;
		fprintf ( stderr, "Using defaults : N=%d dt=%g d=%g\n",
			N, dt, d );
	} else {
		N = atoi(argv[1]);
		dt = atof(argv[2]);
		d = atof(argv[3]);
	}

	printf ( "\n\nHow to use this application:\n\n" );
	printf ( "\t Toggle construction/simulation display with the spacebar key\n" );
	printf ( "\t Dump frames by pressing the 'd' key\n" );
	printf ( "\t Quit by pressing the 'q' key\n" );

	dsim = 0;
	dump_frames = 0;
	frame_number = 0;

	init_system();

	win_x = 512;
	win_y = 512;
	open_glut_window ();

	glutMainLoop ();

	exit ( 0 );
}

