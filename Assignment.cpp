#include <iostream>
#include <string>
#include <iomanip>
#include "system_data.h"
#include "utility_fun.h"
#include "input_validation.h"
#include "file_operation.h"
#include "user.h"
#include "event_registration.h"
#include "event_booking.h"   
#include "event_monitoring.h"
#include "feedback.h"            

using namespace std;

// Function declarations
void mainSystemMenu(SystemData& data);
string getCurrentUserName(const SystemData& data);

// Main function
int main() {
    SystemData data;
    
    bool exitProgram = false, loginValidation = false;

    while (!exitProgram) {
        clearScreen();
        displayLogo();
        cout << "======= USER MENU =======" << endl;
        cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;
        cout << "1. Sign Up" << endl;
        cout << "2. Log In" << endl;
        cout << "3. Exit" << endl;
        cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;

        int choice = getValidIntegerInput("Enter your choice [1-3]: ", 1, 3);

        switch (choice) {
        case 1:
            signUp(data);
            pauseScreen();
            break;
        case 2:
            loginUser(data, &loginValidation);
            if (loginValidation) {  // If login successful
                cout << "Login successful! Loading your data..." << endl;
                pauseScreen();

                // Enter main system - stay here until user chooses to logout
                mainSystemMenu(data);

                // After user logs out from main system, clear current user
                data.currentUser.clear();

                // Save data when user logs out
                saveDataToFiles(data);
                cout << "Logged out successfully!" << endl;
            }
            else {
                pauseScreen();
            }
            break;
        case 3:
            // Save data before exiting
            saveDataToFiles(data);
            cout << "Thank you for using the system!" << endl;
            exitProgram = true;
            break;
        }
    }

    return 0;
}

void mainSystemMenu(SystemData& data) {
    bool logoutRequested = false;

    while (!logoutRequested) {
        clearScreen();
        displayLogo();
        cout << "Welcome, " << getCurrentUserName(data) << "!" << endl;
        cout << "======= MAIN SYSTEM MENU =======" << endl;
        cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;
        cout << "1. Event Registration" << endl;
        cout << "2. Event Booking" << endl;
        cout << "3. Event Monitoring" << endl;
        cout << "4. Event Payment" << endl;
        cout << "5. Feedback" << endl;
        cout << "6. View My Profile" << endl;
        cout << "7. Save Data & Logout" << endl;
        cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;

        int choice = getValidIntegerInput("Enter your choice [1-7]: ", 1, 7);

        switch (choice) {
        case 1:
            eventRegistrationMenu(data);  
            break;
        case 2:
            eventBookingMenu(data);
            break;
        case 3:
            eventMonitoringMenu(data);
            break;
        case 4:
            eventPaymentMenu(data);
        case 5:
            feedbackMenu(data);
            break;
        case 6:
            viewUserProfile(data);
            pauseScreen();
            break;
        case 7:
            saveDataToFiles(data);
            saveRegistrationsToFile(data.registrations);
            cout << "Data saved successfully!" << endl;
            cout << "Logging out..." << endl;
            logoutRequested = true;
            pauseScreen();
            break;
        }
    }
}