#pragma once
#include "discord.h"

namespace SporePresence {
	/// If defined, forces no updates to Discord, to prevent abuse of API rate limits when testing.
	#define DISCORD_GHOST_STATUS

	/// ID received when creating application. Not a Client Secret.
	/// Change if you want to create custom Rich Presence using own application.
	constexpr discord::ClientId discordApplicationID = 890708854332076123;
	/// In seconds.
	constexpr int rateFriendlyValue = 5;

	/// Contains data used by displayed or pending activity.
	/// @param lastModeID - Mode ID of this activity
	/// @param startTimestamp - Related Mode ID of this activity.
	/// @param activity - Container of discord activity.
	/// @param startTimestamp - Timestamp of application launch.
	/// @param refreshTimestamp - Last time this activity was refreshed.
	/// @param requiresRefresh - If this activity needs to notify Discord.
	struct SporeActivity {
		discord::Activity activity;
		uint32_t lastModeID = 0;
		std::uint64_t startTimestamp = 0;

		uint64_t refreshTimestamp = 0;
		bool requiresRefresh = false;
	};

	/// Manager for Discord Rich Presence.
	/// Initializes, updates and listens to game messages for GameMode handling.
	class DiscordPresenceManager
		: public Object
		, public DefaultRefCounted
		, public App::IUpdatable
		, public App::DefaultMessageListener
	{
	public:
		static const uint32_t type = id("DiscordPresenceManager");

		DiscordPresenceManager();
		~DiscordPresenceManager();

	private:
		SporeActivity discordData{};

		/// Creates initial state for the discord application client.
		void InitDiscord();

		/// Heartbeat of the modification.
		/// Mostly communicates with external listeners.
		void Update() override;

		/// Read and write data into activity if file exists.
		/// @param fileID - Source file identifier.
		void UpdateActivityData(ResourceID fileID);

		/// Refresh activity to show new data.
		/// @param forceRefresh - Refresh activity, even if it's not pending.
		/// Doesn't override rate protection.
		void NotifyDiscord(bool forceRefresh = false);

		/// Create new activity for new Game Mode.
		/// @param newModeID - ID of entered mode. 
		void NewModeActivity(uint32_t newModeID);


		/// Called by using ModAPI's MessageManager.
		/// @param messageID - Use `PostMSG` method with StageMessageID::kDiscordUpdateActivity, if you want to handle your custom Game Mode.
		/// @param messageData - Use `StageMessageData` object for mode implementation.
		bool HandleMessage(uint32_t messageID, void* messageData) override;
		

		// Generic ModAPI functions

		virtual int AddRef() override;
		virtual int Release() override;
		virtual void* Cast(uint32_t typeID) const override;
	};
}
