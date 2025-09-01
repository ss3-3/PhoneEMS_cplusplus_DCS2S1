#include "venue.h"
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
#include "event_registration.h"
#include "event_booking.h"
#include "system_data.h"
#include "timeslot.h"
#include "product.h"
#include "utility_fun.h"
#include "input_validation.h"
#include "file_operation.h"

using namespace std;

// Venue management functions
int selectAvailableVenue(const vector<Venue>& venues, const Date& date, const string& time) {
    // Build list of available venue indices
    vector<int> availableIndices;

    for (size_t i = 0; i < venues.size(); i++) {
        if (isVenueAvailableInSchedule(venues[i], date, time)) {
            availableIndices.push_back(static_cast<int>(i));
        }
    }

    if (availableIndices.empty()) {
        return -1; // No venues available
    }

    cout << "\nTotal available venues: " << availableIndices.size() << endl;
    int choice = getValidIntegerInput("Select venue number: ", 1, static_cast<int>(availableIndices.size()));

    return availableIndices[choice - 1]; // Return actual venue index
}

void displayAvailableVenues(const vector<Venue>& venues, const Date& date, const string& time) {
    cout << "\n=== AVAILABLE VENUES ===" << endl;
    cout << "Date: " << date.toString() << " | Time: " << time << endl;
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;

    cout << left << setw(4) << "No."
        << setw(8) << "ID"
        << setw(20) << "Venue Name"
        << setw(25) << "Address"
        << setw(8) << "Capacity"
        << setw(12) << "Cost (RM)"
        << setw(15) << "Contact"
        << setw(12) << "Phone" << endl;
    cout << setfill('-') << setw(100) << "-" << setfill(' ') << endl;

    int count = 1;
    for (size_t i = 0; i < venues.size(); i++) {
        if (isVenueAvailableInSchedule(venues[i], date, time)) {
            cout << left << setw(4) << count
                << setw(8) << venues[i].venueID
                << setw(20) << venues[i].venueName.substr(0, 19)
                << setw(25) << venues[i].address.substr(0, 24)
                << setw(8) << venues[i].capacity
                << "RM " << setw(9) << fixed << setprecision(2) << venues[i].rentalCost
                << setw(15) << venues[i].contactPerson.substr(0, 14)
                << setw(12) << venues[i].phoneNumber << endl;
            count++;
        }
    }

    if (count == 1) {
        cout << "No venues available for the selected date and time." << endl;
    }
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;
}

bool isVenueAvailable(const vector<EventBooking>& bookings, const string& venueID, const Date& eventDate, const string& eventTime) {
    for (const auto& booking : bookings) {
        if (booking.venue.venueID == venueID &&
            booking.eventDate.day == eventDate.day &&
            booking.eventDate.month == eventDate.month &&
            booking.eventDate.year == eventDate.year &&
            booking.eventTime == eventTime &&
            booking.bookingStatus != "Cancelled") {

            cout << "Venue conflict found:" << endl;
            cout << "Existing booking ID: " << booking.bookingID << endl;
            cout << "Event: " << booking.eventReg.eventTitle << endl;
            cout << "Organizer: " << booking.eventReg.organizer.organizerName << endl;
            return false;
        }
    }
    return true;
}

bool isVenueAvailableInSchedule(const Venue& venue, const Date& date, const string& time) {
    for (const auto& slot : venue.bookingSchedule) {
        if (slot.date.day == date.day &&
            slot.date.month == date.month &&
            slot.date.year == date.year &&
            slot.time == time &&
            slot.isBooked) {
            return false;
        }
    }
    return true;
}

EventRegistration* findRegistrationByID(vector<EventRegistration>& registrations, const string& eventID) {
    for (size_t i = 0; i < registrations.size(); i++) {
        if (registrations[i].eventID == eventID) {
            return &registrations[i];
        }
    }
    return nullptr;
}