//
//  feather.h
//  English Suite
//
//  Created by QIUYI on 12/12/12.
//
//

#ifndef __English_Suite__feather__
#define __English_Suite__feather__


#include "ofMain.h"

typedef struct{
	int			count;
	ofVec2f		sum;
	float		distance;
	float		strength;
    
    
} flockingForce;

class particle
{
public:
    ofVec2f pos;
    ofVec2f vel;
    ofVec2f frc;   // frc is also know as acceleration (newton says "f=ma")

    ofPoint  scl;  //size 

    ofImage * img;
		
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
    
    void setInitialCondition(float px, float py, float vx, float vy);
    void update();
    void draw();
	
    void bounceOffWalls();
	
	
    float damping;
    
    void addForFlocking(particle &p);
    void addFlockingForce();
   
    flockingForce cohesion;
    flockingForce seperation;
    flockingForce alignment;
    
    float		angle;
    
    ofPoint		pos2;
    ofPoint		prevPos;
	

    ofColor color;
    
protected:
private:
};


#endif /* defined(__English_Suite__feather__) */
