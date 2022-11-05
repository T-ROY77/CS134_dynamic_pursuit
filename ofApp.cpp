#pragma once

#include "ofApp.h"
#include "Emitter.h"
#include "ofxGui.h"

// Troy Perez - CS134 SJSU


//set difficulty levels

//--------------------------------------------------------------
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
//setup parameters to start a new game
//
void ofApp::setupParameters() {
	p.pos = glm::vec3(ofGetWindowWidth() / 2.0 - 100, ofGetWindowHeight() / 2.0 - 100,
		0);
	p.rotation = 0;

	if (start) {
		invaders->start();
	}

	startTime = ofGetElapsedTimeMillis();
	gameTime = 0;
	smokeTime = -smokeCooldown;

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
		agentLife = 10000;
		agentSpeed = 3;
	}
}

//--------------------------------------------------------------
//setup emitters, gui, sounds, and images
//
void ofApp::setup() {
	ofSetVerticalSync(true);

	//setup Player
	p = Player(glm::vec3(-50, 50, 0), glm::vec3(50, 50, 0), glm::vec3(0,
		-50, 0));
	p.pos = glm::vec3(ofGetWindowWidth() / 2.0 - 100, ofGetWindowHeight() / 2.0 - 100,
		0);
	p.rotation = 0;

	//gui setup
	gui.setup();
	gui.add(speed.setup("Player speed", 5, 1, 20));
	gui.add(size.setup("Player size", 1, .1, 3));
	gui.add(penergy.setup("Player energy", 10, 1, 30));

	gui.add(agentSize.setup("Agent size", 45, 20, 100));
	gui.add(agentRate.setup("Agent spawn rate", 2, .5, 5));
	gui.add(agentLife.setup("Agent lifespan", 0, -1, 20000));
	gui.add(agentSpeed.setup("Agent speed", 1, .5, 3));

	gui.add(head.setup("Show heading vector", false));
	gui.add(sprite.setup("Show sprites", true));
	gui.add(invincible.setup("Invincible", false));

	guiHide = true;
	gameOver = false;
	gui.setPosition(20, 0);

	//setup background image
	background.load("floor.png");
	background.resize(ofGetWindowWidth(), ofGetWindowHeight());


	//sprite setup
	p.image.load("ninja_spritesheet_3.png");
	p.image.resize(350, 350);

	//setup agents
	invaders = new Emitter(new SpriteSystem);
	invaders->setPosition(glm::vec3(ofGetWindowWidth() / 2, 10, 0));
	invaders->setChildSize(10, 10);
	ofImage image;
	image.load("demon.png");
	image.resize(invaders->childHeight * 7.5, invaders->childHeight * 7.5);
	invaders->setChildImage(image);
	invaders->setRate(20);
	invaders->setLifespan(1000);

	//setup player gun
	gun = new ParticleEmitter(new ParticleSystem);
	gun->setRate(1);
	gun->setLifespan(100);
 	gun->setParticleRadius(5);
	gun->oneShot = true;
	gun->groupSize = 1;
	gun->setVelocity(p.heading());
	gun->acceleration = p.heading() * 2;
	ofImage image2;
	image2.load("Star.png");
	image2.resize(75, 75);
	gun->setChildImage(image2);

	//setup death explosions
	explosions = new ParticleEmitter(new ParticleSystem);
	explosions->groupSize = 25;
	explosions->lifespan = 1;
	explosions->setVelocity(glm::vec3(0, 0, 0));
	explosions->oneShot = true;
	explosions->setEmitterType(RadialEmitter);
	explosions->setParticleRadius(5);
	explosions->radius = 0;
	explosions->setRate(1);
	explosions->pos = glm::vec3(200, 200, 0);
	explosions->color = ofColor::darkRed;
	radialForce = new ImpulseRadialForce(.2);
	explosions->sys->addForce(radialForce);

	//setup smoke bomb particles
	smoke = new ParticleEmitter(new ParticleSystem);
	smoke->setEmitterType(RadialEmitter);
	smoke->setParticleRadius(1.5);
	smoke->groupSize = 2000;
	smoke->setLifespan(1.2);
	smoke->oneShot = true;
	smoke->setVelocity(glm::vec3(0, 0, 0));
	smoke->setRate(1);
	smoke->color = ofColor::ghostWhite;
	smokeRadialForce = new ImpulseRadialForce(.25);
	smoke->sys->addForce(smokeRadialForce);

	//setup smokebomb indicator
	smokeBombSprite.load("smoke_bomb.png");
	smokeBombSprite.resize(150, 150);

	//setup sounds
	explosionSound.load("chiptune_explosion.wav");
	explosionSound.setMultiPlay(true);
	explosionSound.setVolume(.25);

	smokeSound.load("short_smoke_bomb.wav");
	smokeSound.setVolume(.5);

	hitSound.load("hit_2.mp3");
	hitSound.setVolume(.25);
	hitSound.setMultiPlay(true);

	setupParameters();
}

