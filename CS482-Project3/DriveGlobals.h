//////////////////////////////////////////////////
// DriveGlobals.h - Global Constant Definitions //
//////// /////////////////////////////////////////

#ifndef DRIVE_GLOBALS_H

enum VIEW { DRIVER, INFIELD, OUTFIELD }; // Perspective of viewer        //
enum SOR { LHS, RHS, TRANSITION };      // Side-of-road enumerated type //

										/********************/
										/* Global constants */
										/********************/

										/* Location of upper left-hand corner of display window */
const GLint INIT_WINDOW_POSITION[2] = { 50, 50 };

/* Light source intensity and position */
const GLfloat LIGHT_INTENSITY[] = { 0.95f, 0.95f, 0.95f, 1.0f };
const GLfloat LIGHT_POSITION[] = { 2.0f, 5.0f, 2.0f, 0.0f };

/* Viewport-related constants */
const GLfloat ASPECT_RATIO = 1.6f;
const GLfloat VIEWING_ANGLE = 45.0f;
const GLfloat NEAR_PLANE = 0.01f;
const GLfloat FAR_PLANE = 300.0f;
const int REFRESH_RATE = 100;
const GLfloat MILLISECONDS_PER_HOUR = 3600000.0f;

/* Road-related constants */
const GLfloat ROAD_RADIUS = 30.0f;
const GLfloat ROAD_WIDTH = 2.0f;
const GLfloat ROADSIDE_MARGIN = 0.2f;
const GLfloat INNER_ROAD_RADIUS = ROAD_RADIUS - 0.5f * ROAD_WIDTH;
const GLfloat OUTER_ROAD_RADIUS = ROAD_RADIUS + 0.5f * ROAD_WIDTH;
const GLfloat ROAD_BOTTOM = 0.00f;
const GLfloat GUARDRAIL_SCALE_FACTOR[] = { 0.2f, 1.0f, 0.2f };
const GLfloat LAP_MARKER_SCALE_FACTOR[] = { 0.2f, 5.0f, 0.2f };
const GLfloat TRACK_LENGTH_IN_MILES = 0.25f;

/* Lane-shifting constants */
const GLfloat RIGHT_LANE_OFFSET = 0.5f;
const GLfloat LEFT_LANE_OFFSET = -0.5f;
const GLfloat LANE_CHANGE_INCREMENT = 0.1f;

/* Position information when viewing from inside vehicle. */
const GLfloat VEHICLE_UP_VECTOR[] = { 0.0f, 1.0f, 0.0f };
const GLfloat DRIVER_LEVEL = 2.0f;
const GLfloat DRIVER_LOOK_LEVEL = 1.0f;

/* Position information when viewing from the infield. */
const GLfloat INFIELD_CAMERA_POSITION[] = { 0.0f, 7.0f, 0.0f };
const GLfloat INFIELD_CAMERA_UP_VECTOR[] = { 0.0f, 1.0f, 0.0f };

/* Position information when viewing from the outfield. */
const GLfloat OUTFIELD_CAMERA_POSITION[] = { 0.0f, 15.0f, -60.0f };
const GLfloat TRACK_CENTER[] = { 0.0f, 0.0f, 0.0f };
const GLfloat OUTFIELD_CAMERA_UP_VECTOR[] = { 0.0f, 4.0f, 1.0f };

/* Pi-related constants. */
const GLfloat PI = 3.1415926535f;
const GLfloat DEGREES_PER_RADIAN = 180 / PI;
const GLfloat RADIANS_PER_DEGREE = PI / 180;

/* Constants associated with ground beneath the circular track. */
const GLfloat GROUND_RADIUS = 44.0f;
const GLfloat GROUND_BOTTOM = -0.02f;

/* Spacing constants for roadside guardrails. */
const int NBR_ROAD_INTERVALS = 90;

/* Tree-based constants. */
const GLfloat MAX_TREE_HEIGHT = 8.0f;
const int NUMBER_TREES = 200;
const GLfloat MINIMUM_TREE_BASE_RADIUS = 0.01f * GROUND_RADIUS;
const GLfloat MAXIMUM_TREE_BASE_RADIUS = 0.03f * GROUND_RADIUS;
const GLfloat MINIMUM_TREE_CENTER_DISTANCE = 0.2f * GROUND_RADIUS;
const GLfloat MAXIMUM_TREE_CENTER_DISTANCE = 0.95f * GROUND_RADIUS;
const GLfloat MINIMUM_TREE_HEIGHT_TO_RADIUS_RATIO = 2.0f;
const GLfloat MAXIMUM_TREE_HEIGHT_TO_RADIUS_RATIO = 6.0f;

