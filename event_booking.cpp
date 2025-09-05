#include "event_booking.h"
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
#include "system_data.h"
#include "timeslot.h"
#include "user.h"
#include "product.h"
#include "utility_fun.h"
#include "input_validation.h"
#include "file_operation.h"

using namespace std;

// Event Booking System Functions
void eventBookingMenu(SystemData& data) {
    bool exitMenu = false;

    while (!exitMenu) {
        clearScreen();
        displayLogo();
        cout << "=== EVENT BOOKING SYSTEM ===" << endl;
        cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;
        cout << "1. Create New Event Booking" << endl;
        cout << "2. View My Event Bookings" << endl;
        cout << "3. Update Event Booking" << endl;
        cout << "4. Cancel Event Booking" << endl;
        cout << "5. Back to Main Menu" << endl;
        cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;

        int choice = getValidIntegerInput("Enter your choice [1-5]: ", 1, 5);

        switch (choice) {
        case 1:
            createEventBooking(data);
            break;
        case 2:
            viewEventBookings(data);
            break;
        case 3:
            updateEventBooking(data);
            break;
        case 4:
            cancelEventBooking(data);
            break;
        case 5:
            exitMenu = true;
            break;
        }

        if (choice != 5) {
            pauseScreen();
        }
    }
}

void createEventBooking(SystemData& data) {
    clearScreen();
    cout << "=== CREATE NEW EVENT BOOKING ===" << endl;
    cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    if (data.registrations.empty()) {
        cout << "No event registrations available for booking!" << endl;
        cout << "Please create an event registration first." << endl;
        return;
    }

    // Display approved registrations that belong to current user (exclude cancelled)
    vector<EventRegistration*> userApprovedRegs;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);
    for (auto& reg : data.registrations) {
        string normalizedRegUser = normalizeUserID(reg.organizer.userID);
        if ((reg.eventStatus == "UNSCHEDULED") &&
            normalizedRegUser == normalizedCurrentUser) {
            userApprovedRegs.push_back(&reg);
        }
    }

    if (userApprovedRegs.empty()) {
        cout << "No approved event registrations available for booking under your account!" << endl;
        cout << "Please wait for your registrations to be approved or create new ones." << endl;
        return;
    }

    cout << "=== YOUR AVAILABLE REGISTRATIONS FOR BOOKING ===" << endl;
    cout << setfill('=') << setw(80) << "=" << setfill(' ') << endl;
    cout << left
        << setw(4) << "No."
        << setw(8) << "Event ID"
        << setw(15) << "Event Title"
        << setw(12) << "Manufacturer"
        << setw(20) << "Organizer"
        << setw(12) << "Status" << endl;
    cout << setfill('=') << setw(80) << "=" << setfill(' ') << endl;

    for (size_t i = 0; i < userApprovedRegs.size(); i++) {
        cout << left
            << setw(4) << (i + 1)
            << setw(8) << userApprovedRegs[i]->eventID
            << setw(15) << userApprovedRegs[i]->eventTitle.substr(0, 14)
            << setw(12) << userApprovedRegs[i]->manufacturer.substr(0, 11)
            << setw(20) << userApprovedRegs[i]->organizer.organizerName.substr(0, 19)
            << setw(12) << userApprovedRegs[i]->eventStatus<< endl;
    }
    cout << setfill('=') << setw(80) << "=" << setfill(' ') << endl;

    int regChoice = getValidIntegerInput("Select registration number: ", 1, static_cast<int>(userApprovedRegs.size()));
    EventRegistration* selectedReg = userApprovedRegs[regChoice - 1];

    EventBooking newBooking;
    newBooking.bookingID = generateBookingID(data.bookings);
    newBooking.eventReg = *selectedReg;

    cout << "\nSelected Event: " << selectedReg->eventTitle << " by " << selectedReg->manufacturer << endl;
    cout << "Generated Booking ID: " << newBooking.bookingID << endl;

    // Get event date
    cout << "\nEnter Event Date:" << endl;
    newBooking.eventDate = getValidDateInput();

    // Select time slot
    cout << "\nAvailable Time Slots:" << endl;
    for (size_t i = 0; i < data.timeConfig.timeSlotNames.size(); i++) {
        cout << (i + 1) << ". " << data.timeConfig.timeSlotNames[i] << endl;
    }

    int timeChoice = getValidIntegerInput("Select time slot [1-" +
        to_string(data.timeConfig.timeSlotNames.size()) + "]: ", 1,
        static_cast<int>(data.timeConfig.timeSlotNames.size()));

    newBooking.eventTime = data.timeConfig.timeSlots[timeChoice - 1];

    // CHECK FOR DUPLICATE BOOKING
    for (const auto& existingBooking : data.bookings) {
        if (existingBooking.eventReg.eventID == selectedReg->eventID &&
            existingBooking.eventDate.day == newBooking.eventDate.day &&
            existingBooking.eventDate.month == newBooking.eventDate.month &&
            existingBooking.eventDate.year == newBooking.eventDate.year &&
            existingBooking.eventTime == newBooking.eventTime &&
            existingBooking.bookingStatus != "Cancelled") {

            cout << "\nERROR: Duplicate booking detected!" << endl;
            cout << "This event registration already has a booking for:" << endl;
            cout << "Date: " << existingBooking.eventDate.toString() << endl;
            cout << "Time: " << existingBooking.eventTime << endl;
            cout << "Booking ID: " << existingBooking.bookingID << endl;
            cout << "Status: " << existingBooking.bookingStatus << endl;
            cout << "\nPlease choose a different date/time or cancel the existing booking first." << endl;
            return;
        }
    }

    // Display and select venue
    displayAvailableVenues(data.venues, newBooking.eventDate, newBooking.eventTime);

    int venueIndex = selectAvailableVenue(data.venues, newBooking.eventDate, newBooking.eventTime);

    if (venueIndex == -1) {
        cout << "No venues available for the selected date and time." << endl;
        return;
    }

    newBooking.venue = data.venues[venueIndex];

    // Verify venue is truly available across all bookings
    if (!isVenueAvailable(data.bookings, newBooking.venue.venueID,
        newBooking.eventDate, newBooking.eventTime)) {
        cout << "\nERROR: Selected venue is already booked by another user!" << endl;
        cout << "Please choose a different venue or date/time." << endl;
        return;
    }

    newBooking.finalCost = newBooking.venue.rentalCost;
    newBooking.bookingStatus = "Pending";

    // Check capacity
    if (newBooking.eventReg.expectedGuests > newBooking.venue.capacity) {
        cout << "\nWARNING: Expected guests (" << newBooking.eventReg.expectedGuests
            << ") exceeds venue capacity (" << newBooking.venue.capacity << ")!" << endl;

        vector<char> validChars = { 'Y', 'N' };
        char choice = getValidCharInput("Continue anyway? (Y/N): ", validChars);

        if (choice != 'Y' && choice != 'y') {
            cout << "Booking cancelled." << endl;
            return;
        }
    }

    // LOGISTICS SELECTION
    vector<string> selectedLogistics;
    double logisticsCost = selectLogistics(selectedLogistics);
    newBooking.finalCost += logisticsCost;

    // Add booking to the venue's schedule
    TimeSlot newSlot;
    newSlot.date = newBooking.eventDate;
    newSlot.time = newBooking.eventTime;
    newSlot.eventID = newBooking.bookingID;
    newSlot.isBooked = true;
    data.venues[venueIndex].bookingSchedule.push_back(newSlot);

    selectedReg->eventStatus = "SCHEDULED";

    data.bookings.push_back(newBooking);

    cout << "\n=== BOOKING CREATED SUCCESSFULLY ===" << endl;
    cout << "Booking ID: " << newBooking.bookingID << endl;
    cout << "Event: " << newBooking.eventReg.eventTitle << " by " << newBooking.eventReg.manufacturer << endl;
    cout << "Date & Time: " << newBooking.eventDate.toString() << " (" << newBooking.eventTime << ")" << endl;
    cout << "Venue: " << newBooking.venue.venueName << endl;
    cout << "Expected Guests: " << newBooking.eventReg.expectedGuests << endl;

    cout << "\n=== COST BREAKDOWN ===" << endl;
    cout << "Venue Rental: RM " << fixed << setprecision(2) << newBooking.venue.rentalCost << endl;
    if (logisticsCost > 0) {
        cout << "Logistics & Services: RM " << fixed << setprecision(2) << logisticsCost << endl;
        cout << "\nSelected Logistics:" << endl;
        for (size_t i = 0; i < selectedLogistics.size(); i++) {
            cout << "  " << (i + 1) << ". " << selectedLogistics[i] << endl;
        }
    }
    cout << "------------------------" << endl;
    cout << "TOTAL COST: RM " << fixed << setprecision(2) << newBooking.finalCost << endl;
    cout << "Status: " << newBooking.bookingStatus << endl;

	saveRegistrationsToFile(data.registrations);
    saveBookingsToFile(data.bookings);
}

