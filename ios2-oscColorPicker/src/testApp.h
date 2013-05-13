#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxOsc.h"
#include "ofxUI.h"

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
    
    vector<ofxUIToggle *> vFlockToggles, vDrawToggles, vBlendToggles;
    vector <int> getToggleMatrixValues(string received_name , ofxUIEventArgs &e);
    
    //ofxUI
    void setGUI();
    void setGUI1();
    void setGUI2();
    void setGUI3();
    void setGUI4();
//    void setGUI5();
    
    ofxUICanvas *gui;
    ofxUICanvas *gui1;
    ofxUICanvas *gui2;
    ofxUICanvas *gui3;
    ofxUICanvas *gui4;
//    ofxUICanvas *gui5;
	
    bool hideGUI;
	float red, green, blue;
	bool bdrawGrid;
	bool bdrawPadding;

	void guiEvent(ofxUIEventArgs &e);
    
    ofxUIMovingGraph *mg;
    float *buffer;
    

    
    //osc
    string ip;
    int port;
    ofxOscSender sender;
    bool bFlockMode[8], bDrawMode[8], bBlendMode[4];
    bool bDrawScreen, bClearScreen, bAllColor;
    float fadeBar, satBar;
    int cpHue, cpSat, cpBri;
    
    //color picker interface
    ofImage img;
    ofImage sketch;
    
    
    ofxiPhoneKeyboard * iosKeyboard;
    string inputModeName;
    ofPoint keyModeButton;
    float keyModeButtonRad;
    bool keyMode;
    
};

