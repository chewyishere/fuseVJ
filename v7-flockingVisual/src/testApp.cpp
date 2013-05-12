#include "testApp.h"


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
//	CGDisplayHideCursor(kCGDirectMainDisplay);
    
    // set mode
    number = 0;
    
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

  // THE BACKGROUND FLOWS ----------------------------
    
    for (int i = 0; i < 450; i++){
		particle * myParticle = new particle;
        myParticle->setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
		bgParti.push_back(myParticle);
	}
	
 // THE FOREGROUND INDIVIDUALS FOR DRAWING----------------------------   
    
    for (int i = 0; i < 50; i++){
		myParticle.setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
        dwgParti.push_back(myParticle);
		
	}
   	
    //local interactions
	bRepel		= true;
	radius		= 40;
	strength	= 0.9f;
    
    //sound
	int bufferSize = 256;
	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	volHistory.assign(400, 0.0);
	
	bufferCounter	= 0;
	drawCounter		= 0;
	smoothedVol     = 0.0;
    scaledVol		= 0.0;
    
	soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
}

//--------------------------------------------------------------
void testApp::update(){
    //fbo
    rgbaFboFloat.begin();
    drawFboTest();
	rgbaFboFloat.end();
    // sort all the dwgParti:
	sort( bgParti.begin(), bgParti.end(), comparisonFunction );
    for (int i = 0; i < bgParti.size(); i++){
		bgParti[i]->resetForce();
	}
    
    for (int i = 0; i < bgParti.size(); i++){
		for (int j = i-1; j >= 0; j--){
			if ( fabs(bgParti[j]->pos.x - bgParti[i]->pos.x) >	50) break;
            if (i != j){
                bgParti[i]->addForFlocking(*bgParti[j]);
            }
		}
        bgParti[i]->addRepulsionForce(mouseX, mouseY, 40, 0.4);
	}
    
	for (int i = 0; i < bgParti.size(); i++){
		bgParti[i]->addFlockingForce();
		bgParti[i]->addDampingForce();
		bgParti[i]->update();
	}
	
	
	// wrap torroidally.
	for (int i = 0; i < bgParti.size(); i++){
		ofVec2f pos = bgParti[i]->pos;
		if (pos.x < 0) pos.x = ofGetWidth();
		if (pos.x > ofGetWidth()) pos.x = 0;
		if (pos.y < 0) pos.y = ofGetHeight();
		if (pos.y > ofGetHeight()) pos.y = 0;
		bgParti[i]->pos = pos;
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
        for (int i = 0; i < dwgParti.size(); i++){
            // check for mouse moved message
            if(m.getAddress() == "/accel/x"){
                bgParti[i]->r3 = m.getArgAsFloat(0);
                bgParti[i]->vel.x += m.getArgAsFloat(0);
                cout << m.getArgAsFloat(0) << endl;
            }
            
            if(m.getAddress() == "/accel/y"){
                bgParti[i]->r2 = m.getArgAsFloat(0);
                bgParti[i]->vel.y += m.getArgAsFloat(0);
            }
            
            if(m.getAddress() == "/accel/z"){
                bgParti[i]->r1 = m.getArgAsFloat(0);
                bgParti[i]->vel.z += m.getArgAsFloat(0);
            }
            
            
            if(m.getAddress() == "/hue"){
                bgParti[i]->c.setHue(m.getArgAsFloat(0));
            }
            
            if(m.getAddress() == "/sat"){
                bgParti[i]->c.setSaturation(m.getArgAsFloat(0));
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
    
    ofPushView();
    ofPushStyle();
    
    ofPushMatrix();
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofClear(0, 255);
    
    float fade = abs(sin( ofGetElapsedTimef()*0.099 ));
    ofSetColor( ofMap( fade , 0.0f, 0.5f, 0.0f, 1.0f, true) * 255,255);
    
    ofDisableBlendMode();
    ofPopMatrix();
    
    ofPopView();
    ofPopStyle();
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
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
        for (int i = 0; i < dwgParti.size(); i++){
            // check for mouse moved message
            if(m.getAddress() == "/accel/x"){
                dwgParti[i].r3 = m.getArgAsFloat(0);
                dwgParti[i].vel.x += m.getArgAsFloat(0);
                cout << m.getArgAsFloat(0) << endl;
            }
            
            if(m.getAddress() == "/accel/y"){
                dwgParti[i].r2 = m.getArgAsFloat(0);
                dwgParti[i].vel.y += m.getArgAsFloat(0);
            }
            
            if(m.getAddress() == "/accel/z"){
                dwgParti[i].r1 = m.getArgAsFloat(0);
                dwgParti[i].vel.z += m.getArgAsFloat(0);
            }
            
            
            if(m.getAddress() == "/hue"){
                dwgParti[i].c.setHue(m.getArgAsFloat(0));
            }
            
            if(m.getAddress() == "/sat"){
                dwgParti[i].c.setSaturation(m.getArgAsFloat(0));
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
    
    
    // number == 1 +++++++++++++++++++++++++++++++++++++
    
    
    if(number==1) {
        
        // background dwgParti
        
        sort( bgParti.begin(), bgParti.end(), comparisonFunction );
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->cohesion.strength = 0.1;
            bgParti[i]->cohesion.distance = 50;
            bgParti[i]->alignment.strength =  0.7;
            bgParti[i]->alignment.distance = 42;
            bgParti[i]->seperation.strength = 0.7;
            bgParti[i]->seperation.distance = 38;
            bgParti[i]->damping = 0.08;            
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->resetForce();
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(bgParti[j]->pos.x - bgParti[i]->pos.x) >	50) break;
                if (i != j){
                    bgParti[i]->addForFlocking(*bgParti[j]);
                }
            }
        }

//            for (int i = 0; i < bgParti.size(); i++){
//                bgParti[i]->c.set(ofRandom(100,255),0,ofRandom(0,50),50);
//        }
//        
//        
//        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->addFlockingForce();
            bgParti[i]->addDampingForce();
            bgParti[i]->update();
        }
        
        // wrap torroidally.
        for (int i = 0; i < bgParti.size(); i++){
            ofVec2f pos = bgParti[i]->pos;
            if (pos.x < 0) pos.x = ofGetWidth();
            if (pos.x > ofGetWidth()) pos.x = 0;
            if (pos.y < 0) pos.y = ofGetHeight();
            if (pos.y > ofGetHeight()) pos.y = 0;
            bgParti[i]->pos = pos;
        }

    };
    
    
    
    // number == 2 addAttractionForce to Mouse +++++++++++++++++++++++++++++++++++++
    
    if(number==2)  {
        
        
        // background dwgParti
        
        sort( bgParti.begin(), bgParti.end(), comparisonFunction );
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->cohesion.strength = 0.76;
            bgParti[i]->cohesion.distance = 50;
            bgParti[i]->alignment.strength =  0.5;
            bgParti[i]->alignment.distance = 130;
            bgParti[i]->seperation.strength = 0.4;
            bgParti[i]->seperation.distance = 157;
            bgParti[i]->damping = 0.192;
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->resetForce();
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(bgParti[j]->pos.x - bgParti[i]->pos.x) >	50) break;
                if (i != j){
                    bgParti[i]->addForFlocking(*bgParti[j]);
                }
            }
        }
        
        
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->addFlockingForce();
            bgParti[i]->addDampingForce();
            bgParti[i]->update();
        }
        
        
        // wrap torroidally.
        for (int i = 0; i < bgParti.size(); i++){
            ofVec2f pos = bgParti[i]->pos;
            if (pos.x < 0) pos.x = ofGetWidth();
            if (pos.x > ofGetWidth()) pos.x = 0;
            if (pos.y < 0) pos.y = ofGetHeight();
            if (pos.y > ofGetHeight()) pos.y = 0;
            bgParti[i]->pos = pos;
        }
    }
    
    
    // number == 3  addRepulsionForce to Mouse  +++++++++++++++++++++++++++++++++++++
    
    if(number==3)  {
        
        // background dwgParti
        
        sort( bgParti.begin(), bgParti.end(), comparisonFunction );
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->cohesion.strength = 0.2;
            bgParti[i]->cohesion.distance = 200;
            bgParti[i]->alignment.strength =  0.24;
            bgParti[i]->alignment.distance = 198;
            bgParti[i]->seperation.strength = 0.4;
            bgParti[i]->seperation.distance = 33;
            bgParti[i]->damping = 0.029;
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->resetForce();
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(bgParti[j]->pos.x - bgParti[i]->pos.x) >	50) break;
                if (i != j){
                    bgParti[i]->addForFlocking(*bgParti[j]);
                }
            }
        }
        
        
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->addFlockingForce();
            bgParti[i]->addDampingForce();
            bgParti[i]->update();
        }
        
        
        // wrap torroidally.
        for (int i = 0; i < bgParti.size(); i++){
            ofVec2f pos = bgParti[i]->pos;
            if (pos.x < 0) pos.x = ofGetWidth();
            if (pos.x > ofGetWidth()) pos.x = 0;
            if (pos.y < 0) pos.y = ofGetHeight();
            if (pos.y > ofGetHeight()) pos.y = 0;
            bgParti[i]->pos = pos;
        }
        
        
        
         }
    // number == 4  seperation.distance +++++++++++++++++++++++++++++++++++++
    
    if(number == 4) {
        
        // background dwgParti
        
        sort( bgParti.begin(), bgParti.end(), comparisonFunction );
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->cohesion.strength = 0.19;
            bgParti[i]->cohesion.distance = 200;
            bgParti[i]->alignment.strength =  0.2;
            bgParti[i]->alignment.distance = 175;
            bgParti[i]->seperation.strength = 0.46;
            bgParti[i]->seperation.distance = 200;
            bgParti[i]->damping = 0.45;
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->resetForce();
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(bgParti[j]->pos.x - bgParti[i]->pos.x) >	50) break;
                if (i != j){
                    bgParti[i]->addForFlocking(*bgParti[j]);
                }
            }
        }
        
        
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->addFlockingForce();
            bgParti[i]->addDampingForce();
            bgParti[i]->update();
        }
        
        
        // wrap torroidally.
        for (int i = 0; i < bgParti.size(); i++){
            ofVec2f pos = bgParti[i]->pos;
            if (pos.x < 0) pos.x = ofGetWidth();
            if (pos.x > ofGetWidth()) pos.x = 0;
            if (pos.y < 0) pos.y = ofGetHeight();
            if (pos.y > ofGetHeight()) pos.y = 0;
            bgParti[i]->pos = pos;
        }
        
        
         }
    
    
    
    // number == 5  +++++++++++++++++++++++++++++++++++++
    
    if(number == 5) {
        // background dwgParti
        
        sort( bgParti.begin(), bgParti.end(), comparisonFunction );
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->cohesion.strength = 0.5;
            bgParti[i]->cohesion.distance = 100;
            bgParti[i]->alignment.strength =  0.38;
            bgParti[i]->alignment.distance = 52;
            bgParti[i]->seperation.strength = 0.9;
            bgParti[i]->seperation.distance = 15;
            bgParti[i]->damping = 0.68;
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->resetForce();
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(bgParti[j]->pos.x - bgParti[i]->pos.x) >	50) break;
                if (i != j){
                    bgParti[i]->addForFlocking(*bgParti[j]);
                }
            }
        }
        
        
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->addFlockingForce();
            bgParti[i]->addDampingForce();
            bgParti[i]->update();
        }
        
        
        // wrap torroidally.
        for (int i = 0; i < bgParti.size(); i++){
            ofVec2f pos = bgParti[i]->pos;
            if (pos.x < 0) pos.x = ofGetWidth();
            if (pos.x > ofGetWidth()) pos.x = 0;
            if (pos.y < 0) pos.y = ofGetHeight();
            if (pos.y > ofGetHeight()) pos.y = 0;
            bgParti[i]->pos = pos;
        }
        
        
      }
    
    
