#include "cPlayer.h"

cPlayer::cPlayer() : cModel()
{

	
}

void cPlayer::update(float elapsedTime)
{
	if (dead == false)
	{

		// Find out what direction we should be thrusting, using rotation.
		glm::vec3 mdlVelocityAdd;
		mdlVelocityAdd.x = -(float)glm::sin(glm::radians(cModel::m_mdlRotation));
		mdlVelocityAdd.y = 0.0f;
		mdlVelocityAdd.z = -(float)glm::cos(glm::radians(cModel::m_mdlRotation));

		cModel::m_mdlRotation -= rotationAngle;

		mdlVelocityAdd *= translationZ;
		cModel::m_mdlDirection += mdlVelocityAdd;

		cModel::m_mdlPosition += cModel::m_mdlDirection * cModel::m_mdlSpeed *elapsedTime;
		cModel::m_mdlDirection *= 0.95f;

		rotationAngle = 0;
		translationZ = 0;
	}

	
}


cPlayer::~cPlayer()
{

}