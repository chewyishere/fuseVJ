#pragma once
#define FISH_NUM 15

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxAutoControlPanel.h"
#include "fish.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
    void draw();
	void exit();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    void audioIn(float * input, int bufferSize, int nChannels);
	
    
    //sound part
    
    vector <float> left;
    vector <float> volHistory;
    int 	bufferCounter;
    int 	drawCounter;
    float smoothedVol;
    float scaledVol;
    float scaledVol1;
    ofSoundStream soundStream;
    
    
    
    int number;
    float radius;
    
    float xoffset;
    float yoffset;
    float x;
    float y;
    float orin;
    
    int threshold;
    float amount;
    
    
    
    float time[300];
    
	ofxKinect kinect;
	ofxAutoControlPanel panel;
    ofBlendMode blendMode;
    
    
    
    bool vidtoggle;
    int dottoggle, flashtoggle;
    int flash;
    
    
    ofImage image;
    ofImage result , testimage;
    ofxCvGrayscaleImage		resultAsCvImage;
    ofVideoPlayer 	vidPlayer, bigvidPlayer;;
    
    
    // fish
    fish myFish[FISH_NUM];
    
    float knock;
    int fishnumber;

};
