#pragma once
#ifndef DATE_H
#define DATE_H

#include <string>
using namespace std;

struct Date {
    int year;
    int month;
    int day;

    string toString() const;
};

#endif