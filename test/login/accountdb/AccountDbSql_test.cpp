#include <gtest/gtest.h>

#include <login/accountdb/AccountDbSql.hpp>

#include "mocks/MockSqlHandle.hpp"
#include "mocks/MockSqlStatement.hpp"

using ::testing::StrEq;
using ::testing::Invoke;
using ::testing::InSequence;

class AccountDbSqlTest : public ::testing::Test {
public:
    AccountDbSql db;

    bool didInit{false};
    std::unique_ptr<MockSqlHandle> mockSqlHandle{std::make_unique<MockSqlHandle>()};

    void dbInit() {
        db.setProperty("login_server_ip", "127.0.0.1");
        db.setProperty("login_server_port", "3306");
        db.setProperty("login_server_id", "test_user");
        db.setProperty("login_server_pw", "test_password");
        db.setProperty("login_server_db", "test_db");
        db.setProperty("login_server_account_db", "test_account_table");
        db.setProperty("login_server_global_acc_reg_str_table", "test_global_acc_reg_str");
        db.setProperty("login_server_global_acc_reg_num_table", "test_global_acc_reg_num");
        db.setProperty("login_codepage", "utf8");
        db.setProperty("login_case_sensitive", "1");

        EXPECT_CALL(*mockSqlHandle, connect(StrEq("test_user"), StrEq("test_password"), StrEq("127.0.0.1"), 3306, StrEq("test_db")))
            .WillOnce(::testing::Return(SQL_SUCCESS));

        EXPECT_CALL(*mockSqlHandle, setEncoding(StrEq("utf8")))
            .WillOnce(::testing::Return(SQL_SUCCESS));
        
        EXPECT_CALL(*mockSqlHandle, MOCKquery(StrEq("UPDATE `test_account_table` SET `web_auth_token` = NULL, `web_auth_token_enabled` = '0'")))
            .WillOnce(::testing::Return(SQL_SUCCESS));

        EXPECT_CALL(*mockSqlHandle, MOCKquery(StrEq("UPDATE `test_account_table` SET `web_auth_token` = NULL")))
            .WillRepeatedly(Invoke([this](const char* query) {
                if (!didInit) {
                    ADD_FAILURE() << "Ran destructor query when not initialized";
                }
                return SQL_SUCCESS;
            }));

        didInit = db.init(std::move(mockSqlHandle));
    }
};

TEST_F(AccountDbSqlTest, InitTest) {
    dbInit();
}

TEST_F(AccountDbSqlTest, InitTestFailConnect) {
    db.setProperty("login_server_ip", "127.0.0.1");
    db.setProperty("login_server_port", "3306");
    db.setProperty("login_server_id", "test_user");
    db.setProperty("login_server_pw", "test_password");
    db.setProperty("login_server_db", "test_db");
    db.setProperty("login_server_account_db", "test_account_table");
    db.setProperty("login_server_global_acc_reg_str_table", "test_global_acc_reg_str");
    db.setProperty("login_server_global_acc_reg_num_table", "test_global_acc_reg_num");
    db.setProperty("login_codepage", "utf8");
    db.setProperty("login_case_sensitive", "1");

    EXPECT_CALL(*mockSqlHandle, connect(StrEq("test_user"), StrEq("test_password"), StrEq("127.0.0.1"), 3306, StrEq("test_db")))
        .WillOnce(::testing::Return(SQL_ERROR));

    EXPECT_CALL(*mockSqlHandle, MOCKquery(StrEq("UPDATE `test_account_table` SET `web_auth_token` = NULL")))
        .WillRepeatedly(Invoke([this](const char* query) {
            if (!didInit) {
                ADD_FAILURE() << "Ran destructor query when not initialized";
            }
            return SQL_SUCCESS;
        }));

    didInit = db.init(std::move(mockSqlHandle));
    EXPECT_FALSE(didInit);
}

