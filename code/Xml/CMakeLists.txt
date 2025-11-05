# Traktor.Xml

set(TRAKTOR_XML_SOURCES
    Attribute.cpp
    Document.cpp
    Element.cpp
    Module.cpp
    Node.cpp
    Text.cpp
    XmlClassFactory.cpp
    XmlDeserializer.cpp
    XmlPullParser.cpp
    XmlSerializer.cpp
)

add_traktor_module(Traktor.Xml
    SOURCES ${TRAKTOR_XML_SOURCES}
    EXPORT_DEFINE T_XML_EXPORT
    DEPENDENCIES
        Traktor.Core
        Extern.expat
)

target_include_directories(Traktor.Xml PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)