/* Vehicle-related constants */
const GLfloat VEHICLE_SCALE_FACTOR[] = { 0.5f * ROAD_WIDTH, 0.3f * ROAD_WIDTH, 0.3f * ROAD_WIDTH };
const GLfloat VEHICLE_ELEVATION = 0.3f * ROAD_WIDTH;
const GLfloat TIRE_RADIUS = 0.2f * ROAD_WIDTH;
const GLfloat TIRE_DEPTH = 0.15f * ROAD_WIDTH;
const GLfloat TIRE_OFFSET[4][3] = { { 0.3f * ROAD_WIDTH, -0.1f * ROAD_WIDTH, 0.2f* ROAD_WIDTH },
{ 0.3f * ROAD_WIDTH, -0.1f * ROAD_WIDTH, -0.2f* ROAD_WIDTH },
{ -0.3f * ROAD_WIDTH, -0.1f * ROAD_WIDTH, 0.2f* ROAD_WIDTH },
{ -0.3f * ROAD_WIDTH, -0.1f * ROAD_WIDTH, -0.2f* ROAD_WIDTH } };

/* Constants associated with velocity around circular track. */
const GLfloat MAX_USER_ANGLE_INCREMENT = PI / 30;
const GLfloat MIN_USER_ANGLE_INCREMENT = PI / 180;
const GLfloat USER_ANGLE_ACCELERATION_FACTOR = 1.1f;
const GLfloat INITIAL_USER_ANGLE = 0.0f;
const GLfloat INITIAL_USER_ANGLE_INCREMENT = PI / 90;
const GLfloat INITIAL_LOOK_AT_ANGLE_DELTA = PI / 20;
const GLfloat INITIAL_DISTANCE_TRAVELED = 0.0f;
const GLfloat INITIAL_LANE_OFFSET = RIGHT_LANE_OFFSET;
const SOR     INITIAL_SIDE_OF_ROAD = RHS;
const VIEW    INITIAL_CAMERA_VIEWPOINT = DRIVER;

/* Scene color constants. */
const GLfloat ROAD_COLOR[] = { 0.2f, 0.2f, 0.2f, 1.0f };
const GLfloat RAIL_COLOR[] = { 0.15f, 0.1f,  0.0f,  1.0f };
const GLfloat MARKER_COLOR[] = { 0.6f,  0.5f,  0.1f,  1.0f };
const GLfloat GRASS_COLOR[] = { 0.1f,  0.4f,  0.15f, 1.0f };
const GLfloat TREE_COLOR[] = { 0.0f,  0.35f, 0.2f,  1.0f };
const GLfloat VEHICLE_COLOR[] = { 1.0f,  0.0f,  0.0f,  1.0f };
const GLfloat TIRE_COLOR[] = { 0.2f,  0.2f,  0.2f,  1.0f };

/* Scene object shininess constants. */
const GLfloat ROAD_SHININESS = 0.0f;
const GLfloat RAIL_SHININESS = 0.8f;
const GLfloat MARKER_SHININESS = 0.8f;
const GLfloat GRASS_SHININESS = 0.0f;
const GLfloat TREE_SHININESS = 0.8f;
const GLfloat VEHICLE_SHININESS = 0.8f;
const GLfloat TIRE_SHININESS = 0.8f;

/* Control panel color constants. */
const GLfloat CONTROL_PANEL_COLOR[] = { 0.0f, 0.0f, 0.5f, 0.0f };
const GLfloat LEFT_LETTER_COLOR[] = { 0.9f, 0.4f, 0.4f };
const GLfloat RIGHT_LETTER_COLOR[] = { 0.9f, 0.9f, 0.0f };
const GLfloat LINE_COLOR[] = { 0.4f, 0.9f, 0.9f };
const GLfloat PANEL_TO_WINDOW_HEIGHT_RATIO = 1.0f / 6;
const GLfloat UNITS_PER_MILE = .5;
const GLfloat T_PER_SECOND = 5;
#define DRIVE_GLOBALS_H
#endif