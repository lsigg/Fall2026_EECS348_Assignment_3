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
 * A text file containing EMAIL, NEXT, READ, and COUNT commands
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
 * ChatGPT was used to generate the initial C++ implementation
 * The assignment instructions provided the program requirements
 *
 * Author: Logan Sigg
 * Creation Date: 09/24/2026
 * Revision Date: 10/1/2026
 * Revisions: Initial implementation of the C++ MaxHeap email priority queue
 *
 */

/*
 * These libraries provide terminal I/O, file handling, strings,
 * list-based storage with vector, and date parsing with stringstream
 * Source: ChatGPT-generated code reviewed by the author
 */
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

/*
 * This statement permits standard-library names to be used without std::
 */
using namespace std;

/*
 * The Email class represents one email in the CEO's inbox
 * Each object stores its sender category, subject, and date
 */
class Email
{
private:
    /* Store the email's sender category */
    string senderCategory;

    /* Store the email's subject line */
    string subject;

    /* Store the email's date in MM-DD-YYYY format */
    string date;

public:
    /* Construct an empty Email object */
    Email()
    {
        /* Initialize the sender category to an empty string */
        senderCategory = "";

        /* Initialize the subject to an empty string */
        subject = "";

        /* Initialize the date to an empty string */
        date = "";
    }

    /* Construct an Email object using the supplied email information */
    Email(string sender, string emailSubject, string emailDate)
    {
        /* Store the supplied sender category */
        senderCategory = sender;

        /* Store the supplied subject line */
        subject = emailSubject;

        /* Store the supplied date */
        date = emailDate;
    }

    /* Return this email's sender category */
    string getSenderCategory() const
    {
        /* Return the stored sender category */
        return senderCategory;
    }

    /* Return this email's subject line */
    string getSubject() const
    {
        /* Return the stored subject */
        return subject;
    }

    /* Return this email's date */
    string getDate() const
    {
        /* Return the stored date */
        return date;
    }

    /* Convert the sender category into a numerical heap priority */
    int getSenderPriority() const
    {
        /* Give Boss emails the highest sender priority */
        if (senderCategory == "Boss")
        {
            /* Return the Boss priority */
            return 5;
        }

        /* Give Subordinate emails the second-highest sender priority */
        if (senderCategory == "Subordinate")
        {
            /* Return the Subordinate priority */
            return 4;
        }

        /* Give Peer emails the third-highest sender priority */
        if (senderCategory == "Peer")
        {
            /* Return the Peer priority */
            return 3;
        }

        /* Give ImportantPerson emails the fourth-highest sender priority */
        if (senderCategory == "ImportantPerson")
        {
            /* Return the ImportantPerson priority */
            return 2;
        }

        /* OtherPerson is the remaining valid category and has lowest priority */
        return 1;
    }

    /* Convert MM-DD-YYYY into YYYYMMDD so newer dates have larger values */
    int getDateValue() const
    {
        /* Store the month value */
        int month;

        /* Store the day value */
        int day;

        /* Store the year value */
        int year;

        /* Store the first date separator */
        char dash1;

        /* Store the second date separator */
        char dash2;

        /* Create a stream that reads from the date string */
        stringstream dateStream(date);

        /* Extract the month, separators, day, and year */
        dateStream >> month >> dash1 >> day >> dash2 >> year;

        /* Return the date in sortable YYYYMMDD integer form */
        return (year * 10000) + (month * 100) + day;
    }

    /* Return true when this email has higher priority than another email */
    bool hasHigherPriorityThan(const Email& other) const
    {
        /* Compare sender priorities first when the categories differ */
        if (getSenderPriority() != other.getSenderPriority())
        {
            /* The larger sender-priority value wins */
            return getSenderPriority() > other.getSenderPriority();
        }

        /* For equal sender priorities, the newer date wins */
        return getDateValue() > other.getDateValue();
    }
};

/*
 * The MaxHeap class implements the email priority queue from scratch
 * A vector provides the required list-based heap representation
 */
class MaxHeap
{
private:
    /* Store all unread emails in list-based heap order */
    vector<Email> heap;

