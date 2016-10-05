/*************************************************************/
/* Filename: CircularDrive.cpp                               */
/*                                                           */
/* This program animates a vehicle driving around a circular */
/* track, with the user controlling the speed of the vehicle */
/* (between low and high limits), the lane of the track in   */
/* which the vehicle is being driven (left or right), and    */
/* the perspective at which the scene is viewed (from the    */
/* driver's seat, from the infield, or from the outfield).   */
/*************************************************************/

#include <gl/freeglut.h>
#include <iostream>		// For diagnostic I/O              //
#include <cmath>		// Contains math functions         //
#include <ctime>		// Accesses system time info       //
#include <stdlib.h>		// Enables random number generator //
#include "Font.h"		// Font generation routines        //
#include "DriveGlobals.h"
#include "Track.h"
using namespace std;

#define HISTORY_BUFFER_SIZE 10
#define NUM_VERTICIES 100
#define TRACK_THICKNESS .1
#define ptA (&point-2)
#define ptB (&point-1)
#define ptC (&point)
//////////////////////
// Global variables //
//////////////////////

// The initial window and viewport sizes (in pixels). //
// When the window is resized, the viewport will be   //
// adjusted to preserve the aspect ratio.             //
GLint currWindowSize[2] = { 900, 600 };
GLint currViewportSize[2] = { 800, 500 };
int MULT = 1;
// The driver's position within the scene. //
GLfloat time_hour;
GLfloat time_increment_hour;
GLfloat lookAtAngleDelta;
GLfloat distanceTraveled;
GLfloat laneOffset;
SOR     sideOfRoad;

// The camera's current viewpoint. //
VIEW cameraViewpoint;

// Flags indicating whether vehicle is changing lanes. //
bool movingLeft = false;
bool movingRight = false;

// Coordinates of scene components that will be rendered. //
GLfloat treeBaseRadius[NUMBER_TREES];
GLfloat treeHeight[NUMBER_TREES];
GLfloat treePosition[NUMBER_TREES][3];

// Fonts for use in the display panel. //
GLFONT *TextFont;
GLFONT *SmallTextFont;
GLFONT *MediumTextFont;
GLFONT *LargeTextFont;
Track* track = NULL;

/***********************/
/* Function prototypes */
/***********************/
void KeyboardPress(unsigned char pressedKey, int mouseXPosition, int mouseYPosition);
void NonASCIIKeyboardPress(int pressedKey, int mouseXPosition, int mouseYPosition);
void TimerFunction(int value);
void InitializeScene();
void InitializeTrees();
bool TreeCollision(int index);
void Display();
void DrawTrack();
void DrawGround();
void DrawTrees();
void DrawVehicle();
void DrawDisplayPanel();
void InitializeTrack();
void ResizeWindow(GLsizei w, GLsizei h);
float GenerateRandomNumber(float lowerBound, float upperBound);
double xCoord(double t);
double yCoord(double t);
double zCoord(double t);
void InitializeTrack() {
	track = new Track(xCoord, yCoord, zCoord, -PI_OVER_2, 3 * PI_OVER_2);

	track->generateVerticies(NUM_VERTICIES, ROAD_WIDTH, TRACK_THICKNESS);
	
}

// The main function sets up the data and the
// environment to display the textured objects.
void main(int argc, char **argv)
{
	// Set up the display window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(INIT_WINDOW_POSITION[0], INIT_WINDOW_POSITION[1]);
	glutInitWindowSize(currWindowSize[0], currWindowSize[1]);
	glutCreateWindow("DRIVING IN CIRCLES (D=Driver's View; I=Infield View; O=Outfield View)");

	// Specify the resizing and refreshing routines.
	glutReshapeFunc(ResizeWindow);
	glutKeyboardFunc(KeyboardPress);
	glutSpecialFunc(NonASCIIKeyboardPress);
	glutDisplayFunc(Display);
	glutTimerFunc(REFRESH_RATE, TimerFunction, 1);

	// Set up standard lighting, shading, and depth testing.
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glClearColor(CONTROL_PANEL_COLOR[0], CONTROL_PANEL_COLOR[1],
		CONTROL_PANEL_COLOR[2], CONTROL_PANEL_COLOR[3]);
	glViewport(0, 0, currWindowSize[0], currWindowSize[1]);

	// Enable alpha test for transparency.
	glAlphaFunc(GL_GREATER, 0.5);
	glEnable(GL_ALPHA_TEST);

	// Set up the scene.
	InitializeScene();
	InitializeTrack();
	// Set up all fonts, initializing to medium size.
	SmallTextFont = FontCreate(wglGetCurrentDC(), "Arial", 10, 100, 1);
	MediumTextFont = FontCreate(wglGetCurrentDC(), "Arial", 14, 600, 1);
	LargeTextFont = FontCreate(wglGetCurrentDC(), "Arial", 20, 900, 1);
	TextFont = MediumTextFont;

	glutMainLoop();
}

