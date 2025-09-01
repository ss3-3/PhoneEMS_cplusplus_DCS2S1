#pragma once
#ifndef FILE_OPERATION_H
#define FILE_OPERATION_H

#include "system_data.h"

using namespace std;
// Function prototypes - File Operations
void saveUsersToFile(const vector<Organizer>& users);
void loadUserFromFile(vector<Organizer>& users);
void saveDataToFiles(const SystemData& data);
void loadDataFromFiles(SystemData& data);
void saveVenuesToFile(const vector<Venue>& venues);
void saveRegistrationsToFile(const vector<EventRegistration>& registrations);
void saveBookingsToFile(const vector<EventBooking>& bookings);
void loadVenuesFromFile(vector<Venue>& venues);
void loadRegistrationsFromFile(vector<EventRegistration>& registrations);
void loadBookingsFromFile(vector<EventBooking>& bookings);
void saveFeedbackToFile(const vector<EventFeedback>& feedbacks);
void loadFeedbackFromFile(vector<EventFeedback>& feedbacks);
void savePaymentsToFile(const vector<Payment>& payments);
void loadPaymentsFromFile(vector<Payment>& payments);
#endif
