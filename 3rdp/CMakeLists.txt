# External dependencies
# Based on paths from scripts/config.bat and scripts/config.sh

# Include CMake modules needed for configuration
include(CheckIncludeFile)
include(CheckFunctionExists)
include(TestBigEndian)

# Set TRAKTOR_HOME
set(TRAKTOR_HOME ${CMAKE_SOURCE_DIR})

# Define SDK paths matching scripts/config.bat and scripts/config.sh
# These variables are made available globally for modules to use in their include paths
set(AGG_SDK ${CMAKE_CURRENT_SOURCE_DIR}/agg-2.6 CACHE PATH "AGG SDK path")
set(ASTCENC_SDK ${CMAKE_CURRENT_SOURCE_DIR}/astc-encoder CACHE PATH "ASTC Encoder SDK path")
set(BULLET_SDK ${CMAKE_CURRENT_SOURCE_DIR}/bullet3 CACHE PATH "Bullet Physics SDK path")
set(EXPAT_SDK ${CMAKE_CURRENT_SOURCE_DIR}/libexpat/expat CACHE PATH "Expat SDK path")
set(FLAC_SDK ${CMAKE_CURRENT_SOURCE_DIR}/flac CACHE PATH "FLAC SDK path")
set(FREETYPE_SDK ${CMAKE_CURRENT_SOURCE_DIR}/freetype CACHE PATH "FreeType SDK path")
set(JPEG_SDK ${CMAKE_CURRENT_SOURCE_DIR}/jpeg/jpeg-9e CACHE PATH "JPEG SDK path")
set(LUA_SDK ${CMAKE_CURRENT_SOURCE_DIR}/lua/lua-5.4.8 CACHE PATH "Lua SDK path")
set(LZF_SDK ${CMAKE_CURRENT_SOURCE_DIR}/liblzf/liblzf-3.6 CACHE PATH "LZF SDK path")
set(MIKKTSPACE_SDK ${CMAKE_CURRENT_SOURCE_DIR}/MikkTSpace CACHE PATH "MikkTSpace SDK path")
set(MIMALLOC_SDK ${CMAKE_CURRENT_SOURCE_DIR}/mimalloc CACHE PATH "mimalloc SDK path")
set(OGG_SDK ${CMAKE_CURRENT_SOURCE_DIR}/ogg CACHE PATH "OGG SDK path")
set(PNG_SDK ${CMAKE_CURRENT_SOURCE_DIR}/libpng CACHE PATH "libpng SDK path")
set(RAPIDJSON_SDK ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson CACHE PATH "RapidJSON SDK path")
set(RECAST_SDK ${CMAKE_CURRENT_SOURCE_DIR}/recastnavigation CACHE PATH "Recast SDK path")
set(RG_ETC1_SDK ${CMAKE_CURRENT_SOURCE_DIR}/rg-etc1 CACHE PATH "RG ETC1 SDK path")
set(STB_SDK ${CMAKE_CURRENT_SOURCE_DIR}/stb CACHE PATH "STB SDK path")
set(SQLITE_SDK ${CMAKE_CURRENT_SOURCE_DIR}/sqlite/sqlite-amalgamation-3390200 CACHE PATH "SQLite SDK path")
set(SQUISH_SDK ${CMAKE_CURRENT_SOURCE_DIR}/squish CACHE PATH "Squish SDK path")
set(THEORA_SDK ${CMAKE_CURRENT_SOURCE_DIR}/theora CACHE PATH "Theora SDK path")
set(TINYEXR_SDK ${CMAKE_CURRENT_SOURCE_DIR}/tinyexr CACHE PATH "TinyEXR SDK path")
set(UFBX_SDK ${CMAKE_CURRENT_SOURCE_DIR}/ufbx CACHE PATH "UFBX SDK path")
set(VORBIS_SDK ${CMAKE_CURRENT_SOURCE_DIR}/vorbis CACHE PATH "Vorbis SDK path")
set(XATLAS_SDK ${CMAKE_CURRENT_SOURCE_DIR}/xatlas CACHE PATH "xatlas SDK path")
set(ZLIB_SDK ${CMAKE_CURRENT_SOURCE_DIR}/zlib CACHE PATH "zlib SDK path")
set(BC6H_ENC_SDK ${CMAKE_CURRENT_SOURCE_DIR}/bc6h_enc CACHE PATH "BC6H encoder SDK path")
set(MINIMP3_SDK ${CMAKE_CURRENT_SOURCE_DIR}/minimp3 CACHE PATH "minimp3 SDK path")
set(JOLT_SDK ${CMAKE_CURRENT_SOURCE_DIR}/JoltPhysics CACHE PATH "Jolt Physics SDK path")
set(VMA_SDK ${CMAKE_CURRENT_SOURCE_DIR}/VulkanMemoryAllocator CACHE PATH "Vulkan Memory Allocator SDK path")
set(SPIRVTOOLS_SDK ${CMAKE_CURRENT_SOURCE_DIR}/spirv-tools/install CACHE PATH "SPIRV-Tools SDK path")

# Platform-specific SDKs
if(WIN32)
    set(EMBREE_SDK ${CMAKE_CURRENT_SOURCE_DIR}/embree-windows CACHE PATH "Embree SDK path")
    set(RENDERDOC_SDK ${CMAKE_CURRENT_SOURCE_DIR}/renderdoc/RenderDoc_1.36_64 CACHE PATH "RenderDoc SDK path")
    set(VULKAN_SDK ${CMAKE_CURRENT_SOURCE_DIR}/vulkan-windows/vulkan-sdk CACHE PATH "Vulkan SDK path")
elseif(APPLE)
    set(EMBREE_SDK ${CMAKE_CURRENT_SOURCE_DIR}/embree-macOS CACHE PATH "Embree SDK path")
    set(RENDERDOC_SDK ${CMAKE_CURRENT_SOURCE_DIR}/renderdoc/renderdoc_1.36 CACHE PATH "RenderDoc SDK path")
    set(VULKAN_BASE_SDK ${CMAKE_CURRENT_SOURCE_DIR}/vulkan-linux/latest/x86_64 CACHE PATH "Vulkan base SDK path")
else()
    set(EMBREE_SDK ${CMAKE_CURRENT_SOURCE_DIR}/embree-linux CACHE PATH "Embree SDK path")
    set(RENDERDOC_SDK ${CMAKE_CURRENT_SOURCE_DIR}/renderdoc/renderdoc_1.36 CACHE PATH "RenderDoc SDK path")
    set(VULKAN_BASE_SDK ${CMAKE_CURRENT_SOURCE_DIR}/vulkan-linux/latest/x86_64 CACHE PATH "Vulkan base SDK path")
endif()

# Export all SDK paths to parent scope for use in other CMakeLists
set(AGG_SDK ${AGG_SDK} PARENT_SCOPE)
set(ASTCENC_SDK ${ASTCENC_SDK} PARENT_SCOPE)
set(BULLET_SDK ${BULLET_SDK} PARENT_SCOPE)
set(EXPAT_SDK ${EXPAT_SDK} PARENT_SCOPE)
set(FLAC_SDK ${FLAC_SDK} PARENT_SCOPE)
set(FREETYPE_SDK ${FREETYPE_SDK} PARENT_SCOPE)
set(JPEG_SDK ${JPEG_SDK} PARENT_SCOPE)
set(LUA_SDK ${LUA_SDK} PARENT_SCOPE)
set(LZF_SDK ${LZF_SDK} PARENT_SCOPE)
set(MIKKTSPACE_SDK ${MIKKTSPACE_SDK} PARENT_SCOPE)
set(MIMALLOC_SDK ${MIMALLOC_SDK} PARENT_SCOPE)
set(OGG_SDK ${OGG_SDK} PARENT_SCOPE)
set(PNG_SDK ${PNG_SDK} PARENT_SCOPE)
set(RAPIDJSON_SDK ${RAPIDJSON_SDK} PARENT_SCOPE)
set(RECAST_SDK ${RECAST_SDK} PARENT_SCOPE)
set(RG_ETC1_SDK ${RG_ETC1_SDK} PARENT_SCOPE)
set(STB_SDK ${STB_SDK} PARENT_SCOPE)
set(SQLITE_SDK ${SQLITE_SDK} PARENT_SCOPE)
set(SQUISH_SDK ${SQUISH_SDK} PARENT_SCOPE)
set(THEORA_SDK ${THEORA_SDK} PARENT_SCOPE)
set(TINYEXR_SDK ${TINYEXR_SDK} PARENT_SCOPE)
set(UFBX_SDK ${UFBX_SDK} PARENT_SCOPE)
set(VORBIS_SDK ${VORBIS_SDK} PARENT_SCOPE)
set(XATLAS_SDK ${XATLAS_SDK} PARENT_SCOPE)
set(ZLIB_SDK ${ZLIB_SDK} PARENT_SCOPE)
set(BC6H_ENC_SDK ${BC6H_ENC_SDK} PARENT_SCOPE)
set(MINIMP3_SDK ${MINIMP3_SDK} PARENT_SCOPE)
set(JOLT_SDK ${JOLT_SDK} PARENT_SCOPE)
set(VMA_SDK ${VMA_SDK} PARENT_SCOPE)
set(SPIRVTOOLS_SDK ${SPIRVTOOLS_SDK} PARENT_SCOPE)
set(EMBREE_SDK ${EMBREE_SDK} PARENT_SCOPE)
set(RENDERDOC_SDK ${RENDERDOC_SDK} PARENT_SCOPE)
set(VULKAN_SDK ${VULKAN_SDK} PARENT_SCOPE)
set(VULKAN_BASE_SDK ${VULKAN_BASE_SDK} PARENT_SCOPE)

