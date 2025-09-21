#pragma once

#include <memory>

#include "ISqlHandle.hpp"
#include "ISqlStatement.hpp"
#include "sql.hpp"


class SqlStatement : public ISqlStatement {
public:
    explicit SqlStatement(Sql& sql);
    int32_t prepare(const char* query, ...) override;
    int32_t prepareStr(const char* query) override;
    size_t numParams() override;
    int32_t bindParam(size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) override;
    int32_t execute() override;
    uint64_t lastInsertId() override;
    size_t numColumns() override;
    int32_t bindColumn(size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) override;
    void freeResult() override;
    void showDebug_(const char* debug_file, const unsigned long debug_line) const override;

private:
    std::unique_ptr<SqlStmt> stmt_{nullptr};
};
