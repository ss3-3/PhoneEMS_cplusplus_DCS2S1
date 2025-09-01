#include "event_booking.h"
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
#include "system_data.h"
#include "timeslot.h"
#include "product.h"
#include "utility_fun.h"
#include "input_validation.h"
#include "file_operation.h"
#include "logistics.h"

// Initialize logistics items
vector<LogisticsItem> getLogisticsItems() {
    vector<LogisticsItem> items;

    items.push_back({ "Sound System & Microphones", 150.00, "Professional audio setup with wireless microphones" });
    items.push_back({ "Projector & Screen", 120.00, "HD projector with large projection screen" });
    items.push_back({ "LED Stage Lighting", 200.00, "Professional stage lighting setup" });
    items.push_back({ "Registration Desk & Setup", 80.00, "Welcome desk with registration materials" });
    items.push_back({ "Catering Service (Basic)", 25.00, "Per person - Light refreshments and beverages" });
    items.push_back({ "Photography & Videography", 300.00, "Professional event documentation" });
    items.push_back({ "Decoration Package", 180.00, "Event theming and decoration setup" });
    items.push_back({ "Security Service", 100.00, "Professional security personnel" });
    items.push_back({ "Parking Management", 60.00, "Organized parking assistance" });
    items.push_back({ "Technical Support Staff", 150.00, "On-site technical assistance" });

    return items;
}

// Function to handle logistics selection
double selectLogistics(vector<string>& selectedItems) {
    vector<LogisticsItem> logisticsItems = getLogisticsItems();
    double totalLogisticsCost = 0.0;

    cout << "\n=== LOGISTICS & ADDITIONAL SERVICES ===" << endl;
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;
    cout << left << setw(4) << "No." << setw(35) << "Service/Equipment"
        << setw(12) << "Cost (RM)" << setw(29) << "Description" << endl;
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;

    for (size_t i = 0; i < logisticsItems.size(); i++) {
        cout << left << setw(4) << (i + 1)
            << setw(35) << logisticsItems[i].itemName
            << "RM " << setw(8) << fixed << setprecision(2) << logisticsItems[i].itemCost
            << setw(29) << logisticsItems[i].description.substr(0, 50) << endl;
    }
    cout << setfill('=') << setw(100) << "=" << setfill(' ') << endl;

    vector<char> validChars = { 'Y', 'N' };
    char addLogistics = getValidCharInput("\nWould you like to add logistics services? (Y/N): ", validChars);

    if (addLogistics == 'Y' || addLogistics == 'y') {
        cout << "\nSelect logistics items (enter 0 to finish selection):" << endl;

        while (true) {
            int choice = getValidIntegerInput("Enter item number [0-" +
                to_string(logisticsItems.size()) + "]: ", 0, logisticsItems.size());

            if (choice == 0) {
                break;
            }

            LogisticsItem selectedItem = logisticsItems[choice - 1];

            // Check if item already selected
            bool alreadySelected = false;
            for (const string& item : selectedItems) {
                if (item.find(selectedItem.itemName) != string::npos) {
                    alreadySelected = true;
                    break;
                }
            }

            if (alreadySelected) {
                cout << "Item already selected!" << endl;
                continue;
            }

            // For catering, ask for number of people
            if (selectedItem.itemName.find("Catering") != string::npos) {
                int numPeople = getValidIntegerInput("Enter number of people for catering: ", 1, 1000);
                double cateringCost = selectedItem.itemCost * numPeople;
                totalLogisticsCost += cateringCost;
                selectedItems.push_back(selectedItem.itemName + " (for " + to_string(numPeople) +
                    " people) - RM " + formatDouble(cateringCost));
                cout << "Added: " << selectedItem.itemName << " for " << numPeople
                    << " people - RM " << fixed << setprecision(2) << cateringCost << endl;
            }
            else {
                totalLogisticsCost += selectedItem.itemCost;
                selectedItems.push_back(selectedItem.itemName + " - RM " + formatDouble(selectedItem.itemCost));
                cout << "Added: " << selectedItem.itemName << " - RM "
                    << fixed << setprecision(2) << selectedItem.itemCost << endl;
            }

            cout << "\nCurrent logistics total: RM " << fixed << setprecision(2)
                << totalLogisticsCost << endl;
            cout << "\nSelected items so far:" << endl;
            for (size_t i = 0; i < selectedItems.size(); i++) {
                cout << (i + 1) << ". " << selectedItems[i] << endl;
            }
        }
    }

    return totalLogisticsCost;
}