// Function to react to user-presed keyboard
// keys by changing the camera perspective.
void KeyboardPress(unsigned char pressedKey, int mouseXPosition, int mouseYPosition)
{
	switch (pressedKey)
	{
	case 'D': case 'd': { cameraViewpoint = DRIVER;   break; }
	case 'I': case 'i': { cameraViewpoint = INFIELD;  break; }
	case 'O': case 'o': { cameraViewpoint = OUTFIELD;MULT = -MULT; break; }
	}
}

// Function to react to user-pressed non-ASCII keyboard keys by
// accelerating/decelerating the vehicle or by changing lanes.
void NonASCIIKeyboardPress(int pressedKey, int mouseXPosition, int mouseYPosition)
{
	switch (pressedKey)
	{
		// Up arrow: Accelerate viewer if possible.
	case GLUT_KEY_UP: {
		time_increment_hour *= USER_ANGLE_ACCELERATION_FACTOR;
		if (time_increment_hour > MAX_USER_ANGLE_INCREMENT)
			time_increment_hour = MAX_USER_ANGLE_INCREMENT;
		break;
	}
					  // Down arrow: Decelerate viewer if possible.
	case GLUT_KEY_DOWN: {
		time_increment_hour /= USER_ANGLE_ACCELERATION_FACTOR;
		if (time_increment_hour < MIN_USER_ANGLE_INCREMENT)
			time_increment_hour = MIN_USER_ANGLE_INCREMENT;
		break;
	}
						// Left arrow: Switch user to left lane.
	case GLUT_KEY_LEFT: {
		movingLeft = true;
		movingRight = false;
		sideOfRoad = TRANSITION;
		break;
	}
						// Right arrow: Switch user to right lane.
	case GLUT_KEY_RIGHT: {
		movingLeft = false;
		movingRight = true;
		sideOfRoad = TRANSITION;
		break;
	}
	}
}

