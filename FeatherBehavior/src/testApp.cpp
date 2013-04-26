#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
    //background setting
    col1 = (ofRandom(100),ofRandom(100),ofRandom(100));
    colPink = (255,ofRandom(100,250),255);

    ofEnableAlphaBlending();
    
    background.loadImage("bg1_star.jpg");
     
    // particles
    
    ofSetVerticalSync(true);
	ofSetFrameRate(60);
		spot.loadImage("spot.png");     // particle 1
	    spot.setAnchorPercent(0,0);   // set Anchor Percent
        
	for (int i = 0; i < 50; i++){
		myParticle.setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
		myParticle.img = &spot;
        myParticle.scl.x = 100;
        myParticle.scl.y = 100;
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
    
    //Sound input
    
    piano.loadSound("La Campanella.mp3");
    piano.play();
    	
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
    //background setting
    
    float fade = abs(sin( ofGetElapsedTimef()*0.099 ));
    
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
    

// number == 1 +++++++++++++++++++++++++++++++++++++   
    

if(number==1) {
    
  	    
	for (int i = 0; i < particles.size(); i++){
		particles[i].resetForce();
	}
		
	for (int i = 0; i < particles.size(); i++){
		
		particles[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
        particles[i].scl.x = scaledVol*1000;
        particles[i].scl.y = scaledVol*1000;
        
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
    
    
};
    

// number == 2 +++++++++++++++++++++++++++++++++++++
    
      if(number==2)  {
                    
          for (int i = 0; i < particles.size(); i++){
              particles[i].resetForce();
          }
                   
          
          for (int i = 0; i < particles.size(); i++){
              
              particles[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
              particles[i].scl.x = 100;
              particles[i].scl.y = 100;
              
              
              
              for (int j = 0; j < i; j++){
                  if (bRepel){
                      particles[i].addRepulsionForce(particles[j], radius + volumeSmooth * 300, strength + volumeSmooth);
                  } else {
                      particles[i].addAttractionForce(particles[j], radius, strength);
                  }
              }
          }
          
          for (int i = 0; i < particles.size(); i++){
              particles[i].addDampingForce();
              particles[i].damping = 0.1;
              particles[i].update();
          }
    
    }


// number == 3   Draw with chosen feather + flocking +++++++++++++++++++++++++++++++++++++ 
    
    if(number==3)  {
        for (int i = 0; i < particles.size(); i++){
            particles[i].resetForce();
        }
        
        for (int i = 0; i < particles.size(); i++){
            for (int j = 0; j < particles.size(); j++){
                if (i != j){
                    particles[i].addForFlocking(particles[j]);
                }
            }
            
            
            particles[i].addRepulsionForce(mouseX, mouseY, 40, 0.4);
        }
        
        for (int i = 0; i < particles.size(); i++){
            particles[i].color.set(255, 255,ofRandom(150), smoothedVol*10000);
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

// number == 4 feather4 seperation.distance +++++++++++++++++++++++++++++++++++++
    
    if(number == 4) {
        
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
    
// number == 5 feather +++++++++++++++++++++++++++++++++++++       
       
        if(number == 5) {

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
    

// number == 6 == draw +++++++++++++++++++++++++++++++++++++
        
    
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
        particles[i].alignment.distance = smoothedVol ;
        particles[i].alignment.strength = 60;
        particles[i].damping = 0.15;
        
        particles[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
        
    }


    // number == 7 == Launch +++++++++++++++++++++++++++++++++++++

    if(number == 7) {
        
        for (int i = 0; i < particles.size(); i++){
            particles[i].resetForce();
        }
        
        for (int i = 0; i < particles.size(); i++){
            
            particles[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
            particles[i].scl.x = 150;
            particles[i].scl.y = 150;
            
            
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
    
    
if(number>0) {   
    
    pts.erase(pts.begin());   

    }  
    
     //lets scale the vol up to a 0-1 range 
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    
    
    if (scaledVol > volumeSmooth){
        volumeSmooth = 0.8 * volumeSmooth + 0.2 * scaledVol;
    } else {
        volumeSmooth = 0.99 * volumeSmooth + 0.01 * scaledVol;
    }
    
    // change 0.97 / 0.03 to get different smoothing rates
    //volumeSmooth = 0.8 * volumeSmooth + 0.2 * scaledVol;

	//lets record the volume into an array
  	volHistory.push_back( scaledVol );
	
	//if we are bigger the the size we want to record - lets drop the oldest value
 	if( volHistory.size() >= 400 ){
		volHistory.erase(volHistory.begin(), volHistory.begin()+1);
	}
    
}



//--------------------------------------------------------------
void testApp::draw(){
    
    
    //background
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofBackgroundGradient(ofColor(252,239,233,255), ofColor(58,30,0,255));
    
    ofSetColor(255, 50);
    background.draw(0,0,ofGetWidth(), ofGetHeight());
    
    
    ofEnableAlphaBlending();
	//ofSetColor(0,130,130);
	
	for (int i = 0; i < particles.size(); i++){
		particles[i].draw();
	}
	
	
	string reportString =	// "(space) = reset\n(a/s) strength = " + ofToString(strength) +
    // "\n(z/x) radius = " + ofToString(radius)
        +
   // "\n(r) toggle mode = " + (bRepel ? "repelling" : "attracting")
        +
    "\n scaledVol = " + ofToString(scaledVol)
     +
    
    + "\n volumeSmooth = " + ofToString(volumeSmooth)
    
    +
    "\n 1 = sound to size "
    +
    "\n 2 = sound to spread "
    +
    "\n 3 = sound to tint "
    +
    "\n 4 = sound to drift "
    +
    "\n 5 = sound to wiggle "
    +
    "\n 6 = draw with current feather "
    +
    "\n 7 = launch fill up screen "
;
    

	ofSetColor(40,40,0);
	ofRect(10,10,320,150);
	ofSetColor(255,255,255);
	ofDrawBitmapString(reportString, 30, 30);
    
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
void testApp::keyPressed  (int key){
    
    switch (key){
			
		case ' ':
			// reposition everything:
			for (int i = 0; i < particles.size(); i++){
				particles[i].setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
			}
			break;
		case 'a':
			strength += 0.02f;
			break;
		case 's':
			strength -= 0.02f;
			if (strength <= 0) strength = 0.02f;
			break;
		case 'z':
			radius += 5.0f;
			break;
		case 'x':
			radius -= 5.0f;
			if (radius <= 0) radius = 5.0f;
			break;
		case 'r':
			bRepel = !bRepel;
			break;
	}
	
	if( key == 's' ){
		soundStream.start();
	}

    if( key == 'e' ){
		soundStream.stop();
	}
    
    if( key == '`' ){
		number=0;
	}
    
    if( key == '1' ){
        number=1;
        spot.setAnchorPercent(0,0);
        spot.loadImage("spot.png");
        for (int i = 0; i < particles.size(); i++){
           particles[i].color.set(colPink);
        }
         
    }
    
    if( key == '2' ){
        
        number=2;
        
        spot.loadImage("feather.png");
        spot.setAnchorPercent(0.5,0.5);
        
        for (int i = 0; i < particles.size(); i++){
            particles[i].color.set(0,255,ofRandom(100,255),90);
        }     
        
	}
    
    if( key == '3' ){
        number=3;
        spot.loadImage("feather2.png");
        spot.setAnchorPercent(0.5,0.5);
        
        for (int i = 0; i < particles.size(); i++){
            particles[i].color.set(255, 255,ofRandom(150), smoothedVol*10000);
        }
        
    }
    
    if( key == '4' ){
        number=4;
        
        spot.loadImage("feather3.png");
        spot.setAnchorPercent(0.5,0.5);

        for (int i = 0; i < particles.size(); i++){
           particles[i].color.set(ofRandom(100,255),255 ,255,90);
        }
              
		
	}
    
    if( key == '5' ){
        
		number=5;
        spot.loadImage("feather4.png");
        spot.setAnchorPercent(0.5,0.5);
        
        
        for (int i = 0; i < particles.size(); i++){
             particles[i].color.set(255, ofRandom(100,255), 255,90);
        }
	}
    
    
    if( key == '6' ){
		number=6;
        
        
	}
    
    if( key == '7' ){
		number=7;
        
        for (int i = 0; i < particles.size(); i++){
            particles[i].color.set(ofRandom(100,255), ofRandom(100,255), ofRandom(100,255));
        }
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
    myParticle.img = &spot;
	myParticle.setInitialCondition(x,y,0,0);
	particles.push_back(myParticle);
    
   if (number == 1){
        for (int i = 0; i < particles.size(); i++){
            particles[i].color.set(colPink);
        }

    }
    
    if (number == 2){
        for (int i = 0; i < particles.size(); i++){
            particles[i].color.set(0,255,ofRandom(100,255),90);
        }
    }
    
    if (number == 3){
        for (int i = 0; i < particles.size(); i++){
            particles[i].color.set(255, 255,ofRandom(150), smoothedVol*10000);
        }
    }
    
    if (number == 4){
        for (int i = 0; i < particles.size(); i++){
            particles[i].color.set(ofRandom(100,255),255 ,255,90);
        }
        
    }
    
    if (number == 5){
        for (int i = 0; i < particles.size(); i++){
            particles[i].color.set(255, ofRandom(100,255), 255,90);
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

