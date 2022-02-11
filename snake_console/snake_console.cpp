

#include <iostream>
#include <list>
#include <thread>
#include <windows.h>

int nScreenWidth = 120;
int nScreenHeight = 30;
using namespace std;

struct sSnakeSegment
{
	int x;
	int y;
};

int main()
{
	// Create screen buffer
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	while (true)
	{
		//Inititilization
		std::list<sSnakeSegment> snake = { {60,15},{61,15},{62,15},{63,15},{64,15},{65,15},{66,15},{67,15},{68,15},{69,15} };
		int nFoodX = 30;
		int nFoodY = 15;
		int nScore = 0;
		int nSnakeDirection = 3;
		bool bDead = false;
		bool bKeyLeft = false, bKeyRight = false, bKeyLeftOld = false, bKeyRightOld = false, bKeyLevelEasy = false, bKeyLevelHard = false;
		std::chrono::microseconds nSnakeSpeed = 100ms;


		//Clear screen
		for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';

		//Wait for player to choose speed
		wsprintf(&screen[15 * nScreenWidth + 40], L"   How fast do you want to play?     ");
		wsprintf(&screen[17 * nScreenWidth + 40], L"   NORMAL: PRESS 'A'     ");
		wsprintf(&screen[19 * nScreenWidth + 40], L"   FAST: PRESS 'B'     ");

		//Draw screen
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

		//Wait for user input of speed (A or B)
		while (!(0x8000 & GetAsyncKeyState('A') | (0x8000 & GetAsyncKeyState('B'))));

		bKeyLevelEasy = GetAsyncKeyState('A');
		bKeyLevelHard = GetAsyncKeyState('B');

		if (bKeyLevelEasy) nSnakeSpeed = 200ms;
		if (bKeyLevelHard) nSnakeSpeed = 80ms;

		//Main game loop
		while (!bDead)
		{	
			
			auto time = chrono::system_clock::now();
			while ((chrono::system_clock::now() - time) < nSnakeSpeed)
			{

				bKeyRight = GetAsyncKeyState(VK_RIGHT) != 0;
				bKeyLeft = GetAsyncKeyState(VK_LEFT) != 0;

				if (bKeyRight && !bKeyRightOld)
				{
					nSnakeDirection++;
					if (nSnakeDirection == 4) nSnakeDirection = 0;
				}

				if (bKeyLeft && !bKeyLeftOld)
				{
					nSnakeDirection--;
					if (nSnakeDirection == -1) nSnakeDirection = 3;
				}

				bKeyRightOld = bKeyRight;
				bKeyLeftOld = bKeyLeft;
			}

			// Game logic
			sSnakeSegment newSeg;

			switch (nSnakeDirection)
			{
			case 0: // UP
				newSeg.x = snake.front().x;
				newSeg.y = snake.front().y - 1;
				break;
			
			case 1: // RIGHT
				newSeg.x = snake.front().x + 1;
				newSeg.y = snake.front().y;
				break;

			case 2: // DOWN
				newSeg.x = snake.front().x;
				newSeg.y = snake.front().y + 1;
				break;

			case 3: // LEFT
				newSeg.x = snake.front().x - 1;
				newSeg.y = snake.front().y;
				break;
			}
			snake.push_front(newSeg);


			// Collision detection

			// Collision detection snake vs. food

			if (snake.front().x == nFoodX && snake.front().y == nFoodY)
			{
				++nScore;
				while (screen[nFoodY * nScreenWidth + nFoodX] != L' ')
				{
					nFoodX = rand() % nScreenWidth;
					nFoodY = (rand() % (nScreenHeight - 3)) + 3;
				}

				for (int i = 0; i < 5; ++i)
					snake.push_back({ snake.back().x, snake.back().y });
			}


			// Collision detection snake vs. boarders

			if (snake.front().x < 0 || snake.front().x >= nScreenWidth)
				bDead = true;

			if (snake.front().y < 0 || snake.front().y >= nScreenHeight)
				bDead = true;

			//Collision detection snake vs itself
			for (auto i = snake.begin(); i != snake.end(); ++i)
			{
				if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y)
					bDead = true;
			}



			//Chop off snakes tail
			snake.pop_back();


			// Presentation

			// clear screen
			for (int i = 0; i < nScreenHeight * nScreenWidth; ++i) screen[i] = L' ';

			// Draw header & boarder 
			for (int i = 0; i < nScreenWidth; ++i)
			{
				screen[i] = L'=';
				screen[2 * nScreenWidth + i] = L'=';
			}
			wsprintf(&screen[nScreenWidth + 10], L"SPEED: %s", (nSnakeSpeed == 200ms) ? L"Normal" : L"Fast");
			wsprintf(&screen[nScreenWidth + 70], L"SCORE: %d", nScore);

			//Draw snake body 
			for (auto s : snake)
			{
				screen[s.y * nScreenWidth + s.x] = bDead ? L'+' : L'O';
			}

			// Draw snake head

			screen[snake.front().y * nScreenWidth + snake.front().x] = bDead ? L'X' : L'@';

			// Draw food

			screen[nFoodY * nScreenWidth + nFoodX] = L'%';

			if (bDead)
				wsprintf(&screen[15 * nScreenWidth + 40], L"    PRESS 'SPACE' TO PLAY AGAIN    ");


			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

		}

		// wait for space
		while (!GetAsyncKeyState(VK_SPACE));

	}

	return 0;
}

