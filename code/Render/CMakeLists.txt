# Traktor.Render

set(TRAKTOR_RENDER_SOURCES
    Buffer.cpp
    Context/ProgramParameters.cpp
    Context/RenderBlock.cpp
    Context/RenderContext.cpp
    Frame/RenderGraph.cpp
    Frame/RenderGraphBufferPool.cpp
    Frame/RenderGraphContext.cpp
    Frame/RenderGraphTargetSet.cpp
    Frame/RenderGraphTargetSetPool.cpp
    Frame/RenderGraphTexturePool.cpp
    Frame/RenderGraphTypes.cpp
    Frame/RenderPass.cpp
    IAccelerationStructure.cpp
    IBufferView.cpp
    IProgram.cpp
    IRenderSystem.cpp
    IRenderTargetSet.cpp
    IRenderView.cpp
    ITexture.cpp
    IVertexLayout.cpp
    Image2/AmbientOcclusion.cpp
    Image2/AmbientOcclusionData.cpp
    Image2/Compute.cpp
    Image2/ComputeData.cpp
    Image2/DirectionalBlur.cpp
    Image2/DirectionalBlurData.cpp
    Image2/ImageGraph.cpp
    Image2/ImageGraphContext.cpp
    Image2/ImageGraphData.cpp
    Image2/ImageGraphFactory.cpp
    Image2/ImagePass.cpp
    Image2/ImagePassData.cpp
    Image2/ImagePassStep.cpp
    Image2/ImagePassStepData.cpp
    Image2/ImageStructBuffer.cpp
    Image2/ImageStructBufferData.cpp
    Image2/ImageTargetSet.cpp
    Image2/ImageTargetSetData.cpp
    Image2/ImageTexture.cpp
    Image2/ImageTextureData.cpp
    Image2/ShadowProject.cpp
    Image2/ShadowProjectData.cpp
    Image2/Simple.cpp
    Image2/SimpleData.cpp
    Mesh/Mesh.cpp
    Mesh/MeshFactory.cpp
    Mesh/MeshReader.cpp
    Mesh/MeshWriter.cpp
    Mesh/RenderMeshFactory.cpp
    Mesh/SystemMeshFactory.cpp
    Module.cpp
    PrimitiveRenderer.cpp
    RenderClassFactory.cpp
    Resource/AliasTextureFactory.cpp
    Resource/AliasTextureResource.cpp
    Resource/ProgramResource.cpp
    Resource/ShaderFactory.cpp
    Resource/ShaderResource.cpp
    Resource/TextureFactory.cpp
    Resource/TextureLinker.cpp
    Resource/TextureProxy.cpp
    Resource/TextureResource.cpp
    Resource/VirtualTexture.cpp
    SH/SHCoeffs.cpp
    SH/SHEngine.cpp
    SH/SHFunction.cpp
    SH/SHMatrix.cpp
    SH/SHUtilities.cpp
    ScreenRenderer.cpp
    Shader.cpp
    Test/CaseRenderGraph.cpp
    Types.cpp
    VertexElement.cpp
)

add_traktor_module(Traktor.Render
    SOURCES ${TRAKTOR_RENDER_SOURCES}
    EXPORT_DEFINE T_RENDER_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Resource
        Traktor.Compress
)

