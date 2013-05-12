#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxOsc.h"
#include "ofxUI.h"

#define HOST "169.254.92.237"
#define PORT 12345

class testApp : public ofxiPhoneApp {

public:
    void setup();
    void update();
    void draw();
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

    void drawGrid(float x, float y);
    //grab toggleMatrix data
    vector<int> getToggleMatrixValues(string received_name , ofxUIEventArgs &e);
    
    //ofxUI
    void setGUI1();
    void setGUI2();
    void setGUI3();
    void setGUI4();
    
    ofxUICanvas *gui1;
    ofxUICanvas *gui2;
    ofxUICanvas *gui3;
    ofxUICanvas *gui4;
	
    bool hideGUI;
	float red, green, blue;
	bool bdrawGrid;
	bool bdrawPadding;

	void guiEvent(ofxUIEventArgs &e);
    
    ofxUIMovingGraph *mg;
    float *buffer;
    

    
    //osc
    ofxOscSender sender;
    int cFlockMode, cDrawMode, cBlendMode;
    bool bDrawScreen, bClearScreen, bAllColor;
    float fadeBar, satBar;
    int cpHue, cpSat, cpBri;
    
    //color picker interface
    ofImage img;
    ofImage sketch;
};

