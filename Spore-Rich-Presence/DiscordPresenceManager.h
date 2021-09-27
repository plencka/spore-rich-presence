#pragma once
#include "discord.h"
#include "StageHandlers/StageData.h"

namespace SporePresence {
	constexpr discord::ClientId discordApplicationID = 890708854332076123; // Read from env file?
	constexpr int rateFriendlySeconds = 5;

	struct SporeActivity {
		uint32_t lastModeID = 0;
		std::int64_t startTimestamp = 0;
		discord::Activity activity;

		uint64_t refreshTimestamp = 0;
		bool requiresRefresh = false;
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
	private:
		SporeActivity discordData{};

		~DiscordPresenceManager();

		uint64_t GetCurrentTimestamp();


		void UpdateValues(ResourceID fileID);
		void RefreshDiscordStatus(bool forceRefresh = false);
		void NewModeActivity(uint32_t newMode);

		void InitDiscord();

		// Inherited via Object
		virtual int AddRef() override;
		virtual int Release() override;
		virtual void* Cast(uint32_t typeID) const override;

		// Inherited via IUpdatable
		virtual void Update() override;

		bool HandleMessage(uint32_t messageID, void* message) override;
	};
}
