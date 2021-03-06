#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxOsc.h"

#define HOST "192.168.1.125"
#define PORT 12345

class testApp : public ofxiPhoneApp {

	public:
		void setup();
		void update();
		void draw();
		void exit();
		
		void touchDown(ofTouchEventArgs & touch);
		void touchMoved(ofTouchEventArgs & touch);
		void touchUp(ofTouchEventArgs & touch);
		void touchDoubleTap(ofTouchEventArgs & touch);
		void touchCancelled(ofTouchEventArgs & touch);

		void lostFocus();
		void gotFocus();
		void gotMemoryWarning();
		void deviceOrientationChanged(int newOrientation);

		ofxOscSender sender;
    
    
        
        void audioOut(float * output, int bufferSize, int nChannels);
        
        
        float pan;
        int sampleRate;
        bool bNoise;
        float volume;
        
        float * lAudio;
        float * rAudio;
        
        //------------------- for the simple sine wave synthesis
        float targetFrequency;
        float phase;
        float phaseAdder;
        float phaseAdderTarget;
        int initialBufferSize;
};

