# Traktor.Runtime - Runtime

set(TRAKTOR_RUNTIME_SOURCES
	Engine/AudioLayer.h
	Engine/AudioLayerData.h
	Engine/DebugLayer.h
	Engine/Layer.h
	Engine/LayerData.h
	Engine/RuntimePlugin.h
	Engine/ScreenLayer.h
	Engine/ScreenLayerData.h
	Engine/SplitWorldLayer.h
	Engine/SplitWorldLayerData.h
	Engine/Stage.h
	Engine/StageData.h
	Engine/StageLoader.h
	Engine/StageState.h
	Engine/VideoLayer.h
	Engine/VideoLayerData.h
	Engine/WorldLayer.h
	Engine/WorldLayerData.h
	Events/ActiveEvent.h
	Events/HotReloadEvent.h
	Events/ReconfigureEvent.h
	GameClassFactory.h
	IApplication.h
	IAudioServer.h
	IEnvironment.h
	IInputServer.h
	IOnlineServer.h
	IPhysicsServer.h
	IRenderServer.h
	IResourceServer.h
	IRuntimePlugin.h
	IScriptServer.h
	IServer.h
	IState.h
	IStateManager.h
	IWorldServer.h
	Impl/Application.h
	Impl/AudioServer.h
	Impl/Environment.h
	Impl/InputServer.h
	Impl/OnlineServer.h
	Impl/PhysicsServer.h
	Impl/RenderServer.h
	Impl/RenderServerDefault.h
	Impl/RenderServerEmbedded.h
	Impl/ResourceServer.h
	Impl/ScriptServer.h
	Impl/StateManager.h
	Impl/WorldServer.h
	Module.h
	Target/CommandEvent.h
	Target/IRemoteEvent.h
	Target/TargetID.h
	Target/TargetLog.h
	Target/TargetManagerConnection.h
	Target/TargetPerformance.h
	Target/TargetProfilerDictionary.h
	Target/TargetProfilerEvents.h
	Types.h
	UpdateControl.h
	UpdateInfo.h
	Engine/AudioLayer.cpp
	Engine/AudioLayerData.cpp
	Engine/DebugLayer.cpp
	Engine/Layer.cpp
	Engine/LayerData.cpp
	Engine/RuntimePlugin.cpp
	Engine/ScreenLayer.cpp
	Engine/ScreenLayerData.cpp
	Engine/SplitWorldLayer.cpp
	Engine/SplitWorldLayerData.cpp
	Engine/Stage.cpp
	Engine/StageData.cpp
	Engine/StageLoader.cpp
	Engine/StageState.cpp
	Engine/VideoLayer.cpp
	Engine/VideoLayerData.cpp
	Engine/WorldLayer.cpp
	Engine/WorldLayerData.cpp
	Events/ActiveEvent.cpp
	Events/HotReloadEvent.cpp
	Events/ReconfigureEvent.cpp
	GameClassFactory.cpp
	IApplication.cpp
	IAudioServer.cpp
	IEnvironment.cpp
	IInputServer.cpp
	IOnlineServer.cpp
	IPhysicsServer.cpp
	IRenderServer.cpp
	IResourceServer.cpp
	IRuntimePlugin.cpp
	IScriptServer.cpp
	IServer.cpp
	IState.cpp
	IStateManager.cpp
	IWorldServer.cpp
	Impl/Application.cpp
	Impl/AudioServer.cpp
	Impl/Environment.cpp
	Impl/InputServer.cpp
	Impl/OnlineServer.cpp
	Impl/PhysicsServer.cpp
	Impl/RenderServer.cpp
	Impl/RenderServerDefault.cpp
	Impl/RenderServerEmbedded.cpp
	Impl/ResourceServer.cpp
	Impl/ScriptServer.cpp
	Impl/StateManager.cpp
	Impl/WorldServer.cpp
	Module.cpp
	Target/CommandEvent.cpp
	Target/IRemoteEvent.cpp
	Target/TargetID.cpp
	Target/TargetLog.cpp
	Target/TargetManagerConnection.cpp
	Target/TargetPerformance.cpp
	Target/TargetProfilerDictionary.cpp
	Target/TargetProfilerEvents.cpp
	UpdateControl.cpp
	UpdateInfo.cpp
)

