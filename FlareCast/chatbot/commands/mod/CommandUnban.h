﻿#pragma once

#include "Cache.h"
#include "ACommand.h"
#include "parsec-dso.h"

using namespace std;

class CommandUnban : public ACommand
{
public:

	std::string usage = "Usage: !unban <username>\nExample: !unban v60\0";

	/**
	 * @brief Construct a new CommandBan object
	 *
	 * @param msg
	 * @param sender
	 * @param parsec
	 * @param guests
	 * @param guestHistory
	 */
	CommandUnban(const char* msg, Guest& sender, ParsecDSO* parsec, GuestList& guests, GuestDataList& guestHistory)
		: ACommand(msg, sender), _parsec(parsec), guests(guests), _guestHistory(guestHistory)
	{
	}

	/**
	 * @brief Run the command
	 *
	 * @return true
	 * @return false
	 */
	bool run() override {

		// Was a guest specified?
		if (getArgs().size() == 0) {
			setReply(usage);
			return false;
		}

		// Find the guest
		if (findGuest()) {
			GuestData targetData(target.name, target.userID);
			return handleGuest(targetData, true, target.id);
		}

		// Find offline guest
		string guest = getArgString();
		if (guest == "") {
			return false;
		}
		bool found = false;
		try {
			found = _guestHistory.find(guest, [&](GuestData& guest) { _offlineGuest = guest; });
		}
		catch (const std::exception&) {}
		if (found) {
			return handleGuest(_offlineGuest, false);
		}
		return false;

	}

	/**
	 * @brief Get the prefixes object
	 *
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!unban" };
	}

private:
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!unban " };
	}

	ParsecDSO* _parsec;
	Guest target;
	GuestList guests;
	GuestDataList& _guestHistory;
	GuestData _offlineGuest;

	/**
	* Get the guest referenced in the command. Returns nullptr
	* if no guest is found
	*
	* @param guestList The guest list
	*/
	bool findGuest() {

		// Get the guest
		string guest = getArgString();
		if (guest == "") {
			return false;
		}

		try {
			uint32_t id = stoul(guest);
			vector<Guest>::iterator i;
			for (i = guests.getGuests().begin(); i != guests.getGuests().end(); ++i) {
				if ((*i).userID == id) {
					target = *i;
					return true;
				}
			}
		}
		catch (const std::exception&) {
			bool found = guests.find(guest, &target);
			if (found) {
				return true;
			}
		}

		return false;
	}

	/**
	 * @brief Handle the guest
	 *
	 * @param target
	 * @param isOnline
	 * @param guestID
	 * @return true
	 * @return false
	 */
	bool handleGuest(GuestData target, bool isOnline, uint32_t guestID = -1) {

		if (Cache::cache.banList.unban(target.name)) {
			setReply("Unbanned " + target.name + ".");
		} else {
			setReply("Failed to unban " + target.name + ".");
		}

		return true;
	}
};


