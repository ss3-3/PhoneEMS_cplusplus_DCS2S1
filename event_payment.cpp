#include "event_payment.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <map>
#include "system_data.h"
#include "utility_fun.h"
#include "input_validation.h"
#include "file_operation.h"

using namespace std;

void eventPaymentMenu(SystemData& data) {
    bool exitMenu = false;

    while (!exitMenu) {
        clearScreen();
        displayLogo();
        cout << "=== MY EVENT PAYMENT ===" << endl;
        cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;
        cout << "1. Make Payment" << endl;
        cout << "2. View Payment History" << endl;
        cout << "3. Process Refund" << endl;
        cout << "4. View Payment Statistics" << endl;
        cout << "5. Back to Main Menu" << endl;
        cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;

        int choice = getValidIntegerInput("Enter your choice [1-5]: ", 1, 5);

        switch (choice) {
        case 1:
            makePayment(data);
            break;
        case 2:
            viewPaymentHistory(data);
            break;
        case 3:
            processRefund(data);
            break;
        case 4:
            viewPaymentStatistics(data);
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

void makePayment(SystemData& data) {
    clearScreen();
    cout << "=== MAKE PAYMENT ===" << endl;
    cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    // Filter user's confirmed bookings that haven't been paid
    vector<EventBooking> unpaidBookings;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);

    for (const auto& booking : data.bookings) {
        string normalizedBookingUser = normalizeUserID(booking.eventReg.organizer.userID);
        if (normalizedBookingUser == normalizedCurrentUser &&
            (booking.bookingStatus == "Confirmed" || booking.bookingStatus == "Pending")) {

            // Check if payment already exists for this booking
            bool alreadyPaid = false;
            for (const auto& payment : data.payments) {
                if (payment.bookingID == booking.bookingID &&
                    payment.paymentStatus == "Completed") {
                    alreadyPaid = true;
                    break;
                }
            }

            if (!alreadyPaid) {
                unpaidBookings.push_back(booking);
            }
        }
    }

    if (unpaidBookings.empty()) {
        cout << "No unpaid bookings found!" << endl;
        cout << "Make sure you have confirmed bookings that require payment." << endl;
        return;
    }

    // Display unpaid bookings
    cout << "=== YOUR UNPAID BOOKINGS ===" << endl;
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;
    cout << left << setw(4) << "No."
        << setw(10) << "Book ID"
        << setw(20) << "Event Title"
        << setw(12) << "Date"
        << setw(15) << "Venue"
        << setw(15) << "Amount (RM)"
        << setw(12) << "Status" << endl;
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;

    for (size_t i = 0; i < unpaidBookings.size(); i++) {
        cout << left << setw(4) << (i + 1)
            << setw(10) << unpaidBookings[i].bookingID
            << setw(20) << unpaidBookings[i].eventReg.eventTitle.substr(0, 19)
            << setw(12) << unpaidBookings[i].eventDate.toString()
            << setw(15) << unpaidBookings[i].venue.venueName.substr(0, 14)
            << "RM " << setw(12) << fixed << setprecision(2) << unpaidBookings[i].finalCost
            << setw(12) << unpaidBookings[i].bookingStatus << endl;
    }
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;

    int bookingChoice = getValidIntegerInput("Select booking to pay for: ", 1, static_cast<int>(unpaidBookings.size()));
    EventBooking selectedBooking = unpaidBookings[bookingChoice - 1];

    // Create payment record
    Payment newPayment;
    newPayment.paymentID = generatePaymentID(data.payments);
    newPayment.bookingID = selectedBooking.bookingID;
    newPayment.amount = selectedBooking.finalCost;
    newPayment.paymentDate = getCurrentDate();
    newPayment.paymentStatus = "Completed"; // Simplified - payment is immediately completed

    cout << "\n=== PAYMENT DETAILS ===" << endl;
    cout << "Payment ID: " << newPayment.paymentID << endl;
    cout << "Booking ID: " << newPayment.bookingID << endl;
    cout << "Event: " << selectedBooking.eventReg.eventTitle << endl;
    cout << "Amount: RM " << fixed << setprecision(2) << newPayment.amount << endl;
    cout << "Date: " << newPayment.paymentDate.toString() << endl;

    // Payment method selection
    cout << "\n=== PAYMENT METHODS ===" << endl;
    cout << "1. Credit Card" << endl;
    cout << "2. Debit Card" << endl;
    cout << "3. Bank Transfer" << endl;
    cout << "4. Cash" << endl;

    int methodChoice = getValidIntegerInput("Select payment method [1-4]: ", 1, 4);

    switch (methodChoice) {
    case 1: newPayment.paymentMethod = "Credit Card"; break;
    case 2: newPayment.paymentMethod = "Debit Card"; break;
    case 3: newPayment.paymentMethod = "Bank Transfer"; break;
    case 4: newPayment.paymentMethod = "Cash"; break;
    }

    // Get payment details based on method
    if (methodChoice == 1 || methodChoice == 2) {
        cout << "\n=== CARD DETAILS ===" << endl;
        cout << "Note: Card information is for simulation only." << endl;

        string fullCardNumber;
        while (true) {
            cout << "Enter card number (16 digits): ";
            getline(cin, fullCardNumber);

            // Remove spaces
            string cleanCard = "";
            for (char c : fullCardNumber) {
                if (c != ' ') cleanCard += c;
            }

            if (cleanCard.length() == 16) {
                bool allDigits = true;
                for (char c : cleanCard) {
                    if (c < '0' || c > '9') {
                        allDigits = false;
                        break;
                    }
                }
                if (allDigits) {
                    // Store only last 4 digits
                    newPayment.cardNumber = cleanCard.substr(12, 4);
                    break;
                }
            }
            cout << "Invalid card number. Please enter 16 digits." << endl;
        }

        newPayment.cardHolderName = getValidStringInput("Enter cardholder name: ", 2);

        cout << "Enter expiry date (MM/YY): ";
        string expiryDate;
        getline(cin, expiryDate);

        cout << "Enter CVV (3 digits): ";
        string cvv;
        getline(cin, cvv);

        cout << "\nProcessing card payment..." << endl;
    }

    newPayment.transactionReference = generateTransactionReference();

    // Update booking status to confirmed
    for (auto& booking : data.bookings) {
        if (booking.bookingID == selectedBooking.bookingID) {
            booking.bookingStatus = "Confirmed";
            break;
        }
    }

    data.payments.push_back(newPayment);

    cout << "\n=== PAYMENT SUCCESSFUL ===" << endl;
    cout << "Payment ID: " << newPayment.paymentID << endl;
    cout << "Transaction Reference: " << newPayment.transactionReference << endl;
    cout << "Amount Paid: RM " << fixed << setprecision(2) << newPayment.amount << endl;
    cout << "Payment Method: " << newPayment.paymentMethod << endl;
    cout << "Status: " << newPayment.paymentStatus << endl;

    if (!newPayment.cardNumber.empty()) {
        cout << "Card Used: ****-****-****-" << newPayment.cardNumber << endl;
    }

    cout << "\nYour booking is now confirmed!" << endl;

    // Save payment data
    savePaymentsToFile(data.payments);
    saveBookingsToFile(data.bookings);
}

void viewPaymentHistory(const SystemData& data) {
    clearScreen();
    cout << "=== PAYMENT HISTORY ===" << endl;

    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    if (data.payments.empty()) {
        cout << "No payment records found." << endl;
        return;
    }

    // Filter payments for current user
    vector<Payment> userPayments;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);

    for (const auto& payment : data.payments) {
        // Find the corresponding booking to verify ownership
        for (const auto& booking : data.bookings) {
            if (booking.bookingID == payment.bookingID) {
                string normalizedBookingUser = normalizeUserID(booking.eventReg.organizer.userID);
                if (normalizedBookingUser == normalizedCurrentUser) {
                    userPayments.push_back(payment);
                    break;
                }
            }
        }
    }

    if (userPayments.empty()) {
        cout << "No payment history found for your account." << endl;
        return;
    }

    cout << setfill('=') << setw(130) << "=" << setfill(' ') << endl;
    cout << left << setw(12) << "Payment ID"
        << setw(12) << "Booking ID"
        << setw(12) << "Amount"
        << setw(12) << "Date"
        << setw(15) << "Method"
        << setw(12) << "Status"
        << setw(20) << "Reference"
        << setw(12) << "Card" << endl;
    cout << setfill('=') << setw(130) << "=" << setfill(' ') << endl;

    double totalPaid = 0.0;
    for (const auto& payment : userPayments) {
        cout << left << setw(12) << payment.paymentID
            << setw(12) << payment.bookingID
            << "RM " << setw(9) << fixed << setprecision(2) << payment.amount
            << setw(12) << payment.paymentDate.toString()
            << setw(15) << payment.paymentMethod.substr(0, 14)
            << setw(12) << payment.paymentStatus
            << setw(20) << payment.transactionReference.substr(0, 19)
            << setw(12) << (payment.cardNumber.empty() ? "N/A" : "****" + payment.cardNumber) << endl;

        if (payment.paymentStatus == "Completed") {
            totalPaid += payment.amount;
        }
    }

    cout << setfill('=') << setw(130) << "=" << setfill(' ') << endl;
    cout << "Total Payments: " << userPayments.size() << endl;
    cout << "Total Amount Paid: RM " << fixed << setprecision(2) << totalPaid << endl;
}

void processRefund(SystemData& data) {
    clearScreen();
    cout << "=== PROCESS REFUND ===" << endl;

    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    // Filter completed payments for current user
    vector<Payment> refundablePayments;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);

    for (const auto& payment : data.payments) {
        if (payment.paymentStatus == "Completed") {
            // Find the corresponding booking to verify ownership and check if cancelled
            for (const auto& booking : data.bookings) {
                if (booking.bookingID == payment.bookingID) {
                    string normalizedBookingUser = normalizeUserID(booking.eventReg.organizer.userID);
                    if (normalizedBookingUser == normalizedCurrentUser &&
                        booking.bookingStatus == "Cancelled") {
                        refundablePayments.push_back(payment);
                        break;
                    }
                }
            }
        }
    }

    if (refundablePayments.empty()) {
        cout << "No refundable payments found." << endl;
        cout << "Only cancelled bookings with completed payments can be refunded." << endl;
        return;
    }

    cout << "=== REFUNDABLE PAYMENTS ===" << endl;
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;
    cout << left << setw(4) << "No."
        << setw(12) << "Payment ID"
        << setw(12) << "Booking ID"
        << setw(15) << "Amount"
        << setw(15) << "Method"
        << setw(20) << "Reference" << endl;
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;

    for (size_t i = 0; i < refundablePayments.size(); i++) {
        cout << left << setw(4) << (i + 1)
            << setw(12) << refundablePayments[i].paymentID
            << setw(12) << refundablePayments[i].bookingID
            << "RM " << setw(12) << fixed << setprecision(2) << refundablePayments[i].amount
            << setw(15) << refundablePayments[i].paymentMethod.substr(0, 14)
            << setw(20) << refundablePayments[i].transactionReference << endl;
    }
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;

    int refundChoice = getValidIntegerInput("Select payment to refund: ", 1, static_cast<int>(refundablePayments.size()));
    Payment selectedPayment = refundablePayments[refundChoice - 1];

    cout << "\n=== REFUND DETAILS ===" << endl;
    cout << "Payment ID: " << selectedPayment.paymentID << endl;
    cout << "Amount to refund: RM " << fixed << setprecision(2) << selectedPayment.amount << endl;
    cout << "Original payment method: " << selectedPayment.paymentMethod << endl;

    vector<char> validChars = { 'Y', 'N' };
    char confirm = getValidCharInput("\nConfirm refund processing? (Y/N): ", validChars);

    if (confirm == 'Y') {
        // Update payment status to refunded
        for (auto& payment : data.payments) {
            if (payment.paymentID == selectedPayment.paymentID) {
                payment.paymentStatus = "Refunded";
                break;
            }
        }

        cout << "\n=== REFUND PROCESSED ===" << endl;
        cout << "Refund amount: RM " << fixed << setprecision(2) << selectedPayment.amount << endl;
        cout << "Processing time: 5-10 business days" << endl;
        cout << "Refund method: Original payment method" << endl;

        if (!selectedPayment.cardNumber.empty()) {
            cout << "Refund to card: ****-****-****-" << selectedPayment.cardNumber << endl;
        }

        cout << "\nYou will receive a refund confirmation email shortly." << endl;

        // Save updated payment data
        savePaymentsToFile(data.payments);
    }
    else {
        cout << "Refund cancelled." << endl;
    }
}

