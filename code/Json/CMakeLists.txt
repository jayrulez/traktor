# Traktor.Json

set(TRAKTOR_JSON_SOURCES
	JsonArray.h
	JsonClassFactory.h
	JsonDocument.h
	JsonMember.h
	JsonNode.h
	JsonObject.h
	Module.h
	JsonArray.cpp
	JsonClassFactory.cpp
	JsonDocument.cpp
	JsonMember.cpp
	JsonNode.cpp
	JsonObject.cpp
	Module.cpp
)

add_traktor_module(Traktor.Json
    SOURCES ${TRAKTOR_JSON_SOURCES}
    EXPORT_DEFINE T_JSON_EXPORT
    DEPENDENCIES
        Traktor.Core
        Extern.rapidjson
)

target_include_directories(Traktor.Json PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)