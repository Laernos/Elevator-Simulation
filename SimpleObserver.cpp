#include "SimpleObserver.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iostream>

ElevatorSimulatorObserver::ElevatorSimulatorObserver(ECGraphicViewImp &viewIn, const std::string &filename)
    : view(viewIn), elevatorY(600 - (1 * 50)), direction(0), isMoving(false),
      currentFloor(1), targetFloor(1), simulationTime(0), elapsedTime(0), paused(false) {

    std::srand(std::time(nullptr)); 
    InitializeRequests(filename);
    Draw();
    view.SetRedraw(true);
}

void ElevatorSimulatorObserver::InitializeRequests(const std::string &filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open the file: " << filename << std::endl;
        return;
    }

    std::string line;
    bool isMetadataLine = true;

    while (std::getline(infile, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        if (isMetadataLine) {
            isMetadataLine = false;
            continue;
        }
        std::istringstream iss(line);
        int time, startFloor, targetFloor;
        if (iss >> time >> startFloor >> targetFloor) {
            predefinedRequests.push_back({time, startFloor, targetFloor});
        }
    }

    infile.close();

    std::cout << "Passenger requests initialized" << filename << ".\n";
}


int ElevatorSimulatorObserver::FindNearestPassengerFloor(int direction) const {
    int nearestFloor = -1;
    int minDistance = 10;

    for (const auto &passenger : passengers) {
        int distance = abs(passenger.startFloor - currentFloor);
        if ((direction == 1 && passenger.startFloor > currentFloor) || // Going up
            (direction == -1 && passenger.startFloor < currentFloor) || // Going down
            (direction == 0)) {
            if (distance < minDistance) {
                minDistance = distance;
                nearestFloor = passenger.startFloor;
            }
        }
    }

    return nearestFloor;
}

void ElevatorSimulatorObserver::Update() {
    ECGVEventType evt = view.GetCurrEvent();

    // Handle pause
    if (evt == ECGV_EV_KEY_DOWN_SPACE) {
        paused = !paused;
        return;
    }

    // do nothing
    if (paused) {
        return;
    }

    if (evt == ECGV_EV_TIMER) {
        elapsedTime += 1000 / 60;
        if (elapsedTime >= 1000) {
            simulationTime++;
            elapsedTime -= 1000;
            auto it = predefinedRequests.begin();
            while (it != predefinedRequests.end()) {
                if (it->time <= simulationTime) {
                    passengers.push_back({it->startFloor, it->targetFloor});
                    it = predefinedRequests.erase(it);
                } else {
                    ++it;
                }
            }

            if (!isMoving && !passengers.empty()) {
                targetFloor = FindNearestPassengerFloor(0);
                if (targetFloor != -1) {
                    direction = (targetFloor > currentFloor) ? 1 : -1;
                    isMoving = true;
                }
            }
        }

        if (isMoving) {
            MoveElevator();
        }

        // Check for simulation completion
        if (predefinedRequests.empty() && passengers.empty() && cabinPassengers.empty() && !isMoving) {
            // Update the status text to "Simulation Completed"
            std::string statusText = "Simulation Completed";
            view.DrawText(300, 40, statusText.c_str(), ECGV_RED);
            view.SetRedraw(true);
            al_rest(1.0); // Wait for 1 seconds
            exit(0); // Terminate the program
        }

        view.SetRedraw(true);
    }

    Draw();
}

void ElevatorSimulatorObserver::CreateRandomPassenger() {
    int startFloor, targetFloor;

    // draw floors
    do {
        startFloor = std::rand() % 10 + 1;
        targetFloor = std::rand() % 10 + 1;
    } while (startFloor == targetFloor);

    passengers.push_back({startFloor, targetFloor});
    view.SetRedraw(true);

    if (!isMoving) {
        this->targetFloor = FindNearestPassengerFloor(1);
        if (targetFloor != -1) {
            direction = (targetFloor > currentFloor) ? 1 : -1;
            isMoving = true;
        }
    }
}

void ElevatorSimulatorObserver::MoveElevator() {
    int targetY = 600 - (targetFloor * 50);

    // Move the elevator smoothly
    if (direction == 1 && elevatorY > targetY) { // Move up
        elevatorY -= 2;
    } else if (direction == -1 && elevatorY < targetY) { // Head down
        elevatorY += 2;
    } else {
        elevatorY = targetY;
        StopElevator();
    }
    view.SetRedraw(true);
}

void ElevatorSimulatorObserver::AddPassengerRequest(int time, int startFloor, int targetFloor) {
    predefinedRequests.push_back({time, startFloor, targetFloor});
}

