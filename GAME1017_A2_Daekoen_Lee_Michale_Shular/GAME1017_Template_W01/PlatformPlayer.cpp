#include "PlatformPlayer.h"
#include "Engine.h"
#include <algorithm>
#include <iostream>
#include "EventManager.h"
#include "TextureManager.h"

PlatformPlayer::PlatformPlayer(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf)
	:AnimatedSprite(s, d, r, t, sstart, smin, smax, nf), m_state(running), m_dir(0)
{
	m_grounded = false;
	m_accelX = m_accelY = m_velX = m_velY = 0.0;
	m_maxVelX = 10.0;
	m_maxVelY = JUMPFORCE;
	m_grav = GRAV;
	m_drag = 0.88;
}

void PlatformPlayer::Update()
{
	// Do X axis first.
	m_velX += m_accelX;
	m_velX *= (m_grounded?m_drag:1); 
	m_velX = std::min(std::max(m_velX, -(m_maxVelX)), (m_maxVelX));
	
	m_dst.x += (int)m_velX; // Had to cast it to int to get crisp collision with side of platform.
	// Now do Y axis.
	m_velY += m_accelY + m_grav; // Adjust gravity to get slower jump.
	m_velY = std::min(std::max(m_velY, -(m_maxVelY)), (m_grav*5));
	
	m_dst.y += (int)m_velY; // To remove aliasing, I made cast it to an int too.
	m_accelX = m_accelY = 0.0;
	switch (m_state)
	{
	
	case running:
		
		if (EVMA::KeyHeld(SDL_SCANCODE_S) )
		{
			SetState(rolling);
			break; // Skip movement parsing below.
		}
		if (EVMA::KeyHeld(SDL_SCANCODE_A))
		{
			m_dir = 1;
		}
		else if (EVMA::KeyHeld(SDL_SCANCODE_D))
		{
			m_dir = 0;

		}
		break;
	
	case rolling:

		if (EVMA::KeyReleased(SDL_SCANCODE_S))
		{
			SetState(running);
			break; // Skip movement parsing below.
		}

		break;
	}
	Animate();
}

void PlatformPlayer::Render()
{
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, static_cast<SDL_RendererFlip>(m_dir));

}

void PlatformPlayer::Stop() // If you want a dead stop both axes.
{
	StopX();
	StopY();
}
void PlatformPlayer::StopX() { m_velX = 0.0; }
void PlatformPlayer::StopY() { m_velY = 0.0; }

void PlatformPlayer::SetAccelX(double a) { m_accelX = a; }
void PlatformPlayer::SetAccelY(double a) { m_accelY = a; }
bool PlatformPlayer::IsGrounded() { return m_grounded; }
void PlatformPlayer::SetGrounded(bool g) { m_grounded = g; }
double PlatformPlayer::GetVelX() { return m_velX; }
double PlatformPlayer::GetVelY() { return m_velY; }
void PlatformPlayer::SetX(float y) { m_dst.x = y; }
void PlatformPlayer::SetY(float y) { m_dst.y = y; }

void PlatformPlayer::SetState(int s)
{
	m_state = static_cast<state>(s);
	m_frame = 0;
	
	if (m_state == running)// Only other is running for now...
	{
	
		m_src.y = 0;
		m_src.h = 128;
		m_dst.y -= 32;
		m_dst.h = 64;

		m_sprite = m_spriteMin = 0;
		m_spriteMax = 7;
	}
	else if (m_state == rolling)
	{
		m_frame = 0;
		m_src.x = 0;
		m_src.y = 192;
		m_src.h = 64;
		m_dst.h = 32;
		m_sprite = 0;
		m_spriteMin = 0;
		m_spriteMax = 3;
		
	}
	
}

