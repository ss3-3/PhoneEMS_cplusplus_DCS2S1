#include "input_validation.h"
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
#include "timeslot.h"
#include "utility_fun.h"

// Input validation functions
string toUpperCase(const string& str) {
    string result = str;
    for (size_t i = 0; i < result.length(); i++) {
        if (result[i] >= 'a' && result[i] <= 'z') {
            result[i] = result[i] - 32; // Convert to uppercase
        }
    }
    return result;
}

//prompt user to exit some function if user want back to the previous function
bool getExit() {
    char choice;
    bool validInput = false;

    cout << "\n=== RETURN CONFIRMATION ===" << endl;
    cout << "Are you sure you want to return to the previous menu?" << endl;

    while (!validInput) {
        cout << "Enter 'Y' for Yes or 'N' for No: ";
        cin >> choice;
        cin.ignore(); // Clear the input buffer

        // Convert the user input to uppercase
        choice = toupper(choice);

        if (choice == 'Y') {
            cout << "Returning to previous menu..." << endl;
            validInput = true;
            return true; // User confirms return to previous menu
        }
        else if (choice == 'N') {
            cout << "Continuing in current menu..." << endl;
            validInput = true;
            return false; // User cancels return, stays in current menu
        }
        else {
            cout << "Invalid input. Please enter 'Y' for Yes or 'N' for No." << endl;
        }
    }

    return false;
}

int getValidIntegerInput(const string& prompt, int minVal, int maxVal) {
    int value;
    string input;
    bool firstAttempt = true;

    while (true) {
        if (!firstAttempt) {
            cout << "Invalid input. Please try again." << endl;
        }
        firstAttempt = false;

        cout << prompt;
        getline(cin, input);

        // Check for empty input
        if (input.empty()) {
            cout << "Error: Input cannot be empty." << endl;
            continue;
        }

        // Remove leading and trailing whitespace
        size_t start = input.find_first_not_of(" \t");
        if (start != string::npos) {
            size_t end = input.find_last_not_of(" \t");
            input = input.substr(start, end - start + 1);
        }

        // Check if input contains only digits (and optional minus sign for negative numbers)
        bool isValid = true;
        size_t startPos = 0;

        if (!input.empty() && input[0] == '-') {
            startPos = 1;
            if (input.length() == 1) {
                isValid = false; // Just a minus sign
            }
        }

        for (size_t i = startPos; i < input.length() && isValid; i++) {
            if (input[i] < '0' || input[i] > '9') {
                isValid = false;
            }
        }

        if (!isValid) {
            cout << "Error: Please enter a valid whole number." << endl;
            continue;
        }

        // Convert to integer
        stringstream ss(input);
        if (ss >> value && ss.eof()) {
            if (value >= minVal && value <= maxVal) {
                return value;
            }
            else {
                cout << "Error: Number must be between " << minVal << " and " << maxVal << "." << endl;
            }
        }
        else {
            cout << "Error: Please enter a valid number." << endl;
        }
    }
}

int getValidIntegerInputWithExit(const string& prompt, int minVal, int maxVal) {
    int value;
    string input;
    bool firstAttempt = true;

    while (true) {
        if (!firstAttempt) {
            cout << "Invalid input. Please try again." << endl;
        }
        firstAttempt = false;

        cout << prompt;
        getline(cin, input);

        //Check whether user enter '0' or not
        if (input == "0")
        {
            if (getExit())
            {
                return -1; //empty value string
            }
            continue;
        }

        // Check for empty input
        if (input.empty()) {
            cout << "Error: Input cannot be empty." << endl;
            continue;
        }

        // Remove leading and trailing whitespace
        size_t start = input.find_first_not_of(" \t");
        if (start != string::npos) {
            size_t end = input.find_last_not_of(" \t");
            input = input.substr(start, end - start + 1);
        }

        // Check if input contains only digits (and optional minus sign for negative numbers)
        bool isValid = true;
        size_t startPos = 0;

        if (!input.empty() && input[0] == '-') {
            startPos = 1;
            if (input.length() == 1) {
                isValid = false; // Just a minus sign
            }
        }

        for (size_t i = startPos; i < input.length() && isValid; i++) {
            if (input[i] < '0' || input[i] > '9') {
                isValid = false;
            }
        }

        if (!isValid) {
            cout << "Error: Please enter a valid whole number." << endl;
            continue;
        }

        // Convert to integer
        stringstream ss(input);
        if (ss >> value && ss.eof()) {
            if (value >= minVal && value <= maxVal) {
                return value;
            }
            else {
                cout << "Error: Number must be between " << minVal << " and " << maxVal << "." << endl;
            }
        }
        else {
            cout << "Error: Please enter a valid number." << endl;
        }
    }
}

