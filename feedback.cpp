#include "feedback.h"
#include <map>
#include "utility_fun.h"
#include "system_data.h"
#include "input_validation.h"
#include "file_operation.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
using namespace std;

void feedbackMenu(SystemData& data) {
    bool exitMenu = false;
    while (!exitMenu) {
        clearScreen();
        displayLogo();
        cout << "=== EVENT FEEDBACK SYSTEM ===" << endl;
        cout << setfill('=') << setw(60) << "=" << setfill(' ') << endl;
        cout << "1. Submit Event Feedback" << endl;
        cout << "2. View My Feedback" << endl;
        cout << "3. View My Feedback Statistics" << endl;
        cout << "4. Delete My Feedback" << endl;
        cout << "5. Back to Main Menu" << endl;
        cout << setfill('=') << setw(60) << "=" << setfill(' ') << endl;

        int choice = getValidIntegerInput("Enter your choice [1-5]: ", 1, 5);

        switch (choice) {
        case 1:  submitEventFeedback(data); break;
        case 2:  viewAllFeedback(data); break;
        case 3:  viewFeedbackStatistics(data); break;
        case 4:  deleteFeedback(data); break;
        case 5:  exitMenu = true; break;
        }

        if (choice != 5) {
            pauseScreen();
        }
    }
}

// Generate unique feedback ID
string generateFeedbackID(const SystemData& data) {
    return "FB" + to_string(1000 + data.feedbacks.size());
}

