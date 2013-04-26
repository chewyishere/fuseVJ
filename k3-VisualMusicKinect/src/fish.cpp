//
//  fish.cpp
//  ofxKinectExample
//
//  Created by Yinan Zhang on 4/22/13.
//  Copyright (c) 2013 TJU. All rights reserved.
//

#include <iostream>
#include "fish.h"


//------------------------------------------------------------------
fish::fish(){
    
for (int i=0; i<RECT_NUM;i++) {
myRectangle[i].scl.x = 10 ;
myRectangle[i].scl.y = 10 ;
myRectangle[i].pos.x = ofGetWidth()/2;
myRectangle[i].pos.y = ofGetHeight()/2;
}
    
    
    ppx = ofGetWidth()/2;
ppy=  ofGetHeight()/2;
    
//    for (int i=1; i<RECT_NUM;i++) {
//        myRectangle[i].scl.x = 0 ;
//        myRectangle[i].scl.y = 0 ;
//        myRectangle[i].pos.x = ofGetWidth()/2;
//        myRectangle[i].pos.y = ofGetHeight()/2;
//        
//    }

}


//--------------------------------------------------------------
void fish::update(float vol,  float size , float offset, float knock){
   

    
    float angle = ofGetElapsedTimef()*(3.5-offset*.1)+vol*100;
    ppx = ppx +  cos(angle)*vol*200;
    ppy = ppy - sin(angle)*vol*200;
   
    if(ppx>ofGetWidth()) { ppx=ofGetWidth();};    
    if(ppy>ofGetHeight()/2) { ppy=ofGetHeight()/2;};
    if(ppx<300){ ppx=300;};
    if(ppy<0){ ppy=0;};
   
    
    float speed = vol*.4;
    
    myRectangle[0].scl.x = 10; 
    myRectangle[0].scl.y = size+ vol ;
    myRectangle[0].catchUpSpeed = speed  ;
    myRectangle[0].xenoToPoint(ppx, ppy);

    
    for (int i=1; i<RECT_NUM;i++) {
    myRectangle[i].scl.x = 18+knock; 
        myRectangle[i].scl.y =   4-offset*.8;  //ofClamp(offset, 0.1, 1.3) ;
    myRectangle[i].catchUpSpeed = ofClamp(vol*3.5, 0,.6) ;
    myRectangle[i].xenoToPoint(myRectangle[i-1].pos.x, myRectangle[i-1].pos.y);
    }
}

//------------------------------------------------------------------
void fish::draw( ) {

for (int i=1; i<RECT_NUM;i++) {    
  myRectangle[i].draw(i);
}
    
    
//    for (int i=0; i<RECT_NUM;i++) {
//        // ofSetColor(198-i*5, 246-i*5, 55,  255-i*4);        
//        // ofSetColor(ofMap(i, 0, 99, 255, 50),ofMap(i, 0, 99, 200, 100));  ///??
//        myRectangle[i].draw(i);
//       
//    }



}