add_traktor_module(Traktor.Runtime
    SOURCES ${TRAKTOR_RUNTIME_SOURCES}
    EXPORT_DEFINE T_RUNTIME_EXPORT
    DEPENDENCIES
        Traktor.Animation
        Traktor.Core
        Traktor.Database
        Traktor.I18N
        Traktor.Input
        Traktor.Mesh
        Traktor.Net
        Traktor.Physics
        Traktor.Render
        Traktor.Resource
        Traktor.Scene
        Traktor.Script
        Traktor.Sound
        Traktor.Spray
        Traktor.Terrain
        Traktor.Video
        Traktor.Weather
        Traktor.World
        Traktor.Xml
        Traktor.Online
        Traktor.Heightfield
        Traktor.Ai
        Traktor.Drawing
        Traktor.Theater
)

target_include_directories(Traktor.Runtime PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Runtime.App - Runtime

set(TRAKTOR_RUNTIME_APP_SOURCES
	App/Win32/ErrorDialog.h
	App/Module.cpp
	App/Win32/ErrorDialog.cpp
	App/Win32/Main.cpp
)

add_traktor_module(Traktor.Runtime.App
    SOURCES ${TRAKTOR_RUNTIME_APP_SOURCES}
    IS_EXECUTABLE
    IS_GUI
    DEPENDENCIES
        Traktor.Core
        Traktor.Runtime
        Traktor.Ui
        Traktor.Xml
        Traktor.Ui.Win32
        Traktor.Script
    SYSTEM_LIBS
        Winmm.lib
)

target_include_directories(Traktor.Runtime.App PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Generate resource headers from source files (.png, .image, etc.)
add_resource_headers(Traktor.Runtime.App "${CMAKE_CURRENT_SOURCE_DIR}/App")

# Traktor.Runtime.Deploy - Runtime

set(TRAKTOR_RUNTIME_DEPLOY_SOURCES
	Editor/Deploy/BuildTargetAction.h
	Editor/Deploy/DeployTargetAction.h
	Editor/Deploy/DeployTool.h
	Editor/Deploy/Feature.h
	Editor/Deploy/ITargetAction.h
	Editor/Deploy/LaunchTargetAction.h
	Editor/Deploy/MigrateTargetAction.h
	Editor/Deploy/Platform.h
	Editor/Deploy/Target.h
	Editor/Deploy/TargetConfiguration.h
	Editor/Deploy/BuildTargetAction.cpp
	Editor/Deploy/DeployTargetAction.cpp
	Editor/Deploy/DeployTool.cpp
	Editor/Deploy/Feature.cpp
	Editor/Deploy/ITargetAction.cpp
	Editor/Deploy/LaunchTargetAction.cpp
	Editor/Deploy/MigrateTargetAction.cpp
	Editor/Deploy/Platform.cpp
	Editor/Deploy/Target.cpp
	Editor/Deploy/TargetConfiguration.cpp
)

add_traktor_module(Traktor.Runtime.Deploy
    SOURCES ${TRAKTOR_RUNTIME_DEPLOY_SOURCES}
    EXPORT_DEFINE T_RUNTIME_DEPLOY_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Database
        Traktor.Xml
)

target_include_directories(Traktor.Runtime.Deploy PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Runtime.Deploy.App - Runtime

set(TRAKTOR_RUNTIME_DEPLOY_APP_SOURCES
	Editor/Deploy/App/Main.cpp
)

add_traktor_module(Traktor.Runtime.Deploy.App
    SOURCES ${TRAKTOR_RUNTIME_DEPLOY_APP_SOURCES}
    IS_EXECUTABLE
    DEPENDENCIES
        Traktor.Runtime.Deploy
        Traktor.Database
        Traktor.Database.Local
        Traktor.Xml
        Traktor.Core
        Extern.expat
        Extern.liblzf
        Extern.mimalloc
        Extern.zlib
    SYSTEM_LIBS
        Dbghelp.lib
        Winmm.lib
        shlwapi.lib
)

target_include_directories(Traktor.Runtime.Deploy.App PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Runtime.Editor - Editor support

set(TRAKTOR_RUNTIME_EDITOR_SOURCES
	Editor/DeploySettingsPage.h
	Editor/EditorPlugin.h
	Editor/FeatureEditor.h
	Editor/FeatureEditorFactory.h
	Editor/GameEntityWizardDialog.h
	Editor/GameEntityWizardTool.h
	Editor/HostEnumerator.h
	Editor/LaunchBlenderTool.h
	Editor/LaunchRenderDocTool.h
	Editor/LaunchSolutionBuilderTool.h
	Editor/LaunchThemeEditorTool.h
	Editor/ProfilerDialog.h
	Editor/StagePipeline.h
	Editor/TargetConnection.h
	Editor/TargetEditor.h
	Editor/TargetEditorFactory.h
	Editor/TargetInstance.h
	Editor/TargetManager.h
	Editor/Ui/ButtonCell.h
	Editor/Ui/DropListCell.h
	Editor/Ui/ProgressCell.h
	Editor/Ui/TargetBuildEvent.h
	Editor/Ui/TargetCaptureEvent.h
	Editor/Ui/TargetCommandEvent.h
	Editor/Ui/TargetInstanceListItem.h
	Editor/Ui/TargetListControl.h
	Editor/Ui/TargetMigrateEvent.h
	Editor/Ui/TargetPlayEvent.h
	Editor/Ui/TargetStopEvent.h
	Editor/DeploySettingsPage.cpp
	Editor/EditorPlugin.cpp
	Editor/FeatureEditor.cpp
	Editor/FeatureEditorFactory.cpp
	Editor/GameEntityWizardDialog.cpp
	Editor/GameEntityWizardTool.cpp
	Editor/HostEnumerator.cpp
	Editor/LaunchBlenderTool.cpp
	Editor/LaunchRenderDocTool.cpp
	Editor/LaunchSolutionBuilderTool.cpp
	Editor/LaunchThemeEditorTool.cpp
	Editor/ProfilerDialog.cpp
	Editor/StagePipeline.cpp
	Editor/TargetConnection.cpp
	Editor/TargetEditor.cpp
	Editor/TargetEditorFactory.cpp
	Editor/TargetInstance.cpp
	Editor/TargetManager.cpp
	Editor/Ui/ButtonCell.cpp
	Editor/Ui/DropListCell.cpp
	Editor/Ui/ProgressCell.cpp
	Editor/Ui/TargetBuildEvent.cpp
	Editor/Ui/TargetCaptureEvent.cpp
	Editor/Ui/TargetCommandEvent.cpp
	Editor/Ui/TargetInstanceListItem.cpp
	Editor/Ui/TargetListControl.cpp
	Editor/Ui/TargetMigrateEvent.cpp
	Editor/Ui/TargetPlayEvent.cpp
	Editor/Ui/TargetStopEvent.cpp
)

add_traktor_module(Traktor.Runtime.Editor
    SOURCES ${TRAKTOR_RUNTIME_EDITOR_SOURCES}
    EXPORT_DEFINE T_RUNTIME_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Database.Local
        Traktor.Database.Remote.Server
        Traktor.Editor
        Traktor.I18N
        Traktor.Net
        Traktor.Ui
        Traktor.Database
        Traktor.Xml
        Traktor.Input
        Traktor.Script
        Traktor.Script.Editor
        Traktor.Drawing
        Traktor.Scene.Editor
        Traktor.Runtime
        Traktor.Runtime.Deploy
        Traktor.Mesh
        Traktor.Mesh.Editor
        Traktor.Physics
        Traktor.Physics.Editor
        Traktor.World
        Traktor.World.Editor
        Traktor.Model
        Traktor.Animation
        Traktor.Animation.Editor
)

target_include_directories(Traktor.Runtime.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Generate resource headers from source files (.png, .image, etc.)
add_resource_headers(Traktor.Runtime.Editor "${CMAKE_CURRENT_SOURCE_DIR}/Editor")