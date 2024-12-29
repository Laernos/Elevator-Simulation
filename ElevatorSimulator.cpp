#include "ECGraphicViewImp.h"
#include "SimpleObserver.h"
#include <iostream>
#include <string>

int real_main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    const std::string inputFile = argv[1];

    const int widthWin = 600, heightWin = 700;
    ECGraphicViewImp view(widthWin, heightWin);
    ElevatorSimulatorObserver elevatorSimulator(view, inputFile);
    view.Attach(&elevatorSimulator);
    view.Show();
    return 0;
}

int main(int argc, char **argv) {
    return real_main(argc, argv);
}