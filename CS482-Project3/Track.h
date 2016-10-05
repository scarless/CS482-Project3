#ifndef _H_TRACK_
#include <vector>


#define double_t double
#define int_t int
double_t PI_OVER_180 = 0.01745329251;

//90 degrees
double_t PI_OVER_2 = 1.57079632679;
//scale the track up in size
double_t TRACK_MULTIPLIER  = 13;

typedef double_t(*TrackCoord)(double_t);
struct GLfloatPoint {
	double_t x;
	double_t y;
	double_t z;
};
GLfloatPoint* verticies;

class Track {
	TrackCoord _x;
	TrackCoord _y;
	TrackCoord _z;
	double_t start, finish;


public:
	Track(TrackCoord xFunc, TrackCoord yFunc, TrackCoord zFunc, double start_t, double finish_t);
	~Track();
	double_t tangent(double_t t, double_t dt);
	double_t sine(double_t t, double_t dt);
	double_t cosine(double_t t, double_t dt);
	double_t normal(double_t t, double_t dt,bool left);
	GLfloatPoint get(double_t t);
	double_t length();
	void set(GLfloatPoint& data, double_t t);
	GLfloatPoint* generateVerticies(int numSegments, double track_width, double track_thickness);
};


Track::Track(TrackCoord xFunc, TrackCoord yFunc, TrackCoord zFunc, double start_t, double finish_t)
	: _x(xFunc), _y(yFunc), _z(zFunc), start(start_t), finish(finish_t) {

}
GLfloatPoint Track::get(double_t t) {
	GLfloatPoint data;
	if (t > length()) {
		int n = t / length();
		t -= n*length();
	}
	double_t tot = start + t;
	data.x = _x(tot);
	data.y = _y(tot);
	data.z = _z(tot);
	return data;
}
void Track::set(GLfloatPoint& data, double_t t) {
	
	double_t tot = start + t;
	data.x = _x(tot);
	data.y = _y(tot);
	data.z = _z(tot);

}


Track::~Track() {
	delete[] verticies;
}
//returns the t-length of this track
double_t Track::length() {
	return finish - start;
}


double_t Track::tangent(double_t t, double_t dt) {
	GLfloatPoint p0, p1;
	double_t tot = start + t - dt;
	//calculate the points
	p0.x = _x(tot);
	p0.z = _z(tot);
	tot = start + t + dt;
	p1.x = _x(tot);
	p1.z = _z(tot);

	double_t tang = atan((p1.z - p0.z) / (p1.x - p0.x));
	return tang;
}


double_t Track::cosine(double_t t, double_t dt) {
	GLfloatPoint p0, p1;
	double_t tot = start + t - dt;
	//calculate the points
	p0.x = _x(tot);
	p0.z = _z(tot);
	tot = start + t + dt;
	p1.x = _x(tot);
	p1.z = _z(tot);

	double_t cos = acos((p1.z - p0.z) / (p1.x - p0.x));
	return cos;
}

//calculate the sine at a current point of the track, not sure if it has a purpose..
double_t Track::sine(double_t t, double_t dt) {
	GLfloatPoint p0, p1;
	double_t tot = start + t - dt;
	//calculate the points
	p0.x = _x(tot);
	p0.z = _z(tot);
	tot = start + t + dt;
	p1.x = _x(tot);
	p1.z = _z(tot);

	double_t sin = asin((p1.z - p0.z) / (p1.x - p0.x));
	return sin;
}

//calculate the normal vector
double_t Track::normal(double_t t, double_t dt, bool left) {
	return tangent(t, dt) + (left ? -PI_OVER_2 : PI_OVER_2);
}


//generate the verticies for the track edges.
GLfloatPoint* Track::generateVerticies(int numVerticies, double track_width, double track_thickness) {
	verticies = new GLfloatPoint[numVerticies];
	const double_t dt = length() / numVerticies;
	bool dir = false;
	GLfloatPoint trackPoint;
	GLfloatPoint targetPoint;

	for (int i = 0; i < numVerticies;i++) {
		
		targetPoint = *new GLfloatPoint();
		set(trackPoint, dt*i);

		double_t norm = normal(dt*i, dt, dir);
		dir = !dir;
		//alternate sides

		targetPoint.x = TRACK_MULTIPLIER*trackPoint.x + track_width*cos(norm);
		targetPoint.z = TRACK_MULTIPLIER*trackPoint.z + track_width*sin(norm);
		targetPoint.y = TRACK_MULTIPLIER*trackPoint.y;
		verticies[i] = targetPoint;
		
	}
	return verticies;
}
#define _H_TRACK_
#endif