TEST_F(AccountDbSqlTest, InitTestFailEncoding) {
    db.setProperty("login_server_ip", "127.0.0.1");
    db.setProperty("login_server_port", "3306");
    db.setProperty("login_server_id", "test_user");
    db.setProperty("login_server_pw", "test_password");
    db.setProperty("login_server_db", "test_db");
    db.setProperty("login_server_account_db", "test_account_table");
    db.setProperty("login_server_global_acc_reg_str_table", "test_global_acc_reg_str");
    db.setProperty("login_server_global_acc_reg_num_table", "test_global_acc_reg_num");
    db.setProperty("login_codepage", "utf8");
    db.setProperty("login_case_sensitive", "1");

    EXPECT_CALL(*mockSqlHandle, connect(StrEq("test_user"), StrEq("test_password"), StrEq("127.0.0.1"), 3306, StrEq("test_db")))
        .WillOnce(::testing::Return(SQL_SUCCESS));

    EXPECT_CALL(*mockSqlHandle, setEncoding(StrEq("utf8")))
        .WillOnce(::testing::Return(SQL_ERROR));
    
    EXPECT_CALL(*mockSqlHandle, MOCKquery(StrEq("UPDATE `test_account_table` SET `web_auth_token` = NULL, `web_auth_token_enabled` = '0'")))
        .WillOnce(::testing::Return(SQL_SUCCESS));

    EXPECT_CALL(*mockSqlHandle, MOCKquery(StrEq("UPDATE `test_account_table` SET `web_auth_token` = NULL")))
        .WillRepeatedly(Invoke([this](const char* query) {
            if (!didInit) {
                ADD_FAILURE() << "Ran destructor query when not initialized";
            }
            return SQL_SUCCESS;
        }));

    didInit = db.init(std::move(mockSqlHandle));
    EXPECT_TRUE(didInit);
}

TEST_F(AccountDbSqlTest, DisableWebToken) {
    EXPECT_CALL(*mockSqlHandle, MOCKquery(StrEq("UPDATE `test_account_table` SET `web_auth_token_enabled` = '0' WHERE `account_id` = 1234")))
        .WillOnce(::testing::Return(SQL_SUCCESS));

    dbInit();

    EXPECT_TRUE(db.disableWebToken(1234));
}

TEST_F(AccountDbSqlTest, TestCreate) {
    auto acc = MmoAccount{};
    acc.account_id = -1;
    safestrncpy(acc.userid, "testuser", sizeof(acc.userid));
    safestrncpy(acc.pass, "testpass", sizeof(acc.pass));
    acc.sex = 'M';
    safestrncpy(acc.email, "test@example.com", sizeof(acc.email));
    acc.group_id = 1;
    acc.char_slots = 4;
    acc.state = 0;
    acc.unban_time = 0;
    acc.expiration_time = 0;
    acc.logincount = 11;
    safestrncpy(acc.lastlogin, "2024-01-01 00:00:00", sizeof(acc.lastlogin));
    safestrncpy(acc.last_ip, "127.0.0.1", sizeof(acc.last_ip));
    safestrncpy(acc.birthdate, "1990-01-01", sizeof(acc.birthdate));
    safestrncpy(acc.pincode, "1234", sizeof(acc.pincode));
    acc.pincode_change = 0;

    EXPECT_CALL(*mockSqlHandle, MOCKquery(StrEq("SELECT MAX(`account_id`)+1 FROM `test_account_table`")))
        .WillOnce(::testing::Return(SQL_SUCCESS));
    EXPECT_CALL(*mockSqlHandle, nextRow())
        .WillOnce(::testing::Return(SQL_SUCCESS));
        
    const char* max_id_str = "2123456";
    EXPECT_CALL(*mockSqlHandle, getData(0, ::testing::_, ::testing::_))
        .WillOnce(Invoke([max_id_str](size_t col, char** out_buf, size_t* out_len) {
            *out_buf = const_cast<char*>(max_id_str);
            if (out_len) {
                *out_len = strlen(max_id_str);
            }
            return SQL_SUCCESS;
        }));

    EXPECT_CALL(*mockSqlHandle, freeResult());

    auto mockStatement = std::make_unique<MockSqlStatement>();
#ifdef VIP_ENABLE
    EXPECT_CALL(*mockStatement, MOCKprepare(StrEq(
        "INSERT INTO `test_account_table` (`account_id`, `userid`, `user_pass`, `sex`, `email`, `group_id`, "
        "`state`, `unban_time`, `expiration_time`, `logincount`, `lastlogin`, `last_ip`, "
        "`birthdate`, `character_slots`, `pincode`, `pincode_change`, `vip_time`, `old_group`) "
        "VALUES "
        "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"))).WillOnce(::testing::Return(SQL_SUCCESS));
#else
    EXPECT_CALL(*mockStatement, MOCKprepare(StrEq(
        "INSERT INTO `test_account_table` (`account_id`, `userid`, `user_pass`, `sex`, `email`, `group_id`, "
        "`state`, `unban_time`, `expiration_time`, `logincount`, `lastlogin`, `last_ip`, "
        "`birthdate`, `character_slots`, `pincode`, `pincode_change`) "
        "VALUES "
        "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"))).WillOnce(::testing::Return(SQL_SUCCESS));

#endif
    EXPECT_CALL(*mockStatement, bindParam(0, SQLDT_INT32, ::testing::_, sizeof(int32_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<int32_t*>(buffer), 2123456);
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(1, SQLDT_STRING, ::testing::_, 8ull))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_STREQ(static_cast<char *>(buffer), "testuser");
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(2, SQLDT_STRING, ::testing::_, 8ull))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_STREQ(static_cast<char *>(buffer), "testpass");
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(3, SQLDT_ENUM, ::testing::_, sizeof(char)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<char*>(buffer), 'M');
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(4, SQLDT_STRING, ::testing::_, 16ull))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_STREQ(static_cast<char *>(buffer), "test@example.com");
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(5, SQLDT_INT32, ::testing::_, sizeof(int32_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<int32_t*>(buffer), 1);
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(6, SQLDT_UINT32, ::testing::_, sizeof(uint32_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<uint32_t*>(buffer), 0);
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(7, SQLDT_LONG, ::testing::_, sizeof(time_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<time_t*>(buffer), 0);
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(8, SQLDT_LONG, ::testing::_, sizeof(time_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<time_t*>(buffer), 0);
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(9, SQLDT_UINT32, ::testing::_, sizeof(uint32_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<uint32_t*>(buffer), 11);
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(10, SQLDT_STRING, ::testing::_, 19ull))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_STREQ(static_cast<char *>(buffer), "2024-01-01 00:00:00");
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(11, SQLDT_STRING, ::testing::_, 9ull))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_STREQ(static_cast<char *>(buffer), "127.0.0.1");
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(12, SQLDT_STRING, ::testing::_, 10ull))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_STREQ(static_cast<char *>(buffer), "1990-01-01");
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(13, SQLDT_UCHAR, ::testing::_, sizeof(unsigned char)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<unsigned char*>(buffer), 4);
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(14, SQLDT_STRING, ::testing::_, 4ull))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_STREQ(static_cast<char *>(buffer), "1234");
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(15, SQLDT_LONG, ::testing::_, sizeof(time_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<time_t*>(buffer), 0);
            return SQL_SUCCESS;
        }));
