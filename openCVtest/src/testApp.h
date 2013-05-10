#ifndef _TEST_APP
#define _TEST_APP
#define PARTICLE_NUM 300

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxControlPanel.h"
#include "particle.h"
#include "timePoint.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        void drawFboTest();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		
		// create a variable of the type ofImage
		ofxControlPanel			panel;
	
		int						width, height;
		ofVideoGrabber			video;
		ofxCvColorImage			videoColorCvImage;
		ofxCvGrayscaleImage		videoGrayscaleCvImage;
		ofxCvGrayscaleImage		videoPrevFrameImage;
	
		ofxCvGrayscaleImage		videoDiffImage;
		ofxCvGrayscaleImage		videoDiffMHI;
	
    // particles

    int number;
    
    bool	bRepel;
    float	radius;
    float	strength;
	

    
    
    vector < timePoint > pts;
    float startTime;
    
    vector <particle *> particles;
   // vector <particle *> particles2;
    particle myParticle;
    particle myParticle2;
    
    ofVec2f oldM;
    bool    bPaint, bObstacle, bBounding, bClear;
        
    //fbo
    ofFbo rgbaFboFloat;
    ofFbo rgbaFbo;
    int fadeAmnt;
    
    //audio

};

#endif
