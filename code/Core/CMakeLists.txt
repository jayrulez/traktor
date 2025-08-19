# Traktor.Core CMakeLists.txt
# This file should be placed in code/Core/

# =============================================================================
# Traktor.Core Library
# =============================================================================

# Create library based on configuration
# The library type (STATIC/SHARED) is determined by the configuration
if(CMAKE_CONFIGURATION_TYPES)
    # Multi-config generators (Visual Studio, Xcode)
    add_library(Traktor.Core STATIC)
    
    # We'll set up generator expressions to handle shared/static per config
    set_target_properties(Traktor.Core PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
    
    # Use generator expressions for compile definitions
    target_compile_definitions(Traktor.Core PRIVATE
        $<$<CONFIG:DebugShared>:T_CORE_EXPORT>
        $<$<CONFIG:ReleaseShared>:T_CORE_EXPORT>
        $<$<CONFIG:DebugStatic>:T_STATIC>
        $<$<CONFIG:ReleaseStatic>:T_STATIC>
    )
else()
    # Single-config generators (Make, Ninja)
    if(CMAKE_BUILD_TYPE MATCHES ".*Shared")
        add_library(Traktor.Core SHARED)
        target_compile_definitions(Traktor.Core PRIVATE T_CORE_EXPORT)
    else()
        add_library(Traktor.Core STATIC)
        target_compile_definitions(Traktor.Core PRIVATE T_STATIC)
    endif()
endif()

# =============================================================================
# Source Files
# =============================================================================

# Root level source files
target_sources(Traktor.Core PRIVATE
    Guid.cpp
    Module.cpp
    Object.cpp
)

# Memory subsystem
target_sources(Traktor.Core PRIVATE
    Memory/Alloc.cpp
    Memory/BlockAllocator.cpp
    Memory/DebugAllocator.cpp
    Memory/FastAllocator.cpp
    Memory/MemoryConfig.cpp
    Memory/PoolAllocator.cpp
    Memory/StdAllocator.cpp
    Memory/TrackAllocator.cpp
)

# Platform-specific IO files
if(WIN32)
    target_sources(Traktor.Core PRIVATE
        Io/Win32/NativeMappedFile.cpp
        Io/Win32/NativeMappedStream.cpp
        Io/Win32/NativeStream.cpp
        Io/Win32/NativeVolume.cpp
    )
elseif(APPLE)
    target_sources(Traktor.Core PRIVATE
        Io/Posix/NativeMappedFile.cpp
        Io/Posix/NativeMappedStream.cpp
        Io/Posix/NativeStream.cpp
        Io/Posix/NativeVolume.cpp
    )
elseif(UNIX)
    target_sources(Traktor.Core PRIVATE
        Io/Posix/NativeMappedFile.cpp
        Io/Posix/NativeMappedStream.cpp
        Io/Posix/NativeStream.cpp
        Io/Posix/NativeVolume.cpp
    )
endif()

# IO subsystem (platform-independent)
target_sources(Traktor.Core PRIVATE
    Io/AnsiEncoding.cpp
    Io/BitReader.cpp
    Io/BitWriter.cpp
    Io/BufferedStream.cpp
    Io/ChunkMemory.cpp
    Io/ChunkMemoryStream.cpp
    Io/DebugStream.cpp
    Io/DynamicMemoryStream.cpp
    Io/File.cpp
    Io/FileOutputStream.cpp
    Io/FileOutputStreamBuffer.cpp
    Io/FileSystem.cpp
    Io/IEncoding.cpp
    Io/IMappedFile.cpp
    Io/IOutputStreamBuffer.cpp
    Io/IStream.cpp
    Io/IVolume.cpp
    Io/MemoryStream.cpp
    Io/OutputStream.cpp
    Io/OutputStreamBuffer.cpp
    Io/Path.cpp
    Io/Reader.cpp
    Io/StreamCompare.cpp
    Io/StreamCopy.cpp
    Io/StreamStream.cpp
    Io/StringOutputStream.cpp
    Io/StringReader.cpp
    Io/Utf16Encoding.cpp
    Io/Utf32Encoding.cpp
    Io/Utf8Encoding.cpp
    Io/Writer.cpp
)

# Library subsystem
if(WIN32)
    target_sources(Traktor.Core PRIVATE
        Library/Win32/Library.cpp
    )
elseif(UNIX OR APPLE)
    target_sources(Traktor.Core PRIVATE
        Library/Posix/Library.cpp
    )
endif()

# Log subsystem
target_sources(Traktor.Core PRIVATE
    Log/Log.cpp
    Log/LogRedirectTarget.cpp
    Log/LogStreamTarget.cpp
)

# Math subsystem
target_sources(Traktor.Core PRIVATE
    Math/Aabb2.cpp
    Math/Aabb3.cpp
    Math/Bezier2nd.cpp
    Math/Bezier3rd.cpp
    Math/BspTree.cpp
    Math/Color4f.cpp
    Math/Color4ub.cpp
    Math/Frustum.cpp
    Math/Half.cpp
    Math/IntervalTransform.cpp
    Math/Line2.cpp
    Math/Matrix33.cpp
    Math/Matrix44.cpp
    Math/Plane.cpp
    Math/Polar.cpp
    Math/Quasirandom.cpp
    Math/Quaternion.cpp
    Math/Random.cpp
    Math/Ray2.cpp
    Math/Ray3.cpp
    Math/Rotator.cpp
    Math/SahTree.cpp
    Math/Scalar.cpp
    Math/Sphere.cpp
    Math/Transform.cpp
    Math/TransformPath.cpp
    Math/Triangulator.cpp
    Math/Variance.cpp
    Math/Vector2.cpp
    Math/Vector2i.cpp
    Math/Vector4.cpp
    Math/Winding2.cpp
    Math/Winding3.cpp
)

# RTTI subsystem
target_sources(Traktor.Core PRIVATE
    Rtti/ITypedObject.cpp
    Rtti/TypeInfo.cpp
)

# Serialization subsystem
target_sources(Traktor.Core PRIVATE
    Serialization/Attribute.cpp
    Serialization/AttributeDirection.cpp
    Serialization/AttributeDirectory.cpp
    Serialization/AttributeHdr.cpp
    Serialization/AttributeHex.cpp
    Serialization/AttributeMultiLine.cpp
    Serialization/AttributeNoHash.cpp
    Serialization/AttributePoint.cpp
    Serialization/AttributePrecision.cpp
    Serialization/AttributePrivate.cpp
    Serialization/AttributeRange.cpp
    Serialization/AttributeReadOnly.cpp
    Serialization/AttributeType.cpp
    Serialization/AttributeUnit.cpp
    Serialization/BinarySerializer.cpp
    Serialization/CompactSerializer.cpp
    Serialization/DeepClone.cpp
    Serialization/DeepHash.cpp
    Serialization/ISerializable.cpp
    Serialization/ISerializer.cpp
    Serialization/MemberArray.cpp
    Serialization/MemberBitMask.cpp
    Serialization/MemberComplex.cpp
    Serialization/MemberEnum.cpp
    Serialization/MemberType.cpp
    Serialization/PackedUnitVector.cpp
    Serialization/Serializer.cpp
)

# Thread subsystem
target_sources(Traktor.Core PRIVATE
    Thread/Atomic.cpp
    Thread/Job.cpp
    Thread/JobManager.cpp
    Thread/JobQueue.cpp
    Thread/ReaderWriterLock.cpp
    Thread/Result.cpp
    Thread/SpinLock.cpp
    Thread/ThreadManager.cpp
    Thread/ThreadPool.cpp
    Thread/TicketLock.cpp
)

# Platform-specific thread files
if(WIN32)
    target_sources(Traktor.Core PRIVATE
        Thread/Win32/CriticalSection.cpp
        Thread/Win32/Event.cpp
        Thread/Win32/Mutex.cpp
        Thread/Win32/Semaphore.cpp
        Thread/Win32/Signal.cpp
        Thread/Win32/Thread.cpp
        Thread/Win32/ThreadLocal.cpp
    )
elseif(UNIX OR APPLE)
    target_sources(Traktor.Core PRIVATE
        Thread/Posix/CriticalSection.cpp
        Thread/Posix/Event.cpp
        Thread/Posix/Mutex.cpp
        Thread/Posix/Semaphore.cpp
        Thread/Posix/Signal.cpp
        Thread/Posix/Thread.cpp
        Thread/Posix/ThreadLocal.cpp
    )
endif()

# Timer subsystem
target_sources(Traktor.Core PRIVATE
    Timer/Profiler.cpp
)

if(WIN32)
    target_sources(Traktor.Core PRIVATE
        Timer/Win32/Timer.cpp
    )
elseif(UNIX OR APPLE)
    target_sources(Traktor.Core PRIVATE
        Timer/Posix/Timer.cpp
    )
endif()

# Misc subsystem
target_sources(Traktor.Core PRIVATE
    Misc/Adler32.cpp
    Misc/Base64.cpp
    Misc/CommandLine.cpp
    Misc/HashStream.cpp
    Misc/IHash.cpp
    Misc/Key.cpp
    Misc/MD5.cpp
    Misc/Murmur3.cpp
    Misc/ObjectStore.cpp
    Misc/Preprocessor.cpp
    Misc/SHA1.cpp
    Misc/String.cpp
    Misc/TString.cpp
    Misc/WildCompare.cpp
)

# Debug subsystem
if(WIN32)
    target_sources(Traktor.Core PRIVATE
        Debug/Win32/CallStack.cpp
        Debug/Win32/Debugger.cpp
    )
elseif(UNIX OR APPLE)
    target_sources(Traktor.Core PRIVATE
        Debug/Posix/CallStack.cpp
        Debug/Posix/Debugger.cpp
    )
endif()

# System subsystem
target_sources(Traktor.Core PRIVATE
    System/Environment.cpp
    System/IProcess.cpp
    System/ISharedMemory.cpp
    System/PipeReader.cpp
    System/ResolveEnv.cpp
)

if(WIN32)
    target_sources(Traktor.Core PRIVATE
        System/Win32/OS.cpp
        System/Win32/ProcessShellWin32.cpp
        System/Win32/ProcessWin32.cpp
        System/Win32/SharedMemoryWin32.cpp
    )
elseif(APPLE)
    target_sources(Traktor.Core PRIVATE
        System/Posix/OS.cpp
        System/Posix/ProcessPosix.cpp
        System/Posix/SharedMemoryPosix.cpp
    )
elseif(UNIX)
    target_sources(Traktor.Core PRIVATE
        System/Posix/OS.cpp
        System/Posix/ProcessPosix.cpp
        System/Posix/SharedMemoryPosix.cpp
    )
endif()

# Singleton subsystem
target_sources(Traktor.Core PRIVATE
    Singleton/SingletonManager.cpp
)

# Date subsystem
target_sources(Traktor.Core PRIVATE
    Date/DateTime.cpp
)

# Containers subsystem
target_sources(Traktor.Core PRIVATE
    Containers/IdAllocator.cpp
)

# Settings subsystem
target_sources(Traktor.Core PRIVATE
    Settings/IPropertyValue.cpp
    Settings/PropertyArray.cpp
    Settings/PropertyBoolean.cpp
    Settings/PropertyColor.cpp
    Settings/PropertyFloat.cpp
    Settings/PropertyGroup.cpp
    Settings/PropertyInteger.cpp
    Settings/PropertyObject.cpp
    Settings/PropertyString.cpp
    Settings/PropertyStringArray.cpp
    Settings/PropertyStringSet.cpp
)

# Reflection subsystem
target_sources(Traktor.Core PRIVATE
    Reflection/Reflection.cpp
    Reflection/ReflectionApplySerializer.cpp
    Reflection/ReflectionInspectSerializer.cpp
    Reflection/ReflectionMember.cpp
    Reflection/ReflectionMemberPredicate.cpp
    Reflection/RfmArray.cpp
    Reflection/RfmCompound.cpp
    Reflection/RfmEnum.cpp
    Reflection/RfmObject.cpp
    Reflection/RfmPrimitive.cpp
    Reflection/RfpMemberName.cpp
    Reflection/RfpMemberType.cpp
    Reflection/RfpObjectType.cpp
)

# Class subsystem
target_sources(Traktor.Core PRIVATE
    Class/Any.cpp
    Class/Boxed.cpp
    Class/BoxedClassFactory.cpp
    Class/CoreClassFactory1.cpp
    Class/CoreClassFactory2.cpp
    Class/IRuntimeClass.cpp
    Class/IRuntimeClassFactory.cpp
    Class/IRuntimeDelegate.cpp
    Class/IRuntimeDispatch.cpp
    Class/IRuntimeObject.cpp
    Class/OperatorDispatch.cpp
    Class/OrderedClassRegistrar.cpp
    Class/PolymorphicDispatch.cpp
    Class/RuntimeClass.cpp
    Class/RuntimeException.cpp
)

# Class/Boxes subsystem
target_sources(Traktor.Core PRIVATE
    Class/Boxes/BoxedAabb2.cpp
    Class/Boxes/BoxedAabb3.cpp
    Class/Boxes/BoxedAlignedVector.cpp
    Class/Boxes/BoxedBezier2nd.cpp
    Class/Boxes/BoxedBezier3rd.cpp
    Class/Boxes/BoxedColor4f.cpp
    Class/Boxes/BoxedColor4ub.cpp
    Class/Boxes/BoxedFrustum.cpp
    Class/Boxes/BoxedGuid.cpp
    Class/Boxes/BoxedIntervalTransform.cpp
    Class/Boxes/BoxedMatrix33.cpp
    Class/Boxes/BoxedMatrix44.cpp
    Class/Boxes/BoxedPlane.cpp
    Class/Boxes/BoxedPointer.cpp
    Class/Boxes/BoxedQuaternion.cpp
    Class/Boxes/BoxedRandom.cpp
    Class/Boxes/BoxedRandomGeometry.cpp
    Class/Boxes/BoxedRange.cpp
    Class/Boxes/BoxedRay3.cpp
    Class/Boxes/BoxedRefArray.cpp
    Class/Boxes/BoxedSphere.cpp
    Class/Boxes/BoxedStdVector.cpp
    Class/Boxes/BoxedTransform.cpp
    Class/Boxes/BoxedTypeInfo.cpp
    Class/Boxes/BoxedVector2.cpp
    Class/Boxes/BoxedVector4.cpp
    Class/Boxes/BoxedVector4Array.cpp
    Class/Boxes/BoxedWinding2.cpp
    Class/Boxes/BoxedWinding3.cpp
)

# Test subsystem
target_sources(Traktor.Core PRIVATE
    Test/Case.cpp
    Test/CaseAlignedVector.cpp
    Test/CaseAtomic.cpp
    Test/CaseBitReader.cpp
    Test/CaseCircularVector.cpp
    Test/CaseClone.cpp
    Test/CaseColor.cpp
    Test/CaseHash.cpp
    Test/CaseIdAllocator.cpp
    Test/CaseJob.cpp
    Test/CaseMath.cpp
    Test/CaseMatrix33.cpp
    Test/CaseMatrix44.cpp
    Test/CaseMeta.cpp
    Test/CaseMurmur.cpp
    Test/CasePath.cpp
    Test/CaseQuaternion.cpp
    Test/CaseRef.cpp
    Test/CaseSemaphore.cpp
    Test/CaseSmallMap.cpp
    Test/CaseThread.cpp
    Test/CaseThreadId.cpp
    Test/CaseThreadLocal.cpp
    Test/CaseThreadPool.cpp
    Test/CaseTransform.cpp
    Test/CaseWinding.cpp
    Test/TestClassFactory.cpp
)

# Windows-specific resource file
if(WIN32)
    target_sources(Traktor.Core PRIVATE
        Debug/Win32/Traktor.Core.rc
    )
endif()

# =============================================================================
# Include Directories
# =============================================================================
target_include_directories(Traktor.Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../..  # Points to code/ directory
    ${CMAKE_CURRENT_SOURCE_DIR}/..      # Points to code/Core/ parent
    ${CMAKE_CURRENT_SOURCE_DIR}         # Current directory
)

# Add mimalloc include if available
if(TARGET Extern.mimalloc)
    target_include_directories(Traktor.Core PRIVATE
        ${MIMALLOC_SDK}/include
    )
endif()

# =============================================================================
# Compile Definitions
# =============================================================================
target_compile_definitions(Traktor.Core PRIVATE
    $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:_DEBUG>
    $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:NDEBUG>
)

if(WIN32)
    target_compile_definitions(Traktor.Core PRIVATE
        WIN32
        _WINDOWS
        UNICODE
        _UNICODE
    )
endif()

# =============================================================================
# Compile Options
# =============================================================================
if(MSVC)
    target_compile_options(Traktor.Core PRIVATE
        /W3
        /MP  # Multi-processor compilation
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:/Od>
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:/RTC1>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/O2>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/Oi>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/Ot>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/fp:fast>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/GS->
    )
    
    # Set C++ standard
    target_compile_features(Traktor.Core PUBLIC cxx_std_20)
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Core PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:/GR->
    )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Traktor.Core PRIVATE
        -Wall
        -Wextra
        -Wno-unused-parameter
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-O0>
        $<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:-g>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-O3>
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-ffast-math>
    )
    
    # Disable RTTI in Release configurations
    target_compile_options(Traktor.Core PRIVATE
        $<$<OR:$<CONFIG:ReleaseShared>,$<CONFIG:ReleaseStatic>>:-fno-rtti>
    )
