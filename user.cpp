#include "user.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <regex>
#include <cstring>
#include <cctype>
#include "date.h"
#include "venue.h"
#include "event_registration.h"
#include "event_booking.h"
#include "event_monitoring.h"
#include "system_data.h"
#include "timeslot.h"
#include "product.h"
#include "utility_fun.h"
#include "input_validation.h"
#include "file_operation.h"
#include "feedback.h"
using namespace std;

void signUp(SystemData& data) {
    string password = "", password2 = "";
    bool validation = false;

    Organizer currentUser;
    //need loop to ask user enter a valid userID
    cout << "\t === REGISTER AN NEW ACCOUNT ===" << endl;
    // Generate unique event ID
    currentUser.userID = generateUserID(data.organizer);
    cout << "Your User ID: " << currentUser.userID << endl;

    // Name
    if (IsIdDuplicate(currentUser.userID)) { //check from the file
        cout << "This name has already been taken" << endl;
    }
    currentUser.organizerName = getValidStringInputWithExit("Enter your Name: ");
    if (currentUser.organizerName.empty())
    {
        confirmExit();
        return;
    }
    currentUser.age = getValidIntegerInputWithExit("Enter your age: ", 18, 100);
    if (currentUser.age < 18 || currentUser.age >= 100)
    {
        // only accept 18 to 100 years old
        cout << "Warning: The user age must be 18 to 100 years old [Invalid age]" << endl;
    }
    if (currentUser.age == -1)
    {
        confirmExit();
        return;
    }
    // manufacturer
    currentUser.manufacturer = getValidStringInputWithExit("Enter Manufacturer/OEM: ");
    if (currentUser.manufacturer == "")
    {
        confirmExit(); //show the exit message
        return; //back to previous
    }
    // Position
    currentUser.position = getValidStringInputWithExit("Enter your position: ");
    if (currentUser.position.empty())
    {
        confirmExit();
        return;
    }
    currentUser.organizerContact = getValidPhoneNumber("Enter Organizer Contact: ");
    if (currentUser.organizerContact.empty())
    {
        confirmExit();
        return;
    }
    currentUser.organizerEmail = getValidEmailAddress("Enter Organizer Email Address  (must small capital letter with @XXX.com): ");
    if (currentUser.organizerEmail.empty())
    {
        confirmExit();
        return;
    }

    // Password
    while (!validation)
    {
        password = getValidPassword("Enter your password (must contain at least one uppercase letter, lowercase letter and number) \n-> New Password: ");

        if (password == "")
        {
            confirmExit();
            return;
        }

        password2 = getValidPassword("-> Re-type new Password: ");
        if (password2 == "")
        {
            confirmExit();
            return;
        }
        // Matching password validation
        if (password != password2)
        {
            cout << "Error: The first password does not match the second password!" << endl;
            cout << "Please try again! " << endl;
        }
        else
        {
            strcpy_s(currentUser.password, password.c_str()); //assign string value in char array
            validation = true; //exit loop
        }
    }

    cout << "==========================" << endl;
    cout << "  Register Successfully  " << endl;
    cout << "==========================" << endl;
    displayUserInfo(currentUser);
    data.organizer.push_back(currentUser);
    saveUsersToFile(data.organizer);
    data.currentUser = currentUser.userID;
}

