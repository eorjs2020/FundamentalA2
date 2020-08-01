#include "Obstacle.h"
#include <ctime>
#include "TextureManager.h"
#include "Engine.h"

Obstacle::Obstacle()
{
	srand((unsigned)time(NULL));
	genNum = rand()% 3;
	std::cout << genNum;
	switch (genNum)
	{
	case patternone:
		obstaclePatternOne[0] = new Sprite({ 0,0, 128, 448 }, { 1024 ,0, 128, 448 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"));
		obstaclePatternOne[1] = new Sprite({ 129,65, 127, 63 }, { 1424,447,128,63 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"));
		obstaclePatternOne[2] = new Sprite({ 0,0, 128, 448 }, { 1824 ,0, 128, 448 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"));
		break;
	case patternTwo:
		obstaclePatternTwo[0] = new Sprite({ 129, 129, 127, 127 }, { 1024 ,447, 127, 127 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"));
		obstaclePatternTwo[1] = new Sprite({ 129,65, 127, 63 }, { 1424,447,128,63 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"));
		obstaclePatternTwo[2] = new Sprite({ 0,0, 128, 448 }, { 1824 ,0, 128, 448 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"));
		break;
	case patterthree:
		obstaclePatternThree[0] = new Sprite({ 129, 129, 127, 127 }, { 1024 ,447, 127, 127 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"));
		obstaclePatternThree[1] = new Sprite({ 129,65, 127, 63 }, { 1424,447,128,63 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"));
		obstaclePatternThree[2] = new Sprite({ 129,65, 127, 63 }, { 1424,447,128,63 },
			Engine::Instance().GetRenderer(), TEMA::GetTexture("Obs"));
		break;
	default:
		break;
	}
		

		


}

void Obstacle::Update()
{
	switch (genNum)
	{
	case patternone:
		for(int i = 0; i < 3; i++)
		obstaclePatternOne[i]->GetDstP()->x -= 2;
		break;
	case patternTwo:
		for (int i = 0; i < 3; i++)
		obstaclePatternTwo[i]->GetDstP()->x -= 2;
		break;
	case patterthree:	
		for (int i = 0; i < 3; i++)
		obstaclePatternThree[i]->GetDstP()->x -= 2;
		break;
	default:
		break;
	}
	



}

void Obstacle::Render()
{
	switch (genNum)
	{
	case patternone:
		for (int i = 0; i < 3; i++)
			obstaclePatternOne[i]->Render();

		break;
	case patternTwo:
		for (int i = 0; i < 3; i++)
			obstaclePatternTwo[i]->Render();
		break;
	case patterthree:
		for (int i = 0; i < 3; i++)
			obstaclePatternThree[i]->Render();
		break;
	default:
		break;
	}
}
	
