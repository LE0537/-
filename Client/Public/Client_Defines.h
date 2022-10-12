#pragma once

namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
	enum POKETYPE { NORMAL, FIRE, WATER, THUNDER, GRASS, ICE, FIGHT, POISON, TERRAIN, FLY, ESPER, BUG, ROCK, GHOST, DRAGON, EVIL, STEEL, FAIRY, END };

}

#include <process.h>

// Client_Defines.h

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;

using namespace Client;