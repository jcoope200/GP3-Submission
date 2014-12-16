/*
==================================================================================
GameConstants.cpp
==================================================================================
*/

#include "GameConstants.h"

int drawMode = 0;
float rotationAngle = 0;
float translationZ = 0;
bool fire = false; //whether or not we're shooting, by default we aren't
bool topDown = true; //whether or not we're in the top down view, by default we are
bool bgmPlaying = true; //whether or not we're playing the background music, by default we are
int health = 5; //we start with 5 health
bool dead = false; //whether or not we're dead, by default we aren't