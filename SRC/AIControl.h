#ifndef AICONTROL_H
#define AICONTROL_H
#include "GameUtil.h"
#include "GameObject.h"
#include "GameObjectType.h"
#include "AIControlListener.h"
#include "IGameWorldListener.h"

// Listener for AI to detect asteroids positioning
class AIControl : public IGameWorldListener
{

public:
	AIControl() {}
	virtual ~AIControl() {}
	//Ever World update run
	void OnWorldUpdated(GameWorld* world) {
		AsteroidCheck();
	}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object){}
	void AddListener(shared_ptr<AIControlListener> listener)
	{
		mListeners.push_back(listener);
	}
	//For all litners run AIDetectAsteroids
	void AsteroidCheck()
	{
		// Send message to all listeners
		for (AIControlListenerList::iterator lit = mListeners.begin();
			lit != mListeners.end(); ++lit) {
			(*lit)->AIDetectAsteroids(detect);
		}
	}
private:
	//Variable required as parameter for Asteroids class to inherit AIControlListener 
	bool detect = true;
	//List of AIControlListeners
	typedef std::list< shared_ptr<AIControlListener> > AIControlListenerList;
	AIControlListenerList mListeners;
};
#endif