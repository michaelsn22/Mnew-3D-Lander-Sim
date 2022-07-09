
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Octree Test - startup scene
// 
//
//  Student Name:   < Michael Newman >
//  Date: <05.18.22>
#include "ofApp.h"
#include "Util.h"



//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup() {
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	camAdjust = false;
	bLanderLoaded = false;
	bTerrainSelected = true;

	//fuel
	//int fuel = 7500;
	//background
	background.load("images/starfield.jpg");
	//sound
	thrusterSoundPlayer.load("sound/thrusters.ogg");
	explodeNoise.load("sound/explosionNoise.wav");
	goodLandNoise.load("sound/Cheer.wav");
	hardLandNoise.load("sound/slowclapping.wav");
	backgroundMusic.load("sound/backgroundMusic.wav");
	backgroundMusic.play();
	//backgroundMusic.setLoop(true);


	cam.setDistance(30);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	theCam = &cam;

	cam1.setGlobalPosition(glm::vec3(50, 10, 0));
	cam1.lookAt(lander.getPosition());


	//setup the emitter stuff for the exhaust
	conicEmitter.setParticleRadius(0.6);
	conicEmitter.setEmitterType(DirectionalEmitter);
	conicEmitter.setLifespan(0.2);
	conicEmitter.setVelocity(ofVec3f(70, 0, 0));
	conicEmitter.setPosition(glm::vec3(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, 0));
	conicEmitter.setOneShot(true);
	// end of emitter stuff for exhaust
	//create forces for particles.
	turbForce = new TurbulenceForce(ofVec3f(0, 0, 0), ofVec3f(20, 20, 20));
	gravityForce = new GravityForce(ofVec3f(0, -5, 0));
	radialForce = new ImpulseRadialForce(1000);
	cyclicForce = new CyclicForce(250);

	crashEmitter.sys->addForce(turbForce);
	crashEmitter.sys->addForce(gravityForce);
	crashEmitter.sys->addForce(radialForce);
	crashEmitter.sys->addForce(cyclicForce);

	crashEmitter.setVelocity(ofVec3f(0, 0, 0));
	crashEmitter.setOneShot(true);
	crashEmitter.setEmitterType(RadialEmitter);
	crashEmitter.setGroupSize(250);
	//crashEmitter.setRandomLife(true);
	crashEmitter.setLifespan(0.8);
	//end of explosion stuff
	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();

	// setup rudimentary lighting 
	//
	initLightingAndMaterials();

	// Setup 3 - Light System
	// 
	keyLight.setup();
	keyLight.enable();
	keyLight.setAreaLight(1, 3);
	keyLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	keyLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	keyLight.setSpecularColor(ofFloatColor(1, 1, 1));
	keyLight.rotate(45, ofVec3f(0, 1, 0));
	keyLight.rotate(-45, ofVec3f(1, 0, 0));
	keyLight.setPosition(5, 4, 5);

	fillLight.setup();
	fillLight.enable();
	fillLight.setSpotlight();
	fillLight.setScale(.05);
	fillLight.setSpotlightCutOff(150);
	fillLight.setAttenuation(4, .001, .001);
	fillLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	fillLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	fillLight.setSpecularColor(ofFloatColor(1, 1, 1));
	fillLight.rotate(-10, ofVec3f(1, 0, 0));
	fillLight.rotate(-45, ofVec3f(0, 1, 0));
	fillLight.setPosition(-55, 35, 15);

	rimLight.setup();
	rimLight.enable();
	rimLight.setSpotlight();
	rimLight.setScale(.05);
	rimLight.setSpotlightCutOff(30);
	rimLight.setAttenuation(.2, .001, .001);
	rimLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	rimLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	rimLight.setSpecularColor(ofFloatColor(1, 1, 1));
	rimLight.rotate(180, ofVec3f(0, 1, 0));
	rimLight.setPosition(-75, 5, 5);

	//add in one other light
	directionalLight.setup();
	directionalLight.enable();
	directionalLight.setDirectional();
	directionalLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	directionalLight.setDiffuseColor(ofColor(255, 239, 167));
	directionalLight.setGlobalPosition(ofVec3f(10, 10, 10));
	directionalLight.setGlobalOrientation(ofQuaternion(0.3826834, 0, 0, 0.9238795));

	//spot light for ray.
	spotLightFromLander.setup();
	spotLightFromLander.enable();
	spotLightFromLander.setDirectional();
	spotLightFromLander.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	spotLightFromLander.setDiffuseColor(ofColor(255, 239, 167));

	//end of 3 light system setup.
	//bring the lander into the scene instead of having to drag it in.
	lander.loadModel("geo/lander.obj");
	lander.setScaleNormalization(false);
	lander.setPosition(1, 25, 0);
	bLanderLoaded = true;
	//load terrain
	mars.loadModel("geo/moon-houdini.obj");
	mars.setScaleNormalization(false);

	// create sliders for testing
	//
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));
	bHide = false;

	//  Create Octree for testing.
	//
	octree.create(mars.getMesh(0), 20);
	cout << "Number of Verts: " << mars.getMesh(0).getNumVertices() << endl;

	//the box we use to detect whether player is in the landing zone or not!
	testBox = Box(Vector3(-20, -20, -20), Vector3(20, 20, 20));

}

