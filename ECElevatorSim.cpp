//
//  ECElevatorSim.cpp
//  
//
//  Created by Yufeng Wu on 6/27/23.
//  Elevator simulation

#include "ECElevatorSim.h"
#include <algorithm>
#include <unordered_set>

using namespace std;

// Constructor
ECElevatorSim::ECElevatorSim(int numFloors, std::vector<ECElevatorSimRequest> &listRequests)
    : ElevatorBase(numFloors), timeElapsed(0), listRequests(listRequests)
{
    currFloor = 1; // Start at floor 1
    currDir = EC_ELEVATOR_STOPPED;
}

// Destructor
ECElevatorSim::~ECElevatorSim() {}

//elevator
void ECElevatorSim::Simulate(int lenSim)
{
    while (timeElapsed < lenSim)
    {
        CollectRequests(timeElapsed);

        if (currDir != EC_ELEVATOR_STOPPED)
        {
            MoveOneFloor();
        }
        else
        {
            // elevator is stopped
            if (!pendingRequests.empty() || !passengersInCabin.empty())
            {
                DecideDirection();
                if (currDir != EC_ELEVATOR_STOPPED)
                {
                    MoveOneFloor();
                }
                else
                {
                    ++timeElapsed;
                }
            }
            else
            {
                ++timeElapsed;
            }
        }
    }
}

void ECElevatorSim::CollectRequests(int currentTime)
{
    for (auto &request : listRequests)
    {
        if (request.GetTime() == currentTime && !request.IsServiced())
        {
            if (request.GetFloorSrc() == currFloor && currDir == EC_ELEVATOR_STOPPED)
            {
                request.SetFloorRequestDone(true);
                passengersInCabin.push_back(&request);
            }
            else
            {
                pendingRequests.push_back(&request);
            }
        }
    }
}

void ECElevatorSim::MoveOneFloor()
{
    currFloor += (currDir == EC_ELEVATOR_UP) ? 1 : -1;
    ++timeElapsed;

    CollectRequests(timeElapsed);

    bool needToStop = HandlePassengers();

    if (needToStop)
    {
        ++timeElapsed;
        CollectRequests(timeElapsed);
    }

    if (!HasFurtherRequestsInCurrentDirection())
    {
        currDir = EC_ELEVATOR_STOPPED;
    }

    if ((currFloor == numFloors && currDir == EC_ELEVATOR_UP) ||
        (currFloor == 1 && currDir == EC_ELEVATOR_DOWN))
    {
        currDir = EC_ELEVATOR_STOPPED;
    }
}

bool ECElevatorSim::HandlePassengers()
{
    bool needToStop = false;

    // Handle passengers exiting
    auto it = passengersInCabin.begin();
    while (it != passengersInCabin.end())
    {
        if ((*it)->GetFloorDest() == currFloor)
        {
            (*it)->SetServiced(true);
            (*it)->SetArriveTime(timeElapsed);
            it = passengersInCabin.erase(it);
            needToStop = true;
        }
        else
        {
            ++it;
        }
    }

    // Handle new passengers boarding
    auto itReq = pendingRequests.begin();
    while (itReq != pendingRequests.end())
    {
        if (!(*itReq)->IsFloorRequestDone() && (*itReq)->GetFloorSrc() == currFloor)
        {
            (*itReq)->SetFloorRequestDone(true);
            passengersInCabin.push_back(*itReq);
            itReq = pendingRequests.erase(itReq);
            needToStop = true;
        }
        else
        {
            ++itReq;
        }
    }

    return needToStop;
}

void ECElevatorSim::DecideDirection()
{
    int nearestFloor = -1;
    int minDistance = numFloors + 1;
    EC_ELEVATOR_DIR dir = EC_ELEVATOR_STOPPED;

    for (auto *request : passengersInCabin)
    {
        int distance = abs(request->GetFloorDest() - currFloor);
        if (distance < minDistance || (distance == minDistance && request->GetFloorDest() > currFloor))
        {
            nearestFloor = request->GetFloorDest();
            minDistance = distance;
            dir = (request->GetFloorDest() > currFloor) ? EC_ELEVATOR_UP : EC_ELEVATOR_DOWN;
        }
    }

    // check pending requests
    for (auto *request : pendingRequests)
    {
        if (!request->IsFloorRequestDone())
        {
            int distance = abs(request->GetFloorSrc() - currFloor);
            if (distance < minDistance || (distance == minDistance && request->GetFloorSrc() > currFloor))
            {
                nearestFloor = request->GetFloorSrc();
                minDistance = distance;
                dir = (request->GetFloorSrc() > currFloor) ? EC_ELEVATOR_UP : EC_ELEVATOR_DOWN;
            }
        }
    }

    currDir = dir;
}

bool ECElevatorSim::HasFurtherRequestsInCurrentDirection()
{
    // check if any passengers in the caibn
    for (auto *request : passengersInCabin)
    {
        int floor = request->GetFloorDest();
        if ((currDir == EC_ELEVATOR_UP && floor > currFloor) ||
            (currDir == EC_ELEVATOR_DOWN && floor < currFloor))
        {
            return true;
        }
    }

    // check all requests
    for (auto *request : pendingRequests)
    {
        if (!request->IsFloorRequestDone())
        {
            int floor = request->GetFloorSrc();
            if ((currDir == EC_ELEVATOR_UP && floor > currFloor) ||
                (currDir == EC_ELEVATOR_DOWN && floor < currFloor))
            {
                return true;
            }
        }
    }

    return false;
}

void ECElevatorSim::MoveToFloor(int targetFloor)
{

}

int ECElevatorSimRequest::GetRequestedFloor() const
{
    if (IsServiced())
    {
        return -1;
    }
    else if (IsFloorRequestDone())
    {
        return GetFloorDest();
    }
    else
    {
        return GetFloorSrc();
    }
}