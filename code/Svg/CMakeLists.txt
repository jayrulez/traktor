# Traktor.Svg

set(TRAKTOR_SVG_SOURCES
    ClassFactory.cpp
    Document.cpp
    Gradient.cpp
    IShapeVisitor.cpp
    ImageShape.cpp
    Module.cpp
    Parser.cpp
    Path.cpp
    PathShape.cpp
    Rasterizer.cpp
    Shape.cpp
    Style.cpp
    TextShape.cpp
)

add_traktor_module(Traktor.Svg
    SOURCES ${TRAKTOR_SVG_SOURCES}
    EXPORT_DEFINE T_SVG_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Xml
        Traktor.Drawing
)

target_include_directories(Traktor.Svg PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)