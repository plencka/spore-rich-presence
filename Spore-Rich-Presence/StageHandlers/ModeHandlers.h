#pragma once
#include "StageData.h"

namespace SporePresence {
	namespace Handlers {
		class SpaceHandler
			: public StageHandler
		{
		public:
			SpaceHandler::SpaceHandler()
			{
				modeID = KnownModeID::kStageSpace;
				defaultResourceID = { id("Unknown"), id("RPC_SpaceArchetypes") };
			}

		protected:
			bool HandleMessage(uint32_t messageID, void* message) override;
		};


		class EditorHandler
			: public StageHandler
		{
		public:
			EditorHandler::EditorHandler()
			{
				modeID = KnownModeID::kEditor;
				defaultResourceID = { id("Unknown"), id("RPC_EditorTypes") };
			}

		protected:
			bool HandleMessage(uint32_t messageID, void* message) override;
		};


		class AdventureHandler
			: public StageHandler
		{
		public:
			AdventureHandler::AdventureHandler()
			{
				modeID = KnownModeID::kAdventure;
				defaultResourceID = { id("Unknown"), id("RPC_Adventures") };
			}

		protected:
			bool HandleMessage(uint32_t messageID, void* message) override;
		};
	}

	static void InitializeListeners() {
		MessageManager.AddListener(new Handlers::SpaceHandler(), StageMessageID::kDiscordRequestStageActivity);
		MessageManager.AddListener(new Handlers::EditorHandler(), StageMessageID::kDiscordRequestStageActivity);
		MessageManager.AddListener(new Handlers::AdventureHandler(), StageMessageID::kDiscordRequestStageActivity);
	}
}
