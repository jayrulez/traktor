# Traktor.Script

set(TRAKTOR_SCRIPT_SOURCES
    IScriptBlob.cpp
    IScriptCompiler.cpp
    IScriptContext.cpp
    IScriptDebugger.cpp
    IScriptManager.cpp
    IScriptProfiler.cpp
    IValue.cpp
    Module.cpp
    Remote/RemoteScriptDebugger.cpp
    Remote/RemoteScriptProfiler.cpp
    Remote/ScriptDebuggerBreadcrumbs.cpp
    Remote/ScriptDebuggerBreakpoint.cpp
    Remote/ScriptDebuggerControl.cpp
    Remote/ScriptDebuggerLocals.cpp
    Remote/ScriptDebuggerStackFrame.cpp
    Remote/ScriptDebuggerStateChange.cpp
    Remote/ScriptDebuggerStatus.cpp
    Remote/ScriptProfilerCallMeasured.cpp
    ScriptChunk.cpp
    ScriptFactory.cpp
    ScriptResource.cpp
    StackFrame.cpp
    Value.cpp
    ValueObject.cpp
    Variable.cpp
)

add_traktor_module(Traktor.Script
    SOURCES ${TRAKTOR_SCRIPT_SOURCES}
    EXPORT_DEFINE T_SCRIPT_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Resource
        Traktor.Net
)

target_include_directories(Traktor.Script PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Script.Editor - Editor support

set(TRAKTOR_SCRIPT_EDITOR_SOURCES
    Editor/IScriptDebuggerSessions.cpp
    Editor/IScriptOutline.cpp
    Editor/Lexer.cpp
    Editor/LuaGrammar.cpp
    Editor/Script.cpp
    Editor/ScriptClassWizardTool.cpp
    Editor/ScriptClassesView.cpp
    Editor/ScriptDebuggerDialog.cpp
    Editor/ScriptDebuggerSessions.cpp
    Editor/ScriptDebuggerTool.cpp
    Editor/ScriptDebuggerView.cpp
    Editor/ScriptEditorPage.cpp
    Editor/ScriptEditorPageFactory.cpp
    Editor/ScriptEditorPlugin.cpp
    Editor/ScriptOutlineLua.cpp
    Editor/ScriptPipeline.cpp
    Editor/ScriptProfilerDialog.cpp
    Editor/ScriptProfilerTool.cpp
    Editor/ScriptProfilerView.cpp
    Editor/ScriptSettingsPage.cpp
)

add_traktor_module(Traktor.Script.Editor
    SOURCES ${TRAKTOR_SCRIPT_EDITOR_SOURCES}
    EXPORT_DEFINE T_SCRIPT_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Editor
        Traktor.Ui
        Traktor.Script
        Traktor.I18N
        Traktor.Database
)

target_include_directories(Traktor.Script.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Generate resource headers from source files (.png, .image, etc.)
add_resource_headers(Traktor.Script.Editor "${CMAKE_CURRENT_SOURCE_DIR}/Editor")

# Traktor.Script.Lua

set(TRAKTOR_SCRIPT_LUA_SOURCES
    Lua/Module.cpp
    Lua/ScriptBlobLua.cpp
    Lua/ScriptClassLua.cpp
    Lua/ScriptCompilerLua.cpp
    Lua/ScriptContextLua.cpp
    Lua/ScriptDebuggerLua.cpp
    Lua/ScriptDelegateLua.cpp
    Lua/ScriptManagerLua.cpp
    Lua/ScriptObjectLua.cpp
    Lua/ScriptProfilerLua.cpp
    Lua/ScriptUtilitiesLua.cpp
)

add_traktor_module(Traktor.Script.Lua
    SOURCES ${TRAKTOR_SCRIPT_LUA_SOURCES}
    EXPORT_DEFINE T_SCRIPT_LUA_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Script
        Extern.lua
)

target_include_directories(Traktor.Script.Lua PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Generate resource headers from source files (.lua, etc.)
add_resource_headers(Traktor.Script.Lua "${CMAKE_CURRENT_SOURCE_DIR}/Lua")