#ifndef PLAYER_H
#define PLAYER_H
#include "GameUtil.h"
#include "GameObject.h"
#include "GameObjectType.h"
#include "IPlayerListener.h"
#include "IGameWorldListener.h"
class Player : public IGameWorldListener
{
public:
	Player() { mLives = 3; }
	virtual ~Player() {}
	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
	{
		if (object->GetType() == GameObjectType("Spaceship")) {
			mLives -= 1;
			FirePlayerKilled();
		}
	}
	
	

	void AddListener(shared_ptr<IPlayerListener> listener)
	{
		mListeners.push_back(listener);
	}

	void AddLife() {
		mLives += 1;
		for (PlayerListenerList::iterator lit = mListeners.begin();
			lit != mListeners.end(); ++lit) {
			(*lit)->UpdateLifeCount(mLives);
		}
	}
	void FirePlayerKilled()
	{
		// Send message to all listeners
		for (PlayerListenerList::iterator lit = mListeners.begin();
			lit != mListeners.end(); ++lit) {
			(*lit)->OnPlayerKilled(mLives);
		}
	}
private:
	int mLives;
	typedef std::list< shared_ptr<IPlayerListener> > PlayerListenerList;
	PlayerListenerList mListeners;
};
#endif