#ifdef VIP_ENABLE
    EXPECT_CALL(*mockStatement, bindParam(16, SQLDT_LONG, ::testing::_, sizeof(time_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<time_t*>(buffer), 0);
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(17, SQLDT_INT32, ::testing::_, sizeof(int32_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<int32_t*>(buffer), 0);
            return SQL_SUCCESS;
        }));
#endif
    EXPECT_CALL(*mockStatement, execute())
        .WillOnce(::testing::Return(SQL_SUCCESS));

    EXPECT_CALL(*mockSqlHandle, createStatement())
        .WillOnce(::testing::Return(std::move(mockStatement)));

    dbInit();
    EXPECT_TRUE(db.create(acc));
}


TEST_F(AccountDbSqlTest, TestSave) {
    auto acc = MmoAccount{};
    acc.account_id = 2123456;
    safestrncpy(acc.userid, "testuser", sizeof(acc.userid));
    safestrncpy(acc.pass, "testpass", sizeof(acc.pass));
    acc.sex = 'M';
    safestrncpy(acc.email, "test@example.com", sizeof(acc.email));
    acc.group_id = 1;
    acc.char_slots = 4;
    acc.state = 0;
    acc.unban_time = 0;
    acc.expiration_time = 0;
    acc.logincount = 11;
    safestrncpy(acc.lastlogin, "2024-01-01 00:00:00", sizeof(acc.lastlogin));
    safestrncpy(acc.last_ip, "127.0.0.1", sizeof(acc.last_ip));
    safestrncpy(acc.birthdate, "1990-01-01", sizeof(acc.birthdate));
    safestrncpy(acc.pincode, "1234", sizeof(acc.pincode));
    acc.pincode_change = 0;

    auto mockStatement = std::make_unique<MockSqlStatement>();
#ifdef VIP_ENABLE
    EXPECT_CALL(*mockStatement, MOCKprepare(StrEq(
        "UPDATE `test_account_table` SET `userid`=?,`user_pass`=?,`sex`=?,`email`=?,`group_id`=?,"
        "`state`=?,`unban_time`=?,`expiration_time`=?,`logincount`=?,`lastlogin`=?,`last_ip`=?,"
        "`birthdate`=?,`character_slots`=?,`pincode`=?,`pincode_change`=?,`vip_time`=?,`old_group`=? "
        "WHERE `account_id` = '2123456'"))).WillOnce(::testing::Return(SQL_SUCCESS));
#else
    EXPECT_CALL(*mockStatement, MOCKprepare(StrEq(
        "UPDATE `test_account_table` SET `userid`=?,`user_pass`=?,`sex`=?,`email`=?,`group_id`=?,"
        "`state`=?,`unban_time`=?,`expiration_time`=?,`logincount`=?,`lastlogin`=?,`last_ip`=?,"
        "`birthdate`=?,`character_slots`=?,`pincode`=?,`pincode_change`=? "
        "WHERE `account_id` = '2123456'"))).WillOnce(::testing::Return(SQL_SUCCESS));

#endif
    EXPECT_CALL(*mockStatement, bindParam(0, SQLDT_STRING, ::testing::_, 8ull))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_STREQ(static_cast<char *>(buffer), "testuser");
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(1, SQLDT_STRING, ::testing::_, 8ull))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_STREQ(static_cast<char *>(buffer), "testpass");
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(2, SQLDT_ENUM, ::testing::_, sizeof(char)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<char*>(buffer), 'M');
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(3, SQLDT_STRING, ::testing::_, 16ull))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_STREQ(static_cast<char *>(buffer), "test@example.com");
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(4, SQLDT_INT32, ::testing::_, sizeof(int32_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<int32_t*>(buffer), 1);
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(5, SQLDT_UINT32, ::testing::_, sizeof(uint32_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<uint32_t*>(buffer), 0);
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(6, SQLDT_LONG, ::testing::_, sizeof(time_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<time_t*>(buffer), 0);
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(7, SQLDT_LONG, ::testing::_, sizeof(time_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<time_t*>(buffer), 0);
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(8, SQLDT_UINT32, ::testing::_, sizeof(uint32_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<uint32_t*>(buffer), 11);
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(9, SQLDT_STRING, ::testing::_, 19ull))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_STREQ(static_cast<char *>(buffer), "2024-01-01 00:00:00");
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(10, SQLDT_STRING, ::testing::_, 9ull))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_STREQ(static_cast<char *>(buffer), "127.0.0.1");
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(11, SQLDT_STRING, ::testing::_, 10ull))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_STREQ(static_cast<char *>(buffer), "1990-01-01");
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(12, SQLDT_UCHAR, ::testing::_, sizeof(unsigned char)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<unsigned char*>(buffer), 4);
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(13, SQLDT_STRING, ::testing::_, 4ull))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_STREQ(static_cast<char *>(buffer), "1234");
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(14, SQLDT_LONG, ::testing::_, sizeof(time_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<time_t*>(buffer), 0);
            return SQL_SUCCESS;
        }));
