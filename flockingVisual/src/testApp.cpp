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
    
    //color setting
    col1 = (ofRandom(255),ofRandom(255),ofRandom(100));
    colPink = (255,ofRandom(100,250),255);
    
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

  // THE FLOWS ----------------------------
    
    for (int i = 0; i < PARTICLE_NUM; i++){
		particle * myParticle = new particle;
        myParticle->setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
		particles2.push_back(myParticle);
	}
	
 // THE INDIVIDUALS ----------------------------   
    
    for (int i = 0; i < 50; i++){
		myParticle.setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
        particles.push_back(myParticle);
		
	}
   	
	bRepel		= true;
	radius		= 40;
	strength	= 0.9f;
    
    
	CGDisplayHideCursor(kCGDirectMainDisplay);
    ofToggleFullscreen();
	ofSetVerticalSync(true);
	ofSetCircleResolution(80);
	ofBackground(0, 0, 0);
    
    
    number=0;
    
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
    
    volumeSmooth  = 0.0;



}

//--------------------------------------------------------------
void testApp::update(){
    
    //fbo
    
    rgbaFboFloat.begin();
    drawFboTest();
	rgbaFboFloat.end();
    
    // sort all the particles:
	sort( particles2.begin(), particles2.end(), comparisonFunction );
    
    for (int i = 0; i < particles2.size(); i++){
		particles2[i]->resetForce();
	}

    
    
    float fade = abs(sin( ofGetElapsedTimef()*0.099 ));
    
    for (int i = 0; i < particles2.size(); i++){
		for (int j = i-1; j >= 0; j--){
			if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >	50) break;
            if (i != j){
                particles2[i]->addForFlocking(*particles2[j]);
            }
		}
        particles2[i]->addRepulsionForce(mouseX, mouseY, 40, 0.4);
	}
    
	for (int i = 0; i < particles2.size(); i++){
		particles2[i]->addFlockingForce();
		particles2[i]->addDampingForce();
		particles2[i]->update();
	}
	
	
	// wrap torroidally.
	for (int i = 0; i < particles2.size(); i++){
		ofVec2f pos = particles2[i]->pos;
		if (pos.x < 0) pos.x = ofGetWidth();
		if (pos.x > ofGetWidth()) pos.x = 0;
		if (pos.y < 0) pos.y = ofGetHeight();
		if (pos.y > ofGetHeight()) pos.y = 0;
		particles2[i]->pos = pos;
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
        for (int i = 0; i < particles.size(); i++){
            // check for mouse moved message
            if(m.getAddress() == "/accel/x"){
                particles2[i]->r3 = m.getArgAsFloat(0);
                particles2[i]->vel.x += m.getArgAsFloat(0);
                cout << m.getArgAsFloat(0) << endl;
            }
            
            if(m.getAddress() == "/accel/y"){
                particles2[i]->r2 = m.getArgAsFloat(0);
                particles2[i]->vel.y += m.getArgAsFloat(0);
            }
            
            if(m.getAddress() == "/accel/z"){
                particles2[i]->r1 = m.getArgAsFloat(0);
                particles2[i]->vel.z += m.getArgAsFloat(0);
            }
            
            
            if(m.getAddress() == "/hue"){
                particles2[i]->c.setHue(m.getArgAsFloat(0));
            }
            
            if(m.getAddress() == "/sat"){
                particles2[i]->c.setSaturation(m.getArgAsFloat(0));
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
    ofSetColor( ofMap( fade , 0.0f, 0.5f, 0.0f, 1.0f, true) * 255,255);
    font.drawString(message,
                    width*0.5f - font.getStringBoundingBox(message,0,0).width*0.5f,
                    height*0.5f);
    
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
        for (int i = 0; i < particles.size(); i++){
            // check for mouse moved message
            if(m.getAddress() == "/accel/x"){
                particles[i].r3 = m.getArgAsFloat(0);
                particles[i].vel.x += m.getArgAsFloat(0);
                cout << m.getArgAsFloat(0) << endl;
            }
            
            if(m.getAddress() == "/accel/y"){
                particles[i].r2 = m.getArgAsFloat(0);
                particles[i].vel.y += m.getArgAsFloat(0);
            }
            
            if(m.getAddress() == "/accel/z"){
                particles[i].r1 = m.getArgAsFloat(0);
                particles[i].vel.z += m.getArgAsFloat(0);
            }
            
            
            if(m.getAddress() == "/hue"){
                particles[i].c.setHue(m.getArgAsFloat(0));
            }
            
            if(m.getAddress() == "/sat"){
                particles[i].c.setSaturation(m.getArgAsFloat(0));
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
        
        // background particles
        
        sort( particles2.begin(), particles2.end(), comparisonFunction );
        
        for (int i = 0; i < particles2.size(); i++){
            particles2[i]->cohesion.strength = 0.1;
            particles2[i]->cohesion.distance = 50;
            particles2[i]->alignment.strength =  0.7;
            particles2[i]->alignment.distance = 42;
            particles2[i]->seperation.strength = 0.7;
            particles2[i]->seperation.distance = 38;
            particles2[i]->damping = 0.08;            
        }
        
        
        for (int i = 0; i < particles2.size(); i++){
            particles2[i]->resetForce();
        }
        
        
        for (int i = 0; i < particles2.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >	50) break;
                if (i != j){
                    particles2[i]->addForFlocking(*particles2[j]);
                }
            }
        }

//            for (int i = 0; i < particles2.size(); i++){
//                particles2[i]->c.set(ofRandom(100,255),0,ofRandom(0,50),50);
//        }
//        
//        
//        
        for (int i = 0; i < particles2.size(); i++){
            particles2[i]->addFlockingForce();
            particles2[i]->addDampingForce();
            particles2[i]->update();
        }
        
        // wrap torroidally.
        for (int i = 0; i < particles2.size(); i++){
            ofVec2f pos = particles2[i]->pos;
            if (pos.x < 0) pos.x = ofGetWidth();
            if (pos.x > ofGetWidth()) pos.x = 0;
            if (pos.y < 0) pos.y = ofGetHeight();
            if (pos.y > ofGetHeight()) pos.y = 0;
            particles2[i]->pos = pos;
        }

    };
    
    
    
    // number == 2 addAttractionForce to Mouse +++++++++++++++++++++++++++++++++++++
    
    if(number==2)  {
        
        
        // background particles
        
        sort( particles2.begin(), particles2.end(), comparisonFunction );
        
        for (int i = 0; i < particles2.size(); i++){
            particles2[i]->cohesion.strength = 0.76;
            particles2[i]->cohesion.distance = 50;
            particles2[i]->alignment.strength =  0.5;
            particles2[i]->alignment.distance = 130;
            particles2[i]->seperation.strength = 0.4;
            particles2[i]->seperation.distance = 157;
            particles2[i]->damping = 0.192;
        }
        
        
        for (int i = 0; i < particles2.size(); i++){
            particles2[i]->resetForce();
        }
        
        
        for (int i = 0; i < particles2.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >	50) break;
                if (i != j){
                    particles2[i]->addForFlocking(*particles2[j]);
                }
            }
        }
        
        
        
        for (int i = 0; i < particles2.size(); i++){
            particles2[i]->addFlockingForce();
            particles2[i]->addDampingForce();
            particles2[i]->update();
        }
        
        
        // wrap torroidally.
        for (int i = 0; i < particles2.size(); i++){
            ofVec2f pos = particles2[i]->pos;
            if (pos.x < 0) pos.x = ofGetWidth();
            if (pos.x > ofGetWidth()) pos.x = 0;
            if (pos.y < 0) pos.y = ofGetHeight();
            if (pos.y > ofGetHeight()) pos.y = 0;
            particles2[i]->pos = pos;
        }
    }
    
    
    // number == 3  addRepulsionForce to Mouse  +++++++++++++++++++++++++++++++++++++
    
    if(number==3)  {
        
        // background particles
        
        sort( particles2.begin(), particles2.end(), comparisonFunction );
        
        for (int i = 0; i < particles2.size(); i++){
            particles2[i]->cohesion.strength = 0.2;
            particles2[i]->cohesion.distance = 200;
            particles2[i]->alignment.strength =  0.24;
            particles2[i]->alignment.distance = 198;
            particles2[i]->seperation.strength = 0.4;
            particles2[i]->seperation.distance = 33;
            particles2[i]->damping = 0.029;
        }
        
        
        for (int i = 0; i < particles2.size(); i++){
            particles2[i]->resetForce();
        }
        
        
        for (int i = 0; i < particles2.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >	50) break;
                if (i != j){
                    particles2[i]->addForFlocking(*particles2[j]);
                }
            }
        }
        
        
        
        for (int i = 0; i < particles2.size(); i++){
            particles2[i]->addFlockingForce();
            particles2[i]->addDampingForce();
            particles2[i]->update();
        }
        
        
        // wrap torroidally.
        for (int i = 0; i < particles2.size(); i++){
            ofVec2f pos = particles2[i]->pos;
            if (pos.x < 0) pos.x = ofGetWidth();
            if (pos.x > ofGetWidth()) pos.x = 0;
            if (pos.y < 0) pos.y = ofGetHeight();
            if (pos.y > ofGetHeight()) pos.y = 0;
            particles2[i]->pos = pos;
        }
        
        
        
         }
    // number == 4  seperation.distance +++++++++++++++++++++++++++++++++++++
    
    if(number == 4) {
        
        // background particles
        
        sort( particles2.begin(), particles2.end(), comparisonFunction );
        
        for (int i = 0; i < particles2.size(); i++){
            particles2[i]->cohesion.strength = 0.19;
            particles2[i]->cohesion.distance = 200;
            particles2[i]->alignment.strength =  0.2;
            particles2[i]->alignment.distance = 175;
            particles2[i]->seperation.strength = 0.46;
            particles2[i]->seperation.distance = 200;
            particles2[i]->damping = 0.45;
        }
        
        
        for (int i = 0; i < particles2.size(); i++){
            particles2[i]->resetForce();
        }
        
        
        for (int i = 0; i < particles2.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >	50) break;
                if (i != j){
                    particles2[i]->addForFlocking(*particles2[j]);
                }
            }
        }
        
        
        
        for (int i = 0; i < particles2.size(); i++){
            particles2[i]->addFlockingForce();
            particles2[i]->addDampingForce();
            particles2[i]->update();
        }
        
        
        // wrap torroidally.
        for (int i = 0; i < particles2.size(); i++){
            ofVec2f pos = particles2[i]->pos;
            if (pos.x < 0) pos.x = ofGetWidth();
            if (pos.x > ofGetWidth()) pos.x = 0;
            if (pos.y < 0) pos.y = ofGetHeight();
            if (pos.y > ofGetHeight()) pos.y = 0;
            particles2[i]->pos = pos;
        }
        
        
         }
    
    
    
    // number == 5  +++++++++++++++++++++++++++++++++++++
    
    if(number == 5) {
        // background particles
        
        sort( particles2.begin(), particles2.end(), comparisonFunction );
        
        for (int i = 0; i < particles2.size(); i++){
            particles2[i]->cohesion.strength = 0.5;
            particles2[i]->cohesion.distance = 100;
            particles2[i]->alignment.strength =  0.38;
            particles2[i]->alignment.distance = 52;
            particles2[i]->seperation.strength = 0.9;
            particles2[i]->seperation.distance = 15;
            particles2[i]->damping = 0.68;
        }
        
        
        for (int i = 0; i < particles2.size(); i++){
            particles2[i]->resetForce();
        }
        
        
        for (int i = 0; i < particles2.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >	50) break;
                if (i != j){
                    particles2[i]->addForFlocking(*particles2[j]);
                }
            }
        }
        
        
        
        for (int i = 0; i < particles2.size(); i++){
            particles2[i]->addFlockingForce();
            particles2[i]->addDampingForce();
            particles2[i]->update();
        }
        
        
        // wrap torroidally.
        for (int i = 0; i < particles2.size(); i++){
            ofVec2f pos = particles2[i]->pos;
            if (pos.x < 0) pos.x = ofGetWidth();
            if (pos.x > ofGetWidth()) pos.x = 0;
            if (pos.y < 0) pos.y = ofGetHeight();
            if (pos.y > ofGetHeight()) pos.y = 0;
            particles2[i]->pos = pos;
        }
        
        
      }
    
    
