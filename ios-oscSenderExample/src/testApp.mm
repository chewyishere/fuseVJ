#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetOrientation(OF_ORIENTATION_90_LEFT);
    
	ofxAccelerometer.setup();
    

	// open an outgoing connection to HOST:PORT
	sender.setup( HOST, PORT );
    
    
	// 2 output channels,
	// 0 input channels
	// 44100 samples per second
	// 512 samples per buffer
	// 4 num buffers (latency)
    
	sampleRate = 44100;
	phase = 0;
	phaseAdder = 0.0f;
	phaseAdderTarget = 0.0;
	volume = 0.15f;
	pan = 0.5;
	bNoise = false;
	
	//for some reason on the iphone simulator 256 doesn't work - it comes in as 512!
	//so we do 512 - otherwise we crash
	initialBufferSize = 512;
	
	lAudio = new float[initialBufferSize];
	rAudio = new float[initialBufferSize];
	
	memset(lAudio, 0, initialBufferSize * sizeof(float));
	memset(rAudio, 0, initialBufferSize * sizeof(float));
	
	//we do this because we don't have a mouse move function to work with:
	targetFrequency = 444.0;
	phaseAdderTarget = (targetFrequency / (float)sampleRate) * TWO_PI;
	
	ofSoundStreamSetup(2, 0, this, sampleRate, initialBufferSize, 4);
	ofSetFrameRate(60);

    
    
}

//--------------------------------------------------------------
void testApp::update(){
	//we do a heartbeat on iOS as the phone will shut down the network connection to save power
	//this keeps the network alive as it thinks it is being used. 
	if( ofGetFrameNum() % 120 == 0 ){
		ofxOscMessage m;
		m.setAddress( "/misc/heartbeat" );
		m.addIntArg( ofGetFrameNum() );
		sender.sendMessage( m );
	}
    
    ofxOscMessage m;
    m.setAddress( "/accel/x" );
    m.addFloatArg( ofxAccelerometer.getForce().x );
    m.setAddress( "/accel/y" );
    m.addFloatArg( ofxAccelerometer.getForce().y );
    m.setAddress( "/accel/z" );
    m.addFloatArg( ofxAccelerometer.getForce().z );
    
    

    sender.sendMessage( m );
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofColor background;
    float inX = ofMap(mouseX, 0, ofGetWidth(), 0, 255);
    float inY = ofMap(mouseY, 0, ofGetHeight(), 0, 255);
    
    background.setHsb(inX, inY, 255);
	ofBackground(background);
    
    ofxOscMessage m;
    m.setAddress( "/hue" );
    m.addFloatArg( inX );
    m.setAddress( "/sat" );
    m.addFloatArg( inY );
    sender.sendMessage( m );
    
    
	// display instructions
	string buf;
	buf = "sending osc messages to" + string( HOST ) + ofToString( PORT );
	ofDrawBitmapString( buf, 10, 20 );
	ofDrawBitmapString( "move the mouse to send osc message [/mouse/position <x> <y>]", 10, 50 );
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs & touch){
	ofxOscMessage m;
	m.setAddress( "/mouse/button" );
	m.addStringArg( "down" );
	sender.sendMessage( m );
    
    
    bNoise = true;
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs & touch){
	ofxOscMessage m;
	m.setAddress( "/mouse/position" );
	m.addIntArg( touch.x );
	m.addIntArg( touch.y );
	sender.sendMessage( m );
    
    
    if(touch.id ==  0){
		int width = ofGetWidth();
		pan = (float)touch.x / (float)width;
        
        int height = ofGetHeight();
        targetFrequency = ((float)touch.y / (float)height) * 1000;
        phaseAdderTarget = (targetFrequency / (float)sampleRate) * TWO_PI;
	}
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs & touch){
	ofxOscMessage m;
	m.setAddress( "/mouse/button" );
	m.addStringArg( "up" );
	sender.sendMessage( m );
    
    bNoise = false;
}


//--------------------------------------------------------------
void testApp::audioOut(float * output, int bufferSize, int nChannels){
    
	if( initialBufferSize != bufferSize ){
		ofLog(OF_LOG_ERROR, "your buffer size was set to %i - but the stream needs a buffer size of %i", initialBufferSize, bufferSize);
		return;
	}
	//pan = 0.5f;
	float leftScale = 1 - pan;
	float rightScale = pan;
    
	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	while(phase > TWO_PI){
		phase -= TWO_PI;
	}
    
	if(bNoise == true){
		// ---------------------- noise --------------
		for(int i = 0; i < bufferSize; i++){
			lAudio[i] = output[i * nChannels] = ofRandomf() * volume * leftScale;
			rAudio[i] = output[i * nChannels + 1] = ofRandomf() * volume * rightScale;
		}
	} else {
		phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
		for(int i = 0; i < bufferSize; i++){
			phase += phaseAdder;
			float sample = sin(phase);
			lAudio[i] = output[i * nChannels] = sample * volume * leftScale;
			rAudio[i] = output[i * nChannels + 1] = sample * volume * rightScale;
		}
	}
    
	
}


//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs & touch){
    bNoise = false;
}

//--------------------------------------------------------------
void testApp::lostFocus(){

}

//--------------------------------------------------------------
void testApp::gotFocus(){

}

//--------------------------------------------------------------
void testApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){

}
