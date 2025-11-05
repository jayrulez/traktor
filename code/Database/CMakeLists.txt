# Traktor.Database

set(TRAKTOR_DATABASE_SOURCES
	ConnectionString.h
	Database.h
	DatabaseClassFactory.h
	Events/EvtGroupRenamed.h
	Events/EvtInstance.h
	Events/EvtInstanceCommitted.h
	Events/EvtInstanceCreated.h
	Events/EvtInstanceGuidChanged.h
	Events/EvtInstanceRemoved.h
	Events/EvtInstanceRenamed.h
	Group.h
	IEvent.h
	IGroupEventListener.h
	IInstanceEventListener.h
	Instance.h
	Isolate.h
	Module.h
	Provider/IProviderBus.h
	Provider/IProviderDatabase.h
	Provider/IProviderGroup.h
	Provider/IProviderInstance.h
	Traverse.h
	Types.h
	ConnectionString.cpp
	Database.cpp
	DatabaseClassFactory.cpp
	Events/EvtGroupRenamed.cpp
	Events/EvtInstance.cpp
	Events/EvtInstanceCommitted.cpp
	Events/EvtInstanceCreated.cpp
	Events/EvtInstanceGuidChanged.cpp
	Events/EvtInstanceRemoved.cpp
	Events/EvtInstanceRenamed.cpp
	Group.cpp
	IEvent.cpp
	Instance.cpp
	Isolate.cpp
	Module.cpp
	Provider/IProviderBus.cpp
	Provider/IProviderDatabase.cpp
	Provider/IProviderGroup.cpp
	Provider/IProviderInstance.cpp
)

add_traktor_module(Traktor.Database
    SOURCES ${TRAKTOR_DATABASE_SOURCES}
    EXPORT_DEFINE T_DATABASE_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Xml
        Traktor.Compress
)

