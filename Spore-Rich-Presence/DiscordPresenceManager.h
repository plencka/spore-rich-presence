#pragma once
#include "discord.h"
#include "StageHandlers/StageData.h"

namespace SporePresence {
	struct SporeActivity {
		uint32_t lastModeID;
		std::int64_t startTimestamp;
		discord::Activity activity;
	};

	class DiscordPresenceManager
		: public Object
		, public DefaultRefCounted
		, public App::IUpdatable
		, public App::DefaultMessageListener
	{
	public:
		static const uint32_t type = id("DiscordPresenceManager");
		DiscordPresenceManager();
		SporeActivity discordData{};

		void UpdateValues(ResourceID fileID);
		void NewModeActivity(uint32_t newMode);
		// Inherited via Object
		virtual int AddRef() override;
		virtual int Release() override;
		virtual void* Cast(uint32_t typeID) const override;

		// Inherited via IUpdatable
		virtual void Update() override;

		bool HandleMessage(uint32_t messageID, void* message) override;
	};
}