// Submit new event feedback
void submitEventFeedback(SystemData& data) {
    clearScreen();
    displayLogo();
    cout << "=== SUBMIT EVENT FEEDBACK ===" << endl;
    cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    // Check if there are any completed bookings for current user
    vector<EventBooking*> userCompletedBookings;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);

    for (auto& booking : data.bookings) {
        string normalizedBookingUser = normalizeUserID(booking.eventReg.organizer.userID);
        if (normalizedBookingUser == normalizedCurrentUser &&
            (booking.bookingStatus == "Confirmed" || booking.bookingStatus == "Completed")) {
            userCompletedBookings.push_back(&booking);
        }
    }

    if (userCompletedBookings.empty()) {
        cout << "No completed events found under your account. Cannot submit feedback." << endl;
        cout << "Please complete some events first or wait for your bookings to be confirmed." << endl;
        return;
    }

    // Display user's completed bookings only
    cout << "User: " << data.currentUser << endl;
    cout << setfill('-') << setw(80) << "-" << setfill(' ') << endl;
    cout << "Your completed events available for feedback:" << endl;
    for (size_t i = 0; i < userCompletedBookings.size(); i++) {
        cout << (i + 1) << ". Event: " << userCompletedBookings[i]->eventReg.eventTitle << endl;
        cout << "   Booking ID: " << userCompletedBookings[i]->bookingID << endl;
        cout << "   Organizer: " << userCompletedBookings[i]->eventReg.organizer.organizerName << endl;
        cout << "   Date: " << userCompletedBookings[i]->eventDate.toString() << endl;
        cout << "   Venue: " << userCompletedBookings[i]->venue.venueName << endl;
        cout << endl;
    }

    // Select event
    int eventChoice = getValidIntegerInput("Select event to provide feedback for [1-" +
        to_string(userCompletedBookings.size()) + "]: ",
        1, userCompletedBookings.size());
    EventBooking* selectedBooking = userCompletedBookings[eventChoice - 1];

    // Check if feedback already exists for this booking by this user
    for (const auto& feedback : data.feedbacks) {
        if (feedback.bookingID == selectedBooking->bookingID &&
            normalizeUserID(feedback.submittedBy) == normalizedCurrentUser) {
            cout << "\nYou have already submitted feedback for this event!" << endl;
            cout << "Existing Feedback ID: " << feedback.feedbackID << endl;
            cout << "Submission Date: " << feedback.submissionDate.toString() << endl;
            return;
        }
    }

    // Create new feedback
    EventFeedback newFeedback;
    newFeedback.feedbackID = generateFeedbackID(data);
    newFeedback.bookingID = selectedBooking->bookingID;
    newFeedback.eventTitle = selectedBooking->eventReg.eventTitle;
    newFeedback.organizerName = selectedBooking->eventReg.organizer.organizerName;
    newFeedback.eventDate = selectedBooking->eventDate;
    newFeedback.venueName = selectedBooking->venue.venueName;

    cout << "\nSelected Event: " << newFeedback.eventTitle << endl;
    cout << setfill('-') << setw(50) << "-" << setfill(' ') << endl;

    // Rating system using 2D array
    const int NUM_CATEGORIES = 4;
    const int RATING_SCALE = 5;

    // 2D array to store rating options and user selections
    // Row 0: venue, Row 1: organization, Row 2: logistics, Row 3: overall
    int ratingMatrix[NUM_CATEGORIES][RATING_SCALE + 1]; // +1 for 1-based indexing

    // Initialize rating matrix to 0
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        for (int j = 0; j <= RATING_SCALE; j++) {
            ratingMatrix[i][j] = 0;
        }
    }

    // Rating categories
    string categories[NUM_CATEGORIES] = {
        "Venue Rating",
        "Event Organization Rating",
        "Logistics Rating",
        "Overall Rating"
    };

    // Collect ratings using the 2D array structure
    cout << "\nPlease rate the following aspects (1-5 scale, where 5 is excellent):" << endl;

    for (int i = 0; i < NUM_CATEGORIES; i++) {
        int rating = getValidIntegerInput(categories[i] + " [1-5]: ", 1, 5);
        ratingMatrix[i][rating] = 1; // Mark the selected rating

        // Store in the feedback structure
        switch (i) {
        case 0: newFeedback.venueRating = rating; break;
        case 1: newFeedback.organizationRating = rating; break;
        case 2: newFeedback.logisticsRating = rating; break;
        case 3: newFeedback.overallRating = rating; break;
        }
    }

    // Display rating summary using the matrix
    cout << "\n--- Rating Summary ---" << endl;
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        cout << categories[i] << ": ";
        for (int j = 1; j <= RATING_SCALE; j++) {
            if (ratingMatrix[i][j] == 1) {
                cout << j << "/5" << endl;
                break;
            }
        }
    }

    // Get comments
    cout << "\nPlease provide your comments:" << endl;
    cout << "Venue Comments: ";
    cin.ignore();
    getline(cin, newFeedback.venueComments);

    cout << "Organization Comments: ";
    getline(cin, newFeedback.organizationComments);

    cout << "Logistics Comments: ";
    getline(cin, newFeedback.logisticsComments);

    cout << "General Comments: ";
    getline(cin, newFeedback.generalComments);

    cout << "Suggestions for improvement: ";
    getline(cin, newFeedback.suggestions);

    // Set submitter as current user
    newFeedback.submittedBy = data.currentUser;

    // Would recommend
    vector<char> validChars = { 'Y', 'N' };
    char recommend = getValidCharInput("Would you recommend this venue/service to others? (Y/N): ", validChars);
    newFeedback.wouldRecommend = (recommend == 'Y');

    // Set submission date (current date)
    newFeedback.submissionDate = getCurrentDate();

    // Add to system
    data.feedbacks.push_back(newFeedback);
    saveFeedbackToFile(data.feedbacks);

    cout << "\nFeedback submitted successfully!" << endl;
    cout << "Feedback ID: " << newFeedback.feedbackID << endl;
}

