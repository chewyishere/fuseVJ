#include "testApp.h"


// comparison routine for sort...
bool comparisonFunction(  particle * a, particle * b ) {
	return a->pos.x < b->pos.x;
}

//--------------------------------------------------------------
void testApp::setup(){	
	cout << "listening for osc messages on port " << PORT << "\n";
  //  sender.setup("127.0.0.1", 10001);
    receiver.setup(PORT);
    
	ofBackground(0, 0, 0);
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
	
	for (int i = 0; i < PARTICLE_NUM; i++){
		particle * myParticle = new particle;
        myParticle->setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
		particles.push_back(myParticle);
		
	}
	
	
	panel.setup("Mouse control", 770, 0, 300, 400);
	panel.addPanel("Disabled!!!, try Ableton Control!", 1, false);
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
    
    Abletonpanel.setup("Ableton Control", 400, 0, 330, 200);
    Abletonpanel.addPanel("Ableton Data", 1, false);
    Abletonpanel.setWhichPanel("Ableton Data = Flocking");
  

}

//--------------------------------------------------------------
void testApp::update(){

	panel.update();
	
    // sort all the particles:
	sort( particles.begin(), particles.end(), comparisonFunction );
	
    for (int i = 0; i < particles.size(); i++){
		//particles[i]->cohesion.distance = panel.getValueF("COHESION_RADIUS");
		//particles[i]->alignment.distance = panel.getValueF("ALIGNMENT_RADIUS");
		//particles[i]->seperation.distance = panel.getValueF("SEPERATION_RADIUS");		
		//particles[i]->cohesion.strength = panel.getValueF("COHESION_STRENGTH");
		//particles[i]->alignment.strength = panel.getValueF("ALIGNMENT_STRENGTH");
		//particles[i]->seperation.strength = panel.getValueF("SEPERATION_STRENGTH");
		//particles[i]->damping = panel.getValueF("DAMPING");
		
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

    
    //++++++++++ osc reciever ++++++++++++++
    
    
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
		if(m.getAddress() == "/midi/cc1/2"){
			// both the arguments are int32's
			Vdrum = m.getArgAsFloat(0);
           	particles[i]->cohesion.strength = Vdrum;
            
		}
        
        if(m.getAddress() == "/midi/cc2/2"){
			// both the arguments are int32's
			Vbase = m.getArgAsFloat(0);
            particles[i]->cohesion.distance= Edrum;
		}
        
        if(m.getAddress() == "/midi/cc3/2"){
			// both the arguments are int32's
			Vmelody = m.getArgAsFloat(0);
            particles[i]->alignment.strength = Vbase;
		}
        
        if(m.getAddress() == "/midi/cc1/1"){
			// both the arguments are int32's
			Edrum = m.getArgAsFloat(0);
            particles[i]->alignment.distance = Ebase;
		}
        
        if(m.getAddress() == "/midi/cc2/1"){
			// both the arguments are int32's
			Ebase = m.getArgAsFloat(0);
            particles[i]->seperation.strength = Vmelody;
		}
        
        if(m.getAddress() == "/midi/cc3/1"){
			// both the arguments are int32's
			Emelody = m.getArgAsFloat(0);
            particles[i]->seperation.distance = Emelody;
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
     
}

//--------------------------------------------------------------
void testApp::draw(){
	
    ofColor dark(40);
	ofBackgroundGradient(dark, ofColor::black);
	for (int i = 0; i < particles.size(); i++){
		particles[i]->draw();
	}
	panel.draw();
    Abletonpanel.draw();
        
    string buf;
    
	buf = "listening for osc messages on port" + ofToString(PORT);
	ofDrawBitmapString(buf, 10, 20);
    
	// draw fader
	buf = "Vdrum = COHESION_STRENGTH : " + ofToString(Vdrum);
	ofDrawBitmapString(buf, 430, 40);
    
    buf = "Edrum = COHESION_RADIUS : " + ofToString(Edrum);
    ofDrawBitmapString(buf, 430, 60);
    
    buf = "Vbase = ALIGNMENT_STRENGTH : " + ofToString(Vbase);
    ofDrawBitmapString(buf, 430, 80);
    
    buf = "Ebase = ALIGNMENT_RADIUS : " + ofToString(Ebase);
    ofDrawBitmapString(buf, 430, 100);
    
    buf = "Vmelody = SEPERATION_STRENGTH : " + ofToString(Vmelody);
    ofDrawBitmapString(buf, 430, 120);
    
    buf = "Emelody = SEPERATION_RADIUS : " + ofToString(Emelody);
    ofDrawBitmapString(buf, 430, 140);
    
	//for(int i = 0; i < NUM_MSG_STRINGS; i++){
	//	ofDrawBitmapString(msg_strings[i], 10, 40 + 15 * i);
	//}
    

    
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
