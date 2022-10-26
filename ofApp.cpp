#include "ofApp.h"
#include "Emitter.h"

#include "ofxGui.h"

//
// agents slow down when near player
// how to rotate
// 
// choppy movement
//
//
void TriangleShape::draw() {
	ofFill();
	ofPushMatrix();
	ofMultMatrix(getMatrix());
	ofDrawTriangle(verts[0], verts[1], verts[2]);
	ofPopMatrix();
}
//--------------------------------------------------------------

void Player::draw() {
	ofFill();
	ofPushMatrix();
	ofMultMatrix(getMatrix());
	ofDrawTriangle(verts[0], verts[1], verts[2]);
	ofPopMatrix();
}
//--------------------------------------------------------------

void ofApp::shootGun() {
	// Create initialize a new particle with values from sliders
	//


	
	//gun->setVelocity(p.heading());
	//gun->start();

	
	Particle particle;

	particle.position = p.getPoint();
	particle.velocity = p.heading();
	particle.acceleration = p.heading() * 2;
	particle.radius = 3;

	particles.push_back(particle);
}

void ofApp::setupParameters() {

	p.pos = glm::vec3(ofGetWindowWidth() / 2.0 - 100, ofGetWindowHeight() / 2.0 - 100,
		0);
	p.rotation = -45;

	if (start) {
		invaders->start();
	}




	startTime = ofGetElapsedTimeMillis();

	gameTime = 0;

	if (easy) {
		penergy = 15;
		agentRate = 5;
		agentLife = 2000;
		agentSpeed = 1;
	}
	else if (hard) {
		penergy = 5;
		agentRate = 10;
		agentLife = 7000;
		agentSpeed = 3;
	}
	else
	{
		penergy = 10;
		agentRate = 7;
		agentLife = 5000;
		agentSpeed = 2;
	}
}

//--------------------------------------------------------------

void ofApp::setup() {
	ofSetVerticalSync(true);

	//setup Player
	p = Player(glm::vec3(-50, 50, 0), glm::vec3(50, 50, 0), glm::vec3(0,
		-50, 0));
	p.pos = glm::vec3(ofGetWindowWidth() / 2.0 - 100, ofGetWindowHeight() / 2.0 - 100,
		0);
	p.rotation = -45;

	//gui setup
	gui.setup();
	gui.add(speed.setup("Player speed", 5, 1, 20));
	gui.add(size.setup("Player size", 1, .1, 3));
	gui.add(penergy.setup("Player energy", 10, 1, 30));

	gui.add(agentSize.setup("Agent size", 15, 5, 50));
	gui.add(agentRate.setup("Agent spawn rate", 2, .5, 5));
	gui.add(agentLife.setup("Agent lifespan", 3000, 0, 10000));
	gui.add(agentSpeed.setup("Agent speed", 1, .5, 3));



	gui.add(head.setup("Show heading vector", false));
	gui.add(sprite.setup("Show sprites", false));
	guiHide = true;
	gameOver = false;


	background.load("space_photo.png");
	background.resize(ofGetWindowWidth(), ofGetWindowHeight());


	//sprite setup
	p.image.load("ship.png");
	p.image.resize(300, 300);

	invaders = new Emitter(new SpriteSystem);

	invaders->setPosition(glm::vec3(ofGetWindowWidth() / 2, 10, 0));
	invaders->setChildSize(10, 10);
	ofImage image;
	image.load("agent.png");
	image.resize(invaders->childHeight * 10, invaders->childHeight * 10);
	invaders->setChildImage(image);
	invaders->setRate(20);
	invaders->setLifespan(1000);

	gun = new ParticleEmitter(new ParticleSystem);
	gun->setRate(1);
	gun->setLifespan(1000);
	gun->setParticleRadius(2);

	particles.clear();

	setupParameters();
}

