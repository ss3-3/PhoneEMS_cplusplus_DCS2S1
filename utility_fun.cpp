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
#include "system_data.h"
#include "timeslot.h"
#include "product.h"
#include "utility_fun.h"
#include "input_validation.h"
#include "file_operation.h"

using namespace std;

string trimWhitespace(const string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

string normalizeUserID(const string& userID) {
    string trimmed = trimWhitespace(userID);
    // Convert to uppercase for consistent comparison
    transform(trimmed.begin(), trimmed.end(), trimmed.begin(), ::toupper);
    return trimmed;
}

string getCurrentUserName(const SystemData& data) {
    // Return the name of the currently logged in user
    if (!data.currentUser.empty()) {
        // Find user by ID and return name
        for (const auto& user : data.organizer) {
            if (user.userID == data.currentUser) {
                return user.organizerName;
            }
        }
    }
    return "Unknown User";
}

// Display system logo
void displayLogo() {
    clearScreen();
    cout << endl;
    cout << "	.----------------.  .----------------.  .----------------. " << endl;
    cout << "	| .--------------. || .--------------. || .--------------. | " << endl;
    cout << "	| |     ____     | || |  _________   | || | ____    ____ | | " << endl;
    cout << "	| |   .'    `.   | || | |_   ___  |  | || ||_   \\  /   _|| | " << endl;
    cout << "	| |  /  .--.  \\  | || |   | |_  \\_|  | || |  |   \\/   |  | | " << endl;
    cout << "	| |  | |    | |  | || |   |  _ | _   | || |  | |\\  /| |  | | " << endl;
    cout << "	| |  \\  `- - '/  | || |  _| |___/ |  | || | _| |_\\/_| |_ | | " << endl;
    cout << "	| |   `.____.'   | || | |_________|  | || ||_____||_____|| | " << endl;
    cout << "	| |              | || |              | || |              | | " << endl;
    cout << "	| '--------------' || '--------------' || '--------------' | " << endl;
    cout << "	'----------------'  '----------------'  '----------------' " << endl;
    cout << endl;

    cout << "	______ _                         _____                _ " << endl;
    cout << "	| ___ \\ |                       |  ___|              | | " << endl;
    cout << "	| |_/ / |__   ___  _ __   ___   | |____   _____ _ __ | |_ " << endl;
    cout << "	| __/ | '_ \\ / _ \\| '_ \\ / _ \\  |  __\\ \\ / / _ \\ '_ \\| __|                                                " << endl;
    cout << "	| |   | | | | (_) | | | |  __/  | |___\\ V /  __/ | | | |_ " << endl;
    cout << "	\\_|   |_| |_|\\___/|_| |_|\\___|  \\____/ \\_/ \\___|_| |_|\\__| " << endl;


    cout << "	___  ___                                                  _     _____           _ " << endl;
    cout << "	|  \\/  |                                                 | |   /  ___|         | | " << endl;
    cout << "	| .  . | __ _ _ __   __ _  __ _  ___ _ __ ___   ___ _ __ | |_  \\ `-- ._   _ ___| |_ ___ _ __ ___ " << endl;
    cout << "	| |\\/| |/ _` | '_ \\ / _` |/ _` |/ _ \\ '_ ` _ \\ / _ \\ '_ \\| __|  `__. \\ | | / __| __/ _ \\ '_ ` _ \\ " << endl;
    cout << "	| |  | | (_| | | | | (_| | (_| |  __/ | | | | |  __/ | | | |_  /\\__/ / |_| \\__ \\ | | __/ | | | | | " << endl;
    cout << "	\\_|  |_/\\__,_|_| |_|\\__,_|\\__, |\\___|_| |_| |_|\\___|_| |_|\\__| \\____/\\__, |___ /\\__\\___|_| |_| |_| " << endl;
    cout << "                                   __/ |                                      __/ | " << endl;
    cout << "				  |___/                                      |___/ " << endl;
    cout << endl;
}

// Clear screen function
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

Date getCurrentDate() {
    // For demonstration purposes - in real application, use system date
    Date current;
    current.year = 2025;
    current.month = 9;
    current.day = 20;
    return current;
}

string generateUserID(const vector<Organizer>& user) {
    int nextNumber = user.size() + 1001;
    return "USER" + to_string(nextNumber);
}

string generateEventID(const vector<EventRegistration>& registrations) {
    int maxNumber = 1000; // Starting base number (so first ID will be EVT1001)

    // Parse all existing event IDs to find the maximum number
    for (const auto& reg : registrations) {

        // Extract the number part using stringstream
        string numberPart = reg.eventID.substr(3); // Remove "EVT" prefix
        stringstream ss(numberPart);
        int currentNumber;

        // Try to convert the number part to integer
        if (ss >> currentNumber) {
            // Successfully parsed the entire string as a number
            if (currentNumber > maxNumber) {
                maxNumber = currentNumber;
            }
        }
    }

    // Generate new ID with next available number
    int nextNumber = maxNumber + 1;
    return "EVT" + to_string(nextNumber);
}

string generateBookingID(const vector<EventBooking>& bookings) {
    int maxNumber = 2000;

    for (const auto& booking : bookings) {
        // Extract the number part using stringstream
        string numberPart = booking.bookingID.substr(3); // Remove "BKG" prefix
        stringstream ss(numberPart);
        int currentNumber;

        // Try to convert the number part to integer
        if (ss >> currentNumber) {
            // Successfully parsed the entire string as a number
            if (currentNumber > maxNumber) {
                maxNumber = currentNumber;
            }
        }
    }

    // Generate new ID with next available number
    int nextNumber = maxNumber + 1;
    return "BKG" + to_string(nextNumber);
}

string generateVenueID(const vector<Venue>& venues) {
    int nextNumber = venues.size() + 1;
    string id = "V" + string(3 - to_string(nextNumber).length(), '0') + to_string(nextNumber);
    return id;
}

string formatDouble(double value, int precision) {
    ostringstream out;
    out << fixed << setprecision(precision) << value;
    return out.str();
}

// Pause screen for user interaction
void pauseScreen() {
    cout << "\nPress any key to continue...";
    system("pause");
}