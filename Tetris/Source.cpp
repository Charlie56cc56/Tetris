#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <vector>
using std::vector;
using namespace std;


wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char* pField = nullptr;

int nScreenWidth = 120;
int nScreenHeight = 30;
int rotate(int px, int py, int r)

//Change one

{
	switch (r % 4)
	{
	   case 0: return py * 4 + px; //0 degrees 
	   case 1: return 12 + py - (px * 4); //90  degrees
	   case 2: return 15 - (py * 4) - px; //180 degrees
	   case 3: return 3 - py + (px * 4); //270 degrees
	}
	return 0;

}
bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{

	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
		{
			//Get Index into Piece
			int pi = rotate(px, py, nRotation);

			//Get Index into Field
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
				{
					if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
						return false; //fail on first hit.
				}
			}
		}
	
	return true;
}
int main()
{

	//create assets
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");


	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X.. ");
	tetromino[1].append(L".... ");



	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");



	tetromino[3].append(L"..XX.");
	tetromino[3].append(L"..XX.");
	tetromino[3].append(L"....");
	tetromino[3].append(L"....");


	tetromino[4].append(L".X..");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");


	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");


	tetromino[6].append(L".X.."); 
	tetromino[6].append(L".XX.");
	tetromino[6].append(L"..X.");
	tetromino[6].append(L"..X.");


	pField = new unsigned char[nFieldWidth * nFieldHeight]; //create playing field
	for (int x = 0; x < nFieldWidth; ++x)//boundary border
		for (int y = 0; y < nFieldHeight; ++y)
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	//Game Logic Stuff
	bool bGameOver = false;

	int nCurrentPiece = 1;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;
	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool BForceDown = false;
	bool bKey[4];
	bool bRotateHold = false;
	int nPieceCount = 0;
	int nScore = 0;

	vector<int> vLines;
	while (!bGameOver)
	{
		//GAME TIMING
		this_thread::sleep_for(50ms); //gametick
		nSpeedCounter++;
		BForceDown = (nSpeedCounter == nSpeed);

		//INPUT
		for (int k = 0; k < 4; k++)
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

		//GAME LOGIC
		nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
		nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
		nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;



		if (bKey[3])
		{
			nCurrentRotation += (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = true;
		}
		else
			bRotateHold = false;


		if (BForceDown)
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++;//it can do it!

			else
			{
				//Lock the current piece in the game
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] == L'X')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

				nPieceCount++;
				if (nPieceCount % 10 == 0)
					if (nSpeed >= 10) nSpeed--;
				//Check have we got any Lines
				for(int py = 0; py < 4; py++)
					if (nCurrentY + py < nFieldHeight - 1)
					{
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++)
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

						if (bLine)
						{
							//Remove Line set to =
							for (int px = 1; px < nFieldWidth - 1; px++)
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;

							vLines.push_back(nCurrentY + py);
						}
					}

				nScore += 25;
				if (!vLines.empty()) nScore += (1 << vLines.size()) * 100;

				//Choose next piece
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;
				//If piece doesn't fit
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1);

			}
			nSpeedCounter = 0;

		}

		//RENDER OUTPUT

		//Draw Field
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];

		//Draw Current Piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] == L'X')
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
		
		//Draw Score
		swprintf_s(&screen[2 * nScreenWidth + nFieldHeight + 6], 16, L"SCORE: %8d",  nScore);
		if (!vLines.empty())
		{
			//Display frame
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
			//Delay a bit
			this_thread::sleep_for(400ms);

			for(auto &v : vLines)
				for(int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
						pField[px] = 0;
				}

			vLines.clear();

		}


		//Display Frame
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
	}

	//Oh Dear
	CloseHandle(hConsole);
	cout << "Game Over! Score:" << nScore << endl;
	system("pause");
	return 0;
} 