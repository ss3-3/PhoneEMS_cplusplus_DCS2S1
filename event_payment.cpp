#include "event_payment.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <format>
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
        cout << format("{:=<50}", "") << endl;
        cout << "1. Make Payment" << endl;
        cout << "2. View Payment History" << endl;
        cout << "3. Process Refund" << endl;
        cout << "4. View Payment Statistics" << endl;
        cout << "5. Back to Main Menu" << endl;
        cout << format("{:=<50}", "") << endl;

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
    const double PROMO_DISCOUNT = 0.10;
    
    clearScreen();
    cout << "=== MAKE PAYMENT ===" << endl;
    cout << format("{:=<50}", "") << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    // Filter user's bookings that haven't been paid
    vector<EventBooking> unpaidBookings;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);

    for (const auto& booking : data.bookings) {
        string normalizedBookingUser = normalizeUserID(booking.eventReg.organizer.userID);
        if (normalizedBookingUser == normalizedCurrentUser &&
            (booking.bookingStatus == "Pending")) {

            // Check if payment already exists for this booking
            bool alreadyPaid = false;
            for (const auto& payment : data.payments) {
                if (payment.bookingID == booking.bookingID &&
                    payment.paymentStatus == "Confirmed") {
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
    cout << format("{:=<100}", "") << endl;
    cout << format("{:<4}{:<10}{:<20}{:<12}{:<15}{:<15}{:<12}",
        "No.", "Book ID", "Event Title", "Date", "Venue", "Amount (RM)", "Status") << endl;
    cout << format("{:=<100}", "") << endl;

    for (size_t i = 0; i < unpaidBookings.size(); i++) {
        cout << format("{:<4}{:<10}{:<20}{:<12}{:<15}RM {:>12.2f}{:<12}",
            i + 1,
            unpaidBookings[i].bookingID,
            unpaidBookings[i].eventReg.eventTitle.substr(0, 19),
            unpaidBookings[i].eventDate.toString(),
            unpaidBookings[i].venue.venueName.substr(0, 14),
            unpaidBookings[i].finalCost,
            unpaidBookings[i].bookingStatus) << endl;
    }
    cout << format("{:=<100}", "") << endl;

    int bookingChoice = getValidIntegerInput("Select booking to pay for: ", 1, static_cast<int>(unpaidBookings.size()));
    EventBooking selectedBooking = unpaidBookings[bookingChoice - 1];

     // Promo code (optional)
     cout << "\nDo you have a promo code? (Enter or leave blank): ";
     string promoCode;
     getline(cin, promoCode);

     // Apply discount if valid
     if (!promoCode.empty()) {
         if (promoCode == "PROMO10") { // Example valid promo
             cout << "Promo code applied! You get 10% discount." << endl;
             selectedBooking.finalCost *= (1.0 - PROMO_DISCOUNT);
         }
         else {
             cout << "Invalid promo code. No discount applied." << endl;
         }
     }

    // Create payment record
    Payment newPayment;
    newPayment.paymentID = generatePaymentID(data.payments);
    newPayment.bookingID = selectedBooking.bookingID;
    newPayment.amount = selectedBooking.finalCost;
    newPayment.paymentDate = getCurrentDate();
    newPayment.paymentStatus = "Completed"; // Simplified - payment is immediately completed

    cout << "\n=== PAYMENT DETAILS ===" << endl;
    cout << format("Payment ID: {}", newPayment.paymentID) << endl;
    cout << format("Booking ID: {}", newPayment.bookingID) << endl;
    cout << format("Event: {}", selectedBooking.eventReg.eventTitle) << endl;
    cout << format("Amount: RM {:.2f}", newPayment.amount) << endl;
    cout << format("Date: {}", newPayment.paymentDate.toString()) << endl;

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

        string cvv;
        while (true) {
            cout << "Enter CVV (3 digits): ";
            getline(cin, cvv);

            // Remove spaces
            string cleanCard = "";
            for (char c : cvv) {
                if (c != ' ') cleanCard += c;
            }

            if (cleanCard.length() == 3) {
                bool allDigits = true;
                for (char c : cleanCard) {
                    if (c < '0' || c > '9') {
                        allDigits = false;
                        break;
                    }
                }
                if (allDigits) {
                    // Store only last 4 digits
                    cvv = cleanCard;
                    break;
                }
            }
            cout << "Invalid card number. Please enter 3 digits." << endl;
        }

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
    cout << format("Payment ID: {}", newPayment.paymentID) << endl;
    cout << format("Transaction Reference: {}", newPayment.transactionReference) << endl;
    cout << format("Amount Paid: RM {:.2f}", newPayment.amount) << endl;
    cout << format("Payment Method: {}", newPayment.paymentMethod) << endl;
    cout << format("Status: {}", newPayment.paymentStatus) << endl;

    if (!newPayment.cardNumber.empty()) {
        cout << format("Card Used: ****-****-****-{}", newPayment.cardNumber) << endl;
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

    cout << format("{:=<130}", "") << endl;
    cout << format("{:<12}{:<12}{:<12}{:<12}{:<15}{:<12}{:<20}{:<12}",
        "Payment ID", "Booking ID", "Amount", "Date", "Method", "Status", "Reference", "Card") << endl;
    cout << format("{:=<130}", "") << endl;

    double totalPaid = 0.0;
    for (const auto& payment : userPayments) {
        string cardDisplay = payment.cardNumber.empty() ? "N/A" : format("****{}", payment.cardNumber);

        cout << format("{:<12}{:<12}RM {:>9.2f}{:<12}{:<15}{:<12}{:<20}{:<12}",
            payment.paymentID,
            payment.bookingID,
            payment.amount,
            payment.paymentDate.toString(),
            payment.paymentMethod.substr(0, 14),
            payment.paymentStatus,
            payment.transactionReference.substr(0, 19),
            cardDisplay) << endl;

        if (payment.paymentStatus == "Completed") {
            totalPaid += payment.amount;
        }
    }

    cout << format("{:=<130}", "") << endl;
    cout << format("Total Payments: {}", userPayments.size()) << endl;
    cout << format("Total Amount Paid: RM {:.2f}", totalPaid) << endl;
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
    cout << format("{:=<100}", "") << endl;
    cout << format("{:<4}{:<12}{:<12}{:<15}{:<15}{:<20}",
        "No.", "Payment ID", "Booking ID", "Amount", "Method", "Reference") << endl;
    cout << format("{:=<100}", "") << endl;

    for (size_t i = 0; i < refundablePayments.size(); i++) {
        cout << format("{:<4}{:<12}{:<12}RM {:>12.2f}{:<15}{:<20}",
            i + 1,
            refundablePayments[i].paymentID,
            refundablePayments[i].bookingID,
            refundablePayments[i].amount,
            refundablePayments[i].paymentMethod.substr(0, 14),
            refundablePayments[i].transactionReference) << endl;
    }
    cout << format("{:=<100}", "") << endl;

    int refundChoice = getValidIntegerInput("Select payment to refund: ", 1, static_cast<int>(refundablePayments.size()));
    Payment selectedPayment = refundablePayments[refundChoice - 1];

    cout << "\n=== REFUND DETAILS ===" << endl;
    cout << format("Payment ID: {}", selectedPayment.paymentID) << endl;
    cout << format("Amount to refund: RM {:.2f}", selectedPayment.amount) << endl;
    cout << format("Original payment method: {}", selectedPayment.paymentMethod) << endl;

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
        cout << format("Refund amount: RM {:.2f}", selectedPayment.amount) << endl;
        cout << "Processing time: 5-7 business days" << endl;
        cout << "Refund method: Original payment method" << endl;

        if (!selectedPayment.cardNumber.empty()) {
            cout << format("Refund to card: ****-****-****-{}", selectedPayment.cardNumber) << endl;
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

    cout << format("{:=<60}", "") << endl;
    cout << "PAYMENT SUMMARY:" << endl;
    cout << format("{:-<60}", "") << endl;
    cout << format("{:<24}: {}", "Total Payments Made", userPayments.size()) << endl;
    cout << format("{:<24}: {}", "Completed Payments", completedCount) << endl;
    cout << format("{:<24}: {}", "Refunded Payments", refundedCount) << endl;
    cout << format("{:<24}: RM {:.2f}", "Total Amount Paid", totalCompleted) << endl;
    cout << format("{:<24}: RM {:.2f}", "Total Amount Refunded", totalRefunded) << endl;
    cout << format("{:<24}: RM {:.2f}", "Net Amount", (totalCompleted - totalRefunded)) << endl;

    cout << "\nPAYMENT METHOD BREAKDOWN:" << endl;
    cout << format("{:-<60}", "") << endl;
    cout << format("{:<20}{:<10}{:<15}", "Method", "Count", "Amount") << endl;
    cout << format("{:-<60}", "") << endl;

    for (const auto& method : methodCount) {
        cout << format("{:<20}{:<10}RM {:.2f}",
            method.first,
            method.second,
            methodAmount[method.first]) << endl;
    }

    cout << format("{:=<60}", "") << endl;
}

string generatePaymentID(const vector<Payment>& payments) {
    srand(static_cast<unsigned int>(time(0)));
    string paymentID;
    bool isUnique = false;

    while (!isUnique) {
        paymentID = format("PAY{}", 1000 + rand() % 9000);

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
        return format("****-****-****-{}", cardNumber.substr(cardNumber.length() - 4));
    }
    return cardNumber;
}
