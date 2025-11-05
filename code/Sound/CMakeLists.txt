# Traktor.Sound

set(TRAKTOR_SOUND_SOURCES
    AudioChannel.cpp
    AudioClassFactory.cpp
    AudioDriverNull.cpp
    AudioDriverWriteOut.cpp
    AudioMixer.cpp
    AudioResourceFactory.cpp
    AudioSystem.cpp
    Avx/AudioMixerAvx.cpp
    BoxedAudioBlock.cpp
    Decoders/FlacStreamDecoder.cpp
    Decoders/Mp3StreamDecoder.cpp
    Decoders/OggStreamDecoder.cpp
    Decoders/TssStreamDecoder.cpp
    Decoders/WavStreamDecoder.cpp
    Filters/CombFilter.cpp
    Filters/DitherFilter.cpp
    Filters/EchoFilter.cpp
    Filters/EqualizerFilter.cpp
    Filters/FFTFilter.cpp
    Filters/GroupFilter.cpp
    Filters/IIR/IIR.cpp
    Filters/IIRFilter.cpp
    Filters/LowPassFilter.cpp
    Filters/NormalizationFilter.cpp
    Filters/ReverbFilter.cpp
    Filters/RingModulationFilter.cpp
    Filters/SurroundEnvironment.cpp
    Filters/SurroundFilter.cpp
    Filters/TimeStretchFilter.cpp
    IAudioBuffer.cpp
    IAudioDriver.cpp
    IAudioFilter.cpp
    IAudioMixer.cpp
    IAudioResource.cpp
    IStreamDecoder.cpp
    Module.cpp
    Player/SoundHandle.cpp
    Player/SoundListener.cpp
    Player/SoundPlayer.cpp
    Processor/Edge.cpp
    Processor/Graph.cpp
    Processor/GraphBuffer.cpp
    Processor/GraphEvaluator.cpp
    Processor/GraphResource.cpp
    Processor/ImmutableNode.cpp
    Processor/InputPin.cpp
    Processor/Node.cpp
    Processor/Nodes/Add.cpp
    Processor/Nodes/Blend.cpp
    Processor/Nodes/Custom.cpp
    Processor/Nodes/Divide.cpp
    Processor/Nodes/Filter.cpp
    Processor/Nodes/Multiply.cpp
    Processor/Nodes/Output.cpp
    Processor/Nodes/Parameter.cpp
    Processor/Nodes/Pitch.cpp
    Processor/Nodes/Scalar.cpp
    Processor/Nodes/Sine.cpp
    Processor/Nodes/Source.cpp
    Processor/Nodes/Subtract.cpp
    Processor/Nodes/Time.cpp
    Processor/OutputPin.cpp
    Resound/BankBuffer.cpp
    Resound/BankResource.cpp
    Resound/BlendGrain.cpp
    Resound/BlendGrainData.cpp
    Resound/EnvelopeGrain.cpp
    Resound/EnvelopeGrainData.cpp
    Resound/GrainFactory.cpp
    Resound/IGrain.cpp
    Resound/IGrainData.cpp
    Resound/IGrainFactory.cpp
    Resound/InLoopOutGrain.cpp
    Resound/InLoopOutGrainData.cpp
    Resound/MuteGrain.cpp
    Resound/MuteGrainData.cpp
    Resound/PlayGrain.cpp
    Resound/PlayGrainData.cpp
    Resound/RandomGrain.cpp
    Resound/RandomGrainData.cpp
    Resound/RepeatGrain.cpp
    Resound/RepeatGrainData.cpp
    Resound/SequenceGrain.cpp
    Resound/SequenceGrainData.cpp
    Resound/SimultaneousGrain.cpp
    Resound/SimultaneousGrainData.cpp
    Resound/TriggerGrain.cpp
    Resound/TriggerGrainData.cpp
    Sound.cpp
    StaticAudioBuffer.cpp
    StaticAudioResource.cpp
    StreamAudioBuffer.cpp
    StreamAudioResource.cpp
    Test/CaseSound.cpp
    Tracker/GotoEvent.cpp
    Tracker/GotoEventData.cpp
    Tracker/IEvent.cpp
    Tracker/IEventData.cpp
    Tracker/Pattern.cpp
    Tracker/PatternData.cpp
    Tracker/Play.cpp
    Tracker/PlayData.cpp
    Tracker/SetBpmEvent.cpp
    Tracker/SetBpmEventData.cpp
    Tracker/SongBuffer.cpp
    Tracker/SongResource.cpp
    Tracker/Track.cpp
    Tracker/TrackData.cpp
    Tracker/VolumeEvent.cpp
    Tracker/VolumeEventData.cpp
    Tracker/VolumeSlideEvent.cpp
    Tracker/VolumeSlideEventData.cpp
    Types.cpp
)

