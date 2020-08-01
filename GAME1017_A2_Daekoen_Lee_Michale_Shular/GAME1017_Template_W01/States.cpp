#include "States.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "StateManager.h" // Make sure this is NOT in "States.h" or circular reference.
#include "TextureManager.h"
#include "Engine.h"
#include "Button.h"
#include <iostream>
#include <fstream>

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
	m_pPlayer = new PlatformPlayer({ 0,0,0,0 }, { 512.0f,256.0f,32.0f,64.0f },
		Engine::Instance().GetRenderer(), nullptr);
	m_pTileText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Tiles.png");
	SOMA::Load("Aud/jump.wav", "jump", SOUND_SFX);
	std::ifstream inFile("Dat/Tiledata.txt");
	if (inFile.is_open())
	{ // Create map of Tile prototypes.
		char key;
		int x, y;
		bool o, h;
		while (!inFile.eof())
		{
			inFile >> key >> x >> y >> o >> h;
			m_tiles.emplace(key, new Tile({ x * 32, y * 32, 32, 32 }, { 0,0,32,32 }, Engine::Instance().GetRenderer(), m_pTileText, o, h));
		}
	}
	inFile.close();
	inFile.open("Dat/Level1.txt");
	if (inFile.is_open())
	{ // Build the level from Tile prototypes.
		char key;
		for (int row = 0; row < ROWS; row++)
		{
			for (int col = 0; col < COLS; col++)
			{
				inFile >> key;
				m_level[row][col] = m_tiles[key]->Clone(); // Prototype design pattern used.
				m_level[row][col]->GetDstP()->x = (float)(32 * col);
				m_level[row][col]->GetDstP()->y = (float)(32 * row);
				if (m_level[row][col]->IsObstacle())
					m_platforms.push_back(m_level[row][col]);
			}
		}
	}
	inFile.close();
}

void GameState::Update()
{
	// Player input.
	if (EVMA::KeyHeld(SDL_SCANCODE_A))
		m_pPlayer->SetAccelX(-1.0);
	else if (EVMA::KeyHeld(SDL_SCANCODE_D))
		m_pPlayer->SetAccelX(1.0);
	if (EVMA::KeyPressed(SDL_SCANCODE_SPACE) && m_pPlayer->IsGrounded())
	{
		SOMA::PlaySound("jump");
		m_pPlayer->SetAccelY(-JUMPFORCE); // Sets the jump force.
		m_pPlayer->SetGrounded(false);
	}
	//// Wrap the player on screen.
	//if (m_pPlayer->GetDstP()->x < -51.0) m_pPlayer->SetX(1024.0);
	//else if (m_pPlayer->GetDstP()->x > 1024.0) m_pPlayer->SetX(-50.0);
	// Do the rest. 
	m_bgScrollX = m_bgScrollY = false;
	if (m_pPlayer->GetVelX() > 0 && m_pPlayer->GetDstP()->x > WIDTH*0.7f)
	{
		if (m_level[0][COLS - 1]->GetDstP()->x > WIDTH - 32)
		{
			m_bgScrollX = true;
			UpdateTiles((float)m_pPlayer->GetVelX(), true);
		}
	}
	else if (m_pPlayer->GetVelX() < 0 && m_pPlayer->GetDstP()->x < WIDTH*0.3f)
	{
		if (m_level[0][0]->GetDstP()->x < 0)
		{
			m_bgScrollX = true;
			UpdateTiles((float)m_pPlayer->GetVelX(), true);
		}
	}
	if (m_pPlayer->GetVelY() > 0 && m_pPlayer->GetDstP()->y > HEIGHT*0.7f)
	{
		if (m_level[ROWS-1][0]->GetDstP()->y > HEIGHT - 32)
		{
			m_bgScrollY = true;
			UpdateTiles((float)m_pPlayer->GetVelY());
		}
	}
	else if (m_pPlayer->GetVelY() < 0 && m_pPlayer->GetDstP()->y < HEIGHT*0.3f)
	{
		if (m_level[0][0]->GetDstP()->y < 0)
		{
			m_bgScrollY = true;
			UpdateTiles((float)m_pPlayer->GetVelY());
		}
	}
	m_pPlayer->Update(m_bgScrollX, m_bgScrollY); // Change to player Update here.
	CheckCollision();
}

void GameState::UpdateTiles(float scroll, bool x)
{
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			if (x)
				m_level[row][col]->GetDstP()->x -= scroll;
			else
				m_level[row][col]->GetDstP()->y -= scroll;
		}
	}
}

void GameState::CheckCollision()
{
	for (unsigned i = 0; i < m_platforms.size(); i++) // For each platform.
	{
		if (COMA::AABBCheck(*m_pPlayer->GetDstP(), *m_platforms[i]->GetDstP()))
		{
			if (m_pPlayer->GetDstP()->y + m_pPlayer->GetDstP()->h - (float)m_pPlayer->GetVelY() <= m_platforms[i]->GetDstP()->y)
			{ // Colliding top side of platform.
				m_pPlayer->SetGrounded(true);
				m_pPlayer->StopY();
				m_pPlayer->SetY(m_platforms[i]->GetDstP()->y - m_pPlayer->GetDstP()->h);
			}
			else if (m_pPlayer->GetDstP()->y - (float)m_pPlayer->GetVelY() >= m_platforms[i]->GetDstP()->y + m_platforms[i]->GetDstP()->h)
			{ // Colliding bottom side of platform.
				m_pPlayer->StopY();
				m_pPlayer->SetY(m_platforms[i]->GetDstP()->y + m_platforms[i]->GetDstP()->h);
			}
			else if (m_pPlayer->GetDstP()->x + m_pPlayer->GetDstP()->w - m_pPlayer->GetVelX() <= m_platforms[i]->GetDstP()->x)
			{ // Collision from left.
				m_pPlayer->StopX(); // Stop the player from moving horizontally.
				m_pPlayer->SetX(m_platforms[i]->GetDstP()->x - m_pPlayer->GetDstP()->w);
			}
			else if (m_pPlayer->GetDstP()->x - (float)m_pPlayer->GetVelX() >= m_platforms[i]->GetDstP()->x + m_platforms[i]->GetDstP()->w)
			{ // Colliding right side of platform.
				m_pPlayer->StopX();
				m_pPlayer->SetX(m_platforms[i]->GetDstP()->x + m_platforms[i]->GetDstP()->w);
			}
		}
	}
}

void GameState::Render()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());
	// Draw the platforms.
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			m_level[row][col]->Render();
		}
	}
	// Draw the player.
	m_pPlayer->Render();
	// If GameState != current state.
	if (dynamic_cast<GameState*>(STMA::GetStates().back()))
		State::Render();
}

void GameState::Exit()
{
	delete m_pPlayer;
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			delete m_level[row][col];
			m_level[row][col] = nullptr; // Wrangle your dangle.
		}
	}
	for (auto const& i : m_tiles)
		delete m_tiles[i.first];
	m_tiles.clear();
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
