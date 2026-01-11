#pragma once

#include <memory>
#include <common/ISqlHandle.hpp>
#include <common/sql.hpp>

#include "IpBanDb.hpp"

class IpBanDbSql : public IpBanDb {
public:
	~IpBanDbSql() override;

	bool init(std::unique_ptr<ISqlHandle> sqlHandle) override;

	// Set a property in the database
	bool setProperty(std::string_view key, std::string_view value) override;

	// Checks if the given IP is banned
	bool fetch(uint32 ip) override;

	// Creates a new IP ban entry
	bool create(uint32 ip, int32 duration_minutes, std::string_view reason) override;

	// Cleans up expired bans
	bool cleanup() override;

private:
	std::unique_ptr<ISqlHandle> sqlHandle_{nullptr};
	std::string db_hostname_{"127.0.0.1"};
	uint16 db_port_{3306};
	std::string db_username_{"ragnarok"};
	std::string db_password_{""};
	std::string db_database_{"ragnarok"};
	std::string codepage_{""};
	// table name
	std::string ipban_table_{"ipbanlist"};

};

