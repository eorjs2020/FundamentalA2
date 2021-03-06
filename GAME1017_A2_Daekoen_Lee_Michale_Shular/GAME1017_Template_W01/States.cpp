#include "States.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "StateManager.h" // Make sure this is NOT in "States.h" or circular reference.
#include "TextureManager.h"
#include "Engine.h"
#include "tinyxml2.h"
#include <iostream>
#include "PatternManager.h"
#include <string>

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

	m_pQuitBtn = new ExitButton({ 0,0,490,140 }, { 380.0f,280.0f,240.0f,70.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("quit"));
	m_pResumeBtn = new ResumeButton({ 0,0,490,140 }, { 380.0f,420.0f,240.0f,70.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("resume"));
	m_pPauseBtn = new PauseButton({ 0,0,490,140 }, { 2.0f,2.0f,240.0f,70.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("pause"));

	std::cout << "Entering GameState..." << std::endl;
	m_plose = false;
	m_pPlayer = new PlatformPlayer({ 0,0,128,128 }, { 60.0f,200.0f,64.0f,64.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("player"), 0, 0, 7, 7);
	/*m_pTileText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Tiles.png");*/
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
	deadtimer = 0;
	m_pObs = new PatternManager();
}

void GameState::Update()
{
		
	if (!Engine::Instance().Pause())
	{
		if (m_pPauseBtn->Update() == 1)
			return;

		if (m_plose && deadtimer == 0)
		{
			m_pPlayer->SetState(2);
			
		}
		if (m_plose)
		{
			++deadtimer;
		}
		if (!m_plose)
		{
			if (EVMA::KeyHeld(SDL_SCANCODE_A) && m_pPlayer->GetDstP()->x > 0)
				m_pPlayer->SetAccelX(-1.0);
			else if (EVMA::KeyHeld(SDL_SCANCODE_D) && m_pPlayer->GetDstP()->x <= WIDTH - 64)
				m_pPlayer->SetAccelX(1.0);
			if (EVMA::KeyPressed(SDL_SCANCODE_SPACE) && m_pPlayer->IsGrounded())
			{
				SOMA::PlaySound("jump", 0, 1);
				m_pPlayer->SetAccelY(-JUMPFORCE); // Sets the jump force.
				m_pPlayer->SetGrounded(false);
			}

			//// Wrap the player on screen.
			if (m_pPlayer->GetDstP()->x < -51.0) m_pPlayer->SetX(1024.0);
			else if (m_pPlayer->GetDstP()->x > 1024.0) m_pPlayer->SetX(-50.0);
			// Do the rest. 
			m_updateTimer = m_defualtTimer + timer.getrunnningtime(timer);
			m_timer->SetText(m_updateTimer);
			

			for (int i = 0; i < 2; i++) {
				m_pBackgroundOne[i]->GetDstP()->x -= m_pSrollSpeed[0];
				if (m_pBackgroundOne[i]->GetDstP()->x == -1024) {
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
			m_pObs->Update();

		}
		m_pPlayer->Update(); // Change to player Update here.
		CheckCollision();
	}
	else
	{
		if (m_pQuitBtn->Update() == 1)
			return;
		if (m_pResumeBtn->Update() == 1)
			return;
	}
	
	if(deadtimer == 35)
		STMA::ChangeState(new LoseState);
	
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
	for (auto i = 0; i < m_pObs->GetObs().size(); i++)
	{
		if (!m_pObs->GetObs()[i]->Saw())
		{
			if (COMA::AABBCheck(*m_pPlayer->GetDstP(), *m_pObs->GetObs()[i]->GetDstP()))
			{	
				m_plose = true;
			}
		}
		else
		{
			SDL_FPoint temp = { m_pObs->GetObs()[i]->GetDstP()->x + m_pObs->GetObs()[i]->GetDstP()->w / 2, 
				m_pObs->GetObs()[i]->GetDstP()->y + m_pObs->GetObs()[i]->GetDstP()->h / 2 };
			if (COMA::CircleAABBCheck(temp,	m_pObs->GetObs()[i]->GetDstP()->w / 2, *m_pPlayer->GetDstP()))
			{
				m_plose = true;
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
	for (auto i = 0; i < m_pObs->GetObs().size(); ++i)
	{
		m_pObs->GetObs()[i]->Render();
	}
	for (int i = 0; i < 3; i++) 
		m_pPlatform[i]->Render();
	if (Engine::Instance().Pause())
	{
		m_pQuitBtn->Render();
		m_pResumeBtn->Render();
		
	}
	else
		m_pPauseBtn->Render();
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
	delete m_pObs;
	XMLDocument xmlDoc;
	xmlDoc.LoadFile("Score.xml");
	XMLElement* pRoot = xmlDoc.FirstChildElement();
	XMLElement* pElement = pRoot->FirstChildElement();
	int m, s;
	pElement->QueryIntAttribute("Minute", &m);
	pElement->QueryIntAttribute("Second", &s);
	if (m <= timer.m_minutes)
	{
		if (m == timer.m_minutes)
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
	pElement = pElement->NextSiblingElement();
	pElement->SetAttribute("Minute", timer.m_minutes);
	pElement->SetAttribute("Second", timer.m_seconds);
	
	xmlDoc.SaveFile("Score.xml");
}

void GameState::Resume() { }
// End GameState.

// Begin TitleState.
TitleState::TitleState() {}

void TitleState::Enter()
{
	m_playBtn = new PlayButton({ 0,0,400,100 }, { 312.0f,400.0f,400.0f,100.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("play"));
	m_pQuitButton = new ExitButton({ 0,0,400,100 }, { 312.0f,550.0f,400.0f,100.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("exit"));
	m_pBackground = new Sprite({ 0,0, 1024, 768 }, { 0.0,0,1024,768 },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("background"));
	SOMA::Load("Aud/power.wav", "beep", SOUND_SFX);
	m_pName = new Sprite({ 0,0, 1053, 136 }, { 312.0f,100.0f,500,60 },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("TitleName"));
}

void TitleState::Update()
{
	if (m_playBtn->Update() == 1)
		return;
	if (m_pQuitButton->Update() == 1)
		return;
}

void TitleState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 128, 0, 255, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	m_pBackground->Render();
	m_playBtn->Render();
	m_pQuitButton->Render();
	m_pName->Render();
	State::Render();
}

void TitleState::Exit()
{
	std::cout << "Exiting TitleState..." << std::endl;
	delete m_playBtn;
}
// End TitleState.

LoseState::LoseState() {}

void LoseState::Update()
{

	if (m_pMenu->Update() == 1)
		return;
	if (m_pQuitButton->Update() == 1)
		return;
}

void LoseState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 128, 0, 255, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	m_pBackground->Render();
	m_pMenu->Render();
	m_pQuitButton->Render();
	m_bestScore->Render();
	m_score->Render();
	
	State::Render();

}

void LoseState::Enter()
{
	
	XMLDocument xmlDoc;
	xmlDoc.LoadFile("Score.xml");
	XMLElement* pRoot = xmlDoc.FirstChildElement();
	XMLElement* pElement = pRoot->FirstChildElement();
	int m, s;
	pElement->QueryIntAttribute("Minute", &m);
	pElement->QueryIntAttribute("Second", &s);
	pElement = pElement->NextSiblingElement();
	int cm, cs;
	pElement->QueryIntAttribute("Minute", &cm);
	pElement->QueryIntAttribute("Second", &cs);
	xmlDoc.SaveFile("Score.xml");
	
	std::string bestM, bestS, currM, currS;
	bestM = std::to_string(m);
	bestS = std::to_string(s);
	currM = std::to_string(cm);
	currS = std::to_string(cs);

	std::string BestTime, CurrTime;

	BestTime = "BestTime : " + bestM + " : " + bestS;
	CurrTime = "CurrentTime : " + currM + " : " + currS;
	
	m_pMenu = new MenuButton({ 0,0,200,80 }, { 312.0f,100.0f,400.0f,100.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("menu"));
	m_pQuitButton = new ExitButton({ 0,0,400,100 }, { 312.0f,250.0f,400.0f,100.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("exit"));
	m_pBackground = new Sprite({ 0,0, 1024, 768 }, { 0.0,0,1024,768 },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("background"));
	
	m_score = new Label("Font", 312.0, 400, CurrTime, { 255,255,255,255 });
	m_bestScore = new Label("Font", 312.0, 450, BestTime, { 255,255,255,255 });

}

void LoseState::Exit()
{
}
