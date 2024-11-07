#pragma once

#include "ofMain.h"

// Troy Perez - CS134 SJSU


class ParticleForceField;

class Particle {
public:
	Particle();

	glm::vec3 position;
	float rot;
	glm::vec3 scale;
	ofVec3f velocity;
	ofVec3f acceleration;
	ofVec3f forces;
	float	damping;
	float   mass;
	float   lifespan;
	float   radius;
	float   birthtime;
	void    integrate();
	void    draw();
	float   age();        // sec
	glm::mat4 getMatrix();
	ofColor color;
	ofImage image;
	bool haveImage = false;
	bool done = false;
};


