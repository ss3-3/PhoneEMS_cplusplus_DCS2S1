#include "event_monitoring.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <regex>
#include <map> 
#include <utility>
#include <format>  // C++20 format library
#include "date.h"
#include "venue.h"
#include "event_registration.h"
#include "event_booking.h"
#include "system_data.h"
#include "timeslot.h"
#include "product.h"
#include "utility_fun.h"
#include "input_validation.h"
#include "file_operation.h"
using namespace std;

// Event Monitoring Module Implementation with User Restrictions and <format>
void eventMonitoringMenu(SystemData& data) {
    bool exitMenu = false;

    while (!exitMenu) {
        clearScreen();
        displayLogo();
        cout << "=== EVENT MONITORING & REPORTING ===" << endl;
        cout << string(50, '=') << endl;
        cout << "1. My Event Summary Dashboard" << endl;
        cout << "2. My Venue Utilization Report" << endl;
        cout << "3. My Financial Report" << endl;
        cout << "4. My Upcoming Events" << endl;
        cout << "5. My Registration Statistics" << endl;
        cout << "6. Search My Events" << endl;
        cout << "7. Generate My Event Report" << endl;
        cout << "8. Back to Main Menu" << endl;
        cout << string(50, '=') << endl;

        int choice = getValidIntegerInput("Enter your choice [1-8]: ", 1, 8);

        switch (choice) {
        case 1:
            displayEventSummary(data);
            break;
        case 2:
            displayVenueUtilization(data);
            break;
        case 3:
            displayFinancialReport(data);
            break;
        case 4:
            displayUpcomingEvents(data);
            break;
        case 5:
            displayRegistrationStatistics(data);
            break;
        case 6:
            searchEvents(data);
            break;
        case 7:
            generateEventReport(data);
            break;
        case 8:
            exitMenu = true;
            break;
        }

        if (choice != 8) {
            pauseScreen();
        }
    }
}

void displayEventSummary(const SystemData& data) {
    clearScreen();
    cout << "=== MY EVENT SUMMARY DASHBOARD ===" << endl;
    cout << string(60, '=') << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    // Filter user's data only
    string normalizedCurrentUser = normalizeUserID(data.currentUser);
    vector<EventRegistration> userRegistrations;
    vector<EventBooking> userBookings;

    for (const auto& reg : data.registrations) {
        string normalizedRegUser = normalizeUserID(reg.organizer.userID);
        if (normalizedRegUser == normalizedCurrentUser) {
            userRegistrations.push_back(reg);
        }
    }

    for (const auto& booking : data.bookings) {
        string normalizedBookingUser = normalizeUserID(booking.eventReg.organizer.userID);
        if (normalizedBookingUser == normalizedCurrentUser) {
            userBookings.push_back(booking);
        }
    }

    int totalRegistrations = userRegistrations.size();
    int totalBookings = userBookings.size();
    int totalVenues = data.venues.size(); // All venues are available for viewing

    // Count different statuses for user's events only
    int registeredCount = 0, approvedCount = 0, rejectedCount = 0;
    int pendingCount = 0, confirmedCount = 0, completedCount = 0, cancelledCount = 0;

    for (const auto& reg : userRegistrations) {
        if (reg.eventStatus == "Registered") registeredCount++;
        else if (reg.eventStatus == "Approved") approvedCount++;
        else if (reg.eventStatus == "Rejected") rejectedCount++;
    }

    for (const auto& booking : userBookings) {
        if (booking.bookingStatus == "Pending") pendingCount++;
        else if (booking.bookingStatus == "Confirmed") confirmedCount++;
        else if (booking.bookingStatus == "Completed") completedCount++;
        else if (booking.bookingStatus == "Cancelled") cancelledCount++;
    }

    cout << format("User: {}\n", data.currentUser);
    cout << string(60, '-') << endl;
    cout << format("{:<34}: {}\n", "My Total Registrations", totalRegistrations);
    cout << format("{:<34}: {}\n", "My Total Bookings", totalBookings);
    cout << format("{:<34}: {}\n", "Available Venues", totalVenues);
    cout << endl;

    cout << "=== MY REGISTRATION STATUS ===" << endl;
    cout << format("{:<19}: {}\n", "Registered", registeredCount);
    cout << format("{:<19}: {}\n", "Approved", approvedCount);
    cout << format("{:<19}: {}\n", "Rejected", rejectedCount);
    cout << endl;

    cout << "=== MY BOOKING STATUS ===" << endl;
    cout << format("{:<19}: {}\n", "Pending", pendingCount);
    cout << format("{:<19}: {}\n", "Confirmed", confirmedCount);
    cout << format("{:<19}: {}\n", "Completed", completedCount);
    cout << format("{:<19}: {}\n", "Cancelled", cancelledCount);
    cout << string(60, '=') << endl;

    if (totalRegistrations == 0 && totalBookings == 0) {
        cout << "\nNo events found for your account." << endl;
        cout << "Please create event registrations and bookings first." << endl;
    }
}

