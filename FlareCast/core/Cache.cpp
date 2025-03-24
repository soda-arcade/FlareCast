﻿#include "Cache.h"

// Global cache object
Cache Cache::cache = Cache();

/**
 * @brief Cache constructor
 * Initializes the cache object.
 */
Cache::Cache() {

	// Set the version
	version = "7.0.0";

    // Load verified users
    vector<GuestData> verified = VerifiedList::LoadFromFile();
    verifiedList = VerifiedList(verified);

    // Load banned users
    vector<GuestData> banned = BanList::LoadFromFile();
    banList = BanList(banned);

    // Load moderators
    vector<GuestData> mods = ModList::LoadFromFile();
    modList = ModList(mods);

    // Load game data
    vector<GameData> games = GameDataList::LoadFromFile();
    gameList = GameDataList(games);

    // Load VIP users
    vector<GuestData> vips = VIPList::LoadFromFile();
    vipList = VIPList(vips);

	// Load SFX
	sfxList.init("./SFX/custom/_sfx.json");

    // Load banned IP addresses from file
    LoadBannedIpAddresses();

    // Load tiers
	tierList.loadTiers();

	// Check for updates
	checkForUpdates();

	// Get the VPN list
	getVPNList();

	// Start thread to check for updates
	std::thread updateThread = std::thread([this]() {
		while (true) {
			std::this_thread::sleep_for(std::chrono::minutes(2));
			checkForUpdates();
		}
	});
	updateThread.detach();

}

/**
 * Check for Smash Soda updates.
 */
bool Cache::checkForUpdates() {
	
	string data = "";
	size_t bodySize = sizeof(char) * data.length();

	// Prepare the response
	uint16_t _status = 0;
	void* response = nullptr;
	size_t responseSize = 0;

	// Send the request
	string headers = "Content-Type: application/json\r\n";
	headers += "Content-Length: " + to_string(bodySize) + "\r\n";

	string domain = "gist.githubusercontent.com";
	string path = "/MickeyUK/48e2000c8194a38b30c79bdb629252a8/raw";
	string method = "GET";

	const bool success = MTY_HttpRequest(
		domain.c_str(), 0, true, method.c_str(), path.c_str(),
		headers.c_str(),
		data.c_str(), bodySize, 20000,
		&response, &responseSize, &_status
	);

	const char* responseStr = (const char*)response;
	if (responseSize > 0 && _status == 200) {
		string responseString = string(responseStr, responseSize);
		json j = json::parse(responseString);

		// Set update properties
		update.version = j["version"].get<string>();
		update.notes = j["notes"].get<string>();
		update.overlay = j["overlay"].get<bool>();
		update.critical = j["critical"].get<bool>();

		// Add banned users if not already banned
		globalBans.clear();
		for (auto& bannedId : j["banned"]) {
			if (!isGlobalBanned(bannedId.get<uint32_t>())) {
				addGlobalBan(bannedId.get<uint32_t>());
			}
		}

		// Add the SODA COPS!!!
		sodaCops.clear();
		for (auto& copId : j["cops"]) {
			if (!isSodaCop(copId.get<uint32_t>())) {
				addSodaCop(copId.get<uint32_t>());
			}
		}

		// Has the version changed?
		if (version != update.version) {
			return true;
		} else {
			return false;
		}

		return false;

	}

	return false;

}

/**
 * @brief Ban an IP address
 * @param ip The IP address to ban
 */
void Cache::banIPAddress(std::string ip) {

    // If the ip address is already banned do nothing
	for (auto& bannedIP : bannedIPs) {
		if (bannedIP == ip) {
			return;
		}
	}

	// Add the ip address to the list
	bannedIPs.push_back(ip);

	// Save the banned ip addresses
	json j;
	for (auto& ip : bannedIPs) {
		j.push_back(ip);
	}

	// Save the file
	string ipPath = PathHelper::GetConfigPath() + "banned_ip.json";
	string ipString = j.dump(4);
	bool success = MTY_WriteTextFile(ipPath.c_str(), "%s", ipString.c_str());

}

/**
 * @brief Unban an IP address
 * @param ip The IP address to unban
 */
void Cache::unbanIPAddress(std::string ip) {
    for (auto& bannedIP : bannedIPs) {
		if (bannedIP == ip) {
			bannedIPs.erase(std::remove(bannedIPs.begin(), bannedIPs.end(), ip), bannedIPs.end());
		}
	}

	// Save the banned ip addresses
	json j;
	for (auto& ip : bannedIPs) {
		j.push_back(ip);
	}

	// Save the file
	string ipPath = PathHelper::GetConfigPath() + "banned_ip.json";
	string ipString = j.dump(4);
	bool success = MTY_WriteTextFile(ipPath.c_str(), "%s", ipString.c_str());
}

/**
 * @brief Remove last IP address from ban list.
 */
