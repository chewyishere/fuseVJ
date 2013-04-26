#include "testApp.h"



//--------------------------------------------------------------

void testApp::setup() {
	ofBackground(0); 
    ofSetVerticalSync(true);
	kinect.init(false, false);  // disable infrared/rgb video iamge (faster fps)
	kinect.open();
	
	panel.setup(280, 400);
	panel.addPanel("Settings");
	panel.addSlider("nearThreshold", 1800, 0, 5000);
	panel.addSlider("farThreshold", 3000, 0, 5000);
	panel.addToggle("nearWhite", true);
	panel.addToggle("threshold", false);
	
	result.allocate(kinect.getWidth(), kinect.getHeight(), OF_IMAGE_GRAYSCALE);
    
    
    
    
    
/////////////////////////video///////////////////////////////
    
    vidPlayer.loadMovie("xxx.mov");
    vidPlayer.play();
    
    fore.loadImage("forground.jpg");
    
    back.loadImage("background.jpg");
    
    
    colorImg.allocate(1280,720);
	grayImage.allocate(640,480);
	grayBg.allocate(640,480);
	grayDiff.allocate(640,480);
    
	bLearnBakground = true;
	threshold = 80;

    mix.allocate(640,480, OF_IMAGE_COLOR);
    
}


//--------------------------------------------------------------

void testApp::update() {
	kinect.update();
	if(kinect.isFrameNew()) {
		float* distancePixels = kinect.getDistancePixels(); // distance in millimeters
		unsigned char* resultPixels = result.getPixels();
		int n = kinect.getWidth() * kinect.getHeight();
		float nearThreshold = panel.getValueF("nearThreshold");
		float farThreshold = panel.getValueF("farThreshold");
		int nearTarget, farTarget;
		bool threshold = panel.getValueB("threshold");
		int thresholdLevel = (nearTarget + farTarget) / 2;
		bool nearWhite = panel.getValueB("nearWhite");
		if(nearWhite) {
			nearTarget = 255;
			farTarget = 0;
		} else {
			nearTarget = 0;
			farTarget = 255;
		}
		if(nearThreshold != farThreshold) {
			for(int i = 0; i < n; i++) {
				float cur = distancePixels[i];
				if(cur != 0) { // only remap nonzeros
				
					// this code will threshold data to white or black
					if(threshold) {
						if(cur > thresholdLevel) {
							if(nearWhite) {
								cur = 0;
							} else {
								cur = 255;
							}
						} else {
							if(nearWhite) {
								cur = 255;
							} else {
								cur = 0;
							}
						}
					}
					
					// alternatively, just remap between 0 and 255
					else {
						cur = ofMap(cur, nearThreshold, farThreshold, nearTarget, farTarget, true);
					}
				}
				resultPixels[i] = cur;
			}
		}
		result.update();
	}
    
    
    
    
//////////////////////////////////////cv//////////////////////////////////////////////
    //	ofBackground(100,100,100);
    
 
    

   grayImage.setFromPixels(result.getPixels(), 640,480);
    
    

    
    //        grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg 
    
    
    
    // take the abs value of the difference between background and incoming and then threshold:
    grayDiff.absDiff(grayBg, grayImage);
    grayDiff.threshold(threshold);
    
    grayDiff.blur(5);
    // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
    // also, find holes is set to true so we will get interior contours as well....
    contourFinder.findContours(grayDiff, 20, (640,480)/3, 10, true);	// find holes

    
    
    // we are dealing with 4 images
    // image a, b, 
    // image mix
    // and the greysacale image: 
    // get the pixels
    
    unsigned char * grayPixels = grayDiff.getPixels();
    unsigned char * colorPixelsA = back.getPixels();
    unsigned char * colorPixelsB = vidPlayer.getPixels();
    unsigned char * mixPixels = mix.getPixels();
    
    
    for (int i = 0; i < 640*480; i++){
        
        // get the color from each color image. 
        
        ofColor colorA( colorPixelsA[i * 3], colorPixelsA[i * 3 + 1], colorPixelsA[i * 3 + 2]);
        ofColor colorB( colorPixelsB[i * 3], colorPixelsB[i * 3 + 1], colorPixelsB[i * 3 + 2]);
        
        // turn grayscale into a pct: 
        
        float pct = ofMap(grayPixels[i], 0, 255, 0,1);
        
        
        // mix the two colors based on pct:
        
        ofColor colorMix = colorA * (1-pct)  + colorB * pct;
        
        // set the color of the pixel in the mix image to the new mixed color
        
        mixPixels[i * 3] = colorMix.r;
        mixPixels[i * 3+1] = colorMix.g;
        mixPixels[i * 3+2] = colorMix.b;
        
        
        
    }
    
    // update the mix image
    
    mix.update();
    
    
      
}




//--------------------------------------------------------------


void testApp::draw() {
ofSetColor(255);
//result.draw(0, 0);
    
  mix.draw(0,0,1280,720);
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    
    if(key == 't') { ofToggleFullscreen();CGDisplayHideCursor(kCGDirectMainDisplay); }
    

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
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



void testApp::exit() {
	kinect.close();
}
