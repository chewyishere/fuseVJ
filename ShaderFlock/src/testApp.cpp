#include "testApp.h"
#define STRINGIFY(A) #A

// comparison routine for sort...
bool comparisonFunction(  particle * a, particle * b ) {
	return a->pos.x < b->pos.x;
}

//--------------------------------------------------------------
void testApp::setup(){
    
    receiver.setup(PORT);
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
	CGDisplayHideCursor(kCGDirectMainDisplay);

    
    mode = 0;
    
    //fading FBO
    
        // FBO
    rgbaFbo.allocate(ofGetScreenWidth(), ofGetScreenHeight(), GL_RGBA);
    rgbaFboFloat.allocate(ofGetScreenWidth(), ofGetScreenHeight(), GL_RGBA32F_ARB);
    
        // clear once otherwise glitchy memories would show up
    rgbaFbo.begin();
	ofClear(255,255,255, 0);
    rgbaFbo.end();
    
    rgbaFboFloat.begin();
	ofClear(255,255,255, 0);
    rgbaFboFloat.end();
    
    fadeAmnt=10;

    
    // back particles
    
    for (int i = 0; i < NUM_PARTICLES; i++){
		particle * myParticle = new particle;
        myParticle->setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
		particles2.push_back(myParticle);
	}
    
    
    // front particles
    for (int i = 0; i < 50; i++){
		particle myParticle;
        myParticle.setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
        particles.push_back(myParticle);
	}
    
    
    
    // FBO Particles
    posFbo.allocate(NUM_PARTICLES, 1, GL_RGB32F);
    for (int i = 0; i < NUM_PARTICLES; i ++) {
        pos[i].x = ofRandom(0, ofGetWidth());
        pos[i].y = ofRandom(0, ofGetHeight());
        float speed = ofRandom(0.5, 3.0);
        float angle = ofRandom(0.0, TWO_PI);
        vec[i].x = cos(angle) * speed;
        vec[i].y = sin(angle) * speed;
    }
    
    
    
    // Sound input
	int bufferSize = 256;
	
	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	volHistory.assign(400, 0.0);
	bufferCounter	= 0;
	drawCounter		= 0;
	smoothedVol     = 0.0;
    scaledVol		= 0.0;
	soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
    
    
    
    //SHADER CHUNK
    ofShader* shader;
    string shaderProgram;

                    
                    //--------------------------------------------------------------
                    
                    shader = new ofShader();
                    shaderProgram = STRINGIFY(
                                              uniform sampler2DRect tex0;
                                              uniform sampler2DRect tex1;
                                              uniform int numParticles;
                                              uniform int width;
                                              uniform int height;
                                              void main (void){
                                                  bool flag = false;
                                                  for (int i = 0; i < numParticles; i ++) {
                                                      vec2 posSample = texture2DRect(tex1, vec2(i, 0) + vec2(0.5, 0.5)).rg;
                                                      posSample.r *= float(width);
                                                      posSample.g *= float(height);
                                                      float dist = distance(posSample, gl_TexCoord[0].st);
                                                      if (dist < 3.0) {
                                                          flag = true;
                                                      }
                                                  }
                                                  gl_FragColor = (flag) ? vec4(1.0, 1.0, 0.0, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
                                              }
                                              );
                    
                    
                    
                    shader->setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
                    shader->linkProgram();
                    shaders.push_back(shader);
                    
                    //--------------------------------------------------------------
                    
                    shader = new ofShader();
                    shaderProgram = STRINGIFY(
                                              uniform sampler2DRect tex0;
                                              uniform sampler2DRect tex1;
                                              uniform int numParticles;
                                              uniform int width;
                                              uniform int height;
                                              void main (void){
                                                  bool flag = false;
                                                  float lev = 0.0;
                                                  for (int i = 0; i < numParticles; i ++) {
                                                      vec2 posSample = texture2DRect(tex1, vec2(i, 0) + vec2(0.5, 0.5)).rg;
                                                      posSample.r *= float(width);
                                                      posSample.g *= float(height);
                                                      float dist = distance(posSample, gl_TexCoord[0].st);
                                                      lev += abs(cos(dist * 0.1)) / float(numParticles) / dist * 100.0;
                                                      if (dist < 3.0) {
                                                          flag = true;
                                                      }
                                                  }
                                                  gl_FragColor = (flag) ? vec4(1.0, 1.0, 0.0, 1.0) : vec4(lev, lev, lev, 1.0);
                                              }
                                              );
                    
                    
                    
                    shader->setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
                    shader->linkProgram();
                    shaders.push_back(shader);
                    
                    //--------------------------------------------------------------
                    
                    shader = new ofShader();
                    shaderProgram = STRINGIFY(
                                              uniform sampler2DRect tex0;
                                              uniform sampler2DRect tex1;
                                              uniform int numParticles;
                                              uniform int width;
                                              uniform int height;
                                              void main (void){
                                                  bool flag = false;
                                                  float minDist = 2000.0;
                                                  for (int i = 0; i < numParticles; i ++) {
                                                      vec2 posSample = texture2DRect(tex1, vec2(i, 0) + vec2(0.5, 0.5)).rg;
                                                      posSample.r *= float(width);
                                                      posSample.g *= float(height);
                                                      float dist = distance(posSample, gl_TexCoord[0].st);
                                                      if (dist < minDist) {
                                                          minDist = dist;
                                                      }
                                                      if (dist < 3.0) {
                                                          flag = true;
                                                      }
                                                  }
                                                  float lev = minDist / 200.0;
                                                  gl_FragColor = (flag) ? vec4(1.0, 1.0, 0.0, 1.0) : vec4(lev, lev, lev, 1.0);
                                              }
                                              );
                        
                    shader->setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
                    shader->linkProgram();
                    shaders.push_back(shader);
                    
                    //--------------------------------------------------------------
                    
                    shader = new ofShader();
                    shaderProgram = STRINGIFY(
                                              uniform sampler2DRect tex0;
                                              uniform sampler2DRect tex1;
                                              uniform int numParticles;
                                              uniform int width;
                                              uniform int height;
                                              void main (void){
                                                  bool flag = false;
                                                  float lev = 0.0;
                                                  for (int i = 0; i < numParticles; i ++) {
                                                      vec2 posSample = texture2DRect(tex1, vec2(i, 0) + vec2(0.5, 0.5)).rg;
                                                      posSample.r *= float(width);
                                                      posSample.g *= float(height);
                                                      float dist = distance(posSample, gl_TexCoord[0].st);
                                                      lev += cos(dist * 0.05) / float(numParticles);
                                                      if (dist < 3.0) {
                                                          flag = true;
                                                      }
                                                  }
                                                  gl_FragColor = (flag) ? vec4(1.0, 1.0, 0.0, 1.0) : vec4(lev, lev, lev, 1.0);
                                              }
                                              );
                    
                    //--------------------------------------------------------------
                    shader = new ofShader();
                    shaderProgram = STRINGIFY(
                                              uniform sampler2DRect tex0;
                                              uniform sampler2DRect tex1;
                                              uniform int numParticles;
                                              uniform int width;
                                              uniform int height;
                                              void main (void){
                                                  bool flag = false;
                                                  float minDist = 2000.0;
                                                  for (int i = 0; i < numParticles; i ++) {
                                                      vec2 posSample = texture2DRect(tex1, vec2(i, 0) + vec2(0.5, 0.5)).rg;
                                                      posSample.r *= float(width);
                                                      posSample.g *= float(height);
                                                      float dist = distance(posSample, gl_TexCoord[0].st);
                                                      if (dist < minDist) {
                                                          minDist = dist;
                                                      }
                                                      if (dist < 3.0) {
                                                          flag = true;
                                                      }
                                                  }
                                                  float lev = pow(cos(minDist * 0.2), 2.0);
                                                  gl_FragColor = (flag) ? vec4(1.0, 1.0, 0.0, 1.0) : vec4(lev, lev, lev, 1.0);
                                              }
                                              );
                    
                    shader->setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
                    shader->linkProgram();
                    shaders.push_back(shader);
                    
                    //--------------------------------------------------------------
                    
                    shader = new ofShader();
                    shaderProgram = STRINGIFY(
                                              uniform sampler2DRect tex0;
                                              uniform sampler2DRect tex1;
                                              uniform int numParticles;
                                              uniform int width;
                                              uniform int height;
                                              void main (void){
                                                  bool flag = false;
                                                  float lev = 0.0;
                                                  for (int i = 0; i < numParticles; i ++) {
                                                      vec2 posSample = texture2DRect(tex1, vec2(i, 0) + vec2(0.5, 0.5)).rg;
                                                      posSample.r *= float(width);
                                                      posSample.g *= float(height);
                                                      float dist = distance(posSample, gl_TexCoord[0].st);
                                                      float pow = max(0.0, 200.0 - dist) / 200.0;
                                                      lev += pow * pow;
                                                      if (dist < 3.0) {
                                                          flag = true;
                                                      }
                                                  }
                                                  gl_FragColor = (flag) ? vec4(1.0, 1.0, 0.0, 1.0) :
                                                  (lev < 0.5 ) ? vec4(1.0, 1.0, 1.0, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
                                              }
                                              );
                    
                    shader->setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
                    shader->linkProgram();
                    shaders.push_back(shader);
                    
                    //--------------------------------------------------------------
                    
                    shader = new ofShader();
                    shaderProgram = STRINGIFY(
                                              uniform sampler2DRect tex0;
                                              uniform sampler2DRect tex1;
                                              uniform int numParticles;
                                              uniform int width;
                                              uniform int height;
                                              void main (void){
                                                  bool flag = false;
                                                  float lev = 0.0;
                                                  for (int i = 0; i < numParticles; i ++) {
                                                      vec2 posSample = texture2DRect(tex1, vec2(i, 0) + vec2(0.5, 0.5)).rg;
                                                      posSample.r *= float(width);
                                                      posSample.g *= float(height);
                                                      float dist = distance(posSample, gl_TexCoord[0].st);
                                                      lev += cos(dist * 0.05) / float(numParticles);
                                                      if (dist < 3.0) {
                                                          flag = true;
                                                      }
                                                  }
                                                  gl_FragColor = (flag) ? vec4(1.0, 1.0, 0.0, 1.0) : vec4(lev, lev, lev, 1.0);
                                              }
                                              );
                    
                    
                    
                    shader->setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
                    shader->linkProgram();
                    shaders.push_back(shader);
                   
                    //--------------------------------------------------------------
                    
                    shader = new ofShader();
                    shaderProgram = STRINGIFY(
                                              uniform sampler2DRect tex0;
                                              uniform sampler2DRect tex1;
                                              uniform int numParticles;
                                              uniform int width;
                                              uniform int height;
                                              void main (void){
                                                  bool flag = false;
                                                  int ind0 = 0;
                                                  int ind1 = 0;
                                                  int ind2 = 0;
                                                  float minDist = 2000.0;
                                                  for (int i = 0; i < numParticles; i ++) {
                                                      vec2 posSample = texture2DRect(tex1, vec2(i, 0) + vec2(0.5, 0.5)).rg;
                                                      posSample.r *= float(width);
                                                      posSample.g *= float(height);
                                                      float dist = distance(posSample, gl_TexCoord[0].st);
                                                      if (dist < minDist) {
                                                          ind2 = ind1; ind1 = ind0; ind0 = i;
                                                          minDist = dist;
                                                      }
                                                      if (dist < 3.0) {
                                                          flag = true;
                                                      }
                                                  }
                                                  float g = float(ind0) / float(numParticles);
                                                  float b = float(ind1) / float(numParticles);
                                                  float r = float(ind2) / float(numParticles);
                                                  gl_FragColor = (flag) ? vec4(1.0, 1.0, 0.0, 1.0) : vec4(r, g, b, 1.0);
                                              }
                                              );
                    
                    
                    
                    shader->setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
                    shader->linkProgram();
                    shaders.push_back(shader);
                    
                    //--------------------------------------------------------------
                    
                    shader = new ofShader();
                    shaderProgram = STRINGIFY(
                                              uniform sampler2DRect tex0;
                                              uniform sampler2DRect tex1;
                                              uniform int numParticles;
                                              uniform int width;
                                              uniform int height;
                                              void main (void){
                                                  bool flag = false;
                                                  float PI = 3.14159265;
                                                  float rSum = 0.0;
                                                  float gSum = 0.0;
                                                  float bSum = 0.0;
                                                  for (int i = 0; i < numParticles; i ++) {
                                                      vec2 posSample = texture2DRect(tex1, vec2(i, 0) + vec2(0.5, 0.5)).rg;
                                                      posSample.r *= float(width);
                                                      posSample.g *= float(height);
                                                      float dist = distance(posSample, gl_TexCoord[0].st);
                                                      float angle = float(i) / float(numParticles) * PI;
                                                      float adj = 1.0 / float(numParticles) / dist * 100.0;
                                                      rSum += (sin(angle) + 1.0) / 2.0 * adj;
                                                      gSum += (sin(angle + PI / 3.0) + 1.0) / 2.0 * adj;
                                                      bSum += (sin(angle + PI / 3.0) + 1.0) / 2.0 * adj;
                                                      if (dist < 3.0) {
                                                          flag = true;
                                                      }
                                                  }
                                                  gl_FragColor = (flag) ? vec4(1.0, 1.0, 0.0, 1.0) : vec4(rSum, gSum, bSum, 1.0);
                                              }
                                              );
                    
                    
                    
                    shader->setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
                    shader->linkProgram();
                    shaders.push_back(shader);
                    
                    //--------------------------------------------------------------
                    
                    shader = new ofShader();
                    shaderProgram = STRINGIFY(
                                              uniform sampler2DRect tex0;
                                              uniform sampler2DRect tex1;
                                              uniform int numParticles;
                                              uniform int width;
                                              uniform int height;
                                              void main (void){
                                                  bool flag = false;
                                                  float PI = 3.14159265;
                                                  vec2 pos0 = vec2(0.0);
                                                  vec2 pos1 = vec2(0.0);
                                                  float minDist = 2000.0;
                                                  for (int i = 0; i < numParticles; i ++) {
                                                      vec2 posSample = texture2DRect(tex1, vec2(i, 0) + vec2(0.5, 0.5)).rg;
                                                      posSample.r *= float(width);
                                                      posSample.g *= float(height);
                                                      float dist = distance(posSample, gl_TexCoord[0].st);
                                                      if (dist < minDist) {
                                                          pos1 = pos0;
                                                          pos0 = posSample;
                                                          minDist = dist;
                                                      }
                                                      if (dist < 3.0) {
                                                          flag = true;
                                                      }
                                                  }
                                                  vec2 average = (pos0 + pos1) / 2.0;
                                                  float angle = atan(average.r - gl_TexCoord[0].s, average.g- gl_TexCoord[0].t);
                                                  
                                                  float r = (sin(angle) + 1.0) / 2.0 / max(1.0, minDist * 0.05);
                                                  float g = (sin(angle + PI / 3.0) + 1.0) / 2.0 / max(1.0, minDist * 0.05);
                                                  float b = (sin(angle + PI / 3.0 * 2.0) + 1.0) / 2.0 / max(1.0, minDist * 0.05);
                                                  gl_FragColor = (flag) ? vec4(1.0, 1.0, 0.0, 1.0) : vec4(r, g, b, 1.0);
                                              }
                                              );
                    
                    
                    
                    shader->setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
                    shader->linkProgram();
                    shaders.push_back(shader);
                   
                    //--------------------------------------------------------------
                    
                    shader = new ofShader();
                    shaderProgram = STRINGIFY(
                                              uniform sampler2DRect tex0;
                                              uniform sampler2DRect tex1;
                                              uniform int numParticles;
                                              uniform int width;
                                              uniform int height;
                                              void main (void){
                                                  bool flag = false;
                                                  float PI = 3.14159265;
                                                  float rSum = 0.0;
                                                  float gSum = 0.0;
                                                  float bSum = 0.0;
                                                  for (int i = 0; i < numParticles; i ++) {
                                                      vec2 posSample = texture2DRect(tex1, vec2(i, 0) + vec2(0.5, 0.5)).rg;
                                                      posSample.r *= float(width);
                                                      posSample.g *= float(height);
                                                      float dist = distance(posSample, gl_TexCoord[0].st);
                                                      float angle = float(i) / float(numParticles) * PI;
                                                      float adj = 1.0 / float(numParticles) / dist * 100.0;
                                                      rSum += (sin(angle) + 1.0) / 2.0 * adj;
                                                      gSum += (sin(angle + PI / 3.0) + 1.0) / 2.0 * adj;
                                                      bSum += (sin(angle + PI / 3.0) + 1.0) / 2.0 * adj;
                                                      if (dist < 3.0) {
                                                          flag = true;
                                                      }
                                                  }
                                                  float r = sin(rSum * PI * 5.0);
                                                  float g = sin(gSum * PI * 5.0);
                                                  float b = sin(bSum * PI * 5.0);
                                                  gl_FragColor = (flag) ? vec4(1.0, 1.0, 0.0, 1.0) : vec4(r, g, b, 1.0);
                                              }
                                              );
                    
                    
                    
                    shader->setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
                    shader->linkProgram();
                    shaders.push_back(shader);
                    
                    //--------------------------------------------------------------
                    
                    shader = new ofShader();
                    shaderProgram = STRINGIFY(
                                              uniform sampler2DRect tex0;
                                              uniform sampler2DRect tex1;
                                              uniform int numParticles;
                                              uniform int width;
                                              uniform int height;
                                              void main (void){
                                                  bool flag = false;
                                                  float PI = 3.14159265;
                                                  float r = 0.0;
                                                  float g = 0.0;
                                                  float b = 0.0;
                                                  for (int i = 0; i < numParticles; i ++) {
                                                      vec2 posSample = texture2DRect(tex1, vec2(i, 0) + vec2(0.5, 0.5)).rg;
                                                      posSample.r *= float(width);
                                                      posSample.g *= float(height);
                                                      float dist = distance(posSample, gl_TexCoord[0].st);
                                                      if (abs(posSample.r - gl_TexCoord[0].s) < 2.0) {
                                                          r += 0.3;
                                                          b += 0.2;
                                                      }
                                                      if (abs(posSample.g - gl_TexCoord[0].t) < 2.0) {
                                                          g += 0.3;
                                                          b += 0.2;
                                                      }
                                                      if (dist < 3.0) {
                                                          flag = true;
                                                      }
                                                  }
                                                  gl_FragColor = (flag) ? vec4(1.0, 1.0, 0.0, 1.0) : vec4(r, g, b, 1.0);
                                              }
                                              );
                    
                    
                    
                    shader->setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
                    shader->linkProgram();
                    shaders.push_back(shader);
                    
                    //--------------------------------------------------------------
                    
                    shader = new ofShader();
                    shaderProgram = STRINGIFY(
                                              uniform sampler2DRect tex0;
                                              uniform sampler2DRect tex1;
                                              uniform int numParticles;
                                              uniform int width;
                                              uniform int height;
                                              void main (void){
                                                  bool flag = false;
                                                  float PI = 3.14159265;
                                                  float r = 0.0;
                                                  float g = 0.0;
                                                  float b = 0.0;
                                                  for (int i = 0; i < numParticles; i ++) {
                                                      vec2 posSample = texture2DRect(tex1, vec2(i, 0) + vec2(0.5, 0.5)).rg;
                                                      posSample.r *= float(width);
                                                      posSample.g *= float(height);
                                                      float dist = distance(posSample, gl_TexCoord[0].st);
                                                      r += sin((posSample.r - gl_TexCoord[0].s) * 0.002 * float(i)) * 0.05;
                                                      g += sin((posSample.g - gl_TexCoord[0].t) * 0.002 * float(i)) * 0.05;
                                                      b += sin(r + g) * 0.2;
                                                      if (dist < 3.0) {
                                                          flag = true;
                                                      }
                                                  }
                                                  gl_FragColor = (flag) ? vec4(1.0, 1.0, 0.0, 1.0) : vec4(r, g, b, 1.0);
                                              }
                                              );
                    
                    
                    
                    shader->setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
                    shader->linkProgram();
                    shaders.push_back(shader);
                    
                    //--------------------------------------------------------------
                    
                    shader = new ofShader();
                    shaderProgram = STRINGIFY(
                                              uniform sampler2DRect tex0;
                                              uniform sampler2DRect tex1;
                                              uniform int numParticles;
                                              uniform int width;
                                              uniform int height;
                                              void main (void){
                                                  bool flag = false;
                                                  float PI = 3.14159265;
                                                  float r = 0.0;
                                                  float g = 0.0;
                                                  float b = 0.0;
                                                  for (int i = 0; i < numParticles; i ++) {
                                                      vec2 posSample = texture2DRect(tex1, vec2(i, 0) + vec2(0.5, 0.5)).rg;
                                                      posSample.r *= float(width);
                                                      posSample.g *= float(height);
                                                      float dist = distance(posSample, gl_TexCoord[0].st);
                                                      b += sin((posSample.r - gl_TexCoord[0].s) * 0.001 * float(i)) * 0.03;
                                                      g += sin((posSample.g - gl_TexCoord[0].t) * 0.001 * float(i)) * 0.03;
                                                      r += sin(b + g) * 0.2;
                                                      if (dist < 3.0) {
                                                          flag = true;
                                                      }
                                                  }
                                                  b = min(r, b);
                                                  g = min(g, r);
                                                  gl_FragColor = (flag) ? vec4(1.0, 1.0, 0.0, 1.0) : vec4(r, g, b, 1.0);
                                              }
                                              );
                    
                    
                    
                    shader->setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
                    shader->linkProgram();
                    shaders.push_back(shader);
                    

                    //--------------------------------------------------------------
    shaderIndex = 0;
    renderImage.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void testApp::update(){
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    //fbo
    rgbaFboFloat.begin();
    drawFboTest();
	rgbaFboFloat.end();
    
    // sort all the particles:
	sort( particles2.begin(), particles2.end(), comparisonFunction );
    
    // PARTICLES2 SYSTEM MODES
    
                //----------
                if (mode == 0) {

                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->resetForce();
                    }
                    //flocking compare only of close enough
                    for (int i = 0; i < particles2.size(); i++){
                        for (int j = i-1; j >= 0; j--){
                            if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >	50) break;
                            if (i != j) particles2[i]->addForFlocking(*particles2[j]);
                        }
                        particles2[i]->addRepulsionForce(mouseX, mouseY, 40, 0.4);
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->addFlockingForce();
                        particles2[i]->addDampingForce();
                        particles2[i]->bounceOffWalls();
                        particles2[i]->update();
                    }
                    
                }
                //----------
                if (mode == 1) {
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->cohesion.strength = 0.1;
                        particles2[i]->cohesion.distance = 50;
                        particles2[i]->alignment.strength =  0.7;
                        particles2[i]->alignment.distance = 42;
                        particles2[i]->seperation.strength = 0.7;
                        particles2[i]->seperation.distance = 38;
                        particles2[i]->damping = 0.08;
                    }
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->resetForce();
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        for (int j = i-1; j >= 0; j--){
                            if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >	50) break;
                            if (i != j){
                                particles2[i]->addForFlocking(*particles2[j]);
                            }
                        }
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->addFlockingForce();
                        particles2[i]->addDampingForce();
                        particles2[i]->bounceOffWalls();
                        particles2[i]->update();
                    }

                }
                //----------
                if (mode == 2) {
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->cohesion.strength = 0.76;
                        particles2[i]->cohesion.distance = 50;
                        particles2[i]->alignment.strength =  0.5;
                        particles2[i]->alignment.distance = 130;
                        particles2[i]->seperation.strength = 0.4;
                        particles2[i]->seperation.distance = 157;
                        particles2[i]->damping = 0.192;
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->resetForce();
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        for (int j = i-1; j >= 0; j--){
                            if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >	50) break;
                            if (i != j){
                                particles2[i]->addForFlocking(*particles2[j]);
                            }
                        }
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->addFlockingForce();
                        particles2[i]->addDampingForce();
                        particles2[i]->update();
                    }
                }
                //----------
                if (mode == 3) {
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->cohesion.strength = 0.2;
                        particles2[i]->cohesion.distance = 200;
                        particles2[i]->alignment.strength =  0.24;
                        particles2[i]->alignment.distance = 198;
                        particles2[i]->seperation.strength = 0.4;
                        particles2[i]->seperation.distance = 33;
                        particles2[i]->damping = 0.029;
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->resetForce();
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        for (int j = i-1; j >= 0; j--){
                            if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >	50) break;
                            if (i != j){
                                particles2[i]->addForFlocking(*particles2[j]);
                            }
                        }
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->addFlockingForce();
                        particles2[i]->addDampingForce();
                        particles2[i]->update();
                    }
                }
                //----------
                if (mode == 4) {
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->cohesion.strength = 0.19;
                        particles2[i]->cohesion.distance = 200;
                        particles2[i]->alignment.strength =  0.2;
                        particles2[i]->alignment.distance = 175;
                        particles2[i]->seperation.strength = 0.46;
                        particles2[i]->seperation.distance = 200;
                        particles2[i]->damping = 0.45;
                    }
                    
                    
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->resetForce();
                    }
                    
                    
                    for (int i = 0; i < particles2.size(); i++){
                        for (int j = i-1; j >= 0; j--){
                            if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >	50) break;
                            if (i != j){
                                particles2[i]->addForFlocking(*particles2[j]);
                            }
                        }
                    }
                    
                    
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->addFlockingForce();
                        particles2[i]->addDampingForce();
                        particles2[i]->update();
                    }
                }
                //----------
                if (mode == 5) {
                    
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->cohesion.strength = 0.5;
                        particles2[i]->cohesion.distance = 100;
                        particles2[i]->alignment.strength =  0.38;
                        particles2[i]->alignment.distance = 52;
                        particles2[i]->seperation.strength = 0.9;
                        particles2[i]->seperation.distance = 15;
                        particles2[i]->damping = 0.68;
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->resetForce();
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        for (int j = i-1; j >= 0; j--){
                            if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >	50) break;
                            if (i != j){
                                particles2[i]->addForFlocking(*particles2[j]);
                            }
                        }
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->addFlockingForce();
                        particles2[i]->addDampingForce();
                        particles2[i]->update();
                    }
                }
                //----------
                if (mode == 6) {
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->cohesion.strength = 0.20;
                        particles2[i]->cohesion.distance = 10;
                        particles2[i]->alignment.strength =  0.31;
                        particles2[i]->alignment.distance = 200;
                        particles2[i]->seperation.strength = 0.4;
                        particles2[i]->seperation.distance = 2;
                        particles2[i]->damping = 0.20;
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->resetForce();
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        for (int j = i-1; j >= 0; j--){
                            if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >50) break;
                            if (i != j){
                                particles2[i]->addForFlocking(*particles2[j]);
                            }
                        }
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->addFlockingForce();
                        particles2[i]->addDampingForce();
                        particles2[i]->update();
                    }

                }
                //----------
                if (mode == 7) {
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->cohesion.strength = 0.20;
                        particles2[i]->cohesion.distance = 100;
                        particles2[i]->alignment.strength =  0.31;
                        particles2[i]->alignment.distance = 20;
                        particles2[i]->seperation.strength = 0.4;
                        particles2[i]->seperation.distance = 2;
                        particles2[i]->damping = 0.8;
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->resetForce();
                    }
                
                    for (int i = 0; i < particles2.size(); i++){
                        for (int j = i-1; j >= 0; j--){
                            if ( fabs(particles2[j]->pos.x - particles2[i]->pos.x) >50) break;
                            if (i != j){
                                particles2[i]->addForFlocking(*particles2[j]);
                            }
                        }
                    }
                    
                    for (int i = 0; i < particles2.size(); i++){
                        particles2[i]->addFlockingForce();
                        particles2[i]->addDampingForce();
                        particles2[i]->update();
                    }
                }
    
    // PARTICLES1 SYSTEM MODES
                //----------
                if (mode == 11) {
                    
                    
                    for (int i = 0; i < particles.size(); i++){
                        particles[i].resetForce();
                    }
                    for (int i = 0; i < particles.size(); i++){
                        particles[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
                        particles[i].alignment.strength = scaledVol*2000;
                        particles[i].c.set(255,ofRandom(100,250),255);
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
                }
                //----------
                if (mode == 12) {
                    for (int i = 0; i < particles.size(); i++){
                        particles[i].resetForce();
                    }
                    
                    for (int i = 0; i < particles.size(); i++){
                        particles[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
                        for (int j = 0; j < i; j++){
                            if (bRepel){
                                particles[i].addRepulsionForce(particles[j], radius + smoothedVol * 3000, strength);
                            } else {
                                particles[i].addAttractionForce(particles[j], radius , strength);
                            }
                        }
                    }
                    for (int i = 0; i < particles.size(); i++){
                        particles[i].addDampingForce();
                        particles[i].damping = 0.1;
                        particles[i].update();
                    }
                }
                //----------
                if (mode == 13) {
                    for (int i = 0; i < particles.size(); i++){
                        particles[i].resetForce();
                    }
                    
                    for (int i = 0; i < particles.size(); i++){
                        for (int j = 0; j < particles.size(); j++){
                            if (i != j){
                                particles[i].addForFlocking(particles[j]);
                            }
                        }
                    }
                    
                    for (int i = 0; i < particles.size(); i++){
                        particles[i].damping = smoothedVol*200;
                    }
                    
                    for (int i = 0; i < particles.size(); i++){
                        particles[i].addFlockingForce();
                        particles[i].addDampingForce();
                        particles[i].update();
                    }
                }
                //----------
                if (mode == 14) {
                    
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
                        particles[i].addFlockingForce();
                        particles[i].addDampingForce();
                        particles[i].update();
                    }
                    for (int i = 0; i < particles.size(); i++){
                        particles[i].seperation.distance = smoothedVol*1000 ;
                        particles[i].seperation.strength = 23;
                        particles[i].damping = 0.13;
                        particles[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
                    }
                }
                //----------
                if (mode == 15) {
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
                        particles[i].addFlockingForce();
                        particles[i].cohesion.strength = smoothedVol*100;
                        particles[i].damping = 0.05;
                        particles[i].addDampingForce();
                        particles[i].update();
                    }
                }
                //----------
                if (mode == 16) {
                    for (int i = 0; i < particles.size(); i++){
                        particles[i].resetForce();
                    }
                    for (int i = 0; i < particles.size(); i++){
                        
                        particles[i].addAttractionForce(mouseX, mouseY, 1000, 0.5);
                        particles[i].c.set(ofRandom(100,255), ofRandom(100,255), ofRandom(100,255));
                        
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
                
    
    // FBO Pixels
    ofFloatPixels pixels;
    posFbo.readToPixels(pixels);
    for (int i = 0; i < NUM_PARTICLES; i++){        
        pixels[i * 3 + 0] = particles2[i]->pos.x / ofGetWidth();
        pixels[i * 3 + 1] = particles2[i]->pos.y / ofGetHeight();
    }
    
    posFbo.getTextureReference().loadData(pixels);
    
    
    
    
    //OSC
    	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		if(timers[i] < ofGetElapsedTimef()){
			msg_strings[i] = "";
		}
	}
	while(receiver.hasWaitingMessages()){
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        for (int i = 0; i < particles.size(); i++){
            
            if(m.getAddress() == "/accel/x"){
                particles2[i]->r3 = m.getArgAsFloat(0);
                particles2[i]->vel.x += m.getArgAsFloat(0)*10;
                
            }
            
            if(m.getAddress() == "/accel/y"){
                particles2[i]->r2 = m.getArgAsFloat(0);
                particles2[i]->vel.y += m.getArgAsFloat(0)*10;
            }
            
            if(m.getAddress() == "/accel/z"){
                particles2[i]->r1 = m.getArgAsFloat(0);
                particles2[i]->vel.z += m.getArgAsFloat(0)*10;
            }
            
            if(m.getAddress() == "/touch/state"){
                if (m.getArgAsInt32(0) == 1) {
                    bTouchDown = true;
                }
                if (m.getArgAsInt32(0) == 0) {
                    bTouchDown = false;
                }
            }
            
            if(m.getAddress() == "/touch/position/x"){
                particles2[i]->c.setHue(m.getArgAsFloat(0));
            }
            
            if(m.getAddress() == "/touch/position/y"){
                particles2[i]->c.setSaturation(m.getArgAsFloat(0));
            }
            
            else{
                // unrecognized message: display on the bottom of the screen
                string msg_string;
                msg_string = m.getAddress();
                msg_string += ": ";
                for(int i = 0; i < m.getNumArgs(); i++){
                    // get he argument type
                    msg_string += m.getArgTypeName(i);
                    msg_string += ":";
                    // display the argument - make sure we get the right type
                    if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                        msg_string += ofToString(m.getArgAsInt32(i));
                    }
                    else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                        msg_string += ofToString(m.getArgAsFloat(i));
                    }
                    else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                        msg_string += m.getArgAsString(i);
                    }
                    else{
                        msg_string += "unknown";
                    }
                }
                // add to the list of strings to display
                msg_strings[current_msg_string] = msg_string;
                timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
                current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
                // clear the next line
                msg_strings[current_msg_string] = "";
            }
        }
	}

    
    
    
}
//--------------------------------------------------------------
void testApp::drawFboTest(){
	
    if(ofGetKeyPressed('c')){
		ofClear(255,255,255, 0);
	}
	
	ofFill();
    ofColor dark(10,fadeAmnt);
    ofColor black(0, fadeAmnt);
    
    ofBackgroundGradient(dark, black);
    
    if(mode== 1) {
        fadeAmnt = 100;
        ofSetColor(255,255,255, fadeAmnt);
    }
    
    if(mode== 2) {
        fadeAmnt = 60;
        ofSetColor(255,0,0, fadeAmnt);
    }
    
    if(mode== 3) {
        fadeAmnt = 30;
        ofSetColor(0,255,255, fadeAmnt);
    }
    
    if(mode== 4) {
        fadeAmnt = 5;
        ofSetColor(10,10,10, fadeAmnt);
    }
    
    if(mode== 5) {
        fadeAmnt = 10;
        ofSetColor(10,255,10, fadeAmnt);
    }
    
    
    if(mode== 6) {
        fadeAmnt = 20;
        ofSetColor(10,00,25, fadeAmnt);
    }
    
    
    
    shaders[shaderIndex]->begin();
    shaders[shaderIndex]->setUniform1i("numParticles", NUM_PARTICLES);
    shaders[shaderIndex]->setUniform1i("width", ofGetWidth());
    shaders[shaderIndex]->setUniform1i("height", ofGetHeight());
    shaders[shaderIndex]->setUniformTexture("tex0", renderImage.getTextureReference() , 0 );
    shaders[shaderIndex]->setUniformTexture("tex1", posFbo.getTextureReference() , 1 );
    
    renderImage.draw(0, 0);
    shaders[shaderIndex]->end();
	
	for (int i = 0; i < particles2.size(); i++){
		particles2[i]->draw();
	}
    
    for (int i = 0; i < particles.size(); i++){
		particles[i].draw();
	}
}