void Cache::unbanLastIPAddress() {
	// Was the last IP address banned?
	if (isBannedIPAddress(lastIpAddress)) {
		// Unban the IP address
		unbanIPAddress(lastIpAddress);
	}
}

/**
 * @brief Check if an IP address is banned
 * @param ip The IP address to check
 * @return True if the IP address is banned, false otherwise
 */
bool Cache::isBannedIPAddress(std::string ip) {
    return std::find(bannedIPs.begin(), bannedIPs.end(), ip) != bannedIPs.end();
}

/**
 * @brief Get the IP address of a user
 * @param userId The user ID to get the IP address of
 * @return The IP address of the user
 */
std::string Cache::getUserIpAddress(uint32_t userId) {
    return userIpMap[userId];
}

/**
 * @brief Load the banned IP addresses from file
 */
void Cache::LoadBannedIpAddresses() {

    // Load banned IP addresses
	string ipPath = PathHelper::GetConfigPath() + "banned_ip.json";
	if (MTY_FileExists(ipPath.c_str())) {

		try {

			size_t size;
			void* data = MTY_ReadFile(ipPath.c_str(), &size);

			// Parse the json
			json j = json::parse((char*)data);

			// Set the banned IP addresses
			for (auto& element : j) {
				bannedIPs.push_back(element.get<string>());
			}

		} catch (json::exception &e) {
			// Handle exception
		}

	}

}

// Get the list of VPN IP addresses
void Cache::getVPNList() {
	string data = "";
	size_t bodySize = sizeof(char) * data.length();

	// Prepare the response
	uint16_t _status = 0;
	void* response = nullptr;
	size_t responseSize = 0;

	// Send the request
	string headers = "Content-Type: application/json\r\n";
	headers += "Content-Length: " + to_string(bodySize) + "\r\n";

	string domain = "raw.githubusercontent.com";
	string path = "/X4BNet/lists_vpn/main/ipv4.txt";
	string method = "GET";

	const bool success = MTY_HttpRequest(
		domain.c_str(), 0, true, method.c_str(), path.c_str(),
		headers.c_str(),
		data.c_str(), bodySize, 20000,
		&response, &responseSize, &_status
	);

	const char* responseStr = (const char*)response;
	if (responseSize > 0 && _status == 200) {
		// Convert response to std::string
		string responseString(static_cast<char*>(response), responseSize);
		
		// Parse the response string into lines
		istringstream responseStream(responseString);
		string line;
		cidrRanges.clear();  // Clear previous entries

		while (getline(responseStream, line)) {
			// Trim any leading or trailing whitespace (if needed)
			line.erase(line.find_last_not_of(" \n\r\t") + 1); // Trim trailing spaces
			if (!line.empty()) {
				cidrRanges.push_back(line);  // Add CIDR to the vector
			}
		}

	}
}

// Function to check if an IP address is a VPN
bool Cache::isVPN(const std::string& ip) {

	for (const auto& range : cidrRanges) {
		string base = range.substr(0, range.find_last_of('.'));
		string start = range.substr(range.find_last_of('.') + 1, range.find_last_of('/') - range.find_last_of('.') - 1);
		string end = range.substr(range.find_last_of('/') + 1);

		// Convert the start and end to integers
		uint32_t startInt = std::stoi(start);
		uint32_t endInt = std::stoi(end);

		// Does IP match the base?
		if (ip.find(base) != string::npos) {
			// Get the last octet of the IP
			string lastOctet = ip.substr(ip.find_last_of('.') + 1);
			uint32_t lastOctetInt = std::stoi(lastOctet);

			// Is the last octet within the range?
			if (lastOctetInt >= startInt && lastOctetInt <= endInt) {
				return true;
			}
		}
	}

	return false;
}

/**
 * @brief Add a user to the list of Soda cops
 * @param userId The user ID to add
 */
void Cache::addSodaCop(uint32_t userId) {
    sodaCops.push_back(userId);
}

/**
 * @brief Check if a user is a Soda cop
 * @param userId The user ID to check
 * @return True if the user is a Soda cop, false otherwise
 */
bool Cache::isSodaCop(uint32_t userId) {
    if (isMickey(userId)) return true; // Mickey is like...the police chief, I guess?
    return std::find(sodaCops.begin(), sodaCops.end(), userId) != sodaCops.end();
}

/**
 * @brief Check if a user is MickeyUK
 * @param userId The user ID to check
 * @return True if the user is MickeyUK, false otherwise
 */
bool Cache::isMickey(uint32_t userId) {
    return userId == 1693946;
}

/**
 * @brief Add a user from the global ban list
 * @param userId The user ID to add
 */
void Cache::addGlobalBan(uint32_t userId) {
    globalBans.push_back(userId);
}

/**
 * @brief Check if a user is globally banned
 * @param userId The user ID to check
 * @return True if the user is globally banned, false otherwise
 */
bool Cache::isGlobalBanned(uint32_t userId) {
    return std::find(globalBans.begin(), globalBans.end(), userId) != globalBans.end();
}


