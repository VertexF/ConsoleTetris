#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <thread>
#include <vector>

using namespace std;

wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char *pField = NULL;

int nScreenWidth = 120;
int nScreenHeight = 30;

int rotate(int px, int py, int r)
{
	switch (r % 4)
	{
	case 0: return py * 4 + px; // 0 degrees
	case 1: return 12 + py - (px * 4); // 90 degrees
	case 2: return 15 - (py * 4) - px; // 180 degrees
	case 3: return 3 - py + (px * 4); // 270 degrees
	};

	return 0;
}

bool doesItFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	for (int px = 0; px < 4; px++) 
		for (int py = 0; py < 4; py++) 
		{
			int pi = rotate(px, py, nRotation);

			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight) 
				{
					if (tetromino[nTetromino][pi] != L'.' && pField[fi] != 0)
						return false;
				}
			}
		}

	return true;
}

int main()
{
	//Construct the assess
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");


	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"....");
	tetromino[6].append(L"..XX");
	tetromino[6].append(L"..X.");
	tetromino[6].append(L"..X.");

	pField = new unsigned char[nFieldWidth * nFieldHeight];
	for (int x = 0; x < nFieldWidth; x++)
		for (int y = 0; y < nFieldHeight; y++)
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
		
	

	wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	bool bgameOver = false;
	srand(time(0));
	int nCurrentPiece = rand() % 7;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;

	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;

	bool bKey[4];
	bool bRotateHold = false;
	int nPieceCount = 0;
	int nScore = 0;
	vector<int> vLines;
	int nLevel = 1;
	int nLineCount = 0;

	while (!bgameOver)
	{
		this_thread::sleep_for(50ms);
		nSpeedCounter++;
		bForceDown = (nSpeedCounter == nSpeed);

		for (int k = 0; k < 4; k++)								// R   L   D Z
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

		nCurrentX += (bKey[0] && doesItFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
		nCurrentX -= (bKey[1] && doesItFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
		nCurrentY += (bKey[2] && doesItFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;

		if (bKey[3])
		{
			nCurrentRotation += (!bRotateHold && doesItFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = true;
		}
		else
			bRotateHold = false;

		if (bForceDown) 
		{
			if (doesItFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++;
			else
			{
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] != L'.')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

				nPieceCount++;

				for (int py = 0; py < 4; py++)
					if (nCurrentY + py < nFieldHeight - 1) 
					{
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++)
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

						if (bLine) 
						{
							for(int px = 1; px < nFieldWidth - 1; px++)
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;

							vLines.push_back(nCurrentY + py);
						}
					}

				nScore += 25 * nLevel;
				if (!vLines.empty())
				{
					nScore += (1 << vLines.size()) * nLevel * 100;
					nLineCount += vLines.size();
					if (nLineCount % 10 == 0)
					{
						nLevel++;
						if (nSpeed >= 10) 
							nSpeed--;
					}
				}

				nCurrentPiece = rand() % 7;
				nCurrentRotation = 0;
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;

				bgameOver = !doesItFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY);
			}

			nSpeedCounter = 0;
		}

		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];

		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] != L'.')
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;

		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);
		swprintf_s(&screen[3 * nScreenWidth + nFieldWidth + 6], 16, L"LEVEL: %8d", nLevel);

		if (!vLines.empty())
		{
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms);

			for (auto &v : vLines) 
				for (int px = 1; px < nFieldWidth - 1; px++) 
				{
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					pField[px] = 0;
				}
			vLines.clear();
		}

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
	}

	CloseHandle(hConsole);
	cout << "==================================================" << endl;
	cout << "Game Over! Thank you for playing! Your score was: " << nScore << endl;
	cout << "==================================================" << endl;
	cin.get();

	return 0;
}