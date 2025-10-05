# Allegro Elevator Simulator

A real-time, Allegro-powered visualization of a single-elevator controller. The simulator replays a scripted set of passenger requests, animates pickup/drop-off, and reports status in real time.

## Prerequisites
- C++17 toolchain (`g++` or clang++)
- Allegro 5

### How to Compile (macOS)
```bash
brew install allegro
```
```bash
g++ -std=c++17 ECGraphicViewImp.cpp SimpleObserver.cpp ElevatorSimulator.cpp $(pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 --libs --cflags) -o ElevatorSimulator
```
### How to Run
```bash
./ElevatorSimulator test-file-1.txt
```
