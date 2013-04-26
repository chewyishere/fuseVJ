#ifndef _TEST_APP
#define _TEST_APP
#define RECT_NUM 150  // how to change???

#include "ofMain.h"
#include "timePoint.h"
#include "particle.h"


class testApp : public ofBaseApp{
	
	public:
		
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    
    ofImage background;
    ofTrueTypeFont  font;
    string  message;

    
        ofVec2f oldM;
        bool    bPaint, bObstacle, bBounding, bClear;
    
        int         width, height; // addon H/W 
    
        vector <particle> particles; // particle 
    
		void audioIn(float * input, int bufferSize, int nChannels);
    
        
    
    bool	bRepel;
    float	radius;
    float	strength;
	
    ofImage spot;
    
    ofImage spotFeather;
    
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
    
    // music
    ofSoundPlayer piano;
    
    int number;
    
        
    float volumeSmooth;
    
    // let's make a vector of them
    ofColor col1;
    ofColor colPink;
    
};

#endif	