//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(255,50);
    rgbaFbo.draw(0,0);
    rgbaFboFloat.draw(0,0);
    
}


void testApp::audioIn(float * input, int bufferSize, int nChannels){
	
	float curVol = 0.0;
    int numCounted = 0;
    
	for (int i = 0; i < bufferSize; i++){
		left[i]		= input[i*2]*0.5;
		right[i]	= input[i*2+1]*0.5;
        
		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;
	}
	//rms volume
	curVol /= (float)numCounted;
	curVol = sqrt( curVol );
	
	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;
	
	bufferCounter++;	
}


void testApp::keyPressed(int key){
    if( key == '1' ) mode=1;
    if( key == '2' ) mode=2;
    if( key == '3' ) mode=3;
    if( key == '4' ) mode=4;
    if( key == '5' ) mode=5;
    if( key == '6' ) mode=6;
    if( key == '7' ) mode=7;
    if( key == 'q' || 'Q') mode=11;
    if( key == 'w' ) mode=12;
    if( key == 'e' ) mode=13;
    if( key == 'r' ) mode=14;
    if( key == 't' ) mode=15;
    if( key == 'y' ) mode=16;
    if( key == 'u' ) mode=17;
    if (key == 'f' ) ofToggleFullscreen();
    
    if (key == ' ' ) {
        shaderIndex = (shaderIndex + 1) % shaders.size();
        cout << shaderIndex << " ";
    }
}

