#include "stdafx.h"
#include "DiscordPresenceManager.h"
#include "StageHandlers/StageData.h"
#include "Utilities/ActivityFileReader.h"
#include "Utilities/UintTimeStamps.h"

namespace SporePresence {
	intrusive_ptr<App::UpdateMessageListener> DiscordPresenceManager::listenerInstance;
	uint64_t DiscordPresenceManager::startTimestamp = 0;
	uint64_t DiscordPresenceManager::refreshTimestamp = 0;

	void DiscordPresenceManager::SporeInit()
	{
		UintTimeStamp::SetTimeStamp(startTimestamp);
	}

	DiscordPresenceManager::DiscordPresenceManager() {
		discordData.lastModeID = 0;
		discordData.failCount = 0;

		MessageManager.AddListener(this, App::OnModeEnterMessage::ID);
		MessageManager.AddListener(this, StageMessageID::kDiscordUpdateActivity);

		InitDiscord();
	}

	DiscordPresenceManager::~DiscordPresenceManager()
	{
		if (!MessageManager.Get()) {
			return;
		}
		MessageManager.RemoveListener(this, App::OnModeEnterMessage::ID);
		MessageManager.RemoveListener(this, StageMessageID::kDiscordUpdateActivity);
		discordCore->~Core();
	}

	void DiscordPresenceManager::Initialize()
	{
		if (listenerInstance) {
			return;
		}
		listenerInstance = App::AddUpdateFunction(new SporePresence::DiscordPresenceManager());
	}

	void DiscordPresenceManager::Dispose()
	{
		if (!listenerInstance) {
			return;
		}
		App::RemoveUpdateFunction(listenerInstance);
		listenerInstance = nullptr;
	}

	void DiscordPresenceManager::ResetManager()
	{
		Dispose();
		Initialize();
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
			NotifyDiscord(true);
		}
	}


	void DiscordPresenceManager::Update()
	{
		if (UintTimeStamp::HasElapsed(refreshTimestamp, rateFriendlyValue)) {
			if (!discordCore) {
				ResetManager();
				return;
			}
			MessageManager.PostMSG(StageMessageID::kDiscordRequestStageActivity, nullptr);
			NotifyDiscord();
		}

		if (!discordCore) {
			return;
		}

		if (discordCore->RunCallbacks() == discord::Result::Ok) {
			discordData.failCount = 0;
		}
		else {
			if (++discordData.failCount > reconnectThreshold) {
				ResetManager();
			};
		}
	}


	void DiscordPresenceManager::NotifyDiscord(bool forceRefresh)
	{
		if (discordData.requiresRefresh || forceRefresh) {
#ifdef DISCORD_GHOST_STATUS
			discordData.requiresRefresh = false;
			return;
#endif
			discordCore->ActivityManager().UpdateActivity(discordData.activity, [this](discord::Result result) {
				if (result == discord::Result::Ok) {
					discordData.failCount = 0;
					discordData.requiresRefresh = false;
				}
				else {
					if (++discordData.failCount > reconnectThreshold) {
						ResetManager();
					};
				}
			});
		}
	}

	void DiscordPresenceManager::UpdateActivityData(ResourceID fileID, bool repeatToUnknownOnFail) {
		PropertyListPtr propList;
		if (PropManager.GetPropertyList(fileID.instanceID, fileID.groupID, propList))
		{
			ActivityFileReader(propList).UpdateData(discordData.activity);
			discordData.requiresRefresh = true;
		}
		else if (repeatToUnknownOnFail){
			fileID.instanceID = id("Unknown");
			UpdateActivityData(fileID, false);
		}
	}

	void DiscordPresenceManager::NewModeActivity(uint32_t newMode)
	{
		discordData.lastModeID = newMode;
		discordData.activity = discord::Activity();

		discordData.activity.GetTimestamps().SetStart(startTimestamp);

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
