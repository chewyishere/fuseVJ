#pragma once
#define RECT_NUM 250
#define PARTICLE_NUM 200

#include "ofMain.h"
#include "timePoint.h"
#include "particle.h"

#include "ofxOsc.h"
#define PORT 9000
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
		
    
    ofImage background;
//    ofTrueTypeFont  font;
    string  message;
    
    ofVec2f oldM;
    bool    bPaint, bObstacle, bBounding, bClear;
    
    int         width, height; // addon H/W
    
    vector <particle> particles; // particle
    vector <particle *> particles2;      // pointer particles'
    
    //fbo
    ofFbo rgbaFboFloat;
    ofFbo rgbaFbo;
    int fadeAmnt;
    
    //audio
    
    void audioIn(float * input, int bufferSize, int nChannels);
    
    
    
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
    
    vector < timePoint > pts;
    float startTime;
    
    
    particle myParticle;
    particle myParticle2;
    

    int number;
    string letter;

    float volumeSmooth;
    
    
    // let's make a vector of them
    ofColor col1;
    ofColor colPink;

    //osc
    ofxOscReceiver receiver;
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    ofBlendMode blendMode;
    
    float hue,sat;
    
};
