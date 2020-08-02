#include "PatternManager.h"
#include <ctime>
#include "Engine.h"
#include "TextureManager.h"
#include "Util.h"
PatternManager::PatternManager()
{
	srand(unsigned int(time));
	timer = 0;
	destory = false;
}

void PatternManager::Update()
{
	

	if (timer == 250 || timer == 0)
	{
		int i;
		i = rand() % 3;
		timer = 1;
		Pattern(i);
		
	}
	++timer;
	for (auto i = 0; i < m_obs.size(); ++i)
		m_obs[i]->Update();
	Destory();

	
}

void PatternManager::Render()
{
	for (auto i = 0; i < m_obs.size(); ++i)
		m_obs[i]->Render();
}

void PatternManager::Pattern(int n)
{
	switch (n)
	{
	case 0:
		m_obs.push_back(new Obstacle({ 0,0, 128, 448 }, { 1024 ,0, 128, 460 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"), false));
		m_obs.push_back(new Obstacle({ 129,65, 127, 63 }, { 1424,447,128,63 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"), false));
		m_obs.push_back(new Obstacle({ 0,0, 128, 448 }, { 1824 ,0, 128, 460 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"), false));
		break;
	case 1:
		m_obs.push_back(new Obstacle({ 129, 129, 127, 127 }, { 1024 ,447, 127, 127 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"), true));
		m_obs.push_back(new Obstacle({ 129,65, 127, 63 }, { 1424,447,128,63 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"), false));
		m_obs.push_back(new Obstacle({ 0,0, 128, 448 }, { 1824 ,0, 128, 460 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"), false));
			
		break;
	case 2:
		m_obs.push_back(new Obstacle({ 129, 129, 127, 127 }, { 1024 ,447, 127, 127 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"), true));
		m_obs.push_back(new Obstacle({ 129,65, 127, 63 }, { 1424,447,128,63 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"), false));
		m_obs.push_back(new Obstacle({ 129,65, 127, 63 }, { 1424,447,128,63 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"), false));
		break;
	default:
		break;
	}
}

void PatternManager::Destory()
{
	for (auto i = 0; i < m_obs.size(); ++i)
	{
		if (m_obs[i]->GetDstP()->x < -50)
		{
			delete m_obs[i];
			m_obs[i] = nullptr;
			destory = true;
		}
	}
	if (destory) CleanVector<Obstacle*>(m_obs, destory);
}

std::vector<Obstacle*> PatternManager::GetObs()
{
    return m_obs;
}