//--------------------------------------------------------------
//update emitters
void ofApp::update() {
	//agent sprites
	invaders->setRate(agentRate);
	invaders->setLifespan(agentLife);
	invaders->setSpeed(agentSpeed);
	invaders->childHeight = agentSize;
	invaders->childWidth = agentSize;
	invaders->update(p.pos);

	
	//smoke particles
	smoke->update();

	//check for smokebomb
	smokeBomb();

	//explosion particles
	explosions->update();


	//gun particles
	gun->setVelocity(p.heading());
	gun->acceleration = p.heading();
	gun->update();

	p.image.resize(350 * size, 350 * size);
	p.scalar = glm::vec3(size);

	if (sprite) {
		invaders->haveChildImage = true;
	}
	else {
		invaders->haveChildImage = false;
	}

	
	checkGunCollisions();
	if(!invincible) checkCollisions();
	
	//play sounds
	if (agentDeath) {
		explosionSound.play();
	}
	agentDeath = false;

	if (playerHit) {
		hitSound.play();
	}
	playerHit = false;

	if (bsmokeBomb && !smokeSound.isPlaying()) {
		smokeSound.play();
	}

	
	//game over condition
	if (penergy <= 0) {
		gameOver = true;
		start = false;
	}

	//move forward
	if (keymap['w']) {
		float x = speed;

		//check screen boundaries
		//
		if (p.getPoint().x > ofGetWindowWidth()) {
			keymap['w'] = false;
		}
		if (p.getPoint().y > ofGetWindowHeight()) {
			keymap['w'] = false;
		}
		if (p.getPoint().x < 0) {
			keymap['w'] = false;
		}
		if (p.getPoint().y < 0) {
			keymap['w'] = false;
		}

		//move along heading
		if (keymap['w']) { p.pos += p.heading() * x; }

	}

	//move backwards
	if (keymap['s']) {
		float x = speed;

		//check screen boundaries
		//
		if (p.getBackPoint().x > ofGetWindowWidth()) {
			keymap['s'] = false;
		}
		if (p.getBackPoint().y > ofGetWindowHeight()) {
			keymap['s'] = false;
		}
		if (p.getBackPoint().x < 0) {
			keymap['s'] = false;
		}
		if (p.getBackPoint().y < 0) {
			keymap['s'] = false;
		}

		//move along heading
		if (keymap['s']) { p.pos -= p.heading() * x; }
	}

	//rotate right
	if (keymap['d']) { 
		p.rotation += speed * .5;
		//limit rotation to 0-360
		if (p.rotation >= 360) { p.rotation = p.rotation - 360; }
	}

	//rotate left
	if (keymap['a']) {
		p.rotation -= speed * .5;
		//limit rotation to 0-360
		if (p.rotation <= 0) { p.rotation = p.rotation + 360;}
	}

	//get total game time
	if (!gameOver) {
		gameTime = (ofGetElapsedTimeMillis() - startTime) / 1000;
	}

}


