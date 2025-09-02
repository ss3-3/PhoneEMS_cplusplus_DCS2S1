#include "file_operation.h"
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
#include "feedback.h"
#include "system_data.h"
#include "timeslot.h"
#include "product.h"
#include "utility_fun.h"
#include "input_validation.h"

string formatToTwoDecimals(double value) {
    ostringstream stream;
    stream << fixed << setprecision(2) << value;
    return stream.str();
}

// File operations implementation

void saveUsersToFile(const vector<Organizer>& users) {
    ofstream file("userInfo.txt");
    if (!file.is_open()) {
        cout << "Error: Could not save user data to file!" << endl;
        return;
    }

    for (const auto& user : users) {
        file << user.userID << "|"
            << user.organizerName << "|"
            << user.age << "|"
            << user.manufacturer << "|"
            << user.position << "|"
            << user.organizerContact << "|"
            << user.organizerEmail << "|"
            << user.password << "|"
            << (user.isLoggedIn ? "1" : "0") << endl;
    }

    file.close();
    cout << "User data saved successfully." << endl;
}

void saveDataToFiles(const SystemData& data) {
    saveUsersToFile(data.organizer);
    saveVenuesToFile(data.venues);
    saveRegistrationsToFile(data.registrations);
    saveBookingsToFile(data.bookings);
    savePaymentsToFile(data.payments);
    saveFeedbackToFile(data.feedbacks);
    cout << "Data saved successfully to files." << endl;
}

void loadDataFromFiles(SystemData& data) {
    loadVenuesFromFile(data.venues);
    loadUserFromFile(data.organizer);
    loadRegistrationsFromFile(data.registrations);
    loadBookingsFromFile(data.bookings);
    loadPaymentsFromFile(data.payments);
    loadFeedbackFromFile(data.feedbacks);
}

void saveVenuesToFile(const vector<Venue>& venues) {
    ofstream file("venues.txt");
    if (file.is_open()) {
        for (const auto& venue : venues) {
            file << venue.venueID << "|"
                << venue.venueName << "|"
                << venue.address << "|"
                << venue.capacity << "|"
                << venue.rentalCost << "|"
                << venue.contactPerson << "|"
                << venue.phoneNumber << endl;

            // Save booking schedule
            for (const auto& slot : venue.bookingSchedule) {
                file << "SLOT|" << slot.date.toString() << "|"
                    << slot.time << "|" << slot.eventID << "|"
                    << (slot.isBooked ? true : false) << endl;
            }
            file << "END_VENUE" << endl;
        }
        file.close();
    }
}

void loadUserFromFile(vector<Organizer>& users) {
    ifstream file("userInfo.txt");
    string line;

    if (!file.is_open()) {
        cout << "No existing user file found." << endl;
        return;
    }

    users.clear(); // Clear existing data

    while (getline(file, line)) {
        stringstream ss(line);
        string field;
        Organizer user;

        getline(ss, user.userID, '|');
        getline(ss, user.organizerName, '|');
        getline(ss, field, '|');
        user.age = stoi(field); //change string to int
        getline(ss, user.manufacturer, '|');
        getline(ss, user.position, '|');
        getline(ss, user.organizerContact, '|');
        getline(ss, user.organizerEmail, '|');
        strncpy_s(user.password, field.c_str(), sizeof(user.password));
        user.password[sizeof(user.password) - 1] = '\0'; //make sure end char

        // Parse isLoggedIn boolean
        getline(ss, field, '|');
        user.isLoggedIn = (field == "1");

        users.push_back(user);
    }
    file.close();
}

void saveRegistrationsToFile(const vector<EventRegistration>& registrations) {
    ofstream outFile("registrations.txt");
    if (!outFile) {
        cerr << "Error opening file for saving: " << "registrations.txt" << endl;
        return;
    }

    for (const auto& reg : registrations) {
        outFile << reg.eventID << "|"
            << reg.manufacturer << "|"
            << reg.eventTitle << "|"
            << reg.productQuantity << "|";

        // save phoneInfo (multiple products separated by ;)
        for (size_t i = 0; i < reg.phoneInfo.size(); i++) {
            outFile << reg.phoneInfo[i].productName << ","
                << reg.phoneInfo[i].productModel << ","
                << reg.phoneInfo[i].productPrice;
            if (i < reg.phoneInfo.size() - 1) outFile << ";";
        }
        outFile << "|";

        outFile << reg.description << "|"
            << reg.expectedGuests << "|"
            << reg.estimatedBudget << "|"
            << reg.eventStatus << "|"

            // Organizer info
            << reg.organizer.userID << "|"
            << reg.organizer.organizerName << "|"
            << reg.organizer.organizerContact << "|"
            << reg.organizer.organizerEmail << "|"
            << reg.organizer.position << "\n";
    }

    outFile.close();
}

