#pragma once
#include "Sprite.h"

class Obstacle
{
public:
	Obstacle();
	void Update();
	void Render();
	
private:
	int genNum, choosenpattern;
	enum state {patternone, patternTwo, patterthree } m_pstate;
	Sprite* obstaclePatternOne[3],* obstaclePatternTwo[3], *obstaclePatternThree[3];
};