# Disable warnings for third-party code
if(MSVC)
    add_compile_options(/W0)
else()
    add_compile_options(-w)
endif()

# ============================================================================
# mimalloc - Memory allocator
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/mimalloc/src)
    # Compile mimalloc source files directly (matching vcxproj)
    add_library(Extern.mimalloc STATIC
        mimalloc/src/alloc-aligned.c
        mimalloc/src/alloc-posix.c
        mimalloc/src/alloc.c
        mimalloc/src/arena.c
        mimalloc/src/bitmap.c
        mimalloc/src/heap.c
        mimalloc/src/init.c
        mimalloc/src/libc.c
        mimalloc/src/options.c
        mimalloc/src/os.c
        mimalloc/src/page.c
        mimalloc/src/random.c
        mimalloc/src/segment-map.c
        mimalloc/src/segment.c
        mimalloc/src/stats.c
        mimalloc/src/prim/prim.c
    )
    target_include_directories(Extern.mimalloc PUBLIC
        mimalloc/include
    )
    set_target_properties(Extern.mimalloc PROPERTIES OUTPUT_NAME "mimalloc")
endif()

# ============================================================================
# zlib - Compression library
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/zlib)
    # Only compile files that are in the vcxproj (excludes gz* files)
    add_library(Extern.zlib STATIC
        zlib/adler32.c
        zlib/compress.c
        zlib/crc32.c
        zlib/deflate.c
        zlib/inflate.c
        zlib/infback.c
        zlib/inftrees.c
        zlib/inffast.c
        zlib/trees.c
        zlib/uncompr.c
        zlib/zutil.c
    )
    target_include_directories(Extern.zlib PUBLIC zlib)

    # Add defines from vcxproj
    if(MSVC)
        target_compile_definitions(Extern.zlib PRIVATE
            _CRT_SECURE_NO_WARNINGS
            Z_SOLO
        )
    endif()

    set_target_properties(Extern.zlib PROPERTIES OUTPUT_NAME "zlib")
endif()

