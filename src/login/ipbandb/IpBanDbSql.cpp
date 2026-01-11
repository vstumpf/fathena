#include "IpBanDbSql.hpp"

#include <charconv>

#include <common/cbasetypes.hpp>
#include <common/showmsg.hpp>

constexpr std::string_view IP_BAN_DB_PROPERTY_PREFIX = "ipban_db_";
constexpr std::string_view IP_BAN_PROPERTY_PREFIX = "ipban_";


IpBanDbSql::~IpBanDbSql() {
	sqlHandle_.reset();
};

bool IpBanDbSql::init(std::unique_ptr<ISqlHandle> sqlHandle) {
	sqlHandle_ = std::move(sqlHandle);

	if (SQL_ERROR == sqlHandle_->connect(db_username_.c_str(), db_password_.c_str(), db_hostname_.c_str(), db_port_, db_database_.c_str())) {
		ShowError("Couldn't connect with uname: %s, host: %s, port: %hu, db: %s\n", db_username_.c_str(), db_hostname_.c_str(), db_port_, db_database_.c_str());
		SqlHandleShowDebug(sqlHandle_.get());
		sqlHandle_.reset();
		return false;
	}

	if (codepage_.empty() == false && SQL_ERROR == sqlHandle_->setEncoding(codepage_.c_str())) {
		ShowWarning("Couldn't set encoding to %s\n", codepage_.c_str());
		SqlHandleShowDebug(sqlHandle_.get());
	}


	return true; // Return true if initialization is successful
}

bool IpBanDbSql::setProperty(std::string_view key, std::string_view value) {
	// Implementation for setting a property in the database
	// This is a placeholder implementation and should be replaced with actual database queries

	ShowInfo("Setting property %s to %s\n", key.data(), value.data());
	if (key.rfind(IP_BAN_DB_PROPERTY_PREFIX, 0) == 0) {
		key.remove_prefix(IP_BAN_DB_PROPERTY_PREFIX.length());
		if (key == "ip") {
			db_hostname_ = value;
		}
		else if (key == "port") {
			std::from_chars(value.data(), value.data() + value.size(), db_port_);
		}
		else if (key == "id") {
			db_username_ = value;
		}
		else if (key == "pw") {
			db_password_ = value;
		}
		else if (key == "db") {
			db_database_ = value;
		}
		else if (key == "ipban_table") {
			ipban_table_ = value;
		}
		else {
			return false;
		}
		return true;
	}

	if (key.rfind(IP_BAN_PROPERTY_PREFIX, 0) == 0) {
		key.remove_prefix(IP_BAN_PROPERTY_PREFIX.length());
		if (key == "codepage") {
			codepage_ = value;
		}
		else if (key == "ipban_table") {
			ShowInfo("This property is deprecated, use %sipban_table instead.\n", IP_BAN_DB_PROPERTY_PREFIX.data());
			ipban_table_ = value;
		}
		else {
			return false;
		}
		return true;
	}

	return false;


	return true; // Replace with actual logic
}

bool IpBanDbSql::fetch(uint32 ip) {
	uint8* p = (uint8*)&ip;
	char* data = nullptr;

	auto result = sqlHandle_->query(
		"SELECT COUNT(*) FROM `%s` WHERE `rtime` > NOW() AND (`list` = '%u.*.*.*' OR `list` = '%u.%u.*.*' OR `list` = '%u.%u.%u.*' OR `list` = '%u.%u.%u.%u')",
		ipban_table_.c_str(), p[3], p[3], p[2], p[3], p[2], p[1], p[3], p[2], p[1], p[0]);
	
	if (result == SQL_ERROR) {
		SqlHandleShowDebug(sqlHandle_.get());
		sqlHandle_->freeResult();
		return true; // On error, assume banned
	}

	if (sqlHandle_->nextRow() != SQL_SUCCESS) {
		SqlHandleShowDebug(sqlHandle_.get());
		sqlHandle_->freeResult();
		return true; // On error, assume banned
	}

	sqlHandle_->getData(0, &data, nullptr);
	int matches = (data != nullptr) ? atoi(data) : 0;
	sqlHandle_->freeResult();
	return (matches > 0);
}

bool IpBanDbSql::create(uint32 ip, int32 duration_minutes, std::string_view reason) {
	uint8* p = (uint8*)&ip;
	std::string esc_reason = sqlHandle_->escapeString(reason);
		if (sqlHandle_->query(
				   "INSERT INTO `%s`(`list`, `rtime`, `reason`) VALUES ('%u.%u.%u.*', NOW() + INTERVAL %d MINUTE, '%s')",
				   ipban_table_.c_str(), p[3], p[2], p[1], duration_minutes,
				   esc_reason.c_str()) == SQL_ERROR) {
				SqlHandleShowDebug(sqlHandle_.get());
				return false;
		}
		return true;
}

bool IpBanDbSql::cleanup() {
	if (sqlHandle_->query(("DELETE FROM `%s` WHERE `rtime` <= NOW()"), ipban_table_.c_str()) == SQL_ERROR) {
		SqlHandleShowDebug(sqlHandle_.get());
		return false;
	}
	return true;
}
