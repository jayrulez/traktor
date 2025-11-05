# Traktor.I18N

set(TRAKTOR_I18N_SOURCES
	Dictionary.h
	Format.h
	I18N.h
	I18NClassFactory.h
	Module.h
	Text.h
	Dictionary.cpp
	Format.cpp
	I18N.cpp
	I18NClassFactory.cpp
	Module.cpp
	Text.cpp
)

add_traktor_module(Traktor.I18N
    SOURCES ${TRAKTOR_I18N_SOURCES}
    EXPORT_DEFINE T_I18N_EXPORT
    DEPENDENCIES
        Traktor.Core
)

target_include_directories(Traktor.I18N PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.I18N.Editor - Editor support

set(TRAKTOR_I18N_EDITOR_SOURCES
	Editor/DictionaryAsset.h
	Editor/DictionaryEditorPage.h
	Editor/DictionaryEditorPageFactory.h
	Editor/DictionaryFormatCSV.h
	Editor/DictionaryFormatXLSX.h
	Editor/DictionaryPipeline.h
	Editor/IDictionaryFormat.h
	Editor/ImportDictionaryWizardTool.h
	Editor/Translator.h
	Editor/DictionaryAsset.cpp
	Editor/DictionaryEditorPage.cpp
	Editor/DictionaryEditorPageFactory.cpp
	Editor/DictionaryFormatCSV.cpp
	Editor/DictionaryFormatXLSX.cpp
	Editor/DictionaryPipeline.cpp
	Editor/IDictionaryFormat.cpp
	Editor/ImportDictionaryWizardTool.cpp
	Editor/Translator.cpp
)

add_traktor_module(Traktor.I18N.Editor
    SOURCES ${TRAKTOR_I18N_EDITOR_SOURCES}
    EXPORT_DEFINE T_I18N_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Editor
        Traktor.I18N
        Traktor.Ui
        Traktor.Net
)

target_include_directories(Traktor.I18N.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
