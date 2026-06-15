add_library(ipbandb)


target_sources(ipbandb PRIVATE
    "IpBanDbSql.cpp")

target_link_libraries(ipbandb PUBLIC
    common)
