#include "testApp.h"


// comparison routine for sort...
bool comparisonFunction(  particle * a, particle * b ) {
	return a->pos.x < b->pos.x;
}

//--------------------------------------------------------------
void testApp::setup(){	
	
	
	ofBackground(0, 0, 0);
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
    ofEnableAlphaBlending();
	
    rgbaFbo.allocate(ofGetScreenWidth(), ofGetScreenHeight(), GL_RGBA);
    rgbaFboFloat.allocate(ofGetScreenWidth(), ofGetScreenHeight(), GL_RGBA32F_ARB);
	
    rgbaFbo.begin();
	ofClear(255,255,255, 0);
    rgbaFbo.end();
    
    rgbaFboFloat.begin();
	ofClear(255,255,255, 0);
    rgbaFboFloat.end();
    
    fadeAmnt=10;
    
    
	for (int i = 0; i < PARTICLE_NUM; i++){
		particle * myParticle = new particle;
        myParticle->setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
		particles.push_back(myParticle);
	}
	
    
	panel.setup("control", 770, 0, 300, 400);
	panel.addPanel("flocking", 1, false);
	panel.setWhichPanel("flocking");
	panel.setWhichColumn(0);
	panel.addSlider("cohesion strength", "COHESION_STRENGTH", 0.01,0,1, false);
	panel.addSlider("cohesion radius", "COHESION_RADIUS", 50,0,200, false);
	panel.addSlider("alignment strength", "ALIGNMENT_STRENGTH", 0.01,0,1, false);
	panel.addSlider("alignment radius", "ALIGNMENT_RADIUS",  50,0,200, false);
	panel.addSlider("seperation strength", "SEPERATION_STRENGTH", 0.01,0, 1, false);
	panel.addSlider("seperation radius", "SEPERATION_RADIUS",  50,0,200, false);
	
	panel.addSlider("damping", "DAMPING", 0.01, 0.001, 0.5, false);
	
	panel.loadSettings("settings.xml");
}

//--------------------------------------------------------------
void testApp::update(){

    
	panel.update();
	
    
    rgbaFboFloat.begin();
    drawFboTest();
	rgbaFboFloat.end();
    
    
    // sort all the particles:
	sort( particles.begin(), particles.end(), comparisonFunction );
	
    for (int i = 0; i < particles.size(); i++){
		particles[i]->cohesion.distance = panel.getValueF("COHESION_RADIUS");
		particles[i]->alignment.distance = panel.getValueF("ALIGNMENT_RADIUS");
		particles[i]->seperation.distance = panel.getValueF("SEPERATION_RADIUS");
		
		particles[i]->cohesion.strength = panel.getValueF("COHESION_STRENGTH");
		particles[i]->alignment.strength = panel.getValueF("ALIGNMENT_STRENGTH");
		particles[i]->seperation.strength = panel.getValueF("SEPERATION_STRENGTH");
		
		particles[i]->damping = panel.getValueF("DAMPING");
		
	}
	
	for (int i = 0; i < particles.size(); i++){
		particles[i]->resetForce();
	}
	
//	for (int i = 0; i < particles.size(); i++){
//		for (int j = 0; j < particles.size(); j++){
//			if (i != j){
//				particles[i]->addForFlocking(*particles[j]);
//			}
//		}
//		particles[i]->addRepulsionForce(mouseX, mouseY, 40, 0.4);
//	}
	
    for (int i = 0; i < particles.size(); i++){
		for (int j = i-1; j >= 0; j--){
			if ( fabs(particles[j]->pos.x - particles[i]->pos.x) >	50) break;
                if (i != j){
                    particles[i]->addForFlocking(*particles[j]);
                }
		}
        particles[i]->addRepulsionForce(mouseX, mouseY, 40, 0.4);
	}
    
	for (int i = 0; i < particles.size(); i++){
		particles[i]->addFlockingForce();
		particles[i]->addDampingForce();
		particles[i]->update();
	}
	
	
	// wrap torroidally.
	for (int i = 0; i < particles.size(); i++){
		ofVec2f pos = particles[i]->pos;
		if (pos.x < 0) pos.x = ofGetWidth();
		if (pos.x > ofGetWidth()) pos.x = 0;
		if (pos.y < 0) pos.y = ofGetHeight();
		if (pos.y > ofGetHeight()) pos.y = 0;
		particles[i]->pos = pos;
	}

}

//--------------------------------------------------------------
void testApp::draw(){
	
    
    ofSetColor(255,255,255);
    rgbaFbo.draw(0,0);
    rgbaFboFloat.draw(0,0);
    
    
//    ofDrawBitmapString("non floating point FBO", ofPoint(10,20));
//    ofDrawBitmapString("floating point FBO", ofPoint(420,20));
//    
//	string alphaInfo = "Current alpha fade amnt = " + ofToString(fadeAmnt);
//	alphaInfo += "\nHold '1' to set alpha fade to 1";
//	alphaInfo += "\nHold '2' to set alpha fade to 5";
//	alphaInfo += "\nHold '3' to set alpha fade to 15";
//	alphaInfo += "\nHold 'c' to clear the fbo each frame\n\nMove mouse to draw with a circle";
//	
//    ofDrawBitmapString(alphaInfo, ofPoint(10,430));
    
    
    
    
	panel.draw();
    
    
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
    
    ofColor dark(40,fadeAmnt);
    ofColor black(0, fadeAmnt);
	ofBackgroundGradient(dark, black);
    
//	ofSetColor(255,255,255, fadeAmnt);
//	ofRect(0,0,ofGetScreenWidth(),ofGetScreenHeight());
    
	//2 - Draw graphics
	
	
	for (int i = 0; i < particles.size(); i++){
		particles[i]->draw();
	}
    
    
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	
	switch (key){
			
		case ' ':
			// reposition everything: 
			for (int i = 0; i < particles.size(); i++){
				particles[i]->setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
			}
			break;
        case 'f': ofToggleFullscreen();
	}
	
	
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){ 
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
//	if (x < 500){
//		particles.erase(particles.begin());
//		particle * myParticle;
//		float randomAngle = ofRandom(0, TWO_PI);
//		myParticle->setInitialCondition(x,y,cos(randomAngle),sin(randomAngle));
//		particles.push_back(myParticle);
//	}
    
    
    
	panel.mouseDragged(x,y,button);
    
    fadeAmnt = ofMap(x, 0, ofGetWidth(),0, 40);
	
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
    particles.erase(particles.begin());
    
    
	panel.mousePressed(x,y,button);
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(){
	
	panel.mouseReleased();
}
