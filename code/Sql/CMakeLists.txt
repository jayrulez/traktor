# Traktor.Sql

set(TRAKTOR_SQL_SOURCES
    IConnection.cpp
    IResultSet.cpp
    Module.cpp
    SqlClassFactory.cpp
)

add_traktor_module(Traktor.Sql
    SOURCES ${TRAKTOR_SQL_SOURCES}
    EXPORT_DEFINE T_SQL_EXPORT
    DEPENDENCIES
        Traktor.Core
)

target_include_directories(Traktor.Sql PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Sql.Sqlite3

set(TRAKTOR_SQL_SQLITE3_SOURCES
    Sqlite3/ConnectionSqlite3.cpp
    Sqlite3/Module.cpp
    Sqlite3/ResultSetSqlite3.cpp
    Sqlite3/Sqlite3ClassFactory.cpp
)

add_traktor_module(Traktor.Sql.Sqlite3
    SOURCES ${TRAKTOR_SQL_SQLITE3_SOURCES}
    EXPORT_DEFINE T_SQL_SQLITE3_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Sql
        Extern.sqlite
)

target_include_directories(Traktor.Sql.Sqlite3 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)