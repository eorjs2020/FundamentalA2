#pragma once
#include "Sprite.h"

class Obstacle : public Sprite
{
public:
	Obstacle(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, bool saw);
	void Update();
	void Render();
	
private:
	bool m_saw = false;
	
};
