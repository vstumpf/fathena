#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <common/cbasetypes.hpp>

class IpBanDb {
public:
	virtual ~IpBanDb() = default;

	virtual bool init(std::unique_ptr<ISqlHandle> sqlHandle) = 0;

	// Set a property in the database
	virtual bool setProperty(std::string_view key, std::string_view value) = 0;
	
	// Checks if the given IP is banned
	virtual bool fetch(uint32 ip) = 0;

	// Creates a new IP ban entry
	virtual bool create(uint32 ip, int32 duration_minutes, std::string_view reason) = 0;

	// Cleans up expired bans
	virtual bool cleanup() = 0;
};