add_traktor_module(Traktor.Sound
    SOURCES ${TRAKTOR_SOUND_SOURCES}
    EXPORT_DEFINE T_SOUND_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Resource
        Extern.flac
        Extern.minimp3
        Extern.stb
    SYSTEM_LIBS
        wsock32.lib
)

target_include_directories(Traktor.Sound PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Sound.Ds8

set(TRAKTOR_SOUND_DS8_SOURCES
    Ds8/AudioDriverDs8.cpp
    Ds8/Module.cpp
)

add_traktor_module(Traktor.Sound.Ds8
    SOURCES ${TRAKTOR_SOUND_DS8_SOURCES}
    EXPORT_DEFINE T_SOUND_DS8_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Sound
    SYSTEM_LIBS
        dsound.lib
        dxguid.lib
)

target_include_directories(Traktor.Sound.Ds8 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Sound.Editor - Editor support

set(TRAKTOR_SOUND_EDITOR_SOURCES
    Editor/Encoders/OggStreamEncoder.cpp
    Editor/Encoders/TssStreamEncoder.cpp
    Editor/IStreamEncoder.cpp
    Editor/Processor/GraphAsset.cpp
    Editor/Processor/GraphEditor.cpp
    Editor/Processor/GraphEditorFactory.cpp
    Editor/Processor/GraphPipeline.cpp
    Editor/Resound/BankAsset.cpp
    Editor/Resound/BankAssetEditor.cpp
    Editor/Resound/BankAssetEditorFactory.cpp
    Editor/Resound/BankControl.cpp
    Editor/Resound/BankControlGrain.cpp
    Editor/Resound/BankPipeline.cpp
    Editor/Resound/BlendGrainFacade.cpp
    Editor/Resound/EnvelopeGrainFacade.cpp
    Editor/Resound/GrainProperties.cpp
    Editor/Resound/IGrainFacade.cpp
    Editor/Resound/InLoopOutGrainFacade.cpp
    Editor/Resound/MuteGrainFacade.cpp
    Editor/Resound/PlayGrainFacade.cpp
    Editor/Resound/RandomGrainFacade.cpp
    Editor/Resound/RepeatGrainFacade.cpp
    Editor/Resound/SequenceGrainFacade.cpp
    Editor/Resound/SimultaneousGrainFacade.cpp
    Editor/Resound/TriggerGrainFacade.cpp
    Editor/SoundAsset.cpp
    Editor/SoundAssetEditor.cpp
    Editor/SoundAssetEditorFactory.cpp
    Editor/SoundBatchDialog.cpp
    Editor/SoundBatchWizardTool.cpp
    Editor/SoundCategory.cpp
    Editor/SoundEditorPlugin.cpp
    Editor/SoundPanel.cpp
    Editor/SoundPipeline.cpp
    Editor/SoundSettingsPage.cpp
    Editor/Tracker/ImportMod.cpp
    Editor/Tracker/ImportSongWizardTool.cpp
    Editor/Tracker/SongAsset.cpp
    Editor/Tracker/SongEditor.cpp
    Editor/Tracker/SongEditorFactory.cpp
    Editor/Tracker/SongPipeline.cpp
    Editor/WaveformControl.cpp
)

add_traktor_module(Traktor.Sound.Editor
    SOURCES ${TRAKTOR_SOUND_EDITOR_SOURCES}
    EXPORT_DEFINE T_SOUND_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Editor
        Traktor.Sound
        Traktor.Ui
        Traktor.I18N
        Traktor.Resource
        Traktor.Drawing
        Traktor.Script
        Extern.ogg
        Extern.vorbis
)

target_include_directories(Traktor.Sound.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Generate resource headers from source files (.png, .image, etc.)
add_resource_headers(Traktor.Sound.Editor "${CMAKE_CURRENT_SOURCE_DIR}/Editor")

# Traktor.Sound.WinMM

set(TRAKTOR_SOUND_WINMM_SOURCES
    WinMM/AudioDriverWinMM.cpp
    WinMM/Module.cpp
)

add_traktor_module(Traktor.Sound.WinMM
    SOURCES ${TRAKTOR_SOUND_WINMM_SOURCES}
    EXPORT_DEFINE T_SOUND_WINMM_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Sound
    SYSTEM_LIBS
        winmm.lib
)

target_include_directories(Traktor.Sound.WinMM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Sound.XAudio2

set(TRAKTOR_SOUND_XAUDIO2_SOURCES
    XAudio2/AudioDriverXAudio2.cpp
    XAudio2/Module.cpp
)

add_traktor_module(Traktor.Sound.XAudio2
    SOURCES ${TRAKTOR_SOUND_XAUDIO2_SOURCES}
    EXPORT_DEFINE T_SOUND_XAUDIO2_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Sound
)

target_include_directories(Traktor.Sound.XAudio2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)