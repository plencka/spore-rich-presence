#include "stdafx.h"
#include "DiscordPresenceManager.h"
#include "StageHandlers/StageData.h"
#include "Utilities/PropTextParsing.h"
#include "Utilities/UintTimeStamps.h"

namespace SporePresence {

#undef DISCORD_GHOST_STATUS
	discord::Core* discordCore{};

	DiscordPresenceManager::DiscordPresenceManager() {
		discordData.lastModeID = 0;
		UintTimeStamp::SetTimestamp(discordData.startTimestamp);

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

		if (GameModeManager.Get()) {
			NewModeActivity(GameModeManager.GetActiveModeID());
		}
	}

	void DiscordPresenceManager::Update()
	{
		if (!discordCore) {
			return;
		}
		MessageManager.PostMSG(StageMessageID::kDiscordRequestStageActivity, nullptr);

		NotifyDiscord();
		discordCore->RunCallbacks();
	}


	void DiscordPresenceManager::NotifyDiscord(bool forceRefresh)
	{
		if (discordData.requiresRefresh || forceRefresh) {
			auto now = std::chrono::system_clock::now();
			uint64_t timestamp = UintTimeStamp::GetCurrentTimestamp();
			if (UintTimeStamp::HasElapsed(discordData.refreshTimestamp, rateFriendlyValue)) {
				discordData.refreshTimestamp = timestamp;
				discordData.requiresRefresh = false;

#ifdef DISCORD_GHOST_STATUS
				return;
#endif
				discordCore->ActivityManager().UpdateActivity(discordData.activity, [](discord::Result result) {});
			}
		}
	}

	void DiscordPresenceManager::UpdateActivityData(ResourceID fileID) {
		PropertyListPtr propList;
		if (PropManager.GetPropertyList(fileID.instanceID, fileID.groupID, propList))
		{
			ActivityFileReader activityRead = ActivityFileReader(propList);
			string text;
			if (activityRead.GetTextFromLocaleString(0x64332DFD, text)) {
				discordData.activity.GetAssets().SetLargeText(text.c_str());
			}
			if (activityRead.GetTextFromLocaleString(0x304B070F, text)) {
				discordData.activity.SetDetails(text.c_str());
			}
			if (activityRead.GetTextFromLocaleString(0xBAD876E1, text)) {
				discordData.activity.SetState(text.c_str());
			}
			if (activityRead.GetTextFromLocaleString(0x4AF215B1, text)) {
				discordData.activity.GetAssets().SetSmallText(text.c_str());
			}

			if (activityRead.GetTextFromUnicode(0xABE76E55, text)) {
				discordData.activity.GetAssets().SetLargeImage(text.c_str());
			}
			if (activityRead.GetTextFromUnicode(0x1C6B2351, text)) {
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

		UpdateActivityData({ newMode , id("RPC_GameModes") });
	}


	bool DiscordPresenceManager::HandleMessage(uint32_t messageID, void* message) {
		if (messageID == StageMessageID::kDiscordUpdateActivity)
		{
			auto data = (StageMessageData*)message;

			if (data->stageID == discordData.lastModeID) {
				UpdateActivityData(data->activityPropFile);
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
