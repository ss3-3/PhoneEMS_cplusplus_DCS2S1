#ifndef SYSTEM_DATA_H
#define SYSTEM_DATA_H

#include <vector>
#include "venue.h"
#include "user.h"
#include "event_registration.h"
#include "event_booking.h"
#include "timeslot.h"
#include "event_payment.h"
#include "feedback.h"
using namespace std;

struct SystemData {
    string currentUser;
    vector<Venue> venues;
    vector<Organizer> organizer;
    vector<EventRegistration> registrations;
    vector<EventBooking> bookings;
    TimeSlotConfig timeConfig; 
    vector<EventFeedback> feedbacks;
    vector<Payment> payments;

    SystemData();
    void initializeSampleData();
    void loadAllData();
};

#endif