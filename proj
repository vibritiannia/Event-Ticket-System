#include <iostream>
#include <limits>
#include <string>
#include <deque>
#include <stack>
using namespace std;

// ─────────────────────────────────────────
//  DATA STRUCTURES
// ─────────────────────────────────────────

struct Customer {
    string name;
    bool   isVIP;
};

struct Action {
    string   type;      // "BOOK" | "CANCEL" | "PROCESS"
    Customer customer;
    int      position;  // index in the unified sorted view
};

deque<Customer> vipQueue;
deque<Customer> regularQueue;
stack<Action>   undoStack;

int maxSeats     = 10;
int currentSeats = 0;

// ─────────────────────────────────────────
//  UNIFIED VIEW HELPERS
// ─────────────────────────────────────────

// Returns total number of bookings across both queues.
int totalBookings() {
    return (int)(vipQueue.size() + regularQueue.size());
}

// Converts a 1-based display number to the actual customer.
// Display order: all VIPs first (in arrival order), then all Regulars.
Customer getAtPosition(int pos) {
    if (pos <= (int)vipQueue.size())
        return vipQueue[pos - 1];
    return regularQueue[pos - 1 - (int)vipQueue.size()];
}

// Removes the customer at the given 1-based display position.
// Returns the removed customer and the position for undo.
Customer removeAtPosition(int pos) {
    if (pos <= (int)vipQueue.size()) {
        Customer c = vipQueue[pos - 1];
        vipQueue.erase(vipQueue.begin() + (pos - 1));
        return c;
    }
    int rIdx = pos - 1 - (int)vipQueue.size();
    Customer c = regularQueue[rIdx];
    regularQueue.erase(regularQueue.begin() + rIdx);
    return c;
}

void displayAll() {
    if (totalBookings() == 0) {
        cout << "\nNo current bookings.\n";
    } else {
        cout << "\n--- BOOKINGS ---\n";
        int num = 1;
        for (const Customer &c : vipQueue)
            cout << "  " << num++ << ". " << c.name << " (VIP)\n";
        for (const Customer &c : regularQueue)
            cout << "  " << num++ << ". " << c.name << " (Regular)\n";
    }
    cout << "Seats used: " << currentSeats << "/" << maxSeats << "\n";
}

// ─────────────────────────────────────────
//  INPUT HELPER
// ─────────────────────────────────────────

bool readInt(int &value) {
    if (!(cin >> value)) {
        if (cin.eof()) return false;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

// ─────────────────────────────────────────
//  BOOK
// ─────────────────────────────────────────

void bookTicket() {
    if (currentSeats >= maxSeats) {
        cout << "No available seats.\n";
        return;
    }

    Customer c;
    cout << "Enter name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (!getline(cin, c.name) || c.name.empty()) {
        cout << "No name provided. Returning to menu.\n";
        return;
    }

    int membership;
    cout << "Membership? (1=Yes, 0=No): ";
    if (!readInt(membership)) {
        cout << "No input provided. Returning to menu.\n";
        return;
    }

    bool isVIP = (membership == 1);
    if (!isVIP) {
        int vipTicket;
        cout << "VIP Ticket? (1=Yes, 0=No): ";
        if (!readInt(vipTicket)) {
            cout << "No input provided. Returning to menu.\n";
            return;
        }
        isVIP = (vipTicket == 1);
    }

    c.isVIP = isVIP;

    int position;
    if (c.isVIP) {
        vipQueue.push_back(c);
        position = (int)vipQueue.size(); // position among VIPs = end of VIP block
    } else {
        regularQueue.push_back(c);
        position = totalBookings(); // position at end of full list
    }

    ++currentSeats;
    undoStack.push({"BOOK", c, position});

    cout << "Booked at position " << position << ": "
         << c.name << (c.isVIP ? " (VIP)\n" : " (Regular)\n");
}

// ─────────────────────────────────────────
//  PROCESS NEXT
// ─────────────────────────────────────────

void processBooking() {
    if (totalBookings() == 0) {
        cout << "No bookings to process.\n";
        return;
    }

    // Always position 1 — VIP front if any, otherwise Regular front.
    Customer c = getAtPosition(1);
    removeAtPosition(1);
    --currentSeats;

    undoStack.push({"PROCESS", c, 1});

    cout << "Processed: " << c.name
         << (c.isVIP ? " (VIP)\n" : " (Regular)\n");
}

// ─────────────────────────────────────────
//  CANCEL
// ─────────────────────────────────────────

void cancelBooking() {
    if (totalBookings() == 0) {
        cout << "No bookings to cancel.\n";
        return;
    }

    displayAll();

    int pos;
    cout << "Enter position number to cancel: ";
    if (!readInt(pos)) {
        cout << "No input provided. Returning to menu.\n";
        return;
    }

    if (pos < 1 || pos > totalBookings()) {
        cout << "Invalid position. Returning to menu.\n";
        return;
    }

    Customer removed = removeAtPosition(pos);
    --currentSeats;

    undoStack.push({"CANCEL", removed, pos});

    cout << "Cancelled: " << removed.name
         << (removed.isVIP ? " (VIP)\n" : " (Regular)\n");
}

// ─────────────────────────────────────────
//  UNDO
// ─────────────────────────────────────────

void undoAction() {
    if (undoStack.empty()) {
        cout << "Nothing to undo.\n";
        return;
    }

    Action last = undoStack.top();
    undoStack.pop();
    Customer c = last.customer;

    if (last.type == "BOOK") {
        // Undo booking: remove the last entry from the correct queue.
        if (c.isVIP) vipQueue.pop_back();
        else         regularQueue.pop_back();
        --currentSeats;
        cout << "Undo BOOK: Removed " << c.name
             << (c.isVIP ? " (VIP)\n" : " (Regular)\n");
    }
    else if (last.type == "CANCEL" || last.type == "PROCESS") {
        // Undo cancel/process: restore at original position.
        if (c.isVIP) {
            // last.position is within the VIP block (1-based)
            int vipIdx = min(last.position - 1, (int)vipQueue.size());
            vipQueue.insert(vipQueue.begin() + vipIdx, c);
        } else {
            // last.position is in the full list; offset by vip count to get regular index
            int rIdx = min(last.position - 1 - (int)vipQueue.size(),
                           (int)regularQueue.size());
            rIdx = max(rIdx, 0);
            regularQueue.insert(regularQueue.begin() + rIdx, c);
        }
        ++currentSeats;
        cout << "Undo " << last.type << ": Restored " << c.name
             << (c.isVIP ? " (VIP)\n" : " (Regular)\n");
    }
}

// ─────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────

int main() {
    int choice;
    do {
        cout << "\n==== EVENT TICKET SYSTEM ====\n";
        cout << "1. Book Ticket\n";
        cout << "2. Process Next Booking\n";
        cout << "3. Cancel Booking\n";
        cout << "4. Undo Last Action\n";
        cout << "5. Display All Bookings\n";
        cout << "6. Exit\n";
        cout << "Choose: ";
        if (!readInt(choice)) break;

        switch (choice) {
            case 1: bookTicket();     break;
            case 2: processBooking(); break;
            case 3: cancelBooking();  break;
            case 4: undoAction();     break;
            case 5: displayAll();     break;
            case 6: break;
            default:
                cout << "Invalid option. Choose 1–6.\n";
        }
    } while (choice != 6 && cin);

    return 0;
}