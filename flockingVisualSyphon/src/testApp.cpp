#include "testApp.h"


const int width = 1024;
const int height = 768;

// comparison routine for sort...
bool comparisonFunction(  particle * a, particle * b ) {
	return a->pos.x < b->pos.x;
}


//--------------------------------------------------------------
void testApp::setup(){

    receiver.setup(PORT);
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
    ofEnableAlphaBlending();
    
    // FBO
    
    rgbaFbo.allocate(ofGetScreenWidth(), ofGetScreenHeight(), GL_RGBA);
    rgbaFboFloat.allocate(ofGetScreenWidth(), ofGetScreenHeight(), GL_RGBA32F_ARB);
	
    rgbaFbo.begin();
	ofClear(255,255,255, 0);
    rgbaFbo.end();
    
    rgbaFboFloat.begin();
	ofClear(255,255,255, 0);
    rgbaFboFloat.end();
    
    fadeAmnt=10;
    numberBlend = 4;
    
    //flocking particles
    // THE FLOWS ----------------------------
    for (int i = 0; i < PARTICLE_NUM; i++){
		particle * myParticle = new particle;
        myParticle->setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
		flockPtc.push_back(myParticle);
	}
	
    // funky particles
    // THE INDIVIDUALS ----------------------------
    for (int i = 0; i < 50; i++){
        particle myParticle;
		myParticle.setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
        funkyPtc.push_back(myParticle);
		
	}
   	
	bRepel		= true;
	radius		= 40;
	strength	= 0.9f;
    
    
    //	CGDisplayHideCursor(kCGDirectMainDisplay);
    ofToggleFullscreen();
	ofSetVerticalSync(true);
	ofSetCircleResolution(80);
	ofBackground(0, 0, 0);
    
    number=0;
    userRadius = 30;
    
    //sound
    soundStream.listDevices();
	
	int bufferSize = 256;
	
	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	volHistory.assign(400, 0.0);
	
	bufferCounter	= 0;
	drawCounter		= 0;
	smoothedVol     = 0.0;
    scaledVol		= 0.0;
    
	soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
    
    //syphon
    
	mainOutputSyphonServer.setName("Screen Output");
	individualTextureSyphonServer.setName("Texture Output");
	mClient.setup();
    mClient.setApplicationName("Simple Server");
    mClient.setServerName("");
    tex.allocate(width, height, GL_RGBA);

}

