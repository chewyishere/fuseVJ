#include "testApp.h"

// comparison routine for sort...
bool comparisonFunction(  particle * a, particle * b ) {
	return a->pos.x < b->pos.x;
}

// comparison routine for sort...
bool comparisonFunctionAge(  particle * a, particle * b ) {
	return a->age < b->age;
}

//--------------------------------------------------------------
void testApp::setup(){
    
    ofSetVerticalSync(true);
	ofSetFrameRate(60);
    ofEnableAlphaBlending();
    

	// setup video grabber:
    
    video.setDeviceID(0);
	video.initGrabber(1100, 700);
    
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
	//ofBackground(255, 255, 255);
	
	
	panel.setup("cv settings", 720, 0, 300, 748);
	panel.addPanel("control", 1, false);
	
	panel.setWhichPanel("control");
	panel.setWhichColumn(0);
	panel.addSlider("threshold ", "THRESHOLD", 127, 0, 255, true);
	
	panel.loadSettings("cvSettings.xml");
    
    // FBO
    
    rgbaFbo.allocate(ofGetScreenWidth(), ofGetScreenHeight(), GL_RGBA);
    rgbaFboFloat.allocate(ofGetScreenWidth(), ofGetScreenHeight(), GL_RGBA32F_ARB);
	
    rgbaFbo.begin();
	ofClear(255,255,255, 0);
    rgbaFbo.end();
    
//    rgbaFboFloat.begin();
//	ofClear(255,255,255, 0);
//    rgbaFboFloat.end();
//    
    fadeAmnt=50;
    
    
    //particles
    
    for (int i = 0; i < PARTICLE_NUM; i++){
		particle * myParticle = new particle;
        myParticle->setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
		particles.push_back(myParticle);
		
        
	}

}

