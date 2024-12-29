#ifndef SimpleObserver_h
#define SimpleObserver_h

#include "ECObserver.h"
#include "ECGraphicViewImp.h"
#include <vector>

struct PassengerRequest {
    int time;
    int startFloor;
    int targetFloor;
};

struct Passenger {
    int startFloor;
    int targetFloor;
};

class ElevatorSimulatorObserver : public ECObserver {
public:
    ElevatorSimulatorObserver(ECGraphicViewImp &viewIn, const std::string &filename);

    virtual void Update();

private:
    void Draw();                     // Helper function to draw the elevator and floors
    void MoveElevator();             // Handle elevator movement logic
    void StopElevator();             // Handle stopping at floors
    void CreateRandomPassenger();    // Create a random passenger
    void InitializeRequests(const std::string &filename);
    void AddPassengerRequest(int time, int startFloor, int targetFloor);

    ECGraphicViewImp &view;
    int elevatorY;                   // Elevator's current Y position
    int direction;                   // Direction of elevator: 1 for up, -1 for down, 0 for stopped
    bool isMoving;                   // Is the elevator currently moving?
    int currentFloor;
    int targetFloor;
    int simulationTime;  
    bool paused; 
    int elapsedTime;
    int FindNearestPassengerFloor(int direction) const;
    std::vector<Passenger> passengers;
    std::vector<Passenger> cabinPassengers;
    std::vector<PassengerRequest> predefinedRequests;
};

#endif