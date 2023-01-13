#include "BonusLife.h"
#include "BoundingSphere.h"
#include "Player.h"
#include "Asteroids.h"

//Contructer of bonus life which inherits GameObject
BonusLife::BonusLife(float x, float y) : GameObject("BonusLife") {
	//Set position to the given position
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = 0.0;

}

bool BonusLife::CollisionTest(shared_ptr<GameObject> o)
{
	//If collision test is against a spaceship simulate collsion event
	if (o->GetType() == GameObjectType("Spaceship")) {
		return mBoundingShape->CollisionTest(o->GetBoundingShape());
	}
	if (o->GetType() == GameObjectType("Bullet")) return false;
	if (o->GetType() == GameObjectType("Asteroid")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());

}

void BonusLife::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}
