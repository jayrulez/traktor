# Traktor modules

# Helper function to add Traktor modules
function(add_traktor_module MODULE_NAME)
    set(options IS_EXECUTABLE IS_SHARED IS_GUI)
    set(oneValueArgs EXPORT_DEFINE)
    set(multiValueArgs SOURCES DEPENDENCIES SYSTEM_LIBS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Determine library type
    if(ARG_IS_EXECUTABLE)
        add_executable(${MODULE_NAME} ${ARG_SOURCES})
    elseif(ARG_IS_SHARED OR BUILD_SHARED_LIBS)
        add_library(${MODULE_NAME} SHARED ${ARG_SOURCES})
    else()
        add_library(${MODULE_NAME} STATIC ${ARG_SOURCES})
    endif()

    # Add export definition for shared libraries
    if(ARG_EXPORT_DEFINE AND (ARG_IS_SHARED OR BUILD_SHARED_LIBS))
        target_compile_definitions(${MODULE_NAME} PRIVATE ${ARG_EXPORT_DEFINE})
    endif()

    # Add static definition for static libraries
    if(NOT ARG_IS_SHARED AND NOT BUILD_SHARED_LIBS AND NOT ARG_IS_EXECUTABLE)
        target_compile_definitions(${MODULE_NAME} PUBLIC T_STATIC)
    endif()

    # Link dependencies
    if(ARG_DEPENDENCIES)
        target_link_libraries(${MODULE_NAME} PUBLIC ${ARG_DEPENDENCIES})
    endif()

    # Link system libraries
    if(ARG_SYSTEM_LIBS)
        target_link_libraries(${MODULE_NAME} PRIVATE ${ARG_SYSTEM_LIBS})
    endif()

    # Set output name
    set_target_properties(${MODULE_NAME} PROPERTIES
        OUTPUT_NAME ${MODULE_NAME}
        FOLDER "Traktor"
    )

    # Set subsystem for Windows GUI executables only (use WinMain, not main)
    # Console applications should not have WIN32_EXECUTABLE set (they use main)
    if(ARG_IS_EXECUTABLE AND WIN32 AND ARG_IS_GUI)
        set_target_properties(${MODULE_NAME} PROPERTIES
            WIN32_EXECUTABLE TRUE
        )
    endif()

    # Add post-build copy commands to match old build system
    # Copy binaries to bin/latest/win64/<config> for non-executables
    if(WIN32 AND NOT ARG_IS_EXECUTABLE)
        # Determine library type suffix
        if(ARG_IS_SHARED OR BUILD_SHARED_LIBS)
            set(CONFIG_SUFFIX "shared")
        else()
            set(CONFIG_SUFFIX "static")
        endif()

        # Use generator expressions to copy to the appropriate configuration directory
        # Convert configuration to lowercase for directory name
        set(DEST_DIR "${TRAKTOR_HOME}/bin/latest/win64/$<LOWER_CASE:$<CONFIG>>${CONFIG_SUFFIX}")

        # Copy DLL (for shared libraries)
        if(ARG_IS_SHARED OR BUILD_SHARED_LIBS)
            add_custom_command(TARGET ${MODULE_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:${MODULE_NAME}>"
                    "${DEST_DIR}/"
                COMMENT "Copying ${MODULE_NAME} DLL to bin/latest/win64"
                VERBATIM
            )
            # Copy import library
            add_custom_command(TARGET ${MODULE_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_LINKER_FILE:${MODULE_NAME}>"
                    "${DEST_DIR}/"
                COMMENT "Copying ${MODULE_NAME} import lib to bin/latest/win64"
                VERBATIM
            )
        else()
            # Copy static library
            add_custom_command(TARGET ${MODULE_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory "${DEST_DIR}"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:${MODULE_NAME}>"
                    "${DEST_DIR}/"
                COMMENT "Copying ${MODULE_NAME} lib to bin/latest/win64"
                VERBATIM
            )
        endif()

        # Copy PDB files (debug symbols) - only for Debug/RelWithDebInfo
        add_custom_command(TARGET ${MODULE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_PDB_FILE:${MODULE_NAME}>"
                "${DEST_DIR}/"
            COMMENT "Copying ${MODULE_NAME} PDB to bin/latest/win64"
            VERBATIM
        )
    endif()

    # Install target
    if(ARG_IS_EXECUTABLE)
        install(TARGETS ${MODULE_NAME} RUNTIME DESTINATION bin)
    else()
        install(TARGETS ${MODULE_NAME}
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin
        )
    endif()
endfunction()

# Add subdirectories for each module
add_subdirectory(Core)
add_subdirectory(Compress)
add_subdirectory(Database)
add_subdirectory(Drawing)
add_subdirectory(I18N)
add_subdirectory(Json)
add_subdirectory(Net)
add_subdirectory(Resource)
add_subdirectory(Sql)
add_subdirectory(Xml)

# Editor support
add_subdirectory(Ui)
add_subdirectory(UiKit)
add_subdirectory(Editor)

# Graphics modules
add_subdirectory(Render)
add_subdirectory(Mesh)
add_subdirectory(Model)
add_subdirectory(Shape)
add_subdirectory(Svg)

# Animation modules
add_subdirectory(Animation)

# Scene modules
add_subdirectory(Scene)
add_subdirectory(Theater)
add_subdirectory(World)

# Physics modules
add_subdirectory(Physics)

# Input modules
add_subdirectory(Input)

# Sound modules
add_subdirectory(Sound)

# Script modules
add_subdirectory(Script)

# Other modules
add_subdirectory(Ai)
add_subdirectory(Avalanche)
add_subdirectory(Heightfield)
add_subdirectory(Html)
add_subdirectory(Jungle)
add_subdirectory(Online)
add_subdirectory(Terrain)
add_subdirectory(Weather)
add_subdirectory(Video)
add_subdirectory(Spark)
add_subdirectory(Spray)

# Runtime
add_subdirectory(Runtime)

# Tools and applications (optional)
option(BUILD_TOOLS "Build tools and applications" ON)
if(BUILD_TOOLS)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/SolutionBuilder)
        add_subdirectory(SolutionBuilder)
    endif()
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Pipeline)
        add_subdirectory(Pipeline)
    endif()
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Remote)
        add_subdirectory(Remote)
    endif()
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Run)
        add_subdirectory(Run)
    endif()
endif()
