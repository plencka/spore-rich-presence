// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "DiscordPresenceManager.h"
#include <StageHandlers/ModeHandlers.h>

void Initialize()
{
	SporeDebugPrint("Discord Rich Presence: DEBUG RELEASE");
	SporePresence::DiscordPresenceManager::SporeInit();
	SporePresence::InitializeListeners();
	SporePresence::DiscordPresenceManager::Initialize();
}

void Dispose()
{
	SporePresence::DiscordPresenceManager::Dispose();
}

void AttachDetours()
{
}


// Generally, you don't need to touch any code here
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ModAPI::AddPostInitFunction(Initialize);
		ModAPI::AddDisposeFunction(Dispose);

		PrepareDetours(hModule);
		AttachDetours();
		CommitDetours();
		break;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