string getValidStringInput(const string& prompt, int minLength) {
    string input;
    bool firstAttempt = true;

    while (true) {
        if (!firstAttempt) {
            cout << "Invalid input. Please try again." << endl;
        }
        firstAttempt = false;

        cout << prompt;
        getline(cin, input);

        // Remove leading and trailing whitespace
        size_t start = input.find_first_not_of(" \t");
        if (start != string::npos) {
            size_t end = input.find_last_not_of(" \t");
            input = input.substr(start, end - start + 1);
        }
        else {
            input = ""; // String contains only whitespace
        }

        // Check for empty input after trimming
        if (input.empty()) {
            cout << "Error: Input cannot be empty or contain only spaces." << endl;
            continue;
        }

        // Check minimum length
        if ((int)input.length() < minLength) {
            cout << "Error: Input must be at least " << minLength << " characters long." << endl;
            continue;
        }

        // Check if input contains at least one alphabetic character (for names, etc.)
        bool hasAlpha = false;
        for (size_t i = 0; i < input.length(); i++) {
            if ((input[i] >= 'A' && input[i] <= 'Z') ||
                (input[i] >= 'a' && input[i] <= 'z')) {
                hasAlpha = true;
                break;
            }
        }

        if (!hasAlpha) {
            cout << "Error: Input must contain at least one letter." << endl;
            continue;
        }

        return input;
    }
}

string getValidStringInputWithExit(const string& prompt, int minLength) { //default min length is 1 
    string input;

    while (true) {

        cout << prompt << "(Enter '0' to return previous menu)" << endl;
        getline(cin, input);

        //Check whether user enter '0' or not
        if (input == "0")
        {
            if (getExit())
            {
                return ""; //empty value string
            }
            continue;
        }

        // Remove leading and trailing whitespace
        size_t start = input.find_first_not_of(" \t");
        if (start != string::npos) {
            size_t end = input.find_last_not_of(" \t");
            input = input.substr(start, end - start + 1);
        }
        else {
            input = ""; // String contains only whitespace
        }

        // Check for empty input after trimming
        if (input.empty()) {
            cout << "Error: Input cannot be empty or contain only spaces." << endl;
            continue;
        }

        // Check minimum length
        if ((int)input.length() < minLength) {
            cout << "Error: Input must be at least " << minLength << " characters long." << endl;
            continue;
        }

        // Check if input contains at least one alphabetic character (for names, etc.)
        bool hasAlpha = false;
        for (size_t i = 0; i < input.length(); i++) {
            if ((input[i] >= 'A' && input[i] <= 'Z') ||
                (input[i] >= 'a' && input[i] <= 'z')) {
                hasAlpha = true;
                break;
            }
        }

        if (!hasAlpha) {
            cout << "Error: Input must contain at least one letter." << endl;
            continue;
        }

        return input;
    }
}

