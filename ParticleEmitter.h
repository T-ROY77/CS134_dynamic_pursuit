#pragma once

#include "TransformObject.h"
#include "ParticleSystem.h"

// Troy Perez - CS134 SJSU


typedef enum { DirectionalEmitter, RadialEmitter, SphereEmitter } EmitterType;

//  General purpose Emitter class for emitting particles
//
class ParticleEmitter : public TransformObject {
public:
	ParticleEmitter();
	ParticleEmitter(ParticleSystem *s);
	~ParticleEmitter();
	void init();
	void draw();
	void start(glm::vec3 p);
	void stop();
	void setLifespan(const float life)   { lifespan = life; }
	void setVelocity(const ofVec3f &vel) { velocity = vel; }
	void setRate(const float r) { rate = r; }
	void setParticleRadius(float r) { particleRadius = r; }
	void setEmitterType(EmitterType t) { type = t; }
	void update();
	void spawn(float time);
	void setChildImage(ofImage i) { childImage = i; haveChildImage = true; }
	ParticleSystem *sys;
	float rate;         // per sec
	ofVec3f velocity;
	glm::vec3 acceleration;
	float lifespan;     // sec
	float lastSpawned;  // ms
	float particleRadius;
	float radius;
	bool started;
	bool visible;
	bool createdSys;
	bool oneShot;
	bool haveChildImage;
	EmitterType type;
	int groupSize;
	glm::vec3 pos;
	ofColor color;
	ofImage childImage;
};