// View user's feedback only
void viewAllFeedback(const SystemData& data) {
    clearScreen();
    displayLogo();
    cout << "=== MY EVENT FEEDBACK ===" << endl;
    cout << setfill('=') << setw(80) << "=" << setfill(' ') << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    if (data.feedbacks.empty()) {
        cout << "No feedback records found." << endl;
        return;
    }

    // Filter feedback submitted by current user
    vector<EventFeedback> userFeedbacks;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);

    for (const auto& feedback : data.feedbacks) {
        if (normalizeUserID(feedback.submittedBy) == normalizedCurrentUser) {
            userFeedbacks.push_back(feedback);
        }
    }

    if (userFeedbacks.empty()) {
        cout << "No feedback records found for your account." << endl;
        cout << "Please submit feedback for your completed events first." << endl;
        return;
    }

    cout << "User: " << data.currentUser << endl;
    cout << "Total My Feedback: " << userFeedbacks.size() << endl;
    cout << setfill('-') << setw(80) << "-" << setfill(' ') << endl;

    for (const auto& feedback : userFeedbacks) {
        cout << "Feedback ID: " << feedback.feedbackID << endl;
        cout << "Event: " << feedback.eventTitle << endl;
        cout << "Organizer: " << feedback.organizerName << endl;
        cout << "Event Date: " << feedback.eventDate.toString() << endl;
        cout << "Venue: " << feedback.venueName << endl;
        cout << "Submitted by: " << feedback.submittedBy << endl;
        cout << "Submission Date: " << feedback.submissionDate.toString() << endl;

        cout << "\nRatings:" << endl;
        cout << "  Venue: " << feedback.venueRating << "/5" << endl;
        cout << "  Organization: " << feedback.organizationRating << "/5" << endl;
        cout << "  Logistics: " << feedback.logisticsRating << "/5" << endl;
        cout << "  Overall: " << feedback.overallRating << "/5" << endl;

        cout << "Would Recommend: " << (feedback.wouldRecommend ? "Yes" : "No") << endl;

        if (!feedback.generalComments.empty()) {
            cout << "General Comments: " << feedback.generalComments << endl;
        }
        if (!feedback.suggestions.empty()) {
            cout << "Suggestions: " << feedback.suggestions << endl;
        }

        cout << setfill('-') << setw(80) << "-" << setfill(' ') << endl;
    }
}

