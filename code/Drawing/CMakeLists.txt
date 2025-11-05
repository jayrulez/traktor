# Traktor.Drawing

set(TRAKTOR_DRAWING_SOURCES
    CubeMap.cpp
    DrawingClassFactory.cpp
    Filters/BlurFilter.cpp
    Filters/BrightnessContrastFilter.cpp
    Filters/ChainFilter.cpp
    Filters/ConvolutionFilter.cpp
    Filters/CropFilter.cpp
    Filters/DilateFilter.cpp
    Filters/EncodeRGBM.cpp
    Filters/GammaFilter.cpp
    Filters/GaussianBlurFilter.cpp
    Filters/GrayscaleFilter.cpp
    Filters/MirrorFilter.cpp
    Filters/NoiseFilter.cpp
    Filters/NormalMapFilter.cpp
    Filters/NormalizeFilter.cpp
    Filters/PerlinNoiseFilter.cpp
    Filters/PremultiplyAlphaFilter.cpp
    Filters/QuantizeFilter.cpp
    Filters/ScaleFilter.cpp
    Filters/SeparateAlphaFilter.cpp
    Filters/SharpenFilter.cpp
    Filters/SphereMapFilter.cpp
    Filters/SwizzleFilter.cpp
    Filters/TonemapFilter.cpp
    Filters/TransformFilter.cpp
    Formats/ImageFormatBmp.cpp
    Formats/ImageFormatDds.cpp
    Formats/ImageFormatExr.cpp
    Formats/ImageFormatGif.cpp
    Formats/ImageFormatHdr.cpp
    Formats/ImageFormatIco.cpp
    Formats/ImageFormatJpeg.cpp
    Formats/ImageFormatPcx.cpp
    Formats/ImageFormatPng.cpp
    Formats/ImageFormatTga.cpp
    Formats/ImageFormatTri.cpp
    Functions/BlendFunction.cpp
    IImageFilter.cpp
    IImageFormat.cpp
    ITransferFunction.cpp
    Image.cpp
    ImageInfo.cpp
    Module.cpp
    Palette.cpp
    PixelFormat.cpp
    Raster.cpp
)

add_traktor_module(Traktor.Drawing
    SOURCES ${TRAKTOR_DRAWING_SOURCES}
    EXPORT_DEFINE T_DRAWING_EXPORT
    DEPENDENCIES
        Traktor.Core
        Extern.agg
        Extern.jpeg
        Extern.lpng
        Extern.tinyexr
        Extern.zlib
)

target_include_directories(Traktor.Drawing PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)