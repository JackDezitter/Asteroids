#ifndef IPLAYERLISTENER_H
#define IPLAYERLISTENER_H
class IPlayerListener
{
public:
	virtual void OnPlayerKilled(int lives_left) = 0;
	virtual void UpdateLifeCount(int lives_left) = 0;
};
#endif