#ifndef BONUSLIFE_H
#define BONUSLIFE_H
#include "GameObject.h"

class BonusLife : public GameObject {
public:

	BonusLife(float x, float y);


	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
};
#endif