# Traktor.Weather

set(TRAKTOR_WEATHER_SOURCES
	Module.h
	Precipitation/PrecipitationComponent.h
	Precipitation/PrecipitationComponentData.h
	Precipitation/PrecipitationRenderer.h
	Sky/SkyComponent.h
	Sky/SkyComponentData.h
	Sky/SkyRenderer.h
	WeatherFactory.h
	Module.cpp
	Precipitation/PrecipitationComponent.cpp
	Precipitation/PrecipitationComponentData.cpp
	Precipitation/PrecipitationRenderer.cpp
	Sky/SkyComponent.cpp
	Sky/SkyComponentData.cpp
	Sky/SkyRenderer.cpp
	WeatherFactory.cpp
)

add_traktor_module(Traktor.Weather
    SOURCES ${TRAKTOR_WEATHER_SOURCES}
    EXPORT_DEFINE T_WEATHER_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Render
        Traktor.Resource
        Traktor.World
        Traktor.Mesh
)

target_include_directories(Traktor.Weather PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Weather.Editor - Editor support

set(TRAKTOR_WEATHER_EDITOR_SOURCES
	Editor/WeatherPipeline.h
	Editor/WeatherPipeline.cpp
)

add_traktor_module(Traktor.Weather.Editor
    SOURCES ${TRAKTOR_WEATHER_EDITOR_SOURCES}
    EXPORT_DEFINE T_WEATHER_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Weather
        Traktor.World.Editor
        Traktor.Editor
        Traktor.Drawing
)

target_include_directories(Traktor.Weather.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)