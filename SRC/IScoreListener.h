#ifndef ISCORELISTENER_H
#define ISCORELISTENER_H
class IScoreListener
{
public:
	virtual void OnScoreChanged(int score) = 0;
};
#endif