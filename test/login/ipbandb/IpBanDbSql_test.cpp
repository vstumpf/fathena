#include <gtest/gtest.h>

#include <login/ipbandb/IpBanDbSql.hpp>

#include "mocks/MockSqlHandle.hpp"
#include "mocks/MockSqlStatement.hpp"

using ::testing::StrEq;
using ::testing::Invoke;
using ::testing::InSequence;

class IpBanDbSqlTest : public ::testing::Test {
public:
    IpBanDbSql db;

    bool didInit{false};
    std::unique_ptr<MockSqlHandle> mockSqlHandle{std::make_unique<MockSqlHandle>()};

    void dbInit() {
        db.setProperty("ipban_db_ip", "127.0.0.1");
        db.setProperty("ipban_db_port", "3306");
        db.setProperty("ipban_db_id", "test_user");
        db.setProperty("ipban_db_pw", "test_password");
        db.setProperty("ipban_db_db", "test_db");
        db.setProperty("ipban_db_ipban_table", "test_ipban_table");
        db.setProperty("ipban_codepage", "utf8");

        EXPECT_CALL(*mockSqlHandle, connect(StrEq("test_user"), StrEq("test_password"), StrEq("127.0.0.1"), 3306, StrEq("test_db")))
            .WillOnce(::testing::Return(SQL_SUCCESS));

        EXPECT_CALL(*mockSqlHandle, setEncoding(StrEq("utf8")))
            .WillOnce(::testing::Return(SQL_SUCCESS));
        
        didInit = db.init(std::move(mockSqlHandle));
    }
};

TEST_F(IpBanDbSqlTest, InitTest) {
    dbInit();
}

TEST_F(IpBanDbSqlTest, InitTestFailConnect) {
    db.setProperty("ipban_db_ip", "127.0.0.1");
    db.setProperty("ipban_db_port", "3306");
    db.setProperty("ipban_db_id", "test_user");
    db.setProperty("ipban_db_pw", "test_password");
    db.setProperty("ipban_db_db", "test_db");
    db.setProperty("ipban_db_ipban_table", "test_ipban_table");
    db.setProperty("ipban_codepage", "utf8");

    EXPECT_CALL(*mockSqlHandle, connect(StrEq("test_user"), StrEq("test_password"), StrEq("127.0.0.1"), 3306, StrEq("test_db")))
        .WillOnce(::testing::Return(SQL_ERROR));
    didInit = db.init(std::move(mockSqlHandle));
    EXPECT_FALSE(didInit);
}

TEST_F(IpBanDbSqlTest, InitTestFailEncoding) {
    db.setProperty("ipban_db_ip", "127.0.0.1");
    db.setProperty("ipban_db_port", "3306");
    db.setProperty("ipban_db_id", "test_user");
    db.setProperty("ipban_db_pw", "test_password");
    db.setProperty("ipban_db_db", "test_db");
    db.setProperty("ipban_db_ipban_table", "test_ipban_table");
    db.setProperty("ipban_codepage", "utf8");

    EXPECT_CALL(*mockSqlHandle, connect(StrEq("test_user"), StrEq("test_password"), StrEq("127.0.0.1"), 3306, StrEq("test_db")))
        .WillOnce(::testing::Return(SQL_SUCCESS));
    
    EXPECT_CALL(*mockSqlHandle, setEncoding(StrEq("utf8")))
        .WillOnce(::testing::Return(SQL_ERROR));
    
    didInit = db.init(std::move(mockSqlHandle));
    EXPECT_TRUE(didInit);
}

TEST_F(IpBanDbSqlTest, testFetch) {
    const uint32 test_ip = 2130706433;

    EXPECT_CALL(*mockSqlHandle, MOCKquery(StrEq("SELECT COUNT(*) FROM `test_ipban_table` WHERE `rtime` > NOW() AND (`list` = '127.*.*.*' OR `list` = '127.0.*.*' OR `list` = '127.0.0.*' OR `list` = '127.0.0.1')")))
        .WillOnce(::testing::Return(SQL_SUCCESS));

    EXPECT_CALL(*mockSqlHandle, nextRow())
        .WillOnce(::testing::Return(SQL_SUCCESS));

    const char* count_str = "1";
    EXPECT_CALL(*mockSqlHandle, getData(0, ::testing::_, ::testing::_))
        .WillOnce(Invoke([count_str](size_t col, char** out_buf, size_t* out_len) {
            *out_buf = const_cast<char*>(count_str);
            if (out_len) {
                *out_len = strlen(count_str);
            }
            return SQL_SUCCESS;
        }));

    EXPECT_CALL(*mockSqlHandle, freeResult());

    dbInit();
    EXPECT_TRUE(db.fetch(test_ip));
}


TEST_F(IpBanDbSqlTest, testFetchNone) {
    const uint32 test_ip = 2130706433;

    EXPECT_CALL(*mockSqlHandle, MOCKquery(StrEq("SELECT COUNT(*) FROM `test_ipban_table` WHERE `rtime` > NOW() AND (`list` = '127.*.*.*' OR `list` = '127.0.*.*' OR `list` = '127.0.0.*' OR `list` = '127.0.0.1')")))
        .WillOnce(::testing::Return(SQL_SUCCESS));

    EXPECT_CALL(*mockSqlHandle, nextRow())
        .WillOnce(::testing::Return(SQL_SUCCESS));

    const char* count_str = "0";
    EXPECT_CALL(*mockSqlHandle, getData(0, ::testing::_, ::testing::_))
        .WillOnce(Invoke([count_str](size_t col, char** out_buf, size_t* out_len) {
            *out_buf = const_cast<char*>(count_str);
            if (out_len) {
                *out_len = strlen(count_str);
            }
            return SQL_SUCCESS;
        }));

    EXPECT_CALL(*mockSqlHandle, freeResult());

    dbInit();
    EXPECT_FALSE(db.fetch(test_ip));
}


TEST_F(IpBanDbSqlTest, testCreate) {

    const uint32 test_ip = 2130706433;
    const int32 duration_minutes = 60;
    const std::string test_reason = "UnitTestBan";

    EXPECT_CALL(*mockSqlHandle, MOCKquery(StrEq("INSERT INTO `test_ipban_table`(`list`, `rtime`, `reason`) VALUES ('127.0.0.*', NOW() + INTERVAL 60 MINUTE, 'UnitTestBan')")))
    .WillOnce(::testing::Return(SQL_SUCCESS));
    
    EXPECT_CALL(*mockSqlHandle, escapeString(StrEq(test_reason)))
        .WillOnce(::testing::Return(test_reason));

    dbInit();
    EXPECT_TRUE(db.create(test_ip, duration_minutes, test_reason));
}

TEST_F(IpBanDbSqlTest, testCleanup) {

    EXPECT_CALL(*mockSqlHandle, MOCKquery(StrEq("DELETE FROM `test_ipban_table` WHERE `rtime` <= NOW()")))
        .WillOnce(::testing::Return(SQL_SUCCESS));

    dbInit();
    EXPECT_TRUE(db.cleanup());
}
