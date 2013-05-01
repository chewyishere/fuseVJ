#define PARTICLE_NUM 500

#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "particle.h"
#include "ofxControlPanel.h"

#include "ofxOsc.h"
#define PORT 12345
#define NUM_MSG_STRINGS 20

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
	
        //ctrl panel
		ofxControlPanel panel;
	
        //fbo
        ofFbo rgbaFboFloat;
        ofFbo rgbaFbo;
        int fadeAmnt;
		 
    //osc
    ofxOscReceiver receiver;
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];

    float Vbase, Vmelody, Edrum, Ebase, Emelody;
    
	
};

#endif
	