//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {
	float overallTime = ofGetElapsedTimeMillis();
	//cout << "overallTime : " << overallTime;
	//light stuff
	keyLight.setAmbientColor(ofFloatColor(0.5, 0.9, 0.1));
	fillLight.setAmbientColor(ofFloatColor(0.6, 0.1, 0.9));
	keyLight.setSpecularColor(ofFloatColor(0.5, 0.2, 0.5));
	fillLight.setSpecularColor(ofFloatColor(0.6, 0.2, 0.5));
	rimLight.setSpecularColor(ofFloatColor(0.5, 0.2, 0.5));
	directionalLight.setAmbientColor(ofFloatColor(1, 1, 1));
	directionalLight.setSpecularColor(ofFloatColor(1, 1, 1));
	directionalLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	spotLightFromLander.setGlobalPosition(lander.getPosition());
	spotLightFromLander.setAmbientColor(ofFloatColor(1, 1, 1));
	spotLightFromLander.setSpecularColor(ofFloatColor(1, 1, 1));
	spotLightFromLander.setDiffuseColor(ofFloatColor(1, 1, 1));
	//end of light stuff

	//update the emitters!
	conicEmitter.update();
	crashEmitter.update();

	//thrust value for the lander to use.
	lander.thrust = 75;


	cam1.lookAt(lander.getPosition());
	cam2.setGlobalPosition(lander.getPosition());
	cam2.lookAt(glm::vec3(0, 0, 0));

	// integration done here
   //
	lander.isSimStarted = false;
	if (bStartSim)
	{
		//print statement for debugging the altitude.
		//cout << "Lander Altitude: " << rayAltitudeToOctree() << "\n";
		//CODE FOR THE COLLISION IMPLEMENTATION
		glm::vec3 landerPos = lander.getPosition();

		lander.setPosition(landerPos.x, landerPos.y, landerPos.z);

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();

		octree.intersect(bounds, octree.root, colBoxList); //check if touching terrain
		if (octree.intersect(bounds, octree.root, colBoxList) == true) {
			//apply a restitution force!
			lander.force.y += lander.thrust / 2;
			//cout << "we are touching the moon!!!"; //use this to check if touching terrain.
			if (bounds.overlap(testBox)) { //if we are touching octree, check if we are touching box as well.
				//debug statement to check if player lands in the good area.
				//cout << "Inside landing area." << endl;
				bStartSim = false;
				bCleanLanding = true;
				bCrashCheck = false;
				//lander has landed inside of the landing area. Now decide whether it was a hard landing or a soft landing.
				//hard landing = 60 points. Soft landing = 100 points.
				if (lander.velocity.x > 10 || lander.velocity.x < -10 || lander.velocity.z > 10 || lander.velocity.z < -10 || lander.velocity.y < -3) {
					hardLanding = true;
					goodLandNoise.stop();
					hardLandNoise.play();
					//cout << "hard landing"; //debug statement to check if the landing is ACTUALLY a hard landing.
				}
				else {
					goodLandNoise.play();
				}
				if (bStartSim = true) {
					lander.setPosition(1, 25, 0);
					bStartSim = false;
				}

			}
			else {
				//debug for out of good area.
				//cout << "Out of Landing Area" << endl;
				crashEmitter.stop();
				crashEmitter.sys->reset();
				crashEmitter.setPosition(lander.getPosition() + ofVec3f(0, 5, 0));
				crashEmitter.setVelocity(lander.heading());
				crashEmitter.start();
				bStartSim = false;
				bCleanLanding = false;
				hardLanding = false;
				bCrashCheck = true;
				explodeNoise.play();
				if (bStartSim = true) {
					lander.setPosition(1, 25, 0);
					bStartSim = false;
					//ofDrawBitmapString("Game over! Crash landing! Try again by pressing x!", ofGetWindowWidth() - 200, 150);
				}
			}
		}
		//CODE FOR THE COLLISION IMPLEMENTATION END

		lander.isSimStarted = true;
		ofVec3f gravity = ofVec3f(0, -9.8, 0);
		ofVec3f turbulence = ofVec3f(-0.5, -4, 0);
		lander.force = lander.force + gravity + turbulence;
		if (lander.isForward) {

			lander.force += lander.headingNew(true) * lander.thrust;
		}
		if (lander.isBackward) {

			lander.force -= lander.headingNew(true) * lander.thrust;
		}
		if (lander.rightTravel) {
			lander.force += lander.headingNew(false) * lander.thrust;

		}
		if (lander.leftTravel) {
			lander.force -= lander.headingNew(false) * lander.thrust;

		}
		if (lander.isThrusting) {
			//have the variable for fuel get decremented every frame. time it so after 2 minutes the fuel runs out. primitive way of doing it.
			//int fuel = 7200;
			//int fuel = fuel-1;
			//cout << "fuel is at: " << fuel;
			lander.force.y += lander.thrust;
			//sounds stuff.
			thrusterSoundPlayer.play();
			//emitter stuff.
			conicEmitter.sys->reset();
			conicEmitter.stop();
			conicEmitter.setVelocity(lander.heading());
			conicEmitter.setPosition(lander.getPosition());
			conicEmitter.start();
			//if (fuel <= 0) {
			//	conicEmitter.stop();
			//}
		}
		if (lander.isTurningRight) {
			lander.angularForce -= lander.thrust;
		}
		if (lander.isTurningLeft) {
			lander.angularForce += lander.thrust;
		}
		lander.integrate();

	}
	// zero out forces
	//
	lander.force = glm::vec3(0, 0, 0);
}
//--------------------------------------------------------------
void ofApp::draw() {
	directionalLight.draw();
	spotLightFromLander.draw();
	ofBackground(ofColor::black);
	glDepthMask(false);
	//background.resize(ofGetWidth(), ofGetHeight());
	background.draw(0, 0, ofGetWidth(), ofGetHeight());
	if (!bHide) gui.draw();
	glDepthMask(true);

	//cam.begin();
	theCam->begin();

	ofPushMatrix();
	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		mars.drawWireframe();
		if (bLanderLoaded) {
			lander.drawWireframe();
			if (!bTerrainSelected) drawAxis(lander.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}
	else {
		ofEnableLighting();              // shaded mode
		mars.drawFaces();
		ofMesh mesh;
		if (bLanderLoaded) {
			lander.drawFaces();
			if (!bTerrainSelected) drawAxis(lander.getPosition());
			if (bDisplayBBoxes) {
				ofSetColor(ofColor::slateGray);
				ofNoFill();
				ofSetColor(ofColor::white);
				for (int i = 0; i < lander.getNumMeshes(); i++) {
					ofPushMatrix();
					ofMultMatrix(lander.getModelMatrix());
					ofRotate(-90, 1, 0, 0);
					Octree::drawBox(bboxList[i]);
					ofPopMatrix();
				}
			}

			if (bLanderSelected) {

				ofVec3f min = lander.getSceneMin() + lander.getPosition();
				ofVec3f max = lander.getSceneMax() + lander.getPosition();

				Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
				ofSetColor(ofColor::white);
				Octree::drawBox(bounds);

				// draw colliding boxes
				//
				ofSetColor(ofColor::yellow);
				for (int i = 0; i < colBoxList.size(); i++) {
					Octree::drawBox(colBoxList[i]);
				}
			}
		}
	}
	if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));



	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
		mars.drawVertices();
	}



	//draw the particles for exhaust
	conicEmitter.draw();
	crashEmitter.draw();

	// recursively draw octree
	//
	ofDisableLighting();
	int level = 0;

	if (bDisplayLeafNodes) {
		octree.drawLeafNodes(octree.root);
		cout << "num leaf: " << octree.numLeaf << endl;
	}
	else if (bDisplayOctree) {
		ofNoFill();
		ofSetColor(ofColor::white);
		octree.draw(numLevels, 0);
	}

	ofPopMatrix();
	theCam->end();
	ofSetColor(ofColor::white);

	ofDrawBitmapString("FPS: " + ofToString((int)ofGetFrameRate()), ofGetWindowWidth() - 400, 150);
	ofDrawBitmapString("Time elapsed: " + ofToString((float)ofGetElapsedTimeMillis()), ofGetWindowWidth() - 200, 150);
	if (bStartSim == true) {
		ofDrawBitmapString("Player Altitude: " + ofToString((float)altitudeWithRayCast()), ofGetWindowWidth() - 300, 175);
	}
	
	if (bCrashCheck == true) {
		if (bStartSim == false) {
			ofDrawBitmapString("Game over! Crash landing! Try again by pressing x!", ofGetWindowWidth() - 500, 170);
			ofDrawBitmapString("Your Game Ending Score Was: 0", ofGetWindowWidth() - 700, 250);
			
		}
	}
	if (bCleanLanding == true) {
		if (bStartSim == false) {
			if (hardLanding == true) {
				ofDrawBitmapString("Hard landing!", ofGetWindowWidth() - 500, 170);
				ofDrawBitmapString("Your Game Ending Score Was: 65", ofGetWindowWidth() - 600, 250);
			}
			else {
				ofDrawBitmapString("Great landing!", ofGetWindowWidth() - 500, 170);
				ofDrawBitmapString("Your Game Ending Score Was: 100", ofGetWindowWidth() - 600, 250);
			}
			
		}
	}

}