//--------------------------------------------------------------
//draw all objects and text
void ofApp::draw() {
	//beginning screen
	if (!start && !gameOver) {
		ofSetBackgroundColor(ofColor::black);
		ofSetColor(ofColor::white);
		background.draw(0, 0);

		smoke->draw();
		gun->draw();

		//draw player triangle
		if (!sprite) {
			ofSetColor(ofColor::darkViolet);
			p.draw();
		}

		//draw heading vector
		if (head) {
			float x = size;
			glm::vec3 o = p.heading();
			ofSetColor(ofColor::purple);
			ofSetLineWidth(2);
			ofDrawLine(p.pos, (p.pos + length * x * o));
		}

		//draw player sprite
		if (sprite) {
			if (!bsmokeHide) {
				ofSetColor(ofColor::white);
				//draw sprite based on current rotation
				//for 8 sprites
				//
				if (p.rotation >= 337.5 || p.rotation <= 22.5) {
					//up
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, 0, 0);
				}
				else if (p.rotation >= 292.5 && p.rotation < 337.5) {
					// left up
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, p.image.getWidth() / 3, 0);
				}
				else if (p.rotation >= 247.5 && p.rotation < 292.5) {
					// left 
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, 0, p.image.getHeight() / 3);
				}
				else if (p.rotation >= 202.5 && p.rotation < 247.5) {
					// left down
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, p.image.getWidth() * 2 / 3, 0);
				}
				else if (p.rotation >= 157.5 && p.rotation < 202.5) {
					// down
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, p.image.getWidth() / 3, p.image.getHeight() / 3);
				}
				else if (p.rotation >= 112.5 && p.rotation < 157.5) {
					// right down
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, p.image.getWidth() * 2 / 3, p.image.getHeight() / 3);
				}
				else if (p.rotation >= 67.5 && p.rotation < 112.5) {
					// right
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, 0, p.image.getHeight() * 2 / 3);
				}
				else if (p.rotation >= 22.5 && p.rotation < 67.5) {
					// right up
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, p.image.getWidth() / 3, p.image.getHeight() * 2 / 3);
				}
			}
		}

		ofSetColor(ofColor::red);

		//game title
		string titleText;
		titleText += "Fighting Demons";
		ofDrawBitmapString(titleText, ofPoint(ofGetWindowWidth() / 2 - titleText.length(), 30));

		//how to start
		string startText;
		startText += "Press z to start game ";
		ofDrawBitmapString(startText, ofPoint(20, 200));

		//difficulty setting
		string diffText;
		diffText += "Difficulty: \n";
		diffText += "\n";
		diffText += "Press 1 for easy \n";
		diffText += "\n";
		diffText += "Press 2 for normal \n";
		diffText += "\n";
		diffText += "Press 3 for hard \n";
		ofDrawBitmapString(diffText, ofPoint(20, 80));

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
		ofDrawBitmapString(modeText, ofPoint(110, 80));

		//if smoke bomb is not on cooldown, draw sprite
		if (gameTime - smokeTime > smokeCooldown) {
			ofSetColor(ofColor::white);
			smokeBombSprite.draw(glm::vec3(50, -25, 0));
		}
	}
	//game is started
	if (start) {
		ofSetBackgroundColor(ofColor::black);
		ofSetColor(ofColor::white);
		background.draw(0, 0);

		gameOver = false;

		invaders->draw();
		explosions->draw();
		smoke->draw();
		gun->draw();

		
		//current energy level
		string scoreText;
		scoreText += "Energy: " + std::to_string(penergy);
		ofSetColor(ofColor::red);
		ofDrawBitmapString(scoreText, ofPoint(10, 20));

		//frame rate
		string frameRateText;
		frameRateText += "Frame rate ";
		frameRateText += std::to_string(ofGetFrameRate());
		ofSetColor(ofColor::white);
		ofDrawBitmapString(frameRateText, ofPoint(ofGetWindowWidth() - 210, 20));

		//current game time
		string timeText;
		string gameTimeText = std::to_string(gameTime);
		gameTimeText.resize(4);
		timeText += "Elasped time: " + gameTimeText + " seconds";
		ofSetColor(ofColor::white);
		ofDrawBitmapString(timeText, ofPoint(ofGetWindowWidth() - 210, 40));

		//if smoke bomb is not on cooldown, draw sprite
		if (gameTime - smokeTime > smokeCooldown) {
			ofSetColor(ofColor::white);
			smokeBombSprite.draw(glm::vec3(50, -25, 0));
		}

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
		}

		//draw player sprite
		if (sprite) {
			if (!bsmokeHide) {
				ofSetColor(ofColor::white);
				//draw sprite based on current rotation
				//for 8 sprites
				//
				if (p.rotation >= 337.5 || p.rotation <= 22.5) {
					//up
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, 0, 0);
				}
				else if (p.rotation >= 292.5 && p.rotation < 337.5) {
					// left up
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, p.image.getWidth() / 3, 0);
				}
				else if (p.rotation >= 247.5 && p.rotation < 292.5) {
					// left 
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, 0, p.image.getHeight() / 3);
				}
				else if (p.rotation >= 202.5 && p.rotation < 247.5) {
					// left down
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, p.image.getWidth() * 2 / 3, 0);
				}
				else if (p.rotation >= 157.5 && p.rotation < 202.5) {
					// down
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, p.image.getWidth() / 3, p.image.getHeight() / 3);
				}
				else if (p.rotation >= 112.5 && p.rotation < 157.5) {
					// right down
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, p.image.getWidth() * 2 / 3, p.image.getHeight() / 3);
				}
				else if (p.rotation >= 67.5 && p.rotation < 112.5) {
					// right
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, 0, p.image.getHeight() * 2 / 3);
				}
				else if (p.rotation >= 22.5 && p.rotation < 67.5) {
					// right up
					p.image.drawSubsection(p.pos.x - p.image.getWidth() / 6, p.pos.y - p.image.getHeight() / 6, p.image.getWidth() / 3, p.image.getHeight() / 3, p.image.getWidth() / 3, p.image.getHeight() * 2 / 3);
				}
			}
		}
	}
	//game over screen
	if (gameOver) {

		invaders->stop();
		invaders->sys->sprites.clear();
		gun->stop();

		start = false;
		ofSetBackgroundColor(ofColor::black);


		string gameOverText;
		gameOverText += "Game Over ";
		ofSetColor(ofColor::red);
		ofDrawBitmapString(gameOverText, ofPoint(ofGetWindowWidth() / 2 - gameOverText.length(), ofGetWindowHeight() / 2));

		//total game time
		string timeText;
		string gameTimeText = std::to_string(gameTime);
		gameTimeText.resize(4);
		timeText += "Total time: " + gameTimeText + " seconds";
		ofDrawBitmapString(timeText, ofPoint(ofGetWindowWidth() / 2 - 50, (ofGetWindowHeight() / 2) + 50));

		//start new game
		string startText;
		startText += "Press z to start new game ";
		ofDrawBitmapString(startText, ofPoint(20, 200));

		//difficulty setting
		string diffText;
		diffText += "Difficulty: \n";
		diffText += "\n";
		diffText += "Press 1 for easy \n";
		diffText += "\n";
		diffText += "Press 2 for normal \n";
		diffText += "\n";
		diffText += "Press 3 for hard \n";
		ofDrawBitmapString(diffText, ofPoint(20, 80));

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
		ofDrawBitmapString(modeText, ofPoint(110, 80));
	}
}
//--------------------------------------------------------------
//keymap handler
void ofApp::keyPressed(int key) {
	keymap[key] = true;

	if (keymap['h']) {
		guiHide = !guiHide;
	}
	//start the game
	if (keymap['z']) {
		if (!start) {
			start = true;
			gameOver = false;
			setupParameters();
		}
	}
	//shoot
	if (keymap[' ']) {
		shootGun();
		gunTimeStart = gameTime;
	}
	//easy
	if (keymap['1']) {
		easy = true;
		hard = false;

	}
	//hard
	if (keymap['3']) {
		easy = false;
		hard = true;

	}
	//normal
	if (keymap['2']) {
		easy = false;
		hard = false;

	}
	//smoke bomb
	if (keymap['e']) {
		if (!gameOver) {
			if (gameTime - smokeTime > smokeCooldown) {
				smokeTime = gameTime;
				smokeHideStart = gameTime;
				bsmokeBomb = true;
				smoke->start(p.pos);
			}
		}
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
	background.resize(ofGetWindowWidth(), ofGetWindowHeight());
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}


//--------------------------------------------------------------
//check collisions between player and agents
//reduces player energy by number of collisions
void ofApp::checkCollisions() {
	if (!bsmokeHide) {
		// find the distance at which the two sprites (player and invaders) will collide
		// detect a collision when we are within that distance.
		//
		float collisionDist = 50 * size / 2 + invaders->childHeight;

		// Remove any invaders that are within "collisionDist" of the player.  
		// removeNear() returns the number of agents removed.
		//
		if (penergy > 0) {
			int collisions = invaders->sys->removeNear(p.pos, collisionDist);
			penergy = penergy - collisions;
			if (collisions > 0) {
				//bool to play hit sound
				playerHit = true;
			}
		}
	}
}

//--------------------------------------------------------------
//check collisions between gun particles and agents
void ofApp::checkGunCollisions() {

	// find the distance at which the two sprites (missles and agents) will collide
	// detect a collision when we are within that distance.
	//
	float collisionDist = gun->particleRadius * 2 + invaders->childHeight;

	// Loop through all the missiles, then remove any invaders that are within
	// "collisionDist" of the missiles.  the removeNear() function returns the
	// number of missiles removed.
	//
	int collisions = 0;
	for (int i = 0; i < gun->sys->particles.size(); i++) {
		collisions = 0;
		collisions += invaders->sys->removeNear(gun->sys->particles[i].position, collisionDist);
		if (collisions > 0) {
			//remove the gun particle
			gun->sys->particles[i].lifespan = 0;
			//start explosion particle emitter
			explosions->start(gun->sys->particles[i].position);
			//bool to play death sound
			agentDeath = true;
		}
		//increase player energy
		if (penergy < 15) {
			penergy = penergy + collisions;
		}
	}
}

//shoots player gun
//runs on a cooldown so gun cannot be held down
void ofApp::shootGun() {
	if (gameTime - gunTimeStart > gunCooldown) {
		gun->start(p.getPoint());
	}
}

//after a delay, randomize players location

void ofApp::smokeBomb() {
	if (bsmokeBomb) {
		bsmokeHide = true;
		if (gameTime - smokeHideStart > smokeDelay) {
			p.pos.y = ofRandom(50, ofGetWindowHeight() - 50);
			p.pos.x = ofRandom(50, ofGetWindowWidth() - 50);
			bsmokeHide = false;
			bsmokeBomb = false;
		}
	}
}
