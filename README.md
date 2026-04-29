# 🚀 MidiSynth (MicroSynth)

![Status](https://img.shields.io/badge/status-Completed-blue)
![Language](https://img.shields.io/badge/language-C++-orange)
![Framework](https://img.shields.io/badge/framework-Raylib-yellow)
![License](https://img.shields.io/badge/license-GPL3-green)
![Last update](https://img.shields.io/github/last-commit/Alelith/MidiSynth)

---

## 🧠 Description

MidiSynth is a real-time C++17 FM synthesizer application featuring an audio-reactive visualizer. It processes MIDI inputs and visualizes the harmonic content of the synthesized audio using FFT, creating a spinning "mandala" in a Raylib window.

---

## 🧩 Table of Contents

- [Description](#-description)
- [Features](#-features)
- [Technologies Used](#-technologies-used)
- [Project Structure](#-project-structure)
- [Installation](#-installation)
- [Execution](#-execution)
- [Usage Examples](#-usage-examples)
- [Screenshots / Demo](#-screenshots--demo)
- [Documentation](#-documentation)
- [Credits](#-credits)
- [License](#-license)
- [Author](#-author)

---

## 🌟 Features

-   [x] Real-time Frequency Modulation (FM) synthesis engine
-   [x] Audio-reactive FFT visualizer ("Mandala" visualization)
-   [x] Lock-free ring buffer for thread-safe audio processing
-   [x] MIDI input queuing and processing
-   [x] Voice management and tuning system support

---

## ⚙️ Technologies Used

**Languages:** C++17
**Frameworks:** Raylib
**Audio/Math Libraries:** PortAudio, RtMidi, FFTW3F
**Tools:** CMake, Git

---

## 📂 Project Structure

```
/MidiSynth
 ├── CMakeLists.txt        # CMake configuration file
 ├── include/              # Header files (.hpp)
 │   ├── AudioEngine.hpp
 │   ├── MidiQueue.hpp
 │   ├── Oscilator.hpp
 │   ├── RingBuffer.hpp
 │   ├── TuningSys.hpp
 │   └── VoiceManager.hpp
 ├── Prompts/              # Project templates and guides
 └── src/                  # Source files (.cpp)
     ├── AudioEngine.cpp
     ├── main.cpp
     ├── MidiQueue.cpp
     ├── Oscilator.cpp
     ├── RingBuffer.cpp
     ├── TuningSys.cpp
     └── VoiceManager.cpp
```

---

## 📦 Installation

### Prerequisites

Before you begin, ensure you have the following installed:

-   **C++17** compatible compiler (GCC/Clang)
-   **CMake** version 3.15 or higher
-   **pkg-config**
-   **PortAudio** library (`portaudio19-dev` on Debian/Ubuntu)
-   **RtMidi** library (`librtmidi-dev`)
-   **Raylib** library (`libraylib-dev`)
-   **FFTW3F** library (`libfftw3-dev`)

### Steps

``` bash
git clone https://github.com/Alelith/MidiSynth.git
cd MidiSynth

# Generate build files and compile the project
mkdir build
cd build
cmake ..
make
```

---

## ▶️ Execution

### Local

``` bash
# From the build directory
./MicroSynth
```

---

## 🧪 Usage Examples

Run the application from your terminal. It will open a Raylib window displaying the real-time FFT visualization of the audio engine's output. Make sure you have a valid MIDI input source configured if you intend to send MIDI notes to the synthesizer.

``` bash
./MicroSynth
```

**Example output:**
A 1920x1080 window titled "FM Synthesizer - FFTW Mandala" will appear, showing dynamic visualizations responding to the synth's audio output.

---

## � Screenshots / Demo

MidiSynth playing **"Dialga's Fight to the Finish!"** (*Pokémon Mystery Dungeon: Explorers of Sky*).

Sine wave:


https://github.com/user-attachments/assets/dba0f887-7e4e-41b4-a8c1-9a0b53e183e1


Triangle wave:


https://github.com/user-attachments/assets/6edcbeec-9449-4fce-9cc1-1abd3883e138


Square wave:


https://github.com/user-attachments/assets/b29e569d-8332-4983-b889-051a9490f36a


Sawtooth wave:


https://github.com/user-attachments/assets/79dd8e86-7724-4e1c-a650-aa2f94826bef


---

## �📖 Documentation

The complete code documentation and reference guide for MidiSynth can be found at the following link:

🔗 **[Documentation Link](https://alelith.github.io/MidiSynth-documentation/)**

The documentation includes:
- API references for the audio engine and synthesizer components
- Overview of the MIDI queuing structure
- Implementation details for the visualizer and FFT data handling

---

## 🤝 Credits

-   [PortAudio](http://www.portaudio.com/)
-   [RtMidi](https://www.music.mcgill.ca/~gary/rtmidi/)
-   [Raylib](https://www.raylib.com/)
-   [FFTW](http://www.fftw.org/)

---

## 📜 License

This project is licensed under the GPL-3.0 License - see the LICENSE file for details.

The GNU General Public License is a free, copyleft license that requires any modified versions to be explicitly marked as changed and ensures that any derived works remain free and open.

---

## 👩‍💻 Author

**Lilith Estévez Boeta**  
💻 Backend & Multiplatform Developer  
📍 Málaga, Spain

🔗 [GitHub](https://github.com/Alelith) | [LinkedIn](https://www.linkedin.com/in/alelith/)

---

<p align="center">
  <b>⭐ If you like this project, don't forget to leave a star on GitHub ⭐</b>
</p>
