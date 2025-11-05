# Traktor.Compress

set(TRAKTOR_COMPRESS_SOURCES
	ClassFactory.h
	Lzf/DeflateStreamLzf.h
	Lzf/InflateStreamLzf.h
	Module.h
	Test/CaseLzf.h
	Test/CaseZip.h
	Zip/DeflateStreamZip.h
	Zip/InflateStreamZip.h
	Zip/ZipVolume.h
	ClassFactory.cpp
	Lzf/DeflateStreamLzf.cpp
	Lzf/InflateStreamLzf.cpp
	Module.cpp
	Test/CaseLzf.cpp
	Test/CaseZip.cpp
	Zip/DeflateStreamZip.cpp
	Zip/InflateStreamZip.cpp
	Zip/ZipVolume.cpp
)

add_traktor_module(Traktor.Compress
    SOURCES ${TRAKTOR_COMPRESS_SOURCES}
    EXPORT_DEFINE T_COMPRESS_EXPORT
    DEPENDENCIES
        Traktor.Core
        Extern.liblzf
        Extern.zlib
)

target_include_directories(Traktor.Compress PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)