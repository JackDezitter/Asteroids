#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h"
#include "IGameWorldListener.h"
#include "IScoreListener.h"
#include "ScoreKeeper.h"
#include "GUILabel.h"
#include "IPlayerListener.h"
#include "Player.h"
#include "AIControlListener.h"
#include "AIControl.h"

class GameObject;
class Spaceship;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public GUILabel, public IPlayerListener, public AIControlListener
{

typedef list< shared_ptr< GameObject > > GameObjectList;

public:
	Asteroids(int argc, char *argv[]);
	virtual ~Asteroids(void);

	virtual void Start(void);
	virtual void Stop(void);

	// Declaration of IKeyboardListener interface ////////////////////////////////

	void OnKeyPressed(uchar key, int x, int y);
	void OnKeyReleased(uchar key, int x, int y);
	void OnSpecialKeyPressed(int key, int x, int y);
	void OnSpecialKeyReleased(int key, int x, int y);

	//Declaration of AIMovement functions
	void OnAIMovementPressed(int key, int x, int y);
	void OnAIMovementReleased(int key, int x, int y);

	//Declaration of IScoreListener
	void OnScoreChanged(int score);
	ScoreKeeper mScoreKeeper;

	//Declaration of AIControlListener
	void AIDetectAsteroids(bool detect);
	AIControl mAIController;

	// Declaration of IGameWorldListener interface //////////////////////////////

	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);


	// Override the default implementation of ITimerListener ////////////////////
	void OnTimer(int value);

	//Runs on Player Killed
	void OnPlayerKilled(int lives_left);
	//Updates Players life counter
	void UpdateLifeCount(int lives_left);
	Player mPlayer;

	

private:
	shared_ptr<Spaceship> mSpaceship;
	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mGameOverLabel;
	shared_ptr<GUILabel> mTitleLabel;
	shared_ptr<GUILabel> mStartLabel;

	bool demoMode;
	uint mLevel;
	uint mAsteroidCount;
	float closestTargetDistance;
	shared_ptr<GameObject> closestTarget;
	float previousClosestDistance;
	shared_ptr<GameObject> previousClosestTarget;

	GameObjectList mGameObjects;

	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();

	shared_ptr<GameObject> CreateExplosion();

	void CreateGUI();

	void CreateAsteroids(const uint num_asteroids);

	void CreateBonusLife(float xPosition, float yPosition);

	void AIController(shared_ptr<GameObject> closestTarget);
	int reloadTimer = 0;
	
	void AIDecision(float c);

	void AIShoot();

	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;
	const static uint CLEAR_DEMO = 4;
};

#endif