void displayVenueUtilization(const SystemData& data) {
    clearScreen();
    cout << "=== MY VENUE UTILIZATION REPORT ===" << endl;
    cout << string(80, '=') << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    string normalizedCurrentUser = normalizeUserID(data.currentUser);
    map<string, int> userVenueBookings; // Track user's bookings per venue

    // Count user's bookings per venue
    for (const auto& booking : data.bookings) {
        string normalizedBookingUser = normalizeUserID(booking.eventReg.organizer.userID);
        if (normalizedBookingUser == normalizedCurrentUser) {
            userVenueBookings[booking.venue.venueID]++;
        }
    }

    cout << format("User: {}\n", data.currentUser);
    cout << string(80, '-') << endl;
    cout << format("{:<15}{:<20}{:<10}{:<15}{:<20}\n",
        "Venue ID", "Venue Name", "Capacity", "My Bookings", "My Usage Rate");
    cout << string(80, '-') << endl;

    bool hasBookings = false;
    for (const auto& venue : data.venues) {
        int myBookingCount = userVenueBookings[venue.venueID];
        if (myBookingCount > 0) {
            hasBookings = true;
            double myUsageRate = (static_cast<double>(myBookingCount) / 10.0) * 100.0; // Relative to personal usage

            cout << format("{:<15}{:<20}{:<10}{:<15}{:<7.2f}%\n",
                venue.venueID,
                venue.venueName,
                venue.capacity,
                myBookingCount,
                myUsageRate);
        }
    }

    if (!hasBookings) {
        cout << "No venue bookings found for your account." << endl;
    }

    cout << string(80, '=') << endl;
}

