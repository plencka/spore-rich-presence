#pragma once
#include <discord.h>

namespace SporePresence {
	/// If defined, forces no updates to Discord, to prevent abuse of API rate limits when testing.
	#define DISCORD_GHOST_STATUS
	#undef DISCORD_GHOST_STATUS

	/// ID received when creating application. Not a Client Secret.
	/// Change if you want to create custom Rich Presence using own application.
	constexpr discord::ClientId discordApplicationID = 890708854332076123;

	/// In seconds. Per discord documentation: 5 updates per 20 seconds.
	constexpr int rateFriendlyValue = 5;

	// Reset manager after it reaches this threshold
	constexpr int reconnectThreshold = 5;

	/// Contains data used to generate activities.
	/// @param failCount - Reconnect after reaching threshold.
	/// @param lastModeID - Related Mode ID of this activity.
	/// @param activity - Container of discord activity.
	/// @param requiresRefresh - If this activity needs to notify Discord.
	struct SporeActivity {
		int failCount = 0;
		discord::Activity activity;
		uint32_t lastModeID = 0;

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

		/// Runs when the game launches.
		static void SporeInit();

		static void Initialize();
		static void Dispose();
		static void ResetManager();
	private:
		static intrusive_ptr<App::UpdateMessageListener> listenerInstance;

		// TimeStamp of application launch.
		static std::uint64_t startTimestamp;

		// Serves as general tick rate of an application.
		static uint64_t refreshTimestamp;

		DiscordPresenceManager();
		~DiscordPresenceManager();

		// Activity that should be displayed
		SporeActivity discordData{};

		discord::Core* discordCore{};


		// Creates initial state for the discord application client.
		void InitDiscord();

		/// Heartbeat of the modification.
		/// Communicates with external listeners and runs callbacks.
		void Update() override;

		/// Read and write data into activity if file exists.
		/// @param fileID - Source file identifier.
		void UpdateActivityData(ResourceID fileID);

		/// Refresh activity to show new data.
		/// @param forceRefresh - Refresh activity, even if it's not pending. Doesn't override rate limits.
		void NotifyDiscord(bool forceRefresh = false);

		/// Create new activity for new Game Mode.
		/// @param newModeID - ID of entered mode. 
		void NewModeActivity(uint32_t newModeID);

		/// Called by using ModAPI's MessageManager.
		/// @param messageID - Use `PostMSG` method with StageMessageID::kDiscordUpdateActivity, if you want to handle your custom Game Mode.
		/// @param messageData - Use `StageMessageData` object for mode implementation.
		bool HandleMessage(uint32_t messageID, void* messageData) override;
		
		virtual int AddRef() override;
		virtual int Release() override;
		virtual void* Cast(uint32_t typeID) const override;
	};
}
