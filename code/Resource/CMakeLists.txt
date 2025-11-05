# Traktor.Resource

set(TRAKTOR_RESOURCE_SOURCES
	ExclusiveResourceHandle.h
	ExplicitResourceHandle.h
	FileBundle.h
	IResourceFactory.h
	IResourceManager.h
	Id.h
	IdProxy.h
	Member.h
	MemberIdProxy.h
	Module.h
	Proxy.h
	ResidentResourceHandle.h
	ResourceBundle.h
	ResourceCast.h
	ResourceClassFactory.h
	ResourceHandle.h
	ResourceManager.h
	ExclusiveResourceHandle.cpp
	ExplicitResourceHandle.cpp
	FileBundle.cpp
	IResourceFactory.cpp
	IResourceManager.cpp
	Module.cpp
	ResidentResourceHandle.cpp
	ResourceBundle.cpp
	ResourceClassFactory.cpp
	ResourceHandle.cpp
	ResourceManager.cpp
)

add_traktor_module(Traktor.Resource
    SOURCES ${TRAKTOR_RESOURCE_SOURCES}
    EXPORT_DEFINE T_RESOURCE_EXPORT
    DEPENDENCIES
        Traktor.Core
)

target_include_directories(Traktor.Resource PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Resource.Editor - Editor support

set(TRAKTOR_RESOURCE_EDITOR_SOURCES
	Editor/FileBundleAsset.h
	Editor/FileBundlePipeline.h
	Editor/ResourceBundleAsset.h
	Editor/ResourceBundlePipeline.h
	Editor/FileBundleAsset.cpp
	Editor/FileBundlePipeline.cpp
	Editor/ResourceBundleAsset.cpp
	Editor/ResourceBundlePipeline.cpp
)

add_traktor_module(Traktor.Resource.Editor
    SOURCES ${TRAKTOR_RESOURCE_EDITOR_SOURCES}
    EXPORT_DEFINE T_RESOURCE_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Editor
        Traktor.Resource
)

target_include_directories(Traktor.Resource.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)