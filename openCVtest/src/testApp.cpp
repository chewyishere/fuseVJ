#include "testApp.h"

// comparison routine for sort...
bool comparisonFunction(  particle * a, particle * b ) {
	return a->pos.x < b->pos.x;
}

//--------------------------------------------------------------
void testApp::setup(){

	// setup video grabber:
    
    video.setDeviceID(1);
	video.initGrabber(600, 480);
    
	//video.videoSettings();
	// get the width and height, and allocate color and grayscale images: 
	width = video.width; 
	height = video.height;
	
	videoColorCvImage.allocate(width, height);
	videoGrayscaleCvImage.allocate(width, height);
	
	videoPrevFrameImage.allocate(width, height);
	videoDiffImage.allocate(width, height);
	
	videoDiffMHI.allocate(width, height);
	
	// set background color to be white: 
	ofBackground(255, 255, 255);
	
	
	panel.setup("cv settings", 720, 0, 300, 748);
	panel.addPanel("control", 1, false);
	
	panel.setWhichPanel("control");
	panel.setWhichColumn(0);
	panel.addSlider("threshold ", "THRESHOLD", 127, 0, 255, true);
	
	panel.loadSettings("cvSettings.xml");
    
    //particles
  
    for (int i = 0; i < PARTICLE_NUM; i++){
		particle * myParticle = new particle;
        myParticle->setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
		particles.push_back(myParticle);
		
        
	}

}

//--------------------------------------------------------------
void testApp::update(){
		
	panel.update();
	
	
	int threshold			= panel.getValueI("THRESHOLD");

	
	video.update();
	
	if (video.isFrameNew()){
		
		videoColorCvImage.setFromPixels(video.getPixels(), width, height);
		videoGrayscaleCvImage = videoColorCvImage;
		
		
		videoDiffImage.absDiff(videoGrayscaleCvImage, videoPrevFrameImage);
		videoDiffImage.threshold(threshold);
		
		videoDiffMHI -= 2;
		videoDiffMHI += videoDiffImage;
		videoDiffMHI.blur(9);		 				
		videoPrevFrameImage = videoGrayscaleCvImage;
                                                        
	}
	
    
    int nPointsToCheck = 100;
    int nThresholdForNewTriangle = 230;
    
    unsigned char * pixels = videoDiffMHI.getPixels();
    
    for (int i = 0; i < nPointsToCheck; i++){
        
        int randomW = ofRandom(0, videoDiffMHI.getWidth()-1);
        int randomH = ofRandom(0, videoDiffMHI.getHeight()-1);
        
        if (pixels[ (int)(randomH * videoDiffMHI.getWidth() + randomW) ] > nThresholdForNewTriangle){
            
            float w = ofMap(randomW, 0, videoDiffMHI.getWidth(), 0, ofGetWidth());
            float h = ofMap(randomH, 0, videoDiffMHI.getHeight(), 0, ofGetHeight());
            
            particle * myParticle = new particle;
            myParticle->setInitialCondition(w,h,0,0);
            particles.push_back(myParticle);
            
            if (particles.size() > PARTICLE_NUM){
                particle * temp = particles[0];
                particles.erase(particles.begin());
            }
            
        }
        
    }

    
    // sort all the particles:
	sort( particles.begin(), particles.end(), comparisonFunction );
	
  	for (int i = 0; i < particles.size(); i++){
		particles[i]->resetForce();
	}
	
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
	
//    ofColor (255);
	//ofBackgroundGradient(dark, ofColor::black);
    
    ofSetColor(255);
    videoDiffMHI.draw(50, 50);
	videoGrayscaleCvImage.draw(20,20, 100,80);
	videoPrevFrameImage.draw(20, 120, 100, 80);
	videoDiffImage.draw(20,220,100,80);
    ofEnableAlphaBlending();
    panel.draw();
    
    for (int i = 0; i < particles.size(); i++){
		particles[i]->draw();
	}


}


//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	panel.mouseDragged(x,y,button);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	panel.mousePressed(x,y,button);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	panel.mouseReleased();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