void testApp::keyReleased(int key){ }
void testApp::mouseMoved(int x, int y){}
void testApp::mouseDragged(int x, int y, int button){
    
    if (mode == 11){
        for (int i = 0; i < particles.size(); i++){
            particles[i].c.set((255,ofRandom(100,250),255));
        }
    }
    
    if (mode == 12){
        for (int i = 0; i < particles.size(); i++){
            particles[i].c.set(0,ofRandom(100,255),ofRandom(100,255),90);
        }
    }
    
    if (mode == 13){
        for (int i = 0; i < particles.size(); i++){
            particles[i].c.set(smoothedVol*1000+200,ofRandom(150),ofRandom(150), smoothedVol*10000);
        }
    }
    
    if (mode == 14){
        for (int i = 0; i < particles.size(); i++){
            particles[i].c.set(ofRandom(100,255),255 ,255,90);
        }
    }
    
    if (mode == 15){
        for (int i = 0; i < particles.size(); i++){
            particles[i].c.set(255, ofRandom(100,255), 255,90);
        }
    }
    
    if (mode == 16){
        for (int i = 0; i < particles.size(); i++){
            particles[i].c.set(255, 0, ofRandom(100,255),90);
        }
    }
    
    if (mode == 17){
        for (int i = 0; i < particles.size(); i++){
            particles[i].c.set(ofRandom(0,100),ofRandom(0,100), 0,90);
        }
    }
    
}
void testApp::mousePressed(int x, int y, int button){ }
void testApp::mouseReleased(int x, int y, int button){ }
void testApp::windowResized(int w, int h){ }
void testApp::gotMessage(ofMessage msg){ }
void testApp::dragEvent(ofDragInfo dragInfo){ }