// View user's feedback statistics only
void viewFeedbackStatistics(const SystemData& data) {
    clearScreen();
    displayLogo();
    cout << "=== MY FEEDBACK STATISTICS ===" << endl;
    cout << setfill('=') << setw(60) << "=" << setfill(' ') << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    if (data.feedbacks.empty()) {
        cout << "No feedback data available for statistics." << endl;
        return;
    }

    // Filter user's feedback only
    vector<EventFeedback> userFeedbacks;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);

    for (const auto& feedback : data.feedbacks) {
        if (normalizeUserID(feedback.submittedBy) == normalizedCurrentUser) {
            userFeedbacks.push_back(feedback);
        }
    }

    if (userFeedbacks.empty()) {
        cout << "No feedback data available for your account." << endl;
        cout << "Please submit feedback for your events first." << endl;
        return;
    }

    size_t totalFeedback = userFeedbacks.size();

    // Calculate user's statistics using 2D array
    const int NUM_CATEGORIES = 4;
    const int RATING_SCALE = 5;

    // 2D array for rating frequency analysis
    // Rows: categories (venue, org, logistics, overall)
    // Columns: rating values (1-5)
    int ratingFrequency[NUM_CATEGORIES][RATING_SCALE + 1];

    // Initialize frequency matrix
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        for (int j = 0; j <= RATING_SCALE; j++) {
            ratingFrequency[i][j] = 0;
        }
    }

    // Count frequencies from user's feedback
    for (const auto& feedback : userFeedbacks) {
        ratingFrequency[0][feedback.venueRating]++;
        ratingFrequency[1][feedback.organizationRating]++;
        ratingFrequency[2][feedback.logisticsRating]++;
        ratingFrequency[3][feedback.overallRating]++;
    }

    // Category names
    string categoryNames[NUM_CATEGORIES] = {
        "Venue", "Organization", "Logistics", "Overall"
    };

    cout << "User: " << data.currentUser << endl;
    cout << setfill('-') << setw(60) << "-" << setfill(' ') << endl;

    // Display detailed rating breakdown using the 2D array
    cout << "DETAILED RATING BREAKDOWN" << endl;
    cout << setfill('-') << setw(50) << "-" << setfill(' ') << endl;

    for (int category = 0; category < NUM_CATEGORIES; category++) {
        cout << categoryNames[category] << " Ratings:" << endl;

        for (int rating = 5; rating >= 1; rating--) {
            int count = ratingFrequency[category][rating];
            double percentage = (count * 100.0) / totalFeedback;

            cout << "  " << rating << " stars: " << count
                << " (" << fixed << setprecision(1) << percentage << "%)" << endl;
        }
        cout << endl;
    }

    // Calculate and display averages using the matrix
    cout << "CATEGORY AVERAGES" << endl;
    cout << setfill('-') << setw(30) << "-" << setfill(' ') << endl;

    for (int category = 0; category < NUM_CATEGORIES; category++) {
        double total = 0;
        int count = 0;

        for (int rating = 1; rating <= RATING_SCALE; rating++) {
            total += rating * ratingFrequency[category][rating];
            count += ratingFrequency[category][rating];
        }

        double average = (count > 0) ? total / count : 0;
        cout << categoryNames[category] << " Average: "
            << fixed << setprecision(2) << average << "/5" << endl;
    }

    // Display the comparison matrices
    displayRatingComparisonMatrix(userFeedbacks);
    displayVenuePerformanceMatrix(userFeedbacks);

    // User's venue performance (existing code)
    cout << "\nMY VENUE EXPERIENCES" << endl;
    cout << setfill('-') << setw(40) << "-" << setfill(' ') << endl;

    map<string, vector<int>> venueRatings;
    for (const auto& feedback : userFeedbacks) {
        venueRatings[feedback.venueName].push_back(feedback.venueRating);
    }

    vector<pair<string, double>> venueAvgs;
    for (const auto& venue : venueRatings) {
        double avg = accumulate(venue.second.begin(), venue.second.end(), 0.0) / venue.second.size();
        venueAvgs.push_back({ venue.first, avg });
    }

    // Sort by average rating
    sort(venueAvgs.begin(), venueAvgs.end(),
        [](const pair<string, double>& a, const pair<string, double>& b) {
            return a.second > b.second;
        });

    for (const auto& venue : venueAvgs) {
        cout << venue.first << ": " << fixed << setprecision(2) << venue.second << "/5 "
            << "(" << venueRatings[venue.first].size() << " feedback)" << endl;
    }

    // User's recent feedback trends
    cout << "\nMY RECENT FEEDBACK TRENDS" << endl;
    cout << setfill('-') << setw(40) << "-" << setfill(' ') << endl;

    if (userFeedbacks.size() >= 3) {
        auto recent = userFeedbacks.end() - min(size_t(5), userFeedbacks.size());
        double recentAvg = 0;
        for (auto it = recent; it != userFeedbacks.end(); ++it) {
            recentAvg += it->overallRating;
        }
        recentAvg /= min(size_t(5), userFeedbacks.size());
        cout << "My Recent Average Rating (last " << min(size_t(5), userFeedbacks.size())
            << " feedback): " << fixed << setprecision(2) << recentAvg << "/5" << endl;
    }

    // User's feedback themes
    cout << "\nMY FEEDBACK THEMES" << endl;
    cout << setfill('-') << setw(40) << "-" << setfill(' ') << endl;

    int excellentCount = 0, goodCount = 0, averageCount = 0, poorCount = 0;
    for (const auto& feedback : userFeedbacks) {
        if (feedback.overallRating >= 5) excellentCount++;
        else if (feedback.overallRating >= 4) goodCount++;
        else if (feedback.overallRating >= 3) averageCount++;
        else poorCount++;
    }

    cout << "My Excellent Ratings (5 stars): " << excellentCount << endl;
    cout << "My Good Ratings (4+ stars): " << goodCount << endl;
    cout << "My Average Ratings (3+ stars): " << averageCount << endl;
    cout << "My Critical Ratings (<3 stars): " << poorCount << endl;
}