//--------------------------------------------------------------
void testApp::update(){
    
    
    //fbo
    rgbaFboFloat.begin();
    drawFboTest();
	rgbaFboFloat.end();
    
    // sort all the funkyPtc:
	sort( flockPtc.begin(), flockPtc.end(), comparisonFunction );
    
    for (int i = 0; i < flockPtc.size(); i++){
		flockPtc[i]->resetForce();
	}

    
    
    float fade = abs(sin( ofGetElapsedTimef()*0.099 ));
    
    for (int i = 0; i < flockPtc.size(); i++){
		for (int j = i-1; j >= 0; j--){
			if ( fabs(flockPtc[j]->pos.x - flockPtc[i]->pos.x) >	50) break;
            if (i != j){
                flockPtc[i]->addForFlocking(*flockPtc[j]);
            }
		}
        flockPtc[i]->addRepulsionForce(mouseX, mouseY, 100,0.5);

	}
    
	for (int i = 0; i < flockPtc.size(); i++){
		flockPtc[i]->addFlockingForce();
		flockPtc[i]->addDampingForce();
		flockPtc[i]->update();
	}
	
	
	// wrap torroidally.
	for (int i = 0; i < flockPtc.size(); i++){
		ofVec2f pos = flockPtc[i]->pos;
		if (pos.x < 0) pos.x = ofGetWidth();
		if (pos.x > ofGetWidth()) pos.x = 0;
		if (pos.y < 0) pos.y = ofGetHeight();
		if (pos.y > ofGetHeight()) pos.y = 0;
		flockPtc[i]->pos = pos;
	}
    
    
    //osc
    
    // hide old messages
	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		if(timers[i] < ofGetElapsedTimef()){
			msg_strings[i] = "";
		}
	}
    
	// check for waiting messages
	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
        receiver.getNextMessage(&m);
        if(m.getAddress() == "/flockmode/1" ){
            if(m.getArgAsInt32(0)==1);
            number = 1;
        }
        
        if(m.getAddress() == "/flockmode/2" ){
            if(m.getArgAsInt32(0)==1);
            number = 2;
        }

        if(m.getAddress() == "/flockmode/3" ){
            number = 3;
        }
        
        if(m.getAddress() == "/flockmode/4" ){
            number = 4;
        }

        if(m.getAddress() == "/flockmode/5" ){
            number = 5;
        }
        
        if(m.getAddress() == "/flockmode/6" ){
            number = 6;
        }

        if(m.getAddress() == "/flockmode/7" ){
            number = 7;
        }
        
        if(m.getAddress() == "/flockmode/8" ){
            number = 8;
        }
        
        if(m.getAddress() == "/drawmode/1" ){
            number = 11;
        }

        if(m.getAddress() == "/drawmode/2" ){
            number = 12;
        }
        
        if(m.getAddress() == "/drawmode/1" ){
            number = 11;
        }
        
        if(m.getAddress() == "/drawmode/2" ){
            number = 12;
        }
        
        if(m.getAddress() == "/drawmode/3" ){
            number = 13;
        }
        
        if(m.getAddress() == "/drawmode/4" ){
            number = 14;
        }
        
        if(m.getAddress() == "/drawmode/5" ){
            number = 15;
        }
        
        if(m.getAddress() == "/drawmode/6" ){
            number = 16;
        }
        
        if(m.getAddress() == "/drawmode/7" ){
            number = 17;
        }
        
        if(m.getAddress() == "/drawmode/8" ){
            number = 9;
        }
        
        if(m.getAddress() == "/fadebar/" ){
            float fade;
            fade = m.getArgAsFloat(0);
            fadeAmnt = ofMap(fade,0,150,0,100);
        }
        
        if(m.getAddress() == "/color" ){
       
        }
        
        if(m.getAddress() == "/Saturation"){
            
            sat = m.getArgAsFloat(0);
     
            for (int i = 0; i < funkyPtc.size(); i++){
                funkyPtc[i].c.set(hue,sat,200);
            }
            
            for (int i = 0; i < flockPtc.size(); i++){
                flockPtc[i]->c.set(hue,sat,200);
            }
            
        }


        if(m.getAddress() == "/allcolor" ){
            for (int i = 0; i < funkyPtc.size(); i++){
                funkyPtc[i].c.set(ofRandom(100,250),ofRandom(100,250),ofRandom(10,250));
        }
            
            for (int i = 0; i < flockPtc.size(); i++){
                flockPtc[i]->c.set(ofRandom(100,250),ofRandom(100,250),ofRandom(10,250));
            }

        }

        if(m.getAddress() == "/blendmode/1" ){
            numberBlend = 1;
        }
        if(m.getAddress() == "/blendmode/2" ){
            numberBlend = 2;
        }
        
        if(m.getAddress() == "/blendmode/3" ){
            numberBlend = 3;
        }
        
        if(m.getAddress() == "/blendmode/4" ){
            numberBlend = 4;
        }
        
        
