#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "..\includes\injector\injector.hpp"
#include <cstdint>
#include "..\includes\IniReader.h"
#include <d3d9.h>

bool HDReflections, HDReflectionBlur, FrontEndReflectionBlur, ForceEnableMirror, ExtendRenderDistance;
static int ResolutionX, ResolutionY, ImproveReflectionLOD, RestoreSkybox;
int ResX, ResY;
static float RoadScale, VehicleScale, MirrorScale;
static float SkyboxOrientation = -8000;
static float VehichleSkyboxRenderDist = 0.00625;
static float RVMSkyboxRenderDist = 0.0185;

DWORD VehicleLODCodeCaveExit = 0x63166D;
DWORD FEVehicleLODCodeCaveExit = 0x6311EC;
DWORD RoadReflectionLODCodeCaveExit = 0x5D665F;
DWORD RoadReflectionLODCodeCavePart2Exit = 0x5D6720;
DWORD RoadReflectionLODCodeCavePart2Jump = 0x5D672E;
DWORD RestoreFEReflectionCodeCaveExit = 0x5BA513;
DWORD RestoreLightTrailsCodeCaveExit = 0x6316F5;
DWORD RestoreLightTrailsCodeCaveJump = 0x63198C;
DWORD RestoreMirrorSkyboxCodeCaveExit = 0x5CAE68;
DWORD RestoreVehicleSkyboxCodeCaveExit = 0x5CAD84;
DWORD RestoreRoadSkyboxCodeCaveExit = 0x5CAB60;
DWORD FlipRoadSkyboxCodeCaveExit = 0x60F95D;
DWORD sub_60F880 = 0x60F880;
DWORD sub_5CF420 = 0x5CF420;
DWORD ExtendVehicleRenderDistanceCodeCaveExit = 0x5C4FB5;
DWORD AnimatedMirrorMaskFixCodeCaveExit = 0x5B7D85;
DWORD SkyboxRenderDistanceCodeCaveExit = 0x60F9DE;

void __declspec(naked) RestoreFEReflectionCodeCave()
{
	__asm {
		mov edx, 0x80
		jmp RestoreFEReflectionCodeCaveExit
	}
}

void __declspec(naked) VehicleLODCodeCave()
{
	__asm {
		mov ecx, 0x0 // Road Reflection (Vehicle) LOD setting
		mov edx, 0x0 // Road Reflection (Wheels) LOD setting
		jmp VehicleLODCodeCaveExit
	}
}

void __declspec(naked) FEVehicleLODCodeCave()
{
	__asm {
		mov eax, 0x0 // FE Road Reflection (Vehicle) LOD setting
		mov ecx, 0x0 // FE Road Reflection (Wheels) LOD setting
		jmp FEVehicleLODCodeCaveExit
	}
}

void __declspec(naked) RoadReflectionLODCodeCave()
{
	__asm {
		cmp dword ptr ds : [edi + 0x3C], 0x40E96E29 // freeway sign
		je RoadReflectionLODCodeCavePart2
		cmp dword ptr ds : [edi + 0x3C], 0x45976DD4 // hills
		je RoadReflectionLODCodeCavePart2
		cmp dword ptr ds : [edi + 0x3C], 0x42212D15 // white box
		je RoadReflectionLODCodeCavePart2
		jmp RoadReflectionLODCodeCaveExit

	RoadReflectionLODCodeCavePart2:
		test byte ptr ds : [esi + 0x1B], 0x04
		je RoadReflectionLODCodeCavePart2Condtional
		jmp RoadReflectionLODCodeCavePart2Exit

	RoadReflectionLODCodeCavePart2Condtional:
		jmp RoadReflectionLODCodeCavePart2Jump
	}
}

void __declspec(naked) RestoreLightTrailsCodeCave()
{
	__asm {
		push ebp
		mov ebp, esp
		and esp, 0xFFFFFFF6
		sub esp, 0x00000314
		mov eax, dword ptr ds : [0x008026C8]
		test eax, eax
		push ebx
		push esi
		push edi
		je RestoreLightTrailsCodeCavePart2
		mov edi, dword ptr ds : [ebp + 0x08]
		push edi
		jmp RestoreLightTrailsCodeCaveExit

	RestoreLightTrailsCodeCavePart2:
		jmp RestoreLightTrailsCodeCaveJump
	}
}

void __declspec(naked) RestoreMirrorSkyboxCodeCave()
{
	__asm {
		push esi
		call sub_60F880 // Skybox function call
		add esp, 0x04
		call sub_5CF420
		push 0x01
		push esi
		lea ecx, dword ptr ds : [esp + 0x78]
		jmp RestoreMirrorSkyboxCodeCaveExit
	}
}