#ifdef VIP_ENABLE
    EXPECT_CALL(*mockStatement, bindParam(15, SQLDT_LONG, ::testing::_, sizeof(time_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<time_t*>(buffer), 0);
            return SQL_SUCCESS;
        }));
    EXPECT_CALL(*mockStatement, bindParam(16, SQLDT_INT32, ::testing::_, sizeof(int32_t)))
        .WillOnce(Invoke([](size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
            EXPECT_EQ(*static_cast<int32_t*>(buffer), 0);
            return SQL_SUCCESS;
        }));
#endif
    EXPECT_CALL(*mockStatement, execute()).WillOnce(::testing::Return(SQL_SUCCESS));

    EXPECT_CALL(*mockSqlHandle, createStatement())
        .WillOnce(::testing::Return(std::move(mockStatement)));

    dbInit();
    EXPECT_TRUE(db.save(acc));
}

TEST_F(AccountDbSqlTest, TestLoadFromUsername) {
    auto acc = MmoAccount{};
    std::vector<const char*> vals = {
        "2123456",      // account_id
        "testuser",    // userid
        "testpass",    // user_pass
        "M",    // sex
        "test@example.com", // email
        "1",       // group_id
        "0",       // state
        "0",       // unban_time
        "0",       // expiration_time
        "11",      // logincount
        "2024-01-01 00:00:00", // lastlogin
        "127.0.0.1", // last_ip
        "1990-01-01", // birthdate
        "4",       // character_slots
        "1234",       // pincode
        "0"        // pincode_change
#ifdef VIP_ENABLE
        ,"0" // vip_time
        ,"0" // old_group
#endif // VIP_ENABLE
    };

    {
        InSequence seq;

        EXPECT_CALL(*mockSqlHandle, MOCKquery(StrEq(
            "SELECT `account_id` FROM `test_account_table` WHERE `userid` = BINARY 'testuser'"
        ))).WillOnce(::testing::Return(SQL_SUCCESS));
        EXPECT_CALL(*mockSqlHandle, numRows()).WillOnce(::testing::Return(1));

        EXPECT_CALL(*mockSqlHandle, nextRow()).WillOnce(::testing::Return(SQL_SUCCESS));
        const char* account_id = "2123456";
        EXPECT_CALL(*mockSqlHandle, getData(0, ::testing::_, ::testing::_))
            .WillOnce(Invoke([account_id](size_t col, char** out_buf, size_t* out_len) {
                *out_buf = const_cast<char*>(account_id);
                if (out_len) {
                    *out_len = strlen(account_id);
                }
                return SQL_SUCCESS;
            }));

    #ifdef VIP_ENABLE
            EXPECT_CALL(*mockSqlHandle, MOCKquery(StrEq(
                "SELECT `account_id`,`userid`,`user_pass`,`sex`,`email`,`group_id`,"
                "`state`,`unban_time`,`expiration_time`,`logincount`,`lastlogin`,`last_ip`,"
                "`birthdate`,`character_slots`,`pincode`, `pincode_change`, `vip_time`, `old_group` "
                "FROM `test_account_table` WHERE `account_id` = 2123456"
            ))).WillOnce(::testing::Return(SQL_SUCCESS));
    #else
            EXPECT_CALL(*mockSqlHandle, MOCKquery(StrEq(
                "SELECT `account_id`,`userid`,`user_pass`,`sex`,`email`,`group_id`,"
                "`state`,`unban_time`,`expiration_time`,`logincount`,`lastlogin`,`last_ip`,"
                "`birthdate`,`character_slots`,`pincode`, `pincode_change` "
                "FROM `test_account_table` WHERE `account_id` = 2123456"
            ))).WillOnce(::testing::Return(SQL_SUCCESS));
    #endif // VIP_ENABLE

        EXPECT_CALL(*mockSqlHandle, nextRow()).WillOnce(::testing::Return(SQL_SUCCESS));

        EXPECT_CALL(*mockSqlHandle, getData(::testing::_, ::testing::_, nullptr))
            .WillRepeatedly(Invoke([&vals](size_t col, char** out_buf, size_t* out_len) {
                *out_buf = const_cast<char*>(vals[col]);
                if (out_len) {
                    *out_len = strlen(vals[col]);
                }
                return SQL_SUCCESS;
            }));
    }
    
    dbInit();
    EXPECT_TRUE(db.loadFromUsername(acc, "testuser"));
    EXPECT_EQ(acc.account_id, 2123456);
    EXPECT_STREQ(acc.userid, "testuser");
    EXPECT_STREQ(acc.pass, "testpass");
    EXPECT_STREQ(acc.email, "test@example.com");
    EXPECT_EQ(acc.group_id, 1);
    EXPECT_EQ(acc.state, 0);
    EXPECT_EQ(acc.unban_time, 0);
    EXPECT_EQ(acc.expiration_time, 0);
    EXPECT_EQ(acc.logincount, 11);
    EXPECT_STREQ(acc.lastlogin, "2024-01-01 00:00:00");
    EXPECT_STREQ(acc.last_ip, "127.0.0.1");
    EXPECT_STREQ(acc.birthdate, "1990-01-01");
    EXPECT_EQ(acc.char_slots, 4);
    EXPECT_STREQ(acc.pincode, "1234");
    EXPECT_EQ(acc.pincode_change, 0);
#ifdef VIP_ENABLE
    EXPECT_EQ(acc.vip_time, 0);
    EXPECT_EQ(acc.old_group, 0);
#endif // VIP_ENABLE
}
