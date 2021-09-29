#include "stdafx.h"
#include "ModeHandlers.h"
#include <Spore/App/ScenarioMode.h>

using namespace SporePresence::Handlers;

bool SpaceHandler::HandleMessage(uint32_t messageID, void* message)
{
	if (!StageHandler::HandleMessage(messageID, message)) {
		return false;
	}

	if (Simulator::cEmpire* playerEmpire = Simulator::GetPlayerEmpire()) {
		SetUpCurrentState(playerEmpire->mArchetype);
		return true;
	}

	return false;
}

bool EditorHandler::HandleMessage(uint32_t messageID, void* message)
{
	if (!StageHandler::HandleMessage(messageID, message)) {
		return false;
	}

	if (Editors::GetEditor()) {
		SetUpCurrentState(Editor.mSaveExtension);
		return true;
	}

	return false;
}


bool AdventureHandler::HandleMessage(uint32_t messageID, void* message)
{
	if (!StageHandler::HandleMessage(messageID, message)) {
		return false;
	}

	if (App::ScenarioMode* ScenarioMode = App::ScenarioMode::Get()) {
		SetUpCurrentState((uint32_t)ScenarioMode->mMode);
		return true;
	}

	return false;
}
