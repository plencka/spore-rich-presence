#pragma once
#include <Spore\BasicIncludes.h>

namespace SporePresence {
	enum StageMessageID : uint32_t {
		kDiscordUpdateActivity = id("kDiscordUpdateActivity"),
		kDiscordRequestStageActivity = id("kDiscordRequestStageActivity"),
	};

	enum KnownModeID : uint32_t {
		kStageCell = 0x01654c00,
		kStageCreature = 0x01654c01,
		kStageTribe = 0x01654c02,
		kStageCiv = 0x01654c04,
		kStageSpace = 0x01654c05,
		kAdventure = 0x01654c10,
		kMainMenu = 0x02ccd1d2,
		kLoading = 0x8916f92d, // ? Only pre-cell stage cutscene?
		kEditor = 0x00dbdba1, // Any editor, excluding Adventure
	};

	struct StageMessageData {
		uint32_t stageID;
		ResourceID activityPropFile;
	};

	class StageHandler
		: public App::DefaultMessageListener
	{
	protected:
		uint32_t modeID;
		uint32_t lastUsedID = id("sys_await_correct_mode");

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