void __declspec(naked) RestoreVehicleSkyboxCodeCave()
{
	__asm {
		push esi
		call sub_60F880 // Skybox function call
		add esp, 0x04
		call sub_5CF420
		push ebx
		push esi
		lea ecx, dword ptr ds : [esp + 0x78]
		jmp RestoreVehicleSkyboxCodeCaveExit
	}
}

void __declspec(naked) RestoreRoadSkyboxCodeCave()
{
	__asm {
		push esi
		call sub_60F880 // Skybox function call
		add esp, 0x04
		call sub_5CF420
		push 0x08
		push esi
		lea ecx, dword ptr ds : [esp + 0x78]
		jmp RestoreRoadSkyboxCodeCaveExit
	}
}

void __declspec(naked) FlipRoadSkyboxCodeCave()
{
	__asm {
		fld dword ptr ds : [eax + 0x08]
		cmp edx, 0x04
		jne FlipRoadSkyboxCodeCavePart2
		fadd dword ptr ds : [SkyboxOrientation]
	FlipRoadSkyboxCodeCavePart2:
		mov eax, dword ptr ds : [0x82DA5C]
		jmp FlipRoadSkyboxCodeCaveExit
	}
}

void __declspec(naked) ExtendVehicleRenderDistanceCodeCave()
{
	__asm {
		mov edx, 0x461C4000
		jmp ExtendVehicleRenderDistanceCodeCaveExit
	}
}

void __declspec(naked) AnimatedMirrorMaskFixCodeCave()
{
	__asm {
		cmp dword ptr ds : [ecx + 0x0C], 0x5F4E5254 // "TRN_"
		jne AnimatedMirrorMaskFixCodeCavePart2
		cmp dword ptr ds : [ecx + 0x10], 0x45544157 // "WATE"
		jne AnimatedMirrorMaskFixCodeCavePart2
		cmp dword ptr ds : [ecx + 0x14], 0x4C414652 // "RFAL"
		jne AnimatedMirrorMaskFixCodeCavePart2
		cmp byte ptr ds : [ecx + 0x54], 0x01 // Checks if the address has the correct value
		jne AnimatedMirrorMaskFixCodeCavePart2
		mov byte ptr ds : [ecx + 0x54], 0x00 // Disables mirror mask animation

	AnimatedMirrorMaskFixCodeCavePart2 :
		push ecx
		movsx eax, byte ptr ds : [ecx + 0x58]
		jmp AnimatedMirrorMaskFixCodeCaveExit
	}
}

void __declspec(naked) SkyboxRenderDistanceCodeCave()
{
	__asm {
		cmp byte ptr ds : [ExtendRenderDistance], 0x00
		jg SkyboxRenderDistanceCodeCavePart3 // jumps if extend render distance is enabled
		cmp edx,0x03
		je SkyboxRenderDistanceCodeCavePart2 // jumps if rearview mirror
		cmp edx, 0x01
		je SkyboxRenderDistanceCodeCavePart3 // jumps if main render
		cmp edx, 0x04
		je SkyboxRenderDistanceCodeCavePart3 // jumps if road reflection

		fld dword ptr ds : [VehichleSkyboxRenderDist] // render distance: 0.00625
		jmp SkyboxRenderDistanceCodeCaveExit
		
	SkyboxRenderDistanceCodeCavePart2:
		fld dword ptr ds : [RVMSkyboxRenderDist] // render distance: 0.01825
		jmp SkyboxRenderDistanceCodeCaveExit

	SkyboxRenderDistanceCodeCavePart3:
		fld dword ptr ds : [0x7A6768] // default render distance: 1.0
		jmp SkyboxRenderDistanceCodeCaveExit
	}
}


