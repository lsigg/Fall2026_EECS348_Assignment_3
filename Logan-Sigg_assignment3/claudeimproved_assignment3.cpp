/*
 * Program: EECS 348 Assignment 3
 *
 * Description:
 * This C++ program manages and prioritizes emails for a busy company CEO
 * The program implements a MaxHeap from scratch using a list-based
 * implementation with a vector Emails are prioritized first according
 * to sender category and then according to date Higher-priority sender
 * categories are read before lower-priority categories If two emails
 * have the same sender category, the newest email is given priority
 *
 * Emails with the same sender category and date are read in arrival order
 *
 * Inputs:
 * Standard input or a named file containing EMAIL, NEXT, READ, and COUNT commands
 *
 * Outputs:
 * NEXT displays the highest-priority unread email
 * READ removes the highest-priority unread email
 * COUNT displays the number of unread emails
 *
 * Collaborators:
 * None
 *
 * Other Sources:
 * Claude was used to generate the initial C++ implementation
 * The assignment instructions provided the program requirements
 *
 * Author: Logan Sigg
 * Creation Date: 09/24/2026
 * Revision Date: 09/28/2026
 * Revisions: Added arrival-order tie-breaking, file input support,
 * input validation, private email fields, and separate command processing
 * Updated the comments to match the other implementations
 *
 */

/*
 * These libraries provide strings, list-based storage with vector,
 * terminal I/O, file handling, date parsing, move operations, and exceptions
 */
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <stdexcept>

/*
 * This statement permits standard-library names to be used without std::
 */
using namespace std;

/*
 * The Email class represents one email in the CEO's inbox
 * Each object stores private email fields, priority keys, and arrival order
 */
class Email {
private:

    /* The sender category string, for example "Boss", "Peer", etc */
    string senderCategory;

    /* The subject line of the email (may contain spaces, not commas) */
    string subject;

    /* The date string in MM-DD-YYYY format, kept for display purposes */
    string date;

    /* Numeric priority rank: higher number = higher priority to read */
    int priorityRank;

    /* Numeric date key in YYYYMMDD form, used to break priority ties */
    long dateKey;

    /* Arrival order, used as a final tie-breaker so results are deterministic */
    long arrivalSequence;

    /* Convert the sender category into a numerical heap priority using a lookup table */
    static int categoryToRank(const string &category) {

        /* Pairs of (category name, rank), ordered from highest to lowest */
        static const vector<pair<string, int>> rankTable = {
            {"Boss", 5},
            {"Subordinate", 4},
            {"Peer", 3},
            {"ImportantPerson", 2},
            {"OtherPerson", 1}
        };

        /* Search the table for a matching category name */
        for (const auto &entry : rankTable) {

            /* If the category matches, return its associated rank */
            if (entry.first == category) {
                return entry.second;
            }
        }

        /* Unrecognized categories default to the lowest known rank */
        return 1;
    }

    /*
     * Convert MM-DD-YYYY into YYYYMMDD so newer dates have larger values
     * Missing date components and failed numeric conversions raise exceptions
     */
    static long dateToKey(const string &dt) {

        /* Use a string stream to parse the delimited date components */
        stringstream ss(dt);

        /* Temporary string to hold each token between dashes */
        string token;

        /* Will hold the numeric month, day, and year once parsed */
        int month = 0, day = 0, year = 0;

        /* Read the month token, up to the first '-' delimiter */
        if (!getline(ss, token, '-')) throw invalid_argument("bad date: " + dt);
        month = stoi(token);

        /* Read the day token, up to the second '-' delimiter */
        if (!getline(ss, token, '-')) throw invalid_argument("bad date: " + dt);
        day = stoi(token);

        /* Read the year token, the remainder of the string */
        if (!getline(ss, token)) throw invalid_argument("bad date: " + dt);
        year = stoi(token);

        /* Combine into YYYYMMDD so later dates are numerically bigger */
        return (static_cast<long>(year) * 10000L) + (month * 100L) + day;
    }

public:

    /* Default constructor; leaves fields empty/zero (needed for vector resize) */
    Email() : senderCategory(""), subject(""), date(""), priorityRank(0),
               dateKey(0), arrivalSequence(0) {}

    /* Construct an Email object and compute its priority keys and arrival order */
    Email(const string &category, const string &subj, const string &dt, long seq) {

        /* Store the raw sender category as given */
        senderCategory = category;

        /* Store the raw subject line as given */
        subject = subj;

        /* Store the raw date string as given */
        date = dt;

        /* Record when (in insertion order) this email arrived */
        arrivalSequence = seq;

        /* Convert the category into a numeric rank via helper function */
        priorityRank = categoryToRank(category);

        /* Convert the MM-DD-YYYY date string into a comparable integer */
        dateKey = dateToKey(dt);
    }

    /* Returns this email's sender category */
    const string& getSenderCategory() const { return senderCategory; }

