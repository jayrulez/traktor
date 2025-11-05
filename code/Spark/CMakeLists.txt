# Traktor.Spark

set(TRAKTOR_SPARK_SOURCES
    Acc/AccDisplayRenderer.cpp
    Acc/AccGlyph.cpp
    Acc/AccGradientCache.cpp
    Acc/AccQuad.cpp
    Acc/AccShape.cpp
    Acc/AccShapeResources.cpp
    Acc/AccShapeVertexPool.cpp
    Acc/AccTextureCache.cpp
    Bitmap.cpp
    BitmapImage.cpp
    BitmapResource.cpp
    BitmapTexture.cpp
    Button.cpp
    ButtonInstance.cpp
    Canvas.cpp
    Character.cpp
    CharacterInstance.cpp
    ClassFactory.cpp
    ColorTransform.cpp
    Context.cpp
    Debug/ButtonInstanceDebugInfo.cpp
    Debug/CaptureControl.cpp
    Debug/EditInstanceDebugInfo.cpp
    Debug/FrameDebugInfo.cpp
    Debug/InstanceDebugInfo.cpp
    Debug/MorphShapeInstanceDebugInfo.cpp
    Debug/MovieDebugInfo.cpp
    Debug/MovieDebugger.cpp
    Debug/ShapeInstanceDebugInfo.cpp
    Debug/SpriteInstanceDebugInfo.cpp
    Debug/TextInstanceDebugInfo.cpp
    Debug/WireDisplayRenderer.cpp
    DefaultCharacterFactory.cpp
    Dictionary.cpp
    DisplayList.cpp
    Edit.cpp
    EditInstance.cpp
    Event.cpp
    FillStyle.cpp
    Font.cpp
    Frame.cpp
    ICharacterFactory.cpp
    IDisplayRenderer.cpp
    IMovieLoader.cpp
    ISoundRenderer.cpp
    Key.cpp
    LineStyle.cpp
    Module.cpp
    MorphShape.cpp
    MorphShapeInstance.cpp
    Mouse.cpp
    Movie.cpp
    MovieLoader.cpp
    MoviePlayer.cpp
    MovieRenderer.cpp
    MovieResourceFactory.cpp
    MovieResult.cpp
    Optimizer.cpp
    Packer.cpp
    Path.cpp
    Shape.cpp
    ShapeInstance.cpp
    Sound.cpp
    Sound/SoundBuffer.cpp
    Sound/SoundRenderer.cpp
    Sprite.cpp
    SpriteInstance.cpp
    Stage.cpp
    Sw/SwDisplayRenderer.cpp
    Swf/SwfMovieFactory.cpp
    Swf/SwfMovieFactoryTags.cpp
    Swf/SwfReader.cpp
    Text.cpp
    TextFormat.cpp
    TextInstance.cpp
    TextLayout.cpp
    Triangulator.cpp
    Types.cpp
)

add_traktor_module(Traktor.Spark
    SOURCES ${TRAKTOR_SPARK_SOURCES}
    EXPORT_DEFINE T_SPARK_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Database
        Traktor.Render
        Traktor.Resource
        Traktor.Compress
        Traktor.Drawing
        Traktor.Html
        Traktor.Sound
        Traktor.Net
        Traktor.Xml
)

target_include_directories(Traktor.Spark PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Spark.Editor - Editor support

set(TRAKTOR_SPARK_EDITOR_SOURCES
    Editor/BatchDialog.cpp
    Editor/BatchWizardTool.cpp
    Editor/ConvertFont.cpp
    Editor/ConvertImage.cpp
    Editor/ConvertSvg.cpp
    Editor/ConvertSwf.cpp
    Editor/Debug/ClientPage.cpp
    Editor/Debug/DebugView.cpp
    Editor/Debug/MainForm.cpp
    Editor/DebuggerTool.cpp
    Editor/EditorPage.cpp
    Editor/EditorPageFactory.cpp
    Editor/EmptyMovieAsset.cpp
    Editor/MovieAsset.cpp
    Editor/Pipeline.cpp
    Editor/PreviewControl.cpp
)

add_traktor_module(Traktor.Spark.Editor
    SOURCES ${TRAKTOR_SPARK_EDITOR_SOURCES}
    EXPORT_DEFINE T_SPARK_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Spark
        Traktor.Editor
        Traktor.Render
        Traktor.Resource
        Traktor.Ui
        Traktor.Drawing
        Traktor.Database
        Traktor.I18N
        Traktor.Sound
        Traktor.Sound.Editor
        Traktor.Render.Editor
        Traktor.Net
        Traktor.Xml
        Traktor.Svg
        Extern.freetype
)

target_include_directories(Traktor.Spark.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Generate resource headers from source files (.png, .image, etc.)
add_resource_headers(Traktor.Spark.Editor "${CMAKE_CURRENT_SOURCE_DIR}/Editor")

# Traktor.Spark.Runtime - Runtime

set(TRAKTOR_SPARK_RUNTIME_SOURCES
    Runtime/Module.cpp
    Runtime/SparkClassFactory.cpp
    Runtime/SparkLayer.cpp
    Runtime/SparkLayerData.cpp
)

add_traktor_module(Traktor.Spark.Runtime
    SOURCES ${TRAKTOR_SPARK_RUNTIME_SOURCES}
    EXPORT_DEFINE T_SPARK_RUNTIME_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Runtime
        Traktor.Spark
        Traktor.Net
        Traktor.Spray
        Traktor.Resource
        Traktor.Render
        Traktor.Input
)

target_include_directories(Traktor.Spark.Runtime PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Spark.Runtime.Editor - Editor support

set(TRAKTOR_SPARK_RUNTIME_EDITOR_SOURCES
    Runtime/Editor/SparkLayerPipeline.cpp
)

add_traktor_module(Traktor.Spark.Runtime.Editor
    SOURCES ${TRAKTOR_SPARK_RUNTIME_EDITOR_SOURCES}
    EXPORT_DEFINE T_SPARK_RUNTIME_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Spark
        Traktor.Editor
        Traktor.Spark.Runtime
)

target_include_directories(Traktor.Spark.Runtime.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)