target_include_directories(Traktor.Render PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Render.Editor - Editor support

set(TRAKTOR_RENDER_EDITOR_SOURCES
    Editor/CppStructWizardTool.cpp
    Editor/Edge.cpp
    Editor/Glsl/GlslAccelerationStructure.cpp
    Editor/Glsl/GlslContext.cpp
    Editor/Glsl/GlslEmitter.cpp
    Editor/Glsl/GlslImage.cpp
    Editor/Glsl/GlslLayout.cpp
    Editor/Glsl/GlslResource.cpp
    Editor/Glsl/GlslSampler.cpp
    Editor/Glsl/GlslShader.cpp
    Editor/Glsl/GlslStorageBuffer.cpp
    Editor/Glsl/GlslTexture.cpp
    Editor/Glsl/GlslType.cpp
    Editor/Glsl/GlslUniformBuffer.cpp
    Editor/Glsl/GlslVariable.cpp
    Editor/Graph.cpp
    Editor/GraphTraverse.cpp
    Editor/Group.cpp
    Editor/IProgramCompiler.cpp
    Editor/Image2/IImgStep.cpp
    Editor/Image2/ImageGraphAsset.cpp
    Editor/Image2/ImageGraphClipboardData.cpp
    Editor/Image2/ImageGraphEditorPage.cpp
    Editor/Image2/ImageGraphEditorPageFactory.cpp
    Editor/Image2/ImageGraphPermutations.cpp
    Editor/Image2/ImageGraphPipeline.cpp
    Editor/Image2/ImgInput.cpp
    Editor/Image2/ImgOutput.cpp
    Editor/Image2/ImgPass.cpp
    Editor/Image2/ImgPermutation.cpp
    Editor/Image2/ImgStepAmbientOcclusion.cpp
    Editor/Image2/ImgStepCompute.cpp
    Editor/Image2/ImgStepDirectionalBlur.cpp
    Editor/Image2/ImgStepShadowProject.cpp
    Editor/Image2/ImgStepSimple.cpp
    Editor/Image2/ImgStructBuffer.cpp
    Editor/Image2/ImgTargetSet.cpp
    Editor/Image2/ImgTexture.cpp
    Editor/ImmutableNode.cpp
    Editor/InputPin.cpp
    Editor/Node.cpp
    Editor/OutputPin.cpp
    Editor/RenderEditorPlugin.cpp
    Editor/RenderSettingsPage.cpp
    Editor/SH/DirectionalLight.cpp
    Editor/SH/IBLProbe.cpp
    Editor/SH/ModelFunction.cpp
    Editor/SH/RayLeighPhaseFunction.cpp
    Editor/SH/RenderControl.cpp
    Editor/SH/RenderControlEvent.cpp
    Editor/SH/SHEditorPage.cpp
    Editor/SH/SHEditorPageFactory.cpp
    Editor/SH/SHStage.cpp
    Editor/Shader/Algorithms/ShaderGraphCombinations.cpp
    Editor/Shader/Algorithms/ShaderGraphEvaluator.cpp
    Editor/Shader/Algorithms/ShaderGraphHash.cpp
    Editor/Shader/Algorithms/ShaderGraphOptimizer.cpp
    Editor/Shader/Algorithms/ShaderGraphOrderEvaluator.cpp
    Editor/Shader/Algorithms/ShaderGraphStatic.cpp
    Editor/Shader/Algorithms/ShaderGraphTechniques.cpp
    Editor/Shader/Algorithms/ShaderGraphTypeEvaluator.cpp
    Editor/Shader/Algorithms/ShaderGraphTypePropagation.cpp
    Editor/Shader/Algorithms/ShaderGraphValidator.cpp
    Editor/Shader/Constant.cpp
    Editor/Shader/External.cpp
    Editor/Shader/Facades/ColorNodeFacade.cpp
    Editor/Shader/Facades/CommentNodeFacade.cpp
    Editor/Shader/Facades/DefaultNodeFacade.cpp
    Editor/Shader/Facades/ExternalNodeFacade.cpp
    Editor/Shader/Facades/IndexedUniformNodeFacade.cpp
    Editor/Shader/Facades/InterpolatorNodeFacade.cpp
    Editor/Shader/Facades/ScalarNodeFacade.cpp
    Editor/Shader/Facades/ScriptNodeFacade.cpp
    Editor/Shader/Facades/SwizzleNodeFacade.cpp
    Editor/Shader/Facades/TextureNodeFacade.cpp
    Editor/Shader/Facades/UniformNodeFacade.cpp
    Editor/Shader/Facades/VariableNodeFacade.cpp
    Editor/Shader/FragmentLinker.cpp
    Editor/Shader/INodeFacade.cpp
    Editor/Shader/INodeTraits.cpp
    Editor/Shader/ListUniformsTool.cpp
    Editor/Shader/Nodes.cpp
    Editor/Shader/QuickMenuTool.cpp
    Editor/Shader/Script.cpp
    Editor/Shader/ShaderDependencyPane.cpp
    Editor/Shader/ShaderDependencyTracker.cpp
    Editor/Shader/ShaderGraph.cpp
    Editor/Shader/ShaderGraphBrowsePreview.cpp
    Editor/Shader/ShaderGraphDotTool.cpp
    Editor/Shader/ShaderGraphEditorClipboardData.cpp
    Editor/Shader/ShaderGraphEditorPage.cpp
    Editor/Shader/ShaderGraphEditorPageFactory.cpp
    Editor/Shader/ShaderGraphPreview.cpp
    Editor/Shader/ShaderModule.cpp
    Editor/Shader/ShaderModuleEditorPage.cpp
    Editor/Shader/ShaderModuleEditorPageFactory.cpp
    Editor/Shader/ShaderModulePipeline.cpp
    Editor/Shader/ShaderModuleResolve.cpp
    Editor/Shader/ShaderPipeline.cpp
    Editor/Shader/ShaderViewer.cpp
    Editor/Shader/ShaderWizardTool.cpp
    Editor/Shader/StructDeclaration.cpp
    Editor/Shader/TouchShaderGraphsTool.cpp
    Editor/Shader/Traits/ArcusTanTraits.cpp
    Editor/Shader/Traits/ComputeNodeTraits.cpp
    Editor/Shader/Traits/ConditionalNodeTraits.cpp
    Editor/Shader/Traits/ConstantNodeTraits.cpp
    Editor/Shader/Traits/DotNodeTraits.cpp
    Editor/Shader/Traits/IterativeNodeTraits.cpp
    Editor/Shader/Traits/LengthNodeTraits.cpp
    Editor/Shader/Traits/LerpNodeTraits.cpp
    Editor/Shader/Traits/MathNodeTraits.cpp
    Editor/Shader/Traits/MatrixNodeTraits.cpp
    Editor/Shader/Traits/MetaNodeTraits.cpp
    Editor/Shader/Traits/MixNodeTraits.cpp
    Editor/Shader/Traits/PixelNodeTraits.cpp
    Editor/Shader/Traits/PolynomialNodeTraits.cpp
    Editor/Shader/Traits/SamplerNodeTraits.cpp
    Editor/Shader/Traits/ScriptNodeTraits.cpp
    Editor/Shader/Traits/StructNodeTraits.cpp
    Editor/Shader/Traits/SwitchNodeTraits.cpp
    Editor/Shader/Traits/SwizzleNodeTraits.cpp
    Editor/Shader/Traits/TransformNodeTraits.cpp
    Editor/Shader/Traits/TransposeNodeTraits.cpp
    Editor/Shader/Traits/UniformNodeTraits.cpp
    Editor/Shader/Traits/ValueNodeTraits.cpp
    Editor/Shader/Traits/VertexNodeTraits.cpp
    Editor/Shader/TypedOutputPin.cpp
    Editor/Shader/UniformDeclaration.cpp
    Editor/Shader/UniformLinker.cpp
    Editor/Texture/AstcCompressor.cpp
    Editor/Texture/Bc6hCompressor.cpp
    Editor/Texture/BfnTextureAsset.cpp
    Editor/Texture/BfnTexturePipeline.cpp
    Editor/Texture/ColorGradingAssetEditor.cpp
    Editor/Texture/ColorGradingAssetEditorFactory.cpp
    Editor/Texture/ColorGradingTextureAsset.cpp
    Editor/Texture/ColorGradingTexturePipeline.cpp
    Editor/Texture/DxtnCompressor.cpp
    Editor/Texture/EnvironmentTextureAsset.cpp
    Editor/Texture/EnvironmentTexturePipeline.cpp
    Editor/Texture/EtcCompressor.cpp
    Editor/Texture/ICompressor.cpp
    Editor/Texture/PvrtcCompressor.cpp
    Editor/Texture/TextureAsset.cpp
    Editor/Texture/TextureAssetEditorPage.cpp
    Editor/Texture/TextureAssetEditorPageFactory.cpp
    Editor/Texture/TextureAssetPipeline.cpp
    Editor/Texture/TextureBatchDialog.cpp
    Editor/Texture/TextureBatchWizardTool.cpp
    Editor/Texture/TextureBrowsePreview.cpp
    Editor/Texture/TextureControl.cpp
    Editor/Texture/TextureOutput.cpp
    Editor/Texture/TextureOutputPipeline.cpp
    Editor/Texture/TextureSet.cpp
    Editor/Texture/TextureSetPipeline.cpp
    Editor/Texture/UnCompressor.cpp
)

add_traktor_module(Traktor.Render.Editor
    SOURCES ${TRAKTOR_RENDER_EDITOR_SOURCES}
    EXPORT_DEFINE T_RENDER_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Render
        Traktor.Editor
        Traktor.Ui
        Traktor.Drawing
        Traktor.Database
        Traktor.Compress
        Traktor.I18N
        Traktor.Xml
        Traktor.Resource
        Traktor.Render.Vrfy
        Extern.astcenc
        Extern.bc6h_enc
        Extern.rg_etc1
        Extern.squish
)

target_include_directories(Traktor.Render.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Generate resource headers from source files (.png, .image, etc.)
add_resource_headers(Traktor.Render.Editor "${CMAKE_CURRENT_SOURCE_DIR}/Editor")

# Traktor.Render.Vrfy

set(TRAKTOR_RENDER_VRFY_SOURCES
    Vrfy/AccelerationStructureVrfy.cpp
    Vrfy/BufferViewVrfy.cpp
    Vrfy/BufferVrfy.cpp
    Vrfy/ProgramResourceVrfy.cpp
    Vrfy/ProgramVrfy.cpp
    Vrfy/RenderSystemVrfy.cpp
    Vrfy/RenderTargetSetVrfy.cpp
    Vrfy/RenderViewVrfy.cpp
    Vrfy/ResourceTracker.cpp
    Vrfy/TextureVrfy.cpp
    Vrfy/VertexLayoutVrfy.cpp
)

add_traktor_module(Traktor.Render.Vrfy
    SOURCES ${TRAKTOR_RENDER_VRFY_SOURCES}
    EXPORT_DEFINE T_RENDER_VRFY_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Drawing
        Traktor.Render
)

target_include_directories(Traktor.Render.Vrfy PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Add RenderDoc include directory (for renderdoc_app.h)
if(EXISTS ${CMAKE_SOURCE_DIR}/3rdp/renderdoc/RenderDoc_1.36_64)
    target_include_directories(Traktor.Render.Vrfy PRIVATE
        ${CMAKE_SOURCE_DIR}/3rdp/renderdoc/RenderDoc_1.36_64
    )
endif()

# Traktor.Render.Vrfy.Editor - Editor support

set(TRAKTOR_RENDER_VRFY_EDITOR_SOURCES
    Vrfy/Editor/ProgramCompilerVrfy.cpp
)

add_traktor_module(Traktor.Render.Vrfy.Editor
    SOURCES ${TRAKTOR_RENDER_VRFY_EDITOR_SOURCES}
    EXPORT_DEFINE T_RENDER_VRFY_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Render.Vrfy
        Traktor.Render.Editor
        Traktor.Render
)

target_include_directories(Traktor.Render.Vrfy.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Render.Vulkan

set(TRAKTOR_RENDER_VULKAN_SOURCES
    Vulkan/AccelerationStructureVk.cpp
    Vulkan/BufferDynamicVk.cpp
    Vulkan/BufferStaticVk.cpp
    Vulkan/BufferViewVk.cpp
    Vulkan/BufferVk.cpp
    Vulkan/Module.cpp
    Vulkan/Private/Allocator.cpp
    Vulkan/Private/ApiBuffer.cpp
    Vulkan/Private/CommandBuffer.cpp
    Vulkan/Private/Context.cpp
    Vulkan/Private/Image.cpp
    Vulkan/Private/PipelineLayoutCache.cpp
    Vulkan/Private/Queue.cpp
    Vulkan/Private/RenderPassCache.cpp
    Vulkan/Private/ShaderModuleCache.cpp
    Vulkan/Private/UniformBufferChain.cpp
    Vulkan/Private/UniformBufferPool.cpp
    Vulkan/Private/Utilities.cpp
    Vulkan/Private/VertexAttributes.cpp
    Vulkan/ProgramResourceVk.cpp
    Vulkan/ProgramVk.cpp
    Vulkan/RenderSystemVk.cpp
    Vulkan/RenderTargetDepthVk.cpp
    Vulkan/RenderTargetSetVk.cpp
    Vulkan/RenderTargetVk.cpp
    Vulkan/RenderViewVk.cpp
    Vulkan/TextureVk.cpp
    Vulkan/VertexLayoutVk.cpp
    Vulkan/Win32/ApiLoader.cpp
    Vulkan/Win32/Window.cpp
)

add_traktor_module(Traktor.Render.Vulkan
    SOURCES ${TRAKTOR_RENDER_VULKAN_SOURCES}
    EXPORT_DEFINE T_RENDER_VULKAN_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Render
        Extern.VulkanMemoryAllocator
)

target_include_directories(Traktor.Render.Vulkan PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Add Vulkan SDK include directory
if(DEFINED ENV{VULKAN_SDK})
    target_include_directories(Traktor.Render.Vulkan PRIVATE
        $ENV{VULKAN_SDK}/Include
    )
else()
    # Try local vulkan-windows directory
    if(EXISTS ${CMAKE_SOURCE_DIR}/3rdp/vulkan-windows/vulkan-sdk/Include)
        target_include_directories(Traktor.Render.Vulkan PRIVATE
            ${CMAKE_SOURCE_DIR}/3rdp/vulkan-windows/vulkan-sdk/Include
        )
    endif()
endif()

# Traktor.Render.Vulkan.Editor - Editor support

set(TRAKTOR_RENDER_VULKAN_EDITOR_SOURCES
    Vulkan/Editor/ProgramCompilerVk.cpp
)

add_traktor_module(Traktor.Render.Vulkan.Editor
    SOURCES ${TRAKTOR_RENDER_VULKAN_EDITOR_SOURCES}
    EXPORT_DEFINE T_RENDER_VULKAN_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Render.Editor
        Traktor.Render.Vulkan
        Traktor.Xml
        Traktor.Render
)

# Link glslang/SPIRV libraries with configuration-specific versions
target_link_libraries(Traktor.Render.Vulkan.Editor PRIVATE
    $<$<CONFIG:Debug>:MachineIndependentd.lib>
    $<$<CONFIG:Debug>:OSDependentd.lib>
    $<$<CONFIG:Debug>:SPIRV-Tools-optd.lib>
    $<$<CONFIG:Debug>:SPIRV-Toolsd.lib>
    $<$<CONFIG:Debug>:SPIRVd.lib>
    $<$<CONFIG:Debug>:glslangd.lib>
    $<$<CONFIG:Debug>:shaderc_combinedd.lib>
    $<$<CONFIG:Release>:MachineIndependent.lib>
    $<$<CONFIG:Release>:OSDependent.lib>
    $<$<CONFIG:Release>:SPIRV-Tools-opt.lib>
    $<$<CONFIG:Release>:SPIRV-Tools.lib>
    $<$<CONFIG:Release>:SPIRV.lib>
    $<$<CONFIG:Release>:glslang.lib>
    $<$<CONFIG:Release>:shaderc_combined.lib>
)

target_include_directories(Traktor.Render.Vulkan.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Add Vulkan SDK include and library directories (for vulkan.h, glslang headers, and glslang/SPIRV libs)
if(DEFINED ENV{VULKAN_SDK})
    target_include_directories(Traktor.Render.Vulkan.Editor PRIVATE
        $ENV{VULKAN_SDK}/Include
        $ENV{VULKAN_SDK}/Include/glslang
    )
    target_link_directories(Traktor.Render.Vulkan.Editor PRIVATE
        $ENV{VULKAN_SDK}/Lib
    )
else()
    # Try local vulkan-windows directory
    if(EXISTS ${CMAKE_SOURCE_DIR}/3rdp/vulkan-windows/vulkan-sdk/Include)
        target_include_directories(Traktor.Render.Vulkan.Editor PRIVATE
            ${CMAKE_SOURCE_DIR}/3rdp/vulkan-windows/vulkan-sdk/Include
            ${CMAKE_SOURCE_DIR}/3rdp/vulkan-windows/vulkan-sdk/Include/glslang
        )
        target_link_directories(Traktor.Render.Vulkan.Editor PRIVATE
            ${CMAKE_SOURCE_DIR}/3rdp/vulkan-windows/vulkan-sdk/Lib
        )
    endif()
endif()