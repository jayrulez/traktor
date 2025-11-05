# Traktor.Input

set(TRAKTOR_INPUT_SOURCES
    Binding/CombinedInputSource.cpp
    Binding/CombinedInputSourceData.cpp
    Binding/ConstantInputSource.cpp
    Binding/ConstantInputSourceData.cpp
    Binding/ControlInputSource.cpp
    Binding/ControlInputSourceData.cpp
    Binding/DeviceControl.cpp
    Binding/DeviceControlManager.cpp
    Binding/EnumKeys.cpp
    Binding/GenericInputSource.cpp
    Binding/GenericInputSourceData.cpp
    Binding/IInputNode.cpp
    Binding/IInputSource.cpp
    Binding/IInputSourceData.cpp
    Binding/InBoolean.cpp
    Binding/InClamp.cpp
    Binding/InCombine.cpp
    Binding/InConst.cpp
    Binding/InDelay.cpp
    Binding/InEnvelope.cpp
    Binding/InGesturePinch.cpp
    Binding/InGestureSwipe.cpp
    Binding/InGestureTap.cpp
    Binding/InHysteresis.cpp
    Binding/InLowPass.cpp
    Binding/InPulse.cpp
    Binding/InReadValue.cpp
    Binding/InRemapAxis.cpp
    Binding/InThreshold.cpp
    Binding/InTrigger.cpp
    Binding/InputMapping.cpp
    Binding/InputMappingResource.cpp
    Binding/InputMappingSourceData.cpp
    Binding/InputMappingStateData.cpp
    Binding/InputSourceFabricator.cpp
    Binding/InputState.cpp
    Binding/InputStateData.cpp
    Binding/InputValueSet.cpp
    Binding/KeyboardInputSource.cpp
    Binding/KeyboardInputSourceData.cpp
    IInputDevice.cpp
    IInputDriver.cpp
    InputClassFactory.cpp
    InputSystem.cpp
    InputTypes.cpp
    Module.cpp
    RecordInputDevice.cpp
    RecordInputScript.cpp
    ReplayInputDevice.cpp
    RumbleEffect.cpp
    RumbleEffectFactory.cpp
    RumbleEffectPlayer.cpp
)

add_traktor_module(Traktor.Input
    SOURCES ${TRAKTOR_INPUT_SOURCES}
    EXPORT_DEFINE T_INPUT_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Resource
)

target_include_directories(Traktor.Input PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Input.Di8

set(TRAKTOR_INPUT_DI8_SOURCES
    Di8/InputDriverDi8.cpp
    Di8/JoystickDeviceDi8.cpp
    Di8/KeyboardDeviceDi8.cpp
    Di8/Module.cpp
    Di8/MouseDeviceDi8.cpp
    Di8/TypesDi8.cpp
)

add_traktor_module(Traktor.Input.Di8
    SOURCES ${TRAKTOR_INPUT_DI8_SOURCES}
    EXPORT_DEFINE T_INPUT_DI8_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Input
    SYSTEM_LIBS
        dinput8.lib
        dxguid.lib
)

target_include_directories(Traktor.Input.Di8 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Input.Editor - Editor support

set(TRAKTOR_INPUT_EDITOR_SOURCES
    Editor/InBooleanTraits.cpp
    Editor/InClampTraits.cpp
    Editor/InCombineTraits.cpp
    Editor/InConstTraits.cpp
    Editor/InDelayTraits.cpp
    Editor/InEnvelopeTraits.cpp
    Editor/InGesturePinchTraits.cpp
    Editor/InGestureSwipeTraits.cpp
    Editor/InGestureTapTraits.cpp
    Editor/InHysteresisTraits.cpp
    Editor/InLowPassTraits.cpp
    Editor/InPulseTraits.cpp
    Editor/InReadValueTraits.cpp
    Editor/InRemapAxisTraits.cpp
    Editor/InThresholdTraits.cpp
    Editor/InTriggerTraits.cpp
    Editor/InputMappingAsset.cpp
    Editor/InputMappingEditor.cpp
    Editor/InputMappingEditorFactory.cpp
    Editor/InputMappingPipeline.cpp
)

add_traktor_module(Traktor.Input.Editor
    SOURCES ${TRAKTOR_INPUT_EDITOR_SOURCES}
    EXPORT_DEFINE T_INPUT_EDITOR_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.I18N
        Traktor.Editor
        Traktor.Input
        Traktor.Ui
        Traktor.Xml
)

target_include_directories(Traktor.Input.Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Generate resource headers from source files (.png, .image, etc.)
add_resource_headers(Traktor.Input.Editor "${CMAKE_CURRENT_SOURCE_DIR}/Editor")

# Traktor.Input.Win32

set(TRAKTOR_INPUT_WIN32_SOURCES
    Win32/InputDriverWin32.cpp
    Win32/KeyboardDeviceWin32.cpp
    Win32/Module.cpp
    Win32/MouseDeviceWin32.cpp
    Win32/TypesWin32.cpp
)

add_traktor_module(Traktor.Input.Win32
    SOURCES ${TRAKTOR_INPUT_WIN32_SOURCES}
    EXPORT_DEFINE T_INPUT_WIN32_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Input
)

target_include_directories(Traktor.Input.Win32 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Traktor.Input.Xi

set(TRAKTOR_INPUT_XI_SOURCES
    Xi/InputDeviceXi.cpp
    Xi/InputDriverXi.cpp
    Xi/Module.cpp
)

add_traktor_module(Traktor.Input.Xi
    SOURCES ${TRAKTOR_INPUT_XI_SOURCES}
    EXPORT_DEFINE T_INPUT_XI_EXPORT
    DEPENDENCIES
        Traktor.Core
        Traktor.Input
    SYSTEM_LIBS
        xinput.lib
)

target_include_directories(Traktor.Input.Xi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)