//--------------------------------------------------------------
void ofApp::update() {
	invaders->setRate(agentRate);
	invaders->setLifespan(agentLife);
	invaders->setSpeed(agentSpeed);

	invaders->childHeight = agentSize;
	invaders->childWidth = agentSize;

	//gun->update();
	for (int i = 0; i < particles.size(); i++) {
		particles[i].integrate();
	}

	if (sprite) {
		invaders->haveChildImage = true;
	}
	else {
		invaders->haveChildImage = false;
	}


	invaders->update(p.pos);

	checkCollisions();



	if (penergy <= 0) {
		gameOver = true;
		start = false;
	}

	if (keymap[OF_KEY_UP]) {
		float x = speed;

		//check screen boundaries
		//
		if (p.getPoint().x > ofGetWindowWidth()) {
			keymap[OF_KEY_UP] = false;
		}
		if (p.getPoint().y > ofGetWindowHeight()) {
			keymap[OF_KEY_UP] = false;
		}
		if (p.getPoint().x < 0) {
			keymap[OF_KEY_UP] = false;
		}
		if (p.getPoint().y < 0) {
			keymap[OF_KEY_UP] = false;
		}

		if (keymap[OF_KEY_UP]) { p.pos += p.heading() * x; }

	}

	if (keymap[OF_KEY_DOWN]) {
		float x = speed;

		//check screen boundaries
		//
		if (p.getBackPoint().x > ofGetWindowWidth()) {
			keymap[OF_KEY_DOWN] = false;
		}
		if (p.getBackPoint().y > ofGetWindowHeight()) {
			keymap[OF_KEY_DOWN] = false;
		}
		if (p.getBackPoint().x < 0) {
			keymap[OF_KEY_DOWN] = false;
		}
		if (p.getBackPoint().y < 0) {
			keymap[OF_KEY_DOWN] = false;
		}

		if (keymap[OF_KEY_DOWN]) { p.pos -= p.heading() * x; }
	}

	if (keymap[OF_KEY_RIGHT]) { p.rotation += speed / 2;}
	if (keymap[OF_KEY_LEFT]) {p.rotation -= speed / 2;}

	p.scalar = glm::vec3(size);

	if (!gameOver) {
		gameTime = (ofGetElapsedTimeMillis() - startTime) / 1000;
	}

}

//--------------------------------------------------------------
void ofApp::checkCollisions() {

	// find the distance at which the two sprites (missles and invaders) will collide
	// detect a collision when we are within that distance.
	//
	float collisionDist = 50 * size / 2 + invaders->childHeight / 2;

	// Loop through all the missiles, then remove any invaders that are within
	// "collisionDist" of the missiles.  the removeNear() function returns the
	// number of missiles removed.
	//
	if (penergy > 0) {
		int collisions = invaders->sys->removeNear(p.pos, collisionDist);
		penergy = penergy - collisions;
	}
}



