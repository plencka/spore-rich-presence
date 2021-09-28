#pragma once
#include <Spore/App/PropertyList.h>

namespace SporePresence {
	class ActivityFileReader {
	private:
		PropertyListPtr propFile;

	public:
		~ActivityFileReader() {
			propFile = nullptr;
		};
		ActivityFileReader(PropertyListPtr propList) {
			propFile = propList;
		};

		// Get string from prop file from string16
		bool GetTextFromUnicode(uint32_t propertyID, string& text) {
			string16 unicodeString;
			if (App::Property::GetString16(propFile.get(), propertyID, unicodeString)) { // largeImage
				string cString = "";
				cString.sprintf("%ls", unicodeString.c_str());
				text = cString;
				return true;
			}
			return false;
		}

		// Get string from prop list from LocalizedString
		bool GetTextFromLocaleString(uint32_t propertyID, string& text) {
			LocalizedString lString;
			if (App::Property::GetText(propFile.get(), propertyID, lString)) {
				string cString = "";
				cString.sprintf("%ls", lString.GetText());
				text = cString;
				return true;
			}
			return false;
		}
	};
}
