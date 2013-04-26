#define PARTICLE_NUM 500

#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "particle.h"
#include "ofxControlPanel.h"


class testApp : public ofSimpleApp{
	
	public:
		
		void setup();
		void update();
		void draw();
    
        void drawFboTest();
		
		void keyPressed  (int key);
		void keyReleased (int key);
		
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased();
		
		// let's make a vector of them
		vector <particle *> particles;
	
		ofxControlPanel panel;
	
        //32 bits red, 32 bits green, 32 bits blue, from 0 to 1 in 'infinite' steps
        ofFbo rgbaFboFloat; // with alpha
        ofFbo rgbaFbo;
    
        int fadeAmnt;
		 
	
};

#endif
	
