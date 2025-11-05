# Traktor.Editor - Editor support

set(TRAKTOR_EDITOR_SOURCES
    Asset.cpp
    Assets.cpp
    AssetsPipeline.cpp
    DataAccessCache.cpp
    DefaultPipeline.cpp
    IBrowsePreview.cpp
    IDocument.cpp
    IEditorPage.cpp
    IEditorPageFactory.cpp
    IEditorPageSite.cpp
    IEditorPlugin.cpp
    IEditorTool.cpp
    IObjectEditor.cpp
    IObjectEditorFactory.cpp
    IPipeline.cpp
    IPipelineBuilder.cpp
    IPipelineCache.cpp
    IPipelineCommon.cpp
    IPipelineDb.cpp
    IPipelineDepends.cpp
    IPipelineInstanceCache.cpp
    IPipelineSettings.cpp
    ISettingsPage.cpp
    IThumbnailGenerator.cpp
    IWizardTool.cpp
    LogView.cpp
    Pipeline/Avalanche/AvalanchePipelineCache.cpp
    Pipeline/File/FilePipelineCache.cpp
    Pipeline/File/FilePipelinePutStream.cpp
    Pipeline/Memory/MemoryPipelineCache.cpp
    Pipeline/PipelineBuilder.cpp
    Pipeline/PipelineDbFlat.cpp
    Pipeline/PipelineDependsIncremental.cpp
    Pipeline/PipelineDependsParallel.cpp
    Pipeline/PipelineFactory.cpp
    Pipeline/PipelineInstanceCache.cpp
    Pipeline/PipelineProfiler.cpp
    Pipeline/PipelineSettings.cpp
    PipelineDependency.cpp
    PipelineDependencySet.cpp
    PropertiesView.cpp
    TypeBrowseFilter.cpp
)

add_traktor_module(Traktor.Editor
    SOURCES ${TRAKTOR_EDITOR_SOURCES}
    EXPORT_DEFINE T_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Ui
        Traktor.Database
        Traktor.Net
        Traktor.Sql
        Traktor.Sql.Sqlite3
        Traktor.Database.Remote
        Traktor.Database.Remote.Server
        Traktor.Avalanche
        Traktor.Compress
        Traktor.I18N
)

target_include_directories(Traktor.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Editor.App - Editor support

set(TRAKTOR_EDITOR_APP_SOURCES
    App/BrowseGroupDialog.cpp
    App/BrowseInstanceDialog.cpp
    App/BrowseTypeDialog.cpp
    App/CheckAssetsTool.cpp
    App/CheckSerializableTool.cpp
    App/ClearCachesTool.cpp
    App/ColorSettingsPage.cpp
    App/DatabaseView.cpp
    App/DefaultObjectEditor.cpp
    App/DefaultObjectEditorFactory.cpp
    App/DefaultPropertiesView.cpp
    App/DeleteEmptyGroupsTool.cpp
    App/DependenciesAsDotWizardTool.cpp
    App/DiscoveryPlugin.cpp
    App/Document.cpp
    App/EditorForm.cpp
    App/EditorMain.cpp
    App/EditorPageSite.cpp
    App/EditorPluginSite.cpp
    App/GeneralSettingsPage.cpp
    App/ImportAssetTool.cpp
    App/InstanceClipboardData.cpp
    App/MRU.cpp
    App/ModulesSettingsPage.cpp
    App/NewInstanceDialog.cpp
    App/NewWorkspaceDialog.cpp
    App/ObjectEditor.cpp
    App/ObjectEditorDialog.cpp
    App/PipelineSettingsPage.cpp
    App/PublishWizardTool.cpp
    App/QuickOpenDialog.cpp
    App/SaveAsDialog.cpp
    App/SearchTool.cpp
    App/SearchToolDialog.cpp
    App/SettingsDialog.cpp
    App/Shortcut.cpp
    App/ShortcutsSettingsPage.cpp
    App/Splash.cpp
    App/TextEditorDialog.cpp
    App/ThumbnailGenerator.cpp
    App/TouchAllTool.cpp
    App/WorkspaceDialog.cpp
)

add_traktor_module(Traktor.Editor.App
    SOURCES ${TRAKTOR_EDITOR_APP_SOURCES}
    EXPORT_DEFINE T_EDITOR_APP_EXPORT
    IS_EXECUTABLE
    IS_GUI
    DEPENDENCIES
        Traktor.Core
        Traktor.Ui
        Traktor.Drawing
        Traktor.Database
        Traktor.Xml
        Traktor.Editor
        Traktor.I18N
        Traktor.Database.Compact
        Traktor.Database.Local
        Traktor.Database.Remote
        Traktor.Database.Remote.Server
        Traktor.Net
        Traktor.Ui.Win32
        Extern.agg
        Extern.expat
        Extern.jpeg
        Extern.liblzf
        Extern.lpng
        Extern.mimalloc
        Extern.sqlite
        Extern.tinyexr
        Extern.zlib
    SYSTEM_LIBS
        Dbghelp.lib
        Winmm.lib
        d2d1.lib
        dwrite.lib
        dxguid.lib
        gdiplus.lib
        iphlpapi.lib
        shlwapi.lib
        ws2_32.lib
)

target_include_directories(Traktor.Editor.App PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Generate resource headers from source files (.png, .image, etc.)
add_resource_headers(Traktor.Editor.App "${CMAKE_CURRENT_SOURCE_DIR}/App")

# Traktor.Editor.Build.App - Editor support

set(TRAKTOR_EDITOR_BUILD_APP_SOURCES
    Build/App/Main.cpp
)

add_traktor_module(Traktor.Editor.Build.App
    SOURCES ${TRAKTOR_EDITOR_BUILD_APP_SOURCES}
    EXPORT_DEFINE T_EDITOR_BUILD_APP_EXPORT
    IS_EXECUTABLE
    DEPENDENCIES
        Traktor.Core
        Traktor.Editor
        Traktor.Xml
        Traktor.Database
        Extern.agg
        Extern.expat
        Extern.jpeg
        Extern.liblzf
        Extern.lpng
        Extern.mimalloc
        Extern.sqlite
        Extern.tinyexr
        Extern.zlib
    SYSTEM_LIBS
        Dbghelp.lib
        Winmm.lib
        iphlpapi.lib
        shlwapi.lib
        ws2_32.lib
)

target_include_directories(Traktor.Editor.Build.App PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)