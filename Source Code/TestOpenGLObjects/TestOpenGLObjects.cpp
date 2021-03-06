// TestOpenGLObjects.cpp : Defines the entry point for the console application.
//

// This is a demonstration program, showing how the OpenGLObjects system works.
// It opens a simple OpenGLWindow and renders a few objects in a visual scence.
#include "stdafx.h"

#include "../OpenGLObjects/OpenGLUseful.h"
#include "../OpenGLObjects/OpenGLColors.h"
#include "../OpenGLObjects/OpenGLWindows.h"
#include "../OpenGLObjects/OpenGLObjects.h"
#include "../OpenGLObjects/OpenGLViewpoints.h"
#include "../OpenGLObjects/OpenGLTextures.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////

// Initial parameters of the window and the simulated objects.
// Some of these can be changed by commandline arguments (see below).
// They are only used at start up to determine the default values.

// Parameters of the window.
int width = 640;
int height = 480;
bool border = true;
bool stereo = false;
bool fullscreen = false;

// Place the viewer at the origin, looking in the zero orientation.
double initial_viewpoint_position[3] = { 0.0, 0.0, 0.0 };
double initial_viewpoint_orientation[3][3] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };

// In the OpenGL coordinate system, zero orientation means that one is looking
//  down the negative Z axis with positive Y up and positive X to the right. Here we $
//  place the object where we can see it.
double initial_object_position[3] = { 0.0, 0.0, - 1500.0 };
double initial_object_orientation[3][3] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };

// Dimensions of the room.
double room_width = 1800.0;
double room_height = 2000.0;
double room_length = 8000.0;
double wall_thickness = 10.0;

// A texture that is used to decorate the walls of the room.
// This file has to be in the execution directory.
char *wall_texture_bitmap = "lime.bmp";

///////////////////////////////////////////////////////////////////////////////////////////////////////

// These hold the OpenGLObjects that are shared across the local routines.
OpenGLWindow	*window;			// The window on the screen.
Viewpoint		*viewpoint;			// The viewpoint into the virtual world.
Assembly		*room;				// A walled room in the virtual world.
Texture			*wall_texture;		// The texture that is applied to the walls.
Assembly		*object;			// A 3D object that moves within the virtual room.

///////////////////////////////////////////////////////////////////////////////////////////////////////

// Routines to initialize the GL graphics engine at the beginning of the program 
//  and prior to the rendering of each scene. I wonder if there is a way to integrate these
//  better into the OpenGLObjects system?

void initialize_gl ( void ) {

	// Default lighting and material.
	glDefaultLighting();
	glDefaultMaterial();

	// Shade model
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

}

