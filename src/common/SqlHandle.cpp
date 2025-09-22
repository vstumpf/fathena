#include "SqlHandle.hpp"

#include <memory>
#include "sql.hpp"
#include "SqlStatement.hpp"


SqlHandle::SqlHandle() : handle_(Sql_Malloc(), Sql_Free) {
    if (!handle_) {
        throw std::runtime_error("Failed to allocate Sql handle");
    }
}

int32_t SqlHandle::connect(const char* user, const char* passwd, const char* host, uint16_t port, const char* db) {
    return Sql_Connect(handle_.get(), user, passwd, host, port, db);
}

uint32_t SqlHandle::getError() const {
    return Sql_GetError(handle_.get());
}

int32_t SqlHandle::setEncoding(const char* encoding) {
    return Sql_SetEncoding(handle_.get(), encoding);
}

int32_t SqlHandle::getTimeout(uint32_t* out_timeout) const {
    return Sql_GetTimeout(handle_.get(), out_timeout);
}

int32_t SqlHandle::getColumnNames(const char* table, char* out_buf, size_t buf_len, char sep) const {
    return Sql_GetColumnNames(handle_.get(), table, out_buf, buf_len, sep);
}

int32_t SqlHandle::ping() {
    return Sql_Ping(handle_.get());
}

std::string SqlHandle::escapeString(std::string_view from) const {
    return Sql_GetEscapeString(handle_.get(), from);
}

int32_t SqlHandle::query(const char* query, ...) {
    int32_t res;
    va_list args;
    va_start(args, query);
    res = Sql_QueryV(handle_.get(), query, args);
    va_end(args);
    return res;
}

int32_t SqlHandle::queryStr(const char* query) {
    return Sql_QueryStr(handle_.get(), query);
}

uint64_t SqlHandle::lastInsertId() const {
    return Sql_LastInsertId(handle_.get());
}

uint32_t SqlHandle::numColumns() const {
    return Sql_NumColumns(handle_.get());
}

uint64_t SqlHandle::numRows() const {
    return Sql_NumRows(handle_.get());
}

uint64_t SqlHandle::numRowsAffected() const {
    return Sql_NumRowsAffected(handle_.get());
}

int32_t SqlHandle::nextRow() {
    return Sql_NextRow(handle_.get());
}

int32_t SqlHandle::getData(size_t col, char** out_buf, size_t* out_len) const {
    return Sql_GetData(handle_.get(), col, out_buf, out_len);
}

void SqlHandle::freeResult() {
    Sql_FreeResult(handle_.get());
}

void SqlHandle::showDebug_(const char* debug_file, const unsigned long debug_line) const {
    Sql_ShowDebug_(handle_.get(), debug_file, debug_line);
}

std::unique_ptr<ISqlStatement> SqlHandle::createStatement() {
    return std::make_unique<SqlStatement>(*handle_);
}
