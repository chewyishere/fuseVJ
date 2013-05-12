#pragma once

#include "ofMain.h"
#include "timePoint.h"
#include "particle.h"
#include "ofxOsc.h"

#define PORT 12345
#define NUM_MSG_STRINGS 20


class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
        void draw();
    
        void drawFboTest();
    
		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    
    vector <particle> dwgParti;            // particle
    vector <particle *> bgParti;         // pointer particles'
    
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
    
    //time points
    vector < timePoint > pts;
    float startTime;
    particle myParticle;
    particle myParticle2;
    
    //local interactions
    bool	bRepel;
    float	radius;
    float	strength;

    //modes
    int number;

    //osc
    ofxOscReceiver receiver;
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    ofBlendMode blendMode;
};