void ElevatorSimulatorObserver::StopElevator() {
    currentFloor = targetFloor;
    direction = 0;
    isMoving = false;

    // drop off passengers
    for (auto it = cabinPassengers.begin(); it != cabinPassengers.end();) {
        if (it->targetFloor == currentFloor) {
            it = cabinPassengers.erase(it); // remove passenger from cabin
        } else {
            ++it;
        }
    }

    // pick up passengers
    for (auto it = passengers.begin(); it != passengers.end();) {
        if (it->startFloor == currentFloor) {
            cabinPassengers.push_back(*it); // add to cabin
            it = passengers.erase(it);     // remove from waiting list
        } else {
            ++it;
        }
    }

    if (!cabinPassengers.empty()) {
        // move to the next destination
        targetFloor = cabinPassengers.front().targetFloor;
        direction = (targetFloor > currentFloor) ? 1 : -1;
        isMoving = true;
    } else if (!passengers.empty()) {
        int nearestFloor = FindNearestPassengerFloor(0);
        if (nearestFloor != -1) {
            targetFloor = nearestFloor;
            direction = (targetFloor > currentFloor) ? 1 : -1;
            isMoving = true;
        }
    }
    view.SetRedraw(true);
}

void ElevatorSimulatorObserver::Draw() {
    view.DrawFilledRectangle(0, 0, view.GetWidth(), view.GetHeight(), ECGV_YELLOW);
    
    // Elevator limits
    view.DrawLine(150, 100, 150, 600, 3, ECGV_BLACK); // left limit
    view.DrawLine(450, 100, 450, 600, 3, ECGV_BLACK); // right limit

    for (int i = 600, floor = 1; i > 100; i -= 50, ++floor) {
        view.DrawLine(150, i, 450, i, 2, ECGV_BLACK); // floor line
        ECGVColor textColor = (currentFloor == floor) ? ECGV_RED : ECGV_BLACK; //cabin
        view.DrawText(120, i - 35, std::to_string(floor).c_str(), textColor);

        int circleY = i - 25;
        bool fillUp = false;
        bool fillDown = false;

        // collect passengers
        std::vector<int> upPassengers;
        std::vector<int> downPassengers;

        for (const auto &passenger : passengers) {
            if (passenger.startFloor == floor) {
                if (passenger.targetFloor > passenger.startFloor) {
                    upPassengers.push_back(passenger.targetFloor);
                    fillUp = true;
                } else {
                    downPassengers.push_back(passenger.targetFloor);
                    fillDown = true;
                }
            }
        }

        // up circle
        if (fillUp) {
            view.DrawFilledCircle(470, circleY, 10, ECGV_GREEN);
        } else {
            view.DrawCircle(470, circleY, 10, 3, ECGV_GREEN);
        }

        // down circle
        if (fillDown) {
            view.DrawFilledCircle(470, circleY + 20, 10, ECGV_RED);
        } else {
            view.DrawCircle(470, circleY + 20, 10, 3, ECGV_RED);
        }

        // Passenger Rectangle size
        int rectWidth = 50;
        int rectHeight = 40;

        // Draw up passengers
        for (size_t j = 0; j < upPassengers.size(); ++j) {
            int rectX = 500 + j * (rectWidth + 10);
            int rectY = circleY - rectHeight / 2;
            view.DrawFilledRectangle(rectX, rectY, rectX + rectWidth, rectY + rectHeight, ECGV_BLUE);
            view.DrawText(rectX + 15, rectY + 10, std::to_string(upPassengers[j]).c_str(), ECGV_WHITE);
        }

        // Draw down passengers
        for (size_t j = 0; j < downPassengers.size(); ++j) {
            int rectX = 500 + j * (rectWidth + 10);
            int rectY = circleY + 25;
            view.DrawFilledRectangle(rectX, rectY, rectX + rectWidth, rectY + rectHeight, ECGV_BLUE);
            view.DrawText(rectX + 15, rectY + 10, std::to_string(downPassengers[j]).c_str(), ECGV_WHITE);
        }
    }

    // Draw the elevator cabin
    view.DrawFilledRectangle(150, elevatorY, 450, elevatorY + 50, ECGV_BLUE);

    int cabinX = 200;
    int cabinY = elevatorY + 10;
    for (size_t i = 0; i < cabinPassengers.size(); ++i) {
        int rectX = cabinX + i * 40;
        int rectY = cabinY;
        view.DrawFilledRectangle(rectX, rectY, rectX + 30, rectY + 20, ECGV_BLUE); 
        view.DrawText(rectX + 5, rectY + 5, std::to_string(cabinPassengers[i].targetFloor).c_str(), ECGV_WHITE); // Target floor
    }

    std::string passengerCountText = "Total Riders: " + std::to_string(cabinPassengers.size());
    int bottomTextY = 650;
    view.DrawText(300, bottomTextY, passengerCountText.c_str(), ECGV_BLACK);

    

    // Textz
    //view.DrawText(300, 20, "Elevator Status:", ECGV_BLACK);
    std::string statusText;
    if (paused) {
        statusText = "Status: PAUSED";
    } else if (isMoving) {
        statusText = "Status: " + std::string(direction == 1 ? "Heading Up" : "Heading Down");
    } else {
        statusText = "Status: Stopped";
    }
    view.DrawText(300, 40, statusText.c_str(), ECGV_BLACK);
    //view.DrawText(300, 60, ("Floor: " + std::to_string(currentFloor)).c_str(), ECGV_BLACK);
    std::string timeText = "Time: " + std::to_string(simulationTime) + "s";
    view.DrawText(300, 10, timeText.c_str(), ECGV_BLACK);
}