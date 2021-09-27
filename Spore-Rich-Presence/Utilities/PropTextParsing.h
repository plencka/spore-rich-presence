#pragma once
#include <Spore/LocalizedString.h>
#include <Spore/App/PropertyList.h>

namespace SporePresence {
	static bool GetTextFromUnicode(const App::PropertyList* prop, uint32_t propertyID, string& text) {
		string16 unicodeString;
		if (App::Property::GetString16(prop, propertyID, unicodeString)) { // largeImage
			string cString = "";
			cString.sprintf("%ls", unicodeString.c_str());
			text = cString;
			return true;
		}
		return false;
	}

	static bool GetTextFromLocaleString(const App::PropertyList* prop, uint32_t propertyID, string& text) {
		LocalizedString lString;
		if (App::Property::GetText(prop, propertyID, lString)) {
			string cString = "";
			cString.sprintf("%ls", lString.GetText());
			text = cString;
			return true;
		}
		return false;
	}
}
