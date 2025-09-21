#pragma once

#include <memory>

#include "ISqlHandle.hpp"
#include "ISqlStatement.hpp"
#include "sql.hpp"

class SqlHandle : public ISqlHandle {
public:
    SqlHandle();
    
    uint32_t getError() const override;
    int32_t connect(const char* user, const char* passwd, const char* host, uint16_t port, const char* db) override;
    int32_t setEncoding(const char* encoding) override;
    int32_t getTimeout(uint32_t* out_timeout) const override;
    int32_t getColumnNames(const char* table, char* out_buf, size_t buf_len, char sep) const override;
    int32_t ping() override;
    std::string escapeString(std::string_view from) const override;
    int32_t query(const char* query, ...) override;
    int32_t queryStr(const char* query) override;
    uint64_t lastInsertId() const override;
    uint32_t numColumns() const override;
    uint64_t numRows() const override;
    uint64_t numRowsAffected() const override;
    int32_t nextRow() override;
    int32_t getData(size_t col, char** out_buf, size_t* out_len) const override;
    void freeResult() override;
    void showDebug_(const char* debug_file, const unsigned long debug_line) const override;
    std::unique_ptr<ISqlStatement> createStatement() override;


private:
    std::unique_ptr<Sql, void(*)(Sql*)> handle_;
};