void displayFinancialReport(const SystemData& data) {
    clearScreen();
    cout << "=== MY FINANCIAL REPORT ===" << endl;
    cout << string(70, '=') << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    string normalizedCurrentUser = normalizeUserID(data.currentUser);
    double myTotalSpent = 0.0;
    double myTotalBudget = 0.0;
    double myPendingCost = 0.0;
    double myConfirmedCost = 0.0;
    double myCompletedCost = 0.0;

    vector<EventBooking> myExpensiveEvents;

    // Calculate user's financial data
    for (const auto& booking : data.bookings) {
        string normalizedBookingUser = normalizeUserID(booking.eventReg.organizer.userID);
        if (normalizedBookingUser == normalizedCurrentUser) {
            myExpensiveEvents.push_back(booking);

            if (booking.bookingStatus == "Completed") {
                myCompletedCost += booking.finalCost;
                myTotalSpent += booking.finalCost;
            }
            else if (booking.bookingStatus == "Confirmed") {
                myConfirmedCost += booking.finalCost;
                myTotalSpent += booking.finalCost;
            }
            else if (booking.bookingStatus == "Pending") {
                myPendingCost += booking.finalCost;
            }
        }
    }

    for (const auto& reg : data.registrations) {
        string normalizedRegUser = normalizeUserID(reg.organizer.userID);
        if (normalizedRegUser == normalizedCurrentUser) {
            myTotalBudget += reg.estimatedBudget;
        }
    }

    cout << format("User: {}\n", data.currentUser);
    cout << string(70, '-') << endl;
    cout << format("{:<34}: RM {:.2f}\n", "My Total Budget (All Events)", myTotalBudget);
    cout << format("{:<34}: RM {:.2f}\n", "My Total Spent (Bookings)", myTotalSpent);
    cout << format("{:<34}: RM {:.2f}\n", "My Pending Costs", myPendingCost);
    cout << format("{:<34}: RM {:.2f}\n", "My Confirmed Costs", myConfirmedCost);
    cout << format("{:<34}: RM {:.2f}\n", "My Completed Costs", myCompletedCost);
    cout << endl;

    // User's top 5 most expensive events
    cout << "=== MY TOP 5 MOST EXPENSIVE EVENTS ===" << endl;
    sort(myExpensiveEvents.begin(), myExpensiveEvents.end(),
        [](const EventBooking& a, const EventBooking& b) {
            return a.finalCost > b.finalCost;
        });

    int count = 0;
    for (const auto& booking : myExpensiveEvents) {
        if (count++ >= 5) break;
        cout << format("{:<29}: RM {:.2f} ({})\n",
            booking.eventReg.eventTitle,
            booking.finalCost,
            booking.bookingStatus);
    }

    if (myExpensiveEvents.empty()) {
        cout << "No bookings found for your account." << endl;
    }

    cout << string(70, '=') << endl;
}

void displayUpcomingEvents(const SystemData& data) {
    clearScreen();
    cout << "=== MY UPCOMING EVENTS (Next 30 days) ===" << endl;
    cout << string(100, '=') << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    Date currentDate = getCurrentDate();
    int myUpcomingCount = 0;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);

    cout << format("User: {}\n", data.currentUser);
    cout << string(100, '-') << endl;
    cout << format("{:<12}{:<15}{:<20}{:<15}{:<10}{:<15}\n",
        "Date", "Time", "Event Title", "Venue", "Guests", "Status");
    cout << string(100, '-') << endl;

    for (const auto& booking : data.bookings) {
        string normalizedBookingUser = normalizeUserID(booking.eventReg.organizer.userID);
        if (normalizedBookingUser == normalizedCurrentUser) {
            // Simple date comparison (for demonstration)
            if (booking.eventDate.year >= currentDate.year &&
                booking.eventDate.month >= currentDate.month &&
                booking.eventDate.day >= currentDate.day &&
                booking.bookingStatus != "Cancelled") {

                cout << format("{:<12}{:<15}{:<20}{:<15}{:<10}{:<15}\n",
                    booking.eventDate.toString(),
                    booking.eventTime,
                    booking.eventReg.eventTitle.substr(0, 19),
                    booking.venue.venueName.substr(0, 14),
                    booking.eventReg.expectedGuests,
                    booking.bookingStatus);

                myUpcomingCount++;
            }
        }
    }

    if (myUpcomingCount == 0) {
        cout << "No upcoming events found for your account in the next 30 days." << endl;
    }

    cout << string(100, '=') << endl;
    cout << format("My Total Upcoming Events: {}\n", myUpcomingCount);
}

