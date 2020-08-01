#include "States.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "StateManager.h" // Make sure this is NOT in "States.h" or circular reference.
#include "TextureManager.h"
#include "Engine.h"
#include "Button.h"
#include "tinyxml2.h"
#include <iostream>

using namespace tinyxml2;

// Begin State. CTRL+M+H and CTRL+M+U to turn on/off collapsed code.
void State::Render()
{
	
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}
void State::Resume() {}
// End State.

// Begin GameState.
GameState::GameState() {}

void GameState::Enter()
{
	
	std::cout << "Entering GameState..." << std::endl;
	m_pPlayer = new PlatformPlayer({ 0,0,128,128 }, { 60.0f,200.0f,64.0f,64.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("player"), 0, 0, 7, 7);
	m_pTileText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Tiles.png");
	SOMA::Load("Aud/jump.wav", "jump", SOUND_SFX);
	m_timer = new Label("Font", 850, 10, m_defualtTimer, { 255,255,255,255 });
	timer.start();
	for (int i = 0; i < 2; i++) {
		m_pBackgroundOne[i] = new Sprite({ 0,0, 1024, 768 }, { float(0.0 + 1024 * i),0,1024,768 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("background"));
	}
	for (int i = 0; i < 5; i++) {
		m_pBackgroundTwo[i] = new Sprite({ 1024,0 , 255, 510 }, { float(0.0 + 256.0* i),0,256,510, },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("background"));
	}
	for (int i = 0; i < 3; i++) {
		m_pPlatform[i] = new Sprite({ 1024, 511 , 512, 258 }, { float(0.0 + 512 * i), 510 ,512,258, },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("background"));
	}

	obs = new Obstacle();
}

void GameState::Update()
{
	// Player input.
	if (EVMA::KeyHeld(SDL_SCANCODE_A) && m_pPlayer->GetDstP()->x <= 30)
		m_pPlayer->SetAccelX(-1.0);
	else if (EVMA::KeyHeld(SDL_SCANCODE_D) && m_pPlayer->GetDstP()->x >= WIDTH - 400)
		m_pPlayer->SetAccelX(1.0);
	if (EVMA::KeyPressed(SDL_SCANCODE_SPACE) && m_pPlayer->IsGrounded())
	{
		SOMA::PlaySound("jump");
		m_pPlayer->SetAccelY(-JUMPFORCE); // Sets the jump force.
		m_pPlayer->SetGrounded(false);
	}
	//// Wrap the player on screen.
	if (m_pPlayer->GetDstP()->x < -51.0) m_pPlayer->SetX(1024.0);
	else if (m_pPlayer->GetDstP()->x > 1024.0) m_pPlayer->SetX(-50.0);
	// Do the rest. 
	m_updateTimer = m_defualtTimer + timer.getrunnningtime(timer);
	m_timer->SetText(m_updateTimer);
	m_pPlayer->Update(); // Change to player Update here.
	CheckCollision();
	for (int i = 0; i < 2; i++) {
		m_pBackgroundOne[i]->GetDstP()->x -= m_pSrollSpeed[0];
		if (m_pBackgroundOne[i]->GetDstP()->x == - 1024 ) {
			m_pBackgroundOne[i]->GetDstP()->x = 1024;
		}
	}
	for (int i = 0; i < 5; i++) {
		m_pBackgroundTwo[i]->GetDstP()->x -= m_pSrollSpeed[1];
		if (m_pBackgroundTwo[i]->GetDstP()->x == -256) {
			m_pBackgroundTwo[i]->GetDstP()->x = 1024;
		}
	}
	for (int i = 0; i < 3; i++) {
		m_pPlatform[i]->GetDstP()->x -= m_pSrollSpeed[2];
		if (m_pPlatform[i]->GetDstP()->x == -512) {
			m_pPlatform[i]->GetDstP()->x = 1024;
		}
	}
	obs->Update();
}

void GameState::UpdateTiles(float scroll, bool x)
{
	
}

void GameState::CheckCollision()
{
	for (unsigned i = 0; i < 3; i++) // For each platform.
	{
		if (COMA::AABBCheck(*m_pPlayer->GetDstP(), *m_pPlatform[i]->GetDstP()))
		{
			if (m_pPlayer->GetDstP()->y + m_pPlayer->GetDstP()->h - (float)m_pPlayer->GetVelY() <= m_pPlatform[i]->GetDstP()->y)
			{ // Colliding top side of platform.
				m_pPlayer->SetGrounded(true);
				m_pPlayer->StopY();
				m_pPlayer->SetY(m_pPlatform[i]->GetDstP()->y - m_pPlayer->GetDstP()->h);
			}
			else if (m_pPlayer->GetDstP()->y - (float)m_pPlayer->GetVelY() >= m_pPlatform[i]->GetDstP()->y + m_pPlatform[i]->GetDstP()->h)
			{ // Colliding bottom side of platform.
				m_pPlayer->StopY();
				m_pPlayer->SetY(m_pPlatform[i]->GetDstP()->y + m_pPlatform[i]->GetDstP()->h);
			}
			else if (m_pPlayer->GetDstP()->x + m_pPlayer->GetDstP()->w - m_pPlayer->GetVelX() <= m_pPlatform[i]->GetDstP()->x)
			{ // Collision from left.
				m_pPlayer->StopX(); // Stop the player from moving horizontally.
				m_pPlayer->SetX(m_pPlatform[i]->GetDstP()->x - m_pPlayer->GetDstP()->w);
			}
			else if (m_pPlayer->GetDstP()->x - (float)m_pPlayer->GetVelX() >= m_pPlatform[i]->GetDstP()->x + m_pPlatform[i]->GetDstP()->w)
			{ // Colliding right side of platform.
				m_pPlayer->StopX();
				m_pPlayer->SetX(m_pPlatform[i]->GetDstP()->x + m_pPlatform[i]->GetDstP()->w);
			}
		}
	}
}

void GameState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	// Draw the platforms.
	
	for (int i = 0; i < 2; i++)
		m_pBackgroundOne[i]->Render();
	for (int i = 0; i < 5; i++)
		m_pBackgroundTwo[i]->Render();
	for (int i = 0; i < 3; i++) 
		m_pPlatform[i]->Render();
	obs->Render();
	m_timer->Render();
	// Draw the player.
	m_pPlayer->Render();

	// If GameState != current state.
	if (dynamic_cast<GameState*>(STMA::GetStates().back()))
		State::Render();
	
}

void GameState::Exit()
{
	delete m_pPlayer;
	XMLDocument xmlDoc;
	xmlDoc.LoadFile("Score.xml");
	XMLElement* pRoot = xmlDoc.FirstChildElement();
	XMLElement* pElement = pRoot->FirstChildElement();
	int m, s;
	pElement->QueryIntAttribute("Minute", &m);
	pElement->QueryIntAttribute("Second", &s);
	if (m <= timer.m_minutes)
	{
		if (m = timer.m_minutes)
		{
			if (s < timer.m_seconds)
			{
				pElement->SetAttribute("Minute", timer.m_minutes);
				pElement->SetAttribute("Second", timer.m_seconds);
			}

		}
		else
		{
			pElement->SetAttribute("Minute", timer.m_minutes);
			pElement->SetAttribute("Second", timer.m_seconds);
		}
	}
	xmlDoc.SaveFile("Score.xml");
}

void GameState::Resume() { }
// End GameState.

// Begin TitleState.
TitleState::TitleState() {}

void TitleState::Enter()
{
	m_playBtn = new PlayButton({ 0,0,400,100 }, { 312.0f,100.0f,400.0f,100.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("play"));
	SOMA::Load("Aud/power.wav", "beep", SOUND_SFX);
}

void TitleState::Update()
{
	if (m_playBtn->Update() == 1)
		return;
}

void TitleState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 128, 0, 255, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	m_playBtn->Render();
	State::Render();
}

void TitleState::Exit()
{
	std::cout << "Exiting TitleState..." << std::endl;
	delete m_playBtn;
}
// End TitleState.
