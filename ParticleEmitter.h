#pragma once
//  Kevin M. Smith - CS 134 SJSU

#include "TransformObject.h"
#include "ParticleSystem.h"

typedef enum { DirectionalEmitter, RadialEmitter, SphereEmitter } EmitterType;

//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
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
	bool started;
	float lastSpawned;  // ms
	float particleRadius;
	float radius;
	bool visible;
	bool createdSys;
	bool oneShot;
	EmitterType type;
	int groupSize;
	glm::vec3 pos;
	ofColor color;
	bool haveChildImage = false;
	ofImage childImage;
};