void viewEventBookings(const SystemData& data) {
    clearScreen();
    cout << "=== MY EVENT BOOKINGS ===" << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    if (data.bookings.empty()) {
        cout << "No event bookings found." << endl;
        return;
    }

    // Filter bookings for current user
    vector<EventBooking> userBookings;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);
    for (const auto& booking : data.bookings) {
        string normalizedBookingUser = normalizeUserID(booking.eventReg.organizer.userID);

        if (normalizedBookingUser == normalizedCurrentUser) {
            userBookings.push_back(booking);
        }
    }

    if (userBookings.empty()) {
        cout << "No event bookings found for your account." << endl;
        return;
    }

    cout << setfill('=') << setw(140) << "=" << setfill(' ') << endl;
    cout << left << setw(8) << "Book ID"
        << setw(9) << "Event ID"
        << setw(15) << "Event Title"
        << setw(12) << "Manufacturer"
        << setw(12) << "Date"
        << setw(15) << "Time"
        << setw(8) << "Guests"
        << setw(15) << "Venue"
        << setw(15) << "Cost (RM)"
        << setw(15) << "Status" << endl;
    cout << setfill('=') << setw(140) << "=" << setfill(' ') << endl;

    for (const auto& booking : userBookings) {
        cout << left << setw(8) << booking.bookingID
            << setw(9) << booking.eventReg.eventID
            << setw(15) << booking.eventReg.eventTitle.substr(0, 14)
            << setw(12) << booking.eventReg.manufacturer.substr(0, 11)
            << setw(12) << booking.eventDate.toString()
            << setw(15) << booking.eventTime
            << setw(8) << booking.eventReg.expectedGuests
            << setw(15) << booking.venue.venueName.substr(0, 14)
            << "RM " << setw(12) << fixed << setprecision(2) << booking.finalCost
            << setw(15) << booking.bookingStatus << endl;
    }
    cout << setfill('=') << setw(140) << "=" << setfill(' ') << endl;
    cout << "Total Your Bookings: " << userBookings.size() << endl;

    // Show notice for cancelled bookings
    bool hasCancelledBookings = false;
    for (const auto& booking : userBookings) {
        if (booking.bookingStatus == "Cancelled") {
            if (!hasCancelledBookings) {
                cout << "\nCANCELLED EVENTS NOTICE:" << endl;
                cout << "========================" << endl;
                hasCancelledBookings = true;
            }
            cout << "• " << booking.bookingID << " (" << booking.eventReg.eventTitle
                << ") - Event registration was cancelled" << endl;
        }
    }

    if (hasCancelledBookings) {
        cout << "\nNote: You can delete these cancelled bookings to clean up your history." << endl;
        cout << "Use option 4 (Cancel Event Booking) to remove them." << endl;
    }
}

