#include <stdlib.h>
#include "GameUtil.h"
#include "Asteroid.h"
#include "BoundingShape.h"
#include "BonusLife.h"

//Constructs Asteroid
Asteroid::Asteroid(void) : GameObject("Asteroid")
{
	mAngle = rand() % 360;
	mRotation = rand() % 90;
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 10.0 * cos(DEG2RAD * mAngle);
	mVelocity.y = 10.0 * sin(DEG2RAD * mAngle);
	mVelocity.z = 0.0;
}
Asteroid::~Asteroid(void)
{
}
/* Now in Shape.cpp
void Asteroid::Render(void)
{
	// Disable lighting for solid colour lines
	glDisable(GL_LIGHTING);
	// Start drawing lines
	glBegin(GL_LINE_LOOP);
	// Set colour to grey
	glColor3f(0.6, 0.6, 0.6);
	// Add vertices to draw an octagon
	glVertex3f(-7, -7, 0.0);
	glVertex3f(-10, 0, 0.0);
	glVertex3f(-7, 7, 0.0);
	glVertex3f(0, 10, 0.0);
	glVertex3f(7, 7, 0.0);
	glVertex3f(10, 0, 0.0);
	glVertex3f(7, -7, 0.0);
	glVertex3f(0, -10, 0.0);
	// Finish drawing lines
	glEnd();
	// Enable lighting
	glEnable(GL_LIGHTING);
}
*/

//Collision Tester
bool Asteroid::CollisionTest(shared_ptr<GameObject> o)
{	
	//If other is of type asteroid return collision false
	if (GetType() == o->GetType()) return false;
	if (mBoundingShape.get() == NULL) return false;
	//If collides with BonusLife return collision false
	if (o->GetType() == GameObjectType("BonusLife")) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	//Else return collision
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}
//On Collision
void Asteroid::OnCollision(const GameObjectList& objects)
{
	//Flag object to be removed from the game world
	mWorld->FlagForRemoval(GetThisPtr());
}