#pragma once
#ifndef _STATES_H_
#define _STATES_H_
#define ROWS 48
#define COLS 64

#include <SDL.h>
#include <array>
#include <map>
#include <vector>
#include "Button.h"
#include "PlatformPlayer.h"
#include "Tile.h"
#include "Timer.h"
#include "Label.h"
#include "Obstacle.h"
#include "PatternManager.h"

class State // This is the abstract base class for all specific states.
{
protected:
	State() {}
public:
	virtual void Update() = 0; // Having at least one 'pure virtual' method like this, makes a class abtract.
	virtual void Render();     // Meaning we cannot create objects of the class.
	virtual void Enter() = 0;  // Virtual keyword means we can override in derived class.
	virtual void Exit() = 0;
	virtual void Resume();
};

class GameState : public State
{
private:
	Label* m_timer;
	SDL_Texture *m_pTileText;
	std::map<char, Tile*> m_tiles;
	std::array<std::array<Tile*, COLS>, ROWS> m_level; // Fixed-size STL array of Tile pointers.
	std::vector<Tile*> m_platforms;
	std::string m_TimerNum, m_defualtTimer = "Timer: 0", m_updateTimer;
	Sprite* m_pBackgroundOne[2], *m_pBackgroundTwo[5];
	Sprite* m_pPlatform[3];
	int m_pSrollSpeed[3] = { 1, 2, 4 };
	PlatformPlayer* m_pPlayer;
	LTimer timer;
	bool m_bgScrollX = false, m_bgScrollY = false;
	PatternManager* m_pObs;
public:
	GameState();
	void Update();
	void UpdateTiles(float scroll, bool x = false);
	void CheckCollision();
	void Render();
	void Enter();
	void Exit();
	void Resume();
};

class TitleState : public State
{
public:
	TitleState();
	void Update();
	void Render();
	void Enter();
	void Exit();
private:
	Button* m_playBtn;
};

#endif