string getValidStringInputOrKeepCurrent(const string& prompt, int minLength) {
    string input;

    cout << prompt << " (Enter '0' to return to previous menu, press Enter to keep current value): ";
    getline(cin, input);

    // check if user enter 0
    if (input == "0") {
        if (getExit()) {
            return "EXIT_REQUESTED"; // exit message for system 
        }
        // recursive if user no perform exit action - 'n'
        return getValidStringInputOrKeepCurrent(prompt, minLength);
    }

    // Remove leading and trailing whitespace
    size_t start = input.find_first_not_of(" \t");
    if (start != string::npos) {
        size_t end = input.find_last_not_of(" \t");
        input = input.substr(start, end - start + 1);
    }
    else {
        input = ""; // String contains only whitespace
    }

    // if user press 'Enter' key
    if (input.empty()) {
        return "KEEP_CURRENT"; // keep previous data for system
    }

    // validation
    bool isValid = true;

    // Check minimum length
    if ((int)input.length() < minLength) {
        cout << "Error: Input must be at least " << minLength << " characters long." << endl;
        isValid = false;
    }

    // Check if input contains at least one alphabetic character
    bool hasAlpha = false;
    for (size_t i = 0; i < input.length(); i++) {
        if ((input[i] >= 'A' && input[i] <= 'Z') ||
            (input[i] >= 'a' && input[i] <= 'z')) {
            hasAlpha = true;
            break;
        }
    }
    if (!hasAlpha) {
        cout << "Error: Input must contain at least one letter." << endl;
        isValid = false;
    }

    if (!isValid) {
        return getValidStringInputOrKeepCurrent(prompt, minLength);
    }

    return input;
}

double getValidDoubleInput(const string& prompt, double minVal) {
    double value;
    string input;
    bool firstAttempt = true;

    while (true) {
        if (!firstAttempt) {
            cout << "Invalid input. Please try again." << endl;
        }
        firstAttempt = false;

        cout << prompt;
        getline(cin, input);

        // Check for empty input
        if (input.empty()) {
            cout << "Error: Input cannot be empty." << endl;
            continue;
        }

        // Remove leading and trailing whitespace
        size_t start = input.find_first_not_of(" \t");
        if (start != string::npos) {
            size_t end = input.find_last_not_of(" \t");
            input = input.substr(start, end - start + 1);
        }

        // Validate decimal number format
        bool isValid = true;
        bool hasDecimalPoint = false;
        size_t startPos = 0;

        // Check for negative sign
        if (!input.empty() && input[0] == '-') {
            startPos = 1;
            if (input.length() == 1) {
                isValid = false; // Just a minus sign
            }
        }

        // Check each character
        for (size_t i = startPos; i < input.length() && isValid; i++) {
            if (input[i] == '.') {
                if (hasDecimalPoint) {
                    isValid = false; // Multiple decimal points
                }
                else {
                    hasDecimalPoint = true;
                }
            }
            else if (input[i] < '0' || input[i] > '9') {
                isValid = false;
            }
        }

        // Check if it ends with decimal point
        if (isValid && !input.empty() && input[input.length() - 1] == '.') {
            isValid = false;
        }

        if (!isValid) {
            cout << "Error: Please enter a valid decimal number (e.g., 123.45)." << endl;
            continue;
        }

        // Convert to double
        stringstream ss(input);
        if (ss >> value && ss.eof()) {
            if (value >= minVal) {
                return value;
            }
            else {
                cout << "Error: Value must be at least " << fixed << setprecision(2) << minVal << "." << endl;
            }
        }
        else {
            cout << "Error: Please enter a valid number." << endl;
        }
    }
}