    /* Move a newly inserted email upward until the MaxHeap property is restored */
    void heapifyUp(int index)
    {
        /* Continue until the item reaches the root or no longer outranks its parent */
        while (index > 0)
        {
            /* Calculate the parent index */
            int parentIndex = (index - 1) / 2;

            /* Stop when the current email does not outrank its parent */
            if (!heap[index].hasHigherPriorityThan(heap[parentIndex]))
            {
                /* Leave the loop because the heap property is satisfied */
                break;
            }

            /* Temporarily save the parent email */
            Email temporary = heap[parentIndex];

            /* Move the higher-priority child into the parent position */
            heap[parentIndex] = heap[index];

            /* Move the former parent into the child's old position */
            heap[index] = temporary;

            /* Continue from the inserted email's new index */
            index = parentIndex;
        }
    }

    /* Move a root replacement downward until the MaxHeap property is restored */
    void heapifyDown(int index)
    {
        /* Continue until no child has higher priority than the current item */
        while (true)
        {
            /* Calculate the left-child index */
            int leftChild = (2 * index) + 1;

            /* Calculate the right-child index */
            int rightChild = (2 * index) + 2;

            /* Initially treat the current item as the highest-priority item */
            int highestPriorityIndex = index;

            /* Check whether the left child exists and has higher priority */
            if (leftChild < static_cast<int>(heap.size()) &&
                heap[leftChild].hasHigherPriorityThan(heap[highestPriorityIndex]))
            {
                /* Select the left child as the current highest-priority item */
                highestPriorityIndex = leftChild;
            }

            /* Check whether the right child exists and has higher priority */
            if (rightChild < static_cast<int>(heap.size()) &&
                heap[rightChild].hasHigherPriorityThan(heap[highestPriorityIndex]))
            {
                /* Select the right child as the current highest-priority item */
                highestPriorityIndex = rightChild;
            }

            /* Stop when the current item already outranks both children */
            if (highestPriorityIndex == index)
            {
                /* Leave the loop because the heap property is satisfied */
                break;
            }

            /* Temporarily save the current email */
            Email temporary = heap[index];

            /* Move the higher-priority child upward */
            heap[index] = heap[highestPriorityIndex];

            /* Move the former parent downward */
            heap[highestPriorityIndex] = temporary;

            /* Continue checking from the item's new position */
            index = highestPriorityIndex;
        }
    }

public:
    /* Insert an Email object into the MaxHeap */
    void insert(const Email& email)
    {
        /* Add the new email at the end of the list */
        heap.push_back(email);

        /* Move the new email upward to restore heap order */
        heapifyUp(static_cast<int>(heap.size()) - 1);
    }

    /* Return true when there are no unread emails */
    bool isEmpty() const
    {
        /* Return the vector's empty status */
        return heap.empty();
    }

    /* Return the number of unread emails */
    int getCount() const
    {
        /* Return the number of elements stored in the heap */
        return static_cast<int>(heap.size());
    }

    /* Display the highest-priority email without removing it */
    void displayNext() const
    {
        /* Check whether there are no emails available */
        if (heap.empty())
        {
            /* Display an empty-inbox message */
            cout << "No emails to read." << endl;

            /* End the method because no root email exists */
            return;
        }

        /* Reference the root, which is the highest-priority email */
        const Email& nextEmail = heap[0];

        /* Display the required heading */
        cout << "Next email:" << endl;

        /* Display the sender category */
        cout << "Sender: " << nextEmail.getSenderCategory() << endl;

        /* Display the subject line */
        cout << "Subject: " << nextEmail.getSubject() << endl;

        /* Display the date */
        cout << "Date: " << nextEmail.getDate() << endl;
    }

    /* Remove the highest-priority email without displaying it */
    void removeMax()
    {
        /* Check whether the heap is empty */
        if (heap.empty())
        {
            /* Display an empty-inbox message */
            cout << "No emails to read." << endl;

            /* End the method because there is nothing to remove */
            return;
        }

        /* Handle a heap containing exactly one email */
        if (heap.size() == 1)
        {
            /* Remove the only email */
            heap.pop_back();

            /* End the method because the heap is now empty */
            return;
        }

        /* Replace the root with the final email in the list */
        heap[0] = heap.back();

        /* Remove the duplicate final element */
        heap.pop_back();

        /* Move the replacement root downward to restore heap order */
        heapifyDown(0);
    }
};

