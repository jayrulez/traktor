# Traktor.Html

set(TRAKTOR_HTML_SOURCES
    Attribute.cpp
    Comment.cpp
    Document.cpp
    Element.cpp
    HtmlClassFactory.cpp
    Module.cpp
    Node.cpp
    Text.cpp
)

add_traktor_module(Traktor.Html
    SOURCES ${TRAKTOR_HTML_SOURCES}
    EXPORT_DEFINE T_HTML_EXPORT
    DEPENDENCIES
        Traktor.Core
)

target_include_directories(Traktor.Html PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)