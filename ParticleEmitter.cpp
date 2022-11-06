#pragma once

#include "ParticleEmitter.h"

// Troy Perez - CS134 SJSU

ParticleEmitter::ParticleEmitter() {
	sys = new ParticleSystem();
	createdSys = true;
	init();
}

ParticleEmitter::ParticleEmitter(ParticleSystem *s) {
	if (s == NULL)
	{
		cout << "fatal error: null particle system passed to ParticleEmitter()" << endl;
		ofExit();
	}
	sys = s;
	createdSys = false;
	init();
}

ParticleEmitter::~ParticleEmitter() {

	// deallocate particle system if emitter created one internally
	//
	if (createdSys) delete sys;
}

void ParticleEmitter::init() {
	rate = 1;
	velocity = ofVec3f(0, 20, 0);
	lifespan = 3;
	started = false;
	lastSpawned = 0;
	radius = 1;
	particleRadius = .1;
	visible = true;
	type = DirectionalEmitter;
	groupSize = 1;
	pos = glm::vec3(0);
}


//draw the particle emitter
//draw the particle system
//
void ParticleEmitter::draw() {
	if (visible) {
		switch (type) {
		case DirectionalEmitter:
			ofDrawSphere(position, radius/10);  // just draw a small sphere for point emitters 
			break;
		case SphereEmitter:
		case RadialEmitter:
			ofDrawSphere(position, radius/10);  // just draw a small sphere as a placeholder
			break;
		default:
			break;
		}
	}
	sys->draw();  
}

//start the particle emitter
//
void ParticleEmitter::start(glm::vec3 p) {
	pos = p;
	started = true;
	lastSpawned = ofGetElapsedTimeMillis();
}

//stop the particle emitter
//
void ParticleEmitter::stop() {
	started = false;
}

//update the particle emitter
//update the particle system
//
void ParticleEmitter::update() {

	float time = ofGetElapsedTimeMillis();

	if (oneShot && started) {
		for (int i = 0; i < groupSize; i++) {
			spawn(time);
		}
		lastSpawned = time;
		stop();
	}
	else if ((time - lastSpawned) > (1000.0 / rate) && started) {
		for (int i = 0; i < groupSize; i++) {
			spawn(time);
		}
		lastSpawned = time;
	}
	sys->update();
}

//spawn a new particle and add to system
//
void ParticleEmitter::spawn(float time) {
	Particle particle;

	// set initial velocity and position
	// based on emitter type
	//
	switch (type) {
	case RadialEmitter:
	{
		ofVec3f dir = ofVec3f(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1));
		float speed = velocity.length();
		particle.velocity = dir.getNormalized() * speed;
		particle.position = pos;
	}
	break;
	case SphereEmitter:
		break;
	case DirectionalEmitter:
		particle.velocity = velocity;
		particle.acceleration = acceleration;
		particle.position = pos;
		break;
	}

	// other particle attributes
	//
	particle.lifespan = lifespan;
	particle.birthtime = time;
	particle.radius = particleRadius;
	particle.color = color;

	//particle.color = ofColor(ofRandom(254), ofRandom(254), ofRandom(254));

	if (haveChildImage) {
		particle.image = childImage;
		particle.haveImage = true;
	}

	// add to system
	//
	sys->add(particle);
}