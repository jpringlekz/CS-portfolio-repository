/*
 * ProjectTwo.cpp
 * CS 300 Project Two - ABCU Advising Assistance Program
 *
 * This command-line program loads course information from a CSV file,
 * stores courses in a Binary Search Tree, prints all courses in
 * alphanumeric order, and allows the user to search for a single course.
 */

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Course object stores one course and its prerequisites.
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Node used by the Binary Search Tree.
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(Course aCourse) {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

// Binary Search Tree for storing courses in alphanumeric order.
class BinarySearchTree {
private:
    Node* root;

    // Recursively add a course to the tree.
    void addNode(Node* node, Course course) {
        if (course.courseNumber < node->course.courseNumber) {
            if (node->left == nullptr) {
                node->left = new Node(course);
            } else {
                addNode(node->left, course);
            }
        } else {
            if (node->right == nullptr) {
                node->right = new Node(course);
            } else {
                addNode(node->right, course);
            }
        }
    }

    // Recursively print courses in sorted order.
    void inOrder(Node* node) {
        if (node != nullptr) {
            inOrder(node->left);
            cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
            inOrder(node->right);
        }
    }

    // Recursively delete nodes to prevent memory leaks.
    void deleteTree(Node* node) {
        if (node != nullptr) {
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
        }
    }

public:
    BinarySearchTree() {
        root = nullptr;
    }

    ~BinarySearchTree() {
        deleteTree(root);
    }

    void Insert(Course course) {
        if (root == nullptr) {
            root = new Node(course);
        } else {
            addNode(root, course);
        }
    }

    Course Search(string courseNumber) {
        Node* current = root;

        while (current != nullptr) {
            if (current->course.courseNumber == courseNumber) {
                return current->course;
            } else if (courseNumber < current->course.courseNumber) {
                current = current->left;
            } else {
                current = current->right;
            }
        }

        Course emptyCourse;
        return emptyCourse;
    }

    void PrintCourseList() {
        inOrder(root);
    }
};

// Remove extra spaces from the beginning and end of a string.
string trim(string str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return "";
    }

    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

// Convert course input to uppercase so searches are not case-sensitive.
string toUpperCase(string str) {
    for (char& c : str) {
        c = toupper(static_cast<unsigned char>(c));
    }
    return str;
}

// Split one CSV line into separate fields.
vector<string> splitLine(string line) {
    vector<string> fields;
    string field;
    stringstream ss(line);

    while (getline(ss, field, ',')) {
        fields.push_back(trim(field));
    }

    return fields;
}

// Load courses from the CSV file into the Binary Search Tree.
int loadCourses(string fileName, BinarySearchTree& bst) {
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Error: Could not open file: " << fileName << endl;
        return 0;
    }

    string line;
    int courseCount = 0;

    while (getline(file, line)) {
        if (trim(line).empty()) {
            continue;
        }

        vector<string> fields = splitLine(line);

        // Each valid line must include at least course number and course title.
        if (fields.size() < 2 || fields[0].empty() || fields[1].empty()) {
            cout << "Warning: Skipping invalid line: " << line << endl;
            continue;
        }

        Course course;
        course.courseNumber = toUpperCase(fields[0]);
        course.courseTitle = fields[1];

        // Any remaining fields are prerequisites.
        for (size_t i = 2; i < fields.size(); ++i) {
            if (!fields[i].empty()) {
                course.prerequisites.push_back(toUpperCase(fields[i]));
            }
        }

        bst.Insert(course);
        courseCount++;
    }

    file.close();
    return courseCount;
}

// Print one course and its prerequisites.
void printCourseInformation(Course course, BinarySearchTree& bst) {
    if (course.courseNumber.empty()) {
        cout << "Course not found." << endl;
        return;
    }

    cout << course.courseNumber << ", " << course.courseTitle << endl;

    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    } else {
        cout << "Prerequisites: ";

        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            Course prereq = bst.Search(course.prerequisites[i]);

            cout << course.prerequisites[i];

            if (!prereq.courseTitle.empty()) {
                cout << " - " << prereq.courseTitle;
            }

            if (i < course.prerequisites.size() - 1) {
                cout << ", ";
            }
        }

        cout << endl;
    }
}

int main() {
    BinarySearchTree bst;
    bool dataLoaded = false;
    int choice = 0;
    string fileName;
    string courseNumber;

    cout << "Welcome to the ABCU Advising Assistance Program." << endl;

    while (choice != 9) {
        cout << endl;
        cout << "Menu:" << endl;
        cout << "  1. Load Data Structure" << endl;
        cout << "  2. Print Course List" << endl;
        cout << "  3. Print Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "What would you like to do? ";

        cin >> choice;

        // Handle non-numeric menu input.
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number from the menu." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Enter the file name: ";
            cin.ignore();
            getline(cin, fileName);

            // Rebuild tree by creating a new object.
            bst = BinarySearchTree();

            if (loadCourses(fileName, bst) > 0) {
                dataLoaded = true;
                cout << "Course data loaded successfully." << endl;
            } else {
                dataLoaded = false;
                cout << "No course data was loaded." << endl;
            }
            break;

        case 2:
            if (!dataLoaded) {
                cout << "Please load the course data first." << endl;
            } else {
                cout << endl;
                cout << "Here is a sample schedule:" << endl;
                bst.PrintCourseList();
            }
            break;

        case 3:
            if (!dataLoaded) {
                cout << "Please load the course data first." << endl;
            } else {
                cout << "What course do you want to know about? ";
                cin >> courseNumber;
                courseNumber = toUpperCase(courseNumber);

                printCourseInformation(bst.Search(courseNumber), bst);
            }
            break;

        case 9:
            cout << "Thank you for using the ABCU Advising Assistance Program." << endl;
            break;

        default:
            cout << choice << " is not a valid option." << endl;
            break;
        }
    }

    return 0;
}
