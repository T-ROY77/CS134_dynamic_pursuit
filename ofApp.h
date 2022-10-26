#pragma once
#include "ofMain.h"
#include "Sprite.h"

#include "ofxGui.h"
#include "Particle.h"
#include "ParticleEmitter.h"


class Emitter;

class Shape {
public:
	Shape() {}
	virtual void draw() {}
	virtual bool inside(glm::vec3 p) {
		return true;
	}
	glm::mat4 getMatrix() {
		glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(pos));
		glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation),
			glm::vec3(0, 0, 1));
		glm::mat4 scale = glm::scale(glm::mat4(1.0), scalar);
		return (trans * rot * scale);
	}
	glm::vec3 pos;
	float rotation = 0.0;    // degrees 
	glm::vec3 scalar = glm::vec3(1, 1, 1);
	vector<glm::vec3> verts;
};
class TriangleShape : public Shape {
public:
	TriangleShape() {}
	TriangleShape(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		verts.push_back(p1);
		verts.push_back(p2);
		verts.push_back(p3);
	}
	void draw();
	bool inside(glm::vec3 p) {
		// convert point to "object space"
		//
		//glm::vec3 p0 = glm::inverse(getMatrix()) * glm::vec4(p, 1);
		return (inside(p, verts[0], verts[1], verts[2]));
	}
	bool inside(glm::vec3 p, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		glm::vec3 v1 = glm::normalize(p1 - p);
		glm::vec3 v2 = glm::normalize(p2 - p);
		glm::vec3 v3 = glm::normalize(p3 - p);
		float a1 = glm::orientedAngle(v1, v2, glm::vec3(0, 0, 1));
		float a2 = glm::orientedAngle(v2, v3, glm::vec3(0, 0, 1));
		float a3 = glm::orientedAngle(v3, v1, glm::vec3(0, 0, 1));
		if (a1 < 0 && a2 < 0 && a3 < 0) return true;
		else return false;
	}
};

class Player : public TriangleShape {
public:
	ofImage image;
	glm::vec3 point;
	glm::vec3 backpoint;


	Player() {}
	Player(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		verts.push_back(p1);
		verts.push_back(p2);
		verts.push_back(p3);
	}
	void draw();

	glm::vec3 getPoint() {
		point = pos + heading() * scalar * 50;
		return point;
	}

	glm::vec3 getBackPoint() {
		backpoint = pos - heading() * scalar * 50;
		return backpoint;
	}

	glm::vec3 heading() {
		glm::vec3 o = glm::vec3(0, -1, 0);
		o = glm::rotate(o, glm::radians(rotation), glm::vec3(0, 0, 1));
		glm::normalize(o);
		return o;
	}
};

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void checkCollisions();
	void checkTriangleCollisions();
	void setupParameters();
	void shootGun();

	Player p;
	// state variables
	//
	bool bInDrag = false;
	bool bInRotate = false;
	bool bCtrlKeyDown = false;
	bool moveLt = false;
	bool moveRt = false;
	bool moveUp = false;
	bool moveBack = false;
	bool start = false;
	bool gameOver = false;

	bool easy = false;
	bool hard = false;

	//int gameTime;
	float gameTime;

	float startTime;

	float length = 175;
	glm::vec3 mouseLast;

	int energy;

	//KeyMap variables
	//
	map<int, bool> keymap;
	ofxPanel gui;

	//gui variables
	bool guiHide;
	ofxFloatSlider speed;
	ofxFloatSlider size;
	ofxIntSlider penergy;
	ofxFloatSlider agentSpeed;
	ofxFloatSlider agentSize;
	ofxFloatSlider agentRate;
	ofxFloatSlider agentLife;
	ofxToggle sprite;
	ofxToggle head;

	Emitter* invaders;

	ParticleEmitter* gun;

	vector<Particle> particles;

	ofImage background;

};