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
pkg_check_modules(RAYLIB REQUIRED raylib)
pkg_check_modules(FFTW3F REQUIRED fftw3f)

# Definir nuestro ejecutable y sus archivos fuente
add_executable(MicroSynth 
    src/main.cpp 
    src/AudioEngine.cpp
	src/Oscilator.cpp
	src/VoiceManager.cpp
	src/TuningSys.cpp
	src/RingBuffer.cpp
	src/MidiQueue.cpp
)

target_include_directories(MicroSynth PRIVATE 
    ${RTMIDI_INCLUDE_DIRS} 
    ${RAYLIB_INCLUDE_DIRS}
    ${FFTW3F_INCLUDE_DIRS} # <- NUEVO: Directorio de fftw3.h
)

# --- ENLACE DE LIBRERÍAS (.so / .a) ---
# Unificamos todas las dependencias en una sola llamada limpia
target_link_libraries(MicroSynth PRIVATE 
    ${PORTAUDIO_LIB} 
    ${RTMIDI_LIBRARIES} 
    ${RAYLIB_LIBRARIES}
    ${FFTW3F_LIBRARIES}    # <- NUEVO: Enlazamos el binario de fftw3f
    m                      # <- NUEVO: Enlazamos la librería matemática estándar de C (necesaria para FFT y Raylib)
)
