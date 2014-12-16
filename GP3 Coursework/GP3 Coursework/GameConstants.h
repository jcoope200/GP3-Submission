#ifndef _GAMECONSTANTS_H
#define _GAMECONSTANTS_H

#define GLM_FORCE_RADIANS

#include <stdlib.h>
#include <Windows.h>
#include <GL\glut.h>
//#include <assimp\Importer.hpp>    // C++ importer interface
//#include <assimp\scene.h>           // Output data structure
//#include <assimp\postprocess.h>     // Post processing flags
#include <OpenAL\al.h>
#include <OpenAL\alc.h>
#include <OpenAL\alut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\geometric.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\quaternion.hpp>
#include "glm-0.3.2\glm\glm.h"
#include <vector>


//Define the string to appear in the top left corner of the window
#define WINDOW_TITLE "OpenGl Models, Lighting and Text"

struct mdlDimensions{
	float s_mdlWidth, s_mdlheight, s_mdldepth;
};

const float PLAYFIELDX = 50.0f;
const float PLAYFIELDZ = 150.0f;

extern int drawMode;
extern float rotationAngle;
extern float translationZ;
extern bool fire;
extern bool topDown;
extern bool bgmPlaying;
extern int health;
extern bool dead;

#endif