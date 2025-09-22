#include "SqlStatement.hpp"
#include "sql.hpp"

SqlStatement::SqlStatement(Sql& sql) : stmt_(std::make_unique<SqlStmt>(sql)) {

}

int32_t SqlStatement::prepare(const char* query, ...) {
    int32_t res;
    va_list args;
    va_start(args, query);
    res = stmt_->PrepareV(query, args);
    va_end(args);
    return res;
}

int32_t SqlStatement::prepareStr(const char* query) {
    return stmt_->PrepareStr(query);
}

size_t SqlStatement::numParams() {
    return stmt_->NumParams();
}

int32_t SqlStatement::bindParam(size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
    return stmt_->BindParam(idx, buffer_type, buffer, buffer_len);
}

int32_t SqlStatement::execute() {
    return stmt_->Execute();
}

uint64_t SqlStatement::lastInsertId() {
    return stmt_->LastInsertId();
}

size_t SqlStatement::numColumns() {
    return stmt_->NumColumns();
}

int32_t SqlStatement::bindColumn(size_t idx, enum SqlDataType buffer_type, void* buffer, size_t buffer_len) {
    return stmt_->BindColumn(idx, buffer_type, buffer, buffer_len);
}

void SqlStatement::freeResult() {
    stmt_->FreeResult();
}

void SqlStatement::showDebug_(const char* debug_file, const unsigned long debug_line) const {
    stmt_->ShowDebug_(debug_file, debug_line);
}