    /* Returns this email's subject line */
    const string& getSubject() const { return subject; }

    /* Returns this email's date string */
    const string& getDate() const { return date; }

    /*
     * Return true when this email has higher priority than another email
     * Compare sender priority, date, and then arrival order
     */
    bool hasHigherPriorityThan(const Email &other) const {

        /* If the priority ranks differ, the larger rank wins outright */
        if (priorityRank != other.priorityRank) {
            return priorityRank > other.priorityRank;
        }

        /* If the date keys differ, the newer (larger) date key wins */
        if (dateKey != other.dateKey) {
            return dateKey > other.dateKey;
        }

        /* Both category and date are tied; earlier arrival is read first */
        return arrivalSequence < other.arrivalSequence;
    }
};

/*
 * The MaxHeap class implements the email priority queue from scratch
 * A vector provides the required list-based heap representation
 */
class MaxHeap {
private:

    /* The list (dynamic array) backing the heap; index 0 is the heap root */
    vector<Email> items;

    /* Returns the parent index of the given index */
    int parentIndex(int i) const { return (i - 1) / 2; }

    /* Returns the left child index of the given index */
    int leftChildIndex(int i) const { return (2 * i) + 1; }

    /* Returns the right child index of the given index */
    int rightChildIndex(int i) const { return (2 * i) + 2; }

    /* Move a newly inserted email upward until the MaxHeap property is restored */
    void siftUp(int i) {

        /* Keep bubbling up while we're not at the root (index > 0) */
        while (i > 0) {

            /* Compute this node's parent index */
            int p = parentIndex(i);

            /* Stop once the child no longer outranks its parent */
            if (!items[i].hasHigherPriorityThan(items[p])) {
                break;
            }

            /* Otherwise swap child and parent to move it closer to root */
            swap(items[i], items[p]);

            /* Continue the loop from the parent's old position */
            i = p;
        }
    }

    /* Move a root replacement downward until the MaxHeap property is restored */
    void siftDown(int i) {

        /* Remember how many items are currently in the heap */
        int n = static_cast<int>(items.size());

        /* Loop until this node has no children that outrank it */
        while (true) {

            /* Assume this node is the largest among itself and its children */
            int largest = i;

            /* Compute the indices of the left and right children */
            int left = leftChildIndex(i);
            int right = rightChildIndex(i);

            /* If the left child exists and outranks current largest, update */
            if (left < n && items[left].hasHigherPriorityThan(items[largest])) {
                largest = left;
            }

            /* If the right child exists and outranks current largest, update */
            if (right < n && items[right].hasHigherPriorityThan(items[largest])) {
                largest = right;
            }

            /* If neither child outranks this node, the heap property holds */
            if (largest == i) {
                break;
            }

            /* Otherwise swap this node with the higher-priority child */
            swap(items[i], items[largest]);

            /* Continue sifting down from the child's new position */
            i = largest;
        }
    }

public:

    /* Inserts a new Email into the heap, preserving the max-heap property */
    void insert(const Email &e) {

        /* Append the new email to the end of the underlying list */
        items.push_back(e);

        /* Restore heap order by sifting the new element up as needed */
        siftUp(static_cast<int>(items.size()) - 1);
    }

    /* Returns true if there are no emails currently in the heap */
    bool isEmpty() const { return items.empty(); }

    /* Returns the number of emails currently in the heap */
    int size() const { return static_cast<int>(items.size()); }

    /*
     * Return a reference to the highest-priority email without removing it
     * The caller must check that the heap is not empty
     */
    const Email& peekMax() const { return items[0]; }

    /*
     * Remove the highest-priority email and restore heap order
     * Move the final email into the root to avoid copying its strings
     * The caller must check that the heap is not empty
     */
    void extractMax() {

        /* Move (not copy) the last element into the root position */
        items[0] = std::move(items.back());

        /* Remove the now-moved-from last element from the list */
        items.pop_back();

        /* If any elements remain, restore heap order from the root down */
        if (!items.empty()) {
            siftDown(0);
        }
    }
};

/* The CommandProcessor class parses input commands and controls the MaxHeap */
class CommandProcessor {
private:

    /* The MaxHeap representing the CEO's inbox */
    MaxHeap inbox;

    /* Running count of emails inserted, used as the arrival tie-breaker */
    long nextArrivalSequence = 0;

    /* Removes a trailing '\r' (present in Windows-style line endings) */
    static string stripCarriageReturn(const string &line) {

        /* If the line ends with '\r', return everything before it */
        if (!line.empty() && line.back() == '\r') {
            return line.substr(0, line.size() - 1);
        }

        /* Otherwise the line is already clean; return it unchanged */
        return line;
    }