void updateEventBooking(SystemData& data) {
    clearScreen();
    cout << "=== UPDATE EVENT BOOKING ===" << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    if (data.bookings.empty()) {
        cout << "No event bookings to update." << endl;
        return;
    }

    // Check if user has any bookings
    bool hasBookings = false;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);
    for (const auto& booking : data.bookings) {
        string normalizedBookingUser = normalizeUserID(booking.eventReg.organizer.userID);
        if (normalizedBookingUser == normalizedCurrentUser) {
            hasBookings = true;
            break;
        }
    }

    if (!hasBookings) {
        cout << "You have no event bookings to update." << endl;
        return;
    }

    viewEventBookings(data);

    string bookingID = getValidStringInput("\nEnter Booking ID to update: ");
    bookingID = toUpperCase(bookingID);

    // Find booking using loop and verify ownership
    int bookingIndex = -1;
    for (size_t i = 0; i < data.bookings.size(); i++) {
        string normalizedBookingUser = normalizeUserID(data.bookings[i].eventReg.organizer.userID);
        if (data.bookings[i].bookingID == bookingID &&
            normalizedBookingUser == normalizedCurrentUser) {
            bookingIndex = static_cast<int>(i);
            break;
        }
    }

    if (bookingIndex == -1) {
        cout << "Event booking not found or you don't have permission to update it!" << endl;
        return;
    }

    // Check if booking can be updated (not completed or cancelled)
    if (data.bookings[bookingIndex].bookingStatus == "Completed" ||
        data.bookings[bookingIndex].bookingStatus == "Cancelled") {
        cout << "Cannot update booking with status: " << data.bookings[bookingIndex].bookingStatus << endl;
        return;
    }

    cout << "\n=== Current Booking Details ===" << endl;
    cout << "Booking ID: " << data.bookings[bookingIndex].bookingID << endl;
    cout << "Event: " << data.bookings[bookingIndex].eventReg.eventTitle
        << " by " << data.bookings[bookingIndex].eventReg.manufacturer << endl;
    cout << "Date: " << data.bookings[bookingIndex].eventDate.toString() << endl;
    cout << "Time: " << data.bookings[bookingIndex].eventTime << endl;
    cout << "Venue: " << data.bookings[bookingIndex].venue.venueName << endl;
    cout << "Expected Guests: " << data.bookings[bookingIndex].eventReg.expectedGuests << endl;
    cout << "Final Cost: RM " << fixed << setprecision(2) << data.bookings[bookingIndex].finalCost << endl;
    cout << "Status: " << data.bookings[bookingIndex].bookingStatus << endl;

    cout << "\nWhat would you like to update?" << endl;
    cout << "1. Time Slot" << endl;
    cout << "2. Venue" << endl;
    cout << "3. Add/Update Logistics" << endl;

    int choice = getValidIntegerInput("Enter choice [1-3]: ", 1, 3);

    switch (choice) {
    case 1: // Update Time Slot
    {
        cout << "\n=== UPDATE TIME SLOT ===" << endl;
        cout << "Current time slot: " << data.bookings[bookingIndex].eventTime << endl;

        // Show available time slots
        cout << "\nAvailable Time Slots:" << endl;
        for (size_t i = 0; i < data.timeConfig.timeSlotNames.size(); i++) {
            cout << (i + 1) << ". " << data.timeConfig.timeSlotNames[i] << endl;
        }

        int timeChoice = getValidIntegerInput("Select new time slot [1-" +
            to_string(data.timeConfig.timeSlotNames.size()) + "]: ", 1,
            static_cast<int>(data.timeConfig.timeSlotNames.size()));

        string newTimeSlot = data.timeConfig.timeSlots[timeChoice - 1];

        // Check if the new time slot is different
        if (newTimeSlot == data.bookings[bookingIndex].eventTime) {
            cout << "Selected time slot is the same as current. No changes made." << endl;
            break;
        }

        // Check if venue is available at the new time slot
        if (!isVenueAvailableInSchedule(data.bookings[bookingIndex].venue,
            data.bookings[bookingIndex].eventDate,
            newTimeSlot)) {
            cout << "ERROR: Current venue is not available at the selected time slot!" << endl;
            cout << "Please try a different time or update the venue as well." << endl;
            break;
        }

        // Check for duplicate booking with new time
        bool duplicateFound = false;
        for (const auto& existingBooking : data.bookings) {
            if (existingBooking.bookingID != bookingID && // Don't check against itself
                existingBooking.eventReg.eventID == data.bookings[bookingIndex].eventReg.eventID &&
                existingBooking.eventDate.day == data.bookings[bookingIndex].eventDate.day &&
                existingBooking.eventDate.month == data.bookings[bookingIndex].eventDate.month &&
                existingBooking.eventDate.year == data.bookings[bookingIndex].eventDate.year &&
                existingBooking.eventTime == newTimeSlot &&
                existingBooking.bookingStatus != "Cancelled") {

                duplicateFound = true;
                break;
            }
        }

        if (duplicateFound) {
            cout << "ERROR: This event already has a booking at the selected time slot!" << endl;
            break;
        }

        // Update venue schedule - remove old slot
        for (size_t i = 0; i < data.venues.size(); i++) {
            if (data.venues[i].venueID == data.bookings[bookingIndex].venue.venueID) {
                for (size_t j = 0; j < data.venues[i].bookingSchedule.size(); j++) {
                    if (data.venues[i].bookingSchedule[j].eventID == bookingID) {
                        data.venues[i].bookingSchedule.erase(data.venues[i].bookingSchedule.begin() + j);
                        break;
                    }
                }

                // Add new slot
                TimeSlot newSlot;
                newSlot.date = data.bookings[bookingIndex].eventDate;
                newSlot.time = newTimeSlot;
                newSlot.eventID = bookingID;
                newSlot.isBooked = true;
                data.venues[i].bookingSchedule.push_back(newSlot);
                break;
            }
        }

        data.bookings[bookingIndex].eventTime = newTimeSlot;
        cout << "Time slot updated successfully to: " << newTimeSlot << endl;
        break;
    }
    case 2: // Update Venue
    {
        cout << "\n=== UPDATE VENUE ===" << endl;
        cout << "Current venue: " << data.bookings[bookingIndex].venue.venueName
            << " (Cost: RM " << fixed << setprecision(2)
            << data.bookings[bookingIndex].venue.rentalCost << ")" << endl;

        // Display available venues for current date and time
        displayAvailableVenues(data.venues, data.bookings[bookingIndex].eventDate,
            data.bookings[bookingIndex].eventTime);

        int venueIndex = selectAvailableVenue(data.venues, data.bookings[bookingIndex].eventDate,
            data.bookings[bookingIndex].eventTime);

        if (venueIndex == -1) {
            cout << "No alternative venues available for the current date and time." << endl;
            break;
        }

        // Check if it's the same venue
        if (data.venues[venueIndex].venueID == data.bookings[bookingIndex].venue.venueID) {
            cout << "Selected venue is the same as current. No changes made." << endl;
            break;
        }

        // Check capacity
        if (data.bookings[bookingIndex].eventReg.expectedGuests > data.venues[venueIndex].capacity) {
            cout << "\nWARNING: Expected guests (" << data.bookings[bookingIndex].eventReg.expectedGuests
                << ") exceeds new venue capacity (" << data.venues[venueIndex].capacity << ")!" << endl;

            vector<char> validChars = { 'Y', 'N' };
            char choice = getValidCharInput("Continue with venue change? (Y/N): ", validChars);

            if (choice != 'Y' && choice != 'y') {
                cout << "Venue change cancelled." << endl;
                break;
            }
        }

        // Remove old venue booking from schedule
        for (size_t i = 0; i < data.venues.size(); i++) {
            if (data.venues[i].venueID == data.bookings[bookingIndex].venue.venueID) {
                for (size_t j = 0; j < data.venues[i].bookingSchedule.size(); j++) {
                    if (data.venues[i].bookingSchedule[j].eventID == bookingID) {
                        data.venues[i].bookingSchedule.erase(data.venues[i].bookingSchedule.begin() + j);
                        break;
                    }
                }
                break;
            }
        }

        // Calculate cost difference
        double oldVenueCost = data.bookings[bookingIndex].venue.rentalCost;
        double newVenueCost = data.venues[venueIndex].rentalCost;
        double costDifference = newVenueCost - oldVenueCost;

        // Update venue and recalculate final cost
        data.bookings[bookingIndex].venue = data.venues[venueIndex];
        data.bookings[bookingIndex].finalCost += costDifference;

        // Add booking to new venue schedule
        TimeSlot newSlot;
        newSlot.date = data.bookings[bookingIndex].eventDate;
        newSlot.time = data.bookings[bookingIndex].eventTime;
        newSlot.eventID = bookingID;
        newSlot.isBooked = true;
        data.venues[venueIndex].bookingSchedule.push_back(newSlot);

        cout << "\n=== Venue Updated Successfully ===" << endl;
        cout << "New venue: " << data.venues[venueIndex].venueName << endl;
        cout << "Old venue cost: RM " << fixed << setprecision(2) << oldVenueCost << endl;
        cout << "New venue cost: RM " << fixed << setprecision(2) << newVenueCost << endl;
        cout << "Cost difference: RM " << fixed << setprecision(2) << costDifference << endl;
        cout << "Updated total cost: RM " << fixed << setprecision(2) << data.bookings[bookingIndex].finalCost << endl;
        break;
    }
    case 3: // Add/Update Logistics
    {
        cout << "\n=== UPDATE LOGISTICS ===" << endl;
        vector<string> selectedLogistics;
        double additionalLogisticsCost = selectLogistics(selectedLogistics);

        if (additionalLogisticsCost > 0) {
            data.bookings[bookingIndex].finalCost += additionalLogisticsCost;
            cout << "\nAdditional logistics cost: RM " << fixed << setprecision(2)
                << additionalLogisticsCost << endl;
            cout << "Updated total cost: RM " << fixed << setprecision(2)
                << data.bookings[bookingIndex].finalCost << endl;
        }
        break;
    }
    }

    saveBookingsToFile(data.bookings);
    cout << "Event booking updated successfully!" << endl;
}

