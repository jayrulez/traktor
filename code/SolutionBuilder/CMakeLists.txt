# Traktor.SolutionBuilder

set(TRAKTOR_SOLUTIONBUILDER_SOURCES
    Aggregation.cpp
    AggregationItem.cpp
    CBlocks/SolutionBuilderCBlocks.cpp
    ClassFactory.cpp
    Configuration.cpp
    Dependencies/SolutionBuilderDependencies.cpp
    Dependency.cpp
    Eclipse/SolutionBuilderEclipse.cpp
    ExternalDependency.cpp
    FBuild/SolutionBuilderFBuild.cpp
    File.cpp
    Filter.cpp
    GraphViz/SolutionBuilderGraphViz.cpp
    HeaderScanner.cpp
    Make/SolutionBuilderMake2.cpp
    Msvc/GeneratorContext.cpp
    Msvc/SolutionBuilderMsvc.cpp
    Msvc/SolutionBuilderMsvcSettings.cpp
    Msvc/SolutionBuilderMsvcVCXBuildTool.cpp
    Msvc/SolutionBuilderMsvcVCXClCompileBuildTool.cpp
    Msvc/SolutionBuilderMsvcVCXCustomBuildTool.cpp
    Msvc/SolutionBuilderMsvcVCXDefinition.cpp
    Msvc/SolutionBuilderMsvcVCXImport.cpp
    Msvc/SolutionBuilderMsvcVCXImportCommon.cpp
    Msvc/SolutionBuilderMsvcVCXImportGroup.cpp
    Msvc/SolutionBuilderMsvcVCXProj.cpp
    Msvc/SolutionBuilderMsvcVCXPropertyGroup.cpp
    Ninja/SolutionBuilderNinja.cpp
    Output.cpp
    Project.cpp
    ProjectDependency.cpp
    ProjectItem.cpp
    ScriptProcessor.cpp
    Solution.cpp
    SolutionBuilder.cpp
    SolutionLoader.cpp
    Utilities.cpp
    Xcode/SolutionBuilderXcode2.cpp
)

add_traktor_module(Traktor.SolutionBuilder
    SOURCES ${TRAKTOR_SOLUTIONBUILDER_SOURCES}
    EXPORT_DEFINE T_SOLUTIONBUILDER_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Script
        Traktor.Script.Lua
        Traktor.Xml
)

target_include_directories(Traktor.SolutionBuilder PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.SolutionBuilder.App - Application

set(TRAKTOR_SOLUTIONBUILDER_APP_SOURCES
    App/Main.cpp
)

add_traktor_module(Traktor.SolutionBuilder.App
    SOURCES ${TRAKTOR_SOLUTIONBUILDER_APP_SOURCES}
    EXPORT_DEFINE T_SOLUTIONBUILDER_APP_EXPORT
    IS_EXECUTABLE
    DEPENDENCIES
        Traktor.SolutionBuilder
        Traktor.Core
        Extern.expat
        Extern.lua
        Extern.mimalloc
    SYSTEM_LIBS
        Dbghelp.lib
        Winmm.lib
        iphlpapi.lib
        shlwapi.lib
        ws2_32.lib
)

target_include_directories(Traktor.SolutionBuilder.App PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.SolutionBuilder.Editor.App - Editor support

set(TRAKTOR_SOLUTIONBUILDER_EDITOR_APP_SOURCES
    Editor/App/AddAggregatesTool.cpp
    Editor/App/AddMultipleConfigurations.cpp
    Editor/App/AggregationItemPropertyPage.cpp
    Editor/App/ConfigurationPropertyPage.cpp
    Editor/App/ConfigurationsDialog.cpp
    Editor/App/CustomTool.cpp
    Editor/App/EditConfigurations.cpp
    Editor/App/ExtractSolutionDialog.cpp
    Editor/App/ExtractSolutionTool.cpp
    Editor/App/ImportMsvcProject.cpp
    Editor/App/ImportProject.cpp
    Editor/App/ImportProjectDialog.cpp
    Editor/App/MRU.cpp
    Editor/App/Main.cpp
    Editor/App/ProjectPropertyPage.cpp
    Editor/App/SolutionForm.cpp
    Editor/App/SolutionPropertyPage.cpp
)

add_traktor_module(Traktor.SolutionBuilder.Editor.App
    SOURCES ${TRAKTOR_SOLUTIONBUILDER_EDITOR_APP_SOURCES}
    EXPORT_DEFINE T_SOLUTIONBUILDER_EDITOR_APP_EXPORT
    IS_EXECUTABLE
    IS_GUI
    DEPENDENCIES
        Traktor.Core
        Traktor.SolutionBuilder
        Traktor.Ui
        Traktor.Ui.Win32
        Traktor.Drawing
        Traktor.Xml
        Extern.agg
        Extern.expat
        Extern.jpeg
        Extern.lpng
        Extern.lua
        Extern.mimalloc
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

target_include_directories(Traktor.SolutionBuilder.Editor.App PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)