void viewPaymentStatistics(const SystemData& data) {
    clearScreen();
    cout << "=== PAYMENT STATISTICS ===" << endl;

    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    // Filter payments for current user
    vector<Payment> userPayments;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);

    for (const auto& payment : data.payments) {
        // Find the corresponding booking to verify ownership
        for (const auto& booking : data.bookings) {
            if (booking.bookingID == payment.bookingID) {
                string normalizedBookingUser = normalizeUserID(booking.eventReg.organizer.userID);
                if (normalizedBookingUser == normalizedCurrentUser) {
                    userPayments.push_back(payment);
                    break;
                }
            }
        }
    }

    if (userPayments.empty()) {
        cout << "No payment statistics available." << endl;
        return;
    }

    // Calculate statistics
    double totalCompleted = 0.0, totalRefunded = 0.0;
    int completedCount = 0, refundedCount = 0;
    map<string, int> methodCount;
    map<string, double> methodAmount;

    for (const auto& payment : userPayments) {
        methodCount[payment.paymentMethod]++;
        methodAmount[payment.paymentMethod] += payment.amount;

        if (payment.paymentStatus == "Completed") {
            totalCompleted += payment.amount;
            completedCount++;
        }
        else if (payment.paymentStatus == "Refunded") {
            totalRefunded += payment.amount;
            refundedCount++;
        }
    }

    cout << setfill('=') << setw(60) << "=" << setfill(' ') << endl;
    cout << "PAYMENT SUMMARY:" << endl;
    cout << setfill('-') << setw(60) << "-" << setfill(' ') << endl;
    cout << left << setw(24) << "Total Payments Made" << ": " << userPayments.size() << endl;
    cout << left << setw(24) << "Completed Payments" << ": " << completedCount << endl;
    cout << left << setw(24) << "Refunded Payments" << ": " << refundedCount << endl;
    cout << left << setw(24) << "Total Amount Paid" << ": RM " << fixed << setprecision(2) << totalCompleted << endl;
    cout << left << setw(24) << "Total Amount Refunded" << ": RM " << fixed << setprecision(2) << totalRefunded << endl;
    cout << left << setw(24) << "Net Amount" << ": RM " << fixed << setprecision(2) << (totalCompleted - totalRefunded) << endl;

    cout << "\nPAYMENT METHOD BREAKDOWN:" << endl;
    cout << setfill('-') << setw(60) << "-" << setfill(' ') << endl;
    cout << left << setw(20) << "Method"
        << setw(10) << "Count"
        << setw(15) << "Amount" << endl;
    cout << setfill('-') << setw(60) << "-" << setfill(' ') << endl;

    for (const auto& method : methodCount) {
        cout << left << setw(20) << method.first
            << setw(10) << method.second
            << "RM " << fixed << setprecision(2) << methodAmount[method.first] << endl;
    }

    cout << setfill('=') << setw(60) << "=" << setfill(' ') << endl;
}

string generatePaymentID(const vector<Payment>& payments) {
    srand(static_cast<unsigned int>(time(0)));
    string paymentID;
    bool isUnique = false;

    while (!isUnique) {
        paymentID = "PAY" + to_string(1000 + rand() % 9000);

        isUnique = true;
        for (const auto& payment : payments) {
            if (payment.paymentID == paymentID) {
                isUnique = false;
                break;
            }
        }
    }

    return paymentID;
}

string generateTransactionReference() {
    srand(static_cast<unsigned int>(time(0)));
    string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    string reference = "TXN";

    for (int i = 0; i < 8; i++) {
        reference += chars[rand() % chars.length()];
    }

    return reference;
}

string maskCardNumber(const string& cardNumber) {
    if (cardNumber.length() >= 4) {
        return "****-****-****-" + cardNumber.substr(cardNumber.length() - 4);
    }
    return cardNumber;
}