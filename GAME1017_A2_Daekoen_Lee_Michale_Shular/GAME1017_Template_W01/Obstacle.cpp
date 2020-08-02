#include "Obstacle.h"
#include <ctime>
#include "TextureManager.h"
#include "Engine.h"

Obstacle::Obstacle(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, bool saw)
	:Sprite (s, d, r, t)
{
	m_saw = saw;
}

void Obstacle::Update()
{
	m_dst.x -= 2;
	if (m_saw)
	{
		if(m_angle >= 360)
			m_angle = 0;
		m_angle += 10;
	}
}

void Obstacle::Render()
{
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, SDL_FLIP_NONE);
}

bool Obstacle::Saw()
{
	return m_saw;
}

