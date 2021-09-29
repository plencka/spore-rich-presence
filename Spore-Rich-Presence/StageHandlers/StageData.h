#pragma once
#include <Spore\BasicIncludes.h>

namespace SporePresence {
	enum StageMessageID : uint32_t {
		// Send the Prop Activity FileID to the Main Discord Presence Manager.
		kDiscordUpdateActivity = id("kDiscordUpdateActivity"),
		// Ask GameMode listeners for Prop Activity FileID.
		kDiscordRequestStageActivity = id("kDiscordRequestStageActivity"),
	};

	enum KnownModeID : uint32_t {
		kStageCell = 0x01654c00,
		kStageCreature = 0x01654c01,
		kStageTribe = 0x01654c02,
		kStageCiv = 0x01654c04,
		kStageSpace = 0x01654c05,
		kAdventure = 0x01654c10,
		kMainMenu = 0x02ccd1d2, // Galaxy Screen
		kCinematic = 0x8916f92d, // ? Only pre-cell stage cutscene ?
		kEditor = 0x00dbdba1, // Any editor, excluding Adventure
		kEffectEditor = 0xc6f8bc7c, // ModDevKit Effect Editor
		kAnimationEditor = 0xa5491440, // ModDevKit Animation Editor
	};

	struct StageMessageData {
		uint32_t stageID;
		ResourceID activityPropFile;

		StageMessageData(uint32_t modeID, ResourceID resourceID) :
			stageID(modeID),
			activityPropFile(resourceID)
		{};
	};

	class StageHandler
		: public App::DefaultMessageListener
	{
	protected:
		uint32_t modeID;
		uint32_t lastUsedID = id("sys_await_correct_mode");
		ResourceID defaultResourceID;

		void SetUpCurrentState(uint32_t subModeID) {
			if (lastUsedID == subModeID) {
				// No update required
				return;
			}
			lastUsedID = subModeID;

			SporePresence::StageMessageData messageData(modeID, defaultResourceID);

			if (PropManager.HasPropertyList(subModeID, messageData.activityPropFile.groupID)) {
				messageData.activityPropFile.instanceID = subModeID;
			};

			MessageManager.PostMSG(StageMessageID::kDiscordUpdateActivity, &messageData);
		}

		bool HandleMessage(uint32_t messageID, void* message) override {
			if (messageID != StageMessageID::kDiscordRequestStageActivity) {
				// Ignore: Invalid message
				return false;
			}

			if (!GameModeManager.Get()) {
				// Ignore: No gamemode manager.
				return false;
			}

			if (GameModeManager.GetActiveModeID() != modeID) {
				// Ignore: Incorrect gamemode.
				lastUsedID = id("sys_await_correct_mode");
				return false;
			}
			
			return true;
		};
	};
}
