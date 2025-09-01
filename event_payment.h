#pragma once
#ifndef EVENT_PAYMENT_H
#define EVENT_PAYMENT_H

#include "date.h"
#include "event_booking.h"
#include <string>
#include <vector>

using namespace std;

// Payment structure
struct Payment {
    string paymentID;
    string bookingID;
    double amount;
    Date paymentDate;
    string paymentMethod; // "Credit Card", "Debit Card", "Bank Transfer", "Cash"
    string paymentStatus; // "Pending", "Completed", "Failed", "Refunded"
    string transactionReference;
    string cardNumber; // Last 4 digits only for security
    string cardHolderName;
};

// Payment Status Logic:
// PENDING -> Payment initiated but not yet processed
// COMPLETED -> Payment successfully processed
// FAILED -> Payment processing failed
// REFUNDED -> Payment was refunded due to cancellation

// Function prototypes
class SystemData;
void eventPaymentMenu(SystemData& data);
void makePayment(SystemData& data);
void viewPaymentHistory(const SystemData& data);
void processRefund(SystemData& data);
void viewPaymentStatistics(const SystemData& data); 
string generatePaymentID(const vector<Payment>& payments);
string generateTransactionReference();
string maskCardNumber(const string& cardNumber);

#endif