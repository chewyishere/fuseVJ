#include "particle.h"
#include "ofMain.h"

//------------------------------------------------------------
particle::particle(){
	setInitialCondition(0,0,0,0);
	damping	= 0.01f;
	
	seperation.distance		= 20;
	alignment.distance		= 80;
	cohesion.distance		= 40;
	
	seperation.strength		= .03;
    alignment.strength		= .015;
	cohesion.strength		= .015;
    
    size = 0;
	
}

//------------------------------------------------------------
void particle::resetForce(){
    // we reset the forces every frame
    frc.set(0,0);
	
	// reset the flock info: 
	cohesion.count 	    = 0;
	seperation.count	= 0;
	alignment.count		= 0;
	
	cohesion.sum.set(0,0); 	
	seperation.sum.set(0,0); 	
	alignment.sum.set(0,0); 	

}

//------------------------------------------------------------
void particle::addForce(float x, float y){
    // add in a force in X and Y for this frame.
    frc.x = frc.x + x;
    frc.y = frc.y + y;
}

//------------------------------------------------------------
void particle::addRepulsionForce(float x, float y, float radius, float scale){
    
	// ----------- (1) make a vector of where this position is: 
	
	ofVec2f posOfForce;
	posOfForce.set(x,y);
	
	// ----------- (2) calculate the difference & length 
	
	ofVec2f diff	= pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
        diff.normalize();
		frc.x = frc.x + diff.x * scale * pct;
        frc.y = frc.y + diff.y * scale * pct;
    }
}

//------------------------------------------------------------
void particle::addAttractionForce(float x, float y, float radius, float scale){
    
	// ----------- (1) make a vector of where this position is: 
	
	ofVec2f posOfForce;
	posOfForce.set(x,y);
	
	// ----------- (2) calculate the difference & length 
	
	ofVec2f diff	= pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
		frc.x = frc.x - diff.x * scale * pct;
        frc.y = frc.y - diff.y * scale * pct;
    }
}

//------------------------------------------------------------
void particle::addRepulsionForce(particle &p, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is: 
	ofVec2f posOfForce;
	posOfForce.set(p.pos.x,p.pos.y);
	
	// ----------- (2) calculate the difference & length 
	
	ofVec2f diff	= pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
		frc.x = frc.x + diff.x * scale * pct;
        frc.y = frc.y + diff.y * scale * pct;
		p.frc.x = p.frc.x - diff.x * scale * pct;
        p.frc.y = p.frc.y - diff.y * scale * pct;
    }
}

//------------------------------------------------------------
void particle::addAttractionForce(particle & p, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is: 
	ofVec2f posOfForce;
	posOfForce.set(p.pos.x,p.pos.y);
	
	// ----------- (2) calculate the difference & length 
	
	ofVec2f diff	= pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
		frc.x = frc.x - diff.x * scale * pct;
        frc.y = frc.y - diff.y * scale * pct;
		p.frc.x = p.frc.x + diff.x * scale * pct;
        p.frc.y = p.frc.y + diff.y * scale * pct;
    }
	
}

//------------------------------------------------------------
void particle::addClockwiseForce(particle &p, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is: 
	ofVec2f posOfForce;
	posOfForce.set(p.pos.x,p.pos.y);
	
	// ----------- (2) calculate the difference & length 
	
	ofVec2f diff	= pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
		frc.x = frc.x - diff.y * scale * pct;
        frc.y = frc.y + diff.x * scale * pct;
		p.frc.x = p.frc.x + diff.y * scale * pct;
        p.frc.y = p.frc.y - diff.x * scale * pct;
    }
}

//------------------------------------------------------------
void particle::addCounterClockwiseForce(particle &p, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is: 
	ofVec2f posOfForce;
	posOfForce.set(p.pos.x,p.pos.y);
	
	// ----------- (2) calculate the difference & length 
	
	ofVec2f diff	= pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
		frc.x = frc.x + diff.y * scale * pct;
        frc.y = frc.y - diff.x * scale * pct;
			p.frc.x = p.frc.x - diff.y * scale * pct;
        p.frc.y = p.frc.y + diff.x * scale * pct;
		
    }
}


//------------------------------------------------------------
void particle::addForFlocking(particle &p){
	
	ofVec3f diff, diffNormalized;
	float distance;
	
	diff			= p.pos - pos;
	distance		= diff.length();
	diffNormalized	= diff;
	diffNormalized.normalize();

	if( distance > 0 && distance < seperation.distance ){
		seperation.sum += diffNormalized;
		seperation.count++;
	}
	
	if( distance > 0 && distance < alignment.distance ){
		alignment.sum += p.vel.getNormalized();
		alignment.count++;
	}
	
	if( distance > 0 && distance < cohesion.distance ){
		cohesion.sum += p.pos;
		cohesion.count++;
	}
}

