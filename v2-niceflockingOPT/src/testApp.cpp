#include "testApp.h"


// comparison routine for sort...
bool comparisonFunction(  particle * a, particle * b ) {
	return a->pos.x < b->pos.x;
}

//--------------------------------------------------------------
void testApp::setup(){	
	
	
	ofBackground(0, 0, 0);
	
	ofSetVerticalSync(true);
	ofSetFrameRate(24);
	
	
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
	
    ofColor dark(40);
	ofBackgroundGradient(dark, ofColor::black);
	for (int i = 0; i < particles.size(); i++){
		particles[i]->draw();
	}
	panel.draw();
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
	
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
	panel.mousePressed(x,y,button);
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(){
	
	panel.mouseReleased();
}
