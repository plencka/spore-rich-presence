#include "stdafx.h"
#include <chrono>
#include "DiscordPresenceManager.h"
#include <Spore/App/IGameMode.h>
#include <Spore/App/cGameModeManager.h>

namespace SporePresence {
	discord::Core* core{};
	void InitDiscord() { // Todo: Reconnect on fail
		auto result = discord::Core::Create(890708854332076123, DiscordCreateFlags_Default, &core);
		discord::Activity activity{};
		activity.SetState("Just launched.");
		core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});
	}


	DiscordPresenceManager::DiscordPresenceManager() {
		auto now = std::chrono::system_clock::now();
		discordData.startTimestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
		discordData.lastModeID = 0;
		MessageManager.AddListener(this, StageMessageID::kDiscordUpdateActivity);
		InitDiscord();
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

	void DiscordPresenceManager::UpdateValues(ResourceID fileID) {
		PropertyListPtr propList;
		if (PropManager.GetPropertyList(fileID.instanceID, fileID.groupID, propList))
		{
			LocalizedString lString;
			if (App::Property::GetText(propList.get(), 0x64332DFD, lString)) { // largeHoverName
				string cString = "";
				cString.sprintf("%ls", lString.GetText());
				discordData.activity.GetAssets().SetLargeText(cString.c_str());
			}
			if (App::Property::GetText(propList.get(), 0x304B070F, lString)) { // detailName
				string cString = "";
				cString.sprintf("%ls", lString.GetText());
				discordData.activity.SetDetails(cString.c_str());
			}
			if (App::Property::GetText(propList.get(), 0xBAD876E1, lString)) { // stateName
				string cString = "";
				cString.sprintf("%ls", lString.GetText());
				discordData.activity.SetState(cString.c_str());
			}
			if (App::Property::GetText(propList.get(), 0x4AF215B1, lString)) { // smallHoverName
				string cString = "";
				cString.sprintf("%ls", lString.GetText());
				discordData.activity.GetAssets().SetSmallText(cString.c_str());
			}

			string16 unicodeString;
			if (App::Property::GetString16(propList.get(), 0xABE76E55, unicodeString)) { // largeImage
				string cString = "";
				cString.sprintf("%ls", unicodeString.c_str());
				discordData.activity.GetAssets().SetLargeImage(cString.c_str());
			}
			if (App::Property::GetString16(propList.get(), 0x1C6B2351, unicodeString)) { // smallImage
				string cString = "";
				cString.sprintf("%ls", unicodeString.c_str());
				discordData.activity.GetAssets().SetSmallImage(cString.c_str());
			}

			App::ConsolePrintF("Updated discord status.");
			core->ActivityManager().UpdateActivity(discordData.activity, [](discord::Result result) {});
		}
	}

	void DiscordPresenceManager::NewModeActivity(uint32_t newMode)
	{
		discordData.lastModeID = newMode;
		discordData.activity = discord::Activity();
		string activityText = "";
		activityText.sprintf("Mode ID: %X", discordData.lastModeID);
		discordData.activity.SetState(activityText.c_str());
		discordData.activity.SetDetails("Unknown.");
		discordData.activity.GetTimestamps().SetStart(discordData.startTimestamp);

		UpdateValues({ newMode , id("RPC_GameModes") });
	}


	void DiscordPresenceManager::Update()
	{
		if (auto modeManager = GameModeManager.Get()) {
			auto cMode = reinterpret_cast<App::cGameModeManager*>(modeManager);
			uint32_t id = GameModeManager.GetActiveModeID();
			if (discordData.lastModeID != id) {
				NewModeActivity(id);
			}
		}

		MessageManager.PostMSG(StageMessageID::kDiscordRequestStageActivity, nullptr);
		core->RunCallbacks();
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
		return false;
	}
}