void loginUser(SystemData& data, bool *validation) {
    clearScreen();
    cout << "=== USER LOGIN ===" << endl;
    cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;

    string userID = getValidStringInputWithExit("Enter User ID or Email: ");

    if (userID.empty())
    {
        confirmExit();
        return;
    }
    string password = getValidStringInputWithExit("Enter Password: ");
    if (password.empty())
    {
        confirmExit();
        return;
    }

    //// Normalize input for comparison
    //string normalizedInputID = normalizeUserID(userID);

    // Find user with normalized comparison
    for (auto& user : data.organizer) {
        //string normalizedStoredID = normalizeUserID(user.userID);

        if ((user.userID == userID || user.organizerEmail == userID) && (password == user.password)) {
            user.isLoggedIn = true;

            // CRITICAL: Store the ORIGINAL userID from file, not the input
            data.currentUser = user.userID;  // Use the stored version

            cout << "Login successful!" << endl;
            cout << "Welcome, " << user.organizerName << "!" << endl;

            // Debug output to verify IDs match
            //cout << "\n=== DEBUG INFO ===" << endl;
            //cout << "Input userID: '" << userID << "'" << endl;
            //cout << "Stored userID: '" << user.userID << "'" << endl;
            //cout << "Current user set to: '" << data.currentUser << "'" << endl;

            //// Count user's data immediately
            //int userRegistrations = 0;
            //int userBookings = 0;

            //for (const auto& reg : data.registrations) {
            //    cout << "Checking registration " << reg.eventID
            //        << " with userID: '" << reg.organizer.userID << "'" << endl;
            //    if (normalizeUserID(reg.organizer.userID) == normalizedStoredID) {
            //        userRegistrations++;
            //    }
            //}

            //for (const auto& booking : data.bookings) {
            //    cout << "Checking booking " << booking.bookingID
            //        << " with userID: '" << booking.eventReg.organizer.userID << "'" << endl;
            //    if (normalizeUserID(booking.eventReg.organizer.userID) == normalizedStoredID) {
            //        userBookings++;
            //    }
            //}

            //cout << "Found " << userRegistrations << " registrations and "
            //    << userBookings << " bookings for this user." << endl;
            //cout << "==================" << endl;

            saveUsersToFile(data.organizer);
            *validation = true;
            return;
        }
    }

    cout << "Invalid User ID or Password! \nTips: Enter your User ID with capital letter. " << endl;
    *validation = false;
    return;
}

bool IsIdDuplicate(const string& userID) {
    vector<Organizer> users;
    loadUserFromFile(users);

    for (const auto& user : users) {
        if (user.userID == userID) {
            return true;
        }
    }
    return false;
}

void displayUserInfo(const Organizer& organizer) {
    cout << "==========================" << endl;
    cout << "    USER INFORMATION      " << endl;
    cout << "==========================" << endl;
    cout << "User ID       : " << organizer.userID << endl;
    cout << "Name          : " << organizer.organizerName << endl;
    cout << "Age           : " << organizer.age << endl;
    cout << "Password      : ";
    int len = strlen(organizer.password); // get the length of the password
    for (int i = 0; i < len; i++) {
        cout << '*';
    }
    cout << endl;
    cout << "Contact       : " << organizer.organizerContact << endl;
    cout << "Email         : " << organizer.organizerEmail << endl;
    cout << "Manufacturer  : " << organizer.manufacturer << endl;
    cout << "Position      : " << organizer.position << endl;
    cout << "Login Status  : " << (organizer.isLoggedIn ? "Logged In" : "Logged Out") << endl;
    cout << "==========================" << endl;
}