# ============================================================================
# libpng - PNG image library
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libpng)
    # Copy pnglibconf.h from patch directory
    # Using the configuration from resources/build/patch/libpng/pnglibconf.h
    # This file defines which PNG features are enabled/disabled
    configure_file(
        ${CMAKE_SOURCE_DIR}/resources/build/patch/libpng/pnglibconf.h
        ${CMAKE_CURRENT_BINARY_DIR}/libpng/pnglibconf.h
        COPYONLY
    )

    file(GLOB PNG_SOURCES libpng/*.c)
    # Include all .c files (example.c, pngtest.c, and all png*.c files)
    add_library(Extern.lpng STATIC ${PNG_SOURCES})

    target_include_directories(Extern.lpng PUBLIC
        libpng
        ${CMAKE_CURRENT_BINARY_DIR}/libpng  # Generated pnglibconf.h (must be PUBLIC)
    )

    target_link_libraries(Extern.lpng PUBLIC Extern.zlib)

    # Add _CRT_SECURE_NO_WARNINGS on Windows (from vcxproj)
    if(MSVC)
        target_compile_definitions(Extern.lpng PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()

    set_target_properties(Extern.lpng PROPERTIES OUTPUT_NAME "lpng")
endif()

# ============================================================================
# libjpeg - JPEG image library (jpeg/jpeg-9e)
# ============================================================================
set(JPEG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/jpeg/jpeg-9e)
if(EXISTS ${JPEG_DIR})
    # Generate jconfig.h based on platform
    # Using values from resources/build/patch/jpeg/jconfig.h (which is jconfig.vc)

    # Check for headers
    check_include_file(stddef.h HAVE_STDDEF_H)
    check_include_file(stdlib.h HAVE_STDLIB_H)

    # All modern compilers support these
    set(HAVE_PROTOTYPES 1)
    set(HAVE_UNSIGNED_CHAR 1)
    set(HAVE_UNSIGNED_SHORT 1)

    # Platform-specific settings
    if(WIN32)
        # Windows settings (from jconfig.vc)
        set(CHAR_IS_UNSIGNED 0)
        set(NEED_BSD_STRINGS 0)
        set(NEED_SYS_TYPES_H 0)
        set(NEED_FAR_POINTERS 0)
        set(NEED_SHORT_EXTERNAL_NAMES 0)
        set(INCOMPLETE_TYPES_BROKEN 0)
        set(RIGHT_SHIFT_IS_UNSIGNED 0)
    else()
        # Unix/Linux settings
        set(CHAR_IS_UNSIGNED 0)
        set(NEED_BSD_STRINGS 0)
        check_include_file(sys/types.h HAVE_SYS_TYPES_H_JPEG)
        if(HAVE_SYS_TYPES_H_JPEG)
            set(NEED_SYS_TYPES_H 1)
        else()
            set(NEED_SYS_TYPES_H 0)
        endif()
        set(NEED_FAR_POINTERS 0)
        set(NEED_SHORT_EXTERNAL_NAMES 0)
        set(INCOMPLETE_TYPES_BROKEN 0)
        set(RIGHT_SHIFT_IS_UNSIGNED 0)
    endif()

    # Generate jconfig.h from template
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/jpeg/jconfig.h
"/* jconfig.h - Generated by CMake */

#define HAVE_PROTOTYPES
#define HAVE_UNSIGNED_CHAR
#define HAVE_UNSIGNED_SHORT
#cmakedefine CHAR_IS_UNSIGNED
#cmakedefine01 HAVE_STDDEF_H
#cmakedefine01 HAVE_STDLIB_H
#cmakedefine NEED_BSD_STRINGS
#cmakedefine NEED_SYS_TYPES_H
#cmakedefine NEED_FAR_POINTERS
#cmakedefine NEED_SHORT_EXTERNAL_NAMES
#cmakedefine INCOMPLETE_TYPES_BROKEN

/* Define \"boolean\" as unsigned char, not enum, per Windows custom */
#ifndef __RPCNDR_H__
typedef unsigned char boolean;
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#define HAVE_BOOLEAN

#ifdef JPEG_INTERNALS
#cmakedefine RIGHT_SHIFT_IS_UNSIGNED
#endif /* JPEG_INTERNALS */
")

    # Now configure it with actual values
    configure_file(
        ${CMAKE_CURRENT_BINARY_DIR}/jpeg/jconfig.h
        ${CMAKE_CURRENT_BINARY_DIR}/jpeg/jconfig.h.tmp
        @ONLY
    )
    file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/jpeg/jconfig.h.tmp ${CMAKE_CURRENT_BINARY_DIR}/jpeg/jconfig.h)

    file(GLOB JPEG_SOURCES ${JPEG_DIR}/*.c)
    # Exclude utility programs, file I/O helpers, and platform-specific memory managers
    # Keep: j*.c library files, jmemansi.c, jmemmgr.c, cdjpeg.c, transupp.c, jpegtran.c, rdswitch.c
    # Exclude: main programs (cjpeg, djpeg), alternatives (*alt), file I/O (rd*, wr*), utilities
    # Note: rdswitch.c must be kept because jpegtran.c depends on read_scan_script()
    list(FILTER JPEG_SOURCES EXCLUDE REGEX "/(cjpeg|djpeg|example|ansi2knr|rdjpgcom|wrjpgcom|ckconfig|rdbmp|rdcolmap|rdgif|rdppm|rdrle|rdtarga|wrbmp|wrgif|wrppm|wrrle|wrtarga|jmemdos|jmemmac|jmemname|jmemnobs)|\\.alt\\.c$")
    add_library(Extern.jpeg STATIC ${JPEG_SOURCES})

    target_include_directories(Extern.jpeg PUBLIC
        ${JPEG_DIR}
    )

    target_include_directories(Extern.jpeg PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/jpeg  # Generated jconfig.h
    )

    # Add _CRT_SECURE_NO_WARNINGS on Windows (from vcxproj)
    if(MSVC)
        target_compile_definitions(Extern.jpeg PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()

    set_target_properties(Extern.jpeg PROPERTIES OUTPUT_NAME "jpeg")
endif()

# ============================================================================
# expat - XML parser library (libexpat/expat)
# ============================================================================
set(EXPAT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libexpat/expat)
if(EXISTS ${EXPAT_DIR}/lib)
    # Check for required headers
    check_include_file(dlfcn.h HAVE_DLFCN_H)
    check_include_file(fcntl.h HAVE_FCNTL_H)
    check_include_file(inttypes.h HAVE_INTTYPES_H)
    check_include_file(memory.h HAVE_MEMORY_H)
    check_include_file(stdint.h HAVE_STDINT_H)
    check_include_file(stdlib.h HAVE_STDLIB_H)
    check_include_file(strings.h HAVE_STRINGS_H)
    check_include_file(string.h HAVE_STRING_H)
    check_include_file(sys/stat.h HAVE_SYS_STAT_H)
    check_include_file(sys/types.h HAVE_SYS_TYPES_H)
    check_include_file(unistd.h HAVE_UNISTD_H)

    # Check for functions
    check_function_exists(getpagesize HAVE_GETPAGESIZE)
    check_function_exists(mmap HAVE_MMAP)

    # Determine byte order
    test_big_endian(IS_BIG_ENDIAN)
    if(IS_BIG_ENDIAN)
        set(BYTEORDER 4321)
        set(WORDS_BIGENDIAN ON)
    else()
        set(BYTEORDER 1234)
    endif()

    # Set expat build options
    set(XML_CONTEXT_BYTES 1024)  # Context buffer size
    set(XML_DTD ON)              # Enable DTD support
    set(XML_GE 1)                # Enable general entity support (1=enabled, 0=disabled)
    set(XML_NS ON)               # Enable XML namespace support

    # Package information
    set(PACKAGE_NAME "expat")
    set(PACKAGE_VERSION "2.5.0")
    set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
    set(PACKAGE_BUGREPORT "expat-bugs@libexpat.org")
    set(PACKAGE_TARNAME "expat")

    # Configure expat_config.h
    configure_file(
        ${EXPAT_DIR}/expat_config.h.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/expat_config.h
        @ONLY
    )

    # Build expat library
    add_library(Extern.expat STATIC
        ${EXPAT_DIR}/lib/xmlparse.c
        ${EXPAT_DIR}/lib/xmlrole.c
        ${EXPAT_DIR}/lib/xmltok.c
    )

    target_include_directories(Extern.expat PUBLIC
        ${EXPAT_DIR}/lib
        ${CMAKE_CURRENT_BINARY_DIR}  # For generated expat_config.h
    )

    target_compile_definitions(Extern.expat PRIVATE
        XML_STATIC
        XML_BUILDING_EXPAT
        XML_POOR_ENTROPY
        HAVE_MEMMOVE
        HAVE_EXPAT_CONFIG_H
    )

    set_target_properties(Extern.expat PROPERTIES OUTPUT_NAME "expat")
endif()

# ============================================================================
# Lua - Scripting language (lua/lua-5.4.8)
# ============================================================================
set(LUA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lua/lua-5.4.8)
if(EXISTS ${LUA_DIR}/src)
    file(GLOB LUA_SOURCES ${LUA_DIR}/src/l*.c)
    list(FILTER LUA_SOURCES EXCLUDE REGEX "lua.c|luac.c")
    add_library(Extern.lua STATIC ${LUA_SOURCES})
    target_include_directories(Extern.lua PUBLIC ${LUA_DIR}/src)
    set_target_properties(Extern.lua PROPERTIES OUTPUT_NAME "lua")
endif()

# ============================================================================
# SQLite - Database library (sqlite/sqlite-amalgamation-3390200)
# ============================================================================
set(SQLITE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sqlite/sqlite-amalgamation-3390200)
if(EXISTS ${SQLITE_DIR}/sqlite3.c)
    add_library(Extern.sqlite STATIC ${SQLITE_DIR}/sqlite3.c)
    target_include_directories(Extern.sqlite PUBLIC ${SQLITE_DIR})

    # Enable column metadata API (required by Traktor)
    target_compile_definitions(Extern.sqlite PUBLIC SQLITE_ENABLE_COLUMN_METADATA)

    # Disable warnings for sqlite
    if(MSVC)
        target_compile_options(Extern.sqlite PRIVATE /W0)
    endif()

    set_target_properties(Extern.sqlite PROPERTIES OUTPUT_NAME "sqlite")
endif()

# ============================================================================
# OGG - Audio container format
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ogg/src)
    # Check for standard integer type headers
    check_include_file(inttypes.h HAVE_INTTYPES_H_OGG)
    check_include_file(stdint.h HAVE_STDINT_H_OGG)
    check_include_file(sys/types.h HAVE_SYS_TYPES_H_OGG)

    # Set include flags
    if(HAVE_INTTYPES_H_OGG)
        set(INCLUDE_INTTYPES_H 1)
    else()
        set(INCLUDE_INTTYPES_H 0)
    endif()

    if(HAVE_STDINT_H_OGG)
        set(INCLUDE_STDINT_H 1)
    else()
        set(INCLUDE_STDINT_H 0)
    endif()

    if(HAVE_SYS_TYPES_H_OGG)
        set(INCLUDE_SYS_TYPES_H 1)
    else()
        set(INCLUDE_SYS_TYPES_H 0)
    endif()

    # Set type sizes for OGG
    set(SIZE16 "int16_t")
    set(USIZE16 "uint16_t")
    set(SIZE32 "int32_t")
    set(USIZE32 "uint32_t")
    set(SIZE64 "int64_t")
    set(USIZE64 "uint64_t")

    # Generate config_types.h
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/ogg/include/ogg/config_types.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/ogg/include/ogg/config_types.h
        @ONLY
    )

    file(GLOB OGG_SOURCES ogg/src/*.c)
    add_library(Extern.ogg STATIC ${OGG_SOURCES})
    target_include_directories(Extern.ogg PUBLIC
        ogg/include
        ${CMAKE_CURRENT_BINARY_DIR}/ogg/include  # For generated config_types.h
    )
    set_target_properties(Extern.ogg PROPERTIES OUTPUT_NAME "ogg")
endif()

# ============================================================================
# Vorbis - Audio codec
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vorbis/lib)
    file(GLOB VORBIS_SOURCES vorbis/lib/*.c)
    # Exclude utility/test files that aren't in the vcxproj
    list(FILTER VORBIS_SOURCES EXCLUDE REGEX "barkmel|psytune|tone|misc")
    add_library(Extern.vorbis STATIC ${VORBIS_SOURCES})
    target_include_directories(Extern.vorbis PUBLIC
        vorbis/include
        vorbis/lib
    )
    target_include_directories(Extern.vorbis PRIVATE
        ogg/include
        ${CMAKE_CURRENT_BINARY_DIR}/ogg/include
    )
    target_link_libraries(Extern.vorbis PUBLIC Extern.ogg)
    set_target_properties(Extern.vorbis PROPERTIES OUTPUT_NAME "vorbis")
endif()

# ============================================================================
# Theora - Video codec
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/theora/lib)
    file(GLOB THEORA_SOURCES theora/lib/*.c)
    # Exclude platform-specific files and encoder_disabled.c
    list(FILTER THEORA_SOURCES EXCLUDE REGEX "x86|arm|c64x|encoder_disabled")
    add_library(Extern.theora STATIC ${THEORA_SOURCES})
    target_include_directories(Extern.theora PUBLIC
        theora/include
    )
    target_include_directories(Extern.theora PRIVATE
        theora/lib
        ogg/include
        ${CMAKE_CURRENT_BINARY_DIR}/ogg/include
        vorbis/include
        vorbis/lib
    )
    target_link_libraries(Extern.theora PUBLIC Extern.ogg)
    # Don't define OC_X86_ASM at all - let it default to C implementation (oc_state_accel_init_c)
    # Defining OC_X86_ASM=0 still makes it "defined", triggering x86 code path without x86 files
    set_target_properties(Extern.theora PROPERTIES OUTPUT_NAME "theora")
endif()

# ============================================================================
# FLAC - Audio codec
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/flac/src/libFLAC)
    # Configure FLAC config.h from template
    # Matching values from resources/build/patch/flac/config.h

    # Detect platform endianness
    test_big_endian(IS_BIG_ENDIAN_FLAC)
    if(IS_BIG_ENDIAN_FLAC)
        set(CPU_IS_BIG_ENDIAN 1)
    else()
        set(CPU_IS_BIG_ENDIAN 0)
    endif()

    # FLAC build options (matching patch config.h)
    set(ENABLE_64_BIT_WORDS 0)  # Use 32-bit words
    set(OGG_FOUND 0)            # Not using OGG in FLAC (even though we include it)
    set(HAVE_LROUND 1)          # System has lround() function

    # Architecture-specific intrinsics
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|x86|i386|i686")
        set(FLAC__HAS_X86INTRIN 1)
        set(WITH_AVX ON)
    else()
        set(FLAC__HAS_X86INTRIN 0)
    endif()

    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64|ARM64")
        set(FLAC__HAS_NEONINTRIN 1)
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|ARM64")
            set(FLAC__HAS_A64NEONINTRIN 1)
        else()
            set(FLAC__HAS_A64NEONINTRIN 0)
        endif()
    else()
        set(FLAC__HAS_NEONINTRIN 0)
        set(FLAC__HAS_A64NEONINTRIN 0)
    endif()

    # Check for headers
    check_include_file(inttypes.h HAVE_INTTYPES_H)
    check_include_file(stdint.h HAVE_STDINT_H)
    check_include_file(string.h HAVE_STRING_H)
    check_include_file(byteswap.h HAVE_BYTESWAP_H)
    check_include_file(sys/ioctl.h HAVE_SYS_IOCTL_H)
    check_include_file(termios.h HAVE_TERMIOS_H)
    check_include_file(x86intrin.h HAVE_X86INTRIN_H)

    # Check for functions
    check_function_exists(clock_gettime HAVE_CLOCK_GETTIME)
    check_function_exists(fseeko HAVE_FSEEKO)

    # Package information
    set(PROJECT_VERSION "1.3.4")
    set(PACKAGE_VERSION "1.3.4")
    set(GIT_COMMIT_DATE "20220813")
    set(GIT_COMMIT_HASH "010c60e")

    # Platform detection
    if(APPLE)
        set(FLAC__SYS_DARWIN ON)
    elseif(UNIX AND NOT APPLE)
        set(FLAC__SYS_LINUX ON)
    endif()

    # Generate config.h from template
    configure_file(
        flac/config.cmake.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/flac/config.h
        @ONLY
    )

    # Only compile files that are in the vcxproj (no subdirectories!)
    add_library(Extern.flac STATIC
        flac/src/libFLAC/bitmath.c
        flac/src/libFLAC/bitreader.c
        flac/src/libFLAC/bitwriter.c
        flac/src/libFLAC/cpu.c
        flac/src/libFLAC/crc.c
        flac/src/libFLAC/fixed.c
        flac/src/libFLAC/float.c
        flac/src/libFLAC/format.c
        flac/src/libFLAC/lpc.c
        flac/src/libFLAC/md5.c
        flac/src/libFLAC/memory.c
        flac/src/libFLAC/metadata_iterators.c
        flac/src/libFLAC/metadata_object.c
        flac/src/libFLAC/stream_decoder.c
        flac/src/libFLAC/stream_encoder.c
        flac/src/libFLAC/stream_encoder_framing.c
        flac/src/libFLAC/window.c
        flac/src/share/win_utf8_io/win_utf8_io.c
    )

    # Add includes
    target_include_directories(Extern.flac PUBLIC
        flac/include
        flac/src/libFLAC/include
    )

    # Add generated config.h and OGG includes
    target_include_directories(Extern.flac PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/flac  # Generated config.h
        ogg/include
        ${CMAKE_CURRENT_BINARY_DIR}/ogg/include
    )

    # Add FLAC-specific defines
    # FLAC__NO_DLL must be PUBLIC so consumers know to link statically
    target_compile_definitions(Extern.flac PUBLIC
        FLAC__NO_DLL
    )

    target_compile_definitions(Extern.flac PRIVATE
        FLAC__NO_ASM
        HAVE_CONFIG_H
        HAVE_LROUND
    )

    # Link with OGG if needed
    if(TARGET Extern.ogg)
        target_link_libraries(Extern.flac PRIVATE Extern.ogg)
    endif()

    set_target_properties(Extern.flac PROPERTIES OUTPUT_NAME "flac")
endif()

# ============================================================================
# FreeType - Font rendering library
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/freetype/src)
    file(GLOB FREETYPE_SOURCES
        freetype/src/autofit/autofit.c
        freetype/src/base/ftbase.c
        freetype/src/base/ftbbox.c
        freetype/src/base/ftbdf.c
        freetype/src/base/ftbitmap.c
        freetype/src/base/ftcid.c
        freetype/src/base/ftdebug.c
        freetype/src/base/ftfstype.c
        freetype/src/base/ftgasp.c
        freetype/src/base/ftglyph.c
        freetype/src/base/ftgxval.c
        freetype/src/base/ftinit.c
        freetype/src/base/ftmm.c
        freetype/src/base/ftotval.c
        freetype/src/base/ftpatent.c
        freetype/src/base/ftpfr.c
        freetype/src/base/ftstroke.c
        freetype/src/base/ftsynth.c
        freetype/src/base/ftsystem.c
        freetype/src/base/fttype1.c
        freetype/src/base/ftwinfnt.c
        freetype/src/bdf/bdf.c
        freetype/src/cache/ftcache.c
        freetype/src/cff/cff.c
        freetype/src/cid/type1cid.c
        freetype/src/gzip/ftgzip.c
        freetype/src/lzw/ftlzw.c
        freetype/src/pcf/pcf.c
        freetype/src/pfr/pfr.c
        freetype/src/psaux/psaux.c
        freetype/src/pshinter/pshinter.c
        freetype/src/psnames/psmodule.c
        freetype/src/raster/raster.c
        freetype/src/sdf/sdf.c
        freetype/src/sfnt/sfnt.c
        freetype/src/smooth/smooth.c
        freetype/src/svg/svg.c
        freetype/src/truetype/truetype.c
        freetype/src/type1/type1.c
        freetype/src/type42/type42.c
        freetype/src/winfonts/winfnt.c
    )
    add_library(Extern.freetype STATIC ${FREETYPE_SOURCES})
    target_include_directories(Extern.freetype PUBLIC freetype/include)
    target_compile_definitions(Extern.freetype PRIVATE FT2_BUILD_LIBRARY)
    set_target_properties(Extern.freetype PROPERTIES OUTPUT_NAME "freetype")
endif()

# ============================================================================
# AGG - Anti-Grain Geometry (agg-2.6)
# ============================================================================
set(AGG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/agg-2.6/agg-src)
if(EXISTS ${AGG_DIR}/src)
    file(GLOB AGG_SOURCES ${AGG_DIR}/src/*.cpp)
    add_library(Extern.agg STATIC ${AGG_SOURCES})
    target_include_directories(Extern.agg PUBLIC
        ${AGG_DIR}/include
        ${AGG_DIR}/gpc
    )
    set_target_properties(Extern.agg PROPERTIES OUTPUT_NAME "agg")
endif()

# ============================================================================
# liblzf - Compression library (liblzf/liblzf-3.6)
# ============================================================================
set(LZF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/liblzf/liblzf-3.6)
if(EXISTS ${LZF_DIR})
    add_library(Extern.liblzf STATIC
        ${LZF_DIR}/lzf_c.c
        ${LZF_DIR}/lzf_d.c
    )
    target_include_directories(Extern.liblzf PUBLIC ${LZF_DIR})
    set_target_properties(Extern.liblzf PROPERTIES OUTPUT_NAME "liblzf")
endif()

# ============================================================================
# squish - DXT compression
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/squish)
    file(GLOB SQUISH_SOURCES squish/*.cpp)
    add_library(Extern.squish STATIC ${SQUISH_SOURCES})
    target_include_directories(Extern.squish PUBLIC squish)
    set_target_properties(Extern.squish PROPERTIES OUTPUT_NAME "squish")
endif()

# ============================================================================
# rg-etc1 - ETC1 compression
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/rg-etc1/rg_etc1.cpp)
    add_library(Extern.rg_etc1 STATIC rg-etc1/rg_etc1.cpp)
    target_include_directories(Extern.rg_etc1 PUBLIC rg-etc1)
    set_target_properties(Extern.rg_etc1 PROPERTIES OUTPUT_NAME "rg_etc1")
endif()

# ============================================================================
# MikkTSpace - Tangent space calculation
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/MikkTSpace/mikktspace.c)
    add_library(Extern.MikkTSpace STATIC MikkTSpace/mikktspace.c)
    target_include_directories(Extern.MikkTSpace PUBLIC MikkTSpace)
    set_target_properties(Extern.MikkTSpace PROPERTIES OUTPUT_NAME "MikkTSpace")
endif()

# ============================================================================
# stb - Single-file libraries
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/stb)
    # STB is mostly header-only, but stb_vorbis.c needs to be compiled (from vcxproj)
    add_library(Extern.stb STATIC
        stb/stb_vorbis.c
    )

    target_include_directories(Extern.stb PUBLIC
        stb
    )

    # Disable all warnings for stb (from vcxproj)
    if(MSVC)
        target_compile_options(Extern.stb PRIVATE /W0)
    else()
        target_compile_options(Extern.stb PRIVATE -w)
    endif()

    set_target_properties(Extern.stb PROPERTIES OUTPUT_NAME "Extern.stb")
endif()

# ============================================================================
# tinyexr - EXR image format
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tinyexr)
    add_library(Extern.tinyexr STATIC
        tinyexr/tinyexr.cc
        tinyexr/deps/miniz/miniz.c
    )

    target_include_directories(Extern.tinyexr PUBLIC
        tinyexr
        tinyexr/deps/miniz
    )

    # Disable warnings for tinyexr
    if(MSVC)
        target_compile_options(Extern.tinyexr PRIVATE /W3)
    else()
        target_compile_options(Extern.tinyexr PRIVATE -w)
    endif()

    set_target_properties(Extern.tinyexr PROPERTIES OUTPUT_NAME "Extern.tinyexr")
endif()

# ============================================================================
# ufbx - FBX loader
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ufbx)
    add_library(Extern.ufbx STATIC
        ufbx/ufbx.c
    )

    target_include_directories(Extern.ufbx PUBLIC
        ufbx
    )

    # Disable warnings for ufbx
    if(MSVC)
        target_compile_options(Extern.ufbx PRIVATE /W3)
    else()
        target_compile_options(Extern.ufbx PRIVATE -w)
    endif()

    set_target_properties(Extern.ufbx PROPERTIES OUTPUT_NAME "Extern.ufbx")
endif()

# ============================================================================
# minimp3 - MP3 decoder
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/minimp3)
    add_library(Extern.minimp3 INTERFACE)
    target_include_directories(Extern.minimp3 INTERFACE minimp3)
endif()

# ============================================================================
# RapidJSON - JSON library
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include)
    add_library(Extern.rapidjson INTERFACE)
    target_include_directories(Extern.rapidjson INTERFACE rapidjson/include)
endif()

# ============================================================================
# xatlas - UV atlas generation
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/xatlas/source/xatlas)
    file(GLOB XATLAS_SOURCES xatlas/source/xatlas/*.cpp)
    if(XATLAS_SOURCES)
        add_library(Extern.xatlas STATIC ${XATLAS_SOURCES})
        target_include_directories(Extern.xatlas PUBLIC xatlas/source/xatlas)
        set_target_properties(Extern.xatlas PROPERTIES OUTPUT_NAME "xatlas")
    endif()
endif()

# ============================================================================
# VulkanMemoryAllocator - GPU memory allocator
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/VulkanMemoryAllocator/include)
    add_library(Extern.VulkanMemoryAllocator INTERFACE)
    target_include_directories(Extern.VulkanMemoryAllocator INTERFACE
        VulkanMemoryAllocator/include
    )
endif()

# ============================================================================
# Bullet Physics (bullet3)
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bullet3/src)
    file(GLOB_RECURSE BULLET_SOURCES
        bullet3/src/BulletCollision/*.cpp
        bullet3/src/BulletDynamics/*.cpp
        bullet3/src/LinearMath/*.cpp
    )
    add_library(Extern.bullet STATIC ${BULLET_SOURCES})
    target_include_directories(Extern.bullet PUBLIC bullet3/src)
    set_target_properties(Extern.bullet PROPERTIES OUTPUT_NAME "bullet")
endif()

# ============================================================================
# Jolt Physics (JoltPhysics)
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/JoltPhysics/Jolt)
    # Use explicit file list matching the original vcxproj (not GLOB_RECURSE)
    # The original build only includes specific files, not all .cpp files

    # Header files (for IDE integration and dependency tracking)
    set(JOLT_HEADERS
        JoltPhysics/Jolt/ConfigurationString.h
        JoltPhysics/Jolt/Jolt.h
        JoltPhysics/Jolt/RegisterTypes.h
        # Core
        JoltPhysics/Jolt/Core/ARMNeon.h
        JoltPhysics/Jolt/Core/Array.h
        JoltPhysics/Jolt/Core/Atomics.h
        JoltPhysics/Jolt/Core/BinaryHeap.h
        JoltPhysics/Jolt/Core/ByteBuffer.h
        JoltPhysics/Jolt/Core/Color.h
        JoltPhysics/Jolt/Core/Core.h
        JoltPhysics/Jolt/Core/FPControlWord.h
        JoltPhysics/Jolt/Core/FPException.h
        JoltPhysics/Jolt/Core/FPFlushDenormals.h
        JoltPhysics/Jolt/Core/Factory.h
        JoltPhysics/Jolt/Core/FixedSizeFreeList.h
        JoltPhysics/Jolt/Core/HashCombine.h
        JoltPhysics/Jolt/Core/HashTable.h
        JoltPhysics/Jolt/Core/InsertionSort.h
        JoltPhysics/Jolt/Core/IssueReporting.h
        JoltPhysics/Jolt/Core/JobSystem.h
        JoltPhysics/Jolt/Core/JobSystemSingleThreaded.h
        JoltPhysics/Jolt/Core/JobSystemThreadPool.h
        JoltPhysics/Jolt/Core/JobSystemWithBarrier.h
        JoltPhysics/Jolt/Core/LinearCurve.h
        JoltPhysics/Jolt/Core/LockFreeHashMap.h
        JoltPhysics/Jolt/Core/Memory.h
        JoltPhysics/Jolt/Core/Mutex.h
        JoltPhysics/Jolt/Core/MutexArray.h
        JoltPhysics/Jolt/Core/NonCopyable.h
        JoltPhysics/Jolt/Core/Profiler.h
        JoltPhysics/Jolt/Core/QuickSort.h
        JoltPhysics/Jolt/Core/RTTI.h
        JoltPhysics/Jolt/Core/Reference.h
        JoltPhysics/Jolt/Core/Result.h
        JoltPhysics/Jolt/Core/STLAlignedAllocator.h
        JoltPhysics/Jolt/Core/STLAllocator.h
        JoltPhysics/Jolt/Core/STLLocalAllocator.h
        JoltPhysics/Jolt/Core/STLTempAllocator.h
        JoltPhysics/Jolt/Core/ScopeExit.h
        JoltPhysics/Jolt/Core/Semaphore.h
        JoltPhysics/Jolt/Core/StaticArray.h
        JoltPhysics/Jolt/Core/StreamIn.h
        JoltPhysics/Jolt/Core/StreamOut.h
        JoltPhysics/Jolt/Core/StreamUtils.h
        JoltPhysics/Jolt/Core/StreamWrapper.h
        JoltPhysics/Jolt/Core/StridedPtr.h
        JoltPhysics/Jolt/Core/StringTools.h
        JoltPhysics/Jolt/Core/TempAllocator.h
        JoltPhysics/Jolt/Core/TickCounter.h
        JoltPhysics/Jolt/Core/UnorderedMap.h
        JoltPhysics/Jolt/Core/UnorderedSet.h
        # Geometry
        JoltPhysics/Jolt/Geometry/AABox.h
        JoltPhysics/Jolt/Geometry/AABox4.h
        JoltPhysics/Jolt/Geometry/ClipPoly.h
        JoltPhysics/Jolt/Geometry/ClosestPoint.h
        JoltPhysics/Jolt/Geometry/ConvexHullBuilder.h
        JoltPhysics/Jolt/Geometry/ConvexHullBuilder2D.h
        JoltPhysics/Jolt/Geometry/ConvexSupport.h
        JoltPhysics/Jolt/Geometry/EPAConvexHullBuilder.h
        JoltPhysics/Jolt/Geometry/EPAPenetrationDepth.h
        JoltPhysics/Jolt/Geometry/Ellipse.h
        JoltPhysics/Jolt/Geometry/GJKClosestPoint.h
        JoltPhysics/Jolt/Geometry/IndexedTriangle.h
        JoltPhysics/Jolt/Geometry/Indexify.h
        JoltPhysics/Jolt/Geometry/MortonCode.h
        JoltPhysics/Jolt/Geometry/OrientedBox.h
        JoltPhysics/Jolt/Geometry/Plane.h
        JoltPhysics/Jolt/Geometry/RayAABox.h
        JoltPhysics/Jolt/Geometry/RayCapsule.h
        JoltPhysics/Jolt/Geometry/RayCylinder.h
        JoltPhysics/Jolt/Geometry/RaySphere.h
        JoltPhysics/Jolt/Geometry/RayTriangle.h
        JoltPhysics/Jolt/Geometry/Sphere.h
        JoltPhysics/Jolt/Geometry/Triangle.h
        # Math
        JoltPhysics/Jolt/Math/BVec16.h
        JoltPhysics/Jolt/Math/DMat44.h
        JoltPhysics/Jolt/Math/DVec3.h
        JoltPhysics/Jolt/Math/Double3.h
        JoltPhysics/Jolt/Math/DynMatrix.h
        JoltPhysics/Jolt/Math/EigenValueSymmetric.h
        JoltPhysics/Jolt/Math/FindRoot.h
        JoltPhysics/Jolt/Math/Float2.h
        JoltPhysics/Jolt/Math/Float3.h
        JoltPhysics/Jolt/Math/Float4.h
        JoltPhysics/Jolt/Math/GaussianElimination.h
        JoltPhysics/Jolt/Math/HalfFloat.h
        JoltPhysics/Jolt/Math/Mat44.h
        JoltPhysics/Jolt/Math/Math.h
        JoltPhysics/Jolt/Math/MathTypes.h
        JoltPhysics/Jolt/Math/Matrix.h
        JoltPhysics/Jolt/Math/Quat.h
        JoltPhysics/Jolt/Math/Real.h
        JoltPhysics/Jolt/Math/Swizzle.h
        JoltPhysics/Jolt/Math/Trigonometry.h
        JoltPhysics/Jolt/Math/UVec4.h
        JoltPhysics/Jolt/Math/Vec3.h
        JoltPhysics/Jolt/Math/Vec4.h
        JoltPhysics/Jolt/Math/Vector.h
        # Physics/Body
        JoltPhysics/Jolt/Physics/Body/AllowedDOFs.h
        JoltPhysics/Jolt/Physics/Body/Body.h
        JoltPhysics/Jolt/Physics/Body/BodyAccess.h
        JoltPhysics/Jolt/Physics/Body/BodyActivationListener.h
        JoltPhysics/Jolt/Physics/Body/BodyCreationSettings.h
        JoltPhysics/Jolt/Physics/Body/BodyFilter.h
        JoltPhysics/Jolt/Physics/Body/BodyID.h
        JoltPhysics/Jolt/Physics/Body/BodyInterface.h
        JoltPhysics/Jolt/Physics/Body/BodyLock.h
        JoltPhysics/Jolt/Physics/Body/BodyLockInterface.h
        JoltPhysics/Jolt/Physics/Body/BodyLockMulti.h
        JoltPhysics/Jolt/Physics/Body/BodyManager.h
        JoltPhysics/Jolt/Physics/Body/BodyPair.h
        JoltPhysics/Jolt/Physics/Body/BodyType.h
        JoltPhysics/Jolt/Physics/Body/MassProperties.h
        JoltPhysics/Jolt/Physics/Body/MotionProperties.h
        JoltPhysics/Jolt/Physics/Body/MotionQuality.h
        JoltPhysics/Jolt/Physics/Body/MotionType.h
        # Physics/Character
        JoltPhysics/Jolt/Physics/Character/Character.h
        JoltPhysics/Jolt/Physics/Character/CharacterBase.h
        JoltPhysics/Jolt/Physics/Character/CharacterID.h
        JoltPhysics/Jolt/Physics/Character/CharacterVirtual.h
        # Physics/Collision
        JoltPhysics/Jolt/Physics/Collision/AABoxCast.h
        JoltPhysics/Jolt/Physics/Collision/ActiveEdgeMode.h
        JoltPhysics/Jolt/Physics/Collision/ActiveEdges.h
        JoltPhysics/Jolt/Physics/Collision/BackFaceMode.h
        JoltPhysics/Jolt/Physics/Collision/CastConvexVsTriangles.h
        JoltPhysics/Jolt/Physics/Collision/CastResult.h
        JoltPhysics/Jolt/Physics/Collision/CastSphereVsTriangles.h
        JoltPhysics/Jolt/Physics/Collision/CollectFacesMode.h
        JoltPhysics/Jolt/Physics/Collision/CollideConvexVsTriangles.h
        JoltPhysics/Jolt/Physics/Collision/CollidePointResult.h
        JoltPhysics/Jolt/Physics/Collision/CollideShape.h
        JoltPhysics/Jolt/Physics/Collision/CollideShapeVsShapePerLeaf.h
        JoltPhysics/Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h
        JoltPhysics/Jolt/Physics/Collision/CollideSoftBodyVerticesVsTriangles.h
        JoltPhysics/Jolt/Physics/Collision/CollideSphereVsTriangles.h
        JoltPhysics/Jolt/Physics/Collision/CollisionCollector.h
        JoltPhysics/Jolt/Physics/Collision/CollisionCollectorImpl.h
        JoltPhysics/Jolt/Physics/Collision/CollisionDispatch.h
        JoltPhysics/Jolt/Physics/Collision/CollisionGroup.h
        JoltPhysics/Jolt/Physics/Collision/ContactListener.h
        JoltPhysics/Jolt/Physics/Collision/EstimateCollisionResponse.h
        JoltPhysics/Jolt/Physics/Collision/GroupFilter.h
        JoltPhysics/Jolt/Physics/Collision/GroupFilterTable.h
        JoltPhysics/Jolt/Physics/Collision/InternalEdgeRemovingCollector.h
        JoltPhysics/Jolt/Physics/Collision/ManifoldBetweenTwoFaces.h
        JoltPhysics/Jolt/Physics/Collision/NarrowPhaseQuery.h
        JoltPhysics/Jolt/Physics/Collision/NarrowPhaseStats.h
        JoltPhysics/Jolt/Physics/Collision/ObjectLayer.h
        JoltPhysics/Jolt/Physics/Collision/ObjectLayerPairFilterMask.h
        JoltPhysics/Jolt/Physics/Collision/ObjectLayerPairFilterTable.h
        JoltPhysics/Jolt/Physics/Collision/PhysicsMaterial.h
        JoltPhysics/Jolt/Physics/Collision/PhysicsMaterialSimple.h
        JoltPhysics/Jolt/Physics/Collision/RayCast.h
        JoltPhysics/Jolt/Physics/Collision/ShapeCast.h
        JoltPhysics/Jolt/Physics/Collision/ShapeFilter.h
        JoltPhysics/Jolt/Physics/Collision/SimShapeFilter.h
        JoltPhysics/Jolt/Physics/Collision/SimShapeFilterWrapper.h
        JoltPhysics/Jolt/Physics/Collision/SortReverseAndStore.h
        JoltPhysics/Jolt/Physics/Collision/TransformedShape.h
        # Physics/Collision/BroadPhase
        JoltPhysics/Jolt/Physics/Collision/BroadPhase/BroadPhase.h
        JoltPhysics/Jolt/Physics/Collision/BroadPhase/BroadPhaseBruteForce.h
        JoltPhysics/Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h
        JoltPhysics/Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceMask.h
        JoltPhysics/Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceTable.h
        JoltPhysics/Jolt/Physics/Collision/BroadPhase/BroadPhaseQuadTree.h
        JoltPhysics/Jolt/Physics/Collision/BroadPhase/BroadPhaseQuery.h
        JoltPhysics/Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterMask.h
        JoltPhysics/Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterTable.h
        JoltPhysics/Jolt/Physics/Collision/BroadPhase/QuadTree.h
        # Physics/Collision/Shape
        JoltPhysics/Jolt/Physics/Collision/Shape/BoxShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/CapsuleShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/CompoundShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/CompoundShapeVisitors.h
        JoltPhysics/Jolt/Physics/Collision/Shape/ConvexHullShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/ConvexShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/CylinderShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/DecoratedShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/EmptyShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/GetTrianglesContext.h
        JoltPhysics/Jolt/Physics/Collision/Shape/HeightFieldShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/MeshShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/MutableCompoundShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/PlaneShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/PolyhedronSubmergedVolumeCalculator.h
        JoltPhysics/Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/ScaleHelpers.h
        JoltPhysics/Jolt/Physics/Collision/Shape/ScaledShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/Shape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/SphereShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/StaticCompoundShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/SubShapeID.h
        JoltPhysics/Jolt/Physics/Collision/Shape/SubShapeIDPair.h
        JoltPhysics/Jolt/Physics/Collision/Shape/TaperedCapsuleShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/TaperedCylinderShape.h
        JoltPhysics/Jolt/Physics/Collision/Shape/TriangleShape.h
        # Physics/Ragdoll
        JoltPhysics/Jolt/Physics/Ragdoll/Ragdoll.h
        # Physics/SoftBody
        JoltPhysics/Jolt/Physics/SoftBody/SoftBodyContactListener.h
        JoltPhysics/Jolt/Physics/SoftBody/SoftBodyCreationSettings.h
        JoltPhysics/Jolt/Physics/SoftBody/SoftBodyManifold.h
        JoltPhysics/Jolt/Physics/SoftBody/SoftBodyMotionProperties.h
        JoltPhysics/Jolt/Physics/SoftBody/SoftBodyShape.h
        JoltPhysics/Jolt/Physics/SoftBody/SoftBodySharedSettings.h
        JoltPhysics/Jolt/Physics/SoftBody/SoftBodyUpdateContext.h
        JoltPhysics/Jolt/Physics/SoftBody/SoftBodyVertex.h
        # Physics/Vehicle
        JoltPhysics/Jolt/Physics/Vehicle/MotorcycleController.h
        JoltPhysics/Jolt/Physics/Vehicle/TrackedVehicleController.h
        JoltPhysics/Jolt/Physics/Vehicle/VehicleAntiRollBar.h
        JoltPhysics/Jolt/Physics/Vehicle/VehicleCollisionTester.h
        JoltPhysics/Jolt/Physics/Vehicle/VehicleConstraint.h
        JoltPhysics/Jolt/Physics/Vehicle/VehicleController.h
        JoltPhysics/Jolt/Physics/Vehicle/VehicleDifferential.h
        JoltPhysics/Jolt/Physics/Vehicle/VehicleEngine.h
        JoltPhysics/Jolt/Physics/Vehicle/VehicleTrack.h
        JoltPhysics/Jolt/Physics/Vehicle/VehicleTransmission.h
        JoltPhysics/Jolt/Physics/Vehicle/Wheel.h
        JoltPhysics/Jolt/Physics/Vehicle/WheeledVehicleController.h
        # Physics
        JoltPhysics/Jolt/Physics/DeterminismLog.h
        JoltPhysics/Jolt/Physics/EActivation.h
        JoltPhysics/Jolt/Physics/EPhysicsUpdateError.h
        JoltPhysics/Jolt/Physics/IslandBuilder.h
        JoltPhysics/Jolt/Physics/LargeIslandSplitter.h
        JoltPhysics/Jolt/Physics/PhysicsLock.h
        JoltPhysics/Jolt/Physics/PhysicsScene.h
        JoltPhysics/Jolt/Physics/PhysicsSettings.h
        JoltPhysics/Jolt/Physics/PhysicsStepListener.h
        JoltPhysics/Jolt/Physics/PhysicsSystem.h
        JoltPhysics/Jolt/Physics/PhysicsUpdateContext.h
        JoltPhysics/Jolt/Physics/StateRecorder.h
        JoltPhysics/Jolt/Physics/StateRecorderImpl.h
        # Physics/Constraints
        JoltPhysics/Jolt/Physics/Constraints/ConstraintPart/AngleConstraintPart.h
        JoltPhysics/Jolt/Physics/Constraints/ConstraintPart/AxisConstraintPart.h
        JoltPhysics/Jolt/Physics/Constraints/ConstraintPart/DualAxisConstraintPart.h
        JoltPhysics/Jolt/Physics/Constraints/ConstraintPart/GearConstraintPart.h
        JoltPhysics/Jolt/Physics/Constraints/ConstraintPart/HingeRotationConstraintPart.h
        JoltPhysics/Jolt/Physics/Constraints/ConstraintPart/IndependentAxisConstraintPart.h
        JoltPhysics/Jolt/Physics/Constraints/ConstraintPart/PointConstraintPart.h
        JoltPhysics/Jolt/Physics/Constraints/ConstraintPart/RackAndPinionConstraintPart.h
        JoltPhysics/Jolt/Physics/Constraints/ConstraintPart/RotationEulerConstraintPart.h
        JoltPhysics/Jolt/Physics/Constraints/ConstraintPart/RotationQuatConstraintPart.h
        JoltPhysics/Jolt/Physics/Constraints/ConstraintPart/SpringPart.h
        JoltPhysics/Jolt/Physics/Constraints/ConstraintPart/SwingTwistConstraintPart.h
        JoltPhysics/Jolt/Physics/Constraints/CalculateSolverSteps.h
        JoltPhysics/Jolt/Physics/Constraints/ConeConstraint.h
        JoltPhysics/Jolt/Physics/Constraints/Constraint.h
        JoltPhysics/Jolt/Physics/Constraints/ConstraintManager.h
        JoltPhysics/Jolt/Physics/Constraints/ContactConstraintManager.h
        JoltPhysics/Jolt/Physics/Constraints/DistanceConstraint.h
        JoltPhysics/Jolt/Physics/Constraints/FixedConstraint.h
        JoltPhysics/Jolt/Physics/Constraints/GearConstraint.h
        JoltPhysics/Jolt/Physics/Constraints/HingeConstraint.h
        JoltPhysics/Jolt/Physics/Constraints/MotorSettings.h
        JoltPhysics/Jolt/Physics/Constraints/PathConstraint.h
        JoltPhysics/Jolt/Physics/Constraints/PathConstraintPath.h
        JoltPhysics/Jolt/Physics/Constraints/PathConstraintPathHermite.h
        JoltPhysics/Jolt/Physics/Constraints/PointConstraint.h
        JoltPhysics/Jolt/Physics/Constraints/PulleyConstraint.h
        JoltPhysics/Jolt/Physics/Constraints/RackAndPinionConstraint.h
        JoltPhysics/Jolt/Physics/Constraints/SixDOFConstraint.h
        JoltPhysics/Jolt/Physics/Constraints/SliderConstraint.h
        JoltPhysics/Jolt/Physics/Constraints/SpringSettings.h
        JoltPhysics/Jolt/Physics/Constraints/SwingTwistConstraint.h
        JoltPhysics/Jolt/Physics/Constraints/TwoBodyConstraint.h
        # ObjectStream
        JoltPhysics/Jolt/ObjectStream/GetPrimitiveTypeOfType.h
        JoltPhysics/Jolt/ObjectStream/ObjectStream.h
        JoltPhysics/Jolt/ObjectStream/ObjectStreamBinaryIn.h
        JoltPhysics/Jolt/ObjectStream/ObjectStreamBinaryOut.h
        JoltPhysics/Jolt/ObjectStream/ObjectStreamIn.h
        JoltPhysics/Jolt/ObjectStream/ObjectStreamOut.h
        JoltPhysics/Jolt/ObjectStream/ObjectStreamTextIn.h
        JoltPhysics/Jolt/ObjectStream/ObjectStreamTextOut.h
        JoltPhysics/Jolt/ObjectStream/ObjectStreamTypes.h
        JoltPhysics/Jolt/ObjectStream/SerializableAttribute.h
        JoltPhysics/Jolt/ObjectStream/SerializableAttributeEnum.h
        JoltPhysics/Jolt/ObjectStream/SerializableAttributeTyped.h
        JoltPhysics/Jolt/ObjectStream/SerializableObject.h
        JoltPhysics/Jolt/ObjectStream/TypeDeclarations.h
        # TriangleSplitter
        JoltPhysics/Jolt/TriangleSplitter/TriangleSplitter.h
        JoltPhysics/Jolt/TriangleSplitter/TriangleSplitterBinning.h
        JoltPhysics/Jolt/TriangleSplitter/TriangleSplitterMean.h
        # Skeleton
        JoltPhysics/Jolt/Skeleton/SkeletalAnimation.h
        JoltPhysics/Jolt/Skeleton/Skeleton.h
        JoltPhysics/Jolt/Skeleton/SkeletonMapper.h
        JoltPhysics/Jolt/Skeleton/SkeletonPose.h
        # AABBTree
        JoltPhysics/Jolt/AABBTree/NodeCodec/NodeCodecQuadTreeHalfFloat.h
        JoltPhysics/Jolt/AABBTree/TriangleCodec/TriangleCodecIndexed8BitPackSOA4Flags.h
        JoltPhysics/Jolt/AABBTree/AABBTreeBuilder.h
        JoltPhysics/Jolt/AABBTree/AABBTreeToBuffer.h
    )

    # Source files
    set(JOLT_SOURCES
        JoltPhysics/Jolt/RegisterTypes.cpp
        # Core
        JoltPhysics/Jolt/Core/Color.cpp
        JoltPhysics/Jolt/Core/Factory.cpp
        JoltPhysics/Jolt/Core/IssueReporting.cpp
        JoltPhysics/Jolt/Core/JobSystemSingleThreaded.cpp
        JoltPhysics/Jolt/Core/JobSystemThreadPool.cpp
        JoltPhysics/Jolt/Core/JobSystemWithBarrier.cpp
        JoltPhysics/Jolt/Core/LinearCurve.cpp
        JoltPhysics/Jolt/Core/Memory.cpp
        JoltPhysics/Jolt/Core/Profiler.cpp
        JoltPhysics/Jolt/Core/RTTI.cpp
        JoltPhysics/Jolt/Core/Semaphore.cpp
        JoltPhysics/Jolt/Core/StringTools.cpp
        JoltPhysics/Jolt/Core/TickCounter.cpp
        # Geometry
        JoltPhysics/Jolt/Geometry/ConvexHullBuilder.cpp
        JoltPhysics/Jolt/Geometry/ConvexHullBuilder2D.cpp
        JoltPhysics/Jolt/Geometry/Indexify.cpp
        JoltPhysics/Jolt/Geometry/OrientedBox.cpp
        # Math
        JoltPhysics/Jolt/Math/Vec3.cpp
        # Physics/Body
        JoltPhysics/Jolt/Physics/Body/Body.cpp
        JoltPhysics/Jolt/Physics/Body/BodyCreationSettings.cpp
        JoltPhysics/Jolt/Physics/Body/BodyInterface.cpp
        JoltPhysics/Jolt/Physics/Body/BodyManager.cpp
        JoltPhysics/Jolt/Physics/Body/MassProperties.cpp
        JoltPhysics/Jolt/Physics/Body/MotionProperties.cpp
        # Physics/Character
        JoltPhysics/Jolt/Physics/Character/Character.cpp
        JoltPhysics/Jolt/Physics/Character/CharacterBase.cpp
        JoltPhysics/Jolt/Physics/Character/CharacterVirtual.cpp
        # Physics/Collision
        JoltPhysics/Jolt/Physics/Collision/CastConvexVsTriangles.cpp
        JoltPhysics/Jolt/Physics/Collision/CastSphereVsTriangles.cpp
        JoltPhysics/Jolt/Physics/Collision/CollideConvexVsTriangles.cpp
        JoltPhysics/Jolt/Physics/Collision/CollideSphereVsTriangles.cpp
        JoltPhysics/Jolt/Physics/Collision/CollisionDispatch.cpp
        JoltPhysics/Jolt/Physics/Collision/CollisionGroup.cpp
        JoltPhysics/Jolt/Physics/Collision/EstimateCollisionResponse.cpp
        JoltPhysics/Jolt/Physics/Collision/GroupFilter.cpp
        JoltPhysics/Jolt/Physics/Collision/GroupFilterTable.cpp
        JoltPhysics/Jolt/Physics/Collision/ManifoldBetweenTwoFaces.cpp
        JoltPhysics/Jolt/Physics/Collision/NarrowPhaseQuery.cpp
        JoltPhysics/Jolt/Physics/Collision/NarrowPhaseStats.cpp
        JoltPhysics/Jolt/Physics/Collision/PhysicsMaterial.cpp
        JoltPhysics/Jolt/Physics/Collision/PhysicsMaterialSimple.cpp
        JoltPhysics/Jolt/Physics/Collision/TransformedShape.cpp
        # Physics/Collision/BroadPhase
        JoltPhysics/Jolt/Physics/Collision/BroadPhase/BroadPhase.cpp
        JoltPhysics/Jolt/Physics/Collision/BroadPhase/BroadPhaseBruteForce.cpp
        JoltPhysics/Jolt/Physics/Collision/BroadPhase/BroadPhaseQuadTree.cpp
        JoltPhysics/Jolt/Physics/Collision/BroadPhase/QuadTree.cpp
        # Physics/Collision/Shape
        JoltPhysics/Jolt/Physics/Collision/Shape/BoxShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/CapsuleShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/CompoundShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/ConvexHullShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/ConvexShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/CylinderShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/DecoratedShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/EmptyShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/HeightFieldShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/MeshShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/MutableCompoundShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/PlaneShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/RotatedTranslatedShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/ScaledShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/Shape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/SphereShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/StaticCompoundShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/TaperedCapsuleShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/TaperedCylinderShape.cpp
        JoltPhysics/Jolt/Physics/Collision/Shape/TriangleShape.cpp
        # Physics/Ragdoll
        JoltPhysics/Jolt/Physics/Ragdoll/Ragdoll.cpp
        # Physics/SoftBody
        JoltPhysics/Jolt/Physics/SoftBody/SoftBodyCreationSettings.cpp
        JoltPhysics/Jolt/Physics/SoftBody/SoftBodyMotionProperties.cpp
        JoltPhysics/Jolt/Physics/SoftBody/SoftBodyShape.cpp
        JoltPhysics/Jolt/Physics/SoftBody/SoftBodySharedSettings.cpp
        # Physics/Vehicle
        JoltPhysics/Jolt/Physics/Vehicle/MotorcycleController.cpp
        JoltPhysics/Jolt/Physics/Vehicle/TrackedVehicleController.cpp
        JoltPhysics/Jolt/Physics/Vehicle/VehicleAntiRollBar.cpp
        JoltPhysics/Jolt/Physics/Vehicle/VehicleCollisionTester.cpp
        JoltPhysics/Jolt/Physics/Vehicle/VehicleConstraint.cpp
        JoltPhysics/Jolt/Physics/Vehicle/VehicleController.cpp
        JoltPhysics/Jolt/Physics/Vehicle/VehicleDifferential.cpp
        JoltPhysics/Jolt/Physics/Vehicle/VehicleEngine.cpp
        JoltPhysics/Jolt/Physics/Vehicle/VehicleTrack.cpp
        JoltPhysics/Jolt/Physics/Vehicle/VehicleTransmission.cpp
        JoltPhysics/Jolt/Physics/Vehicle/Wheel.cpp
        JoltPhysics/Jolt/Physics/Vehicle/WheeledVehicleController.cpp
        # Physics
        JoltPhysics/Jolt/Physics/DeterminismLog.cpp
        JoltPhysics/Jolt/Physics/IslandBuilder.cpp
        JoltPhysics/Jolt/Physics/LargeIslandSplitter.cpp
        JoltPhysics/Jolt/Physics/PhysicsScene.cpp
        JoltPhysics/Jolt/Physics/PhysicsSystem.cpp
        JoltPhysics/Jolt/Physics/PhysicsUpdateContext.cpp
        JoltPhysics/Jolt/Physics/StateRecorderImpl.cpp
        # Physics/Constraints
        JoltPhysics/Jolt/Physics/Constraints/ConeConstraint.cpp
        JoltPhysics/Jolt/Physics/Constraints/Constraint.cpp
        JoltPhysics/Jolt/Physics/Constraints/ConstraintManager.cpp
        JoltPhysics/Jolt/Physics/Constraints/ContactConstraintManager.cpp
        JoltPhysics/Jolt/Physics/Constraints/DistanceConstraint.cpp
        JoltPhysics/Jolt/Physics/Constraints/FixedConstraint.cpp
        JoltPhysics/Jolt/Physics/Constraints/GearConstraint.cpp
        JoltPhysics/Jolt/Physics/Constraints/HingeConstraint.cpp
        JoltPhysics/Jolt/Physics/Constraints/MotorSettings.cpp
        JoltPhysics/Jolt/Physics/Constraints/PathConstraint.cpp
        JoltPhysics/Jolt/Physics/Constraints/PathConstraintPath.cpp
        JoltPhysics/Jolt/Physics/Constraints/PathConstraintPathHermite.cpp
        JoltPhysics/Jolt/Physics/Constraints/PointConstraint.cpp
        JoltPhysics/Jolt/Physics/Constraints/PulleyConstraint.cpp
        JoltPhysics/Jolt/Physics/Constraints/RackAndPinionConstraint.cpp
        JoltPhysics/Jolt/Physics/Constraints/SixDOFConstraint.cpp
        JoltPhysics/Jolt/Physics/Constraints/SliderConstraint.cpp
        JoltPhysics/Jolt/Physics/Constraints/SpringSettings.cpp
        JoltPhysics/Jolt/Physics/Constraints/SwingTwistConstraint.cpp
        JoltPhysics/Jolt/Physics/Constraints/TwoBodyConstraint.cpp
        # ObjectStream
        JoltPhysics/Jolt/ObjectStream/ObjectStream.cpp
        JoltPhysics/Jolt/ObjectStream/ObjectStreamBinaryIn.cpp
        JoltPhysics/Jolt/ObjectStream/ObjectStreamBinaryOut.cpp
        JoltPhysics/Jolt/ObjectStream/ObjectStreamIn.cpp
        JoltPhysics/Jolt/ObjectStream/ObjectStreamOut.cpp
        JoltPhysics/Jolt/ObjectStream/ObjectStreamTextIn.cpp
        JoltPhysics/Jolt/ObjectStream/ObjectStreamTextOut.cpp
        JoltPhysics/Jolt/ObjectStream/SerializableObject.cpp
        JoltPhysics/Jolt/ObjectStream/TypeDeclarations.cpp
        # TriangleSplitter
        JoltPhysics/Jolt/TriangleSplitter/TriangleSplitter.cpp
        JoltPhysics/Jolt/TriangleSplitter/TriangleSplitterBinning.cpp
        JoltPhysics/Jolt/TriangleSplitter/TriangleSplitterMean.cpp
        # Skeleton
        JoltPhysics/Jolt/Skeleton/SkeletalAnimation.cpp
        JoltPhysics/Jolt/Skeleton/Skeleton.cpp
        JoltPhysics/Jolt/Skeleton/SkeletonMapper.cpp
        JoltPhysics/Jolt/Skeleton/SkeletonPose.cpp
        # AABBTree
        JoltPhysics/Jolt/AABBTree/AABBTreeBuilder.cpp
    )

    add_library(Extern.jolt STATIC ${JOLT_HEADERS} ${JOLT_SOURCES})
    target_include_directories(Extern.jolt PUBLIC JoltPhysics)

    # Note: Do NOT define JPH_DISABLE_CUSTOM_ALLOCATOR
    # Traktor.Physics.Jolt hooks its own allocators via JPH::Allocate, JPH::Free, etc.

    set_target_properties(Extern.jolt PROPERTIES OUTPUT_NAME "jolt")
endif()

# ============================================================================
# Recast Navigation (recastnavigation/Recast)
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/recastnavigation/Recast)
    file(GLOB RECAST_SOURCES recastnavigation/Recast/Source/*.cpp)
    add_library(Extern.recast STATIC ${RECAST_SOURCES})
    target_include_directories(Extern.recast PUBLIC
        recastnavigation/Recast/Include
    )
    set_target_properties(Extern.recast PROPERTIES OUTPUT_NAME "recast")
endif()

# ============================================================================
# Detour Navigation (recastnavigation/Detour)
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/recastnavigation/Detour)
    file(GLOB DETOUR_SOURCES recastnavigation/Detour/Source/*.cpp)
    add_library(Extern.detour STATIC ${DETOUR_SOURCES})
    target_include_directories(Extern.detour PUBLIC
        recastnavigation/Detour/Include
    )
    set_target_properties(Extern.detour PROPERTIES OUTPUT_NAME "detour")
endif()

# ============================================================================
# ASTC Encoder (astc-encoder/Source)
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/astc-encoder/Source)
    # Compile astcenc source files matching vcxproj (31 files)
    add_library(Extern.astcenc STATIC
        astc-encoder/Source/astcenc_averages_and_directions.cpp
        astc-encoder/Source/astcenc_block_sizes.cpp
        astc-encoder/Source/astcenc_color_quantize.cpp
        astc-encoder/Source/astcenc_color_unquantize.cpp
        astc-encoder/Source/astcenc_compress_symbolic.cpp
        astc-encoder/Source/astcenc_compute_variance.cpp
        astc-encoder/Source/astcenc_decompress_symbolic.cpp
        astc-encoder/Source/astcenc_diagnostic_trace.cpp
        astc-encoder/Source/astcenc_entry.cpp
        astc-encoder/Source/astcenc_find_best_partitioning.cpp
        astc-encoder/Source/astcenc_ideal_endpoints_and_weights.cpp
        astc-encoder/Source/astcenc_image.cpp
        astc-encoder/Source/astcenc_integer_sequence.cpp
        astc-encoder/Source/astcenc_mathlib.cpp
        astc-encoder/Source/astcenc_mathlib_softfloat.cpp
        astc-encoder/Source/astcenc_partition_tables.cpp
        astc-encoder/Source/astcenc_percentile_tables.cpp
        astc-encoder/Source/astcenc_pick_best_endpoint_format.cpp
        astc-encoder/Source/astcenc_quantization.cpp
        astc-encoder/Source/astcenc_symbolic_physical.cpp
        astc-encoder/Source/astcenc_weight_align.cpp
        astc-encoder/Source/astcenc_weight_quant_xfer_tables.cpp
        astc-encoder/Source/astcenccli_entry.cpp
        astc-encoder/Source/astcenccli_entry2.cpp
        astc-encoder/Source/astcenccli_error_metrics.cpp
        astc-encoder/Source/astcenccli_image.cpp
        astc-encoder/Source/astcenccli_image_external.cpp
        astc-encoder/Source/astcenccli_image_load_store.cpp
        astc-encoder/Source/astcenccli_platform_dependents.cpp
        astc-encoder/Source/astcenccli_toplevel.cpp
        astc-encoder/Source/astcenccli_toplevel_help.cpp
    )

    target_include_directories(Extern.astcenc PUBLIC
        astc-encoder/Source
    )

    # Add ASTC encoder defines from vcxproj
    target_compile_definitions(Extern.astcenc PRIVATE
        ASTCENC_SSE=20
        ASTCENC_POPCNT=0
        ASTCENC_AVX=0
        ASTCENC_VECALIGN=16
    )

    set_target_properties(Extern.astcenc PROPERTIES OUTPUT_NAME "Extern.astcenc")
endif()

# ============================================================================
# bc6h_enc - BC6H compression (header-only)
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bc6h_enc)
    add_library(Extern.bc6h_enc INTERFACE)
    target_include_directories(Extern.bc6h_enc INTERFACE bc6h_enc)
endif()

# ============================================================================
# Organize Extern projects into solution folder "Extern Win64"
# ============================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set folder property for all Extern.* targets
if(TARGET Extern.mimalloc)
    set_target_properties(Extern.mimalloc PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.zlib)
    set_target_properties(Extern.zlib PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.lpng)
    set_target_properties(Extern.lpng PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.jpeg)
    set_target_properties(Extern.jpeg PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.expat)
    set_target_properties(Extern.expat PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.lua)
    set_target_properties(Extern.lua PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.sqlite)
    set_target_properties(Extern.sqlite PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.ogg)
    set_target_properties(Extern.ogg PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.vorbis)
    set_target_properties(Extern.vorbis PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.theora)
    set_target_properties(Extern.theora PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.flac)
    set_target_properties(Extern.flac PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.freetype)
    set_target_properties(Extern.freetype PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.squish)
    set_target_properties(Extern.squish PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.rg_etc1)
    set_target_properties(Extern.rg_etc1 PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.tinyexr)
    set_target_properties(Extern.tinyexr PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.xatlas)
    set_target_properties(Extern.xatlas PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.bullet)
    set_target_properties(Extern.bullet PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.jolt)
    set_target_properties(Extern.jolt PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.ufbx)
    set_target_properties(Extern.ufbx PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.MikkTSpace)
    set_target_properties(Extern.MikkTSpace PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.liblzf)
    set_target_properties(Extern.liblzf PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.agg)
    set_target_properties(Extern.agg PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.recast)
    set_target_properties(Extern.recast PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.detour)
    set_target_properties(Extern.detour PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.astcenc)
    set_target_properties(Extern.astcenc PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.stb)
    set_target_properties(Extern.stb PROPERTIES FOLDER "Extern Win64")
endif()
if(TARGET Extern.VulkanMemoryAllocator)
    set_target_properties(Extern.VulkanMemoryAllocator PROPERTIES FOLDER "Extern Win64")
endif()
# Note: Extern.bc6h_enc is an INTERFACE library and doesn't support FOLDER property

message(STATUS "Third-party libraries configured")
