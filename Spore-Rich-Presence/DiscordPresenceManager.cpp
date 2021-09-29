#include "stdafx.h"
#include "DiscordPresenceManager.h"
#include "StageHandlers/StageData.h"
#include "Utilities/ActivityFileReader.h"
#include "Utilities/UintTimeStamps.h"

namespace SporePresence {

	DiscordPresenceManager::DiscordPresenceManager() {
		discordData.lastModeID = 0;
		UintTimeStamp::SetTimeStamp(discordData.startTimestamp);

		MessageManager.AddListener(this, App::OnModeEnterMessage::ID);
		MessageManager.AddListener(this, StageMessageID::kDiscordUpdateActivity);

		InitDiscord();
	}

	DiscordPresenceManager::~DiscordPresenceManager()
	{
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

		if (UintTimeStamp::HasElapsed(discordData.refreshTimestamp, rateFriendlyValue)) {
			MessageManager.PostMSG(StageMessageID::kDiscordRequestStageActivity, nullptr);
			NotifyDiscord();
		}

		discordCore->RunCallbacks();
	}


	void DiscordPresenceManager::NotifyDiscord(bool forceRefresh)
	{
		if (discordData.requiresRefresh || forceRefresh) {
#ifdef DISCORD_GHOST_STATUS
			discordData.requiresRefresh = false;
			return;
#endif
			discordCore->ActivityManager().UpdateActivity(discordData.activity, [this](discord::Result result) {
					discordData.requiresRefresh = !(bool)(result == discord::Result::Ok);
				});
		}
	}

	void DiscordPresenceManager::UpdateActivityData(ResourceID fileID) {
		PropertyListPtr propList;
		if (PropManager.GetPropertyList(fileID.instanceID, fileID.groupID, propList))
		{
			ActivityFileReader(propList).UpdateData(discordData.activity);
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
