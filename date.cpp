#include "date.h"
#include <string>

string Date::toString() const {
    return to_string(year) + "-" +
        (month < 10 ? "0" : "") + to_string(month) + "-" +
        (day < 10 ? "0" : "") + to_string(day);
}
