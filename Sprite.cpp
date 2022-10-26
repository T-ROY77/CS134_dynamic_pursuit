#include "Sprite.h"

//
// Troy Perez - CS134 SJSU

BaseObject::BaseObject() {
	trans = glm::vec3(0, 0, 0);
	scale = ofVec3f(1, 1, 1);
	rot = 0;
}

void BaseObject::setPosition(glm::vec3 pos) {
	trans = pos;
}

//
// Basic Sprite Object
//
Sprite::Sprite() {
	speed = 1;
	velocity = ofVec3f(0, 0, 0);
	acceleration = ofVec3f(0, 0, 0);
	forces = ofVec3f(0, 0, 0);
	lifespan = -1;      // lifespan of -1 => immortal 
	birthtime = 0;
	bSelected = false;
	haveImage = false;
	name = "UnamedSprite";
	width = 20;
	height = 20;
	imageWidth = 20;
	imageHeight = 20;
	mass = 1;

}

// Return a sprite's age in milliseconds
//
float Sprite::age() {
	return (ofGetElapsedTimeMillis() - birthtime);
}

//  Set an image for the sprite. 
//
void Sprite::setImage(ofImage img) {
	image = img;
	haveImage = true;
	imageWidth = image.getWidth();
	imageHeight = image.getHeight();
}

glm::vec3 Sprite::heading() {
	glm::vec3 o = glm::vec3(0, -1, 0);
	o = glm::rotate(o, glm::radians(rot), glm::vec3(0, 0, 1));
	glm::normalize(o);
	return o;
}

void Sprite::moveSprite(glm::vec3 p) {
	glm::vec3 head = heading();
	trans += head * speed;



	//rotate agent to face the player
	ofVec3f headed = heading();
	ofVec3f rotVector = p - trans;
	ofVec3f v1 = rotVector.normalize();
	ofVec3f v2 = headed.normalize();

	float dot = v2.dot(v1);
	float theta = acos(dot);
	if (theta >= .1) {
		theta = glm::degrees(theta);
		rot += theta;
	}
	else {
		if (theta > -360) {
			theta = glm::degrees(theta);
			rot -= theta;
		}
	}
	
}

void Sprite::integrate() {


	// interval for this step
	//
	float dt = 1.0 / ofGetFrameRate();

	//rotational attraction force
	//rot += rotation force;

	// update position based on velocity
	//
	//position += (velocity * dt);
	trans += (velocity * dt);


	
	ofVec3f accel = acceleration;    // start with any acceleration already on the particle
	accel += (forces * (1.0 / mass));
	velocity += accel * dt;
	//velocity += accel;
	velocity *= damping;

	// clear forces on particle (they get re-added each step)
	//
	forces.set(0, 0, 0);
}


glm::mat4 Sprite::getMatrix() {
	glm::mat4 translate = glm::translate(glm::mat4(1.0), trans);
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0), glm::radians(rot),
		glm::vec3(0, 0, 1));
	glm::mat4 scalar = glm::scale(glm::mat4(1.0), scale);
	return (translate * rotate * scalar);
}

//  Render the sprite
//
void Sprite::draw() {

	ofSetColor(255, 255, 255, 255);

	// draw image centered
	//
	if (haveImage) {
		ofSetColor(ofColor::white);
		ofPushMatrix();
		ofMultMatrix(getMatrix());
		image.draw(-imageWidth / 2.0, -imageHeight / 2.0);
		ofPopMatrix();

	}
	else {
		// draw the triangle
		// 
		ofSetColor(ofColor::green);
		ofPushMatrix();
		ofMultMatrix(getMatrix());
		ofDrawTriangle(glm::vec3(-height, height, 0), glm::vec3(height, height, 0), glm::vec3(0, -height, 0));
		ofPopMatrix();

	}
}



//  Add a Sprite to the Sprite System
//
void SpriteSystem::add(Sprite s) {
	sprites.push_back(s);
}

// Remove a sprite from the sprite system. Note that this function is not currently
// used. The typical case is that sprites automatically get removed when the reach
// their lifespan.
//
void SpriteSystem::remove(int i) {
	sprites.erase(sprites.begin() + i);
}



// remove all sprites within a given dist of point, return number removed
//
int SpriteSystem::removeNear(ofVec3f point, float dist) {
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;
	int count = 0;

	while (s != sprites.end()) {
		ofVec3f v = s->trans - point;
		if (v.length() < dist) {
			tmp = sprites.erase(s);
			count++;
			s = tmp;
		}
		else s++;
	}
	return count;
}


//  Update the SpriteSystem by checking which sprites have exceeded their
//  lifespan (and deleting).  Also the sprite is moved to it's next
//  location based on velocity and direction.
//
void SpriteSystem::update(glm::vec3 p) {

	if (sprites.size() == 0) return;
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sprites.end()) {
		if (s->lifespan != -1 && s->age() > s->lifespan) {
			//			cout << "deleting sprite: " << s->name << endl;
			tmp = sprites.erase(s);
			s = tmp;
		}
		else s++;
	}

	//  Move sprite
	for (int i = 0; i < sprites.size(); i++) {
		//sprites[i].moveSprite(p);
		sprites[i].integrate();
	}
}

//  Render all the sprites
//
void SpriteSystem::draw() {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].draw();
	}
}
