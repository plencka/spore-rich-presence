#pragma once
#include <Spore/App/PropertyList.h>
#include <discord.h>

namespace SporePresence {
	class ActivityFileReader {
	private:
		PropertyListPtr propFile;
		string text;
	public:
		ActivityFileReader(PropertyListPtr propList) {
			propFile = propList;
		};
		~ActivityFileReader() {
			propFile = nullptr;
		};


		/// Read data from prop file into given activity.
		void UpdateData(discord::Activity& activity) {
			if (GetTextFromLocaleString(0x64332DFD)) {
				activity.GetAssets().SetLargeText(text.c_str());
			}
			if (GetTextFromLocaleString(0x304B070F)) {
				activity.SetDetails(text.c_str());
			}
			if (GetTextFromLocaleString(0xBAD876E1)) {
				activity.SetState(text.c_str());
			}
			if (GetTextFromLocaleString(0x4AF215B1)) {
				activity.GetAssets().SetSmallText(text.c_str());
			}

			if (GetTextFromUnicode(0xABE76E55)) {
				activity.GetAssets().SetLargeImage(text.c_str());
			}
			if (GetTextFromUnicode(0x1C6B2351)) {
				activity.GetAssets().SetSmallImage(text.c_str());
			}
		};

		// Get string from prop file from string16
		bool GetTextFromUnicode(uint32_t propertyID) {
			string16 unicodeString;
			if (App::Property::GetString16(propFile.get(), propertyID, unicodeString)) {
				text = "";
				text.sprintf("%ls", unicodeString.c_str());
				return true;
			}
			return false;
		}

		// Get string from prop list from LocalizedString
		bool GetTextFromLocaleString(uint32_t propertyID) {
			LocalizedString lString;
			if (App::Property::GetText(propFile.get(), propertyID, lString)) {
				text = "";
				text.sprintf("%ls", lString.GetText());
				return true;
			}
			return false;
		}
	};
}
