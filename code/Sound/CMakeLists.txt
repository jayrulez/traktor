# Traktor.Sound

set(TRAKTOR_SOUND_SOURCES
	AudioChannel.h
	AudioClassFactory.h
	AudioDriverNull.h
	AudioDriverWriteOut.h
	AudioMixer.h
	AudioResourceFactory.h
	AudioSystem.h
	Avx/AudioMixerAvx.h
	BoxedAudioBlock.h
	Decoders/FlacStreamDecoder.h
	Decoders/Mp3StreamDecoder.h
	Decoders/OggStreamDecoder.h
	Decoders/TssStreamDecoder.h
	Decoders/WavStreamDecoder.h
	Filters/CombFilter.h
	Filters/DitherFilter.h
	Filters/EchoFilter.h
	Filters/EqualizerFilter.h
	Filters/FFTFilter.h
	Filters/GroupFilter.h
	Filters/IIRFilter.h
	Filters/IIR/IIR.h
	Filters/LowPassFilter.h
	Filters/NormalizationFilter.h
	Filters/ReverbFilter.h
	Filters/RingModulationFilter.h
	Filters/SurroundEnvironment.h
	Filters/SurroundFilter.h
	Filters/TimeStretchFilter.h
	IAudioBuffer.h
	IAudioDriver.h
	IAudioFilter.h
	IAudioMixer.h
	IAudioResource.h
	IStreamDecoder.h
	Module.h
	Player/SoundHandle.h
	Player/SoundListener.h
	Player/SoundPlayer.h
	Processor/Edge.h
	Processor/Graph.h
	Processor/GraphBuffer.h
	Processor/GraphEvaluator.h
	Processor/GraphResource.h
	Processor/ImmutableNode.h
	Processor/InputPin.h
	Processor/Node.h
	Processor/Nodes/Add.h
	Processor/Nodes/Blend.h
	Processor/Nodes/Custom.h
	Processor/Nodes/Divide.h
	Processor/Nodes/Filter.h
	Processor/Nodes/Multiply.h
	Processor/Nodes/Output.h
	Processor/Nodes/Parameter.h
	Processor/Nodes/Pitch.h
	Processor/Nodes/Scalar.h
	Processor/Nodes/Sine.h
	Processor/Nodes/Source.h
	Processor/Nodes/Subtract.h
	Processor/Nodes/Time.h
	Processor/OutputPin.h
	Processor/ProcessorTypes.h
	Resound/BankBuffer.h
	Resound/BankResource.h
	Resound/BlendGrain.h
	Resound/BlendGrainData.h
	Resound/EnvelopeGrain.h
	Resound/EnvelopeGrainData.h
	Resound/GrainFactory.h
	Resound/IGrain.h
	Resound/IGrainData.h
	Resound/IGrainFactory.h
	Resound/InLoopOutGrain.h
	Resound/InLoopOutGrainData.h
	Resound/MuteGrain.h
	Resound/MuteGrainData.h
	Resound/PlayGrain.h
	Resound/PlayGrainData.h
	Resound/RandomGrain.h
	Resound/RandomGrainData.h
	Resound/RepeatGrain.h
	Resound/RepeatGrainData.h
	Resound/SequenceGrain.h
	Resound/SequenceGrainData.h
	Resound/SimultaneousGrain.h
	Resound/SimultaneousGrainData.h
	Resound/TriggerGrain.h
	Resound/TriggerGrainData.h
	Sound.h
	StaticAudioBuffer.h
	StaticAudioResource.h
	StreamAudioBuffer.h
	StreamAudioResource.h
	Test/CaseSound.h
	Tracker/GotoEvent.h
	Tracker/GotoEventData.h
	Tracker/IEvent.h
	Tracker/IEventData.h
	Tracker/Pattern.h
	Tracker/PatternData.h
	Tracker/Play.h
	Tracker/PlayData.h
	Tracker/SetBpmEvent.h
	Tracker/SetBpmEventData.h
	Tracker/SongBuffer.h
	Tracker/SongResource.h
	Tracker/Track.h
	Tracker/TrackData.h
	Tracker/VolumeEvent.h
	Tracker/VolumeEventData.h
	Tracker/VolumeSlideEvent.h
	Tracker/VolumeSlideEventData.h
	Types.h
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
	Filters/IIRFilter.cpp
	Filters/IIR/IIR.cpp
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
	Ds8/AudioDriverDs8.h
	Ds8/Platform.h
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
	Editor/Encoders/OggStreamEncoder.h
	Editor/Encoders/TssStreamEncoder.h
	Editor/IStreamEncoder.h
	Editor/Processor/GraphAsset.h
	Editor/Processor/GraphEditor.h
	Editor/Processor/GraphEditorFactory.h
	Editor/Processor/GraphPipeline.h
	Editor/Resound/BankAsset.h
	Editor/Resound/BankAssetEditor.h
	Editor/Resound/BankAssetEditorFactory.h
	Editor/Resound/BankControl.h
	Editor/Resound/BankControlGrain.h
	Editor/Resound/BankPipeline.h
	Editor/Resound/BlendGrainFacade.h
	Editor/Resound/EnvelopeGrainFacade.h
	Editor/Resound/GrainProperties.h
	Editor/Resound/IGrainFacade.h
	Editor/Resound/InLoopOutGrainFacade.h
	Editor/Resound/MuteGrainFacade.h
	Editor/Resound/PlayGrainFacade.h
	Editor/Resound/RandomGrainFacade.h
	Editor/Resound/RepeatGrainFacade.h
	Editor/Resound/SequenceGrainFacade.h
	Editor/Resound/SimultaneousGrainFacade.h
	Editor/Resound/TriggerGrainFacade.h
	Editor/SoundAsset.h
	Editor/SoundAssetEditor.h
	Editor/SoundAssetEditorFactory.h
	Editor/SoundBatchDialog.h
	Editor/SoundBatchWizardTool.h
	Editor/SoundCategory.h
	Editor/SoundEditorPlugin.h
	Editor/SoundPanel.h
	Editor/SoundPipeline.h
	Editor/SoundSettingsPage.h
	Editor/Tracker/ImportMod.h
	Editor/Tracker/ImportSongWizardTool.h
	Editor/Tracker/SongAsset.h
	Editor/Tracker/SongEditor.h
	Editor/Tracker/SongEditorFactory.h
	Editor/Tracker/SongPipeline.h
	Editor/WaveformControl.h
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
	WinMM/AudioDriverWinMM.h
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
	XAudio2/AudioDriverXAudio2.h
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