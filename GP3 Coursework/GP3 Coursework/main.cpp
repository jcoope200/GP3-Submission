#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#define GLX_GLXEXT_LEGACY

#include "GameConstants.h"
#include <Windows.h>
#include "windowOGL.h"
#include "cWNDManager.h"
#include "libdrawtext-0.2.1\drawtext.h"
#include "cModelLoader.h"
#include "cModel.h"
#include "cEnemy.h"
#include "cPlayer.h"
#include "cLaser.h"
#include "cSound.h"
#include "CXBOXController.h"
#include <vector>
#include <time.h>

#define FONT_SZ 24

//********************************************************//
//WinMain is the first method called when the program runs//
//********************************************************//

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	//Set our window settings
	const int windowWidth = 1024;
	const int windowHeight = 768;
	const int windowBPP = 16;

	//This is our window
	static cWNDManager* pgmWNDMgr = cWNDManager::getInstance();

	//The example OpenGL code
	windowOGL theOGLWnd;

	//Attach our example to our window
	pgmWNDMgr->attachOGLWnd(&theOGLWnd);

	float elapsedTime = pgmWNDMgr->getElapsedSeconds();
	int score = 0;

	//Attempt to create the window
	if (!pgmWNDMgr->createWND(windowWidth, windowHeight, windowBPP))
	{
		//If it fails
		MessageBox(NULL, "Unable to create the OpenGL window!", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
		return 1;
	}
	if (!theOGLWnd.initOGL()) //Initialize our example
	{
		MessageBox(NULL, "Could not initalize the application!", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
		return 1;
	}

	//seed the random number generator
	srand(time(NULL));

	//load each model

	//jigglypuff is the enemy model
	cModelLoader jigglypuffMdl;
	jigglypuffMdl.loadModel("Models/BR_Jigglypuff.obj");

	//the blue falcon is the player model
	cModelLoader falconMdl;
	falconMdl.loadModel("Models/blue_falcon.obj"); // Player

	//the dynamite is an obstacle model
	cModelLoader theDynamite;
	theDynamite.loadModel("Models/dinamite.obj");

	//the laser is the ammunition model
	cModelLoader theBolt;
	theBolt.loadModel("Models/laser.obj");

	//create a vector list of enemies as well as an iterator to go through the list
	std::vector<cEnemy*> enemyList;
	std::vector<cEnemy*>::iterator indexEnemy;

	for (int loop = 0; loop < 10; loop++)
	{
		//for each of the ten enemies, declare them as an instance of cEnemy, give them Jigglypuff's model dimensions and randomise their positions on the screen
		cEnemy* aEnemy = new cEnemy();
		aEnemy->randomise();
		aEnemy->setMdlDimensions(jigglypuffMdl.getModelDimensions());
		enemyList.push_back(aEnemy);
	}

	//create a vector list of dynamite packs as well as an iterator to go through the list
	std::vector<cEnemy*> dynamiteList;
	std::vector<cEnemy*>::iterator indexDynamite;

	for (int loop = 0; loop < 5; loop++)
	{
		//for each of the five dynamite packs, declare them as an instance of cEnemy, give them the dynamite's model dimensions and randomise their positions on the screen
		cEnemy* aDynamite = new cEnemy();
		aDynamite->randomise();
		aDynamite->setMdlDimensions(theDynamite.getModelDimensions());
		aDynamite->setScale(glm::vec3(5, 5, 5));
		dynamiteList.push_back(aDynamite);
	}
	
	//create the player object and instantiate it on the screen, scaling the model down from the regular large size
	cPlayer thePlayer;
	thePlayer.initialise(glm::vec3(0, 0, 0), 0.0f, glm::vec3(7, 7, 7), glm::vec3(0, 0, 0), 5.0f, true);
	thePlayer.setMdlDimensions(falconMdl.getModelDimensions());

	//create a vector list of lasers as well as an iterator to go through the list
	std::vector<cLaser*> laserList;
	std::vector<cLaser*>::iterator index;

	// Load font
	struct dtx_font *fntmain;

	//specify the font that will be used to write the overlays
	fntmain = dtx_open_font("Fonts/font.ttf", 0);
	dtx_prepare_range(fntmain, FONT_SZ, 0, 256);             /* ASCII */

	dtx_use_font(fntmain, FONT_SZ);

	//instantiate, load and play the background music
	cSound themeMusic;
	themeMusic.createContext();
	themeMusic.loadWAVFile("Audio/patel.wav");
	themeMusic.playAudio(AL_LOOPING);

	//instantiate and load the sound effect for hitting Jigglypuff or a dynamite pack
	cSound explosionFX;
	//explosionFX.createContext();
	explosionFX.loadWAVFile("Audio/feelingit.wav");

	//instantiate and load the sound effect for shooting the laser
	cSound firingFX;
	//firingFX.createContext();
	firingFX.loadWAVFile("Audio/shot.wav");

	//instantiate and load the sound effect for getting hit by the dynamite
	cSound deathFX;
	//deathFX.createContext();
	deathFX.loadWAVFile("Audio/goofy.wav");

	//create an instance of the xbox controller class and assign it to player 1
	CXBOXController* gamepad;
	gamepad = new CXBOXController(1);
	
	//This is the mainloop, we render frames until isRunning returns false
	while (pgmWNDMgr->isWNDRunning())
	{
		pgmWNDMgr->processWNDEvents(); //Process any window events
		//We get the time that passed since the last frame
		float elapsedTime = pgmWNDMgr->getElapsedSeconds();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		theOGLWnd.initOGL();
		glClearColor(0.8, 0.9, 1, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		if (topDown == true && gamepad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y)
		{
			//if Q (Y on Xbox controller) has been pressed while in the first person camera, switch to the top down camera
			//200 refers to the zoom, the eye is 200 units on the Y away from the camera's target
			//1 refers to the up vector on the Z axis
			gluLookAt(0, 200, 0, 0, 0, 0, 0, 0, 1);
		}
		else
		{
			//if Q (Y on Xbox controller) has been pressed while in the top down camera, switch to the first person camera
			//FPScam is an object created to always rotate with where the player is facing
			//pos is an object created to always stay on the player's position
			//combined, the two objects produce a lookAt that follows the front of the ship
			//3 is an arbitrary integer to move the eye's view back so that the view is the "pilot"'s rather than the front of the ship
			glm::vec3 FPScam;
			FPScam.x = -(float)glm::sin(glm::radians(thePlayer.getRotation()));
			FPScam.y = 0.0f;
			FPScam.z = (float)glm::cos(glm::radians(thePlayer.getRotation()));
			FPScam *= -1;

			glm::vec3 pos;
			pos = thePlayer.getPosition();

			gluLookAt(pos.x, 3, -pos.z, pos.x + FPScam.x, 3, -(pos.z + FPScam.z), 0, 1, 0);
		}

		for (indexEnemy = enemyList.begin(); indexEnemy != enemyList.end(); ++indexEnemy)
		{
			//render and update each Jigglypuff in the enemy list
			jigglypuffMdl.renderMdl((*indexEnemy)->getPosition(), (*indexEnemy)->getRotation(), (*indexEnemy)->getScale());
			(*indexEnemy)->update(elapsedTime);

		}
		for (indexDynamite = dynamiteList.begin(); indexDynamite != dynamiteList.end(); ++indexDynamite)
		{
			//render and update each dynamite pack in the enemy list
			theDynamite.renderMdl((*indexDynamite)->getPosition(), (*indexDynamite)->getRotation(), (*indexDynamite)->getScale());
			(*indexDynamite)->update(elapsedTime);

		}

		//render and update the player
		falconMdl.renderMdl(thePlayer.getPosition(), thePlayer.getRotation(), thePlayer.getScale());
		thePlayer.update(elapsedTime);

		// Load Sound
		if ((fire || gamepad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B) && bgmPlaying == false)
		{
			//if E was pressed while the background music wasn't playing
			//or if it was B on the Xbox controller
			themeMusic.playAudio(AL_FALSE);
		}
		gamepad->Vibrate();
		////are we shooting?
		if ((fire || gamepad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A) && dead == false)
		{
			//if we're shooting and we're not dead, instantiate a laser object
			//we can shoot with the A button on the Xbox controller too
			//give the object a direction and rotation based on where the player is facing using sine and cosine
			//scale the laser and add the firing speed
			//push the laser back in the list and set fire to false
			//play the firing sound effect
			cLaser* laser = new cLaser();
			glm::vec3 mdlLaserDirection;
			mdlLaserDirection.x = -(float)glm::sin(glm::radians(thePlayer.getRotation()));
			mdlLaserDirection.y = 0.0f;
			mdlLaserDirection.z = (float)glm::cos(glm::radians(thePlayer.getRotation()));
			mdlLaserDirection *= -1;
			laser->setDirection(mdlLaserDirection);   ///     glm::vec3(0, 0, 5)
			laser->setRotation(thePlayer.getRotation());
			laser->setScale(glm::vec3(5, 5, 5));
			laser->setSpeed(40.0f);
			laser->setPosition(thePlayer.getPosition() + mdlLaserDirection);    //   glm::vec3(0, 0, 0)
			laser->setIsActive(true);
			laser->setMdlDimensions(theBolt.getModelDimensions());
			laser->update(elapsedTime);
			laserList.push_back(laser);
			fire = false;
			firingFX.playAudio(AL_FALSE);
			gamepad->Vibrate(65535, 65535);
		}


		for (index = laserList.begin(); index != laserList.end(); ++index)
		{
			//for each laser in the list
			if ((*index)->isActive())
			{
				//if the laser is still active, update and render it
				theBolt.renderMdl((*index)->getPosition(), (*index)->getRotation(), (*index)->getScale());
				(*index)->update(elapsedTime);
				// check for collisions
				for (indexEnemy = enemyList.begin(); indexEnemy != enemyList.end(); ++indexEnemy)
				{
					//for each Jigglypuff in the enemy list
					if ((*index)->SphereSphereCollision((*indexEnemy)->getPosition(), (*indexEnemy)->getMdlRadius()))
					{
						//if the bounding spheres have collided for the laser and the Jigglypuff the two iterators are pointing to
						//set them each to inactive
						//add one to the player's score and play the hit target audio
						(*index)->setIsActive(false);
						(*indexEnemy)->setIsActive(false);
						score = score + 1;
						explosionFX.playAudio(AL_FALSE);
						break; // No need to check for other bullets.
					}

				}
				for (indexDynamite = dynamiteList.begin(); indexDynamite != dynamiteList.end(); ++indexDynamite)
				{
					//for each dynamite in the list
					if ((*index)->SphereSphereCollision((*indexDynamite)->getPosition(), (*indexDynamite)->getMdlRadius()))
					{
						//if the bounding spheres have collided for the laser and the dynamite pack the two iterators are pointing to
						//set them each to inactive
						//and play the hit target audio
						(*index)->setIsActive(false);
						(*indexDynamite)->setIsActive(false);
						explosionFX.playAudio(AL_FALSE);
						break; // No need to check for other bullets.
					}

				}
			}
		}

		for (indexDynamite = dynamiteList.begin(); indexDynamite != dynamiteList.end(); ++indexDynamite)
		{
			//for each dynamite pack in the dynamite list
			if ((*indexDynamite)->SphereSphereCollision(thePlayer.getPosition(), thePlayer.getMdlRadius()))
			{
				//if the bounding spheres have collided for the dynamite pack the iterator is pointing to and the player
				//set the dynamite pack to inactive
				//reduce our health by one and play the hit audio
				(*indexDynamite)->setIsActive(false);
				health = health - 1;
				gamepad->Vibrate(65535, 65535);
				deathFX.playAudio(AL_FALSE);
				break;
			}

		}

		if (health == 0)
		{
			//if we've ran out of health we are dead
			dead = true;
		}

		//start at the beginning of the laser list
		index = laserList.begin();
		while (index != laserList.end())
		{
			//while we are still in the laser list
			if ((*index)->isActive() == false)
			{
				//if the laser the iterator is pointing to is inactive, erase it from the list
				index = laserList.erase(index);
			}
			else
			{
				//otherwise move on to the next one
				++index;
			}
		}

		//start at the beginning of the Jigglypuff list
		indexEnemy = enemyList.begin();
		while (indexEnemy != enemyList.end())
		{
			//while we are still in the Jigglypuff list
			if ((*indexEnemy)->isActive() == false)
			{
				//if the Jigglypuff the iterator is pointing to is inactive, erase it from the list
				indexEnemy = enemyList.erase(indexEnemy);
			}
			else
			{
				//otherwise move on to the next one
				++indexEnemy;
			}
		}

		//start at the beginning of the dynamite list
		indexDynamite = dynamiteList.begin();
		while (indexDynamite != dynamiteList.end())
		{
			//while we are still in the dynamite list
			if ((*indexDynamite)->isActive() == false)
			{
				//if the dynamite the iterator is pointing to is inactive, erase it from the list
				indexDynamite = dynamiteList.erase(indexDynamite);
			}
			else
			{
				//otherwise move on to the next one
				++indexDynamite;
			}
		}

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, windowWidth, 0, windowHeight);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glPushAttrib(GL_DEPTH_TEST);
		glDisable(GL_DEPTH_TEST);

		//set the first string overlay at this position
		//it is a constant string so that the game's output can use it to display
		//this first one represents the score
		glTranslatef(50, 680, 0);
		std::string scoreString = "Dead Jigglypuffs: " + std::to_string(score);
		char const *ourScore = scoreString.c_str();
		dtx_string(ourScore);

		//this translate is relative to the first string
		//this string represents our health
		glTranslatef(500, 0, 0);
		//std::string healthString = "Blue Falcon Health: " + std::to_string(health);
		std::string healthString = "Blue Falcon Health: " + std::to_string(health);
		char const *ourHealth = healthString.c_str();
		dtx_string(ourHealth);

		//this translate is relative to the second string
		//this string represents a message to the player
		glTranslatef(-400, -70, 0);
		if (dead == false && score < 10)
		{
			//if we aren't dead and we haven't killed all the enemies
			//display the controls
			std::string instructString = "SPACE to fire, Q to toggle cameras and E to toggle background music!";
			char const *Controls = instructString.c_str();
			dtx_string(Controls);
		}

		if (dead == true)
		{
			//if we died display a message to the player instead of the controls
			std::string instructString = "You are dead. The Jigglypuffs will taste blood tonight.";
			char const *Controls = instructString.c_str();
			dtx_string(Controls);
		}

		if (score >= 10)
		{
			//if we killed all the Jigglypuffs display a message to the player instead of the controls
			std::string instructString = "You monster, all they wanted to do was spread music.";
			char const *Controls = instructString.c_str();
			dtx_string(Controls);
		}
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		pgmWNDMgr->swapBuffers();
	}

	theOGLWnd.shutdown(); //Free any resources
	pgmWNDMgr->destroyWND(); //Destroy the program window

	return 0; //Return success
}