target_include_directories(Traktor.Database PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Database.Compact

set(TRAKTOR_DATABASE_COMPACT_SOURCES
	Compact/BlockFile.h
	Compact/BlockReadStream.h
	Compact/BlockWriteStream.h
	Compact/CompactContext.h
	Compact/CompactDatabase.h
	Compact/CompactGroup.h
	Compact/CompactInstance.h
	Compact/CompactRegistry.h
	Compact/BlockFile.cpp
	Compact/BlockReadStream.cpp
	Compact/BlockWriteStream.cpp
	Compact/CompactContext.cpp
	Compact/CompactDatabase.cpp
	Compact/CompactGroup.cpp
	Compact/CompactInstance.cpp
	Compact/CompactRegistry.cpp
	Compact/Module.cpp
)

add_traktor_module(Traktor.Database.Compact
    SOURCES ${TRAKTOR_DATABASE_COMPACT_SOURCES}
    EXPORT_DEFINE T_DATABASE_COMPACT_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Database
        Traktor.Compress
)

target_include_directories(Traktor.Database.Compact PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Database.Local

set(TRAKTOR_DATABASE_LOCAL_SOURCES
	Local/Action.h
	Local/ActionRemove.h
	Local/ActionRemoveAllData.h
	Local/ActionSetGuid.h
	Local/ActionSetName.h
	Local/ActionWriteData.h
	Local/ActionWriteObject.h
	Local/Context.h
	Local/DefaultFileStore.h
	Local/IFileStore.h
	Local/LocalBus.h
	Local/LocalDatabase.h
	Local/LocalFileLink.h
	Local/LocalGroup.h
	Local/LocalInstance.h
	Local/LocalInstanceMeta.h
	Local/NoFileStore.h
	Local/PhysicalAccess.h
	Local/Transaction.h
	Local/Action.cpp
	Local/ActionRemove.cpp
	Local/ActionRemoveAllData.cpp
	Local/ActionSetGuid.cpp
	Local/ActionSetName.cpp
	Local/ActionWriteData.cpp
	Local/ActionWriteObject.cpp
	Local/Context.cpp
	Local/DefaultFileStore.cpp
	Local/IFileStore.cpp
	Local/LocalBus.cpp
	Local/LocalDatabase.cpp
	Local/LocalFileLink.cpp
	Local/LocalGroup.cpp
	Local/LocalInstance.cpp
	Local/LocalInstanceMeta.cpp
	Local/Module.cpp
	Local/NoFileStore.cpp
	Local/PhysicalAccess.cpp
	Local/Transaction.cpp
)

add_traktor_module(Traktor.Database.Local
    SOURCES ${TRAKTOR_DATABASE_LOCAL_SOURCES}
    EXPORT_DEFINE T_DATABASE_LOCAL_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Database
        Traktor.Xml
)

target_include_directories(Traktor.Database.Local PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Database.Migrate.App - Application

set(TRAKTOR_DATABASE_MIGRATE_APP_SOURCES
	Migrate/Main.cpp
)

add_traktor_module(Traktor.Database.Migrate.App
    SOURCES ${TRAKTOR_DATABASE_MIGRATE_APP_SOURCES}
    IS_EXECUTABLE
    DEPENDENCIES
        Traktor.Core
        Traktor.Database
        Traktor.Xml
        Extern.expat
        Extern.liblzf
        Extern.mimalloc
        Extern.zlib
    SYSTEM_LIBS
        Dbghelp.lib
        Winmm.lib
        shlwapi.lib
)

target_include_directories(Traktor.Database.Migrate.App PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Database.Remote

set(TRAKTOR_DATABASE_REMOTE_SOURCES
	Remote/IMessage.h
	Remote/Messages/CnmReleaseObject.h
	Remote/Messages/DbmClose.h
	Remote/Messages/DbmCloseTransaction.h
	Remote/Messages/DbmCommitTransaction.h
	Remote/Messages/DbmCreateGroup.h
	Remote/Messages/DbmCreateInstance.h
	Remote/Messages/DbmGetBus.h
	Remote/Messages/DbmGetChildren.h
	Remote/Messages/DbmGetDataNames.h
	Remote/Messages/DbmGetEvent.h
	Remote/Messages/DbmGetEventResult.h
	Remote/Messages/DbmGetGroupName.h
	Remote/Messages/DbmGetInstanceGuid.h
	Remote/Messages/DbmGetInstanceModifiedTime.h
	Remote/Messages/DbmGetInstanceName.h
	Remote/Messages/DbmGetInstancePrimaryType.h
	Remote/Messages/DbmGetInstanceRevision.h
	Remote/Messages/DbmGetRootGroup.h
	Remote/Messages/DbmOpen.h
	Remote/Messages/DbmOpenTransaction.h
	Remote/Messages/DbmPutEvent.h
	Remote/Messages/DbmReadData.h
	Remote/Messages/DbmReadObject.h
	Remote/Messages/DbmReadObjectResult.h
	Remote/Messages/DbmRemoveAllData.h
	Remote/Messages/DbmRemoveGroup.h
	Remote/Messages/DbmRemoveInstance.h
	Remote/Messages/DbmRenameGroup.h
	Remote/Messages/DbmSetInstanceGuid.h
	Remote/Messages/DbmSetInstanceName.h
	Remote/Messages/DbmWriteData.h
	Remote/Messages/DbmWriteObject.h
	Remote/Messages/DbmWriteObjectResult.h
	Remote/Messages/MsgEpochResult.h
	Remote/Messages/MsgGetChildrenResult.h
	Remote/Messages/MsgGuidResult.h
	Remote/Messages/MsgHandleArrayResult.h
	Remote/Messages/MsgHandleResult.h
	Remote/Messages/MsgIntResult.h
	Remote/Messages/MsgStatus.h
	Remote/Messages/MsgStringArrayResult.h
	Remote/Messages/MsgStringResult.h
	Remote/IMessage.cpp
	Remote/Messages/CnmReleaseObject.cpp
	Remote/Messages/DbmClose.cpp
	Remote/Messages/DbmCloseTransaction.cpp
	Remote/Messages/DbmCommitTransaction.cpp
	Remote/Messages/DbmCreateGroup.cpp
	Remote/Messages/DbmCreateInstance.cpp
	Remote/Messages/DbmGetBus.cpp
	Remote/Messages/DbmGetChildren.cpp
	Remote/Messages/DbmGetDataNames.cpp
	Remote/Messages/DbmGetEvent.cpp
	Remote/Messages/DbmGetEventResult.cpp
	Remote/Messages/DbmGetGroupName.cpp
	Remote/Messages/DbmGetInstanceGuid.cpp
	Remote/Messages/DbmGetInstanceModifiedTime.cpp
	Remote/Messages/DbmGetInstanceName.cpp
	Remote/Messages/DbmGetInstancePrimaryType.cpp
	Remote/Messages/DbmGetInstanceRevision.cpp
	Remote/Messages/DbmGetRootGroup.cpp
	Remote/Messages/DbmOpen.cpp
	Remote/Messages/DbmOpenTransaction.cpp
	Remote/Messages/DbmPutEvent.cpp
	Remote/Messages/DbmReadData.cpp
	Remote/Messages/DbmReadObject.cpp
	Remote/Messages/DbmReadObjectResult.cpp
	Remote/Messages/DbmRemoveAllData.cpp
	Remote/Messages/DbmRemoveGroup.cpp
	Remote/Messages/DbmRemoveInstance.cpp
	Remote/Messages/DbmRenameGroup.cpp
	Remote/Messages/DbmSetInstanceGuid.cpp
	Remote/Messages/DbmSetInstanceName.cpp
	Remote/Messages/DbmWriteData.cpp
	Remote/Messages/DbmWriteObject.cpp
	Remote/Messages/DbmWriteObjectResult.cpp
	Remote/Messages/MsgEpochResult.cpp
	Remote/Messages/MsgGetChildrenResult.cpp
	Remote/Messages/MsgGuidResult.cpp
	Remote/Messages/MsgHandleArrayResult.cpp
	Remote/Messages/MsgHandleResult.cpp
	Remote/Messages/MsgIntResult.cpp
	Remote/Messages/MsgStatus.cpp
	Remote/Messages/MsgStringArrayResult.cpp
	Remote/Messages/MsgStringResult.cpp
	Remote/Module.cpp
)

add_traktor_module(Traktor.Database.Remote
    SOURCES ${TRAKTOR_DATABASE_REMOTE_SOURCES}
    EXPORT_DEFINE T_DATABASE_REMOTE_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Net
        Traktor.Database
)

target_include_directories(Traktor.Database.Remote PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Database.Remote.Client

set(TRAKTOR_DATABASE_REMOTE_CLIENT_SOURCES
	Remote/Client/RemoteBus.h
	Remote/Client/RemoteConnection.h
	Remote/Client/RemoteDatabase.h
	Remote/Client/RemoteGroup.h
	Remote/Client/RemoteInstance.h
	Remote/Client/Module.cpp
	Remote/Client/RemoteBus.cpp
	Remote/Client/RemoteConnection.cpp
	Remote/Client/RemoteDatabase.cpp
	Remote/Client/RemoteGroup.cpp
	Remote/Client/RemoteInstance.cpp
)

add_traktor_module(Traktor.Database.Remote.Client
    SOURCES ${TRAKTOR_DATABASE_REMOTE_CLIENT_SOURCES}
    EXPORT_DEFINE T_DATABASE_REMOTE_CLIENT_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Database
        Traktor.Database.Remote
        Traktor.Net
)

target_include_directories(Traktor.Database.Remote.Client PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Database.Remote.Server

set(TRAKTOR_DATABASE_REMOTE_SERVER_SOURCES
	Remote/Server/BusMessageListener.h
	Remote/Server/Connection.h
	Remote/Server/ConnectionManager.h
	Remote/Server/ConnectionMessageListener.h
	Remote/Server/DatabaseMessageListener.h
	Remote/Server/GroupMessageListener.h
	Remote/Server/IMessageListener.h
	Remote/Server/IMessageListenerImpl.h
	Remote/Server/InstanceMessageListener.h
	Remote/Server/BusMessageListener.cpp
	Remote/Server/Connection.cpp
	Remote/Server/ConnectionManager.cpp
	Remote/Server/ConnectionMessageListener.cpp
	Remote/Server/DatabaseMessageListener.cpp
	Remote/Server/GroupMessageListener.cpp
	Remote/Server/IMessageListener.cpp
	Remote/Server/InstanceMessageListener.cpp
	Remote/Server/Module.cpp
)

add_traktor_module(Traktor.Database.Remote.Server
    SOURCES ${TRAKTOR_DATABASE_REMOTE_SERVER_SOURCES}
    EXPORT_DEFINE T_DATABASE_REMOTE_SERVER_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Database
        Traktor.Database.Compact
        Traktor.Database.Local
        Traktor.Database.Remote
        Traktor.Net
)

target_include_directories(Traktor.Database.Remote.Server PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)