void Init()
{
	// Read values from .ini
	CIniReader iniReader("NFSU2HDReflections.ini");

	// Resolution
	ResX = iniReader.ReadInteger("RESOLUTION", "ResolutionX", 0);
	ResY = iniReader.ReadInteger("RESOLUTION", "ResolutionY", 0);
	RoadScale = iniReader.ReadFloat("RESOLUTION", "RoadScale", 1.0);
	VehicleScale = iniReader.ReadFloat("RESOLUTION", "VehicleScale", 1.0);
	MirrorScale = iniReader.ReadFloat("RESOLUTION", "MirrorScale", 1.0);

	// General
	HDReflections = iniReader.ReadInteger("GENERAL", "HDReflections", 1);
	ImproveReflectionLOD = iniReader.ReadInteger("GENERAL", "ImproveReflectionLOD", 2);
	HDReflectionBlur = iniReader.ReadInteger("GENERAL", "HDReflectionBlur", 1);
	FrontEndReflectionBlur = iniReader.ReadInteger("GENERAL", "FrontEndReflectionBlur", 1);
	ForceEnableMirror = iniReader.ReadInteger("GENERAL", "ForceEnableMirror", 1);
	RestoreSkybox = iniReader.ReadInteger("GENERAL", "RestoreSkybox", 1);
	ExtendRenderDistance = iniReader.ReadInteger("GENERAL", "ExtendRenderDistance", 0);

	if (ResX <= 0 || ResY <= 0)
	{
		ResX = ::GetSystemMetrics(SM_CXSCREEN);
		ResY = ::GetSystemMetrics(SM_CYSCREEN);
	}

	if (HDReflections)
	{
		// Jumps
		injector::MakeJMP(0x5BA50D, RestoreFEReflectionCodeCave, true);
		// Road Reflection X
		injector::WriteMemory<uint32_t>(0x5BA08F, ResX * RoadScale, true);
		injector::WriteMemory<uint32_t>(0x5BA0D1, ResX * RoadScale, true);
		injector::WriteMemory<uint32_t>(0x5C2366, ResX * RoadScale, true);
		// Road Reflection Y
		injector::WriteMemory<uint32_t>(0x5BA08A, ResY * RoadScale, true);
		injector::WriteMemory<uint32_t>(0x5BA0CC, ResY * RoadScale, true);
		injector::WriteMemory<uint32_t>(0x5C236D, ResY * RoadScale, true);
		// Vehicle Reflection
		injector::WriteMemory<uint32_t>(0x7FEE6C, ResY * VehicleScale, true);
		// RVM Reflection
		// Aspect ratio without RVM_MASK is 3:1
		injector::WriteMemory<uint32_t>(0x7FEE80, ResY * MirrorScale, true);
		injector::WriteMemory<uint32_t>(0x7FEE84, (ResY / 3) * MirrorScale, true);
	}

	if (ImproveReflectionLOD >= 1)
	{	
		// Fixes moving RVM mask
		injector::MakeJMP(0x5B7D80, AnimatedMirrorMaskFixCodeCave, true);
		// Road Reflection LOD
		injector::MakeJMP(0x631665, VehicleLODCodeCave, true);
		injector::MakeJMP(0x6311E4, FEVehicleLODCodeCave, true);
		injector::WriteMemory<uint8_t>(0x4888EB, 0xEB, true);
		// Vehicle Reflection LOD
		injector::WriteMemory<uint32_t>(0x5B9C96, 0x00000000, true);
		// RVM LOD
		injector::WriteMemory<uint32_t>(0x5B9CE1, 0x00000000, true);

		if (ImproveReflectionLOD >= 2)
		// Full LOD Improvement
		injector::WriteMemory<uint8_t>(0x4888F3, 0xEB, true);
		if (ImproveReflectionLOD >= 2)
		injector::MakeJMP(0x5D671A, RoadReflectionLODCodeCave, true);
	}

	if (HDReflectionBlur)
	{
		// Reflection Blur X
		injector::WriteMemory<uint32_t>(0x5BA105, ResolutionX * RoadScale, true);
		// Reflection Blur Y
		injector::WriteMemory<uint32_t>(0x5BA100, ResolutionY * RoadScale, true);
	}

	if (ForceEnableMirror)
	{
		// Enables mirror for all camera views
		injector::MakeNOP(0x5C2230, 2, true);
		injector::WriteMemory<uint8_t>(0x6004A0, 0xEB, true);

		// Restores light trails
		injector::MakeCALL(0x5CAE6E, RestoreLightTrailsCodeCave, true);
	}

	if (FrontEndReflectionBlur)
	{
		// Enables gaussian blur in the front end
		injector::MakeNOP(0x5CAC3A, 2, true);
	}

	if (RestoreSkybox)
	{
		// Restores skybox for RVM
		injector::MakeJMP(0x5CAE61, RestoreMirrorSkyboxCodeCave, true);
		// Restores skybox for vehicle reflection
		injector::MakeJMP(0x5CAD7E, RestoreVehicleSkyboxCodeCave, true);
		// Restores skybox for road reflection
		injector::MakeJMP(0x5CAB59, RestoreRoadSkyboxCodeCave, true);
		// Flips skybox so it's visible in road reflection
		injector::MakeJMP(0x60F955, FlipRoadSkyboxCodeCave, true);
		// Enables skybox
		injector::WriteMemory<uint8_t>(0x60F9F6, 0xEB, true);
		injector::MakeJMP(0x60F9D6, SkyboxRenderDistanceCodeCave, true);
	}

	if (ExtendRenderDistance)
	{
		// Extends vehicle reflection distance so skybox is visible
		injector::MakeJMP(0x5C4FAE, ExtendVehicleRenderDistanceCodeCave, true);
		// Extends render RVM distance so skybox is visible
		injector::WriteMemory<uint32_t>(0x7870D8, 0x461C4000, true);
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