// 
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));


	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	switch (key) {
	case 'B':
	case 'b':
		bDisplayBBoxes = !bDisplayBBoxes;
		break;
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		break;
	case 'L':
	case 'l': //the 2nd cam angle for easycam!!!
		theCam = &cam;
		cam.setGlobalPosition(lander.getPosition() + glm::vec3(10, 10, 0));
		cam.lookAt(lander.getPosition());
		break;
	case 'O':
	case 'o':
		bDisplayOctree = !bDisplayOctree;
		break;
	case 'r':
		cam.reset();
		break;
	case 's':
		savePicture();
		break;
	case 't':
		setCameraTarget();
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'w':
		toggleWireframeMode();
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_LEFT:   // left
		lander.leftTravel = true;
		break;
	case OF_KEY_RIGHT:  // right
		lander.rightTravel = true;
		break;
	case OF_KEY_UP:     // move forward
		lander.isForward = true;
		break;
	case OF_KEY_DOWN:   // move backward
		lander.isBackward = true;
		break;
	case 'u':   // rotate left
		lander.isTurningLeft = true;
		break;
	case 'i':   // rotate right
		lander.isTurningRight = true;
		break;
	case 'x':
		bStartSim = !bStartSim;
		break;
	case ' ':
		lander.isThrusting = true;
		break;
	case OF_KEY_F1:
		theCam = &cam;
		break;
	case OF_KEY_F2:
		theCam = &cam1;
		break;
	case OF_KEY_F3:
		theCam = &cam2;
		break;
	default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {

	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_LEFT:   // turn left
		lander.leftTravel = false;
		lander.angularForce = 0;
		break;
	case OF_KEY_RIGHT:  // turn right
		lander.rightTravel = false;
		lander.angularForce = 0;
		break;
	case OF_KEY_UP:     // go forward
		lander.isForward = false;
		break;
	case OF_KEY_DOWN:   // go backward
		lander.isBackward = false;
		break;
	case 'u':   // rotate left
		lander.isTurningLeft = false;
		lander.angularForce = 0;
		break;
	case 'i':   // rotate right
		lander.isTurningRight = false;
		lander.angularForce = 0;
		break;
	case ' ':   // go up w thrust
		lander.isThrusting = false;
		thrusterSoundPlayer.stop();
		break;
		//case 'l':   // cam stuff
		//	camAdjust = false;
		//	break;
	default:
		break;

	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {


}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	// if moving camera, don't allow mouse interaction
//
	if (cam.getMouseInputEnabled()) return;

	// if rover is loaded, test for selection
	//
	if (bLanderLoaded) {
		glm::vec3 origin = cam.getPosition();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			bLanderSelected = true;
			mouseDownPos = getMousePointOnPlane(lander.getPosition(), cam.getZAxis());
			mouseLastPos = mouseDownPos;
			bInDrag = true;
		}
		else {
			bLanderSelected = false;
		}
	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

bool ofApp::raySelectWithOctree(ofVec3f &pointRet) {
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	int startTime = ofGetElapsedTimeMicros();
	pointSelected = octree.intersect(ray, octree.root, selectedNode);
	int endTime = ofGetElapsedTimeMicros();

	if (pointSelected) {
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
	}
	return pointSelected;
}




//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	if (bInDrag) {

		glm::vec3 landerPos = lander.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;

		landerPos += delta;
		lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();

		int startTime = ofGetElapsedTimeMicros(); // WHERE WE NEED TO DO THE INTERSECT STUFF!@!@!!@
		octree.intersect(bounds, octree.root, colBoxList);
		int endTime = ofGetElapsedTimeMicros();
		cout << "Time for lander to intersect Octree: " << (endTime - startTime) << " microseconds" << "\n";
		//lander.force += (0, 5, 0); //do something to move it out of collision.


	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
}



// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

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
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{ 5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
}

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}
/*
the ray starts from the positon of the lander
and the direction of it is a normalized vector facing straight down (0, -1, 0)
if the ray intersects the octree, we calculate the distance from the lander to the center of the octree point it intersected
*/
float ofApp::altitudeWithRayCast() {
	Ray ray = Ray(Vector3(lander.getPosition().x, lander.getPosition().y, lander.getPosition().z), Vector3(0, -1, 0));
	if (octree.intersect(ray, octree.root, selectedNode)) {
		Vector3 point = selectedNode.box.center();
		return lander.getPosition().y - point.y();
	}
}
//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent2(ofDragInfo dragInfo) {

	ofVec3f point;
	mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point);
	if (lander.loadModel(dragInfo.files[0])) {
		lander.setScaleNormalization(false);
		//		lander.setScale(.1, .1, .1);
			//	lander.setPosition(point.x, point.y, point.z);
		lander.setPosition(1, 1, 0);

		bLanderLoaded = true;
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		cout << "Mesh Count: " << lander.getMeshCount() << endl;
	}
	else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {
	if (lander.loadModel(dragInfo.files[0])) {
		bLanderLoaded = true;
		lander.setScaleNormalization(false);
		lander.setPosition(0, 0, 0);
		cout << "number of meshes: " << lander.getNumMeshes() << endl;
		bboxList.clear();
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}
				// We want to drag and drop a 3D object in space so that the model appears 
				// under the mouse pointer where you drop it !
				//
				// Our strategy: intersect a plane parallel to the camera plane where the mouse drops the model
				// once we find the point of intersection, we can position the lander/lander
				// at that location.
				//

				// Setup our rays
				//
		glm::vec3 origin = cam.getPosition();
		glm::vec3 camAxis = cam.getZAxis();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
		float distance;

		bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
		if (hit) {
			// find the point of intersection on the plane using the distance 
			// We use the parameteric line or vector representation of a line to compute
			//
			// p' = p + s * dir;
			//
			glm::vec3 intersectPoint = origin + distance * mouseDir;

			// Now position the lander's origin at that intersection point
			//
			glm::vec3 min = lander.getSceneMin();
			glm::vec3 max = lander.getSceneMax();
			float offset = (max.y - min.y) / 2.0;
			lander.setPosition(intersectPoint.x, intersectPoint.y - offset, intersectPoint.z);

			// set up bounding box for lander while we are at it
			//
			landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		}
	}


}

//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	//
	glm::vec3 origin = cam.getPosition();
	glm::vec3 camAxis = cam.getZAxis();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}

// Integrator for simple trajectory physics
//
void Lander::integrate() {

	// init current framerate (or you can use ofGetLastFrameTime())
	//
	float framerate = ofGetFrameRate();
	float dt = 1.0 / framerate;

	// linear motion
	//
	pos += (velocity * dt);
	glm::vec3 accel = acceleration;
	accel += (force * 1.0 / mass);
	velocity += accel * dt;
	velocity *= damping;

	// angular motion
	//
	rotation += (angularVelocity * dt);
	setRotation(0, getRotationAngle(0) + angularVelocity * dt, 0, 1, 0);
	float a = angularAcceleration;;
	a += (angularForce * 1.0 / mass);
	angularVelocity += a * dt;
	angularVelocity *= damping;

}