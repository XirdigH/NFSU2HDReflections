﻿#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "..\includes\injector\injector.hpp"
#include <cstdint>
#include "..\includes\IniReader.h"
#include <d3d9.h>

DWORD WINAPI Thing(LPVOID);

bool HDReflections, ForceEnableMirror;
static int ResolutionX, ResolutionY;
DWORD GameState;
HWND windowHandle;

DWORD ImproveReflectionLODCodeCaveExit = 0x63166D;
DWORD RestoreFEReflectionCodeCaveExit = 0x5BA513;

void __declspec(naked) RestoreFEReflectionCodeCave()
{
	__asm {
		mov edx, 0x80
		jmp RestoreFEReflectionCodeCaveExit
	}
}

void __declspec(naked) ImproveReflectionLODCodeCave()
{
	__asm {
		mov ecx, 0x0 // LOD setting
		mov edx, 0x0 // LOD setting
		jmp ImproveReflectionLODCodeCaveExit
	}
}

void Init()
{
	// Read values from .ini
	CIniReader iniReader("NFSU2HDReflections.ini");

	// Resolution
	ResolutionX = iniReader.ReadInteger("RESOLUTION", "ResolutionX", 1920);
	ResolutionY = iniReader.ReadInteger("RESOLUTION", "ResolutionY", 1080);

	// General
	HDReflections = iniReader.ReadInteger("GENERAL", "HDReflections", 1);
	ForceEnableMirror = iniReader.ReadInteger("GENERAL", "ForceEnableMirror", 1);


	if (HDReflections)
	{
		// Jumps
		injector::MakeJMP(0x631665, ImproveReflectionLODCodeCave, true);
		injector::MakeJMP(0x5BA50D, RestoreFEReflectionCodeCave, true);
		// Reflection LOD
		injector::WriteMemory<uint8_t>(0x4888EB, 0xEB, true);
		// Road Reflection X
		injector::WriteMemory<uint32_t>(0x5BA08F, ResolutionX, true);
		injector::WriteMemory<uint32_t>(0x5BA0D1, ResolutionX, true);
		injector::WriteMemory<uint32_t>(0x5C2366, ResolutionX, true);
		// Road Reflection Y
		injector::WriteMemory<uint32_t>(0x5BA08A, ResolutionY, true);
		injector::WriteMemory<uint32_t>(0x5BA0CC, ResolutionY, true);
		injector::WriteMemory<uint32_t>(0x5C236D, ResolutionY, true);
		// Vehicle Reflection
		injector::WriteMemory<uint32_t>(0x7FEE6C, ResolutionY, true);
		// RVM Reflection X
		injector::WriteMemory<uint32_t>(0x7FEE80, ResolutionY, true);
		// RVM Reflection Y
		injector::WriteMemory<uint32_t>(0x7FEE84, ResolutionY, true);

	}

	if (ForceEnableMirror)
	{
		// Enables mirror for all camera views
		injector::MakeNOP(0x5C2230, true);
		injector::MakeNOP(0x5C2231, true);
		injector::WriteMemory<uint8_t>(0x6004A0, 0xEB, true);
	}
}


BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);
		IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)(base);
		IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);

		if ((base + nt->OptionalHeader.AddressOfEntryPoint + (0x400000 - base)) == 0x75BCC7) // Check if .exe file is compatible - Thanks to thelink2012 and MWisBest
			Init();

		else
		{
			MessageBoxA(NULL, "This .exe is not supported.\nPlease use v1.2 NTSC speed2.exe (4,57 MB (4.800.512 bytes)).", "NFSU2 HD Reflections", MB_ICONERROR);
			return FALSE;
		}
	}
	return TRUE;
	
}