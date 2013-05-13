#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetOrientation(OF_ORIENTATION_90_LEFT);
    
	ofxAccelerometer.setup();
    sender.setup( HOST, PORT );
	ofSetFrameRate(60);
    ofEnableSmoothing();
    
    setGUI();
	setGUI1();
    setGUI2();
    setGUI3();
    setGUI4();
    gui->setDrawBack(false);
    gui1->setDrawBack(false);
    gui2->setDrawBack(false);
    gui3->setDrawBack(false);
    gui4->setDrawBack(false);
    
    sketch.loadImage("sketch.png");
    //initialize color picker
    float w = 340;
    float h = 340;
    float cx = w/2;
    float cy = h/2;
    img.allocate(w,h,OF_IMAGE_COLOR);
    
    for (float y=0; y<h; y++) {
        for (float x=0; x<w; x++) {
            float angle = atan2(y-cy,x-cx)+PI;
            float dist = ofDist(x,y,cx,cy);
            float hue = angle/TWO_PI*255;
            float sat = ofMap(dist,0,w/4,0,255,true);
            float bri = ofMap(dist,w/4,w/2,255,0,true);
            img.setColor(x,y,ofColor::fromHsb(hue,sat,bri));
        }
    }
    img.reloadTexture();
    
    
}

//--------------------------------------------------------------
void testApp::update(){
	//we do a heartbeat on iOS as the phone will shut down the network connection to save power
	//this keeps the network alive as it thinks it is being used. 
	if( ofGetFrameNum() % 120 == 0 ){
		ofxOscMessage m;
		m.setAddress( "/misc/heartbeat" );
		m.addIntArg( ofGetFrameNum() );
		sender.sendMessage( m );
	}
    
    ofxOscMessage m;
    m.setAddress( "/accel/x" );
    m.addFloatArg( ofxAccelerometer.getForce().x);
    m.setAddress( "/accel/y" );
    m.addFloatArg( ofxAccelerometer.getForce().y);
    m.setAddress( "/accel/z" );
    m.addFloatArg( ofxAccelerometer.getForce().z);
    
    

    sender.sendMessage( m );
    
}

//--------------------------------------------------------------
void testApp::draw(){
//    ofColor dark(20);
//    ofColor lightDark(70);
//    ofBackgroundGradient(dark, lightDark);
    ofBackground(0);
    ofSetColor(255,100);
    sketch.draw(0,-75, ofGetWidth() ,ofGetHeight()+75);
    
    ofSetColor(255,255);
    img.draw(500,200);
    
    ofSetColor(150,150);
    ofSetLineWidth(0.5);
    drawGrid(10, 10);
    
	// display instructions
	string buf;
	buf = "sending osc messages to" + string( HOST ) + ofToString( PORT );
	ofDrawBitmapString( buf
                       + "\nmove the mouse to send osc message [/mouse/position <x> <y>]",
                       10, 20 );
}

//--------------------------------------------------------------

void testApp::touchDown(ofTouchEventArgs & touch){
	ofxOscMessage m;
	m.setAddress( "/touch/state" );
	m.addIntArg(1);
	m.setAddress( "/touch/position/x" );
	m.addIntArg( touch.x );
    m.setAddress("/touch/position/y");
	m.addIntArg( touch.y );
	sender.sendMessage( m );
    
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs & touch){
	ofxOscMessage m;
	m.setAddress( "/touch/position/x" );
	m.addIntArg( touch.x );
    m.setAddress("/touch/position/y");
	m.addIntArg( touch.y );
	sender.sendMessage( m );
    
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs & touch){
	ofxOscMessage m;
	m.setAddress( "/touch/state" );
	m.addIntArg(0);
	sender.sendMessage( m );
}

void testApp::touchDoubleTap(ofTouchEventArgs & touch){}
void testApp::touchCancelled(ofTouchEventArgs & touch){
//    bNoise = false;
}
void testApp::lostFocus(){}
void testApp::gotFocus(){}
void testApp::gotMemoryWarning(){}
void testApp::deviceOrientationChanged(int newOrientation){}
void testApp::exit(){    
	delete gui1;
}
void testApp::drawGrid(float x, float y)
{
    float w = ofGetWidth();
    float h = ofGetHeight();
    
    for(int i = 0; i < h; i+=y)
    {
        ofLine(0,i,w,i);
    }
    
    for(int j = 0; j < w; j+=x)
    {
        ofLine(j,0,j,h);
    }
}

vector<int> testApp:: getToggleMatrixValues(string received_name , ofxUIEventArgs &e){
    
    
    ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
    bool val = toggle->getValue();
    
    vector<string> result1 = ofSplitString(received_name, ",");
    vector<string> result2 = ofSplitString(result1[1], ")");
    vector<string> result3 = ofSplitString(result1[0], "(");
    
    string srow = result3[1];
    string scol = result2[0];
    
    int row = ofToInt(srow);
    int col = ofToInt(scol);
    
    //cout << srow << scol << val << endl;
    vector<int> vresult;
    vresult.push_back(row);
    vresult.push_back(col);
    vresult.push_back((int)val);
    
    cout << vresult[0] << " " <<vresult[1] <<" " << vresult[2] << endl;
    
    return vresult;
    
}


