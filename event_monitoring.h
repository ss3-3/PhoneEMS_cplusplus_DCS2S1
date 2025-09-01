#pragma once
#ifndef EVENT_MONITORING_H
#define EVENT_MONITORING_H

#include "system_data.h"

using namespace std;
//Function prototypes - Event Monitoring System
void eventMonitoringMenu(SystemData& data);
void displayEventSummary(const SystemData& data);
void displayVenueUtilization(const SystemData& data);
void displayFinancialReport(const SystemData& data);
void displayUpcomingEvents(const SystemData& data);
void displayRegistrationStatistics(const SystemData& data);
void searchEvents(const SystemData& data);
void generateEventReport(const SystemData& data);

#endif