//--------------------------------------------------------------
void ofApp::draw() {
	if (!start) {
		ofSetBackgroundColor(ofColor::darkBlue);

		string titleText;
		titleText += "Pursuit Game";
		ofSetColor(ofColor::white);
		ofDrawBitmapString(titleText, ofPoint(ofGetWindowWidth() / 2 - titleText.length(), 20));

		string startText;
		startText += "Press space to start game ";
		ofSetColor(ofColor::white);
		ofDrawBitmapString(startText, ofPoint(10, 20));

		string diffText;
		diffText += "Difficulty: \n";
		diffText += "\n";
		diffText += "Press q for easy \n";
		diffText += "\n";
		diffText += "Press w for medium \n";
		diffText += "\n";
		diffText += "Press e for hard \n";

		ofSetColor(ofColor::white);
		ofDrawBitmapString(diffText, ofPoint(10, 50));

		string modeText;
		if (easy) {
			modeText += "Easy";
		}
		else if (hard) {
			modeText += "Hard";
		}
		else {
			modeText += "Normal";
		}
		ofSetColor(ofColor::white);
		ofDrawBitmapString(modeText, ofPoint(100, 50));
	}
	if (gameOver) {

		invaders->stop();
		invaders->sys->sprites.clear();

		start = false;
		ofSetBackgroundColor(ofColor::black);

		
		string gameOverText;
		gameOverText += "Game Over ";
		ofSetColor(ofColor::red);
		ofDrawBitmapString(gameOverText, ofPoint(ofGetWindowWidth() / 2 - gameOverText.length(), ofGetWindowHeight() / 2));


		string timeText;
		string gameTimeText = std::to_string(gameTime);
		gameTimeText.resize(4);
		timeText += "Total time: " + gameTimeText + " seconds";
		ofSetColor(ofColor::red);
		ofDrawBitmapString(timeText, ofPoint(ofGetWindowWidth() / 2 - 50, (ofGetWindowHeight() / 2) + 50));
	}
	if (start) {
		ofSetColor(ofColor::white);
		background.draw(0, 0);

		//ofSetBackgroundColor(ofColor::black);

		gameOver = false;

		invaders->draw();


		ofSetColor(ofColor::red);
		//gun->draw();

		for (int i = 0; i < particles.size(); i++) {
			particles[i].draw();
		}

		string scoreText;
		scoreText += "Energy: " + std::to_string(penergy);
		ofSetColor(ofColor::red);
		ofDrawBitmapString(scoreText, ofPoint(10, 20));

		string frameRateText;
		frameRateText += "Frame rate ";
		frameRateText += std::to_string(ofGetFrameRate());
		ofSetColor(ofColor::white);
		ofDrawBitmapString(frameRateText, ofPoint(ofGetWindowWidth() - 210, 20));

		string timeText;
		string gameTimeText = std::to_string(gameTime);
		gameTimeText.resize(4);
		timeText += "Elasped time: " + gameTimeText + " seconds";
		ofSetColor(ofColor::white);
		ofDrawBitmapString(timeText, ofPoint(ofGetWindowWidth() - 210, 40));


		//draw player triangle
		if (!sprite) {
			ofSetColor(ofColor::darkViolet);
			p.draw();
		}

		//draw gui
		if (!guiHide) {
			gui.draw();
		}

		//draw heading vector
		if (head) {
			float x = size;
			glm::vec3 o = p.heading();
			ofSetColor(ofColor::purple);
			ofSetLineWidth(2);
			ofDrawLine(p.pos, (p.pos + length * x * o));

			for (int i = 0; i < invaders->sys->sprites.size(); i++) {
				glm::vec3 p1 = invaders->sys->sprites[i].heading();
				ofSetColor(ofColor::purple);
				ofSetLineWidth(2);
				ofDrawLine(invaders->sys->sprites[i].trans, (invaders->sys->sprites[i].trans + length * p1));
			}
		}

		//draw sprite
		if (sprite) {
			ofSetColor(ofColor::white);
			ofPushMatrix();
			ofMultMatrix(p.getMatrix());
			p.image.draw(0, 0);
			p.image.setAnchorPoint(p.image.getWidth() / 2, p.image.getHeight() / 2);
			ofPopMatrix();
		}
	}
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	keymap[key] = true;
	
	if (keymap[OF_KEY_RIGHT]) {
		moveRt = true;
	}
	if (keymap[OF_KEY_UP]) {
		moveUp = true;
	}
	if (keymap[OF_KEY_DOWN]) {
		moveBack = true;
	}
	if (keymap['h']) {
		guiHide = !guiHide;
	}
	if (keymap[' ']) {
		if (!start) {
			start = true;
			gameOver = false;
			setupParameters();
		}
		if (start) {
			shootGun();
		}
	}
	if (keymap['q']) {
		easy = true;
		hard = false;

	}
	if (keymap['e']) {
		easy = false;
		hard = true;

	}
	if (keymap['w']) {
		easy = false;
		hard = false;

	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	keymap[key] = false;
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
