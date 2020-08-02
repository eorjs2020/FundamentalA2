#pragma once
#include "Obstacle.h"
#include <vector>
#include <iostream>

class PatternManager
{
private:
	std::vector<Obstacle*> m_obs;
	int timer;
	bool destory;
public:
	PatternManager();
	void Update();
	void Render();
	void Pattern(int n);
	void Destory();
	std::vector<Obstacle*> GetObs();

};