// particles
        
        float rSmooth;
		
        for (int i = 0; i < flockPtc.size(); i++){
            // check for mouse moved message
            if(m.getAddress() == "/accel/x"){
//flocking
                rSmooth = m.getArgAsFloat(0)/2;
                flockPtc[i]->r3 = (m.getArgAsFloat(0)+rSmooth)/2;
                flockPtc[i]->vel.x += (m.getArgAsFloat(0)+rSmooth)/2;
                accelX = m.getArgAsFloat(0);
            }

            if(m.getAddress() == "/accel/y"){
                rSmooth = m.getArgAsFloat(0)/2;
// HEAD
                flockPtc[i]->r2 = (m.getArgAsFloat(0)+rSmooth)/2;
                flockPtc[i]->vel.y += (m.getArgAsFloat(0)+rSmooth)/2;
                accelY = m.getArgAsFloat(0);
            }
             if(m.getAddress() == "/accel/z"){
                 rSmooth = m.getArgAsFloat(0)/2;
                 flockPtc[i]->r1 = (m.getArgAsFloat(0)+rSmooth)/2;
                 flockPtc[i]->vel.z = (m.getArgAsFloat(0)+rSmooth)/2;
//adjustments
                 accelZ = m.getArgAsFloat(0);
             }
            
            if(m.getAddress() == "/hue"){
                hue = m.getArgAsFloat(0) + flockPtc[i]->randomizer;
                ofClamp(hue, 0, 255);
                flockPtc[i]->c.setHue(hue);
            }
            
            if(m.getAddress() == "/sat"){
                sat = m.getArgAsFloat(0) + flockPtc[i]->randomizer;
                ofClamp(sat, 0, 255);
                flockPtc[i]->c.setSaturation(sat);
            }
        }
        
        for (int i = 0; i < funkyPtc.size(); i++){
//funky             
                if(m.getAddress() == "/accel/x"){
// HEAD
                    
                    funkyPtc[i].r3 = m.getArgAsFloat(0);
                    funkyPtc[i].vel.x += m.getArgAsFloat(0);
// adjustments
                    cout << m.getArgAsFloat(0) << endl;
                }
                
                if(m.getAddress() == "/accel/y"){
// HEAD
                    funkyPtc[i].r2 = m.getArgAsFloat(0);
                    funkyPtc[i].vel.y += m.getArgAsFloat(0);
                }
            
    
                if(m.getAddress() == "/accel/z"){
                    funkyPtc[i].r1 = m.getArgAsFloat(0);
                    funkyPtc[i].vel.z += m.getArgAsFloat(0);
// adjustments
                }
                
                
                if(m.getAddress() == "/hue"){
                    funkyPtc[i].c.setHue(m.getArgAsFloat(0));
                }
                
                if(m.getAddress() == "/sat"){
                    funkyPtc[i].c.setSaturation(m.getArgAsFloat(0));
                }
        

            else{
                // unrecognized message: display on the bottom of the screen
                string msg_string;
                msg_string = m.getAddress();
                msg_string += ": ";
                for(int i = 0; i < m.getNumArgs(); i++){
                    // get he argument type
                    msg_string += m.getArgTypeName(i);
                    msg_string += ":";
                    // display the argument - make sure we get the right type
                    if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                        msg_string += ofToString(m.getArgAsInt32(i));
                    }
                    else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                        msg_string += ofToString(m.getArgAsFloat(i));
                    }
                    else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                        msg_string += m.getArgAsString(i);
                    }
                    else{
                        msg_string += "unknown";
                    }
                }
                // add to the list of strings to display
                msg_strings[current_msg_string] = msg_string;
                timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
                current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
                // clear the next line
                msg_strings[current_msg_string] = "";
            }
            
        }
        
	}

    if( numberBlend == 1)blendMode = OF_BLENDMODE_ADD;
    else if( numberBlend == 2)blendMode = OF_BLENDMODE_SCREEN;
    else if( numberBlend == 3)blendMode = OF_BLENDMODE_SUBTRACT;
    else if( numberBlend == 4)blendMode = OF_BLENDMODE_DISABLED;

    // number == 1 +++++++++++++++++++++++++++++++++++++
    
    
    if(number==1) {
              
        sort( flockPtc.begin(), flockPtc.end(), comparisonFunction );
        
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->cohesion.strength = 0.1;
            flockPtc[i]->cohesion.distance = 20;
            flockPtc[i]->alignment.strength =  0.7;
            flockPtc[i]->alignment.distance = 80;
            flockPtc[i]->seperation.strength = 0.7;
            flockPtc[i]->seperation.distance = 28;
            flockPtc[i]->damping = 0.09;
        }
        
        
        for (int i = 0; i < flockPtc.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(flockPtc[j]->pos.x - flockPtc[i]->pos.x) >	50) break;
                if (i != j){
                    flockPtc[i]->addForFlocking(*flockPtc[j]);
                    
                
                }
            
                flockPtc[i]->addRepulsionForce(mouseX, mouseY,100,1);

            
            }
            
        }
        
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->addFlockingForce();
            flockPtc[i]->addDampingForce();
            flockPtc[i]->update();
        }
        
    };
    
    
    
    // number == 2 +++++++++++++++++++++++++++++++++++++
    
    if(number==2)  {
        
        sort( flockPtc.begin(), flockPtc.end(), comparisonFunction );
        
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->cohesion.strength = 0.76;
            flockPtc[i]->cohesion.distance = 50;
            flockPtc[i]->alignment.strength =  0.5;
            flockPtc[i]->alignment.distance = 130;
            flockPtc[i]->seperation.strength = 0.8;
            flockPtc[i]->seperation.distance = 157;
            flockPtc[i]->damping = 0.292;
        }
        
        
        for (int i = 0; i < flockPtc.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(flockPtc[j]->pos.x - flockPtc[i]->pos.x) >200) break;
                if (i != j){
                    flockPtc[i]->addForFlocking(*flockPtc[j]);
                }
            }
            flockPtc[i]->addAttractionForce(mouseX, mouseY, 200, 5);
        
        }
        
          
        // wrap torroidally.
        for (int i = 0; i < flockPtc.size(); i++){
            ofVec2f pos = flockPtc[i]->pos;
            if (pos.x < 0) pos.x = ofGetWidth();
            if (pos.x > ofGetWidth()) pos.x = 0;
            if (pos.y < 0) pos.y = ofGetHeight();
            if (pos.y > ofGetHeight()) pos.y = 0;
            flockPtc[i]->pos = pos;
        }
    }
    
    
    // number == 3 +++++++++++++++++++++++++++++++++++++
    
    if(number==3)  {
        
        
        sort( flockPtc.begin(), flockPtc.end(), comparisonFunction );
        
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->cohesion.strength = 0.2;
            flockPtc[i]->cohesion.distance = 200;
            flockPtc[i]->alignment.strength =  0.24;
            flockPtc[i]->alignment.distance = 198;
            flockPtc[i]->seperation.strength = 0.4;
            flockPtc[i]->seperation.distance = 33;
            flockPtc[i]->damping = 0.029;
        }
        
        
        
        for (int i = 0; i < flockPtc.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(flockPtc[j]->pos.x - flockPtc[i]->pos.x) >50) break;
                if (i != j){
                    flockPtc[i]->addForFlocking(*flockPtc[j]);
                }
            }
       
        }

    }
    // number == 4   +++++++++++++++++++++++++++++++++++++
    
    if(number == 4) {
        
       
        
        sort( flockPtc.begin(), flockPtc.end(), comparisonFunction );
        
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->cohesion.strength = 0.19;
            flockPtc[i]->cohesion.distance = 200;
            flockPtc[i]->alignment.strength =  0.2;
            flockPtc[i]->alignment.distance = 175;
            flockPtc[i]->seperation.strength = 0.46;
            flockPtc[i]->seperation.distance = 200;
            flockPtc[i]->damping = 0.45;
        }
        
        
        for (int i = 0; i < flockPtc.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(flockPtc[j]->pos.x - flockPtc[i]->pos.x) >	50) break;
                if (i != j){
                    flockPtc[i]->addForFlocking(*flockPtc[j]);
                }
           }
            
            flockPtc[i]->addRepulsionForce(mouseX, mouseY, 300, 1);
        
        
        }
        
        
        
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->addFlockingForce();
            flockPtc[i]->addDampingForce();
            flockPtc[i]->update();
        }
        
         }
    
    
    
    // number == 5  +++++++++++++++++++++++++++++++++++++
    
    if(number == 5) {
              
        sort( flockPtc.begin(), flockPtc.end(), comparisonFunction );
        
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->cohesion.strength = 0.5;
            flockPtc[i]->cohesion.distance = 100;
            flockPtc[i]->alignment.strength =  0.38;
            flockPtc[i]->alignment.distance = 52;
            flockPtc[i]->seperation.strength = 0.9;
            flockPtc[i]->seperation.distance = 15;
            flockPtc[i]->damping = 0.68;
        }
        
        
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->resetForce();
        }
        
        
        for (int i = 0; i < flockPtc.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(flockPtc[j]->pos.x - flockPtc[i]->pos.x) > 50) break;
                if (i != j){
                    flockPtc[i]->addForFlocking(*flockPtc[j]);
                }
            }
         
             flockPtc[i]->addAttractionForce(mouseX, mouseY, 100, 5);
        }
        
        
        
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->addFlockingForce();
            flockPtc[i]->addDampingForce();
            flockPtc[i]->update();
        }
        
      
      }
    
    
