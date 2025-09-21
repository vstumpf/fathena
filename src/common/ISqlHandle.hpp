#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <cstdarg>
#include <memory>

#include "ISqlStatement.hpp"

class ISqlHandle {
public:
    virtual ~ISqlHandle() = default;
    virtual uint32_t getError() const = 0;
    virtual int32_t connect(const char* user, const char* passwd, const char* host, uint16_t port, const char* db) = 0;
    virtual int32_t setEncoding(const char* encoding) = 0;
    virtual int32_t getTimeout(uint32_t* out_timeout) const = 0;
    virtual int32_t getColumnNames(const char* table, char* out_buf, size_t buf_len, char sep) const = 0;
    virtual int32_t ping() = 0;
    virtual std::string escapeString(std::string_view from) const = 0;
    virtual int32_t query(const char* query, ...) = 0;
    virtual int32_t queryStr(const char* query) = 0;
    virtual uint64_t lastInsertId() const = 0;
    virtual uint32_t numColumns() const = 0;
    virtual uint64_t numRows() const = 0;
    virtual uint64_t numRowsAffected() const = 0;
    virtual int32_t nextRow() = 0;
    virtual int32_t getData(size_t col, char** out_buf, size_t* out_len) const = 0;
    virtual void freeResult() = 0;
    virtual void showDebug_(const char* debug_file, const unsigned long debug_line) const = 0;
    virtual std::unique_ptr<ISqlStatement> createStatement() = 0;
};


#if defined(SQL_REMOVE_SHOWDEBUG)
#define SqlHandleShowDebug(handle) (void)0
#else
#define SqlHandleShowDebug(handle) (handle)->showDebug_(__FILE__, __LINE__)
#endif
