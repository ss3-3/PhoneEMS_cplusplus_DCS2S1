#pragma once
#pragma once
#ifndef LOGISTICS_H
#define LOGISTICS_H

#include <string>
#include <vector>
#include "date.h"
using namespace std;

struct LogisticsItem {
    string itemName;
    double itemCost;
    string description;
};

vector<LogisticsItem> getLogisticsItems();
double selectLogistics(vector<string>& selectedItems);

#endif