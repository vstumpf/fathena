// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "ipban.hpp"

#include <cstdlib>
#include <cstring>

#include <common/cbasetypes.hpp>
#include <common/showmsg.hpp>
#include <common/SqlHandle.hpp>
#include <common/strlib.hpp>
#include <common/timer.hpp>

#include "ipbandb/IpBanDb.hpp"
#include "ipbandb/IpBanDbSql.hpp"

#include "login.hpp"
#include "loginlog.hpp"


std::string ipban_db_hostname = "127.0.0.1";
uint16 ipban_db_port = 3306;
std::string ipban_db_username = "ragnarok";
std::string ipban_db_password = "";
std::string ipban_db_database = "ragnarok";
std::string ipban_codepage = "";
std::string ipban_table = "ipbanlist";

// globals
static int32 cleanup_timer_id = INVALID_TIMER;
static bool ipban_inited = false;

static std::unique_ptr<IpBanDb> ipban_db;

//early declaration
TIMER_FUNC(ipban_cleanup);

/**
 * Check if ip is in the active bans list.
 * @param ip: ipv4 ip to check if ban
 * @return true if found or error, false if not in list
 */
bool ipban_check(uint32 ip) {
	uint8* p = (uint8*)&ip;
	char* data = nullptr;

	if( !login_config.ipban )
		return false;// ipban disabled

	return getIpBanDb()->fetch(ip);
}

/**
 * Log a failed attempt.
 *  Also bans the user if too many failed attempts are made.
 * @param ip: ipv4 ip to record the failure
 */
void ipban_log(uint32 ip) {
	unsigned long failures;

	if( !login_config.ipban )
		return;// ipban disabled

	failures = loginlog_failedattempts(ip, login_config.dynamic_pass_failure_ban_interval);// how many times failed account? in one ip.

	// if over the limit, add a temporary ban entry
	if( failures >= login_config.dynamic_pass_failure_ban_limit )
	{
		getIpBanDb()->create(ip, login_config.dynamic_pass_failure_ban_duration, "Password error ban");
	}
}

/**
 * Timered function to remove expired bans.
 *  Request all characters to update their registered ip and transmit their new ip.
 *  Performed each ip_sync_interval.
 * @param tid: timer id
 * @param tick: tick of execution
 * @param id: unused
 * @param data: unused
 * @return 0
 */
TIMER_FUNC(ipban_cleanup){
	if( !login_config.ipban )
		return 0;// ipban disabled

	getIpBanDb()->cleanup();

	return 0;
}

/**
 * Read configuration options.
 * @param key: config keyword
 * @param value: config value for keyword
 * @return true if successful, false if config not complete or server already running
 */
bool ipban_config_read(const char* key, const char* value) {
	if( ipban_inited )
		return false;// settings can only be changed before init

	if (getIpBanDb()->setProperty(key, value))
		return true;

	const char* signature = "ipban_";
	if( strncmpi(key, signature, strlen(signature)) == 0 )
	{
		key += strlen(signature);

		if( strcmpi(key, "enable") == 0 )
			login_config.ipban = (config_switch(value) != 0);
		else
		if( strcmpi(key, "dynamic_pass_failure_ban") == 0 )
			login_config.dynamic_pass_failure_ban = (config_switch(value) != 0);
		else
		if( strcmpi(key, "dynamic_pass_failure_ban_interval") == 0 )
			login_config.dynamic_pass_failure_ban_interval = atoi(value);
		else
		if( strcmpi(key, "dynamic_pass_failure_ban_limit") == 0 )
			login_config.dynamic_pass_failure_ban_limit = atoi(value);
		else
		if( strcmpi(key, "dynamic_pass_failure_ban_duration") == 0 )
			login_config.dynamic_pass_failure_ban_duration = atoi(value);
		else
			return false;// not found
		return true;
	}

	return false;// not found
}


/// Constructor destructor

/**
 * Initialize the module.
 * Launched at login-serv start, create db or other long scope variable here.
 */
void ipban_init(void) {
	ipban_inited = true;

	if( !login_config.ipban )
		return;// ipban disabled

	if (!getIpBanDb()->init(std::make_unique<SqlHandle>())) {
		ShowError("Ipban database initialization failed.\n");
		exit(EXIT_FAILURE);
	}

	ShowInfo("Ipban connection made.\n");

	if( login_config.ipban_cleanup_interval > 0 )
	{ // set up periodic cleanup of connection history and active bans
		add_timer_func_list(ipban_cleanup, "ipban_cleanup");
		cleanup_timer_id = add_timer_interval(gettick()+10, ipban_cleanup, 0, 0, login_config.ipban_cleanup_interval*1000);
	} else // make sure it gets cleaned up on login-server start regardless of interval-based cleanups
		ipban_cleanup(0,0,0,0);
}

/**
 * Destroy the module.
 * Launched at login-serv end, cleanup db connection or other thing here.
 */
void ipban_final(void) {
	if( !login_config.ipban )
		return;// ipban disabled

	if( login_config.ipban_cleanup_interval > 0 )
		// release data
		delete_timer(cleanup_timer_id, ipban_cleanup);

	ipban_cleanup(0,0,0,0); // always clean up on login-server stop
}
