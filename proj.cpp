#include <iostream>
#include <limits>
#include <string>
#include <deque>
#include <stack>
#include <vector>

using namespace std;

/* =========================
   DATA STRUCTURES
   ========================= */

// Represents a customer booking
struct Customer {
    string name;
    bool isVIP; // true = VIP, false = Regular
};

// Stores actions for UNDO functionality
struct Action {
    string type;      // BOOK, CANCEL, PROCESS, CANCEL_PROCESSED
    Customer customer;
    int position;     // original position (used for restoring order)
};

/* =========================
   QUEUES / STORAGE
   ========================= */

deque<Customer> vipQueue;       // VIP customers queue
deque<Customer> regularQueue;   // Regular customers queue

vector<Customer> processedList; // Customers already seated

stack<Action> undoStack;        // Stores history for undo (LIFO)

/* =========================
   GLOBAL VARIABLES
   ========================= */

int maxSeats = 10;      // Maximum allowed seats
int currentSeats = 0;   // Current occupied seats

/* =========================
   HELPER FUNCTIONS
   ========================= */

// Returns total customers waiting
int totalBookings() {
    return vipQueue.size() + regularQueue.size();
}

// Gets customer at global position (VIP first, then Regular)
Customer getAtPosition(int pos) {
    if (pos <= (int)vipQueue.size())
        return vipQueue[pos - 1];
    return regularQueue[pos - 1 - vipQueue.size()];
}

// Removes customer at global position
Customer removeAtPosition(int pos) {
    if (pos <= (int)vipQueue.size()) {
        Customer c = vipQueue[pos - 1];
        vipQueue.erase(vipQueue.begin() + (pos - 1));
        return c;
    }

    int rIdx = pos - 1 - vipQueue.size();
    Customer c = regularQueue[rIdx];
    regularQueue.erase(regularQueue.begin() + rIdx);
    return c;
}

/* =========================
   DISPLAY FUNCTION
   ========================= */

// Shows all queues and seated customers
void displayAll() {
    cout << "\n--- QUEUE ---\n";

    if (totalBookings() == 0) {
        cout << "  (empty)\n";
    } else {
        int num = 1;

        // Display VIP first
        for (const Customer &c : vipQueue)
            cout << "  " << num++ << ". " << c.name << " (VIP)\n";

        // Then regular
        for (const Customer &c : regularQueue)
            cout << "  " << num++ << ". " << c.name << " (Regular)\n";
    }

    cout << "\n--- SEATED ---\n";

    if (processedList.empty()) {
        cout << "  (empty)\n";
    } else {
        for (int i = 0; i < (int)processedList.size(); i++)
            cout << "  " << i + 1 << ". " << processedList[i].name
                 << (processedList[i].isVIP ? " (VIP)\n" : " (Regular)\n");
    }

    cout << "\nSeats occupied: " << currentSeats << "/" << maxSeats << "\n";
}

/* =========================
   INPUT VALIDATION
   ========================= */

// Safely reads integer input
bool readInt(int &value) {
    if (!(cin >> value)) {
        if (cin.eof()) return false;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

/* =========================
   BOOKING FUNCTION
   ========================= */

void bookTicket() {
    if (currentSeats >= maxSeats) {
        cout << "No available seats — venue is full.\n";
        return;
    }

    Customer c;

    // Get customer name
    cout << "Enter name: ";
    getline(cin, c.name);

    // Get membership status
    int membership;
    cout << "Membership? (1=Yes, 0=No): ";
    if (!readInt(membership)) return;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    bool isVIP = (membership == 1);

    // Optional VIP ticket upgrade
    if (!isVIP) {
        int vipTicket;
        cout << "VIP Ticket? (1=Yes, 0=No): ";
        if (!readInt(vipTicket)) return;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        isVIP = (vipTicket == 1);
    }

    c.isVIP = isVIP;

    int position;

    // Add to correct queue
    if (c.isVIP) {
        vipQueue.push_back(c);
        position = vipQueue.size();
    } else {
        regularQueue.push_back(c);
        position = totalBookings();
    }

    // Save action for undo
    undoStack.push({"BOOK", c, position});

    cout << "Queued at position " << position << ": " << c.name << "\n";
}

/* =========================
   PROCESS BOOKING
   ========================= */

void processBooking() {
    if (totalBookings() == 0) {
        cout << "No one in the queue.\n";
        return;
    }

    if (currentSeats >= maxSeats) {
        cout << "Venue full.\n";
        return;
    }

    // Always process first customer (VIP priority already handled)
    Customer c = getAtPosition(1);
    removeAtPosition(1);

    processedList.push_back(c);
    currentSeats++;

    undoStack.push({"PROCESS", c, (int)processedList.size()});

    cout << "Seated: " << c.name << "\n";
}

/* =========================
   CANCEL BOOKING
   ========================= */

void cancelBooking() {
    if (totalBookings() == 0 && processedList.empty()) {
        cout << "Nothing to cancel.\n";
        return;
    }

    int section;

    // Choose where to cancel from
    cout << "1. Queue\n2. Seated\nChoice: ";
    if (!readInt(section)) return;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (section == 1) {
        int pos;
        cout << "Enter position: ";
        if (!readInt(pos)) return;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        Customer removed = removeAtPosition(pos);
        undoStack.push({"CANCEL", removed, pos});

        cout << "Cancelled: " << removed.name << "\n";

    } else {
        int pos;
        cout << "Enter seated position: ";
        if (!readInt(pos)) return;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        Customer removed = processedList[pos - 1];
        processedList.erase(processedList.begin() + (pos - 1));

        currentSeats--;
        undoStack.push({"CANCEL_PROCESSED", removed, pos});

        cout << "Cancelled seated: " << removed.name << "\n";
    }
}

/* =========================
   UNDO FUNCTION
   ========================= */

void undoAction() {
    if (undoStack.empty()) {
        cout << "Nothing to undo.\n";
        return;
    }

    Action last = undoStack.top();
    undoStack.pop();

    Customer c = last.customer;

    if (last.type == "BOOK") {
        // Remove last booking
        if (c.isVIP) vipQueue.pop_back();
        else regularQueue.pop_back();

    } else if (last.type == "PROCESS") {
        // Move back to queue
        processedList.pop_back();
        currentSeats--;

        if (c.isVIP) vipQueue.push_front(c);
        else regularQueue.push_front(c);

    } else if (last.type == "CANCEL") {
        // Restore cancelled booking
        if (c.isVIP) vipQueue.push_back(c);
        else regularQueue.push_back(c);

    } else if (last.type == "CANCEL_PROCESSED") {
        // Restore seated customer
        processedList.push_back(c);
        currentSeats++;
    }

    cout << "Undo successful.\n";
}

/* =========================
   MAIN MENU
   ========================= */

int main() {
    int choice;

    do {
        cout << "\n==== EVENT TICKET SYSTEM ====\n";
        cout << "1. Book Ticket\n";
        cout << "2. Process Booking\n";
        cout << "3. Cancel Booking\n";
        cout << "4. Undo\n";
        cout << "5. Display\n";
        cout << "6. Exit\n";
        cout << "Choice: ";

        if (!readInt(choice)) break;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: bookTicket(); break;
            case 2: processBooking(); break;
            case 3: cancelBooking(); break;
            case 4: undoAction(); break;
            case 5: displayAll(); break;
            case 6: break;
            default: cout << "Invalid choice.\n";
        }

    } while (choice != 6);

    return 0;
}