#include "GL/freeglut.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include "vecmath.h"
#include <fstream>
using namespace std;
# define my_sizeof(type) ((char *)(&type+1)-(char*)(&type))

// Globals

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;


// You will need more global variables to implement color and position changes
int color_index = 0;
GLfloat light_pos[4] = { 1.0f, 1.0f, 5.0f, 1.0f };
int shape_index = 49;
int prev_shape = 49;
void loadInput();

// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f &a) 
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }


// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{	
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
		color_index++;
		if (color_index % 4 == 0) color_index = 0;
        break;
    default:
        //cout << "Unhandled key press " << key << "." << endl; 
		break;
    }

	//load shape
	if (key >= 49 && key <= 52) {
		prev_shape = shape_index;
		shape_index = key;
	}

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        // add code to change light position
		light_pos[1] += 0.5f;
		break;
    case GLUT_KEY_DOWN:
        // add code to change light position
		light_pos[1] -= 0.5f;
		break;
    case GLUT_KEY_LEFT:
        // add code to change light position
		light_pos[0] -= 0.5f;
		break;
    case GLUT_KEY_RIGHT:
        // add code to change light position
		light_pos[0] += 0.5f;
		break;
    }
	
	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}

// This function is responsible for displaying the object.
void drawScene(void)
{
    int i = color_index;

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
    gluLookAt(0.0, 0.0, 5.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    // Set material properties of object

	// Here are some colors you might use - feel free to add more
    GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
                                 {0.9, 0.5, 0.5, 1.0},
                                 {0.5, 0.9, 0.3, 1.0},
                                 {0.3, 0.8, 0.9, 1.0} };
    
	// Here we use the first color entry as the diffuse color
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[i]);

	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position
	GLfloat Lt0pos[4];
	for (int index = 0; index <4; index++) {
		Lt0pos[index] = light_pos[index];
	}

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	loadInput();
    
    // Dump the image to the screen.
    glutSwapBuffers();


}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

void readFile(std::string object_file) {
	//clear previous data
	vecv.clear();
	vecn.clear();
	vecf.clear();


	ifstream inFile;
	inFile.open(object_file);
	if (!inFile) {
		cerr << "Unable to open file datafile.txt";
		exit(1);   // call system to stop
	}

	string buffer;

	while (std::getline(inFile, buffer)) {
		stringstream ss(buffer);

		Vector3f v;
		string s;
		ss >> s;

		if (s == "v") {
			ss >> v[0] >> v[1] >> v[2];
			vecv.push_back(v);
		}
		else if (s == "vn") {
			ss >> v[0] >> v[1] >> v[2];
			vecn.push_back(v);
		}
		else if (s == "f") {
			int nb_faces = 3;
			vector<unsigned> face_data;

			std::string f1, f2, f3;
			ss >> f1 >> f2 >> f3;
			vector<std::string> f_array;
			f_array.push_back(f1);
			f_array.push_back(f2);
			f_array.push_back(f3);
			
			std::string token;
			//saving a,c,d,f,g,i
			for (int i = 0; i < nb_faces; i++) {
				stringstream st(f_array[i]);
				std::getline(st, token, '/');
				face_data.push_back(std::stoi(token));
				std::getline(st, token, '/');
				std::getline(st, token, '/');
				face_data.push_back(std::stoi(token));
			}
			vecf.push_back(face_data);
		}

	}
}

void loadInput()
{	
	std::string object_file;
	// load the OBJ file here
	switch (shape_index) {
	case 49:
		glutSolidTeapot(1.0);
		return;
	case 50:
		object_file = "sphere.obj";
		break;
	case 51:
		object_file = "garg.obj";
		break;
	case 52:
		object_file = "torus.obj";
		break;
	default:
		std::cout << "Shape not Specified" << std::endl;
		return;
	}

	if (shape_index != prev_shape) {
		readFile(object_file);
	}

	
	//draw mesh
	for (unsigned int index = 0; index < vecv.size(); index++) {
		int a = vecf[index][0];
		int c = vecf[index][1];
		int d = vecf[index][2];
		int f = vecf[index][3];
		int g = vecf[index][4];
		int i = vecf[index][5];

		glBegin(GL_TRIANGLES);
		glNormal3d(vecn[c - 1][0], vecn[c - 1][1], vecn[c - 1][2]);
		glVertex3d(vecv[a - 1][0], vecv[a - 1][1], vecv[a - 1][2]);
		glNormal3d(vecn[f - 1][0], vecn[f - 1][1], vecn[f - 1][2]);
		glVertex3d(vecv[d - 1][0], vecv[d - 1][1], vecv[d - 1][2]);
		glNormal3d(vecn[i - 1][0], vecn[i - 1][1], vecn[i - 1][2]);
		glVertex3d(vecv[g - 1][0], vecv[g - 1][1], vecv[g - 1][2]);
		glEnd();
	}

}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 1200, 800 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
