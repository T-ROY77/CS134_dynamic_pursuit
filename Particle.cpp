#include "Particle.h"


Particle::Particle() {

	// initialize particle with some reasonable values first;
	//
	velocity.set(0, 0, 0);
	acceleration.set(0, 0, 0);
	position = glm::vec3(0, 0, 0);
	forces.set(0, 0, 0);
	lifespan = 5;
	birthtime = 0;
	radius = 10;
	damping = .99;
	mass = 1;
	color = ofColor::red;
	scale = glm::vec3(1);
}

void Particle::draw() {
	if (!done) {
		if (!haveImage) {
			ofSetColor(color);
			//ofDrawSphere(position, radius);
			ofDrawRectangle(position, radius, radius);
		}
		else {
			ofSetColor(ofColor::white);
			//position = glm::rotate(position, glm::radians(rot),glm::vec3(0, 0, 1));
			image.draw(position);

			/*
			ofPushMatrix();
			ofMultMatrix(getMatrix());
			image.draw(-image.getWidth() / 2.0, -image.getHeight() / 2.0);
			ofPopMatrix;
			*/
		}
	}

}

// write your own integrator here.. (hint: it's only 3 lines of code)
//
void Particle::integrate() {
	if (!done) {

		// interval for this step
		//
		float dt = 1.0 / ofGetFrameRate();

		// update position based on velocity
		//
		//position += (velocity * dt);
		position += (velocity);


		// update acceleration with accumulated paritcles forces
		// remember :  (f = ma) OR (a = 1/m * f)
		//
		ofVec3f accel = acceleration;    // start with any acceleration already on the particle
		accel += (forces * (1.0 / mass));
		//velocity += accel * dt;
		velocity += accel;

		// add a little damping for good measure
		//
		velocity *= damping;

		// clear forces on particle (they get re-added each step)
		//
		forces.set(0, 0, 0);
	}
}

//  return age in seconds
//
float Particle::age() {
	return (ofGetElapsedTimeMillis() - birthtime)/1000.0;
}

glm::mat4 Particle::getMatrix() {
	glm::mat4 translate = glm::translate(glm::mat4(1.0), position);
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0), glm::radians(rot),
		glm::vec3(0, 0, 1));
	glm::mat4 scalar = glm::scale(glm::mat4(1.0), scale);
	return (translate * rotate * scalar);
}
