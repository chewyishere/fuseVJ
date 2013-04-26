#include "rectangle.h"


//------------------------------------------------------------------
rectangle::rectangle(){
	catchUpSpeed = 0.0001f;
	
	pos.set(0,0);
	prevPos.set(0,0);
    scl.x=0;
    scl.y=0;
}

//------------------------------------------------------------------
void rectangle::draw( int colori) {
	ofFill();
	
	ofSetRectMode(OF_RECTMODE_CENTER); // center around the position
   // ofSetColor(198,246,55);


	ofPushMatrix();
  
    ofTranslate(pos.x, pos.y);
    ofRotateZ(angle * RAD_TO_DEG);
   
    ofEnableAlphaBlending();  

     ofSetColor(233, 244, 201,  155-colori*2);
    ofRect( 0,0, scl.x, scl.y*1.2);

    
    ofSetColor(198-colori*5, 246-colori*5, 55,  200-colori*2);
    //ofCircle(0, 0, scl.y*4);
    
    ofEllipse(0, 0, scl.x, scl.y*4);
    
    
    ofDisableAlphaBlending();
	
	ofPopMatrix();
	
    ofSetRectMode(OF_RECTMODE_CORNER); 
 
  
	
}

//------------------------------------------------------------------
void rectangle::xenoToPoint(float catchX, float catchY){
	
	
	pos.x = catchUpSpeed * catchX + (1-catchUpSpeed) * pos.x; 
	pos.y = catchUpSpeed * catchY + (1-catchUpSpeed) * pos.y; 
    
	float dx = pos.x - prevPos.x;
	float dy = pos.y - prevPos.y;
	
	angle = atan2(dy, dx);
    
	prevPos.x = pos.x;
	prevPos.y = pos.y;
	
    
}
