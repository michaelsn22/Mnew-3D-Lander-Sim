#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"
#include "Sprite.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include <glm/gtx/intersect.hpp>
#include "Particle.h"

//  Student Name:   < Michael Newman >
//  Date: <05.18.22>
class Lander : public ofxAssimpModelLoader {
public:

	//  Integrator Function;
	//
	void integrate();

	// Physics data goes here  (for integrate() );
	//
	glm::vec3 velocity = glm::vec3(0, 0, 0);
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	//im adding gravity and turbulence in ofApp. Not here.
	//ofVec3f gravity = ofVec3f(0, -0.5, 0);
	glm::vec3 force = glm::vec3(0, 0, 0);

	float altitude;

	float mass = 1.0;
	/*float damping = .99;*/
	float damping = .973;
	float angularForce = 0;
	float angularVelocity = 0.0;
	float angularAcceleration = 0.0;
	bool bThrust = false;
	float prevDist = 0;
	float thrust;

	// Player Movement
	//
	bool isForward;
	bool isBackward;
	bool isTurningLeft;
	bool isTurningRight;
	bool isThrusting;
	bool rightTravel;
	bool leftTravel;
	


	float rotation = 0.0;

	// Get heading vector by using a matrix rotation 
	//uses rotation around Z and applies it to a negative y vector.....
	//boiler plate code that came from Kevin Smith to find a heading vec for midterm and proj 2.
	glm::vec3 Lander::heading() {
		glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1)); //rotation around z currently
		glm::vec3 heading = rot * glm::vec4(0, -1, 0, 1);
		return heading;
	}
	//headingNew rot's around y and applies to a neg z vector.
	glm::vec3 Lander::headingNew(bool b) {
		if (b == true) {
			glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 1, 0)); //rotation around y
			glm::vec3 heading = rot * glm::vec4(0, 0, -1, 1); //apply to a negative z vector (axis is first 3 args, final # is just a way to multiply mat4 with a vec3.)
			return heading;
		}
		if (b == false) {
			glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation + 90), glm::vec3(0, 1, 0)); //rotation around y
			glm::vec3 heading = rot * glm::vec4(0, 0, -1, 1); //apply to a negative z vector (axis is first 3 args, final # is just a way to multiply mat4 with a vec3.)
			return heading;
		}
	}
	bool isSimStarted = true;
	glm::vec3 newPos;
};


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent2(ofDragInfo dragInfo);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void drawAxis(ofVec3f);
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();
		void toggleSelectTerrain();
		void setCameraTarget();
		bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
		bool raySelectWithOctree(ofVec3f &pointRet);
		glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 p , glm::vec3 n);

		//camera stuff
		ofEasyCam cam;
		ofCamera cam1, cam2; //follow and on-board cams.
		ofCamera *theCam; //way to bind 

		//model stuff
		ofxAssimpModelLoader mars;

		//lights
		ofLight light, keyLight, fillLight, rimLight, directionalLight, spotLightFromLander;

		//box stuff and octree stuff
		Box boundingBox, landerBounds;
		Box testBox;
		vector<Box> colBoxList;
		bool bLanderSelected = false;
		Octree octree;
		TreeNode selectedNode;
		glm::vec3 mouseDownPos, mouseLastPos;
		bool bInDrag = false;

		ofTrueTypeFont font;

		//octree level adjust
		ofxIntSlider numLevels;
		ofxPanel gui;

		ofImage background;

		//stuff for particles!
		Particle player;
		ParticleSystem sys;

		//emitter for the conic shape exhaust
		ParticleEmitter conicEmitter;
		//emitter for the explosion of the lander upon crash landing
		ParticleEmitter crashEmitter;
		//forces for the explosion!
		TurbulenceForce *turbForce;
		GravityForce *gravityForce;
		ImpulseRadialForce *radialForce;
		CyclicForce *cyclicForce;
		//end of particle stuff

		//sound stuff
		ofSoundPlayer thrusterSoundPlayer;
		ofSoundPlayer explodeNoise;
		ofSoundPlayer goodLandNoise;
		ofSoundPlayer hardLandNoise;
		ofSoundPlayer backgroundMusic;

		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
		bool bPointSelected;
		bool bCrashCheck; //bool to check if player is outside of the zone.
		bool bCleanLanding; //bool to check if player is inside of zone.
		bool hardLanding; //bool to see if player has hard landing.
		bool bHide;
		bool pointSelected = false;
		bool bDisplayLeafNodes = false;
		bool bDisplayOctree = false;
		bool bDisplayBBoxes = false;
		
		//easy-cam button
		bool camAdjust;

		bool bLanderLoaded;
		bool bTerrainSelected;
	
		ofVec3f selectedPoint;
		ofVec3f intersectPoint;

		vector<Box> bboxList;

		const float selectionRange = 4.0;
		float altitudeWithRayCast();

		ofTime time;
		Lander lander;
		bool bStartSim = false;
};