void prepare_gl_for_rendering ( void ) {

	// Get ready to do 3D rendering with GL.
	// This should be intergrated more cleanly into the OpenGLObjects system.
	// Perhaps it should be a method of OpenGLWindow.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}
///////////////////////////////////////////////////////////////////////////////////////////////////////

void create_objects( void ) {

	// Local objects used to construct the OpenGLObject assemblies.
	Sphere *sphere;
	Slab *slab;
	Box *box;
	Cylinder *cylinder;
	Disk *disk;

	// Create a room to put the object in.

	// The wall texture is 256 pixels wide by 512 high.
	// We map this onto a patch that is 2 meters wide by 4 meter high in the virtual scene.
	wall_texture = new Texture( wall_texture_bitmap, 2000, 4000 );
	room = new Assembly();
	room->SetColor( WHITE );

	box = new Box( room_height, room_width, room_length );
	box->SetColor( GRAY );
	box->SetTexture( wall_texture );
	room->AddComponent( box );

	// A disk on the front wall, something to look at.
	disk = new Disk( 750.0 );
	disk->SetPosition( 0.0, 0.0, - room_length / 2.0 + wall_thickness );
	disk->SetColor( ORANGE );
	room->AddComponent( disk );

	// The center of the room is at the origin.
	room->SetPosition( 0.0, 0.0, 0.0 );
	room->SetOrientation( 0.0, 0.0, 0.0 );

	// Create a viewpoint into the virtual room.
   /* 
	* Define a viewing projection with:
	*  45� vertical field-of-view - horizontal fov will be determined by window aspect ratio.
	*  60 mm inter-pupilary distance - the units don't matter to OpenGL, but all the dimensions
	*      that I give for the model room here are in mm.
	*  100.0 to 10000.0  depth clipping planes - making this smaller would improve the depth resolution.
	*/
	viewpoint = new Viewpoint( 6.0, 45.0, 10.0, 10000.0);

	// Initial viewpoint is at the center of the room in the null orientation.
	viewpoint->SetPosition( 0.0, 0.0, 0.0 );
	viewpoint->SetOrientation( 0.0, 0.0, 0.0 );

	// Create a simple object to look at.
	object = new Assembly();

	slab = new Slab( 150.0, 100.0, 10.0 );
	slab->SetPosition( 0.0, 0.0, 10.0 );
	slab->SetColor( BLUE );
	object->AddComponent( slab );

	slab = new Slab( 150.0, 100.0, 10.0 );
	slab->SetPosition( 0.0, 0.0, - 10.0 );
	slab->SetColor( GREEN );
	object->AddComponent( slab );

	sphere = new Sphere( 40.0 );
	sphere->SetColor( RED );
	object->AddComponent( sphere );

	cylinder = new Cylinder( 10.0, 30.0, 60.0 );
	cylinder->SetPosition( 0.0, 0.0, 0.0 );
	cylinder->SetColor( YELLOW );
	object->AddComponent( cylinder );

	cylinder = new Cylinder( 10.0, 10.0, 80.0 );
	cylinder->SetPosition( 0.0, 0.0, 0.0 );
	cylinder->SetOrientation( 0.0, - 90.0, 0.0 );
	cylinder->SetColor( GREEN );
	object->AddComponent( cylinder );

	// Initialize the position of the mobile object.
	object->SetPosition( initial_object_position);
	object->SetOrientation( initial_object_orientation );

}

void render( void ) {

	// Draw the virtual room.
	room->Draw();

	// Draw the complex object at its current position. 
	object->Draw();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	int frame_counter;

	// Parse the command line arguments.
	int arg;
	for ( arg = 1; arg < argc; arg++ ) {

		// These options determine the size and form of the window.
		if ( !strcmp( argv[arg], "-full" ) ) {
			width = 640;
			height = 480;
			border = false;
			fullscreen = true;
			stereo = false;
		}
		else if ( !strcmp( argv[arg], "-hmd" ) ) {
			width = 1024;
			height = 384;
			border = false;
			fullscreen = true;
			stereo = true;
			//       HMDScreen( HMD_STEREO );
		}
		else if ( !strcmp( argv[arg], "-svga" ) ) {
			width = 2048;
			height = 768;
			border = false;
			fullscreen = true;
			stereo = true;
			//      HMDScreen( HMD_STEREO );
		}
		else if ( !strcmp( argv[arg], "-nVisL" ) ) {
			fprintf( stderr, "LowRes nVis\n" );
			width = 2048;
			height = 768;
			border = false;
			fullscreen = true;
			stereo = true;
			//      HMDScreen( HMD_STEREO );
		}
		else if ( !strcmp( argv[arg], "-nVis" ) ) {
			width = 2560;
			height = 1024;
			border = false;
			fullscreen = true;
			stereo = true;
			//      HMDScreen( HMD_STEREO );
		}
		else if ( !strcmp( argv[arg], "-noborder" ) ) border = FALSE;
	}

	// Create and instance of a window object.
	// This does not yet create the actual window on the screen.
	window = new OpenGLWindow();

	// Set border and fullscreen mode depending on arguments above.
	window->Border = border; 
	window->FullScreen = fullscreen;

	// Create sets the new window to be the active window.
	if ( !window->Create( NULL, argv[0], 0, 0, width, height ) ) {
		fMessageBox( MB_OK, "TestOpenGLObjects", "Error creating window." );
		exit( -1 );
	}  
	window->Activate();

	// Initialize the state of the GL graphics engine.
	initialize_gl();

	// Create the room and the moving object.
	create_objects();

	frame_counter = 0;
	for ( double angle = 0.0; true; angle += 5.0 ) {

		object->SetOrientation( angle, object->jVector );
		object->SetPosition( initial_object_position );

		window->Clear();

		prepare_gl_for_rendering();

		if ( stereo ) {
			viewpoint->Apply( window, LEFT_EYE );
			render();

			viewpoint->Apply( window, RIGHT_EYE );
			render();
		}
		else {
			viewpoint->Apply( window, CYCLOPS );
			render();
		}
		window->Swap();
		if ( ! window->RunOnce() ) break;

		frame_counter++;

	}

	window->Destroy();
	fMessageBox( MB_OK, "TestOpenGLObjects", "Frame count: %d", frame_counter );
}

