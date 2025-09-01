#pragma once
#ifndef TIMESLOT_H
#define TIMESLOT_H

#include <string>
#include <vector>
#include "date.h"
using namespace std;

// Time slot configuration
struct TimeSlotConfig {
    vector<string> timeSlots;
    vector<string> timeSlotNames;

    TimeSlotConfig() {
        timeSlots = { "09:00-13:00", "17:00-21:00" };
        timeSlotNames = {
            "Morning Session (9AM - 1PM)",
            "Evening Session (5PM - 9PM)"
        };
    }
};

// Time slot for venue booking
struct TimeSlot {
    Date date;
    string time;       // e.g. "09:00-13:00"
    string eventID;    // link to EventRegistration
    bool isBooked = false;

    string getDateString() const;
};

#endif