double getValidDoubleInputWithExit(const string& prompt, double minVal) {
    double value;
    string input;
    bool firstAttempt = true;

    while (true) {
        if (!firstAttempt) {
            cout << "Invalid input. Please try again." << endl;
        }
        firstAttempt = false;

        cout << prompt;
        getline(cin, input);


        //Check whether user enter '0' or not
        if (input == "0")
        {
            if (getExit())
            {
                return -1.0; //empty value string
            }
            continue;
        }

        // Check for empty input
        if (input.empty()) {
            cout << "Error: Input cannot be empty." << endl;
            continue;
        }

        // Remove leading and trailing whitespace
        size_t start = input.find_first_not_of(" \t");
        if (start != string::npos) {
            size_t end = input.find_last_not_of(" \t");
            input = input.substr(start, end - start + 1);
        }

        // Validate decimal number format
        bool isValid = true;
        bool hasDecimalPoint = false;
        size_t startPos = 0;

        // Check for negative sign
        if (!input.empty() && input[0] == '-') {
            startPos = 1;
            if (input.length() == 1) {
                isValid = false; // Just a minus sign
            }
        }

        // Check each character
        for (size_t i = startPos; i < input.length() && isValid; i++) {
            if (input[i] == '.') {
                if (hasDecimalPoint) {
                    isValid = false; // Multiple decimal points
                }
                else {
                    hasDecimalPoint = true;
                }
            }
            else if (input[i] < '0' || input[i] > '9') {
                isValid = false;
            }
        }

        // Check if it ends with decimal point
        if (isValid && !input.empty() && input[input.length() - 1] == '.') {
            isValid = false;
        }

        if (!isValid) {
            cout << "Error: Please enter a valid decimal number (e.g., 123.45)." << endl;
            continue;
        }

        // Convert to double
        stringstream ss(input);
        if (ss >> value && ss.eof()) {
            if (value >= minVal) {
                return value;
            }
            else {
                cout << "The minimum cost is " << fixed << setprecision(2) << minVal << ". Please re-enter again." << endl;
            }
        }
        else {
            cout << "The minimum cost is " << fixed << setprecision(2) << minVal << ". Please re-enter again." << endl;
        }

    }
}

char getValidCharInput(const string& prompt, const vector<char>& validChars) {
    string input;
    bool firstAttempt = true;

    while (true) {
        if (!firstAttempt) {
            cout << "Invalid choice. Please try again." << endl;
        }
        firstAttempt = false;

        cout << prompt;
        getline(cin, input);

        // Remove whitespace
        size_t start = input.find_first_not_of(" \t");
        if (start != string::npos) {
            size_t end = input.find_last_not_of(" \t");
            input = input.substr(start, end - start + 1);
        }
        else {
            input = "";
        }

        if (input.empty()) {
            cout << "Error: Please enter a choice." << endl;
            cout << "Valid options: ";
            for (size_t i = 0; i < validChars.size(); i++) {
                cout << validChars[i];
                if (i < validChars.size() - 1) cout << "/";
            }
            cout << endl;
            continue;
        }

        if (input.length() != 1) {
            cout << "Error: Please enter only one character." << endl;
            cout << "Valid options: ";
            for (size_t i = 0; i < validChars.size(); i++) {
                cout << validChars[i];
                if (i < validChars.size() - 1) cout << "/";
            }
            cout << endl;
            continue;
        }

        char inputChar = input[0];

        // Check against valid characters (case-insensitive)
        for (size_t i = 0; i < validChars.size(); i++) {
            char validUpper = validChars[i];
            char validLower = validChars[i];

            // Convert to lowercase if it's uppercase
            if (validUpper >= 'A' && validUpper <= 'Z') {
                validLower = validUpper + 32;
            }
            // Convert to uppercase if it's lowercase
            else if (validUpper >= 'a' && validUpper <= 'z') {
                validLower = validUpper;
                validUpper = validUpper - 32;
            }

            if (inputChar == validUpper || inputChar == validLower) {
                return validUpper; // Return uppercase version for consistency
            }
        }

        cout << "Error: Invalid choice '" << inputChar << "'." << endl;
        cout << "Valid options: ";
        for (size_t i = 0; i < validChars.size(); i++) {
            cout << validChars[i];
            if (i < validChars.size() - 1) cout << "/";
        }
        cout << endl;
    }
}