// number == 6 repulsion = sound +++++++++++++++++++++++++++++++++++++
    
    
        if(number == 6) {
            // background dwgParti
            
            sort( bgParti.begin(), bgParti.end(), comparisonFunction );
            
            for (int i = 0; i < bgParti.size(); i++){
                bgParti[i]->cohesion.strength = 0.20;
                bgParti[i]->cohesion.distance = 10;
                bgParti[i]->alignment.strength =  0.31;
                bgParti[i]->alignment.distance = 200;
                bgParti[i]->seperation.strength = 0.4;
                bgParti[i]->seperation.distance = 2;
                bgParti[i]->damping = 0.20;
            }
            
            
            for (int i = 0; i < bgParti.size(); i++){
                bgParti[i]->resetForce();
            }
            
            
            for (int i = 0; i < bgParti.size(); i++){
                for (int j = i-1; j >= 0; j--){
                    if ( fabs(bgParti[j]->pos.x - bgParti[i]->pos.x) >50) break;
                    if (i != j){
                        bgParti[i]->addForFlocking(*bgParti[j]);
                    }
                }
            }
            
            
            
            for (int i = 0; i < bgParti.size(); i++){
                bgParti[i]->addFlockingForce();
                bgParti[i]->addDampingForce();
                bgParti[i]->update();
            }
            
            
            // wrap torroidally.
            for (int i = 0; i < bgParti.size(); i++){
                ofVec2f pos = bgParti[i]->pos;
                if (pos.x < 0) pos.x = ofGetWidth();
                if (pos.x > ofGetWidth()) pos.x = 0;
                if (pos.y < 0) pos.y = ofGetHeight();
                if (pos.y > ofGetHeight()) pos.y = 0;
                bgParti[i]->pos = pos;
            }
            
            
        }
    
    
    
    
    // number == 7 repulsion  +++++++++++++++++++++++++++++++++++++
    
    if(number == 7) {
        
            // background dwgParti
            
            sort( bgParti.begin(), bgParti.end(), comparisonFunction );
            
            for (int i = 0; i < bgParti.size(); i++){
                bgParti[i]->cohesion.strength = 0.20;
                bgParti[i]->cohesion.distance = 100;
                bgParti[i]->alignment.strength =  0.31;
                bgParti[i]->alignment.distance = 20;
                bgParti[i]->seperation.strength = 0.4;
                bgParti[i]->seperation.distance = 2;
                bgParti[i]->damping = 0.8;
            }
            
            
            for (int i = 0; i < bgParti.size(); i++){
                bgParti[i]->resetForce();
            }
            
            
            for (int i = 0; i < bgParti.size(); i++){
                for (int j = i-1; j >= 0; j--){
                    if ( fabs(bgParti[j]->pos.x - bgParti[i]->pos.x) >50) break;
                    if (i != j){
                        bgParti[i]->addForFlocking(*bgParti[j]);
                    }
                }
            }
            
            
            
            for (int i = 0; i < bgParti.size(); i++){
                bgParti[i]->addFlockingForce();
                bgParti[i]->addDampingForce();
                bgParti[i]->update();
            }
            
            
            // wrap torroidally.
            for (int i = 0; i < bgParti.size(); i++){
                ofVec2f pos = bgParti[i]->pos;
                if (pos.x < 0) pos.x = ofGetWidth();
                if (pos.x > ofGetWidth()) pos.x = 0;
                if (pos.y < 0) pos.y = ofGetHeight();
                if (pos.y > ofGetHeight()) pos.y = 0;
                bgParti[i]->pos = pos;
            }
            
            
        }
        
    
    // number == 8  +++++++++++++++++++++++++++++++++++++
    
    if(number == 8) {
        
        // background dwgParti
        
        sort( bgParti.begin(), bgParti.end(), comparisonFunction );
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->cohesion.strength = 0.43;
            bgParti[i]->cohesion.distance = 13.3;
            bgParti[i]->alignment.strength =  0.2;
            bgParti[i]->alignment.distance = 16.7;
            bgParti[i]->seperation.strength = 0.73;
            bgParti[i]->seperation.distance = 105;
            bgParti[i]->damping = 0.3;
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->resetForce();
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(bgParti[j]->pos.x - bgParti[i]->pos.x) >50) break;
                if (i != j){
                    bgParti[i]->addForFlocking(*bgParti[j]);
                }
            }
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->addFlockingForce();
            bgParti[i]->addDampingForce();
            bgParti[i]->update();
        }
        
        
    }

    
    // number == 9  +++++++++++++++++++++++++++++++++++++
    
    if(number == 9) {
        
        // background dwgParti
        
        sort( bgParti.begin(), bgParti.end(), comparisonFunction );
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->cohesion.strength = 1;
            bgParti[i]->cohesion.distance = 200;
            bgParti[i]->alignment.strength =  0.5;
            bgParti[i]->alignment.distance = 200;
            bgParti[i]->seperation.strength = 0.77;
            bgParti[i]->seperation.distance = 84.4;
            bgParti[i]->damping = 0.2;
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->resetForce();
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(bgParti[j]->pos.x - bgParti[i]->pos.x) >50) break;
                if (i != j){
                    bgParti[i]->addForFlocking(*bgParti[j]);
                }
            }
        }
        
        
        for (int i = 0; i < bgParti.size(); i++){
            bgParti[i]->addFlockingForce();
            bgParti[i]->addDampingForce();
            bgParti[i]->update();
        }
        
        
    }
    

    
    
       // number == 11 == draw +++++++++++++++++++++++++++++++++++++

    // font dwgParti
    
    
    if(number == 11) {
    
    for (int i = 0; i < dwgParti.size(); i++){
        dwgParti[i].resetForce();
    }
      blendMode = OF_BLENDMODE_SCREEN;
    for (int i = 0; i < dwgParti.size(); i++){
        
        dwgParti[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
        dwgParti[i].alignment.strength = scaledVol*2000;
        dwgParti[i].c.set(255,ofRandom(100,250),255);
        
        
        for (int j = 0; j < i; j++){
            if (bRepel){
                dwgParti[i].addRepulsionForce(dwgParti[j], radius, strength);
            } else {
                dwgParti[i].addAttractionForce(dwgParti[j], radius, strength);
            }
        }
    }
    
    for (int i = 0; i < dwgParti.size(); i++){
        dwgParti[i].addDampingForce();
        dwgParti[i].update();
    }
    

}

    
// number == 12 == draw +++++++++++++++++++++++++++++++++++++
    
    // font dwgParti
    
    if(number == 12) {

    
    // font dwgParti
    
    for (int i = 0; i < dwgParti.size(); i++){
        dwgParti[i].resetForce();
    }
    
    
    for (int i = 0; i < dwgParti.size(); i++){
        
        dwgParti[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
        // dwgParti[i].scl.x = 500;
        // dwgParti[i].scl.y = 500;
        
        for (int j = 0; j < i; j++){
            if (bRepel){
                dwgParti[i].addRepulsionForce(dwgParti[j], radius + smoothedVol * 3000, strength);
            } else {
                dwgParti[i].addAttractionForce(dwgParti[j], radius , strength);
            }
        }
    }
    
    for (int i = 0; i < dwgParti.size(); i++){
        dwgParti[i].addDampingForce();
        dwgParti[i].damping = 0.1;
        dwgParti[i].update();
    }
    }
    
    
// number == 13 == draw +++++++++++++++++++++++++++++++++++++
    
    // font dwgParti
    
    if(number == 13) {
  
    
    // font dwgParti 
    
    for (int i = 0; i < dwgParti.size(); i++){
        dwgParti[i].resetForce();
    }
    
    for (int i = 0; i < dwgParti.size(); i++){
        
        for (int j = 0; j < dwgParti.size(); j++){
            if (i != j){
                dwgParti[i].addForFlocking(dwgParti[j]);
            }
        }
        
    }
    
    
    
    for (int i = 0; i < dwgParti.size(); i++){
        
        dwgParti[i].damping = smoothedVol*200;
    }
    
    for (int i = 0; i < dwgParti.size(); i++){
        
        dwgParti[i].addFlockingForce();
        dwgParti[i].addDampingForce();
        dwgParti[i].update();
    }
    
    
    // wrap torroidally.
    for (int i = 0; i < dwgParti.size(); i++){
        ofVec2f pos = dwgParti[i].pos;
        if (pos.x < 0) pos.x = ofGetWidth();
        if (pos.x > ofGetWidth()) pos.x = 0;
        if (pos.y < 0) pos.y = ofGetHeight();
        if (pos.y > ofGetHeight()) pos.y = 0;
        dwgParti[i].pos = pos;
    }
    

}
    
   // number == 14 == draw +++++++++++++++++++++++++++++++++++++
    
    // font dwgParti
    
    if(number == 14) {
        
    for (int i = 0; i < dwgParti.size(); i++){
        dwgParti[i].resetForce();
    }
    
    
    for (int i = 0; i < dwgParti.size(); i++){
        for (int j = 0; j < dwgParti.size(); j++){
            if (i != j){
                dwgParti[i].addForFlocking(dwgParti[j]);
            }
        }
        
        // this was something we tried in class (partitcle move away from mouse...)
        dwgParti[i].addRepulsionForce(mouseX, mouseY, 40, 0.4);
    }
    
    for (int i = 0; i < dwgParti.size(); i++){
        dwgParti[i].addFlockingForce();
        dwgParti[i].addDampingForce();
        dwgParti[i].update();
    }
    
    
    // wrap torroidally.
    for (int i = 0; i < dwgParti.size(); i++){
        ofVec2f pos = dwgParti[i].pos;
        if (pos.x < 0) pos.x = ofGetWidth();
        if (pos.x > ofGetWidth()) pos.x = 0;
        if (pos.y < 0) pos.y = ofGetHeight();
        if (pos.y > ofGetHeight()) pos.y = 0;
        dwgParti[i].pos = pos;
    }
    
    for (int i = 0; i < dwgParti.size(); i++){
        dwgParti[i].seperation.distance = smoothedVol*1000 ;
        dwgParti[i].seperation.strength = 23;
        dwgParti[i].damping = 0.13;
        
        dwgParti[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
        
    }
}
    
    
    
    // number == 15 == Launch +++++++++++++++++++++++++++++++++++++
    
    if(number == 15) {
        
   
    for (int i = 0; i < dwgParti.size(); i++){
        dwgParti[i].resetForce();
    }
    
    for (int i = 0; i < dwgParti.size(); i++){
        for (int j = 0; j < dwgParti.size(); j++){
            if (i != j){
                dwgParti[i].addForFlocking(dwgParti[j]);
            }
        }
        
        
        // this was something we tried in class (partitcle move away from mouse...)
        dwgParti[i].addRepulsionForce(mouseX, mouseY, 40, 0.4);
    }
    
    for (int i = 0; i < dwgParti.size(); i++){
        
        
        dwgParti[i].addFlockingForce();
        dwgParti[i].cohesion.strength = smoothedVol*100;
        dwgParti[i].damping = 0.05;
        dwgParti[i].addDampingForce();
        dwgParti[i].update();
    }
    
    
    // wrap torroidally.
    for (int i = 0; i < dwgParti.size(); i++){
        ofVec2f pos = dwgParti[i].pos;
        if (pos.x < 0) pos.x = ofGetWidth();
        if (pos.x > ofGetWidth()) pos.x = 0;
        if (pos.y < 0) pos.y = ofGetHeight();
        if (pos.y > ofGetHeight()) pos.y = 0;
        dwgParti[i].pos = pos;
    }
    
}
    
    
    // number == 17 == Launch +++++++++++++++++++++++++++++++++++++
    
    if(number == 17) {
        
        for (int i = 0; i < dwgParti.size(); i++){
            dwgParti[i].resetForce();
        }
        
        for (int i = 0; i < dwgParti.size(); i++){
            
            dwgParti[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
            dwgParti[i].c.set(ofRandom(100,255), ofRandom(100,255), ofRandom(100,255));
            
            for (int j = 0; j < i; j++){
                if (bRepel){
                    dwgParti[i].addRepulsionForce(dwgParti[j], radius+200, strength);
                } else {
                    dwgParti[i].addAttractionForce(dwgParti[j], radius, strength);
                }
            }
        }
        
        for (int i = 0; i < dwgParti.size(); i++){
            dwgParti[i].addDampingForce();
            dwgParti[i].update();
        }
        
    }
    
    

    

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
	//we clear the fbo if c is pressed.
	//this completely clears the buffer so you won't see any trails
	if( ofGetKeyPressed('0') ){
		ofClear(255,255,255, 0);
	}
    
    //1 - Fade Fbo
	
	//this is where we fade the fbo
	//by drawing a rectangle the size of the fbo with a small alpha value, we can slowly fade the current contents of the fbo.
  
    ofFill();
  
    ofColor dark(10,fadeAmnt);
    ofColor black(0, fadeAmnt);
    ofBackgroundGradient(dark, black);

     if(number== 1) {
         fadeAmnt = 100;
     }
    
     if(number== 2) {
         fadeAmnt = 60;
      }

    if(number== 3) {
        fadeAmnt = 30;
    }
    
    if(number== 4) {
        fadeAmnt = 5;
        
    }
    
    if(number== 5) {
        fadeAmnt = 10;
        
    }
    
    
    if(number== 6) {
        fadeAmnt = 20;
        
    }
    
    if(number== 7) {
        fadeAmnt = 0;
        
    }
    
    if(number== 8) {
        fadeAmnt = 20;
        
    }
    
    if(number== 9) {
        fadeAmnt = 20;
        
    }
    
    if(number== 10) {
        fadeAmnt = 5;
        
    }
    
    if(number== 12) {
        fadeAmnt = 100;
        
    }
    
    
      
    //	ofRect(0,0,ofGetScreenWidth(),ofGetScreenHeight());
    
	//2 - Draw graphics

	for (int i = 0; i < bgParti.size(); i++){
		bgParti[i]->draw();
	}
    
    
    for (int i = 0; i < dwgParti.size(); i++){

		dwgParti[i].draw();
	   
    }

 
}


//--------------------------------------------------------------
void testApp::draw(){ 
    
    ofSetColor(255, 50);
    rgbaFbo.draw(0,0);
    rgbaFboFloat.draw(0,0);
    
    ofDrawBitmapString(ofToString(fadeAmnt),0,100);    
    ofEnableBlendMode(blendMode);


//    string alphaInfo = "Current alpha fade amnt = " + ofToString(fadeAmnt);
//    
//	string reportString =
//    
//    "\n background = " + ofToString(smoothedVol)
//    
//    +
//    "\n default = growing "
//    +
//    "\n 1 = flows "
//    +
//    "\n 2 = funky "
//    +
//    "\n 3 = happy "
//    +
//    "\n 4 = colorful lines "
//    +
//    "\n 5 = groups "
//    +
//    "\n 6 = straight "
//    +
//    "\n 7 = combine ";
//    
//    
//    string reportString2 =
//    
//    "\n front visual  " + ofToString(smoothedVol)
//    +
//    "\n default = drawing volum (size) "
//    +
//    "\n q = rythem (white) "
//    +
//    "\n w = orchastral (expend)"
//    +
//    "\n e = electrical (tint + damping + red) "
//    +
//    "\n r=  funky (seperate)  "
//    +
//    "\n t = soul (drift) "
//    +
//    "\n y = stop / erase "
//    +
//    "\n u =  fill up screen";
    
    
//	ofSetColor(40,40,0);
//	ofRect(10,10,320,150);
//	ofSetColor(255,255,255);
//	ofDrawBitmapString(reportString, 30, 30);
//  ofDrawBitmapString(reportString2, 1100, 30);
   

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
    
    
    if( key == '1' ){
        number=1;
        
    }
    
    if( key == '2' ){
        
        number=2;
        
	}
    
    if( key == '3' ){
        number=3;
          
    }
    
    if( key == '4' ){
        number=4;
		
	}
    
    if( key == '5' ){
        
		number=5;
  	}
    
    
    if( key == '6' ){
		number=6;
        
	}
    
    
    if( key == '7' ){
		number=7;
        
	}

    if( key == '8' ){
        
		number=8;
  	}
    
    
    if( key == '9' ){
		number=9;
        
	}
    
    
    if( key == '0' ){
		number=0;
        
	}

 //-------------------- Front ---------------
    
    if( key == 'q' ){
		number = 11 ;
        
	}
    
    if( key == 'w' ){
		number= 12 ;
	}
    if( key == 'e' ){
		number= 13;
        
	}
    if( key == 'r' ){
		number= 14;
        
	}
    
    if( key == 't' ){
		number=15;
        
	}
    if( key == 'y' ){
		number=16;
        
	}
    if( key == 'u' ){
		number=17;
        
	}
   
    
  //-------------------- BlendMode ---------------
    if( key == 'z' ){
        number= 20;
        ofBackground(255,255,0,100); 
        blendMode = OF_BLENDMODE_DISABLED;
	}
    if( key == 'x' ){
        number= 21;
      
		blendMode = OF_BLENDMODE_ADD;
        
	}
    if( key == 'c' ){
        number= 22;
        blendMode = OF_BLENDMODE_SCREEN;

	}
    if( key == 'v' ){
        number= 23;
		blendMode = OF_BLENDMODE_SUBTRACT;
        
	}
    if( key == 'b' ){
        number= 24;
		blendMode = OF_BLENDMODE_ALPHA;
        
	}
    if( key == 'n' ){
        number= 25;
        
		blendMode = OF_BLENDMODE_MULTIPLY;
        
	}

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
  
    dwgParti.erase(dwgParti.begin());
	particle myParticle;
	myParticle.setInitialCondition(x,y,0,0);
	dwgParti.push_back(myParticle);
    

    
    if (number == 11){
        for (int i = 0; i < dwgParti.size(); i++){
            dwgParti[i].c.set(255,ofRandom(100,250),255);
        }
        
    }
    
    if (number == 12){
        for (int i = 0; i < dwgParti.size(); i++){
            dwgParti[i].c.set(0,ofRandom(100,255),ofRandom(100,255),90);
            
        }
    }
    
    if (number == 13){
        for (int i = 0; i < dwgParti.size(); i++){
            dwgParti[i].c.set(smoothedVol*1000+200,ofRandom(150),ofRandom(150), smoothedVol*10000);
        }
    }
    
    if (number == 14){
        for (int i = 0; i < dwgParti.size(); i++){
            dwgParti[i].c.set(ofRandom(100,255),255 ,255,90);
        }
        
    }
    
    if (number == 15){
        for (int i = 0; i < dwgParti.size(); i++){
            dwgParti[i].c.set(255, ofRandom(100,255), 255,90);
        }
        
    }
    if (number == 16){
        for (int i = 0; i < dwgParti.size(); i++){
            dwgParti[i].c.set(255, 0, ofRandom(100,255),90);
        }
        
    }
    
    if (number == 17){
        for (int i = 0; i < dwgParti.size(); i++){
            dwgParti[i].c.set(ofRandom(0,100),ofRandom(0,100), 0,90);
        }
        
    }
    

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}