// number == 6 +++++++++++++++++++++++++++++++++++++
    
    
        if(number == 6) {
            
            sort( flockPtc.begin(), flockPtc.end(), comparisonFunction );
            
            for (int i = 0; i < flockPtc.size(); i++){
                flockPtc[i]->cohesion.strength = 0.20;
                flockPtc[i]->cohesion.distance = 10;
                flockPtc[i]->alignment.strength =  0.31;
                flockPtc[i]->alignment.distance = 200;
                flockPtc[i]->seperation.strength = 0.4;
                flockPtc[i]->seperation.distance = 2;
                flockPtc[i]->damping = 0.20;
            }
            
            
            for (int i = 0; i < flockPtc.size(); i++){
                flockPtc[i]->resetForce();
            }
            
            
            for (int i = 0; i < flockPtc.size(); i++){
                for (int j = i-1; j >= 0; j--){
                    if ( fabs(flockPtc[j]->pos.x - flockPtc[i]->pos.x) >50) break;
                    if (i != j){
                        flockPtc[i]->addForFlocking(*flockPtc[j]);
                    }
                }
                flockPtc[i]->vel.x += mouseX/1000;
                flockPtc[i]->vel.y += mouseY/1000;
                flockPtc[i]->addRepulsionForce(mouseX, mouseY, 30, 3);
            }
            
            
            
            for (int i = 0; i < flockPtc.size(); i++){
                flockPtc[i]->addFlockingForce();
                flockPtc[i]->addDampingForce();
                flockPtc[i]->update();
            }
            
            
        }
    
    // number == 7 repulsion  +++++++++++++++++++++++++++++++++++++
    
    if(number == 7) {
                  
            sort( flockPtc.begin(), flockPtc.end(), comparisonFunction );
            
            for (int i = 0; i < flockPtc.size(); i++){
                flockPtc[i]->cohesion.strength = 0.20;
                flockPtc[i]->cohesion.distance = 100;
                flockPtc[i]->alignment.strength =  0.31;
                flockPtc[i]->alignment.distance = 100;
                flockPtc[i]->seperation.strength = 0.4;
                flockPtc[i]->seperation.distance = 2;
                flockPtc[i]->damping = 0.2;
            }
            
                 
            for (int i = 0; i < flockPtc.size(); i++){
                for (int j = i-1; j >= 0; j--){
                    if ( fabs(flockPtc[j]->pos.x - flockPtc[i]->pos.x) >50) break;
                    if (i != j){
                        flockPtc[i]->addForFlocking(*flockPtc[j]);
                    }
                }
                
                flockPtc[i]->addAttractionForce(mouseX, mouseY, 400, 1);
            }
            
                        
            for (int i = 0; i < flockPtc.size(); i++){
                flockPtc[i]->addFlockingForce();
                flockPtc[i]->addDampingForce();
                flockPtc[i]->update();
            }
            
            
            // wrap torroidally.
            for (int i = 0; i < flockPtc.size(); i++){
                ofVec2f pos = flockPtc[i]->pos;
                if (pos.x < 0) pos.x = ofGetWidth();
                if (pos.x > ofGetWidth()) pos.x = 0;
                if (pos.y < 0) pos.y = ofGetHeight();
                if (pos.y > ofGetHeight()) pos.y = 0;
                flockPtc[i]->pos = pos;
            }
            
            
        }
        
    
    // number == 8  +++++++++++++++++++++++++++++++++++++
    
    if(number == 8) {
        
        // background funkyPtc
        
        sort( flockPtc.begin(), flockPtc.end(), comparisonFunction );
        
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->cohesion.strength = 0.43;
            flockPtc[i]->cohesion.distance = 13.3;
            flockPtc[i]->alignment.strength =  0.2;
            flockPtc[i]->alignment.distance = 16.7;
            flockPtc[i]->seperation.strength = 0.73;
            flockPtc[i]->seperation.distance = 105;
            flockPtc[i]->damping = 0.3;
        }
        
        
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->resetForce();
        }
        
        
        for (int i = 0; i < flockPtc.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(flockPtc[j]->pos.x - flockPtc[i]->pos.x) >50) break;
                if (i != j){
                    flockPtc[i]->addForFlocking(*flockPtc[j]);
                }
            }
        }
        
        
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->addFlockingForce();
            flockPtc[i]->addDampingForce();
            flockPtc[i]->update();
        }
        
    
    }

    
    // number == 9  +++++++++++++++++++++++++++++++++++++
    
    if(number == 9) {
        
        sort( flockPtc.begin(), flockPtc.end(), comparisonFunction );
        
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->cohesion.strength = 1;
            flockPtc[i]->cohesion.distance = 200;
            flockPtc[i]->alignment.strength =  1;
            flockPtc[i]->alignment.distance = 200;
            flockPtc[i]->seperation.strength = 0.77;
            flockPtc[i]->seperation.distance = 84.4;
            flockPtc[i]->damping = 0.2;
        }
        
        
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->resetForce();
        }
        
        
        for (int i = 0; i < flockPtc.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(flockPtc[j]->pos.x - flockPtc[i]->pos.x) >50) break;
                if (i != j){
                    flockPtc[i]->addForFlocking(*flockPtc[j]);
                }
            }
             flockPtc[i]->addAttractionForce(mouseX, mouseY, 100, 10);
        }
        
        
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->addFlockingForce();
            flockPtc[i]->addDampingForce();
            flockPtc[i]->update();
        }
        
        
    }
    

    
    
   // number == 11 == draw +++++++++++++++++++++++++++++++++++++

    
    if(number == 11) {
    
    for (int i = 0; i < funkyPtc.size(); i++){
        funkyPtc[i].resetForce();
    }
    for (int i = 0; i < funkyPtc.size(); i++){
        
        funkyPtc[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
        funkyPtc[i].alignment.strength = scaledVol*2000;
        funkyPtc[i].c.set(255,ofRandom(100,250),255);
        
        
        for (int j = 0; j < i; j++){
            if (bRepel){
                funkyPtc[i].addRepulsionForce(funkyPtc[j], radius, strength);
            } else {
                funkyPtc[i].addAttractionForce(funkyPtc[j], radius, strength);
            }
        }
    }
    
    for (int i = 0; i < funkyPtc.size(); i++){
        funkyPtc[i].addDampingForce();
        funkyPtc[i].update();
    }
    

}

    
// number == 12 +++++++++++++++++++++++++++++++++++++
    
    if(number == 12) {

    
    // font funkyPtc
    
    for (int i = 0; i < funkyPtc.size(); i++){
        funkyPtc[i].resetForce();
    }
    
    
    for (int i = 0; i < funkyPtc.size(); i++){
        
        funkyPtc[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
        
        for (int j = 0; j < i; j++){
            if (bRepel){
                funkyPtc[i].addRepulsionForce(funkyPtc[j], radius + smoothedVol * 3000, strength);
            } else {
                funkyPtc[i].addAttractionForce(funkyPtc[j], radius , strength);
            }
        }
    }
    
    for (int i = 0; i < funkyPtc.size(); i++){
        funkyPtc[i].addDampingForce();
        funkyPtc[i].damping = 0.1;
        funkyPtc[i].update();
    }
    }
    
    
// number == 13 +++++++++++++++++++++++++++++++++++++
    
    if(number == 13) {
  
    
    
    for (int i = 0; i < funkyPtc.size(); i++){
        funkyPtc[i].resetForce();
    }
    
    for (int i = 0; i < funkyPtc.size(); i++){
        
        for (int j = 0; j < funkyPtc.size(); j++){
            if (i != j){
                funkyPtc[i].addForFlocking(funkyPtc[j]);
            }
        }
        
    }
    
    
    for (int i = 0; i < funkyPtc.size(); i++){
        
        funkyPtc[i].damping = smoothedVol*10;
    }
    
    for (int i = 0; i < funkyPtc.size(); i++){
        
        funkyPtc[i].addFlockingForce();
        funkyPtc[i].addDampingForce();
        funkyPtc[i].update();
    }
    
    
    // wrap torroidally.
    for (int i = 0; i < funkyPtc.size(); i++){
        ofVec2f pos = funkyPtc[i].pos;
        if (pos.x < 0) pos.x = ofGetWidth();
        if (pos.x > ofGetWidth()) pos.x = 0;
        if (pos.y < 0) pos.y = ofGetHeight();
        if (pos.y > ofGetHeight()) pos.y = 0;
        funkyPtc[i].pos = pos;
    }
    

}
    
   // number == 14 == draw +++++++++++++++++++++++++++++++++++++
    
    
    if(number == 14) {
        
    for (int i = 0; i < funkyPtc.size(); i++){
        funkyPtc[i].resetForce();
    }
    
    
    for (int i = 0; i < funkyPtc.size(); i++){
        for (int j = 0; j < funkyPtc.size(); j++){
            if (i != j){
                funkyPtc[i].addForFlocking(funkyPtc[j]);
            }
        }
               funkyPtc[i].addRepulsionForce(mouseX, mouseY, 40, 0.4);
    }
    
    for (int i = 0; i < funkyPtc.size(); i++){
        funkyPtc[i].addFlockingForce();
        funkyPtc[i].addDampingForce();
        funkyPtc[i].update();
    }
    
    
    // wrap torroidally.
    for (int i = 0; i < funkyPtc.size(); i++){
        ofVec2f pos = funkyPtc[i].pos;
        if (pos.x < 0) pos.x = ofGetWidth();
        if (pos.x > ofGetWidth()) pos.x = 0;
        if (pos.y < 0) pos.y = ofGetHeight();
        if (pos.y > ofGetHeight()) pos.y = 0;
        funkyPtc[i].pos = pos;
    }
    
    for (int i = 0; i < funkyPtc.size(); i++){
        funkyPtc[i].seperation.distance = smoothedVol*1000 ;
        funkyPtc[i].seperation.strength = 23;
        funkyPtc[i].damping = 0.13;
        
    }
}
    
    
    
    // number == 15  +++++++++++++++++++++++++++++++++++++
    
    if(number == 15) {
        
   
    for (int i = 0; i < funkyPtc.size(); i++){
        funkyPtc[i].resetForce();
    }
    
    for (int i = 0; i < funkyPtc.size(); i++){
        for (int j = 0; j < funkyPtc.size(); j++){
            if (i != j){
                funkyPtc[i].addForFlocking(funkyPtc[j]);
            }
        }
        
        
        funkyPtc[i].addRepulsionForce(mouseX, mouseY, 40, 0.4);
    }
    
    for (int i = 0; i < funkyPtc.size(); i++){
        
        
        funkyPtc[i].addFlockingForce();
        funkyPtc[i].cohesion.strength = smoothedVol*100;
        funkyPtc[i].damping = 0.05;
        funkyPtc[i].addDampingForce();
        funkyPtc[i].update();
    }
    
    
    // wrap torroidally.
    for (int i = 0; i < funkyPtc.size(); i++){
        ofVec2f pos = funkyPtc[i].pos;
        if (pos.x < 0) pos.x = ofGetWidth();
        if (pos.x > ofGetWidth()) pos.x = 0;
        if (pos.y < 0) pos.y = ofGetHeight();
        if (pos.y > ofGetHeight()) pos.y = 0;
        funkyPtc[i].pos = pos;
    }
    
}
    
    
    // number == 16 == Launch +++++++++++++++++++++++++++++++++++++
    
    if(number == 16) {
        
        for (int i = 0; i < funkyPtc.size(); i++){
            funkyPtc[i].resetForce();
        }
        
        for (int i = 0; i < funkyPtc.size(); i++){
            
            funkyPtc[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
            funkyPtc[i].c.set(ofRandom(100,255), ofRandom(100,255), ofRandom(100,255));
            
            for (int j = 0; j < i; j++){
                if (bRepel){
                    funkyPtc[i].addRepulsionForce(funkyPtc[j], radius+200, strength);
                } else {
                    funkyPtc[i].addAttractionForce(funkyPtc[j], radius, strength);
                }
            }
        }
        
        for (int i = 0; i < funkyPtc.size(); i++){
            funkyPtc[i].addDampingForce();
            funkyPtc[i].update();
        }
        
    }
    
//cout << "accel " + ofToString(accelX) + " , "
//+ ofToString(accelY) + " , "
//+ ofToString(accelZ) + "\n";

ofPushView();
ofPushStyle();

ofPushMatrix();

ofEnableBlendMode(OF_BLENDMODE_ALPHA);
ofClear(0, 255);
ofSetColor( ofMap( fade , 0.0f, 0.5f, 0.0f, 1.0f, true) * 255,255);


ofDisableBlendMode();
ofPopMatrix();

ofPopView();
ofPopStyle();

ofSetWindowTitle(ofToString(ofGetFrameRate()));




    //tail testing
    timePoint temp;

  	temp.t = ofGetElapsedTimef() - startTime;
	pts.push_back(temp);
	
    
    if(number==0) {
        
        if (pts.size() > 750){
            pts.erase(pts.begin());
        }
        
    }

}

//--------------------------------------------------------------
void testApp::drawFboTest(){
	
	if( ofGetKeyPressed('0') ){
		ofClear(255,255,255, 0);
	}
    

   ofFill();
  
   ofColor dark(10,fadeAmnt);
   ofColor black(0, fadeAmnt);
    
    ofBackgroundGradient(dark, black);
    
    if(number == 1) fadeAmnt = 100;
    if(number == 2) fadeAmnt = 60;
    if(number == 3) fadeAmnt = 30;
    if(number == 4) fadeAmnt = 5;
    if(number == 5) fadeAmnt = 10;
    if(number == 6) fadeAmnt = 20;
    if(number == 7) fadeAmnt = 0;
    if(number == 8) fadeAmnt = 20;
    if(number == 9) fadeAmnt = 20;
    if(number == 10) fadeAmnt = 5;
    if(number == 12) fadeAmnt = 100;
    
    
    if (number < 10) {
        for (int i = 0; i < flockPtc.size(); i++){
            flockPtc[i]->draw();
        }
    }

    else if(number > 10){
        for (int i = 0; i < funkyPtc.size(); i++){
            funkyPtc[i].draw();
            funkyPtc[i].c.setHsb(hue,sat,200);
        }
    }

 
}


//--------------------------------------------------------------
void testApp::draw(){
    
    // Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    
    
    
    ofSetColor(255, 50);
    rgbaFbo.draw(0,0);
    rgbaFboFloat.draw(0,0);
    
    ofDrawBitmapString(ofToString(fadeAmnt),0,100);    
    ofEnableBlendMode(blendMode);


    string alphaInfo = "Current alpha fade amnt = " + ofToString(fadeAmnt);
   
    for (int i = 0; i < flockPtc.size(); i++){
	string reportString =
    
   "\n background = " + ofToString(smoothedVol)
    +  "\n accelX= " + ofToString(accelX)
    +  "\n accelY= " + ofToString(accelY)
    +  "\n accelZ= " + ofToString(accelZ);
   
//        ofDrawBitmapString(reportString, 30, 30);
   }

//  ofDrawBitmapString(reportString2, 1100, 30);
   

    
    // draw static into our one texture.
    unsigned char pixels[200*100*4];
    for (int i = 0; i < 200*100*4; i++){
        pixels[i] = (int)(255 * ofRandomuf());
    }
    tex.loadData(pixels, 200, 100, GL_RGBA);
    tex.draw(50, 50);
    
    
	// Syphon Stuff
    ofSetColor(255, 255, 255);
    ofEnableAlphaBlending();
    mClient.draw(50, 50);
	mainOutputSyphonServer.publishScreen();
    individualTextureSyphonServer.publishTexture(&tex);
    ofDrawBitmapString("Note this text is not captured by Syphon since it is drawn after publishing.\nYou can use this to hide your GUI for example.", 150,500);
    
    
}




//--------------------------------------------------------------
void testApp::audioIn(float * input, int bufferSize, int nChannels){
	
	float curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;
    
	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume
	for (int i = 0; i < bufferSize; i++){
		left[i]		= input[i*2]*0.5;
		right[i]	= input[i*2+1]*0.5;
        
		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;
	}
	
	//this is how we get the mean of rms :)
	curVol /= (float)numCounted;
	
	// this is how we get the root of rms :)
	curVol = sqrt( curVol );
	
	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;
	
	bufferCounter++;
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if( key == '1' ) number = 1;
    if( key == '2' ) number = 2;
    if( key == '3' ) number = 3;
    if( key == '4' ) number = 4;
    if( key == '5' ) number = 5;
  	if( key == '6' ) number = 6;
    if( key == '7' ) number = 7;
    if( key == '8' ) number = 8;
    if( key == '9' ) number = 9;
    if( key == '0' ) number = 0;

 //-------------------- Front ---------------
    if( key == 'q' ) number = 11;
    if( key == 'w' ) number = 12;
    if( key == 'e' ) number = 13;
    if( key == 'r' ) number = 14;
    if( key == 't' ) number = 15;
    if( key == 'y' ) number = 16;
    if( key == 'u' ) number = 17;

  //-------------------- BlendMode ---------------
    if( key == 'z' ){
        numberBlend = 1;
	}
    if( key == 'x' ){
        numberBlend = 2;
	}
    if( key == 'c' ){
        numberBlend = 3;
	}
    if( key == 'v' ){
        numberBlend = 4;
	}
    if( key == 'b' ){
        numberBlend = 5;
	}
    if( key == 'n' ){
        numberBlend = 6;
	}
    if( key == 'f') ofToggleFullscreen();
    if( key == 'g') CGDisplayHideCursor(kCGDirectMainDisplay);

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
  
   funkyPtc.erase(funkyPtc.begin());
   particle myParticle;
   myParticle.setInitialCondition(x,y,0,0);
   funkyPtc.push_back(myParticle);
    
    
    if (10< number < 18){
        for (int i = 0; i < funkyPtc.size(); i++){
            funkyPtc[i].c.setHsb((ofRandom(hue-40),(hue+40)),sat,200);
        }
        
    }

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){}
void testApp::mouseReleased(int x, int y, int button){}
void testApp::windowResized(int w, int h){}
void testApp::gotMessage(ofMessage msg){}
void testApp::dragEvent(ofDragInfo dragInfo){}
