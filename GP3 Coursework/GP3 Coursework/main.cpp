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
#include <vector>
#include <time.h>

#define FONT_SZ 24

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

	srand(time(NULL));

	cModelLoader jigglypuffMdl;
	jigglypuffMdl.loadModel("Models/BR_Jigglypuff.obj");

	cModelLoader falconMdl;
	falconMdl.loadModel("Models/blue_falcon.obj"); // Player

	cModelLoader theDynamite;
	theDynamite.loadModel("Models/dinamite.obj");

	cModelLoader theBolt;
	theBolt.loadModel("Models/laser.obj");

	std::vector<cEnemy*> enemyList;
	std::vector<cEnemy*>::iterator indexEnemy;

	for (int loop = 0; loop < 10; loop++)
	{
		cEnemy* aEnemy = new cEnemy();
		aEnemy->randomise();
		aEnemy->setMdlDimensions(jigglypuffMdl.getModelDimensions());
		enemyList.push_back(aEnemy);
	}

	std::vector<cEnemy*> dynamiteList;
	std::vector<cEnemy*>::iterator indexDynamite;

	for (int loop = 0; loop < 5; loop++)
	{
		cEnemy* aDynamite = new cEnemy();
		aDynamite->randomise();
		aDynamite->setMdlDimensions(theDynamite.getModelDimensions());
		dynamiteList.push_back(aDynamite);
	}

	cPlayer thePlayer;
	thePlayer.initialise(glm::vec3(0, 0, 0), 0.0f, glm::vec3(7, 7, 7), glm::vec3(0, 0, 0), 5.0f, true);
	thePlayer.setMdlDimensions(falconMdl.getModelDimensions());

	std::vector<cLaser*> laserList;
	std::vector<cLaser*>::iterator index;

	// Load font
	struct dtx_font *fntmain;

	fntmain = dtx_open_font("Fonts/font.ttf", 0);
	dtx_prepare_range(fntmain, FONT_SZ, 0, 256);             /* ASCII */

	dtx_use_font(fntmain, FONT_SZ);

	cSound themeMusic;
	themeMusic.createContext();
	themeMusic.loadWAVFile("Audio/patel.wav");
	themeMusic.playAudio(AL_LOOPING);

	

	//explosion
	cSound explosionFX;
	//explosionFX.createContext();
	explosionFX.loadWAVFile("Audio/feelingit.wav");

	//firing sound
	cSound firingFX;
	//firingFX.createContext();
	firingFX.loadWAVFile("Audio/shot.wav");

	//death sound
	cSound deathFX;
	//deathFX.createContext();
	deathFX.loadWAVFile("Audio/goofy.wav");

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
		if (topDown == true)
		{
			gluLookAt(0, 200, 0, 0, 0, 0, 0, 0, 1);
		}
		else{
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
			//glScalef(3, 3, 3);
			jigglypuffMdl.renderMdl((*indexEnemy)->getPosition(), (*indexEnemy)->getRotation(), (*indexEnemy)->getScale());
			(*indexEnemy)->update(elapsedTime);

		}
		for (indexDynamite = dynamiteList.begin(); indexDynamite != dynamiteList.end(); ++indexDynamite)
		{
			//glScalef(3, 3, 3);
			theDynamite.renderMdl((*indexDynamite)->getPosition(), (*indexDynamite)->getRotation(), (*indexDynamite)->getScale());
			(*indexDynamite)->update(elapsedTime);

		}
		falconMdl.renderMdl(thePlayer.getPosition(), thePlayer.getRotation(), thePlayer.getScale());
		thePlayer.update(elapsedTime);

		// Load Sound
		if (bgmPlaying == false)
		{
			themeMusic.playAudio(AL_FALSE);
		}

		////are we shooting?
		if (fire && dead == false)
		{
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
		}


		for (index = laserList.begin(); index != laserList.end(); ++index)
		{
			if ((*index)->isActive())
			{
				theBolt.renderMdl((*index)->getPosition(), (*index)->getRotation(), (*index)->getScale());
				(*index)->update(elapsedTime);
				// check for collisions
				for (indexEnemy = enemyList.begin(); indexEnemy != enemyList.end(); ++indexEnemy)
				{
					if ((*index)->SphereSphereCollision((*indexEnemy)->getPosition(), (*indexEnemy)->getMdlRadius()))
					{
						(*index)->setIsActive(false);
						(*indexEnemy)->setIsActive(false);
						score = score + 1;
						explosionFX.playAudio(AL_FALSE);
						break; // No need to check for other bullets.
					}

				}
				for (indexDynamite = dynamiteList.begin(); indexDynamite != dynamiteList.end(); ++indexDynamite)
				{
					if ((*index)->SphereSphereCollision((*indexDynamite)->getPosition(), (*indexDynamite)->getMdlRadius()))
					{
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
			if ((*indexDynamite)->SphereSphereCollision(thePlayer.getPosition(), thePlayer.getMdlRadius()))
			{
				(*indexDynamite)->setIsActive(false);
				health = health - 1;
				deathFX.playAudio(AL_FALSE);
				break;
			}

		}

		if (health == 0){
			dead = true;
		}

		index = laserList.begin();
		while (index != laserList.end())
		{
			if ((*index)->isActive() == false)
			{
				index = laserList.erase(index);
			}
			else
			{
				++index;
			}
		}
		indexEnemy = enemyList.begin();
		while (indexEnemy != enemyList.end())
		{
			if ((*indexEnemy)->isActive() == false)
			{
				indexEnemy = enemyList.erase(indexEnemy);
			}
			else
			{
				++indexEnemy;
			}
		}
		indexDynamite = dynamiteList.begin();
		while (indexDynamite != dynamiteList.end())
		{
			if ((*indexDynamite)->isActive() == false)
			{
				indexDynamite = dynamiteList.erase(indexDynamite);
			}
			else
			{
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

		glTranslatef(0, 720, 0);


		std::string scoreString = "Dead Jigglypuffs: " + std::to_string(score);
		char const *ourScore = scoreString.c_str();
		dtx_string(ourScore);

		glTranslatef(800, 0, 0);

		std::string healthString = "Blue Falcon Health: " + std::to_string(health);
		char const *ourHealth = healthString.c_str();
		dtx_string(ourHealth);

		glTranslatef(-600, -50, 0);

		if (dead == false){
			std::string instructString = "SPACE to fire, Q to toggle cameras and E to toggle background music!";
			char const *Controls = instructString.c_str();
			dtx_string(Controls);
		}

		if (dead == true){
			std::string instructString = "You are dead. The Jigglypuffs will taste blood tonight.";
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