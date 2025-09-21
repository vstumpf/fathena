#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include <common/ISqlHandle.hpp>

class MockSqlHandle : public ISqlHandle {
public:
    MOCK_METHOD(uint32_t, getError, (), (const, override));
    MOCK_METHOD(int32_t, connect, (const char* user, const char* passwd, const char* host, uint16_t port, const char* db), (override));
    MOCK_METHOD(int32_t, setEncoding, (const char* encoding), (override));
    MOCK_METHOD(int32_t, getTimeout, (uint32_t* out_timeout), (const, override));
    MOCK_METHOD(int32_t, getColumnNames, (const char* table, char* out_buf, size_t buf_len, char sep), (const, override));
    MOCK_METHOD(int32_t, ping, (), (override));
    MOCK_METHOD(std::string, escapeString, (std::string_view from), (const, override));
    MOCK_METHOD(int32_t, MOCKquery, (const char* query));
    MOCK_METHOD(int32_t, queryStr, (const char* query), (override));
    MOCK_METHOD(uint64_t, lastInsertId, (), (const, override));
    MOCK_METHOD(uint32_t, numColumns, (), (const, override));
    MOCK_METHOD(uint64_t, numRows, (), (const, override));
    MOCK_METHOD(uint64_t, numRowsAffected, (), (const, override));
    MOCK_METHOD(int32_t, nextRow, (), (override));
    MOCK_METHOD(int32_t, getData, (size_t col, char** out_buf, size_t* out_len), (const, override));
    MOCK_METHOD(void, freeResult, (), (override));
    MOCK_METHOD(void, showDebug_, (const char* debug_file, const unsigned long debug_line), (const, override));
    MOCK_METHOD(std::unique_ptr<ISqlStatement>, createStatement, (), (override));

    int32_t query(const char* query, ...) override {
        va_list args;
        va_start(args, query);
        char buffer[4096];
        vsnprintf(buffer, sizeof(buffer), query, args);
        va_end(args);
        return MOCKquery(buffer);
    }
};
