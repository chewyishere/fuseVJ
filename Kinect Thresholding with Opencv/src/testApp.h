#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxAutoControlPanel.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);		
    
    
    
    ofxKinect kinect;
	ofxAutoControlPanel panel;
	ofImage result;




//////////cv/////////////
    
    ofVideoPlayer 		vidPlayer;
    ofVideoPlayer 		vidPlayer2;
    ofVideoPlayer 		vidPlayer3;
    
    ofxCvColorImage			colorImg;
    
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    
    ofxCvContourFinder 	contourFinder;
    
    int 				threshold;
    bool				bLearnBakground;
    
    
    ofImage a, b;
    ofImage mix;
    ofImage back, fore;


};