void viewUserProfile(SystemData& data) {
    clearScreen();
    cout << "=== MY PROFILE ===" << endl;
    cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    // Find current user in the organizer vector
    Organizer currentUserInfo;
    bool userFound = false;

    for (const auto& user : data.organizer) {
        if (user.userID == data.currentUser) {
            currentUserInfo = user;
            userFound = true;
            break;
        }
    }

    if (!userFound) {
        cout << "Error: Current user information not found!" << endl;
        cout << "This might be a system error. Please try logging in again." << endl;
        return;
    }

    // Display user profile information
    displayUserInfo(currentUserInfo);

    // Show user's event statistics
    cout << "\n=== MY EVENT STATISTICS ===" << endl;
    cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;

    // Count user's registrations by status
    int totalRegistrations = 0;
    int pendingRegistrations = 0;
    int scheduledRegistrations = 0;
    int rejectedRegistrations = 0;

    for (const auto& reg : data.registrations) {
        if (reg.organizer.userID == data.currentUser) {
            totalRegistrations++;
            if (reg.eventStatus == "UNSCHEDULED") pendingRegistrations++;
            else if (reg.eventStatus == "SCHEDULED" || reg.eventStatus == "REGISTERED") scheduledRegistrations++;
            else if (reg.eventStatus == "CANCELLED") rejectedRegistrations++;
        }
    }

    // Count user's bookings by status
    int totalBookings = 0;
    int pendingBookings = 0;
    int confirmedBookings = 0;
    int completedBookings = 0;
    int cancelledBookings = 0;
    double totalSpent = 0.0;

    for (const auto& booking : data.bookings) {
        if (booking.eventReg.organizer.userID == data.currentUser) {
            totalBookings++;
            if (booking.bookingStatus != "Cancelled") {
                totalSpent += booking.finalCost;
            }

            if (booking.bookingStatus == "Pending") pendingBookings++;
            else if (booking.bookingStatus == "Confirmed") confirmedBookings++;
            else if (booking.bookingStatus == "Completed") completedBookings++;
            else if (booking.bookingStatus == "Cancelled") cancelledBookings++;
        }
    }

    // Count user's feedback submissions
    int totalFeedbacks = 0;
    double averageRating = 0.0;
    double totalRatings = 0.0;

    for (const auto& feedback : data.feedbacks) {
        if (feedback.organizerName == currentUserInfo.organizerName) {
            totalFeedbacks++;
            totalRatings += feedback.overallRating;
        }
    }

    if (totalFeedbacks > 0) {
        averageRating = totalRatings / totalFeedbacks;
    }

    // Display statistics
    cout << "\n--- EVENT REGISTRATIONS ---" << endl;
    cout << left << setw(20) << "Total Registrations:" << totalRegistrations << endl;
    cout << left << setw(20) << "Pending:" << pendingRegistrations << endl;
    cout << left << setw(20) << "Approved:" << scheduledRegistrations << endl;
    cout << left << setw(20) << "Rejected:" << rejectedRegistrations << endl;

    cout << "\n--- EVENT BOOKINGS ---" << endl;
    cout << left << setw(20) << "Total Bookings:" << totalBookings << endl;
    cout << left << setw(20) << "Pending:" << pendingBookings << endl;
    cout << left << setw(20) << "Confirmed:" << confirmedBookings << endl;
    cout << left << setw(20) << "Completed:" << completedBookings << endl;
    cout << left << setw(20) << "Cancelled:" << cancelledBookings << endl;
    cout << left << setw(20) << "Total Spent:" << "RM " << fixed << setprecision(2) << totalSpent << endl;

    cout << "\n--- FEEDBACK HISTORY ---" << endl;
    cout << left << setw(20) << "Total Feedbacks:" << totalFeedbacks << endl;
    if (totalFeedbacks > 0) {
        cout << left << setw(20) << "Average Rating:" << fixed << setprecision(1) << averageRating << "/5.0" << endl;
    }

    cout << "\n" << setfill('=') << setw(50) << "=" << setfill(' ') << endl;

    // Profile management options
    cout << "\nProfile Options:" << endl;
    cout << "1. Edit Profile Information" << endl;
    cout << "2. Change Password" << endl;
    cout << "3. View My Event Registrations" << endl;
    cout << "4. View My Event Bookings" << endl;
    cout << "5. Back to Main Menu" << endl;

    int choice = getValidIntegerInput("Enter your choice [1-5]: ", 1, 5);

    switch (choice) {
    case 1:
        editUserProfile(data);
        break;
    case 2:
        changeUserPassword(data);
        break;
    case 3:
        viewMyEventRegistrations(data);
        pauseScreen();
        break;
    case 4:
        viewEventBookings(data);
        pauseScreen();
        break;
    case 5:
        return;
    }
}

