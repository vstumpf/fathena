#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include <common/ISqlStatement.hpp>

class MockSqlStatement : public ISqlStatement {
public:
    MOCK_METHOD(int32_t, MOCKprepare, (const char* query));
    MOCK_METHOD(int32_t, prepareStr, (const char* query), (override));
    MOCK_METHOD(size_t, numParams, (), (override));
    MOCK_METHOD(int32_t, bindParam, (size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len), (override));
    MOCK_METHOD(int32_t, execute, (), (override));
    MOCK_METHOD(uint64_t, lastInsertId, (), (override));
    MOCK_METHOD(size_t, numColumns, (), (override));
    MOCK_METHOD(int32_t, bindColumn, (size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len), (override));
    MOCK_METHOD(void, freeResult, (), (override));
    MOCK_METHOD(void, showDebug_, (const char* debug_file, const unsigned long debug_line), (const, override));


    int32_t prepare(const char* query, ...) override {
        va_list args;
        va_start(args, query);
        char buffer[4096];
        vsnprintf(buffer, sizeof(buffer), query, args);
        va_end(args);
        return MOCKprepare(buffer);
    }
};
