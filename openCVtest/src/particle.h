#ifndef PARTICLE_H
#define PARTICLE_H

#include "ofMain.h"


// C style 80s :)

typedef struct{
	int			count;
	ofVec2f		sum;
	float		distance;
	float		strength;
} flockingForce;
 

// C++ style 90s :)
/*
class flockingForce {
	public:
	
 ` r  	ofVec2f	sum;
	float		distance;
	float		strength;
	
};
*/



class particle{

	public:
        ofPoint pos;
        ofPoint vel;
        ofPoint frc;   // frc is also know as acceleration (newton says "f=ma")
        ofPoint prevPos;
			
        float age;
    
    
    
        particle();
		virtual ~particle(){};

        void resetForce();
		void addForce(float x, float y);
		void addRepulsionForce(float x, float y, float radius, float scale);
		void addAttractionForce(float x, float y, float radius, float scale);
		
		void addRepulsionForce(particle &p, float radius, float scale);
		void addAttractionForce(particle &p, float radius, float scale);
		void addClockwiseForce(particle &p, float radius, float scale);
		void addCounterClockwiseForce(particle &p, float radius, float scale);
	
		void addDampingForce();
 
		void addForFlocking(particle &p);
		void addFlockingForce();
	
		flockingForce cohesion;
		flockingForce seperation;
		flockingForce alignment;
	
	
		void setInitialCondition(float px, float py, float vx, float vy);
        void update();
        void draw();
	
		void bounceOffWalls();
	
		float damping;
        float angle;
        float size;
        float r,g,b;
        ofColor c;
    
    float scale;
    float r1, r2, r3;

    protected:
    private:
};

#endif // PARTICLE_H
