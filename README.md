## HOW TO COMPILE (MAC)
g++ -std=c++17 ECGraphicViewImp.cpp SimpleObserver.cpp ElevatorSimulator.cpp $(pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 --libs --cflags) -o ElevatorSimulator

## HOW TO RUN
./ElevatorSimulator test-file-1.txt