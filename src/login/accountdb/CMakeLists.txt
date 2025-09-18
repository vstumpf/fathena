add_library(accountdb)


target_sources(accountdb PRIVATE
    "AccountDbSql.cpp")

target_link_libraries(accountdb PUBLIC
    common)
