#pragma once
#include "StageData.h"

namespace SporePresence {
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

	static void InitializeListeners() {
		MessageManager.AddListener(new SpaceHandler(), StageMessageID::kDiscordRequestStageActivity);
	}
}