//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e){
    
    
    string name = e.widget->getName();
    int kind = e.widget->getKind();
    cout << "got event from: " << name << endl;
    
    if(name == "FLOCK MODE"){
        vector<int> res =  getToggleMatrixValues(name, e);
        int row = res[0];
        int col = res[1];
        bool val = (bool) res[2];
        cout << name << " <<<<<< " << row << " " << col << " " << val << endl;
    }
	else if(name == "DRAW MODE"){
        vector<int> res =  getToggleMatrixValues(name, e);
        int row = res[0];
        int col = res[1];
        bool val = (bool) res[2];
        cout << name << " <<<<<< " << row << " " << col << " " << val << endl;
	}
	else if(name == "FADE"){
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		cout << "FADE " << slider->getScaledValue() << endl;
		fadeBar = slider->getScaledValue();
	}
    else if(name == "DRAW SCREEN"){
        ofxUIButton *button = (ofxUIButton *) e.widget;
        bDrawScreen = button->getValue();
    }
    else if(name == "CLEAR SCREEN"){
        ofxUIButton *button = (ofxUIButton *) e.widget;
        bClearScreen = button ->getValue();
    }
    
    
    
}


void testApp::setGUI(){
	
    
	float dim = 100;
    float xInit = 10;
    float length = 450-xInit;
    fadeBar = 150;
    cFlockMode = 1;
    
    
    
    cDrawMode = 11;
    
	hideGUI = false;
	
	gui = new ofxUICanvas(40, 60, length+xInit, 660);
	gui->addWidgetDown(new ofxUILabel("FUSE Controller", OFX_UI_FONT_LARGE));
    
    gui->addSpacer(length-xInit, 15);
	
    gui->addWidgetDown(new ofxUILabel("Flock Mode", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIToggleMatrix (dim, dim, 2, 4, "FLOCK MODE"));
    ofxUIToggleMatrix* mtx = (ofxUIToggleMatrix *) gui->getWidget("FLOCK MODE");
    mtx->setAllowMultiple(false);
//    mtx->getToggles()[0].set
    
    gui->addSpacer(length-xInit, 15);
    
    gui->addWidgetDown(new ofxUILabel("Draw Mode", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIToggleMatrix (dim, dim, 2, 4, "DRAW MODE"));
    ofxUIToggleMatrix* mtx2 = (ofxUIToggleMatrix *) gui->getWidget("DRAW MODE");
    mtx2->setAllowMultiple(false);
    
    gui->addSpacer(length-xInit, 15);
	
    gui->addWidgetDown(new ofxUILabel("fadeAmount", OFX_UI_FONT_MEDIUM));
	gui->addSlider("FADE", 0.0, 255.0, fadeBar, 415, dim/2);
    
	ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
}
void testApp::setGUI1(){
    
	float dim = 150;
    float xInit = 10;
    float length = 160-xInit;
    
    gui1 = new ofxUICanvas(500, 80, length+xInit, 180);
    
    gui1->addLabel("Draw Screen Mode", OFX_UI_FONT_SMALL);
    gui1->addButton("DRAW SCREEN", false, dim,dim/2);
	ofAddListener(gui1->newGUIEvent,this,&testApp::guiEvent);
}

void testApp::setGUI2(){
    
	float dim = 150;
    float xInit = 10;
    float length = 160-xInit;
    
    gui2 = new ofxUICanvas(690, 80, length+xInit, 180);
    
    gui2->addLabel("Clear Screen", OFX_UI_FONT_SMALL);
    gui2->addButton("CLEAR SCREEN", false, dim,dim/2);
	ofAddListener(gui2->newGUIEvent,this,&testApp::guiEvent);
}
void testApp::setGUI3(){
    
	float dim = 50;
    float xInit = 10;
    float length = 70-xInit;
    
    satBar = 200;
    
    gui3 = new ofxUICanvas(895, 80, length+xInit, 500);
    gui3->addLabel("All Color", OFX_UI_FONT_SMALL);
    gui3->addWidgetDown(new ofxUIButton("ALL COLOR", false, dim, dim*3/2));
    
    gui3->addSpacer(length-xInit, 15);
    gui3->addSlider("Saturation", 0, 255, satBar, dim, 340);
    
    
	ofAddListener(gui3->newGUIEvent,this,&testApp::guiEvent);
    
}
void testApp::setGUI4(){
    
	float dim = 110;
    float xInit = 10;
    float length = 500-xInit;
    
    gui4 = new ofxUICanvas(500, 550, length+xInit, ofGetHeight());
    gui4->addWidgetDown(new ofxUILabel("Blend Mode", OFX_UI_FONT_MEDIUM));
    gui4->addWidgetDown(new ofxUIToggleMatrix (dim, dim, 2, 4, "BLEND MODE"));
    ofxUIToggleMatrix* mtx = (ofxUIToggleMatrix *) gui4->getWidget("BLEND MODE");
    mtx->setAllowMultiple(false);
    
	ofAddListener(gui4->newGUIEvent,this,&testApp::guiEvent);

}