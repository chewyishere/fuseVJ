#pragma once

#include "ofMain.h"
#include "timePoint.h"
#include "particle.h"
#include "ofxOsc.h"

#define NUM_PARTICLES 200
#define PORT 12345
#define NUM_MSG_STRINGS 20

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
    
        void drawFboTest();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
    
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    
    //shader
    ofFbo posFbo;
    ofImage renderImage;
    ofPoint pos[NUM_PARTICLES];
    ofPoint vec[NUM_PARTICLES];
    vector<ofShader*> shaders;
    int shaderIndex;
    
    //initializing particles
    vector <particle> particles;        // particle
    vector <particle *> particles2;     // pointer particles'

    //fbo
    ofFbo rgbaFboFloat;
    ofFbo rgbaFbo;
    int fadeAmnt;
    
    //audio
    void audioIn(float * input, int bufferSize, int nChannels);
    vector <float> left;
    vector <float> right;
    vector <float> volHistory;
    int 	bufferCounter;
    int 	drawCounter;
    ofSoundStream soundStream;
    float smoothedVol;
    float scaledVol;

    //local forces
    bool	bRepel;
    float	radius;
    float	strength;
	
    //timePoints
    vector < timePoint > pts;
    float startTime;
    
    //mode Selector
    int mode;
    
    //osc
    ofxOscReceiver receiver;
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    bool bTouchDown, bTouchUp;

    
};