endif()

# =============================================================================
# Dependencies
# =============================================================================

# System libraries
if(WIN32)
    target_link_libraries(Traktor.Core PUBLIC
        Dbghelp
        Winmm
        shlwapi
    )
endif()

# External dependencies
if(TARGET Extern.mimalloc)
    target_link_libraries(Traktor.Core PUBLIC Extern.mimalloc)
endif()

# =============================================================================
# Output Configuration
# =============================================================================
# Set output directories based on configuration
set_target_properties(Traktor.Core PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUGSHARED ${CMAKE_BINARY_DIR}/lib/DebugShared
    LIBRARY_OUTPUT_DIRECTORY_DEBUGSHARED ${CMAKE_BINARY_DIR}/lib/DebugShared
    RUNTIME_OUTPUT_DIRECTORY_DEBUGSHARED ${CMAKE_BINARY_DIR}/bin/DebugShared
    
    ARCHIVE_OUTPUT_DIRECTORY_RELEASESHARED ${CMAKE_BINARY_DIR}/lib/ReleaseShared
    LIBRARY_OUTPUT_DIRECTORY_RELEASESHARED ${CMAKE_BINARY_DIR}/lib/ReleaseShared
    RUNTIME_OUTPUT_DIRECTORY_RELEASESHARED ${CMAKE_BINARY_DIR}/bin/ReleaseShared
    
    ARCHIVE_OUTPUT_DIRECTORY_DEBUGSTATIC ${CMAKE_BINARY_DIR}/lib/DebugStatic
    LIBRARY_OUTPUT_DIRECTORY_DEBUGSTATIC ${CMAKE_BINARY_DIR}/lib/DebugStatic
    RUNTIME_OUTPUT_DIRECTORY_DEBUGSTATIC ${CMAKE_BINARY_DIR}/bin/DebugStatic
    
    ARCHIVE_OUTPUT_DIRECTORY_RELEASESTATIC ${CMAKE_BINARY_DIR}/lib/ReleaseStatic
    LIBRARY_OUTPUT_DIRECTORY_RELEASESTATIC ${CMAKE_BINARY_DIR}/lib/ReleaseStatic
    RUNTIME_OUTPUT_DIRECTORY_RELEASESTATIC ${CMAKE_BINARY_DIR}/bin/ReleaseStatic
    
    OUTPUT_NAME "Traktor.Core"
)

# Handle runtime library selection for MSVC
if(MSVC)
    set_target_properties(Traktor.Core PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:DebugShared>,$<CONFIG:DebugStatic>>:Debug>DLL"
    )
endif()

# =============================================================================
# Installation
# =============================================================================
install(TARGETS Traktor.Core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/traktor/Core
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.inl"
    PATTERN "Test" EXCLUDE
)

# =============================================================================
# Export Target
# =============================================================================
#export(TARGETS Traktor.Core FILE ${CMAKE_BINARY_DIR}/TraktorCoreTargets.cmake)