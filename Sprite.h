#pragma once

// Troy Perez - CS134 SJSU


#include "ofMain.h"

// This is a base object that all drawable object inherit from
// It is possible this will be replaced by ofNode when we move to 3D
//
class BaseObject {
public:
	BaseObject();
	glm::vec3 trans, scale;
	float	rot;
	bool	bSelected;
	void setPosition(glm::vec3 p);
};

//  General Sprite class  (similar to a Particle)
//
class Sprite : public BaseObject {
public:
	Sprite();
	void draw();
	float age();
	void setImage(ofImage);
	float speed;    //   in pixels/sec
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 forces;
	float angularForce;
	float angularVelocity;
	float angularAcceleration;
	float mass;
	float damping;
	ofImage image;
	float birthtime; // elapsed time in ms
	float lifespan;  //  time in ms
	string name;
	bool haveImage;
	float width, height, imageWidth, imageHeight;
	glm::vec3 heading();
	void moveSprite(glm::vec3 p);
	glm::mat4 getMatrix();
	void integrate(glm::vec3 p);


};

//  Manages all Sprites in a system.  You can create multiple systems
//
class SpriteSystem {
public:
	void add(Sprite);
	void remove(int);
	void update(glm::vec3 p);
	int removeNear(ofVec3f point, float dist);
	void draw();
	vector<Sprite> sprites;
	vector<glm::vec3> spritePos;

};