#pragma once

#include <cstdarg>

#include "sql.hpp"

class ISqlStatement {
public:
    virtual ~ISqlStatement() = default;

    virtual int32_t prepare(const char* query, ...) = 0;
    virtual int32_t prepareStr(const char* query) = 0;
    virtual size_t numParams() = 0;
    virtual int32_t bindParam(size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) = 0;
    virtual int32_t execute() = 0;
    virtual uint64_t lastInsertId() = 0;
    virtual size_t numColumns() = 0;
    virtual int32_t bindColumn(size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) = 0;
    virtual void freeResult() = 0;
    virtual void showDebug_(const char* debug_file, const unsigned long debug_line) const = 0;
};

#if defined(SQL_REMOVE_SHOWDEBUG)
#define SqlStatementShowDebug(statement) (void)0
#else
// TODO: we have to keep this until C++20 and std::source_location is available
#define SqlStatementShowDebug(statement) (statement)->showDebug_(__FILE__, __LINE__)
#endif
