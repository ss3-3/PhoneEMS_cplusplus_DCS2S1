#pragma once

#ifndef UTILITY_FUN_H
#define UTILITY_FUN_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <regex>
#include "date.h"
#include "venue.h"
#include "event_registration.h"
#include "event_booking.h"
#include "user.h"
#include "timeslot.h"

using namespace std;

// Function prototypes - Utility functions
string trimWhitespace(const string& str);
string normalizeUserID(const string& userID);
string getCurrentUserName(const SystemData& data);
void displayLogo();
void clearScreen();
Date getCurrentDate();
string generateEventID(const vector<EventRegistration>& registrations);
string generateBookingID(const vector<EventBooking>& bookings);
string generateVenueID(const vector<Venue>& venues);
string formatDouble(double value, int precision = 2);
void pauseScreen();

#endif
