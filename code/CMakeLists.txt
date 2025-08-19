# CMakeLists.txt for code directory
# This file should be placed in code/

# Add Core library
add_subdirectory(Core)
add_subdirectory(Xml)
add_subdirectory(Net)
add_subdirectory(Compress)
add_subdirectory(Database)
add_subdirectory(Drawing)
add_subdirectory(Svg)

# Add other libraries as they become available
# add_subdirectory(Editor)
# add_subdirectory(Engine)
# add_subdirectory(Heightfield)
# add_subdirectory(Input)
# add_subdirectory(Model)
# add_subdirectory(Physics)
# add_subdirectory(Render)
# add_subdirectory(Resource)
# add_subdirectory(Scene)
# add_subdirectory(Script)
# add_subdirectory(Sound)
# add_subdirectory(Spark)
# add_subdirectory(Spray)
# add_subdirectory(Ui)
# add_subdirectory(Video)
# add_subdirectory(World)

# Export all targets
#export(EXPORT TraktorTargets
#    FILE "${CMAKE_BINARY_DIR}/TraktorTargets.cmake"
#    NAMESPACE Traktor::
#)