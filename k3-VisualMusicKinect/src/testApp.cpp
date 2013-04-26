#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    ofBackground(0);
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofSetFrameRate(50);
    ofSetVerticalSync(true);
	kinect.init(false, false);  // disable infrared/rgb video iamge (faster fps)
	kinect.open();
	panel.setup(280, 400);
	panel.addPanel("Settings");
	panel.addSlider("nearThreshold", 2100, 0, 7000);
	panel.addSlider("farThreshold", 2138, 0, 7000);
    panel.addSlider("contourX", -100, -300, 300);
    panel.addSlider("contourY", 90, 300, -300);
    panel.addSlider("scaleX", 333, -600, 600);
    panel.addSlider("scaleY", 286, -600, 600);
    panel.addSlider("contourBlur", 0, 0, 30);
	panel.addToggle("nearWhite", true);
	panel.addToggle("threshold", false);
	
	result.allocate(kinect.getWidth(), kinect.getHeight(), OF_IMAGE_GRAYSCALE);
    resultAsCvImage.allocate(result.width, result.height);
    
    
    
    blendMode = OF_BLENDMODE_MULTIPLY;
    
    vidPlayer.loadMovie("background.jpg");
    bigvidPlayer.loadMovie("background.jpg");
    testimage.loadImage("testimage.jpg");
    
    vidPlayer.setLoopState(OF_LOOP_NONE);
    bigvidPlayer.setLoopState(OF_LOOP_NONE);
    
    //image.loadImage("white.jpg");
    
    threshold=0;
    number=0;
    
    
    //Sound input
	
	soundStream.listDevices();
	
	//if you want to set a different device id
	//soundStream.setDeviceID(0); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
	
	int bufferSize = 256;
	
	
	left.assign(bufferSize, 0.0);
	volHistory.assign(400, 0.0);
	
	bufferCounter	= 0;
	drawCounter		= 0;
	smoothedVol     = 0.0;
	scaledVol		= 0.0;
    scaledVol1  =0.0;
    
    
	soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
    vidtoggle=0;
    flash=0;
    dottoggle=0;
    flashtoggle=2;
    
    for (int i = 0; i < 80; i++){
        time[i]= ofRandom(10);
    }
    
    knock=0;
    fishnumber=0;

}

//--------------------------------------------------------------
void testApp::update(){
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
    
    
    
    
    resultAsCvImage.setFromPixels(result.getPixels(),result.width, result.height);
    
    resultAsCvImage.blur(panel.getValueF("contourBlur"));
    
    
    // cout << MAX(mouseX * 2 + 1, 1) << endl;
    
    vidPlayer.idleMovie();
    bigvidPlayer.idleMovie();
    
    // threshold
    
    
    if (threshold==1){
        amount+=0.1;
        threshold++;
    };
    
    
    if ( threshold==-1) {
        
        amount -=0.1;
        threshold++;
    };
    
    
    
    
    scaledVol= scaledVol1* (1.0+amount);
    
    //lets scale the vol up to a 0-1 range
	scaledVol1 = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    
	//lets record the volume into an array
	volHistory.push_back( scaledVol1 );
	
	//if we are bigger the the size we want to record - lets drop the oldest value
	if( volHistory.size() >= 400 ){
		volHistory.erase(volHistory.begin(), volHistory.begin()+1);
	}
    
	radius = radius*scaledVol ;
    

}

//--------------------------------------------------------------
void testApp::draw(){
    
    
    int posx=panel.getValueF("contourX");
    int posy=panel.getValueF("contourY");
    int sclx=panel.getValueF("scaleX");
    int scly=panel.getValueF("scaleY");
    
    
    if (vidtoggle==0) {
        if (dottoggle==0){
            ofSetColor(255);
            vidPlayer.draw(posx, posy, 640+sclx, 480+scly);
            vidPlayer.play();
        }
        
        
        if (dottoggle==1 ){
            float rotate1=0;
            rotate1+=scaledVol*0.1;
            
            
            for (int i = 0; i < 80; i++){
                
                //          time2+=scaledVol*0.1;
                time[i]+=scaledVol*0.1;
                
                float ppx = ofNoise(time[i]*1.5)*ofGetWidth()-400;
                float ppy = ofNoise(time[i]+300)*ofGetHeight()+50;
                //          ofColor col = getColorForPixel( ofMap(posx,0,ofGetWidth(), 0, colorScheme.width),
                //                                         ofMap(posy,0,ofGetHeight(), 0, colorScheme.height));
                //col.a = 100;	// set some alpha.
                ofEnableAlphaBlending();
                ofSetColor(200,200,200,30+scaledVol*200*i);
                ofCircle(ppx, ppy ,3+scaledVol*40);
                ofDisableAlphaBlending();
                
            }
            
            
            
        }
        
        
        if (dottoggle==2){
            
            image.draw(posx, posy,640+sclx,480+scly);
            
            
        }
        
        
        
        
        if (flashtoggle%2==0 && flash==1){
            ofEnableAlphaBlending();
            ofSetColor(255,255,255,scaledVol*1000);
            ofRect(posx, posy,640+sclx,480+scly);
            ofDisableAlphaBlending();
        }
        
        
        ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
        ofSetColor(255);
        resultAsCvImage.draw(posx, posy,640+sclx,480+scly);
        ofDisableBlendMode();
        
        
        
        
    }
    
    
    //    else{
    //        ofSetColor(255);
    //        bigvidPlayer.draw(0,0, ofGetScreenWidth(), ofGetScreenHeight());
    //        bigvidPlayer.play();
    //    }
    
    
    
    if (flashtoggle%2>0 && flash==1){
        ofEnableAlphaBlending();
        ofSetColor(255,255,255,scaledVol*1000);
        ofRect(0, 0,  ofGetScreenWidth(), ofGetScreenHeight());
        ofDisableAlphaBlending();
    }
    
    
    
    if (dottoggle==3){
        
        
        if(fishnumber>0){
            for (int i = 1; i < 3; i++){
                myFish[i].update(scaledVol,3 , i, knock);
                myFish[i].draw();
            }
            
            if(fishnumber>1){
                for (int i = 3; i < 6; i++){
                    myFish[i].update(scaledVol,3 , i, knock);
                    myFish[i].draw();
                }
                
            }
            
            if(fishnumber>2){
                for (int i = 6; i < 9; i++){  
                    myFish[i].update(scaledVol,3 , i, knock);
                    myFish[i].draw();
                }
                
            }
            
            
            //   for (int i = 0; i < FISH_NUM; i++){     
            //        myFish[i].draw();
        } 
    } 





}

