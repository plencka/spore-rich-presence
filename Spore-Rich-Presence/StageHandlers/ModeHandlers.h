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
			}

		protected:
			bool HandleMessage(uint32_t messageID, void* message) override;
		};
	}

	static void InitializeListeners() {
		MessageManager.AddListener(new Handlers::SpaceHandler(), StageMessageID::kDiscordRequestStageActivity);
		MessageManager.AddListener(new Handlers::EditorHandler(), StageMessageID::kDiscordRequestStageActivity);
	}
}
