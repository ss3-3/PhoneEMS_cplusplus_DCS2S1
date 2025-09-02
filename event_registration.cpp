#include "event_registration.h"
#include "system_data.h"
#include "event_booking.h"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <regex>
#include "date.h"
#include "venue.h"
#include "user.h"
#include "timeslot.h"
#include "utility_fun.h"
#include "input_validation.h"
#include "file_operation.h"

using namespace std;

// Event Registration System Functions
void eventRegistrationMenu(SystemData& data) {
    bool exitMenu = false;
    while (!exitMenu) {
        clearScreen();
        displayLogo();
        cout << "=== EVENT REGISTRATION SYSTEM ===" << endl;
        cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;
        cout << "1. Create New Event Registration" << endl;
        cout << "2. View My Event Registrations" << endl;
        cout << "3. Update Event Registration" << endl;
        cout << "4. Delete Event Registration" << endl;
        cout << "5. Back to Main Menu" << endl;
        cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;

        int choice = getValidIntegerInput("Enter your choice [1-5]: ", 1, 5);

        switch (choice) {
        case 1:
            createEventRegistration(data);
            break;
        case 2:
            viewEventRegistrations(data);
            break;
        case 3:
            updateEventRegistration(data);
            break;
        case 4:
            deleteEventRegistration(data);
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

void createEventRegistration(SystemData& data) {
    clearScreen();
    cout << "=== CREATE NEW EVENT REGISTRATION ===" << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    EventRegistration newReg;

    // Find current user's organizer info - SAME LOGIC AS BOOKING
    Organizer currentOrganizerData;
    bool organizerFound = false;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);

    // Try to find organizer info from existing registrations first (same as booking logic)
    for (const auto& reg : data.registrations) {
        string normalizedRegUser = normalizeUserID(reg.organizer.userID);
        if (normalizedRegUser == normalizedCurrentUser) {
            currentOrganizerData = reg.organizer;
            organizerFound = true;
            break;
        }
    }

    // If not found in existing registrations, check data.organizer array
    if (!organizerFound) {
        for (const auto& org : data.organizer) {
            string normalizedOrgUser = normalizeUserID(org.userID);
            if (normalizedOrgUser == normalizedCurrentUser) {
                currentOrganizerData = org;
                organizerFound = true;
                break;
            }
        }
    }

    if (!organizerFound) {
        cout << "Error: User organizer information not found!" << endl;
        cout << "Debug Info:" << endl;
        cout << "Current User: '" << data.currentUser << "'" << endl;
        cout << "Normalized Current User: '" << normalizedCurrentUser << "'" << endl;

        cout << "Available organizers in data.organizer:" << endl;
        for (const auto& org : data.organizer) {
            cout << "  User ID: '" << org.userID << "'" << endl;
            cout << "  Normalized: '" << normalizeUserID(org.userID) << "'" << endl;
            cout << "  Name: " << org.organizerName << endl;
        }

        cout << "Available organizers in existing registrations:" << endl;
        for (const auto& reg : data.registrations) {
            cout << "  User ID: '" << reg.organizer.userID << "'" << endl;
            cout << "  Normalized: '" << normalizeUserID(reg.organizer.userID) << "'" << endl;
            cout << "  Name: " << reg.organizer.organizerName << endl;
        }
        return;
    }

    // Set the organizer for this registration
    newReg.organizer = currentOrganizerData;

    string productName, productModel;
    double productPrice;

    cout << "+ ====================================================================== +" << endl;
    cout << "||                               EVENT DETAILS                           ||" << endl;
    cout << "+ ====================================================================== +" << endl << endl;

    // Generate unique event ID
    newReg.eventID = generateEventID(data.registrations);
    cout << "Generated Event ID: " << newReg.eventID << endl;
    cout << "Organizer: " << currentOrganizerData.organizerName << " (" << currentOrganizerData.position << ")" << endl << endl;

    newReg.manufacturer = getValidStringInputWithExit("Enter Manufacturer: ");
    if (newReg.manufacturer == "") {
        confirmExit();
        return;
    }

    newReg.eventTitle = getValidStringInputWithExit("Enter Event Title: ");
    if (newReg.eventTitle.empty()) {
        confirmExit();
        return;
    }

    // Product details section
    cout << "+ ====================================================================== +" << endl;
    cout << "||                               PRODUCT DETAILS                        ||" << endl;
    cout << "+ ====================================================================== +" << endl << endl;

    newReg.productQuantity = getValidIntegerInputWithExit("Enter Product Quantity to launch: ", 1, 20);
    if (newReg.productQuantity == -1) {
        confirmExit();
        return;
    }

    // Ensure productQuantity is valid before resizing
    if (newReg.productQuantity <= 0 || newReg.productQuantity > 20) {
        cout << "Error: Invalid product quantity!" << endl;
        confirmExit();
        return;
    }

    // Resize the vector safely
    try {
        newReg.phoneInfo.resize(newReg.productQuantity);
    }
    catch (const std::exception& e) {
        cout << "Error: Failed to allocate memory for products!" << endl;
        confirmExit();
        return;
    }

    for (int i = 0; i < newReg.productQuantity; i++) {
        cout << "Product " << i + 1 << ": " << endl;
        productName = getValidStringInputWithExit("Enter product name: ");
        if (productName.empty()) {
            confirmExit();
            return;
        }
        productModel = getValidStringInputWithExit("Enter product model: ");
        if (productModel.empty()) {
            confirmExit();
            return;
        }
        productPrice = getValidDoubleInputWithExit("Enter product price (RM): ", 5);
        if (productPrice == -1.0) {
            confirmExit();
            return;
        }

        // Ensure valid price before assignment
        if (productPrice < 0) {
            cout << "Error: Invalid product price!" << endl;
            confirmExit();
            return;
        }

        newReg.phoneInfo[i] = { productName, productModel, productPrice };
    }

    // Additional details section
    cout << "+ ====================================================================== +" << endl;
    cout << "||                            ADDITIONAL DETAILS                       ||" << endl;
    cout << "+ ====================================================================== +" << endl << endl;

    newReg.description = getValidStringInputWithExit("Enter Event Description: ");
    if (newReg.description.empty()) {
        confirmExit();
        return;
    }

    newReg.expectedGuests = getValidIntegerInputWithExit("Enter Expected Number of Guests (100-1200): ", 100, 1200);
    if (newReg.expectedGuests == -1) {
        confirmExit();
        return;
    }

    newReg.estimatedBudget = getValidDoubleInputWithExit("Enter Estimated Budget (RM): ", 2500);
    if (newReg.estimatedBudget == -1.0) {
        confirmExit();
        return;
    }

    newReg.eventStatus = "UNSCHEDULED";

    //Ensure all required fields are valid before adding to vector
    if (newReg.eventID.empty() || newReg.eventTitle.empty() || newReg.manufacturer.empty()) {
        cout << "Error: Critical registration data is missing!" << endl;
        confirmExit();
        return;
    }

    try {
        data.registrations.push_back(newReg);
    }
    catch (const std::exception& e) {
        cout << "Error: Failed to save registration!" << endl;
        confirmExit();
        return;
    }

    cout << "\n=== EVENT REGISTRATION CREATED SUCCESSFULLY ===" << endl;
    cout << "Event ID: " << newReg.eventID << endl;
    cout << "Manufacturer: " << newReg.manufacturer << endl;
    cout << "Event Title: " << newReg.eventTitle << endl;
    cout << "Product launched quantity: " << newReg.productQuantity << endl;
    for (int i = 0; i < newReg.productQuantity; i++) {
        cout << "Product " << i + 1 << endl;
        cout << "Product Name: " << newReg.phoneInfo[i].productName << endl;
        cout << "Product Model: " << newReg.phoneInfo[i].productModel << endl;
        cout << "Product Price (RM): " << newReg.phoneInfo[i].productPrice << endl;
    }
    cout << "Organizer: " << newReg.organizer.organizerName << endl;
    cout << "Position: " << newReg.organizer.position << endl;
    cout << "Contact No: " << newReg.organizer.organizerContact << endl;
    cout << "Email Address: " << newReg.organizer.organizerEmail << endl;
    cout << "Expected Guests: " << newReg.expectedGuests << endl;
    cout << "Estimated Budget: RM " << fixed << setprecision(2) << newReg.estimatedBudget << endl;
    cout << "Status: " << newReg.eventStatus << endl;

    saveRegistrationsToFile(data.registrations);
}

void viewEventRegistrations(const SystemData& data) {
    clearScreen();
    cout << "=== MY EVENT REGISTRATIONS ===" << endl;

    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    if (data.registrations.empty()) {
        cout << "No event registrations found in system." << endl;
        return;
    }

    // Filter user registrations with normalized comparison
    vector<EventRegistration> userRegistrations;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);

    for (const auto& reg : data.registrations) {
        string normalizedRegUser = normalizeUserID(reg.organizer.userID);

        if (normalizedRegUser == normalizedCurrentUser) {
            userRegistrations.push_back(reg);
        }
    }

    if (userRegistrations.empty()) {
        cout << "No event registrations found for your account." << endl;
        return;
    }

    //Display user's registrations only
    for (const auto& reg : userRegistrations) {
        cout << left << '+' << setfill('=') << setw(100) << '+' << endl;
        cout << "Event ID: " << reg.eventID << endl;
        cout << "Event Title: " << reg.eventTitle << endl;
        cout << "Manufacturer: " << reg.manufacturer << endl;
        cout << "Product launched quantity: " << reg.productQuantity << endl;

        for (int i = 0; i < reg.productQuantity; i++) {
            cout << "Product " << i + 1 << ": " << endl;
            cout << "  Product Name: " << reg.phoneInfo[i].productName << endl;
            cout << "  Product Model: " << reg.phoneInfo[i].productModel << endl;
            cout << "  Product Price (RM): " << fixed << setprecision(2) << reg.phoneInfo[i].productPrice << endl;
        }

        cout << "Event Description: " << reg.description << endl;
        cout << "Organizer Name: " << reg.organizer.organizerName << " (" << reg.organizer.position << ")" << endl;
        cout << "Organizer Contact No: " << reg.organizer.organizerContact << endl;
        cout << "Organizer Email Address: " << reg.organizer.organizerEmail << endl;
        cout << "Maximum Participants: " << reg.expectedGuests << endl;
        cout << "Budget: RM " << fixed << setprecision(2) << reg.estimatedBudget << endl;
        cout << "Status: " << reg.eventStatus << endl;
    }

    cout << left << '+' << setfill('=') << setw(100) << '+' << endl;
    cout << "Total Your Registrations: " << userRegistrations.size() << endl;
}

void editPhoneInfo(EventRegistration* regPtr) {
    string checkPhoneModel, newName, newModel, newPrice;
    double newPriceDouble;

    checkPhoneModel = getValidStringInputWithExit("Enter the phone model you want to update: ");
    if (checkPhoneModel.empty()) return;

    bool found = false;
    for (auto& phone : regPtr->phoneInfo) {
        if (phone.productModel == checkPhoneModel) {
            found = true;

            cout << "Current Name: " << phone.productName << endl;
            cout << "Current Model: " << phone.productModel << endl;
            cout << "Current Price: RM" << phone.productPrice << endl;

            // Phone name
            newName = getValidStringInputOrKeepCurrent("\nEnter new name: ");
            if (newName == "EXIT_REQUESTED") return;
            if (newName != "KEEP_CURRENT") phone.productName = newName;

            // Phone Model
            newModel = getValidStringInputOrKeepCurrent("\nEnter new model: ");
            if (newModel == "EXIT_REQUESTED") return;
            if (newModel != "KEEP_CURRENT") phone.productModel = newModel;

            // Phone price - FIX THE EXCEPTION HERE
            cout << endl << "Enter new price (or press Enter to keep current): ";
            getline(cin, newPrice);

            if (newPrice == "0") {
                if (getExit()) return;
            }
            else if (!newPrice.empty()) {
                // Add proper exception handling for stod()
                try {
                    // Trim whitespace first
                    size_t start = newPrice.find_first_not_of(" \t");
                    if (start != string::npos) {
                        size_t end = newPrice.find_last_not_of(" \t");
                        newPrice = newPrice.substr(start, end - start + 1);
                    }

                    // Validate that it's a valid number format before conversion
                    bool isValidFormat = true;
                    bool hasDecimal = false;

                    for (size_t i = 0; i < newPrice.length(); i++) {
                        if (newPrice[i] == '.') {
                            if (hasDecimal) {
                                isValidFormat = false;
                                break;
                            }
                            hasDecimal = true;
                        }
                        else if (newPrice[i] < '0' || newPrice[i] > '9') {
                            isValidFormat = false;
                            break;
                        }
                    }

                    if (!isValidFormat || newPrice.empty() || newPrice == ".") {
                        cout << "Invalid price format. Keeping current price." << endl;
                    }
                    else {
                        newPriceDouble = stod(newPrice);
                        if (newPriceDouble > 0) {
                            phone.productPrice = newPriceDouble;
                        }
                        else {
                            cout << "Price must be greater than 0. Keeping current price." << endl;
                        }
                    }
                }
                catch (const std::invalid_argument& e) {
                    cout << "Invalid price format. Keeping current price." << endl;
                }
                catch (const std::out_of_range& e) {
                    cout << "Price value is too large. Keeping current price." << endl;
                }
                catch (const std::exception& e) {
                    cout << "Error processing price. Keeping current price." << endl;
                }
            }

            cout << endl << "Phone updated successfully!" << endl;
            break;
        }
    }
    if (!found) {
        cout << "No phone found with model " << checkPhoneModel << endl;
    }
}

void updateEventRegistration(SystemData& data) {
    clearScreen();
    cout << "=== UPDATE EVENT REGISTRATION ===" << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    // Check if user has any registrations
    bool hasRegistrations = false;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);

    for (const auto& reg : data.registrations) {
        string normalizedRegUser = normalizeUserID(reg.organizer.userID);
        cout << "DEBUG (user for event reg): " << normalizedRegUser << endl;
        if (normalizedRegUser == normalizedCurrentUser) {
            hasRegistrations = true;
            break;
        }
    }

    if (!hasRegistrations) {
        cout << "You have no event registrations to update." << endl;
        cout << "\nDebug Info:" << endl;
        cout << "Current User: '" << data.currentUser << "'" << endl;
        cout << "Normalized Current User: '" << normalizedCurrentUser << "'" << endl;
        cout << "Total Registrations: " << data.registrations.size() << endl;

        for (size_t i = 0; i < data.registrations.size(); i++) {
            cout << "Registration " << (i + 1) << ":" << endl;
            cout << "  Event ID: " << data.registrations[i].eventID << endl;
            cout << "  Organizer User ID: '" << data.registrations[i].organizer.userID << "'" << endl;
            cout << "  Normalized: '" << normalizeUserID(data.registrations[i].organizer.userID) << "'" << endl;
            cout << "  Match: " << (normalizeUserID(data.registrations[i].organizer.userID) == normalizedCurrentUser ? "YES" : "NO") << endl;
        }
        return;
    }

    viewEventRegistrations(data);

    string eventID = getValidStringInput("\nEnter Event ID to update: ");
    if (eventID.empty()) return;

    eventID = toUpperCase(eventID);

    // Find registration and verify ownership - FIXED WITH NORMALIZATION
    EventRegistration* regPtr = nullptr;
    for (auto& reg : data.registrations) {
        string normalizedRegUser = normalizeUserID(reg.organizer.userID);
        if (reg.eventID == eventID && normalizedRegUser == normalizedCurrentUser) {
            regPtr = &reg;
            break;
        }
    }

    if (regPtr == nullptr) {
        cout << "Event registration not found or you don't have permission to edit it!" << endl;
        return;
    }

    cout << "\n=== Current Registration Details ===" << endl;
    cout << "Event Title Registered: " << regPtr->eventTitle << endl;
    cout << "Phone launches quantity: " << regPtr->productQuantity << endl;
    int i = 1;
    cout << "Phone(s) registered" << endl;
    for (const auto& phone : regPtr->phoneInfo) {
        cout << "-------------------" << endl;
        cout << "Phone " << i << ": " << endl;
        cout << "Phone Name: " << phone.productName << endl;
        cout << "Phone Model: " << phone.productModel << endl;
        cout << "Phone Price: " << fixed << setprecision(2) << phone.productPrice << endl;
        i++;
    }
    cout << "Manufacturer: " << regPtr->manufacturer << endl;
    cout << "Description: " << regPtr->description << endl;
    cout << "Organizer: " << regPtr->organizer.organizerName << endl;
    cout << "Contact: " << regPtr->organizer.organizerContact << endl;
    cout << "Maximum Participants: " << regPtr->expectedGuests << endl;
    cout << "Budget: RM " << fixed << setprecision(2) << regPtr->estimatedBudget << endl;
    cout << "Status: " << regPtr->eventStatus << endl;

    cout << "\nWhat would you like to update?" << endl;
    cout << "1. Event Title" << endl;
    cout << "2. Product Launch Quantity" << endl;
    cout << "3. Phone Model" << endl;
    cout << "4. Manufacturer" << endl;
    cout << "5. Description" << endl;
    cout << "6. Maximum Participants" << endl;
    cout << "7. Estimated Budget" << endl;
    cout << "8. Cancel Modify" << endl;

    int choice = getValidIntegerInput("Enter choice [1-8]: ", 1, 8);

    switch (choice) {
    case 1: {
        string newEventTitle = getValidStringInputWithExit("Enter new event title: ");
        if (newEventTitle.empty()) return;
        regPtr->eventTitle = newEventTitle;
        break;
    }
    case 2: {
        regPtr->productQuantity = getValidIntegerInput("Enter new quantity: ", 1, 20);
        break;
    }
    case 3: {
        editPhoneInfo(regPtr);
        break;
    }
    case 4: {
        string newManufacturer = getValidStringInputWithExit("Enter new Manufacturer: ");
        if (newManufacturer.empty()) return;
        regPtr->manufacturer = newManufacturer;
        break;
    }
    case 5: {
        string newDescription = getValidStringInputWithExit("Enter new Description: ");
        if (newDescription.empty()) return;
        regPtr->description = newDescription;
        break;
    }
    case 6:
        regPtr->expectedGuests = getValidIntegerInput("Enter new Expected Guests (100-1200): ", 100, 1200);
        break;
    case 7:
        regPtr->estimatedBudget = getValidDoubleInput("Enter new Estimated Budget (RM): ");
        break;
    case 8:
        break;
    }

    saveRegistrationsToFile(data.registrations);
    cout << "Event registration updated successfully!" << endl;
}