Date getValidDateInput() {
    Date inputDate;
    Date currentDate = getCurrentDate();
    int minYear = 2025;
    int maxYear = 2040;
    bool firstAttempt = true;

    cout << "\n=== DATE INPUT ===" << endl;
    cout << "Current date: " << currentDate.toString() << endl;
    cout << "Note: Event date must be today or in the future." << endl;

    // Year input with validation
    while (true) {
        if (!firstAttempt) {
            cout << "Please enter a valid year." << endl;
        }
        firstAttempt = false;

        inputDate.year = getValidIntegerInput(
            "Enter Year (" + to_string(minYear) + "-" + to_string(maxYear) + "): ",
            minYear, maxYear
        );

        if (inputDate.year >= currentDate.year) {
            break;
        }
        else {
            cout << "Error: Year cannot be in the past." << endl;
        }
    }

    // Month input with validation
    firstAttempt = true;
    while (true) {
        if (!firstAttempt) {
            cout << "Please enter a valid month." << endl;
        }
        firstAttempt = false;

        int minMonth = (inputDate.year == currentDate.year) ? currentDate.month : 1;
        inputDate.month = getValidIntegerInput(
            "Enter Month (" + to_string(minMonth) + "-12): ",
            minMonth, 12
        );

        if (inputDate.year > currentDate.year || inputDate.month >= currentDate.month) {
            break;
        }
        else {
            cout << "Error: Month cannot be in the past for current year." << endl;
        }
    }

    // Day input with comprehensive validation
    int daysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // Check for leap year
    if (inputDate.month == 2) {
        if ((inputDate.year % 4 == 0 && inputDate.year % 100 != 0) || (inputDate.year % 400 == 0)) {
            daysInMonth[1] = 29;
        }
    }

    int maxDays = daysInMonth[inputDate.month - 1];
    int minDay = (inputDate.year == currentDate.year && inputDate.month == currentDate.month)
        ? currentDate.day : 1;

    firstAttempt = true;
    while (true) {
        if (!firstAttempt) {
            cout << "Please enter a valid day." << endl;
        }
        firstAttempt = false;

        cout << "Days in " << inputDate.month << "/" << inputDate.year << ": " << maxDays << endl;
        inputDate.day = getValidIntegerInput(
            "Enter Day (" + to_string(minDay) + "-" + to_string(maxDays) + "): ",
            minDay, maxDays
        );

        // Additional validation for current year and month
        if (inputDate.year == currentDate.year &&
            inputDate.month == currentDate.month &&
            inputDate.day < currentDate.day) {
            cout << "Error: Date cannot be in the past." << endl;
            continue;
        }

        break;
    }

    cout << "Selected date: " << inputDate.toString() << endl;
    return inputDate;
}