//--------------------------------------------------------------
void testApp::update(){
    
    rgbaFboFloat.begin();
    drawFboTest();
	rgbaFboFloat.end();
    
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
    
 
   // sort all the particles:
    sort( particles.begin(), particles.end(), comparisonFunctionAge );
	
    
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
    
    float fade = abs(sin( ofGetElapsedTimef()*0.099 ));
    
    
    for (int i = 0; i < particles.size(); i++){
        for (int j = i-1; j >= 0; j--){
            if ( fabs(particles[j]->pos.x - particles[i]->pos.x) >	50) break;
            if (i != j){
                particles[i]->addForFlocking(*particles[j]);
            }
        }
        particles[i]->addRepulsionForce(mouseX,mouseY, 40, 0.4);
    }
    
    for (int i = 0; i < particles.size(); i++){
        particles[i]->addFlockingForce();
        particles[i]->addDampingForce();
        particles[i]->update();
    }

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

    
    
    // number == 1 +++++++++++++++++++++++++++++++++++++
    
    
    if(number==1) {
        
        
        //rgbaFboFloat.begin();
        //drawFboTest();
        //rgbaFboFloat.end();
        
        
        // sort all the particles
        
        
        sort( particles.begin(), particles.end(), comparisonFunction );

        float fade = abs(sin( ofGetElapsedTimef()*0.099 ));
        
        for (int i = 0; i < particles.size(); i++){
            particles[i]->cohesion.strength = 0.1;
            particles[i]->cohesion.distance = 50;
            particles[i]->alignment.strength =  0.7;
            particles[i]->alignment.distance = 42;
            particles[i]->seperation.strength = 0.07;
            particles[i]->seperation.distance = 38;
            particles[i]->damping = 0.1;
        }
        
        
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
        }
        
        
        for (int i = 0; i < particles.size(); i++){
            particles[i]->addFlockingForce();
            particles[i]->addDampingForce();
            particles[i]->update();
        }
    
        
             
    }
    
    
    // number == 2  +++++++++++++++++++++++++++++++++++++
    
    if(number==2)  {
 
        sort( particles.begin(), particles.end(), comparisonFunction );
        
        for (int i = 0; i < particles.size(); i++){
            particles[i]->cohesion.strength = 0.76;
            particles[i]->cohesion.distance = 50;
            particles[i]->alignment.strength =  0.5;
            particles[i]->alignment.distance = 130;
            particles[i]->seperation.strength = 0.4;
            particles[i]->seperation.distance = 157;
            particles[i]->damping = 0.192;
        }
        
        for (int i = 0; i < particles.size(); i++){
            for (int j = i-1; j >= 0; j--){
                if ( fabs(particles[j]->pos.x - particles[i]->pos.x) >	50) break;
                if (i != j){
                    particles[i]->addForFlocking(*particles[j]);
                }
        }
        
    }
    }
    
    // number == 3   +++++++++++++++++++++++++++++++++++++
    
    if(number==3)  {
        
        // background particles
        
        sort( particles.begin(), particles.end(), comparisonFunction );
        
        for (int i = 0; i < particles.size(); i++){
            particles[i]->cohesion.strength = 0.2;
            particles[i]->cohesion.distance = 200;
            particles[i]->alignment.strength =  0.24;
            particles[i]->alignment.distance = 198;
            particles[i]->seperation.strength = 0.4;
            particles[i]->seperation.distance = 33;
            particles[i]->damping = 0.029;
        }
        
        
    }
        
        
   // number == 4   +++++++++++++++++++++++++++++++++++++
    
    if(number == 4) {
         
        for (int i = 0; i < particles.size(); i++){
            particles[i]->cohesion.strength = 0.19;
            particles[i]->cohesion.distance = 200;
            particles[i]->alignment.strength =  0.2;
            particles[i]->alignment.distance = 175;
            particles[i]->seperation.strength = 0.46;
            particles[i]->seperation.distance = 200;
            particles[i]->damping = 0.45;
        }
    
        
}
        
         
    // number == 5  +++++++++++++++++++++++++++++++++++++
    
    if(number == 5) {
        
        for (int i = 0; i < particles.size(); i++){
            particles[i]->cohesion.strength = 0.1;
            particles[i]->cohesion.distance = 50;
            particles[i]->alignment.strength =  0.38;
            particles[i]->alignment.distance = 52;
            particles[i]->seperation.strength = 0.4;
            particles[i]->seperation.distance = 15;
            particles[i]->damping = 0.68;
        }
        
    }
        
    // number == 6 +++++++++++++++++++++++++++++++++++++
    
    if(number == 6) {
        
        for (int i = 0; i < particles.size(); i++){
            particles[i]->resetForce();
        }
        
        
        for (int i = 0; i < particles.size(); i++){
            for (int j = 0; j < particles.size(); j++){
                if (i != j){
                    particles[i]->addForFlocking(*particles[j]);
                }
            }
         
        for (int i = 0; i < particles.size(); i++){
            particles[i]->alignment.strength = 0.5;
            particles[i]-> damping = 0.15;
    
            }
        }
    }
    
    // number == 7  +++++++++++++++++++++++++++++++++++++
    
    if(number == 7) {
        
         
        for (int i = 0; i < particles.size(); i++){
            
           // particles[i]->addAttractionForce(mouseX, mouseY, 1000, 0.5);
            particles[i]->c.set(ofRandom(200,255), ofRandom(100,255), ofRandom(100,255));
            
            for (int j = 0; j < i; j++){
                if (bRepel){
                    particles[i]->addRepulsionForce(*particles[j], radius+20, strength);
                } else {
                    particles[i]->addAttractionForce(*particles[j], radius, strength);
                }
            }
            
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
	
	    
	//1 - Fade Fbo
	
	//this is where we fade the fbo
	//by drawing a rectangle the size of the fbo with a small alpha value, we can slowly fade the current contents of the fbo.
	ofFill();
    
    ofColor dark(10,fadeAmnt);
    ofColor black(0, fadeAmnt);

    ofBackgroundGradient(dark, black);
    ofSetColor(255,255,255, fadeAmnt);
    
    if(number== 1) {
        fadeAmnt = 1;
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
    
    ofRect(0,0,ofGetScreenWidth(),ofGetScreenHeight());
    
	//2 - Draw graphics
	
	for (int i = 0; i < particles.size(); i++){
		particles[i]->draw();
	}
    
}


//--------------------------------------------------------------
void testApp::draw(){
	
    
    ofSetColor(255,50);
  
    rgbaFbo.draw(0,0);
    rgbaFboFloat.draw(0,0);
  
    string alphaInfo = "Current alpha fade amnt = " + ofToString(fadeAmnt);

    
  videoDiffMHI.draw(0, 0);
//	videoGrayscaleCvImage.draw(20,20, 100,80);
//	videoPrevFrameImage.draw(20, 120, 100, 80);
//	videoDiffImage.draw(20,220,100,80);
//  ofEnableAlphaBlending();
//  panel.draw();
    
    for (int i = 0; i < particles.size(); i++){
		particles[i]->draw();
	}

    ofDrawBitmapString(alphaInfo, ofPoint(10,430));

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

