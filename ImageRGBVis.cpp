/*
	Image visualizer
	Map RGB points to XYZ
	Amos Byon
	July 3, 2015
*/

#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <pic.h>
#include <string>

int g_iMenuId;

int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

/* enum to track which modifier for the mouse */
typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;

/* set default control state */
CONTROLSTATE g_ControlState = ROTATE;

/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.0, -100.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

Pic * g_ImageData;

int prev_ticks = 0;

/* renders the point cloud of RGB values */
void drawPointCloud(){
	//width and height of the picture
	int width = g_ImageData->nx;
	int height = g_ImageData->ny;

	//iterate through image and extract RGB values
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			float r = PIC_PIXEL(g_ImageData, i, j, 0);
			float g = PIC_PIXEL(g_ImageData, i, j, 1);
			float b = PIC_PIXEL(g_ImageData, i, j, 2);
			glColor3f(r/255.f, g/255.f, b/255.f);
			glVertex3f(r, g, b);
		}
	}
}

void myinit(){
	glEnable(GL_DEPTH_TEST);
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

void display(void){
	//clear the previous screen data
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(256.0, 256.0, 256.0,
			      128.0, 128.0, 128.0,
			      0.0, 1.0, 0.0);

	//apply mouse rotations
	glRotatef(g_vLandRotate[0], 1.0, 0.0, 0.0);
	glRotatef(g_vLandRotate[1], 0.0, 1.0, 0.0);
	glRotatef(g_vLandRotate[2], 0.0, 0.0, 1.0);

	//apply mouse translations
  	glTranslatef(g_vLandTranslate[0],
                 g_vLandTranslate[1],
                 g_vLandTranslate[2]);

  	//apply mouse scaling
  	glScalef(g_vLandScale[0],
             g_vLandScale[1],
           	 g_vLandScale[2]);

  	glBegin(GL_LINES);
  		glColor3f(1.0, 0.0, 0.0);
  		glVertex3f(0.0, 0.0, 0.0);
  		glVertex3f(256.0, 0.0, 0.0);

  		glColor3f(0.0, 1.0, 0.0);
  		glVertex3f(0.0, 0.0, 0.0);
  		glVertex3f(0.0, 256.0, 0.0);

  		glColor3f(0.0, 0.0, 1.0);
  		glVertex3f(0.0, 0.0, 0.0);
  		glVertex3f(0.0, 0.0, 256.0);
  	glEnd();

  	glBegin(GL_POINTS);
      glPointSize(100);
  		drawPointCloud();
  	glEnd();

  	glutSwapBuffers();
}

void menufunc(int value){
	switch(value){
		case 0:
			exit(0);
			break;
	}
}

void reshape(int width, int height){
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(65.0, (float)width / height, 0.01, 1000.0);

	glMatrixMode(GL_MODELVIEW);
}

void doIdle(){
	int t = glutGet(GLUT_ELAPSED_TIME);

	int deltaTicks = t - prev_ticks;

	prev_ticks = t;

  // rotate while idle
	g_vLandRotate[1] += (deltaTicks*0.05);
	glutPostRedisplay();
}

void mousedrag(int x, int y){
	int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};

	switch (g_ControlState)
  	{
    	case TRANSLATE:  
      		if (g_iLeftMouseButton)
      		{
        		g_vLandTranslate[0] += vMouseDelta[0];
        		g_vLandTranslate[1] -= vMouseDelta[1];
      		}
      		if (g_iMiddleMouseButton)
      		{
	        	g_vLandTranslate[2] += vMouseDelta[1];
      		}
      		break;
    	case ROTATE:
      		if (g_iLeftMouseButton)
      		{
        		g_vLandRotate[0] += vMouseDelta[1];
        		g_vLandRotate[1] += vMouseDelta[0];
      		}
      		if (g_iMiddleMouseButton)
     		{
				g_vLandRotate[2] += vMouseDelta[1];
  	    	}
    		break;
	    case SCALE:
    		if (g_iLeftMouseButton)
      		{
        		g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
      	  		g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
      		}
      		if (g_iMiddleMouseButton)
      		{
        		g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
      		}
      		break;
  }
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mouseidle(int x, int y){
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{

  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      g_iLeftMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_MIDDLE_BUTTON:
      g_iMiddleMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_RIGHT_BUTTON:
      g_iRightMouseButton = (state==GLUT_DOWN);
      break;
  }
 
  switch(glutGetModifiers())
  {
    case GLUT_ACTIVE_CTRL:
      g_ControlState = TRANSLATE;
      break;
    case GLUT_ACTIVE_SHIFT:
      g_ControlState = SCALE;
      break;
    default:
      g_ControlState = ROTATE;
      break;
  }

  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void keyboard(unsigned char c, int x, int y){
}

int main(int argc, char** argv){
	if(argc < 2){
		printf("usage: %s [IMAGE NAME]\n", argv[0]);
		exit(1);
	}

	g_ImageData = jpeg_read(argv[1], NULL);
	if(!g_ImageData){
		printf("Error reading %s.\n", argv[1]);
		exit(1);
	}
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Image RGB Point Cloud");

	glutDisplayFunc(display);

	glutReshapeFunc(reshape);

	g_iMenuId = glutCreateMenu(menufunc);
	glutSetMenu(g_iMenuId);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutIdleFunc(doIdle);

	glutMotionFunc(mousedrag);

	glutPassiveMotionFunc(mouseidle);

	glutMouseFunc(mousebutton);

	glutKeyboardFunc(keyboard);

	myinit();

	glutMainLoop();
	return(0);
}