// Function to update the vehicle position around the track,
// and, if appropriate, the lane position of the vehicle.
void TimerFunction(int value)
{
	time_hour += time_increment_hour;
	if (movingRight)
	{
		laneOffset += LANE_CHANGE_INCREMENT;
		if (laneOffset >= RIGHT_LANE_OFFSET)
		{
			laneOffset = RIGHT_LANE_OFFSET;
			movingRight = false;
			sideOfRoad = RHS;
		}
	}
	else if (movingLeft)
	{
		laneOffset -= LANE_CHANGE_INCREMENT;
		if (laneOffset <= LEFT_LANE_OFFSET)
		{
			laneOffset = LEFT_LANE_OFFSET;
			movingLeft = false;
			sideOfRoad = LHS;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(100, TimerFunction, 1);
}

// Initialize the user's position to be along
// the circular track, looking slightly ahead.
void InitializeScene()
{
	InitializeTrees();
	time_hour = INITIAL_USER_ANGLE;
	time_increment_hour = INITIAL_USER_ANGLE_INCREMENT;
	lookAtAngleDelta = INITIAL_LOOK_AT_ANGLE_DELTA;
	distanceTraveled = INITIAL_DISTANCE_TRAVELED;
	laneOffset = INITIAL_LANE_OFFSET;
	sideOfRoad = INITIAL_SIDE_OF_ROAD;
	cameraViewpoint = INITIAL_CAMERA_VIEWPOINT;
}

// Set up the conical trees to be randomly positioned in the non-track
// portions of the scene, and so they're not excessively short or tall.
void InitializeTrees()
{
	GLfloat angle, centerDistance;
	int i;

	for (i = 0; i < NUMBER_TREES; i++)
	{
		treeBaseRadius[i] = GenerateRandomNumber(MINIMUM_TREE_BASE_RADIUS, MAXIMUM_TREE_BASE_RADIUS);
		angle = i * 4 * PI / NUMBER_TREES;

		// First half of trees are inside circular track; second half are outside.
		// In neither case is the tree allowed to overlap the circular road itself.
		if (i <= NUMBER_TREES / 2)
			centerDistance = GenerateRandomNumber(MINIMUM_TREE_CENTER_DISTANCE,
				ROAD_RADIUS - 0.5f * ROAD_WIDTH - 2 * treeBaseRadius[i]);
		else
			centerDistance = GenerateRandomNumber(ROAD_RADIUS + 0.5f * ROAD_WIDTH + 2 * treeBaseRadius[i],
				MAXIMUM_TREE_CENTER_DISTANCE);

		treePosition[i][0] = centerDistance * cos(angle);
		treePosition[i][1] = GROUND_BOTTOM;
		treePosition[i][2] = centerDistance * sin(angle);

		treeHeight[i] = GenerateRandomNumber(MINIMUM_TREE_HEIGHT_TO_RADIUS_RATIO * treeBaseRadius[i],
			MAXIMUM_TREE_HEIGHT_TO_RADIUS_RATIO * treeBaseRadius[i]);
		if (treeHeight[i] > MAX_TREE_HEIGHT)
			treeHeight[i] = MAX_TREE_HEIGHT;

		// Redo tree i if it overlaps any of the previous trees.
		if (TreeCollision(i))
			i--;
	}
}

// Determine whether the new tree (at index) overlaps any of the previous trees.
bool TreeCollision(int index)
{
	int i;
	for (i = 0; i < index; i++)
		if (pow(treePosition[i][0] - treePosition[index][0], 2) + pow(treePosition[i][2] - treePosition[index][2], 2)
			<= pow(treeBaseRadius[i] + treeBaseRadius[index], 2))
			return true;
	return false;
}

// Principal display routine: sets up material, lighting, and camera 
// properties, clears the frame buffer, and renders all objects.
void Display()
{
	GLfloat vehiclePosition[] = { (laneOffset + ROAD_RADIUS) * sin(time_hour),
		DRIVER_LEVEL, (laneOffset + ROAD_RADIUS) * cos(time_hour) };
	GLfloat driverLookAtPosition[] = { (laneOffset + ROAD_RADIUS) * sin(time_hour + lookAtAngleDelta),
		DRIVER_LOOK_LEVEL, (laneOffset + ROAD_RADIUS) * cos(time_hour + lookAtAngleDelta) };

	// Set up the properties of the light source.
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LIGHT_INTENSITY);
	glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_POSITION);

	// Limit the animation to the portion of the window above the "control panel".
	if (ASPECT_RATIO > currWindowSize[0] / currWindowSize[1])
	{
		glViewport(0, int(0.5f * (currWindowSize[1] - currViewportSize[1]) + currViewportSize[1] * PANEL_TO_WINDOW_HEIGHT_RATIO),
			currViewportSize[0], currViewportSize[1]);
		glScissor(0, int(0.5f * (currWindowSize[1] - currViewportSize[1]) + currViewportSize[1] * PANEL_TO_WINDOW_HEIGHT_RATIO),
			currViewportSize[0], currViewportSize[1]);
	}
	else
	{
		glViewport(int(0.5f * (currWindowSize[0] - currViewportSize[0])), int(currViewportSize[1] * PANEL_TO_WINDOW_HEIGHT_RATIO),
			currViewportSize[0], currViewportSize[1]);
		glScissor(int(0.5f * (currWindowSize[0] - currViewportSize[0])), int(currViewportSize[1] * PANEL_TO_WINDOW_HEIGHT_RATIO),
			currViewportSize[0], currViewportSize[1]);
	}
	glEnable(GL_SCISSOR_TEST);

	// Set up the properties of the viewing camera.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(VIEWING_ANGLE, ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);

	glDisable(GL_SCISSOR_TEST);

	// Position the camera and draw the 3D scene.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();

	switch (cameraViewpoint)
	{
	case DRIVER: {
		gluLookAt(vehiclePosition[0], vehiclePosition[1], vehiclePosition[2],
			driverLookAtPosition[0], driverLookAtPosition[1], driverLookAtPosition[2],
			VEHICLE_UP_VECTOR[0], VEHICLE_UP_VECTOR[1], VEHICLE_UP_VECTOR[2]);
		break;
	}
	case INFIELD: {
		gluLookAt(INFIELD_CAMERA_POSITION[0], INFIELD_CAMERA_POSITION[1], INFIELD_CAMERA_POSITION[2],
			vehiclePosition[0], vehiclePosition[1], vehiclePosition[2],
			INFIELD_CAMERA_UP_VECTOR[0], INFIELD_CAMERA_UP_VECTOR[1], INFIELD_CAMERA_UP_VECTOR[2]);
		break;
	}
	case OUTFIELD: {
		gluLookAt(OUTFIELD_CAMERA_POSITION[0], MULT*OUTFIELD_CAMERA_POSITION[1], OUTFIELD_CAMERA_POSITION[2],
			TRACK_CENTER[0], TRACK_CENTER[1], TRACK_CENTER[2],
			OUTFIELD_CAMERA_UP_VECTOR[0], OUTFIELD_CAMERA_UP_VECTOR[1], OUTFIELD_CAMERA_UP_VECTOR[2]);
		break;
	}
	}

	// Draw the track and its surroundings.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//DrawGround();
	DrawTrack();
	//DrawTrees();
	if (cameraViewpoint != DRIVER)
		DrawVehicle();

	glPopMatrix();

	// Expand the viewport so the display panel can be drawn.
	glDisable(GL_LIGHTING);
	glViewport(0, 0, currWindowSize[0], currWindowSize[1]);
	DrawDisplayPanel();

	// Exchange old and new display buffers (i.e., animate).
	glutSwapBuffers();
	glFlush();
}
// Draw the track, the guardrails, and the lap marker.
void DrawTrack()
{
	int i;
	GLfloat roadMatAmbient[4], roadMatDiffuse[4], roadMatSpecular[4], roadMatEmission[4];
	GLfloat roadMatShininess[] = { ROAD_SHININESS };
	GLfloat railMatAmbient[4], railMatDiffuse[4], railMatSpecular[4], railMatEmission[4];
	GLfloat railMatShininess[] = { RAIL_SHININESS };
	GLfloat markerMatAmbient[4], markerMatDiffuse[4], markerMatSpecular[4], markerMatEmission[4];
	GLfloat markerMatShininess[] = { MARKER_SHININESS };
	GLUquadricObj *qObj;
	qObj = gluNewQuadric();
	
	
	//track generation
	/*glBegin(GL_TRIANGLE_STRIP);
	GLfloatPoint point;
	GLfloat dt = track->length() / NUM_VERTICIES;
	int state = 0;
	int QUARTER = NUM_VERTICIES / 4;
	int HALF = NUM_VERTICIES / 2;
	int THREE_QUARTER = 3 * QUARTER;
	//the +1 completes the shape
	glColor3f(1.0, 0.0, 0.0);
	for (int i = 0; i <= HALF+1;i++) {
		point = verticies[(i+THREE_QUARTER)%NUM_VERTICIES];
		glVertex3f(point.x, point.y, point.z);
	}
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(0.0,1.0, 0.0);
	for (int i = 0; i <=HALF+1;i++) {
		point = verticies[(i+QUARTER)%NUM_VERTICIES];
		glVertex3f(point.x, point.y, point.z);
	}
	glEnd();
	
	*/
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1.0, 0, 0);
	
	glNormal3f(0, 1, 0);
	GLfloatPoint point;
	for (int i = 2;i < NUM_VERTICIES;i++) {
		point = verticies[i];
		glVertex3f(ptC->x, ptC->y, ptC->z);
	}
	glEnd();
	gluQuadricDrawStyle(qObj, GLU_FILL);
	glColor3f(1.0, 1.0, 1.0);
	
	//glDisable(GL_NORMALIZE);
	
	
	
	// Set up the material properties of the objects.
	for (i = 0; i < 4; i++)
	{
		roadMatAmbient[i] = roadMatDiffuse[i] = roadMatSpecular[i] = roadMatEmission[i] = ROAD_COLOR[i];
		railMatAmbient[i] = railMatDiffuse[i] = railMatSpecular[i] = railMatEmission[i] = RAIL_COLOR[i];
		markerMatAmbient[i] = markerMatDiffuse[i] = markerMatSpecular[i] = markerMatEmission[i] = MARKER_COLOR[i];
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Render the road.
	glPushMatrix();
	glTranslatef(0.0f, ROAD_BOTTOM, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, roadMatAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, roadMatDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, roadMatSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, roadMatEmission);
	glMaterialfv(GL_FRONT, GL_SHININESS, roadMatShininess);
	
	
	glPopMatrix();

	// Render the guardrails.
	for (int i = 0; i < NBR_ROAD_INTERVALS; i++)
	{
		glPushMatrix();
		glRotatef(360.0f * i / NBR_ROAD_INTERVALS, 0.0f, 1.0f, 0.0f);
		glTranslatef(ROAD_RADIUS, 0.0f, 0.0f);

		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, railMatAmbient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, railMatDiffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, railMatSpecular);
		glMaterialfv(GL_FRONT, GL_EMISSION, railMatEmission);
		glMaterialfv(GL_FRONT, GL_SHININESS, railMatShininess);
		glTranslatef(ROAD_WIDTH / 2 + ROADSIDE_MARGIN, 0.0f, 0.0f);
		glScalef(GUARDRAIL_SCALE_FACTOR[0], GUARDRAIL_SCALE_FACTOR[1], GUARDRAIL_SCALE_FACTOR[2]);
		glutSolidCube(1.0f);
		glPopMatrix();

		// Draw the lap marker.
		if (i == 0)
		{
			glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT, markerMatAmbient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, markerMatDiffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, markerMatSpecular);
			glMaterialfv(GL_FRONT, GL_EMISSION, markerMatEmission);
			glMaterialfv(GL_FRONT, GL_SHININESS, markerMatShininess);
			glTranslatef(-ROAD_WIDTH / 2 - ROADSIDE_MARGIN, 0.0f, 0.0f);
			glScalef(LAP_MARKER_SCALE_FACTOR[0], LAP_MARKER_SCALE_FACTOR[1], LAP_MARKER_SCALE_FACTOR[2]);
			glutSolidCube(1.0f);
			glPopMatrix();
		}
		glPopMatrix();
	}
	
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);


}

