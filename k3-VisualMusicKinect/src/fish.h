//
//  fish.h
//  ofxKinectExample
//
//  Created by Yinan Zhang on 4/22/13.
//  Copyright (c) 2013 TJU. All rights reserved.
//

#ifndef ofxKinectExample_fish_h
#define ofxKinectExample_fish_h
#define RECT_NUM 37 
#include "ofMain.h"
#include "rectangle.h"


class fish {

public:
    fish();

    void	draw();
    void	update(float vol, float size ,float offset , float knock );
    rectangle myRectangle[RECT_NUM];
    int ppx, ppy;
} ;
    
#endif
