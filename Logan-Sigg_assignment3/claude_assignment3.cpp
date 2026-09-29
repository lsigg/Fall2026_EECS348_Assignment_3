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
 * Inputs:
 * Standard input containing EMAIL, NEXT, READ, and COUNT commands
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
 * Revision Date: 10/1/2026
 * Revisions: Updated the comments to match the ChatGPT implementation
 *
 */

/*
 * These libraries provide strings, list-based storage with vector,
 * terminal I/O, and date parsing with stringstream
 */
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

/*
 * This statement permits standard-library names to be used without std::
 */
using namespace std;

/*
 * The Email class represents one email in the CEO's inbox
 * Each object stores its sender category, subject, date, and priority keys
 */
class Email {
public:

    /* The sender category string, eg "Boss", "Peer", etc */
    string senderCategory;

    /* The subject line of the email (may contain spaces, not commas) */
    string subject;

    /* The date string in MM-DD-YYYY format, kept for display purposes */
    string date;

    /* Numeric priority rank: higher number = higher priority to read */
    int priorityRank;

    /* Numeric date key in YYYYMMDD form, used to break priority ties */
    long dateKey;

    /* Default constructor; leaves fields empty/zero (needed for vector resize) */
    Email() : senderCategory(""), subject(""), date(""), priorityRank(0), dateKey(0) {}

    /* Construct an Email object and compute its sender priority and date key */
    Email(const string &category, const string &subj, const string &dt) {

        /* Store the raw sender category as given */
        senderCategory = category;

        /* Store the raw subject line as given */
        subject = subj;

        /* Store the raw date string as given */
        date = dt;

        /* Convert the category into a numeric rank via helper function */
        priorityRank = categoryToRank(category);

        /* Convert the MM-DD-YYYY date string into a comparable integer */
        dateKey = dateToKey(dt);
    }

    /* Convert the sender category into a numerical heap priority */
    static int categoryToRank(const string &category) {

        /* Boss is read first, so it gets the highest rank */
        if (category == "Boss") return 5;

        /* Subordinate is read next */
        if (category == "Subordinate") return 4;

        /* Peer is read next */
        if (category == "Peer") return 3;

        /* ImportantPerson is read next */
        if (category == "ImportantPerson") return 2;

        /* OtherPerson (and any unrecognized category) is read last */
        return 1;
    }

    /* Convert MM-DD-YYYY into YYYYMMDD so newer dates have larger values */
    static long dateToKey(const string &dt) {

        /* Use a string stream to parse the delimited date components */
        stringstream ss(dt);

        /* Temporary string to hold each token between dashes */
        string token;

        /* Will hold the numeric month, day, and year once parsed */
        int month = 0, day = 0, year = 0;

        /* Read the month token, up to the first '-' delimiter */
        if (getline(ss, token, '-')) month = stoi(token);

        /* Read the day token, up to the second '-' delimiter */
        if (getline(ss, token, '-')) day = stoi(token);

        /* Read the year token, the remainder of the string */
        if (getline(ss, token, '-')) year = stoi(token);

        /* Combine into YYYYMMDD so later dates are numerically bigger */
        return (static_cast<long>(year) * 10000L) + (month * 100L) + day;
    }