// number == 6 repulsion = sound +++++++++++++++++++++++++++++++++++++
    
    
        if(number == 6) {
            // background particles
            
            sort( particles2.begin(), particles2.end(), comparisonFunction );
            
            for (int i = 0; i < particles2.size(); i++){
                particles2[i]->cohesion.strength = 0.20;
                particles2[i]->cohesion.distance = 10;
                particles2[i]->alignment.strength =  0.31;
                particles2[i]->alignment.distance = 200;
                particles2[i]->seperation.strength = 0.4;
                particles2[i]->seperation.distance = 2;
                particles2[i]->damping = 0.20;
            }
            
            
            for (int i = 0; i < particles2.size(); i++){
                particles2[i]->resetForce();
            }
            
            
            for (int i = 0; i < particles2.size(); i++){
                for (int j = i-1; j >= 0; j--){
                    if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >50) break;
                    if (i != j){
                        particles2[i]->addForFlocking(*particles2[j]);
                    }
                }
            }
            
            
            
            for (int i = 0; i < particles2.size(); i++){
                particles2[i]->addFlockingForce();
                particles2[i]->addDampingForce();
                particles2[i]->update();
            }
            
            
            // wrap torroidally.
            for (int i = 0; i < particles2.size(); i++){
                ofVec2f pos = particles2[i]->pos;
                if (pos.x < 0) pos.x = ofGetWidth();
                if (pos.x > ofGetWidth()) pos.x = 0;
                if (pos.y < 0) pos.y = ofGetHeight();
                if (pos.y > ofGetHeight()) pos.y = 0;
                particles2[i]->pos = pos;
            }
            
            
        }
    
    
    
    
    // number == 7 repulsion = sound +++++++++++++++++++++++++++++++++++++
    
    if(number == 7) {
        
            // background particles
            
            sort( particles2.begin(), particles2.end(), comparisonFunction );
            
            for (int i = 0; i < particles2.size(); i++){
                particles2[i]->cohesion.strength = 0.20;
                particles2[i]->cohesion.distance = 100;
                particles2[i]->alignment.strength =  0.31;
                particles2[i]->alignment.distance = 20;
                particles2[i]->seperation.strength = 0.4;
                particles2[i]->seperation.distance = 2;
                particles2[i]->damping = 0.8;
            }
            
            
            for (int i = 0; i < particles2.size(); i++){
                particles2[i]->resetForce();
            }
            
            
            for (int i = 0; i < particles2.size(); i++){
                for (int j = i-1; j >= 0; j--){
                    if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >50) break;
                    if (i != j){
                        particles2[i]->addForFlocking(*particles2[j]);
                    }
                }
            }
            
            
            
            for (int i = 0; i < particles2.size(); i++){
                particles2[i]->addFlockingForce();
                particles2[i]->addDampingForce();
                particles2[i]->update();
            }
            
            
            // wrap torroidally.
            for (int i = 0; i < particles2.size(); i++){
                ofVec2f pos = particles2[i]->pos;
                if (pos.x < 0) pos.x = ofGetWidth();
                if (pos.x > ofGetWidth()) pos.x = 0;
                if (pos.y < 0) pos.y = ofGetHeight();
                if (pos.y > ofGetHeight()) pos.y = 0;
                particles2[i]->pos = pos;
            }
            
            
        }
        
    
    
       // number == 11 == draw +++++++++++++++++++++++++++++++++++++

    // font particles
    
    if(number == 11) {

    
    for (int i = 0; i < particles.size(); i++){
        particles[i].resetForce();
    }
    
    for (int i = 0; i < particles.size(); i++){
        
        particles[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
        particles[i].alignment.strength = scaledVol*2000;
        particles[i].c.set(colPink);
        
        
        for (int j = 0; j < i; j++){
            if (bRepel){
                particles[i].addRepulsionForce(particles[j], radius, strength);
            } else {
                particles[i].addAttractionForce(particles[j], radius, strength);
            }
        }
    }
    
    for (int i = 0; i < particles.size(); i++){
        particles[i].addDampingForce();
        particles[i].update();
    }
    

}

    
// number == 12 == draw +++++++++++++++++++++++++++++++++++++
    
    // font particles
    
    if(number == 12) {

    
    // font particles
    
    for (int i = 0; i < particles.size(); i++){
        particles[i].resetForce();
    }
    
    
    for (int i = 0; i < particles.size(); i++){
        
        particles[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
        // particles[i].scl.x = 500;
        // particles[i].scl.y = 500;
        
        for (int j = 0; j < i; j++){
            if (bRepel){
                particles[i].addRepulsionForce(particles[j], radius + smoothedVol * 3000, strength);
            } else {
                particles[i].addAttractionForce(particles[j], radius , strength);
            }
        }
    }
    
    for (int i = 0; i < particles.size(); i++){
        particles[i].addDampingForce();
        particles[i].damping = 0.1;
        particles[i].update();
    }
    }
    
    
// number == 13 == draw +++++++++++++++++++++++++++++++++++++
    
    // font particles
    
    if(number == 13) {
  
    
    // font particles 
    
    for (int i = 0; i < particles.size(); i++){
        particles[i].resetForce();
    }
    
    for (int i = 0; i < particles.size(); i++){
        
        for (int j = 0; j < particles.size(); j++){
            if (i != j){
                particles[i].addForFlocking(particles[j]);
            }
        }
        
    }
    
    
    
    for (int i = 0; i < particles.size(); i++){
        
        particles[i].damping = smoothedVol*200;
    }
    
    for (int i = 0; i < particles.size(); i++){
        
        particles[i].addFlockingForce();
        particles[i].addDampingForce();
        particles[i].update();
    }
    
    
    // wrap torroidally.
    for (int i = 0; i < particles.size(); i++){
        ofVec2f pos = particles[i].pos;
        if (pos.x < 0) pos.x = ofGetWidth();
        if (pos.x > ofGetWidth()) pos.x = 0;
        if (pos.y < 0) pos.y = ofGetHeight();
        if (pos.y > ofGetHeight()) pos.y = 0;
        particles[i].pos = pos;
    }
    

}
    
   // number == 14 == draw +++++++++++++++++++++++++++++++++++++
    
    // font particles
    
    if(number == 14) {
        
    for (int i = 0; i < particles.size(); i++){
        particles[i].resetForce();
    }
    
    
    for (int i = 0; i < particles.size(); i++){
        for (int j = 0; j < particles.size(); j++){
            if (i != j){
                particles[i].addForFlocking(particles[j]);
            }
        }
        
        // this was something we tried in class (partitcle move away from mouse...)
        particles[i].addRepulsionForce(mouseX, mouseY, 40, 0.4);
    }
    
    for (int i = 0; i < particles.size(); i++){
        particles[i].addFlockingForce();
        particles[i].addDampingForce();
        particles[i].update();
    }
    
    
    // wrap torroidally.
    for (int i = 0; i < particles.size(); i++){
        ofVec2f pos = particles[i].pos;
        if (pos.x < 0) pos.x = ofGetWidth();
        if (pos.x > ofGetWidth()) pos.x = 0;
        if (pos.y < 0) pos.y = ofGetHeight();
        if (pos.y > ofGetHeight()) pos.y = 0;
        particles[i].pos = pos;
    }
    
    for (int i = 0; i < particles.size(); i++){
        particles[i].seperation.distance = smoothedVol*1000 ;
        particles[i].seperation.strength = 23;
        particles[i].damping = 0.13;
        
        particles[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
        
    }
}
    
    
    
    // number == 15 == Launch +++++++++++++++++++++++++++++++++++++
    
    if(number == 15) {
        
   
    for (int i = 0; i < particles.size(); i++){
        particles[i].resetForce();
    }
    
    for (int i = 0; i < particles.size(); i++){
        for (int j = 0; j < particles.size(); j++){
            if (i != j){
                particles[i].addForFlocking(particles[j]);
            }
        }
        
        
        // this was something we tried in class (partitcle move away from mouse...)
        particles[i].addRepulsionForce(mouseX, mouseY, 40, 0.4);
    }
    
    for (int i = 0; i < particles.size(); i++){
        
        
        particles[i].addFlockingForce();
        particles[i].cohesion.strength = smoothedVol*100;
        particles[i].damping = 0.05;
        particles[i].addDampingForce();
        particles[i].update();
    }
    
    
    // wrap torroidally.
    for (int i = 0; i < particles.size(); i++){
        ofVec2f pos = particles[i].pos;
        if (pos.x < 0) pos.x = ofGetWidth();
        if (pos.x > ofGetWidth()) pos.x = 0;
        if (pos.y < 0) pos.y = ofGetHeight();
        if (pos.y > ofGetHeight()) pos.y = 0;
        particles[i].pos = pos;
    }
    
}
    
    
    // number == 17 == Launch +++++++++++++++++++++++++++++++++++++
    
    if(number == 17) {
        
        for (int i = 0; i < particles.size(); i++){
            particles[i].resetForce();
        }
        
        for (int i = 0; i < particles.size(); i++){
            
            particles[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
            particles[i].c.set(ofRandom(100,255), ofRandom(100,255), ofRandom(100,255));
            
            for (int j = 0; j < i; j++){
                if (bRepel){
                    particles[i].addRepulsionForce(particles[j], radius+200, strength);
                } else {
                    particles[i].addAttractionForce(particles[j], radius, strength);
                }
            }
        }
        
        for (int i = 0; i < particles.size(); i++){
            particles[i].addDampingForce();
            particles[i].update();
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
	if( ofGetKeyPressed('c') ){
		ofClear(255,255,255, 0);
	}
	
	//some different alpha values for fading the fbo
	//the lower the number, the longer the trails will take to fade away.
    //	fadeAmnt = 40;
    //	if(ofGetKeyPressed('1')){
    //		fadeAmnt = 1;
    //	}else if(ofGetKeyPressed('2')){
    //		fadeAmnt = 5;
    //	}else if(ofGetKeyPressed('3')){
    //		fadeAmnt = 15;
    //	}
    
	//1 - Fade Fbo
	
	//this is where we fade the fbo
	//by drawing a rectangle the size of the fbo with a small alpha value, we can slowly fade the current contents of the fbo.
	ofFill();
    
   ofColor dark(10,fadeAmnt);
   ofColor black(0, fadeAmnt);
    
   ofBackgroundGradient(dark, black);
    
     if(number== 1) {
         fadeAmnt = 100;
         ofSetColor(255,255,255, fadeAmnt);
         
     }
    
     if(number== 2) {
         fadeAmnt = 60;
         ofSetColor(255,0,0, fadeAmnt);
      }

    if(number== 3) {
        fadeAmnt = 30;
        ofSetColor(0,255,255, fadeAmnt);
    }
    
    if(number== 4) {
        fadeAmnt = 5;
        ofSetColor(10,10,10, fadeAmnt);
        
    }
    
    if(number== 5) {
        fadeAmnt = 10;
        ofSetColor(10,255,10, fadeAmnt);
        
    }
    
    
    if(number== 6) {
        fadeAmnt = 20;
        ofSetColor(10,00,25, fadeAmnt);
        
    }
    
    //	ofRect(0,0,ofGetScreenWidth(),ofGetScreenHeight());
    
	//2 - Draw graphics
	
	for (int i = 0; i < particles2.size(); i++){
		particles2[i]->draw();
	}
    
}


//--------------------------------------------------------------
void testApp::draw(){ 
    
    
  //  ofEnableBlendMode(OF_BLENDMODE_ALPHA);
  //ofBackgroundGradient(ofColor(252,239,233,255), ofColor(58,30,0,255));
    
    ofSetColor(255, 50);
     
    //ofEnableAlphaBlending();
    
        rgbaFbo.draw(0,0);
        rgbaFboFloat.draw(0,0);
    
   
      for (int i = 0; i < particles.size(); i++){
		particles[i].draw();
	}
    
    
    string alphaInfo = "Current alpha fade amnt = " + ofToString(fadeAmnt);
    
	string reportString =
    
    "\n background = " + ofToString(smoothedVol)
    
    +
    "\n default = growing "
    +
    "\n 1 = flows "
    +
    "\n 2 = funky "
    +
    "\n 3 = happy "
    +
    "\n 4 = colorful lines "
    +
    "\n 5 = groups "
    +
    "\n 6 = straight "
    +
    "\n 7 = combine ";
    
    
    string reportString2 =
    
    "\n front visual  " + ofToString(smoothedVol)
    +
    "\n default = drawing volum (size) "
    +
    "\n q = rythem (white) "
    +
    "\n w = orchastral (expend)"
    +
    "\n e = electrical (tint + damping + red) "
    +
    "\n r=  funky (seperate)  "
    +
    "\n t = soul (drift) "
    +
    "\n y = stop / erase "
    +
    "\n u =  fill up screen";
     
    
	ofSetColor(40,40,0);
	ofRect(10,10,320,150);
	ofSetColor(255,255,255);
	ofDrawBitmapString(reportString, 30, 30);
    ofDrawBitmapString(reportString2, 1100, 30);
    ofDrawBitmapString(alphaInfo, ofPoint(10,430));


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
    

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
  
    particles.erase(particles.begin());
	particle myParticle;
	myParticle.setInitialCondition(x,y,0,0);
	particles.push_back(myParticle);
    

    
    if (number == 11){
        for (int i = 0; i < particles.size(); i++){
            particles[i].c.set(colPink);
        }
        
    }
    
    if (number == 12){
        for (int i = 0; i < particles.size(); i++){
            particles[i].c.set(0,ofRandom(100,255),ofRandom(100,255),90);
            
        }
    }
    
    if (number == 13){
        for (int i = 0; i < particles.size(); i++){
            particles[i].c.set(smoothedVol*1000+200,ofRandom(150),ofRandom(150), smoothedVol*10000);
        }
    }
    
    if (number == 14){
        for (int i = 0; i < particles.size(); i++){
            particles[i].c.set(ofRandom(100,255),255 ,255,90);
        }
        
    }
    
    if (number == 15){
        for (int i = 0; i < particles.size(); i++){
            particles[i].c.set(255, ofRandom(100,255), 255,90);
        }
        
    }
    if (number == 16){
        for (int i = 0; i < particles.size(); i++){
            particles[i].c.set(255, 0, ofRandom(100,255),90);
        }
        
    }
    
    if (number == 17){
        for (int i = 0; i < particles.size(); i++){
            particles[i].c.set(ofRandom(0,100),ofRandom(0,100), 0,90);
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