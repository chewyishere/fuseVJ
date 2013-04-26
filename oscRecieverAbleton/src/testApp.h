#define PARTICLE_NUM 1000

#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "particle.h"
#include "ofxControlPanel.h"
#include "ofxOsc.h"


#define HOST "localhost"
#define PORT 10001
#define NUM_MSG_STRINGS 20

class testApp : public ofSimpleApp{
	
	public:
		
		void setup();
		void update();
		void draw();
		
		void keyPressed  (int key);
		void keyReleased (int key);
		
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased();
		
		// let's make a vector of them
		vector <particle *> particles;
	
		ofxControlPanel panel;
        ofxControlPanel Abletonpanel;
	    ofxOscReceiver receiver;
    	//ofxOscSender sender;
		 
        ofTrueTypeFont font;
    
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    float Vmelody;
    float Emelody;
    float Vdrum;
    float Edrum;
    float Vbase;
    float Ebase;
    float FX1;
    float FX2;
    float FX3;


    
};

#endif
	