// Delete user's feedback only
void deleteFeedback(SystemData& data) {
    clearScreen();
    displayLogo();
    cout << "=== DELETE MY FEEDBACK ===" << endl;
    cout << setfill('=') << setw(50) << "=" << setfill(' ') << endl;

    // Check if user is logged in
    if (data.currentUser.empty()) {
        cout << "Error: No user logged in!" << endl;
        return;
    }

    if (data.feedbacks.empty()) {
        cout << "No feedback records to delete." << endl;
        return;
    }

    // Filter and display user's feedback only
    vector<int> userFeedbackIndices;
    string normalizedCurrentUser = normalizeUserID(data.currentUser);

    cout << "User: " << data.currentUser << endl;
    cout << setfill('-') << setw(70) << "-" << setfill(' ') << endl;
    cout << "Your Feedback Records:" << endl;

    int displayCount = 1;
    for (size_t i = 0; i < data.feedbacks.size(); i++) {
        if (normalizeUserID(data.feedbacks[i].submittedBy) == normalizedCurrentUser) {
            userFeedbackIndices.push_back(static_cast<int>(i));
            const auto& feedback = data.feedbacks[i];
            cout << displayCount << ". ID: " << feedback.feedbackID
                << " | Event: " << feedback.eventTitle
                << " | Rating: " << feedback.overallRating << "/5"
                << " | Date: " << feedback.submissionDate.toString() << endl;
            displayCount++;
        }
    }

    if (userFeedbackIndices.empty()) {
        cout << "No feedback records found for your account." << endl;
        return;
    }

    int choice = getValidIntegerInput("Select feedback to delete [1-" +
        to_string(userFeedbackIndices.size()) + "] (0 to cancel): ",
        0, userFeedbackIndices.size());

    if (choice == 0) {
        cout << "Delete operation cancelled." << endl;
        return;
    }

    // Get the actual index in the main feedbacks vector
    int actualIndex = userFeedbackIndices[choice - 1];

    // Confirm deletion
    const auto& toDelete = data.feedbacks[actualIndex];
    cout << "\nConfirm deletion of your feedback:" << endl;
    cout << "Feedback ID: " << toDelete.feedbackID << endl;
    cout << "Event: " << toDelete.eventTitle << endl;
    cout << "Submitted by: " << toDelete.submittedBy << endl;
    cout << "Overall Rating: " << toDelete.overallRating << "/5" << endl;

    vector<char> validChars = { 'Y', 'N' };
    char confirm = getValidCharInput("Are you sure you want to delete this feedback? (Y/N): ", validChars);

    if (confirm == 'Y') {
        data.feedbacks.erase(data.feedbacks.begin() + actualIndex);
        saveFeedbackToFile(data.feedbacks);
        cout << "Your feedback deleted successfully." << endl;
    }
    else {
        cout << "Delete operation cancelled." << endl;
    }
}

void displayRatingComparisonMatrix(const vector<EventFeedback>& userFeedbacks) {
    const int NUM_CATEGORIES = 4;
    const int MAX_EVENTS = 10; // Display last 10 events

    // 2D array for rating comparison
    // Rows: recent events, Columns: rating categories
    int recentRatings[MAX_EVENTS][NUM_CATEGORIES];
    string eventTitles[MAX_EVENTS];

    // Initialize arrays
    for (int i = 0; i < MAX_EVENTS; i++) {
        eventTitles[i] = "";
        for (int j = 0; j < NUM_CATEGORIES; j++) {
            recentRatings[i][j] = 0;
        }
    }

    // Fill with recent feedback data
    int eventCount = min(MAX_EVENTS, (int)userFeedbacks.size());
    int startIndex = max(0, (int)userFeedbacks.size() - MAX_EVENTS);

    for (int i = 0; i < eventCount; i++) {
        const auto& feedback = userFeedbacks[startIndex + i];
        eventTitles[i] = feedback.eventTitle.substr(0, 20); // Truncate for display
        recentRatings[i][0] = feedback.venueRating;
        recentRatings[i][1] = feedback.organizationRating;
        recentRatings[i][2] = feedback.logisticsRating;
        recentRatings[i][3] = feedback.overallRating;
    }

    // Display comparison matrix
    cout << "\nRATING COMPARISON MATRIX (Recent Events)" << endl;
    cout << setfill('-') << setw(70) << "-" << setfill(' ') << endl;
    cout << left << setw(22) << "Event"
        << setw(8) << "Venue"
        << setw(8) << "Org"
        << setw(8) << "Log"
        << setw(8) << "Overall" << endl;
    cout << setfill('-') << setw(70) << "-" << setfill(' ') << endl;

    for (int i = 0; i < eventCount; i++) {
        cout << left << setw(22) << eventTitles[i];
        for (int j = 0; j < NUM_CATEGORIES; j++) {
            cout << setw(8) << recentRatings[i][j];
        }
        cout << endl;
    }

    // Calculate column averages
    cout << setfill('-') << setw(70) << "-" << setfill(' ') << endl;
    cout << left << setw(22) << "Average:";

    for (int category = 0; category < NUM_CATEGORIES; category++) {
        double total = 0;
        for (int event = 0; event < eventCount; event++) {
            total += recentRatings[event][category];
        }
        double average = (eventCount > 0) ? total / eventCount : 0;
        cout << setw(8) << fixed << setprecision(1) << average;
    }
    cout << endl;
}

