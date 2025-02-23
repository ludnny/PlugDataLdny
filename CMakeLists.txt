cmake_minimum_required(VERSION 3.12)

if(APPLE)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE STRING "Minimum OS X deployment version")
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
endif()

if (NOT DEFINED COPY_PLUGIN_AFTER_BUILD)
  set(COPY_PLUGIN_AFTER_BUILD FALSE)
else()
  set(COPY_PLUGIN_AFTER_BUILD TRUE)
endif()

set(HARDENED_RUNTIME_ENABLED YES)
set(HARDENED_RUNTIME_OPTIONS "com.apple.security.device.audio-input")

set(JUCE_ENABLE_MODULE_SOURCE_GROUPS ON CACHE BOOL "" FORCE)
set_property(GLOBAL PROPERTY USE_FOLDERS YES)

project(PlugData VERSION 0.5 LANGUAGES C CXX)

add_subdirectory(Libraries/)

set(PLUGDATA_VERSION                    "0.5")
set(PLUGDATA_COMPANY_NAME               "Octagon")
set(PLUGDATA_COMPANY_COPYRIGHT          "PlugData by Timothy Schoen")
set(PLUGDATA_COMPANY_WEBSITE            "github.com/timothyschoen/plugdata")
set(PLUGDATA_ICON_BIG                   "${CMAKE_CURRENT_SOURCE_DIR}/Resources/plugd_logo.png")
set(PLUGDATA_PLUGINS_LOCATION           "${CMAKE_CURRENT_SOURCE_DIR}/Plugins")
set(PLUGDATA_LV2_LOCATION           "${CMAKE_CURRENT_SOURCE_DIR}/Plugins/LV2")

set(PLUGDATA_AU_INSTALL_LOCATION           "$ENV{HOME}/Library/Audio/Plug-Ins/Components/")

if(APPLE)
set(PLUGDATA_VST3_INSTALL_LOCATION         "$ENV{HOME}/Library/Audio/Plug-Ins/VST3/")
set(PLUGDATA_LV2_INSTALL_LOCATION          "$ENV{HOME}/Library/Audio/Plug-Ins/LV2/")
set(PLUGDATA_APP_INSTALL_LOCATION          "$ENV{HOME}/Applications/")
elseif(MSVC)
set(PLUGDATA_VST3_INSTALL_LOCATION           "$ENV{PROGRAMFILES}/Common Files/VST3")
set(PLUGDATA_LV2_INSTALL_LOCATION            "$ENV{APPDATA}/LV2/")
set(PLUGDATA_APP_INSTALL_LOCATION            "$ENV{PROGRAMFILES}/PlugData/")
else()
set(PLUGDATA_VST3_INSTALL_LOCATION           "$ENV{HOME}/.vst3/")
set(PLUGDATA_LV2_INSTALL_LOCATION            "$ENV{HOME}/.lv2/")
set(PLUGDATA_APP_INSTALL_LOCATION            "/opt/")
endif()


set(SOURCES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Source)

file(GLOB PlugDataGlobalSources
    ${CMAKE_CACHEFILE_DIR}/PlugData_artefacts/JuceLibraryCode/JuceHeader.h)
source_group("Source" FILES ${PlugDataGlobalSources})

file(GLOB PlugDataFxGlobalSources
    ${CMAKE_CACHEFILE_DIR}/PlugDataFx_artefacts/JuceLibraryCode/JuceHeader.h)
source_group("Source" FILES ${PlugDataFxGlobalSources})

if(APPLE)
file(GLOB PlugDataMidiGlobalSources
    ${CMAKE_CACHEFILE_DIR}/PlugDataMidi_artefacts/JuceLibraryCode/JuceHeader.h)
source_group("Source" FILES ${PlugDataMidiGlobalSources})
endif()