void displayRegistrationStatistics(const SystemData& data) {
    clearScreen();
    cout << "=== MY REGISTRATION STATISTICS ===" << endl;
    cout << string(60, '=') << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    string normalizedCurrentUser = normalizeUserID(data.currentUser);
    map<string, int> myManufacturerStats;
    int myTotalGuests = 0;
    int myTotalProducts = 0;
    vector<EventRegistration> myRegistrations;

    for (const auto& reg : data.registrations) {
        string normalizedRegUser = normalizeUserID(reg.organizer.userID);
        if (normalizedRegUser == normalizedCurrentUser) {
            myRegistrations.push_back(reg);
            myManufacturerStats[reg.manufacturer]++;
            myTotalGuests += reg.expectedGuests;
            myTotalProducts += reg.productQuantity;
        }
    }

    cout << format("User: {}\n", data.currentUser);
    cout << string(60, '-') << endl;
    cout << format("{:<34}: {}\n", "My Total Expected Guests", myTotalGuests);
    cout << format("{:<34}: {}\n", "My Total Products to Launch", myTotalProducts);
    cout << format("{:<34}: {}\n", "My Total Registrations", myRegistrations.size());
    cout << endl;

    if (!myManufacturerStats.empty()) {
        cout << "=== MY MANUFACTURER DISTRIBUTION ===" << endl;
        for (const auto& stat : myManufacturerStats) {
            cout << format("{:<19}: {} events\n", stat.first, stat.second);
        }
    }

    if (myRegistrations.empty()) {
        cout << "No registrations found for your account." << endl;
        cout << "Please create event registrations first." << endl;
    }

    cout << string(60, '=') << endl;
}

void searchEvents(const SystemData& data) {
    clearScreen();
    cout << "=== SEARCH MY EVENTS ===" << endl;
    cout << string(50, '=') << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    string searchTerm = getValidStringInput("Enter search term (event title, manufacturer, or organizer): ");
    string upperSearchTerm = toUpperCase(searchTerm);
    string normalizedCurrentUser = normalizeUserID(data.currentUser);

    vector<EventRegistration> foundRegistrations;
    vector<EventBooking> foundBookings;

    // Search in user's registrations only
    for (const auto& reg : data.registrations) {
        string normalizedRegUser = normalizeUserID(reg.organizer.userID);
        if (normalizedRegUser == normalizedCurrentUser) {
            if (toUpperCase(reg.eventTitle).find(upperSearchTerm) != string::npos ||
                toUpperCase(reg.manufacturer).find(upperSearchTerm) != string::npos ||
                toUpperCase(reg.organizer.organizerName).find(upperSearchTerm) != string::npos) {
                foundRegistrations.push_back(reg);
            }
        }
    }

    // Search in user's bookings only
    for (const auto& booking : data.bookings) {
        string normalizedBookingUser = normalizeUserID(booking.eventReg.organizer.userID);
        if (normalizedBookingUser == normalizedCurrentUser) {
            if (toUpperCase(booking.eventReg.eventTitle).find(upperSearchTerm) != string::npos ||
                toUpperCase(booking.eventReg.manufacturer).find(upperSearchTerm) != string::npos ||
                toUpperCase(booking.eventReg.organizer.organizerName).find(upperSearchTerm) != string::npos) {
                foundBookings.push_back(booking);
            }
        }
    }

    cout << "\n=== MY SEARCH RESULTS ===" << endl;
    cout << format("Found {} of my registrations and {} of my bookings matching '{}'\n",
        foundRegistrations.size(), foundBookings.size(), searchTerm);

    if (!foundRegistrations.empty()) {
        cout << "\n=== MY MATCHING REGISTRATIONS ===" << endl;
        for (const auto& reg : foundRegistrations) {
            cout << format("ID: {} | {} by {} | Status: {}\n",
                reg.eventID, reg.eventTitle, reg.manufacturer, reg.eventStatus);
        }
    }

    if (!foundBookings.empty()) {
        cout << "\n=== MY MATCHING BOOKINGS ===" << endl;
        for (const auto& booking : foundBookings) {
            cout << format("Booking ID: {} | {} on {} at {} | Status: {}\n",
                booking.bookingID,
                booking.eventReg.eventTitle,
                booking.eventDate.toString(),
                booking.venue.venueName,
                booking.bookingStatus);
        }
    }

    if (foundRegistrations.empty() && foundBookings.empty()) {
        cout << "No events found in your account matching the search term." << endl;
    }

    cout << string(50, '=') << endl;
}