void saveBookingsToFile(const vector<EventBooking>& bookings) {
    ofstream file("bookings.txt");
    if (file.is_open()) {
        for (const auto& booking : bookings) {
            file << booking.bookingID << "|"
                << booking.eventReg.eventID << "|"
                << booking.eventReg.eventTitle << "|"
                << booking.eventReg.manufacturer << "|"
                << booking.eventReg.description << "|"
                << booking.eventReg.expectedGuests << "|"
                << booking.eventReg.estimatedBudget << "|"
                << booking.eventReg.eventStatus << "|"
                << booking.eventReg.organizer.userID << "|"
                << booking.eventReg.organizer.organizerName << "|"
                << booking.eventReg.organizer.organizerContact << "|"
                << booking.eventReg.organizer.organizerEmail << "|"
                << booking.eventReg.organizer.position << "|"
                << booking.eventDate.toString() << "|"
                << booking.eventTime << "|"
                << booking.venue.venueID << "|"
                << booking.venue.venueName << "|"
                << booking.venue.address << "|"
                << booking.venue.capacity << "|"
                << booking.venue.rentalCost << "|"
                << booking.venue.contactPerson << "|"
                << booking.venue.phoneNumber << "|"
                << booking.bookingStatus << "|"
                << booking.finalCost << endl;
        }
        file.close();
    }
}

void loadVenuesFromFile(vector<Venue>& venues) {
    ifstream file("venues.txt");
    string line;

    if (file.is_open()) {
        venues.clear(); // Clear existing data

        while (getline(file, line)) {
            if (line == "END_VENUE") continue;

            if (line.substr(0, 5) == "SLOT|") {
                // Handle booking slots for the last venue
                if (!venues.empty()) {
                    stringstream ss(line.substr(5));
                    string token;
                    TimeSlot slot;

                    getline(ss, token, '|'); // date
                    // Parse date
                    stringstream dateStream(token);
                    string datePart;
                    getline(dateStream, datePart, '-');
                    slot.date.year = stoi(datePart);
                    getline(dateStream, datePart, '-');
                    slot.date.month = stoi(datePart);
                    getline(dateStream, datePart);
                    slot.date.day = stoi(datePart);

                    getline(ss, slot.time, '|');
                    getline(ss, slot.eventID, '|');
                    getline(ss, token, '|');
                    slot.isBooked = (token == "1");

                    venues.back().bookingSchedule.push_back(slot);
                }
            }
            else {
                // Handle venue data
                stringstream ss(line);
                string token;
                Venue venue;

                getline(ss, venue.venueID, '|');
                getline(ss, venue.venueName, '|');
                getline(ss, venue.address, '|');
                getline(ss, token, '|');
                venue.capacity = stoi(token);
                getline(ss, token, '|');
                venue.rentalCost = stod(token);
                getline(ss, venue.contactPerson, '|');
                getline(ss, venue.phoneNumber, '|');

                venues.push_back(venue);
            }
        }
        file.close();
    }
}