    /*
     * Parse the sender category, subject, and date from an EMAIL command
     * Return false when the two required commas are missing
     */
    static bool splitCommaFields(const string &rest, string &category,
                                  string &subject, string &date) {

        /* Find the position of the first comma */
        size_t firstComma = rest.find(',');

        /* Find the position of the last comma */
        size_t lastComma = rest.rfind(',');

        /* Fail if there are fewer than two commas, or they're the same one */
        if (firstComma == string::npos || lastComma == string::npos ||
            firstComma == lastComma) {
            return false;
        }

        /* Extract the category as everything before the first comma */
        category = rest.substr(0, firstComma);

        /* Extract the subject as everything between the first and last comma */
        subject = rest.substr(firstComma + 1, lastComma - firstComma - 1);

        /* Extract the date as everything after the last comma */
        date = rest.substr(lastComma + 1);

        /* All three fields were successfully extracted */
        return true;
    }

    /* Parses and handles a single EMAIL command line */
    void handleEmailCommand(const string &line, size_t spacePos) {

        /* Everything after "EMAIL " is the comma-delimited payload */
        string rest = line.substr(spacePos + 1);

        /* Declare holders for the three parsed fields */
        string category, subject, date;

        /* Bail out on a malformed line rather than crashing */
        if (!splitCommaFields(rest, category, subject, date)) {
            cerr << "Warning: skipping malformed EMAIL line: " << line << endl;
            return;
        }

        /* Try to build and insert the Email; catch bad date formats too */
        try {
            Email e(category, subject, date, nextArrivalSequence++);
            inbox.insert(e);
        } catch (const invalid_argument &ex) {

            /* Report the problem and skip this email instead of crashing */
            cerr << "Warning: skipping EMAIL with bad date: " << line << endl;
        }
    }

    /* Handles a NEXT command: show the top email without removing it */
    void handleNextCommand() {

        /* If there are no emails, say so instead of crashing */
        if (inbox.isEmpty()) {
            cout << "No emails to read." << endl;
            return;
        }

        /* Look at (but do not remove) the highest-priority email */
        const Email &top = inbox.peekMax();

        /* Print the header line for a NEXT display */
        cout << "Next email:" << endl;

        /* Print the sender category */
        cout << "Sender: " << top.getSenderCategory() << endl;

        /* Print the subject line */
        cout << "Subject: " << top.getSubject() << endl;

        /* Print the date */
        cout << "Date: " << top.getDate() << endl;
    }

    /* Handles a READ command: remove the top email without displaying it */
    void handleReadCommand() {

        /* Only remove if there is something to remove */
        if (!inbox.isEmpty()) {
            inbox.extractMax();
        }

        /* If the inbox was already empty, READ is simply a no-op */
    }

    /* Handles a COUNT command: display how many emails remain unread */
    void handleCountCommand() {

        /* Print the count using the exact phrasing from the spec */
        cout << "There are " << inbox.size() << " emails to read." << endl;
    }

public:

    /* Reads and dispatches every command line from the given input stream */
    void processStream(istream &in) {

        /* Buffer to hold each line read from the stream */
        string rawLine;

        /* Loop over every line of input until end-of-stream is reached */
        while (getline(in, rawLine)) {

            /* Strip a trailing '\r' left by Windows-style line endings */
            string line = stripCarriageReturn(rawLine);

            /* Skip completely empty lines (for example, trailing blank lines) */
            if (line.empty()) {
                continue;
            }

            /* Find where the first space is, separating the command keyword */
            size_t spacePos = line.find(' ');

            /* Extract the command keyword (EMAIL/NEXT/READ/COUNT) */
            string command = (spacePos == string::npos) ? line : line.substr(0, spacePos);

            /* Dispatch to the appropriate handler based on the keyword */
            if (command == "EMAIL") {
                handleEmailCommand(line, spacePos);
            } else if (command == "NEXT") {
                handleNextCommand();
            } else if (command == "READ") {
                handleReadCommand();
            } else if (command == "COUNT") {
                handleCountCommand();
            }

            /* Any unrecognized command is silently ignored (defensive coding) */
        }
    }
};

/*
 * The main method starts the program and selects the input stream
 * A filename argument selects file input; otherwise standard input is used
 */
int main(int argc, char *argv[]) {

    /* Create the command processor that owns the inbox and dispatch logic */
    CommandProcessor processor;

    /* If a filename was supplied on the command line, read from that file */
    if (argc >= 2) {

        /* Open the requested input file */
        ifstream inputFile(argv[1]);

        /* Check whether the file opened successfully */
        if (!inputFile.is_open()) {

            /* Report the failure and exit with a non-zero status */
            cerr << "Error: unable to open input file: " << argv[1] << endl;
            return 1;
        }

        /* Process every command line from the file */
        processor.processStream(inputFile);
    } else {

        /* No filename was given, so read commands from standard input */
        processor.processStream(cin);
    }

    /* Signal successful completion to the operating system */
    return 0;
}
