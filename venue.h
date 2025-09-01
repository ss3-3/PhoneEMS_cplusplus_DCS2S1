#pragma once
#ifndef VENUE_H
#define VENUE_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "timeslot.h"

using namespace std;

struct Venue {
    string venueID;
    string venueName;
    string address;
    int capacity = 0;
    double rentalCost = 0.0;
    string contactPerson;
    string phoneNumber;
    vector<TimeSlot> bookingSchedule;
};

// Forward declaration to avoid circular dependency
struct Date;
struct Venue;
struct EventBooking;
struct EventRegistration;

// Venue management functions
void displayAvailableVenues(const vector<Venue>& venues, const Date& date, const string& time);
int selectAvailableVenue(const vector<Venue>& venues, const Date& date, const string& time);
bool isVenueAvailable(const vector<EventBooking>& bookings, const string& venueID, const Date& eventDate, const string& eventTime); 
bool isVenueAvailableInSchedule(const Venue& venue, const Date& date, const string& time); 
EventRegistration* findRegistrationByID(vector<EventRegistration>& registrations, const string& eventID);

#endif