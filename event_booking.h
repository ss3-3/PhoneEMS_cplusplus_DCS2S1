#pragma once
#ifndef EVENT_BOOKING_H
#define EVENT_BOOKING_H

#include "date.h"
#include "venue.h"
#include "event_registration.h"
#include "timeslot.h"
#include "logistics.h"

using namespace std;

// Event Booking (links registration with date and venue)
struct EventBooking {
    string bookingID;
    EventRegistration eventReg;
    Date eventDate;
    string eventTime;
    Venue venue;
    string bookingStatus; // "Pending", "Confirmed", "Completed", "Cancelled"
    double finalCost;
    vector<string> logisticsItems;
    double logisticsCost;
};

// Booking Status Logic Clarification:
// PENDING -> User created booking, waiting for payment
// CONFIRMED -> Payment completed, booking is confirmed
// COMPLETED -> Event has finished
// CANCELLED -> Booking was cancelled cause of registration cancellation

// Function prototypes - Event Booking System
class SystemData;
void eventBookingMenu(SystemData& data);
void createEventBooking(SystemData& data);
void viewEventBookings(const SystemData& data);
void updateEventBooking(SystemData& data);
void cancelEventBooking(SystemData& data);
void removeBookingFromVenueSchedule(vector<Venue>& venues, const string& bookingID);
void updateBookingsForCancelledEvent(vector<EventBooking>& bookings, const string& eventID);

#endif