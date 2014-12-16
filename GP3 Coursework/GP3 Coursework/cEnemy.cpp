#include "cEnemy.h"

cEnemy::cEnemy() : cModel()
{
	
}

void cEnemy::randomise()
{
	if (rand() % 2 == 0)
	{
		cModel::m_mdlPosition.x = -PLAYFIELDX + 2.0f;
	}
	else
	{
		cModel::m_mdlPosition.x = PLAYFIELDX - 2.0f;
	}
	cModel::m_mdlPosition.y = 0.0f;
	cModel::m_mdlPosition.z = (rand() / (float)RAND_MAX) * 5.0f;   // random number as a float between 0 & 1
	cModel::m_mdlRotation = (rand() / (float)RAND_MAX) * 2 * 3.14f;
	cModel::m_mdlDirection.x = -(float)glm::sin(cModel::m_mdlRotation);
	cModel::m_mdlDirection.z = (float)glm::cos(cModel::m_mdlRotation);
	cModel::m_mdlSpeed = m_EnemyMinSpeed + rand() / (float)RAND_MAX * m_EnemyMaxSpeed;
	cModel::m_IsActive = true;
}

void cEnemy::update(float elapsedTime)
{
	cModel::m_mdlPosition += cModel::m_mdlDirection * cModel::m_mdlSpeed * elapsedTime;
	if (cModel::m_mdlPosition.x > PLAYFIELDX && cModel::m_mdlDirection.x > 0)
		cModel::m_mdlDirection.x = -cModel::m_mdlDirection.x;	
	if (cModel::m_mdlPosition.x < -PLAYFIELDX && cModel::m_mdlDirection.x < 0)
		cModel::m_mdlDirection.x = -cModel::m_mdlDirection.x;
	if (cModel::m_mdlPosition.z > PLAYFIELDZ && cModel::m_mdlDirection.z > 0)
		cModel::m_mdlDirection.z = -cModel::m_mdlDirection.z;
	if (cModel::m_mdlPosition.z < -PLAYFIELDZ && cModel::m_mdlDirection.z < 0)
		cModel::m_mdlDirection.z = -cModel::m_mdlDirection.z;
}

cEnemy::~cEnemy()
{

}

