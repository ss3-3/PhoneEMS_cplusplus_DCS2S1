#pragma once
#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
using namespace std;

struct Organizer {
    string userID;
    char password[20];
    string organizerName; // Steven Lee
    int age;
    string organizerContact; // 0123456789
    string organizerEmail; // stevenlee123@gmail.com
    string position;
    string manufacturer;
    bool isLoggedIn = false;  // Initialize default value
};

// Forward declaration
class SystemData;

// Function prototypes - User Menu
void signUp(SystemData& data);
void loginUser(SystemData& data, bool *validation);
void displayUserInfo(const Organizer& organizer);
bool IsIdDuplicateEmail(const string& email);
bool IsIdDuplicatePhoneNumber(const string& phoneNo);

void viewUserProfile(SystemData& data);
void editUserProfile(SystemData& data);
void changeUserPassword(SystemData& data);
void viewMyEventRegistrations(const SystemData& data);
#endif