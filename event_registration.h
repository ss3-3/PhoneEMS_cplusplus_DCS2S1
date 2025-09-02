#pragma once
#ifndef EVENT_REGISTRATION_H
#define EVENT_REGISTRATION_H

#include <vector>
#include <string>
#include "product.h"
#include "user.h"
using namespace std;

struct EventRegistration {
    string eventID;
    string manufacturer;
    string eventTitle;
    int productQuantity;
    vector<Product> phoneInfo;   // one or many phone models
    string description;          // describe the event activity
    int expectedGuests;          // how many people attend
    double estimatedBudget;
    string eventStatus;          // "REGISTERED", "SCHEDULED", "UNSCHEDULED"
    Organizer organizer;
};

// Function prototypes - Event Registration System
class SystemData;
struct EventBooking;
void eventRegistrationMenu(SystemData& data);
void createEventRegistration(SystemData& data);
void viewEventRegistrations(const SystemData& data);
void editPhoneInfo(EventRegistration* regPtr);
void updateEventRegistration(SystemData& data);
void deleteEventRegistration(SystemData& data);
#endif