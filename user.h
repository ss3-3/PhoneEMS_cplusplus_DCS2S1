#pragma once
#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
using namespace std;

struct Organizer {
    string userID;
    string password;
    string organizerName; // Steven Lee
    string organizerContact; // 0123456789
    string organizerEmail; // stevenlee123@gmail.com
    string position;
    bool isLoggedIn = false;  // Initialize default value
};

// Forward declaration
class SystemData;

// Function prototypes - User Menu
void signUp(SystemData& data);
bool loginUser(SystemData& data);
void displayUserInfo(const Organizer& organizer);
bool IsIdDuplicate(const string& userID);

void viewUserProfile(SystemData& data);
void editUserProfile(SystemData& data);
void changeUserPassword(SystemData& data);
void viewMyEventRegistrations(const SystemData& data);
#endif