file(GLOB PlugDataSources
    ${SOURCES_DIRECTORY}/*.h
    ${SOURCES_DIRECTORY}/*.cpp
    ${SOURCES_DIRECTORY}/Components/*.h
    ${SOURCES_DIRECTORY}/Components/*.cpp
    ${SOURCES_DIRECTORY}/Objects/*.h
    ${SOURCES_DIRECTORY}/Objects/*.cpp
    ${SOURCES_DIRECTORY}/Sidebar/*.h
    ${SOURCES_DIRECTORY}/Sidebar/*.cpp
    ${SOURCES_DIRECTORY}/Dialogs/*.h
    ${SOURCES_DIRECTORY}/Dialogs/*.cpp
    ${SOURCES_DIRECTORY}/Pd/*.cpp
    ${SOURCES_DIRECTORY}/Pd/*.h
)
foreach(core_SOURCE ${PlugDataSources})
		# Get the path of the file relative to the current source directory
		file(RELATIVE_PATH core_SOURCE_relative "${SOURCES_DIRECTORY}" "${core_SOURCE}")

		# Get the relative folder path
		get_filename_component(core_SOURCE_dir "${core_SOURCE_relative}" PATH)

		# Convert forward slashes to backslashes to get source group identifiers
		string(REPLACE "/" "\\" core_SOURCE_group "${core_SOURCE_dir}")

		source_group("Source\\${core_SOURCE_group}" FILES "${core_SOURCE}")
endforeach()

file(GLOB PlugDataStandaloneSources
    ${SOURCES_DIRECTORY}/Standalone/*.cpp
    ${SOURCES_DIRECTORY}/Standalone/*.h)
source_group("Source\\Standalone" FILES ${PlugDataStandaloneSources})


#source_group("Source\\Pd" FILES ${PlugDataPdSources})

file(GLOB_RECURSE PlugDataLV2Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/LV2/juce_LV2_Wrapper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/LV2/juce_LV2_FileCreator.cpp)

juce_add_plugin(PlugDataStandalone
    VERSION                     ${PLUGDATA_VERSION}
    COMPANY_NAME                ${PLUGDATA_COMPANY_NAME}
    COMPANY_COPYRIGHT           ${PLUGDATA_COMPANY_COPYRIGHT}
    COMPANY_WEBSITE             ${PLUGDATA_COMPANY_WEBSITE}
    PLUGIN_DESCRIPTION          "A plugin that loads Pure Data patches"
    ICON_BIG                    ${PLUGDATA_ICON_BIG}
    MICROPHONE_PERMISSION_ENABLED TRUE
    HARDENED_RUNTIME_ENABLED    ${HARDENED_RUNTIME_ENABLED}
    HARDENED_RUNTIME_OPTIONS    ${HARDENED_RUNTIME_OPTIONS}
    IS_SYNTH                    TRUE
    NEEDS_MIDI_INPUT            TRUE
    NEEDS_MIDI_OUTPUT           TRUE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD     ${COPY_PLUGIN_AFTER_BUILD}
    PLUGIN_MANUFACTURER_CODE    OCTA
    PLUGIN_CODE                 PlDt
    FORMATS                     Standalone
    PRODUCT_NAME                "PlugData"
    AU_MAIN_TYPE                kAudioUnitType_MusicDevice
    VST3_CATEGORIES             Instrument
    VST2_CATEGORY               kPlugCategSynth
    VST3_COPY_DIR               ${PLUGDATA_VST3_INSTALL_LOCATION}
    AU_COPY_DIR                 ${PLUGDATA_AU_INSTALL_LOCATION}
    APP_COPY_DIR                ${PLUGDATA_APP_INSTALL_LOCATION})

juce_add_plugin(PlugData
    VERSION                     ${PLUGDATA_VERSION}
    COMPANY_NAME                ${PLUGDATA_COMPANY_NAME}
    COMPANY_COPYRIGHT           ${PLUGDATA_COMPANY_COPYRIGHT}
    COMPANY_WEBSITE             ${PLUGDATA_COMPANY_WEBSITE}
    PLUGIN_DESCRIPTION          "A plugin that loads Pure Data patches"
    ICON_BIG                    ${PLUGDATA_ICON_BIG}
    MICROPHONE_PERMISSION_ENABLED TRUE
    HARDENED_RUNTIME_ENABLED    ${HARDENED_RUNTIME_ENABLED}
    HARDENED_RUNTIME_OPTIONS    ${HARDENED_RUNTIME_OPTIONS}
    IS_SYNTH                    TRUE
    NEEDS_MIDI_INPUT            TRUE
    NEEDS_MIDI_OUTPUT           TRUE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD     ${COPY_PLUGIN_AFTER_BUILD}
    PLUGIN_MANUFACTURER_CODE    OCTA
    PLUGIN_CODE                 PlDt
    FORMATS                     AU VST3
    PRODUCT_NAME                "PlugData"
    AU_MAIN_TYPE                kAudioUnitType_MusicDevice
    VST3_CATEGORIES             Instrument
    VST2_CATEGORY               kPlugCategSynth
    VST3_COPY_DIR               ${PLUGDATA_VST3_INSTALL_LOCATION}
    AU_COPY_DIR                 ${PLUGDATA_AU_INSTALL_LOCATION}
    APP_COPY_DIR                ${PLUGDATA_APP_INSTALL_LOCATION})

juce_add_plugin(PlugDataFx
    VERSION                     ${PLUGDATA_VERSION}
    COMPANY_NAME                ${PLUGDATA_COMPANY_NAME}
    COMPANY_COPYRIGHT           ${PLUGDATA_COMPANY_COPYRIGHT}
    COMPANY_WEBSITE             ${PLUGDATA_COMPANY_WEBSITE}
    ICON_BIG                    ${PLUGDATA_ICON_BIG}
    HARDENED_RUNTIME_ENABLED    ${HARDENED_RUNTIME_ENABLED}
    HARDENED_RUNTIME_OPTIONS    ${HARDENED_RUNTIME_OPTIONS}
    PLUGIN_DESCRIPTION          "A plugin that loads Pure Data patches"
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            TRUE
    NEEDS_MIDI_OUTPUT           TRUE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD     ${COPY_PLUGIN_AFTER_BUILD}
    PLUGIN_MANUFACTURER_CODE    OCTA
    PLUGIN_CODE                 PlDf
    FORMATS                     AU VST3
    PRODUCT_NAME                "PlugDataFx"
    AU_MAIN_TYPE                kAudioUnitType_MusicEffect
    VST3_CATEGORIES             Fx
    VST2_CATEGORY               kPlugCategEffect
    VST3_COPY_DIR               ${PLUGDATA_VST3_INSTALL_LOCATION}
    AU_COPY_DIR                 ${PLUGDATA_AU_INSTALL_LOCATION})

if(APPLE)
juce_add_plugin(PlugDataMidi
    VERSION                     ${PLUGDATA_VERSION}
    COMPANY_NAME                ${PLUGDATA_COMPANY_NAME}
    COMPANY_COPYRIGHT           ${PLUGDATA_COMPANY_COPYRIGHT}
    COMPANY_WEBSITE             ${PLUGDATA_COMPANY_WEBSITE}
    ICON_BIG                    ${PLUGDATA_ICON_BIG}
    HARDENED_RUNTIME_ENABLED    ${HARDENED_RUNTIME_ENABLED}
    HARDENED_RUNTIME_OPTIONS    ${HARDENED_RUNTIME_OPTIONS}
    PLUGIN_DESCRIPTION          "A plugin that loads Pure Data patches"
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            TRUE
    NEEDS_MIDI_OUTPUT           TRUE
    IS_MIDI_EFFECT              TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD     ${COPY_PLUGIN_AFTER_BUILD}
    PLUGIN_MANUFACTURER_CODE    OCTA
    PLUGIN_CODE                 PlDm
    FORMATS                     AU
    PRODUCT_NAME                "PlugDataMidi"
    AU_MAIN_TYPE                kAudioUnitType_MIDIProcessor
    AU_COPY_DIR                 ${PLUGDATA_AU_INSTALL_LOCATION})
endif()

if(APPLE)
set_target_properties(PlugData PROPERTIES MACOSX_BUNDLE TRUE)
set_target_properties(PlugData PROPERTIES CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
set_target_properties(PlugDataFx PROPERTIES CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
endif()



juce_generate_juce_header(PlugDataStandalone)
set_target_properties(PlugDataStandalone PROPERTIES CXX_STANDARD 20)
target_sources(PlugDataStandalone PRIVATE ${PlugDataSources} ${PlugDataStandaloneSources} ${ELSESources})


juce_generate_juce_header(PlugData)
set_target_properties(PlugData PROPERTIES CXX_STANDARD 20)
target_sources(PlugData PRIVATE ${PlugDataSources} ${PlugDataStandaloneSources} ${ELSESources})

juce_generate_juce_header(PlugDataFx)
set_target_properties(PlugDataFx PROPERTIES CXX_STANDARD 20)
target_sources(PlugDataFx PRIVATE ${PlugDataSources} ${ELSESources})

if(APPLE)
juce_generate_juce_header(PlugDataMidi)

set_target_properties(PlugDataMidi PROPERTIES CXX_STANDARD 20)
set_target_properties(PlugDataMidi PROPERTIES CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
target_sources(PlugDataMidi PRIVATE ${PlugDataSources} ${ELSESources})
endif()

add_library(PlugData_LV2 SHARED ${PlugDataLV2Sources})
target_link_libraries(PlugData_LV2 PRIVATE PlugDataFx pd-multi)
set_target_properties(PlugData_LV2 PROPERTIES PREFIX "")

set(LIBPD_MULTI_COMPILE_DEFINITIONS
    PDINSTANCE=1
    PDTHREADS=1
)

set(STANDALONE_COMPILE_DEFINITIONS
    JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=1
    PLUGDATA_STANDALONE=1)

set(PLUGDATA_COMPILE_DEFINITIONS
    JUCE_APP_CONFIG_HEADER="${SOURCES_DIRECTORY}/PluginConfig.h"
    JUCE_DONT_DECLARE_PROJECTINFO=1
    JUCE_ALLOW_STATIC_NULL_VARIABLES=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_LOG_ASSERTIONS=1
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

if(UNIX AND NOT APPLE)
    set(PLUGDATA_COMPILE_DEFINITIONS
    ${PLUGDATA_COMPILE_DEFINITIONS}
    JUCE_MODAL_LOOPS_PERMITTED=1
    JUCE_USE_XRANDR=1
    )

    target_link_libraries(PlugDataStandalone PUBLIC "-Wl,-export-dynamic")
elseif(MSVC)

target_link_libraries(PlugDataStandalone PUBLIC "-Wl,--export-all-symbols,--whole-archive ws2_32 kernel32 -static-libgcc")

endif()

target_compile_definitions(PlugDataStandalone PUBLIC ${PLUGDATA_COMPILE_DEFINITIONS} ${STANDALONE_COMPILE_DEFINITIONS})

target_compile_definitions(PlugData PUBLIC ${PLUGDATA_COMPILE_DEFINITIONS})
target_compile_definitions(PlugDataFx PUBLIC ${PLUGDATA_COMPILE_DEFINITIONS})
if(APPLE)
target_compile_definitions(PlugDataMidi PUBLIC ${PLUGDATA_COMPILE_DEFINITIONS})
endif()
target_compile_definitions(PlugData_LV2 PRIVATE "JucePlugin_Build_LV2=1")

target_compile_definitions(PlugData PUBLIC ${LIBPD_MULTI_COMPILE_DEFINITIONS})
target_compile_definitions(PlugDataFx PUBLIC ${LIBPD_MULTI_COMPILE_DEFINITIONS})
if(APPLE)
target_compile_definitions(PlugDataMidi PUBLIC ${LIBPD_MULTI_COMPILE_DEFINITIONS})
endif()


list(APPEND PLUGDATA_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/pure-data/src")
list(APPEND PLUGDATA_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/libpd/")
list(APPEND PLUGDATA_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/concurrentqueue/")

list(APPEND PLUGDATA_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Source/")

target_include_directories(PlugDataStandalone PUBLIC "$<BUILD_INTERFACE:${PLUGDATA_INCLUDE_DIRECTORY}>")
target_include_directories(PlugData PUBLIC "$<BUILD_INTERFACE:${PLUGDATA_INCLUDE_DIRECTORY}>")
target_include_directories(PlugDataFx PUBLIC "$<BUILD_INTERFACE:${PLUGDATA_INCLUDE_DIRECTORY}>")

if(APPLE)
target_include_directories(PlugDataMidi PUBLIC "$<BUILD_INTERFACE:${PLUGDATA_INCLUDE_DIRECTORY}>")
endif()

file(GLOB PlugDataBinaryDataSources
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/PlugDataFont.ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/InterRegular.otf
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/plugd_logo.png
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Abstractions.zip
    )

juce_add_binary_data(PlugDataBinaryData SOURCES ${PlugDataBinaryDataSources})
set_target_properties(PlugDataBinaryData PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(MSVC)
  target_link_libraries(PlugDataStandalone PRIVATE pd PlugDataBinaryData juce::juce_audio_utils juce::juce_audio_plugin_client libpthreadVC3)
  target_link_libraries(PlugData PRIVATE pd-multi PlugDataBinaryData juce::juce_audio_utils juce::juce_audio_plugin_client libpthreadVC3)
  target_link_libraries(PlugDataFx PRIVATE pd-multi PlugDataBinaryData juce::juce_audio_utils juce::juce_audio_plugin_client libpthreadVC3)
  target_link_libraries(PlugData_LV2 PRIVATE pd-multi PlugDataBinaryData juce::juce_audio_utils juce::juce_audio_plugin_client libpthreadVC3)
else()
  target_link_libraries(PlugDataStandalone PRIVATE pd PlugDataBinaryData juce::juce_audio_utils juce::juce_audio_plugin_client)
  target_link_libraries(PlugData PRIVATE pd-multi PlugDataBinaryData juce::juce_audio_utils juce::juce_audio_plugin_client)
  target_link_libraries(PlugDataFx PRIVATE pd-multi PlugDataBinaryData juce::juce_audio_utils juce::juce_audio_plugin_client)
  if(APPLE)
    target_link_libraries(PlugDataMidi PRIVATE pd-multi PlugDataBinaryData juce::juce_audio_utils juce::juce_audio_plugin_client)
  endif()
  target_link_libraries(PlugData_LV2 PRIVATE pd-multi PlugDataBinaryData juce::juce_audio_utils juce::juce_audio_plugin_client)
endif()

add_executable(lv2_file_generator ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/LV2/main.c)
target_link_libraries(lv2_file_generator ${CMAKE_DL_LIBS})

set_target_properties(PlugDataStandalone PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PLUGDATA_PLUGINS_LOCATION})
set_target_properties(PlugData PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PLUGDATA_PLUGINS_LOCATION})
set_target_properties(PlugDataFx PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PLUGDATA_PLUGINS_LOCATION})
if(APPLE)
set_target_properties(PlugDataMidi PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PLUGDATA_PLUGINS_LOCATION})
endif()

set_target_properties(PlugDataStandalone PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PLUGDATA_PLUGINS_LOCATION})
set_target_properties(PlugData PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PLUGDATA_PLUGINS_LOCATION})
set_target_properties(PlugDataFx PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PLUGDATA_PLUGINS_LOCATION})
if(APPLE)
set_target_properties(PlugDataMidi PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PLUGDATA_PLUGINS_LOCATION})
endif()




set_target_properties(lv2_file_generator PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PLUGDATA_LV2_LOCATION})
set_target_properties(PlugData_LV2 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PLUGDATA_LV2_LOCATION})
set_target_properties(PlugData_LV2 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PLUGDATA_LV2_LOCATION})
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
		  string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
			set_target_properties(lv2_file_generator PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PLUGDATA_LV2_LOCATION})
      set_target_properties(PlugData_LV2 PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PLUGDATA_LV2_LOCATION})
      set_target_properties(PlugData_LV2 PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PLUGDATA_LV2_LOCATION})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

if(MSVC)
set_target_properties(pthreadVC3 pthreadVSE3 pthreadVCE3 PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
endif()