void generateEventReport(const SystemData& data) {
    clearScreen();
    cout << "=== GENERATE MY EVENT REPORT ===" << endl;
    cout << string(50, '=') << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    string normalizedCurrentUser = normalizeUserID(data.currentUser);
    vector<EventRegistration> myRegistrations;
    vector<EventBooking> myBookings;

    // Filter user's data
    for (const auto& reg : data.registrations) {
        string normalizedRegUser = normalizeUserID(reg.organizer.userID);
        if (normalizedRegUser == normalizedCurrentUser) {
            myRegistrations.push_back(reg);
        }
    }

    for (const auto& booking : data.bookings) {
        string normalizedBookingUser = normalizeUserID(booking.eventReg.organizer.userID);
        if (normalizedBookingUser == normalizedCurrentUser) {
            myBookings.push_back(booking);
        }
    }

    if (myRegistrations.empty() && myBookings.empty()) {
        cout << "No events found for your account to generate report!" << endl;
        return;
    }

    string filename = format("my_event_report_{}_{}{:02}{:02}.txt",
        data.currentUser,
        getCurrentDate().year,
        getCurrentDate().month,
        getCurrentDate().day);

    ofstream reportFile(filename);

    if (!reportFile.is_open()) {
        cout << "Error: Could not create report file!" << endl;
        return;
    }

    // Generate personalized report using format
    reportFile << "PERSONAL EVENT MANAGEMENT REPORT" << endl;
    reportFile << format("User: {}\n", data.currentUser);
    reportFile << format("Generated on: {}\n", getCurrentDate().toString());
    reportFile << string(50, '=') << endl << endl;

    // Personal summary section
    reportFile << "MY SUMMARY" << endl;
    reportFile << string(30, '-') << endl;
    reportFile << format("My Total Registrations: {}\n", myRegistrations.size());
    reportFile << format("My Total Bookings: {}\n", myBookings.size()) << endl;

    // Personal detailed registrations
    if (!myRegistrations.empty()) {
        reportFile << "MY DETAILED REGISTRATIONS" << endl;
        reportFile << string(30, '-') << endl;
        for (const auto& reg : myRegistrations) {
            reportFile << format("Event ID: {}\n", reg.eventID);
            reportFile << format("Title: {}\n", reg.eventTitle);
            reportFile << format("Manufacturer: {}\n", reg.manufacturer);
            reportFile << format("Organizer: {}\n", reg.organizer.organizerName);
            reportFile << format("Status: {}\n", reg.eventStatus);
            reportFile << format("Expected Guests: {}\n", reg.expectedGuests);
            reportFile << format("Budget: RM {:.2f}\n", reg.estimatedBudget);
            reportFile << format("Products: {}\n", reg.productQuantity);
            for (const auto& product : reg.phoneInfo) {
                reportFile << format("  - {} ({}) RM {:.2f}\n",
                    product.productName,
                    product.productModel,
                    product.productPrice);
            }
            reportFile << endl;
        }
    }

    // Personal detailed bookings
    if (!myBookings.empty()) {
        reportFile << "MY DETAILED BOOKINGS" << endl;
        reportFile << string(30, '-') << endl;
        for (const auto& booking : myBookings) {
            reportFile << format("Booking ID: {}\n", booking.bookingID);
            reportFile << format("Event: {}\n", booking.eventReg.eventTitle);
            reportFile << format("Date: {}\n", booking.eventDate.toString());
            reportFile << format("Time: {}\n", booking.eventTime);
            reportFile << format("Venue: {}\n", booking.venue.venueName);
            reportFile << format("Status: {}\n", booking.bookingStatus);
            reportFile << format("Cost: RM {:.2f}\n", booking.finalCost);
            reportFile << endl;
        }
    }

    reportFile.close();
    cout << format("Personal report generated successfully: {}\n", filename);
    cout << format("Your events reported: {} registrations, {} bookings\n",
        myRegistrations.size(), myBookings.size());
    cout << string(50, '=') << endl;
}