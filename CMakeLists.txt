cmake_minimum_required(VERSION 3.15)
project(MicroSynth VERSION 1.0)

# Usaremos C++17, que tiene herramientas modernas muy útiles para audio
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Le decimos a CMake dónde buscar nuestros archivos de cabecera (.h)
include_directories(include)

# Buscar la librería de PortAudio en tu sistema
find_library(PORTAUDIO_LIB NAMES portaudio)
if(NOT PORTAUDIO_LIB)
    message(FATAL_ERROR "No se pudo encontrar PortAudio. Asegúrate de tenerlo instalado.")
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(RTMIDI REQUIRED rtmidi)

# Definir nuestro ejecutable y sus archivos fuente
add_executable(MicroSynth 
    src/main.cpp 
    src/AudioEngine.cpp
	src/Oscilator.cpp
	src/VoiceManager.cpp
)

# Enlazar nuestro ejecutable con la librería de PortAudio
target_link_libraries(MicroSynth PRIVATE ${PORTAUDIO_LIB})

# Reemplaza "tu_ejecutable" por el nombre real de tu proyecto en CMake
target_include_directories(MicroSynth PRIVATE ${RTMIDI_INCLUDE_DIRS})
target_link_libraries(MicroSynth PRIVATE ${RTMIDI_LIBRARIES})
