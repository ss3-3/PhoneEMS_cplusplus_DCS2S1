#pragma once
#ifndef INPUT_VALIDATION_H
#define INPUT_VALIDATION_H

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

using namespace std;

string toUpperCase(const string& str);

// Confirmation / exit handling
bool getExit();
void confirmExit();

// Integer input
int getValidIntegerInput(const string& prompt, int minVal, int maxVal);
int getValidIntegerInputWithExit(const string& prompt, int minVal, int maxVal);

// String input
string getValidStringInput(const string& prompt, int minLength = 1);
string getValidStringInputWithExit(const string& prompt, int minLength = 1);
string getValidStringInputOrKeepCurrent(const string& prompt, int minLength = 1);

// Double input
double getValidDoubleInput(const string& prompt, double minVal = 0.0);
double getValidDoubleInputWithExit(const string& prompt, double minVal);

// Character input
char getValidCharInput(const string& prompt, const vector<char>& validChars);

// Date input
Date getValidDateInput();

// Specialized validation
string getValidPhoneNumber(const string& prompt);
string getValidEmailAddress(const string& prompt);
string getValidPassword(const string& prompt);
string getValidEventStatus(const string& prompt);

// Validate feedback rating input
int getValidRating(const string& prompt);
#endif