void loadRegistrationsFromFile(vector<EventRegistration>& registrations) {
    ifstream inFile("registrations.txt");
    if (!inFile) {
        cerr << "Error: Cannot open file for loading registrations." << endl;
        return;
    }
    registrations.clear();
    string line;

    while (getline(inFile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        EventRegistration reg;
        string temp;

        getline(ss, reg.eventID, '|');
        getline(ss, reg.manufacturer, '|');
        getline(ss, reg.eventTitle, '|');

        getline(ss, temp, '|');
        reg.productQuantity = stoi(temp);

        // phoneInfo
        getline(ss, temp, '|');
        reg.phoneInfo.clear();
        if (!temp.empty()) {
            stringstream phoneStream(temp);
            string phoneToken;
            while (getline(phoneStream, phoneToken, ';')) {
                stringstream phoneDetails(phoneToken);
                string name, model, priceStr;
                getline(phoneDetails, name, ',');
                getline(phoneDetails, model, ',');
                getline(phoneDetails, priceStr, ',');
                if (!name.empty() && !model.empty() && !priceStr.empty()) {
                    Product p{ name, model, stod(priceStr) };
                    reg.phoneInfo.push_back(p);
                }
            }
        }

        getline(ss, reg.description, '|');

        getline(ss, temp, '|');
        reg.expectedGuests = stoi(temp);

        getline(ss, temp, '|');
        reg.estimatedBudget = stod(temp);

        getline(ss, reg.eventStatus, '|');

        // Organizer fields
        getline(ss, reg.organizer.userID, '|');
        getline(ss, reg.organizer.organizerName, '|');
        getline(ss, reg.organizer.organizerContact, '|');
        getline(ss, reg.organizer.organizerEmail, '|');
        getline(ss, reg.organizer.position, '|');

        registrations.push_back(reg);
    }

    inFile.close();
}

void loadBookingsFromFile(vector<EventBooking>& bookings) {
    ifstream file("bookings.txt");
    string line;
    int lineNumber = 0;
    int successCount = 0;
    int errorCount = 0;

    if (!file.is_open()) {
        cout << "No existing booking file found. Starting with empty bookings." << endl;
        return;
    }

    bookings.clear(); // Clear existing data
    cout << "Reading bookings file..." << endl;

    while (getline(file, line)) {
        lineNumber++;

        if (line.empty()) {
            continue;
        }

        cout << "Processing booking line " << lineNumber << endl;

        stringstream ss(line);
        string token;
        EventBooking booking;

        try {
            // Parse fields in the EXACT order they are saved
            if (!getline(ss, booking.bookingID, '|') ||
                !getline(ss, booking.eventReg.eventID, '|') ||
                !getline(ss, booking.eventReg.eventTitle, '|') ||
                !getline(ss, booking.eventReg.manufacturer, '|') ||
                !getline(ss, booking.eventReg.description, '|')) {
                cout << "Error: Missing basic booking info at line " << lineNumber << endl;
                errorCount++;
                continue;
            }

            if (!getline(ss, token, '|')) {
                cout << "Error: Missing expectedGuests at line " << lineNumber << endl;
                errorCount++;
                continue;
            }
            booking.eventReg.expectedGuests = stoi(token);

            if (!getline(ss, token, '|')) {
                cout << "Error: Missing estimatedBudget at line " << lineNumber << endl;
                errorCount++;
                continue;
            }
            booking.eventReg.estimatedBudget = stod(token);

            // Parse event status and organizer info (including userID)
            if (!getline(ss, booking.eventReg.eventStatus, '|') ||
                !getline(ss, booking.eventReg.organizer.userID, '|') ||
                !getline(ss, booking.eventReg.organizer.organizerName, '|') ||
                !getline(ss, booking.eventReg.organizer.organizerContact, '|') ||
                !getline(ss, booking.eventReg.organizer.organizerEmail, '|') ||
                !getline(ss, booking.eventReg.organizer.position, '|')) {
                cout << "Error: Missing organizer info at line " << lineNumber << endl;
                errorCount++;
                continue;
            }

            // Parse event date
            if (!getline(ss, token, '|')) {
                cout << "Error: Missing date at line " << lineNumber << endl;
                errorCount++;
                continue;
            }

            stringstream dateStream(token);
            string datePart;
            if (!getline(dateStream, datePart, '-') ||
                !getline(dateStream, datePart, '-') ||
                !getline(dateStream, datePart)) {
                cout << "Error: Invalid date format at line " << lineNumber << endl;
                errorCount++;
                continue;
            }

            // Parse date parts correctly
            stringstream dateStream2(token);
            getline(dateStream2, datePart, '-');
            booking.eventDate.year = stoi(datePart);
            getline(dateStream2, datePart, '-');
            booking.eventDate.month = stoi(datePart);
            getline(dateStream2, datePart);
            booking.eventDate.day = stoi(datePart);

            // Parse event time and venue info
            if (!getline(ss, booking.eventTime, '|') ||
                !getline(ss, booking.venue.venueID, '|') ||
                !getline(ss, booking.venue.venueName, '|') ||
                !getline(ss, booking.venue.address, '|')) {
                cout << "Error: Missing venue basic info at line " << lineNumber << endl;
                errorCount++;
                continue;
            }

            if (!getline(ss, token, '|')) {
                cout << "Error: Missing venue capacity at line " << lineNumber << endl;
                errorCount++;
                continue;
            }
            booking.venue.capacity = stoi(token);

            if (!getline(ss, token, '|')) {
                cout << "Error: Missing venue rental cost at line " << lineNumber << endl;
                errorCount++;
                continue;
            }
            booking.venue.rentalCost = stod(token);

            if (!getline(ss, booking.venue.contactPerson, '|') ||
                !getline(ss, booking.venue.phoneNumber, '|') ||
                !getline(ss, booking.bookingStatus, '|')) {
                cout << "Error: Missing venue contact info at line " << lineNumber << endl;
                errorCount++;
                continue;
            }

            if (!getline(ss, token)) {
                cout << "Error: Missing final cost at line " << lineNumber << endl;
                errorCount++;
                continue;
            }
            booking.finalCost = stod(token);

            // If we get here, parsing was successful
            bookings.push_back(booking);
            successCount++;
            cout << "Successfully loaded booking: " << booking.bookingID << endl;

        }
        catch (const std::exception& e) {
            cout << "Error loading booking at line " << lineNumber << ": " << e.what() << endl;
            cout << "Line content: " << line << endl;
            errorCount++;
            continue;
        }
    }

    file.close();
    cout << "Booking loading completed: " << successCount << " successful, " << errorCount << " errors." << endl;
}

void saveFeedbackToFile(const vector<EventFeedback>& feedbacks) {
    ofstream file("feedback.txt");
    if (!file.is_open()) {
        cout << "Error: Unable to open feedback file for saving! Kindly try again!" << endl;
        return;
    }

    for (const auto& feedback : feedbacks) {
        file << feedback.feedbackID << "|"
            << feedback.bookingID << "|"
            << feedback.eventTitle << "|"
            << feedback.organizerName << "|"
            << feedback.eventDate.toString() << "|"
            << feedback.venueName << "|"
            << feedback.submittedBy << "|"
            << feedback.submissionDate.toString() << "|"
            << feedback.venueRating << "|"
            << feedback.organizationRating << "|"
            << feedback.logisticsRating << "|"
            << feedback.overallRating << "|"
            << (feedback.wouldRecommend ? "1" : "0") << "|"
            << feedback.venueComments << "|"
            << feedback.organizationComments << "|"
            << feedback.logisticsComments << "|"
            << feedback.generalComments << "|"
            << feedback.suggestions << endl;
    }
    file.close();
}

void loadFeedbackFromFile(vector<EventFeedback>& feedbacks) {
    ifstream file("feedback.txt");
    string line;

    if (!file.is_open()) {
        cout << "No existing feedback file found." << endl;
        return;
    }

    feedbacks.clear(); // Clear existing data

    while (getline(file, line)) {
        stringstream ss(line);
        string field;
        EventFeedback feedback;

        getline(ss, feedback.feedbackID, '|');
        getline(ss, feedback.bookingID, '|');
        getline(ss, feedback.eventTitle, '|');
        getline(ss, feedback.organizerName, '|');

        // Parse event date
        getline(ss, field, '|');
        stringstream eventDateStream(field);
        string datePart;
        getline(eventDateStream, datePart, '-');
        feedback.eventDate.year = stoi(datePart);
        getline(eventDateStream, datePart, '-');
        feedback.eventDate.month = stoi(datePart);
        getline(eventDateStream, datePart);
        feedback.eventDate.day = stoi(datePart);

        getline(ss, feedback.venueName, '|');
        getline(ss, feedback.submittedBy, '|');

        // Parse submission date
        getline(ss, field, '|');
        stringstream submissionDateStream(field);
        getline(submissionDateStream, datePart, '-');
        feedback.submissionDate.year = stoi(datePart);
        getline(submissionDateStream, datePart, '-');
        feedback.submissionDate.month = stoi(datePart);
        getline(submissionDateStream, datePart);
        feedback.submissionDate.day = stoi(datePart);

        // Parse ratings
        getline(ss, field, '|');
        feedback.venueRating = stoi(field);
        getline(ss, field, '|');
        feedback.organizationRating = stoi(field);
        getline(ss, field, '|');
        feedback.logisticsRating = stoi(field);
        getline(ss, field, '|');
        feedback.overallRating = stoi(field);

        // Parse recommendation
        getline(ss, field, '|');
        feedback.wouldRecommend = (field == "1");

        // Parse comments
        getline(ss, feedback.venueComments, '|');
        getline(ss, feedback.organizationComments, '|');
        getline(ss, feedback.logisticsComments, '|');
        getline(ss, feedback.generalComments, '|');
        getline(ss, feedback.suggestions, '|');

        feedbacks.push_back(feedback);
    }
    file.close();
}

void savePaymentsToFile(const vector<Payment>& payments) {
    ofstream file("payments.txt");
    if (file.is_open()) {
        for (const auto& payment : payments) {
            file << payment.paymentID << "|"
                << payment.bookingID << "|"
                << payment.amount << "|"
                << payment.paymentDate.day << "/" << payment.paymentDate.month << "/" << payment.paymentDate.year << "|"
                << payment.paymentMethod << "|"
                << payment.paymentStatus << "|"
                << payment.transactionReference << "|"
                << payment.cardNumber << "|"
                << payment.cardHolderName << endl;
        }
        file.close();
    }
}

void loadPaymentsFromFile(vector<Payment>& payments) {
    ifstream file("payments.txt");
    string line;
    int lineNumber = 0;
    int successCount = 0;
    int errorCount = 0;

    if (!file.is_open()) {
        cout << "No existing payment file found. Starting with empty payments." << endl;
        return;
    }

    payments.clear(); // Clear existing data
    cout << "Reading payments file..." << endl;

    while (getline(file, line)) {
        lineNumber++;

        if (line.empty()) {
            continue;
        }

        cout << "Processing payment line " << lineNumber << endl;

        stringstream ss(line);
        string token;
        Payment payment;

        try {
            // Parse paymentID
            if (!getline(ss, payment.paymentID, '|')) {
                cout << "Error: Missing paymentID at line " << lineNumber << endl;
                errorCount++;
                continue;
            }

            // Parse bookingID
            if (!getline(ss, payment.bookingID, '|')) {
                cout << "Error: Missing bookingID at line " << lineNumber << endl;
                errorCount++;
                continue;
            }

            // Parse amount
            if (!getline(ss, token, '|')) {
                cout << "Error: Missing amount at line " << lineNumber << endl;
                errorCount++;
                continue;
            }
            payment.amount = stod(token);

            // Parse payment date (format: day/month/year)
            if (!getline(ss, token, '|')) {
                cout << "Error: Missing payment date at line " << lineNumber << endl;
                errorCount++;
                continue;
            }

            // Parse the date string (day/month/year format)
            stringstream dateStream(token);
            string datePart;

            if (!getline(dateStream, datePart, '/')) {
                cout << "Error: Invalid date format (day) at line " << lineNumber << endl;
                errorCount++;
                continue;
            }
            payment.paymentDate.day = stoi(datePart);

            if (!getline(dateStream, datePart, '/')) {
                cout << "Error: Invalid date format (month) at line " << lineNumber << endl;
                errorCount++;
                continue;
            }
            payment.paymentDate.month = stoi(datePart);

            if (!getline(dateStream, datePart)) {
                cout << "Error: Invalid date format (year) at line " << lineNumber << endl;
                errorCount++;
                continue;
            }
            payment.paymentDate.year = stoi(datePart);

            // Parse payment method
            if (!getline(ss, payment.paymentMethod, '|')) {
                cout << "Error: Missing payment method at line " << lineNumber << endl;
                errorCount++;
                continue;
            }

            // Parse payment status
            if (!getline(ss, payment.paymentStatus, '|')) {
                cout << "Error: Missing payment status at line " << lineNumber << endl;
                errorCount++;
                continue;
            }

            // Parse transaction reference
            if (!getline(ss, payment.transactionReference, '|')) {
                cout << "Error: Missing transaction reference at line " << lineNumber << endl;
                errorCount++;
                continue;
            }

            // Parse card number (can be empty for cash/bank payments)
            if (!getline(ss, payment.cardNumber, '|')) {
                // If we can't read card number, set it to empty and continue
                payment.cardNumber = "";
            }

            // Parse card holder name (can be empty for cash/bank payments)
            if (!getline(ss, payment.cardHolderName)) {
                // If we can't read card holder name, set it to empty
                payment.cardHolderName = "";
            }

            // If we get here, parsing was successful
            payments.push_back(payment);
            successCount++;
            cout << "Successfully loaded payment: " << payment.paymentID << endl;

        }
        catch (const std::exception& e) {
            cout << "Error loading payment at line " << lineNumber << ": " << e.what() << endl;
            cout << "Line content: " << line << endl;
            errorCount++;
            continue;
        }
    }

    file.close();
    cout << "Payment loading completed: " << successCount << " successful, " << errorCount << " errors." << endl;
}