// Draw the grassy area beneath the track and the trees.
void DrawGround()
{
	int i;
	GLfloat groundMatAmbient[4], groundMatDiffuse[4], groundMatSpecular[4];
	GLfloat groundMatEmission[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat groundMatShininess[] = { GRASS_SHININESS };
	GLUquadricObj *qObj;
	qObj = gluNewQuadric();
	gluQuadricDrawStyle(qObj, GLU_FILL);

	for (i = 0; i < 4; i++)
		groundMatAmbient[i] = groundMatDiffuse[i] = groundMatSpecular[i] = GRASS_COLOR[i];

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPushMatrix();
	glTranslatef(0.0f, GROUND_BOTTOM, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, groundMatAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, groundMatDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, groundMatSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, groundMatEmission);
	glMaterialfv(GL_FRONT, GL_SHININESS, groundMatShininess);
	gluDisk(qObj, 0.0f, GROUND_RADIUS, 16, 16);
	glPopMatrix();
	
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
}

// Draw the trees that are inside and outside of the circular track.
void DrawTrees()
{
	int i;
	GLfloat treeMatAmbient[4], treeMatDiffuse[4], treeMatSpecular[4];
	GLfloat treeMatEmission[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat treeMatShininess[] = { TREE_SHININESS };
	GLUquadricObj *qObj;
	qObj = gluNewQuadric();
	gluQuadricDrawStyle(qObj, GLU_FILL);

	for (i = 0; i < 4; i++)
		treeMatAmbient[i] = treeMatDiffuse[i] = treeMatSpecular[i] = TREE_COLOR[i];

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, treeMatAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, treeMatDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, treeMatSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, treeMatEmission);
	glMaterialfv(GL_FRONT, GL_SHININESS, treeMatShininess);
	for (i = 0; i < NUMBER_TREES; i++)
	{
		glPushMatrix();
		glTranslatef(treePosition[i][0], treePosition[i][1], treePosition[i][2]);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		gluCylinder(qObj, treeBaseRadius[i], 0.0f, treeHeight[i], 36, 6);
		glPopMatrix();
	}
	glPopMatrix();

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
}

// Draw the "vehicle" as a scaled sphere with flattened spherical tires.
void DrawVehicle()
{
	int i;
	GLfloat vehicleMatAmbient[4], vehicleMatDiffuse[4], vehicleMatSpecular[4];
	GLfloat vehicleMatEmission[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat vehicleMatShininess[] = { VEHICLE_SHININESS };
	GLfloat tireMatAmbient[4], tireMatDiffuse[4], tireMatSpecular[4];
	GLfloat tireMatEmission[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat tireMatShininess[] = { TIRE_SHININESS };
	GLUquadricObj *qObj;
	qObj = gluNewQuadric();
	gluQuadricDrawStyle(qObj, GLU_FILL);

	for (i = 0; i < 4; i++)
	{
		vehicleMatAmbient[i] = vehicleMatDiffuse[i] = vehicleMatSpecular[i] = VEHICLE_COLOR[i];
		tireMatAmbient[i] = tireMatDiffuse[i] = tireMatSpecular[i] = TIRE_COLOR[i];
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPushMatrix();
	glRotatef(time_hour * DEGREES_PER_RADIAN, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0, VEHICLE_ELEVATION, laneOffset + ROAD_RADIUS);
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, vehicleMatAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, vehicleMatDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, vehicleMatSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, vehicleMatEmission);
	glMaterialfv(GL_FRONT, GL_SHININESS, vehicleMatShininess);
	glScalef(VEHICLE_SCALE_FACTOR[0], VEHICLE_SCALE_FACTOR[1], VEHICLE_SCALE_FACTOR[2]);
	gluSphere(qObj, 1.0, 20, 20);
	glPopMatrix();
	for (i = 0; i < 4; i++)
	{
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, tireMatAmbient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, tireMatDiffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, tireMatSpecular);
		glMaterialfv(GL_FRONT, GL_EMISSION, tireMatEmission);
		glMaterialfv(GL_FRONT, GL_SHININESS, tireMatShininess);
		glTranslatef(TIRE_OFFSET[i][0], TIRE_OFFSET[i][1], TIRE_OFFSET[i][2]);
		glScalef(TIRE_RADIUS, TIRE_RADIUS, TIRE_DEPTH);
		gluSphere(qObj, 1.0, 20, 20);
		glPopMatrix();
	}
	glPopMatrix();

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
}

// Draw the 2-D display panel in the bottom portion of the display
// window, showing relevant data about the course being traversed.
void DrawDisplayPanel()
{
	// Update the vehicle's velocity (in MPH)
	GLfloat currentSpeed = TRACK_LENGTH_IN_MILES * MILLISECONDS_PER_HOUR * time_increment_hour / (2 * PI * REFRESH_RATE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, (float)currWindowSize[0], 0.0f, (float)currWindowSize[1], -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);

	// Customize the font size within the display panel, according to
	// the current window size, using a large font whenever it would
	// be reasonably spaced and a small font whenever space is scarce.
	if ((currWindowSize[0] < 400) || (currWindowSize[1] < 250))
		TextFont = SmallTextFont;
	else if ((currWindowSize[0] < 550) || (currWindowSize[1] < 450))
		TextFont = MediumTextFont;
	else
		TextFont = LargeTextFont;

	// Draw vertical line to separate the displayed output into columns.
	glColor3f(LINE_COLOR[0], LINE_COLOR[1], LINE_COLOR[2]);
	glBegin(GL_LINES);

	glVertex2i(int(0.5 * currWindowSize[0]), int(currWindowSize[1] * PANEL_TO_WINDOW_HEIGHT_RATIO));
	glVertex2i(int(0.5 * currWindowSize[0]), 0);
	glEnd();

	// Update the distance traveled.
	distanceTraveled += TRACK_LENGTH_IN_MILES * (time_increment_hour / (2 * PI));

	glPushMatrix();

	// Output current travel readouts, starting with the
	// distance traveled and the vehicle's current speed.
	glColor3f(LEFT_LETTER_COLOR[0], LEFT_LETTER_COLOR[1], LEFT_LETTER_COLOR[2]);
	glRasterPos2i(currWindowSize[0] / 4, currWindowSize[1] / 8);
	FontPrintf(TextFont, 0, "Distance = %.2f miles", distanceTraveled);
	glRasterPos2i(currWindowSize[0] / 4, currWindowSize[1] / 16);
	FontPrintf(TextFont, 0, "Speed = %.2f MPH", currentSpeed);

	// Output current position readouts, starting with the vehicle's current lane.
	glColor3f(RIGHT_LETTER_COLOR[0], RIGHT_LETTER_COLOR[1], RIGHT_LETTER_COLOR[2]);
	char laneString[7];
	switch (sideOfRoad)
	{
	case LHS: { strcpy_s(laneString, "Left");   break; }
	case RHS: { strcpy_s(laneString, "Right");  break; }
	case TRANSITION: { strcpy_s(laneString, "Moving"); break; }
	}
	glRasterPos2i(3 * currWindowSize[0] / 4, currWindowSize[1] / 8);
	FontPrintf(TextFont, 0, "Current Lane: %6s", laneString);

	glPopMatrix();
}

// Window-reshaping callback, adjusting the viewport to be as large
// as possible within the window, without changing its aspect ratio.
void ResizeWindow(GLsizei w,
	GLsizei h)
{
	currWindowSize[0] = w;
	currWindowSize[1] = h;
	if (ASPECT_RATIO > w / h)
	{
		currViewportSize[0] = w;
		currViewportSize[1] = int(w / ASPECT_RATIO);
	}
	else
	{
		currViewportSize[1] = h;
		currViewportSize[0] = int(h * ASPECT_RATIO);
	}

	// Center the image within the resized window.
	glViewport(int(0.5*(w - currWindowSize[0])), 0, currWindowSize[0], currWindowSize[1]);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(VIEWING_ANGLE, (GLfloat)w / (GLfloat)h, NEAR_PLANE, FAR_PLANE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Generate a random floating-point value between the two parameterized values.
float GenerateRandomNumber(float lowerBound, float upperBound)
{
	static bool firstTime = true;
	time_t randomNumberSeed;
	if (firstTime)
	{
		time(&randomNumberSeed);
		srand((unsigned int)randomNumberSeed);
		firstTime = false;
	}
	return (lowerBound + ((upperBound - lowerBound) * (float(rand()) / RAND_MAX)));
}

//functions for the track

double xHist[HISTORY_BUFFER_SIZE][2];
int xIdx = 0;
double yHist[HISTORY_BUFFER_SIZE][2];
int yIdx = 0;
double zHist[HISTORY_BUFFER_SIZE][2];
int zIdx = 0;
double history(double hist[HISTORY_BUFFER_SIZE][2], int idx,double t, TrackCoord alt) {
	for (int i = 0;i < HISTORY_BUFFER_SIZE;i++) {
		if (hist[(idx - i+HISTORY_BUFFER_SIZE)%HISTORY_BUFFER_SIZE][0] == t) {
			return hist[idx - i][1];
		}
	}
	return alt(t);
}
double getX(double t) {
	return history(xHist, xIdx, t, xCoord);
}
double getY(double t) {
	return history(yHist, xIdx, t, yCoord);
}
double getZ(double t) {
	return history(zHist, xIdx, t, zCoord);
}
double xCoord(double t) {
	
	return cos(t);
}


double zCoord(double t) {
	return cos(t) * sin(t);
}
double yCoord(double t) {
	return .1;
}