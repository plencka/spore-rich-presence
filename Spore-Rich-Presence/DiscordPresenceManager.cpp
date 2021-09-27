#include "stdafx.h"
#include <chrono>
#include "DiscordPresenceManager.h"
#include <Spore/App/IGameMode.h>
#include <Spore/App/cGameModeManager.h>
#include "Utilities/PropTextParsing.h"

namespace SporePresence {

#undef DISCORD_GHOST_STATUS // If defined, forces no updates to Discord, to prevent abuse of API rate limits when testing.
	discord::Core* discordCore{};

	DiscordPresenceManager::DiscordPresenceManager() {
		discordData.lastModeID = 0;
		discordData.startTimestamp = GetCurrentTimestamp();

		MessageManager.AddListener(this, App::OnModeEnterMessage::ID);
		MessageManager.AddListener(this, StageMessageID::kDiscordUpdateActivity);

		InitDiscord();
	}

	DiscordPresenceManager::~DiscordPresenceManager()
	{
		discordCore->~Core();
	}

	void DiscordPresenceManager::InitDiscord() {
		auto result = discord::Core::Create(discordApplicationID, DiscordCreateFlags_NoRequireDiscord, &discordCore);

		if (!discordCore) {
			return;
		}

		discord::Activity activity{};
		activity.SetState("Loading...");
		discordCore->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});

		if (auto modeManager = GameModeManager.Get()) {
			auto cMode = reinterpret_cast<App::cGameModeManager*>(modeManager);
			NewModeActivity(GameModeManager.GetActiveModeID());
		}
	}

	void DiscordPresenceManager::RefreshDiscordStatus(bool forceRefresh)
	{
		if (discordData.requiresRefresh || forceRefresh) {
			auto now = std::chrono::system_clock::now();
			uint64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

			if ((timestamp - discordData.refreshTimestamp) > rateFriendlySeconds) {
				discordData.refreshTimestamp = timestamp;
				discordData.requiresRefresh = false;

#ifdef DISCORD_GHOST_STATUS
				return;
#endif
				discordCore->ActivityManager().UpdateActivity(discordData.activity, [](discord::Result result) {});
			}
		}
	}

	void DiscordPresenceManager::Update()
	{
		if (!discordCore) {
			return;
		}
		MessageManager.PostMSG(StageMessageID::kDiscordRequestStageActivity, nullptr);

		RefreshDiscordStatus();
		discordCore->RunCallbacks();
	}

	uint64_t DiscordPresenceManager::GetCurrentTimestamp()
	{
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
	}

	void DiscordPresenceManager::UpdateValues(ResourceID fileID) {
		PropertyListPtr propList;
		if (PropManager.GetPropertyList(fileID.instanceID, fileID.groupID, propList))
		{
			string text;
			if (GetTextFromLocaleString(propList.get(), 0x64332DFD, text)) {
				discordData.activity.GetAssets().SetLargeText(text.c_str());
			}
			if (GetTextFromLocaleString(propList.get(), 0x304B070F, text)) {
				discordData.activity.SetDetails(text.c_str());
			}
			if (GetTextFromLocaleString(propList.get(), 0xBAD876E1, text)) {
				discordData.activity.SetState(text.c_str());
			}
			if (GetTextFromLocaleString(propList.get(), 0x4AF215B1, text)) {
				discordData.activity.GetAssets().SetSmallText(text.c_str());
			}

			if (GetTextFromUnicode(propList.get(), 0xABE76E55, text)) {
				discordData.activity.GetAssets().SetLargeImage(text.c_str());
			}
			if (GetTextFromUnicode(propList.get(), 0x1C6B2351, text)) {
				discordData.activity.GetAssets().SetSmallImage(text.c_str());
			}

			discordData.requiresRefresh = true;
		}
	}

	void DiscordPresenceManager::NewModeActivity(uint32_t newMode)
	{
		discordData.lastModeID = newMode;
		discordData.activity = discord::Activity();

		discordData.activity.SetDetails("Unknown Mode");
		discordData.activity.GetTimestamps().SetStart(discordData.startTimestamp);

		UpdateValues({ newMode , id("RPC_GameModes") });
	}


	bool DiscordPresenceManager::HandleMessage(uint32_t messageID, void* message) {
		if (messageID == StageMessageID::kDiscordUpdateActivity)
		{
			auto data = (StageMessageData*)message;

			if (data->stageID == discordData.lastModeID) {
				UpdateValues(data->activityPropFile);
			}

			return true;
		}

		if (messageID == App::OnModeEnterMessage::ID)
		{
			auto data = (App::OnModeEnterMessage*)message;

			uint32_t newModeID = data->GetModeID();
			if (discordData.lastModeID != newModeID) {
				NewModeActivity(newModeID);
			}

			return false;
		}

		return false;
	}

	// Generic API methods
	int DiscordPresenceManager::AddRef()
	{
		return DefaultRefCounted::AddRef();
	}

	int DiscordPresenceManager::Release()
	{
		return DefaultRefCounted::Release();
	}

	void* DiscordPresenceManager::Cast(uint32_t typeID) const
	{
		CLASS_CAST(Object);
		CLASS_CAST(DiscordPresenceManager);
		return nullptr;
	}
}
