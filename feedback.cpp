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

    // Get ratings (1-5 scale)
    cout << "\nPlease rate the following aspects (1-5 scale, where 5 is excellent):" << endl;
    newFeedback.venueRating = getValidIntegerInput("Venue Rating [1-5]: ", 1, 5);
    newFeedback.organizationRating = getValidIntegerInput("Event Organization Rating [1-5]: ", 1, 5);
    newFeedback.logisticsRating = getValidIntegerInput("Logistics Rating [1-5]: ", 1, 5);
    newFeedback.overallRating = getValidIntegerInput("Overall Rating [1-5]: ", 1, 5);

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

    // Calculate user's statistics
    double totalVenue = 0, totalOrg = 0, totalLog = 0, totalOverall = 0;
    int recommendCount = 0;

    for (const auto& feedback : userFeedbacks) {
        totalVenue += feedback.venueRating;
        totalOrg += feedback.organizationRating;
        totalLog += feedback.logisticsRating;
        totalOverall += feedback.overallRating;
        if (feedback.wouldRecommend) recommendCount++;
    }

    cout << "User: " << data.currentUser << endl;
    cout << setfill('-') << setw(60) << "-" << setfill(' ') << endl;
    cout << "MY FEEDBACK STATISTICS" << endl;
    cout << setfill('-') << setw(40) << "-" << setfill(' ') << endl;
    cout << "My Total Feedback Submissions: " << totalFeedback << endl;
    cout << "My Average Venue Rating: " << fixed << setprecision(2) << (totalVenue / totalFeedback) << "/5" << endl;
    cout << "My Average Organization Rating: " << (totalOrg / totalFeedback) << "/5" << endl;
    cout << "My Average Logistics Rating: " << (totalLog / totalFeedback) << "/5" << endl;
    cout << "My Average Overall Rating: " << (totalOverall / totalFeedback) << "/5" << endl;
    cout << "My Recommendation Rate: " << recommendCount << "/" << totalFeedback <<
        " (" << (recommendCount * 100.0 / totalFeedback) << "%)" << endl;

    // User's rating distribution
    cout << "\nMY RATING DISTRIBUTION" << endl;
    cout << setfill('-') << setw(40) << "-" << setfill(' ') << endl;

    vector<int> overallCounts(6, 0); // Index 0 unused, 1-5 for ratings
    for (const auto& feedback : userFeedbacks) {
        overallCounts[feedback.overallRating]++;
    }

    for (int i = 5; i >= 1; i--) {
        cout << i << " stars: " << overallCounts[i] << " (" <<
            (overallCounts[i] * 100.0 / totalFeedback) << "%)" << endl;
    }

    // User's venue performance
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
        // Show trend for user's last few feedback
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