void editUserProfile(SystemData& data) {
    clearScreen();
    cout << "=== EDIT PROFILE ===" << endl;
    cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;

    // Find current user index
    int userIndex = -1;
    for (size_t i = 0; i < data.organizer.size(); i++) {
        if (data.organizer[i].userID == data.currentUser) {
            userIndex = static_cast<int>(i);
            break;
        }
    }

    if (userIndex == -1) {
        cout << "Error: User not found!" << endl;
        return;
    }

    cout << "Current Information:" << endl;
    displayUserInfo(data.organizer[userIndex]);

    cout << "\nWhat would you like to edit?" << endl;
    cout << "1. Organizer Name" << endl;
    cout << "2. Age" << endl;
    cout << "3. Contact Number" << endl;
    cout << "4. Email Address" << endl;
    cout << "5. Manufacturer" << endl;
    cout << "6. Position" << endl;
    cout << "7. Cancel" << endl;

    int choice = getValidIntegerInput("Enter your choice [1-5]: ", 1, 5);

    switch (choice) {
    case 1:
        data.organizer[userIndex].organizerName = getValidStringInput("Enter new Organizer Name: ");
        break;
    case 2:
        data.organizer[userIndex].age = getValidIntegerInputWithExit("Enter your age: ", 18, 100);
        break;
    case 3:
        data.organizer[userIndex].organizerContact = getValidPhoneNumber("Enter new Contact Number: ");
        break;
    case 4:
        data.organizer[userIndex].organizerEmail = getValidEmailAddress("Enter new Email Address: ");
        break;
    case 5:
        data.organizer[userIndex].position = getValidStringInput("Enter new Position: ");
        break;
    case 6:
        data.organizer[userIndex].position = getValidStringInput("Enter new Position: ");
        
        break;
    case 7:
        cout << "Edit cancelled." << endl;
        return;
    }

    // Save updated information
    saveUsersToFile(data.organizer);
    cout << "\nProfile updated successfully!" << endl;
    cout << "\nUpdated Information:" << endl;
    displayUserInfo(data.organizer[userIndex]);
}

void changeUserPassword(SystemData& data) {
    clearScreen();
    cout << "=== CHANGE PASSWORD ===" << endl;
    cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;

    // Find current user index
    int userIndex = -1;
    for (size_t i = 0; i < data.organizer.size(); i++) {
        if (data.organizer[i].userID == data.currentUser) {
            userIndex = static_cast<int>(i);
            break;
        }
    }

    if (userIndex == -1) {
        cout << "Error: User not found!" << endl;
        return;
    }

    // Verify current password
    string currentPassword = getValidStringInput("Enter current password: ");
    if (currentPassword != data.organizer[userIndex].password) {
        cout << "Incorrect current password!" << endl;
        return;
    }

    // Get new password
    string newPassword = getValidStringInput("Enter new password: ");
    string confirmPassword = getValidStringInput("Confirm new password: ");

    if (newPassword != confirmPassword) {
        cout << "Password confirmation does not match!" << endl;
        return;
    }

    if (newPassword == data.organizer[userIndex].password) {
        cout << "New password cannot be the same as current password!" << endl;
        return;
    }

    // Update password
    strcpy_s(data.organizer[userIndex].password, newPassword.c_str());
    saveUsersToFile(data.organizer);

    cout << "Password changed successfully!" << endl;
}

// Helper function to view user's registrations
void viewMyEventRegistrations(const SystemData& data) {
    clearScreen();
    cout << "=== MY EVENT REGISTRATIONS ===" << endl;

    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    if (data.registrations.empty()) {
        cout << "No event registrations found." << endl;
        return;
    }

    // Filter registrations for current user
    vector<EventRegistration> userRegistrations;
    for (const auto& reg : data.registrations) {
        if (reg.organizer.userID == data.currentUser) {
            userRegistrations.push_back(reg);
        }
    }

    if (userRegistrations.empty()) {
        cout << "No event registrations found for your account." << endl;
        return;
    }

    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;
    cout << left
        << setw(8) << "Event ID"
        << setw(20) << "Event Title"
        << setw(15) << "Manufacturer"
        << setw(8) << "Guests"
        << setw(12) << "Budget (RM)"
        << setw(12) << "Status" << endl;
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;

    for (const auto& reg : userRegistrations) {
        cout << left
            << setw(8) << reg.eventID
            << setw(20) << reg.eventTitle.substr(0, 19)
            << setw(15) << reg.manufacturer.substr(0, 14)
            << setw(8) << reg.expectedGuests
            << "RM " << setw(9) << fixed << setprecision(2) << reg.estimatedBudget
            << setw(12) << reg.eventStatus << endl;
    }
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;
    cout << "Total Your Registrations: " << userRegistrations.size() << endl;
}