void displayVenuePerformanceMatrix(const vector<EventFeedback>& userFeedbacks) {
    const int MAX_VENUES = 5;
    const int NUM_METRICS = 3; // venue rating, overall rating, recommendation count

    // 2D array for venue performance data
    double venueMatrix[MAX_VENUES][NUM_METRICS];
    string venueNames[MAX_VENUES];
    int venueCounts[MAX_VENUES];

    // Initialize arrays
    for (int i = 0; i < MAX_VENUES; i++) {
        venueNames[i] = "";
        venueCounts[i] = 0;
        for (int j = 0; j < NUM_METRICS; j++) {
            venueMatrix[i][j] = 0.0;
        }
    }

    // Collect unique venues and their data
    vector<string> uniqueVenues;
    for (const auto& feedback : userFeedbacks) {
        if (find(uniqueVenues.begin(), uniqueVenues.end(), feedback.venueName) == uniqueVenues.end()) {
            uniqueVenues.push_back(feedback.venueName);
        }
    }

    int venueCount = min(MAX_VENUES, (int)uniqueVenues.size());

    // Calculate metrics for each venue
    for (int i = 0; i < venueCount; i++) {
        venueNames[i] = uniqueVenues[i];
        double totalVenueRating = 0, totalOverallRating = 0;
        int recommendCount = 0, feedbackCount = 0;

        for (const auto& feedback : userFeedbacks) {
            if (feedback.venueName == uniqueVenues[i]) {
                totalVenueRating += feedback.venueRating;
                totalOverallRating += feedback.overallRating;
                if (feedback.wouldRecommend) recommendCount++;
                feedbackCount++;
            }
        }

        venueCounts[i] = feedbackCount;
        venueMatrix[i][0] = (feedbackCount > 0) ? totalVenueRating / feedbackCount : 0;
        venueMatrix[i][1] = (feedbackCount > 0) ? totalOverallRating / feedbackCount : 0;
        venueMatrix[i][2] = (feedbackCount > 0) ? (recommendCount * 100.0 / feedbackCount) : 0;
    }

    // Display venue performance matrix
    cout << "\nVENUE PERFORMANCE MATRIX" << endl;
    cout << setfill('-') << setw(65) << "-" << setfill(' ') << endl;
    cout << left << setw(20) << "Venue"
        << setw(12) << "Venue Avg"
        << setw(12) << "Overall Avg"
        << setw(12) << "Recommend %"
        << setw(9) << "Count" << endl;
    cout << setfill('-') << setw(65) << "-" << setfill(' ') << endl;

    for (int i = 0; i < venueCount; i++) {
        cout << left << setw(20) << venueNames[i].substr(0, 19)
            << setw(12) << fixed << setprecision(1) << venueMatrix[i][0]
            << setw(12) << venueMatrix[i][1]
            << setw(12) << venueMatrix[i][2] << "%"
            << setw(9) << venueCounts[i] << endl;
    }
}