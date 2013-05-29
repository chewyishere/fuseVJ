#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxiPhoneExternalDisplay.h"

#include "particle.h"
#include "timePoint.h"

#include "ofxOsc.h"
#define PORT 12345
#define NUM_MSG_STRINGS 20
#define PARTICLE_NUM 100



class testApp : public ofxiPhoneApp, public ofxiPhoneExternalDisplay{
	
    public:
        void setup();
        void update();
        void draw();
    
    
    void drawFboTest();
    
        void exit();
	
        void touchDown(ofTouchEventArgs & touch);
        void touchMoved(ofTouchEventArgs & touch);
        void touchUp(ofTouchEventArgs & touch);
        void touchDoubleTap(ofTouchEventArgs & touch);
        void touchCancelled(ofTouchEventArgs & touch);

        void lostFocus();
        void gotFocus();
        void gotMemoryWarning();
        void deviceOrientationChanged(int newOrientation);
    

    
    
    // particle
    vector <particle> funkyPtc;
    // flocking particle
    vector <particle *> flockPtc;
    
    
    
    //fbo
    ofFbo rgbaFboFloat;
    ofFbo rgbaFbo;
    int fadeAmnt;
    
    
    
    //audio
    void    audioIn(float * input, int bufferSize, int nChannels);
    bool	bRepel;
    float	radius;
    float	strength;
    float   userRadius;
    
    float   accelX, accelY, accelZ;
    
    vector <float> left;
    vector <float> right;
    vector <float> volHistory;
    
    int 	bufferCounter;
    int 	drawCounter;
    
    
    ofSoundStream soundStream;
    float smoothedVol;
    float scaledVol;
    
    
    
    //timepoints
    vector < timePoint > pts;
    float startTime;
    
    
    
    //mode selector
    int number;
    int numberBlend;
    
    //blendmode
    ofBlendMode blendMode;
    
    
    
    //osc
    ofxOscReceiver receiver;
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    float hue,sat;
    bool bDrawMouse;

    

};