void cancelEventBooking(SystemData& data) {
    clearScreen();
    cout << "=== CANCEL/DELETE EVENT BOOKING ===" << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    if (data.bookings.empty()) {
        cout << "No event bookings to cancel." << endl;
        return;
    }

    // Check if user has any bookings
    bool hasBookings = false;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);
    for (const auto& booking : data.bookings) {
        string normalizedBookingUser = normalizeUserID(booking.eventReg.organizer.userID);
        if (booking.bookingStatus != "Cancelled" && normalizedBookingUser == normalizedCurrentUser) {
            hasBookings = true;
            break;
        }
    }

    if (!hasBookings) {
        cout << "You have no event bookings to cancel." << endl;
        return;
    }

    viewEventBookings(data);

    string bookingID = getValidStringInput("\nEnter Booking ID to cancel/delete: ");
    bookingID = toUpperCase(bookingID);

    // Find booking using loop and verify ownership
    int bookingIndex = -1;
    for (size_t i = 0; i < data.bookings.size(); i++) {
        string normalizedBookingUser = normalizeUserID(data.bookings[i].eventReg.organizer.userID);
        if (data.bookings[i].bookingID == bookingID &&
            normalizedBookingUser == normalizedCurrentUser) {
            bookingIndex = static_cast<int>(i);
            break;
        }
    }

    if (bookingIndex == -1) {
        cout << "Event booking not found or you don't have permission to cancel it!" << endl;
        return;
    }

    cout << "\n=== Booking Details ===" << endl;
    cout << "Booking ID: " << data.bookings[bookingIndex].bookingID << endl;
    cout << "Event: " << data.bookings[bookingIndex].eventReg.eventTitle
        << " by " << data.bookings[bookingIndex].eventReg.manufacturer << endl;
    cout << "Date: " << data.bookings[bookingIndex].eventDate.toString() << endl;
    cout << "Time: " << data.bookings[bookingIndex].eventTime << endl;
    cout << "Venue: " << data.bookings[bookingIndex].venue.venueName << endl;
    cout << "Final Cost: RM " << fixed << setprecision(2) << data.bookings[bookingIndex].finalCost << endl;
    cout << "Current Status: " << data.bookings[bookingIndex].bookingStatus << endl;

    // Different handling based on current status
    if (data.bookings[bookingIndex].bookingStatus == "Cancelled") {
        cout << "\nThis booking is already cancelled." << endl;
        cout << "You can delete it to remove from your records." << endl;

        vector<char> validChars = { 'Y', 'N' };
        char confirm = getValidCharInput("Delete this cancelled booking record? (Y/N): ", validChars);

        if (confirm == 'Y' || confirm == 'y') {
            // Remove from venue schedule
            removeBookingFromVenueSchedule(data.venues, bookingID);

            // Remove booking
            data.bookings.erase(data.bookings.begin() + bookingIndex);
            saveBookingsToFile(data.bookings);
            cout << "Cancelled booking record deleted successfully!" << endl;
        }
        else {
            cout << "Deletion cancelled." << endl;
        }
    }
    else {
        // Normal cancellation
        vector<char> validChars = { 'Y', 'N' };
        char confirm = getValidCharInput("\nAre you sure you want to cancel this booking? (Y/N): ", validChars);

        if (confirm == 'Y' || confirm == 'y') {
            // Remove booking from venue schedule
            removeBookingFromVenueSchedule(data.venues, bookingID);
            for (auto& reg : data.registrations) {
                if (reg.eventID == data.bookings[bookingIndex].eventReg.eventID && normalizedCurrentUser == data.bookings[bookingIndex].eventReg.organizer.userID) {
                    reg.eventStatus = "UNSCHEDULED"; //change the event status
                    break;
                }
            }

            // Delete the booking completely
            data.bookings.erase(data.bookings.begin() + bookingIndex);
			saveRegistrationsToFile(data.registrations);
            saveBookingsToFile(data.bookings);
            cout << "Event booking cancelled successfully!" << endl;
        }
        else {
            cout << "Cancellation cancelled." << endl;
        }
    }
}

void removeBookingFromVenueSchedule(vector<Venue>& venues, const string& bookingID) {
    for (size_t i = 0; i < venues.size(); i++) {
        for (size_t j = 0; j < venues[i].bookingSchedule.size(); j++) {
            if (venues[i].bookingSchedule[j].eventID == bookingID) {
                venues[i].bookingSchedule.erase(venues[i].bookingSchedule.begin() + j);
                return; // Exit after finding and removing
            }
        }
    }
}

void updateBookingsForCancelledEvent(vector<EventBooking>& bookings, const string& eventID) {
    int updatedCount = 0;
    for (auto& booking : bookings) {
        if (booking.eventReg.eventID == eventID && booking.bookingStatus != "Cancelled") {
            booking.bookingStatus = "Cancelled";
            updatedCount++;
        }
    }

    if (updatedCount > 0) {
        saveBookingsToFile(bookings);
        cout << "Updated " << updatedCount << " booking(s) to cancelled status." << endl;
    }
}