string getValidPhoneNumber(const string& prompt) {
    string input;
    bool firstAttempt = true;

    while (true) {
        if (!firstAttempt) {
            cout << "Invalid phone number format. Please try again." << endl;
        }
        firstAttempt = false;

        //Check whether user enter '0' or not
        if (input == "0")
        {
            if (getExit())
            {
                return ""; //empty value string
            }
            continue;
        }

        cout << prompt;
        getline(cin, input);

        // Remove whitespace and dashes
        string cleanNumber = "";
        for (size_t i = 0; i < input.length(); i++) {
            if (input[i] != ' ' && input[i] != '-' && input[i] != '(' && input[i] != ')') {
                cleanNumber += input[i];
            }
        }

        if (cleanNumber.empty()) {
            cout << "Error: Phone number cannot be empty." << endl;
            continue;
        }

        // Check if all characters are digits
        bool allDigits = true;
        for (size_t i = 0; i < cleanNumber.length(); i++) {
            if (cleanNumber[i] < '0' || cleanNumber[i] > '9') {
                allDigits = false;
                break;
            }
        }

        if (!allDigits) {
            cout << "Error: Phone number can only contain digits, spaces, dashes, and parentheses." << endl;
            cout << "Example formats: 03-1234-5678, (03) 1234-5678, 0312345678" << endl;
            continue;
        }

        // Check length (Malaysian phone numbers are typically 10-11 digits)
        if (cleanNumber.length() < 10 || cleanNumber.length() > 11) {
            cout << "Error: Phone number must be 10-11 digits long." << endl;
            cout << "Example: 03-1234-5678 (landline) or 012-345-6789 (mobile)" << endl;
            continue;
        }

        // Format the number nicely
        if (cleanNumber.length() == 11 && (cleanNumber.substr(0, 2) == "01")) {
            // Mobile format: 012-345-6789
            return cleanNumber.substr(0, 3) + "-" + cleanNumber.substr(3, 3) + "-" + cleanNumber.substr(6, 4);
        }
        else if (cleanNumber.length() == 10 && cleanNumber.substr(0, 2) == "03") {
            // Landline format: 03-1234-5678
            return cleanNumber.substr(0, 2) + "-" + cleanNumber.substr(2, 4) + "-" + cleanNumber.substr(6, 4);
        }
        else {
            // Generic format for other numbers
            return cleanNumber;
        }
    }
}

string getValidEmailAddress(const string& prompt) {
    string input;
    regex pattern("^[a-z0-9-]+@(?:\\w+\.){1,5}(com|my)$");

    while (true) {
        cout << prompt;
        getline(cin, input);

        //Check whether user enter '0' or not
        if (input == "0")
        {
            if (getExit())
            {
                return ""; //empty value string
            }
            continue;
        }

        if (input.empty()) {
            cout << "Error: Email address cannot be empty." << endl;
            continue;
        }

        if (regex_match(input, pattern))
        {
            return input;
        }
        else
        {
            cout << "Error: Invalid email format. Kindly try again!!! (must small capital letter with @XXX.com or @XXX.my): " << endl;
        }
    }
}

string getValidEventStatus(const string& prompt) {
    string validStatuses[] = { "REGISTERED", "APPROVED", "REJECTED" };
    int numStatuses = 3;
    string input;
    bool firstAttempt = true;

    cout << "Valid status options:" << endl;
    for (int i = 0; i < numStatuses; i++) {
        cout << (i + 1) << ". " << validStatuses[i] << endl;
    }

    while (true) {
        if (!firstAttempt) {
            cout << "Invalid status. Please try again." << endl;
        }
        firstAttempt = false;

        cout << prompt;
        getline(cin, input);

        // Remove whitespace and convert to uppercase
        size_t start = input.find_first_not_of(" \t");
        if (start != string::npos) {
            size_t end = input.find_last_not_of(" \t");
            input = input.substr(start, end - start + 1);
        }
        else {
            input = "";
        }

        if (input.empty()) {
            cout << "Error: Status cannot be empty." << endl;
            continue;
        }

        string upperInput = toUpperCase(input);

        // Check if input matches any valid status
        for (int i = 0; i < numStatuses; i++) {
            if (upperInput == validStatuses[i]) {
                return validStatuses[i]; // Return the properly formatted status
            }
        }

        cout << "Error: Invalid status '" << input << "'." << endl;
        cout << "Valid options: ";
        for (int i = 0; i < numStatuses; i++) {
            cout << validStatuses[i];
            if (i < numStatuses - 1) cout << ", ";
        }
        cout << endl;
    }
}

// Validate feedback rating input
int getValidRating(const string& prompt) {
    return getValidIntegerInput(prompt + " [1-5]: ", 1, 5);
}

//exit or back to main menu information
void confirmExit() {
    cout << " === REGISTRATION CANCELLED ===" << endl;
    cout << "All entered information has been discarded. " << endl;
    cout << "Returning to main menu ..." << endl;
    cout << endl;
}