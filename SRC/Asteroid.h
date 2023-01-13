#ifndef ASTEROID_H
#define ASTEROID_H
#include "GameObject.h"
class Asteroid : public GameObject
{
	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);
public:
	Asteroid(void);
	~Asteroid(void);
	//void Render(void);
};
#endif