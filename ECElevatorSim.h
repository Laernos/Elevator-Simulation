//
//  ECElevatorSim.h
//  
//
//  Created by Yufeng Wu on 6/27/23.
//  Elevator simulation

#ifndef ECElevatorSim_h
#define ECElevatorSim_h

#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <string>

//*****************************************************************************
// DON'T CHANGE THIS CLASS
// 
// Elevator simulation request: 
// (i) time: when the request is made
// (ii) floorSrc: which floor the user is at at present
// (iii) floorDest floor: where the user wants to go; we assume floorDest != floorSrc
// 
// Note: a request is in three stages:
// (i) floor request: the passenger is waiting at floorSrc; once the elevator arrived 
// at the floor (and in the right direction), move to the next stage
// (ii) inside request: passenger now requests to go to a specific floor once inside the elevator
// (iii) Once the passenger arrives at the floor, this request is considered to be "serviced"
//
// two sspecial requests:
// (a) maintenance start: floorSrc=floorDest=-1; put elevator into maintenance 
// starting at the specified time; elevator starts at the current floor
// (b) maintenance end: floorSrc=floorDest=0; put elevator back to operation (from the current floor)

class ECElevatorSimRequest
{
public:
    ECElevatorSimRequest(int timeIn, int floorSrcIn, int floorDestIn)
        : time(timeIn), floorSrc(floorSrcIn), floorDest(floorDestIn),
          fFloorReqDone(false), fServiced(false), timeArrive(-1) {}
    int GetTime() const { return time; }
    int GetFloorSrc() const { return floorSrc; }
    int GetFloorDest() const { return floorDest; }
    bool IsGoingUp() const { return floorDest >= floorSrc; }
    bool IsFloorRequestDone() const { return fFloorReqDone; }
    void SetFloorRequestDone(bool f) { fFloorReqDone = f; }
    bool IsServiced() const { return fServiced; }
    void SetServiced(bool f) { fServiced = f; }
    int GetRequestedFloor() const;
    int GetArriveTime() const { return timeArrive; }
    void SetArriveTime(int t) { timeArrive = t; }

private:
    int time;           // time of request made
    int floorSrc;       // which floor the request is made
    int floorDest;      // which floor is going
    bool fFloorReqDone;   // is this passenger passing stage one (no longer waiting at the floor) or not
    bool fServiced;     // is this request serviced already?
    int timeArrive;     // when the user gets to the desitnation floor
};

//*****************************************************************************
// Elevator moving direction
typedef enum
{
    EC_ELEVATOR_STOPPED = 0,    // not moving
    EC_ELEVATOR_UP,             // moving up
    EC_ELEVATOR_DOWN            // moving down
} EC_ELEVATOR_DIR;

//*****************************************************************************
// Add your own classes here...
// Abstract Base Class for Elevator
class ElevatorBase
{
public:
    ElevatorBase(int numFloors) : numFloors(numFloors), currFloor(1), currDir(EC_ELEVATOR_STOPPED) {}
    virtual ~ElevatorBase() {}

    virtual void Simulate(int lenSim) = 0;
    virtual void MoveToFloor(int targetFloor) = 0;

    int GetCurrFloor() const { return currFloor; }
    void SetCurrFloor(int f) { currFloor = f; }
    EC_ELEVATOR_DIR GetCurrDir() const { return currDir; }
    void SetCurrDir(EC_ELEVATOR_DIR dir) { currDir = dir; }

protected:
    int numFloors;
    int currFloor;
    EC_ELEVATOR_DIR currDir;
};

//*****************************************************************************
// Simulation of elevator

class ECElevatorSim : public ElevatorBase
{
public:
    ECElevatorSim(int numFloors, std::vector<ECElevatorSimRequest> &listRequests);
    ~ECElevatorSim();

    void Simulate(int lenSim) override;
    void MoveToFloor(int targetFloor) override;
    const std::vector<ECElevatorSimRequest *> &GetPassengersInCabin() const {
        return passengersInCabin;
    }

    const std::vector<ECElevatorSimRequest *> &GetPendingRequests() const {
        return pendingRequests;
    }

private:
    // Your code here
    int timeElapsed;
    std::vector<ECElevatorSimRequest *> pendingRequests;
    std::vector<ECElevatorSimRequest> &listRequests;

    // New member variables
    std::vector<ECElevatorSimRequest *> passengersInCabin;

    // New member functions
    void CollectRequests(int currentTime);
    void MoveOneFloor();
    void DecideDirection();
    bool HasFurtherRequestsInCurrentDirection();
    bool HandlePassengers();
};

#endif /* ECElevatorSim_h */