//------------------------------------------------------------
void particle::addFlockingForce(){
	
	
	
	// seperation
	if(seperation.count > 0){							// let's add seperation :)
		seperation.sum /= (float)seperation.count;
		float sepFrc 	= seperation.strength;
		frc -= (seperation.sum.normalized()		    * sepFrc);
	}
	
	// alignment
	if(alignment.count > 0){
		alignment.sum /= (float)alignment.count;
		float alignFrc 	= alignment.strength;
		frc += (alignment.sum		* alignFrc);		// don't normalize the allignment, just use the average
	}
	
	// cohesion
	if(cohesion.count > 0){
		cohesion.sum /= (float)cohesion.count;
		cohesion.sum -= pos;
		float cohFrc 	= cohesion.strength;
		frc += (cohesion.sum.normalized()			* cohFrc);
	}
	
}


//------------------------------------------------------------
void particle::addDampingForce(){
	
	// the usual way to write this is  vel *= 0.99
	// basically, subtract some part of the velocity 
	// damping is a force operating in the oposite direction of the 
	// velocity vector
	
    frc.x = frc.x - vel.x * damping;
    frc.y = frc.y - vel.y * damping;
}

//------------------------------------------------------------
void particle::setInitialCondition(float px, float py, float vx, float vy){
    pos.set(px,py);
	vel.set(vx,vy);
    prevPos = pos;
    angle = atan2(py-ofGetWidth()/2,px-ofGetHeight()/2)+PI;
    c = ofColor::fromHsb(angle/TWO_PI*255,255,255);
}

//------------------------------------------------------------
void particle::update(){	
	
	//printf("vel frc %f %f, %f %f \n", vel.x, vel.y, frc.x, frc.y);
	
	vel = vel + frc;
	pos = pos + vel;
}

//------------------------------------------------------------
void particle::draw(){
    
	
	ofNoFill();
    ofSetColor(80);
//	ofCircle(pos.x, pos.y, 3);
	
	ofPoint velNormal = vel;
	velNormal.normalize();
	
	ofPoint velPerp;
	velPerp.x = -velNormal.y;
	velPerp.y = velNormal.x;
	
//	ofLine(pos.x, pos.y, pos.x + velNormal.x*10, pos.y + velNormal.y*10);
    
    
    float dx = pos.x - prevPos.x;
    float dy = pos.y - prevPos.y;
    
    angle = atan2(dy, dx);
    float diff;
    int clampMax = 100;
    
//    if (dx<100 && dy < 100) diff = (ofDist(0, 0, dx, dy))*5;
    diff = ofDist(0,0, vel.x, vel.y)*10;
    
    ofClamp(diff, 0, 100);
    size = diff * 0.1 + size * 0.9;
    
    prevPos.x = pos.x;
    prevPos.y = pos.y;
    
    ofEnableAlphaBlending();
    ofSetColor(c,150);
    ofFill();
    ofSetCircleResolution(3);
    ofPushMatrix();
    ofTranslate(pos.x, pos.y);
    ofRotateZ(angle * RAD_TO_DEG);
    ofCircle(0, 0, size);
    ofPopMatrix();
    ofSetCircleResolution(60);
    ofDisableAlphaBlending();
	   
}


//------------------------------------------------------------
void particle::bounceOffWalls(){
	
	// sometimes it makes sense to damped, when we hit
	bool bDampedOnCollision = true;
	bool bDidICollide = false;
	
	// what are the walls
	float minx = 0;
	float miny = 0;
	float maxx = ofGetWidth();
	float maxy = ofGetHeight();
	
	if (pos.x > maxx){
		pos.x = maxx; // move to the edge, (important!)
		vel.x *= -1;
		bDidICollide = true;
	} else if (pos.x < minx){
		pos.x = minx; // move to the edge, (important!)
		vel.x *= -1;
		bDidICollide = true;
	}
	
	if (pos.y > maxy){
		pos.y = maxy; // move to the edge, (important!)
		vel.y *= -1;
		bDidICollide = true;
	} else if (pos.y < miny){
		pos.y = miny; // move to the edge, (important!)
		vel.y *= -1;
		bDidICollide = true;
	}
	
	if (bDidICollide == true && bDampedOnCollision == true){
		vel *= 0.3;
	}
	
}