    /* Return true when this email has higher priority than another email */
    bool hasHigherPriorityThan(const Email &other) const {

        /* If the priority ranks differ, the larger rank wins outright */
        if (priorityRank != other.priorityRank) {
            return priorityRank > other.priorityRank;
        }

        /* Ranks are tied, so the newer (larger) date key wins instead */
        return dateKey > other.dateKey;
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
    int parentIndex(int i) const {

        /* Integer division rounds down, matching the standard heap formula */
        return (i - 1) / 2;
    }

    /* Returns the left child index of the given index */
    int leftChildIndex(int i) const {

        /* Standard array-heap left-child formula */
        return (2 * i) + 1;
    }

    /* Returns the right child index of the given index */
    int rightChildIndex(int i) const {

        /* Standard array-heap right-child formula */
        return (2 * i) + 2;
    }

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
    bool isEmpty() const {

        /* An empty underlying list means an empty heap */
        return items.empty();
    }

    /* Returns the number of emails currently in the heap */
    int size() const {

        /* The list's size is exactly the heap's element count */
        return static_cast<int>(items.size());
    }

    /*
     * Return the highest-priority email without removing it
     * The caller must check that the heap is not empty
     */
    Email peekMax() const {

        /* The root of a max-heap is always stored at index 0 */
        return items[0];
    }

    /*
     * Remove the highest-priority email and restore heap order
     * The caller must check that the heap is not empty
     */
    void extractMax() {

        /* Move the last element into the root position */
        items[0] = items.back();

        /* Remove the now-duplicated last element from the list */
        items.pop_back();

        /* If any elements remain, restore heap order from the root down */
        if (!items.empty()) {
            siftDown(0);
        }
    }
};

/*
 * The splitCommaFields function parses the fields of an EMAIL command
 * It extracts the sender category, subject line, and date
 */
void splitCommaFields(const string &rest, string &category, string &subject, string &date) {

    /* Find the position of the first comma, separating category from subject */
    size_t firstComma = rest.find(',');

    /* Find the position of the last comma, separating subject from date */
    size_t lastComma = rest.rfind(',');

    /* Extract the category as everything before the first comma */
    category = rest.substr(0, firstComma);

    /* Extract the subject as everything between the first and last comma */
    subject = rest.substr(firstComma + 1, lastComma - firstComma - 1);

    /* Extract the date as everything after the last comma */
    date = rest.substr(lastComma + 1);
}

/*
 * The main method starts the program and processes standard input
 */
int main() {

    /* Create the single MaxHeap instance representing the CEO's inbox */
    MaxHeap inbox;

    /* Buffer to hold each line read from standard input */
    string line;

    /* Loop over every line of input until end-of-file is reached */
    while (getline(cin, line)) {

        /* Skip completely empty lines (eg, trailing blank lines) */
        if (line.empty()) {
            continue;
        }

        /* Find where the first space is, separating the command keyword */
        size_t spacePos = line.find(' ');

        /* Extract the command keyword (EMAIL/NEXT/READ/COUNT) */
        string command = (spacePos == string::npos) ? line : line.substr(0, spacePos);

        /* Handle the EMAIL command: parse fields and insert into the heap */
        if (command == "EMAIL") {

            /* Everything after "EMAIL " is the comma-delimited payload */
            string rest = line.substr(spacePos + 1);

            /* Declare holders for the three parsed fields */
            string category, subject, date;

            /* Perform the actual comma-based splitting */
            splitCommaFields(rest, category, subject, date);

            /* Construct a new Email object from the parsed fields */
            Email e(category, subject, date);

            /* Insert the new email into the priority queue */
            inbox.insert(e);
        }

        /* Handle the NEXT command: show the top email without removing it */
        else if (command == "NEXT") {

            /* If there are no emails, say so instead of crashing */
            if (inbox.isEmpty()) {
                cout << "No emails to read." << endl;
            } else {

                /* Look at (but do not remove) the highest-priority email */
                Email top = inbox.peekMax();

                /* Print the header line for a NEXT display */
                cout << "Next email:" << endl;

                /* Print the sender category */
                cout << "Sender: " << top.senderCategory << endl;

                /* Print the subject line */
                cout << "Subject: " << top.subject << endl;

                /* Print the date */
                cout << "Date: " << top.date << endl;
            }
        }

        /* Handle the READ command: remove the top email without displaying it */
        else if (command == "READ") {

            /* Only remove if there is something to remove */
            if (!inbox.isEmpty()) {

                /* Delete the highest-priority email from the heap silently */
                inbox.extractMax();
            } else {

                /* Display an empty-inbox message */
                cout << "No emails to read." << endl;
            }
        }

        /* Handle the COUNT command: display how many emails remain unread */
        else if (command == "COUNT") {

            /* Print the count using the exact phrasing from the spec */
            cout << "There are " << inbox.size() << " emails to read." << endl;
        }

        /* Any unrecognized command is silently ignored (defensive coding) */
    }

    /* Signal successful completion to the operating system */
    return 0;
}