void deleteEventRegistration(SystemData& data) {
    clearScreen();
    cout << "=== CANCEL EVENT REGISTRATION ===" << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    // Check if user has any registrations
    bool hasRegistrations = false;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);
    for (const auto& reg : data.registrations) {
        string normalizedRegUser = normalizeUserID(reg.organizer.userID);
        if (normalizedRegUser == normalizedCurrentUser) {
            hasRegistrations = true;
            break;
        }
    }

    if (!hasRegistrations) {
        cout << "You have no event registrations to cancel." << endl;
        return;
    }

    viewEventRegistrations(data);
    string eventID = getValidStringInput("\nEnter Event ID to cancel: ");
    if (eventID.empty()) return;
    eventID = toUpperCase(eventID);

    // Find registration and verify ownership - FIXED WITH NORMALIZATION
    int regIndex = -1;
    for (size_t i = 0; i < data.registrations.size(); i++) {
        string normalizedRegUser = normalizeUserID(data.registrations[i].organizer.userID);
        if (data.registrations[i].eventID == eventID &&
            normalizedRegUser == normalizedCurrentUser) {
            regIndex = i;
            break;
        }
    }

    if (regIndex == -1) {
        cout << "Event registration not found or you don't have permission to cancel it!" << endl;
        return;
    }

    // Check if event is already cancelled
    if (data.registrations[regIndex].eventStatus == "CANCELLED") {
        cout << "This event is already cancelled!" << endl;
        return;
    }

    cout << "\n=== Registration Details to Cancel ===" << endl;
    cout << "Event ID: " << data.registrations[regIndex].eventID << endl;
    cout << "Event Title: " << data.registrations[regIndex].eventTitle << endl;
    cout << "Manufacturer: " << data.registrations[regIndex].manufacturer << endl;
    cout << "Organizer: " << data.registrations[regIndex].organizer.organizerName << endl;
    cout << "Current Status: " << data.registrations[regIndex].eventStatus << endl;

    // Check if registration has active bookings
    vector<int> affectedBookings;
    for (size_t i = 0; i < data.bookings.size(); i++) {
        if (data.bookings[i].eventReg.eventID == eventID &&
            data.bookings[i].bookingStatus != "CANCELLED") {
            affectedBookings.push_back(i);
        }
    }

    // Show warning if there are active bookings
    if (!affectedBookings.empty()) {
        cout << "\nWARNING: This event has " << affectedBookings.size() << " active booking(s)!" << endl;
        cout << "Cancelling this event will automatically cancel all active bookings." << endl;
        cout << "Affected users will see their bookings as 'CANCELLED'." << endl;
    }

    vector<char> validChars = { 'Y', 'N' };
    char confirm = getValidCharInput("\nDo you really want to cancel this event? (Y/N): ", validChars);

    if (confirm == 'Y' || confirm == 'y') {
        // Cancel the event registration
        data.registrations[regIndex].eventStatus = "CANCELLED";

        // Cancel all active bookings for this event
        int cancelledBookings = 0;
        for (int bookingIndex : affectedBookings) {
            data.bookings[bookingIndex].bookingStatus = "CANCELLED";
            cancelledBookings++;
        }

        // Save changes to files
        saveRegistrationsToFile(data.registrations);
        saveBookingsToFile(data.bookings);

        cout << "\nEvent registration cancelled successfully!" << endl;
        if (cancelledBookings > 0) {
            cout << cancelledBookings << " booking(s) have been automatically cancelled." << endl;
            cout << "Affected users will be notified when they view their bookings." << endl;
        }
    }
    else {
        cout << "Cancellation aborted." << endl;
    }
}