/*
 * The EmailManager class parses input commands and controls the MaxHeap
 */
class EmailManager
{
private:
    /* Store the CEO's unread emails in a MaxHeap object */
    MaxHeap emailHeap;

    /* Remove leading and trailing whitespace from a string */
    string trim(const string& text) const
    {
        /* Find the first character that is not whitespace */
        size_t first = text.find_first_not_of(" \t\r\n");

        /* Handle a string containing only whitespace */
        if (first == string::npos)
        {
            /* Return an empty string */
            return "";
        }

        /* Find the final character that is not whitespace */
        size_t last = text.find_last_not_of(" \t\r\n");

        /* Return the trimmed section of the original string */
        return text.substr(first, last - first + 1);
    }

    /* Parse an EMAIL command and insert the resulting Email object */
    void processEmail(const string& line)
    {
        /* Remove the "EMAIL " prefix from the command */
        string emailInformation = line.substr(6);

        /* Locate the comma after the sender category */
        size_t firstComma = emailInformation.find(',');

        /* Locate the comma after the subject line */
        size_t secondComma = emailInformation.find(',', firstComma + 1);

        /* Extract and trim the sender category */
        string sender = trim(emailInformation.substr(0, firstComma));

        /* Extract and trim the subject line */
        string subject = trim(emailInformation.substr(firstComma + 1,
                                                       secondComma - firstComma - 1));

        /* Extract and trim the email date */
        string date = trim(emailInformation.substr(secondComma + 1));

        /* Construct an Email object from the parsed fields */
        Email newEmail(sender, subject, date);

        /* Insert the Email object into the MaxHeap */
        emailHeap.insert(newEmail);
    }

public:
    /* Read and process all commands in the specified input file */
    void processFile(const string& fileName)
    {
        /* Open the requested input file */
        ifstream inputFile(fileName);

        /* Check whether the file opened successfully */
        if (!inputFile.is_open())
        {
            /* Display an error when the file cannot be opened */
            cout << "Error: Unable to open input file." << endl;

            /* End processing because no commands can be read */
            return;
        }

        /* Store one complete input line at a time */
        string line;

        /* Continue reading until the end of the file */
        while (getline(inputFile, line))
        {
            /* Remove unnecessary whitespace from the line */
            line = trim(line);

            /* Ignore blank input lines */
            if (line.empty())
            {
                /* Continue with the next input line */
                continue;
            }

            /* Handle an EMAIL command */
            if (line.rfind("EMAIL ", 0) == 0)
            {
                /* Parse and insert the new email */
                processEmail(line);
            }
            /* Handle a NEXT command */
            else if (line == "NEXT")
            {
                /* Display the root without removing it */
                emailHeap.displayNext();
            }
            /* Handle a READ command */
            else if (line == "READ")
            {
                /* Remove the root without displaying it */
                emailHeap.removeMax();
            }
            /* Handle a COUNT command */
            else if (line == "COUNT")
            {
                /* Display the number of unread emails */
                cout << "There are " << emailHeap.getCount()
                     << " emails to read." << endl;
            }
        }

        /* Close the input file after all commands have been processed */
        inputFile.close();
    }
};

/*
 * The main method starts the program and processes the input-file argument
 * Example execution: /assignment3 inputtxt
 */
int main(int argc, char* argv[])
{
    /* Verify that exactly one input-file argument was supplied */
    if (argc != 2)
    {
        /* Display the correct command-line usage */
        cout << "Usage: " << argv[0] << " <input_file>" << endl;

        /* Return a failure status because no valid input file was supplied */
        return 1;
    }

    /* Create the object responsible for processing the CEO's inbox */
    EmailManager manager;

    /* Process the input file supplied on the command line */
    manager.processFile(argv[1]);

    /* Return zero to indicate successful program completion */
    return 0;
}
