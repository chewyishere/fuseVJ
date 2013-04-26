#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "ofMain.h"

class rectangle {
    
public:
	
    rectangle();
	
    void	draw(int colori);
    void	xenoToPoint(float catchX, float catchY);
    
    ofPoint		pos;
    ofPoint		prevPos;
    ofPoint     scl;
    
    float		angle;
	
	
    float		catchUpSpeed;	
	
	
	
};

#endif // RECTANGLE_H
