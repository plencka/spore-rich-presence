#pragma once
#include <chrono>

namespace SporePresence {
	class UintTimeStamp {
	public:
		static uint64_t GetCurrentTimestamp() {
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
		};

		/// Set current timestamp to the argument.
		static void SetTimeStamp(uint64_t& timestamp) {
			timestamp = GetCurrentTimestamp();
		};

		/// @param prevTimeStamp - Timestamp that should be in the past.
		/// @param waitTime - In seconds.
		/// @param writeTimeStamp - Overwrite given timestamp on true.
		/// @return bool - Has `waitTime` elapsed or not.
		static bool HasElapsed(uint64_t& prevTimeStamp, uint64_t waitTime, bool writeTimeStamp = true) {
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			uint64_t currentTimestamp = GetCurrentTimestamp();
			if ((currentTimestamp - prevTimeStamp) > waitTime) {
				if (writeTimeStamp) {
					prevTimeStamp = currentTimestamp;
				}
				return true;
			}
			return false;
		};
	};
}