//--------------------------------------------------------------
void testApp::exit() {
	kinect.close();
}

//--------------------------------------------------------------
void testApp::audioIn(float * input, int bufferSize, int nChannels){
	
	float curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;
    
	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume
	for (int i = 0; i < bufferSize; i++){
		left[i]		= input[i*2]*0.5;
        
		curVol += left[i] * left[i];
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
    
    switch (key) {
        case '1':
            vidtoggle=0;
            dottoggle=0;
            vidPlayer.loadMovie("Export1.mov");
            vidPlayer.firstFrame();
            vidPlayer.play();
            break;
            
        case '2':
            vidtoggle=0;
            dottoggle=0;
            vidPlayer.loadMovie("Export2.mov");
            break;
            
        case '3':
            vidtoggle=0;
            dottoggle=0;
            vidPlayer.loadMovie("Export3.mov");
            vidPlayer.firstFrame();
            vidPlayer.play();
            break;
            
        case '4':
            vidtoggle=0;
            dottoggle=1;
            
            //vidPlayer.loadMovie("bach640.mov");
            //vidPlayer.firstFrame();
            //vidPlayer.play();
            break;
            
        case '5':
            vidtoggle=0;
            dottoggle=0;
            vidPlayer.loadMovie("Export4.mov");
            vidPlayer.firstFrame();
            vidPlayer.play();
            break;
            
        case '6':
            vidtoggle=0;
            dottoggle=3;
            fishnumber+=1;
            break;
            
        case '7':
            vidtoggle=0;
            dottoggle=0;
            vidPlayer.loadMovie("AEtest1.mov");
            vidPlayer.firstFrame();
            vidPlayer.play();
            break;
            
            
            
        case '8':
            vidtoggle=0;
            dottoggle=0;
            vidPlayer.loadMovie("ALICE Gate.mp4");
            vidPlayer.firstFrame();
            vidPlayer.play();
            break;
            
        case '9':
            vidtoggle=0;
            dottoggle=0;
            vidPlayer.loadMovie("Droplets.mov");
            vidPlayer.firstFrame();
            vidPlayer.play();
            break;
            
        case '0':
            vidtoggle=0;
            dottoggle=0;
            vidPlayer.loadMovie("explosion test.mov");
            vidPlayer.firstFrame();
            vidPlayer.play();
            break;
            
        case 'q':
            vidtoggle=0;
            dottoggle=0;
            vidPlayer.loadMovie("LA132.mov");
            vidPlayer.firstFrame();
            vidPlayer.play();
            break;
            
        case 'w':
            vidtoggle=0;
            dottoggle=0;
            vidPlayer.loadMovie("Lens_Splash_03.mov");
            vidPlayer.firstFrame();
            vidPlayer.play();
            break;
            
        case 'e':
            vidtoggle=0;
            dottoggle=0;
            vidPlayer.loadMovie("Mike Tosetto Promo Animation.mp4");
            vidPlayer.firstFrame();
            vidPlayer.play();
            break;
            
        case 'r':
            vidtoggle=0;
            dottoggle=0;
            vidPlayer.loadMovie(" ");
            vidPlayer.firstFrame();
            vidPlayer.play();
            break;
            
            
            //        case 'a':
            //            vidtoggle=1;
            //            bigvidPlayer.loadMovie("AEtest1.mov");
            //            vidPlayer.firstFrame();
            //            vidPlayer.play();
            //            break;
            
        case '=':
            threshold=1;
            break;
        case '-':
            threshold=-1;
            break;
            
            
        case ' ':
            vidtoggle=0;
            dottoggle=2;
            image.loadImage("background.jpg");
            break;
            
        case 'b':
            vidtoggle=0;
            dottoggle=2;
            image.loadImage("white.jpg");
            break;
            
            
            
            
        case 'v':
            flashtoggle+=1;
            break;
            
            
            
            
            
            //        case ' ':
            //            flash=1;
            //            break; 
    }
    

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
    knock=ofMap(x,0,1000, 0, 550);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
    flash=1;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    flash=0;
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