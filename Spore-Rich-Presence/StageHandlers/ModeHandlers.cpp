#include "stdafx.h"
#include "ModeHandlers.h"

using namespace SporePresence::Handlers;

bool SpaceHandler::HandleMessage(uint32_t messageID, void* message)
{
	if (!StageHandler::HandleMessage(messageID, message)) {
		return false;
	}

	if (Simulator::cEmpire* playerEmpire = Simulator::GetPlayerEmpire()) {
		SporePresence::StageMessageData messageData{};

		messageData.stageID = SporePresence::KnownModeID::kStageSpace;
		messageData.activityPropFile = { id("Unknown"), id("RPC_SpaceArchetypes") };

		if (lastUsedID == playerEmpire->mArchetype) {
			// No update required
			return true;
		}

		if (PropManager.HasPropertyList(playerEmpire->mArchetype, messageData.activityPropFile.groupID)) {
			messageData.activityPropFile.instanceID = playerEmpire->mArchetype;
		};

		lastUsedID = playerEmpire->mArchetype;
		MessageManager.PostMSG(StageMessageID::kDiscordUpdateActivity, &messageData);
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
		SporePresence::StageMessageData messageData{};

		messageData.stageID = SporePresence::KnownModeID::kEditor;
		messageData.activityPropFile = { id("Unknown"), id("RPC_EditorTypes") };

		if (lastUsedID == Editor.mSaveExtension) {
			// No update required
			return true;
		}

		if (PropManager.HasPropertyList(Editor.mSaveExtension, messageData.activityPropFile.groupID)) {
			messageData.activityPropFile.instanceID = Editor.mSaveExtension;
		};

		lastUsedID = Editor.mSaveExtension;
		MessageManager.PostMSG(StageMessageID::kDiscordUpdateActivity, &messageData);
		return true;
	}

	return false;
}