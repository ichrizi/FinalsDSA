#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <limits>

using namespace std;

class Video {
private:
    static int nextID;
    int videoID;
    string title;
    string genre;
    string production;
    int copies;

public:
    Video() : videoID(0), title(""), genre(""), production(""), copies(0) {}

    Video(int id, const string &title, const string &genre, const string &production, int copies)
        : videoID(id), title(title), genre(genre), production(production), copies(copies) {
        if (id >= nextID) {
            nextID = id + 1;
        }
    }

    int getVideoID() const {
        return videoID;
    }

    string getTitle() const {
        return title;
    }

    string getGenre() const {
        return genre;
    }

    string getProduction() const {
        return production;
    }

    int getCopies() const {
        return copies;
    }

    void rentVideo() {
        if (copies > 0) {
            copies--;
            cout << "Video " << title << " rented successfully. Available copies: " << copies << endl;
        } else {
            cout << "No available copies of " << title << " to rent." << endl;
        }
    }

    void returnVideo() {
        copies++;
        cout << "Video " << title << " returned successfully. Available copies: " << copies << endl;
    }

    void showDetails() const {
        cout << "Video ID: " << videoID << "\n"
             << "Title: " << title << "\n"
             << "Genre: " << genre << "\n"
             << "Production: " << production << "\n"
             << "Number of Copies: " << copies << "\n";
    }

    void writeToFile(ofstream &file) const {
        file << videoID << "," << title << "," << genre << "," << production << "," << copies << "\n";
    }

    static int getNextID() {
        return nextID++;
    }

    static void initializeNextID() {
        nextID = 1;
    }
};

int Video::nextID = 1;

class Customer {
public:
	static int nextID;
    int customerID;
    string name;
    string address;

    Customer(int id, string name, string address) : customerID(id), name(name), address(address) {if (customerID >= nextID) {
            nextID = id + 1;
        }
	}
	
	    static int getNextID() {
        return nextID++;
    }

    static void initializeNextID() {
        nextID = 1;
    }
};

int Customer::nextID = 1;

class CustomerRent : public Customer {
public:
    stack<int> rentedVideos;
 	CustomerRent() : Customer(0, "", ""), rentedVideos() {} // Default constructor
    CustomerRent(int id, string name, string address) : Customer(id, name, address) {}

    void rentVideo(int videoID) {
        rentedVideos.push(videoID);
    }

    void returnVideo() {
        if (!rentedVideos.empty()) {
            rentedVideos.pop();
        }
    }

    void printRentedVideos(const vector<Video>& videos) {
        stack<int> temp = rentedVideos;
        while (!temp.empty()) {
            int videoID = temp.top();
            temp.pop();
            for (const auto& video : videos) {
                if (video.getVideoID() == videoID) {
                    cout << "Video ID: " << videoID << " - " << video.getTitle() << endl;
                    break;
                }
            }
        }
    }
    void listRentedVideosByCustomer(const std::queue<CustomerRent>& customers, const std::vector<Video>& videos, int customerId) {
        std::queue<CustomerRent> tempQueue = customers;
        bool customerFound = false;

        while (!tempQueue.empty()) {
            CustomerRent customer = tempQueue.front();
            tempQueue.pop();

            if (customer.customerID == customerId) {
                customerFound = true;
                cout << "Customer ID: " << customer.customerID << endl;
                cout << "Name: " << customer.name << endl;
                cout << "Address: " << customer.address << endl;
                cout << "Rented Videos:" << endl;
                customer.printRentedVideos(videos);
                break; // Exit loop once customer is found
            }
        }

        if (!customerFound) {
            cout << "Customer with ID " << customerId << " not found." << std::endl;
        }
    }
};

queue<CustomerRent> loadCustomers(const string& customerFilename, const string& rentedVideosFilename) {
    queue<CustomerRent> customers;
    ifstream infile(customerFilename);
    if (infile.is_open()) {
        int customerId;
        string name, address;
        while (infile >> customerId >> ws && getline(infile, name, ',') && getline(infile, address)) {
            CustomerRent customer(customerId, name, address);
            customers.push(customer);
        }
        infile.close();
    } else {
        cout << "Unable to open customer file: " << customerFilename << endl;
    }

    // Load rented videos
    ifstream rentedFile(rentedVideosFilename);
    if (rentedFile.is_open()) {
        int customerId, videoId;
        queue<CustomerRent> tempQueue;
        while (!customers.empty()) {
            CustomerRent customer = customers.front();
            customers.pop();
            while (rentedFile >> customerId >> videoId) {
                if (customer.customerID == customerId) {
                    customer.rentVideo(videoId);
                }
            }
            tempQueue.push(customer);
        }
        rentedFile.close();
        customers = tempQueue;
    } else {
        cout << "Unable to open rented videos file: " << rentedVideosFilename << endl;
    }

    return customers;
}


void saveCustomers(queue<CustomerRent>& customers, const string& customerFilename, const string& rentedVideosFilename) {
    ofstream outfile(customerFilename);
    if (outfile.is_open()) {
        queue<CustomerRent> temp = customers;
        while (!temp.empty()) {
            CustomerRent customer = temp.front();
            temp.pop();
            outfile << customer.customerID << " " << customer.name << "," << customer.address << endl;
        }
        outfile.close();
    } else {
        cout << "Unable to open customer file: " << customerFilename << endl;
    }

    // Save rented videos
    ofstream rentedFile(rentedVideosFilename);
    if (rentedFile.is_open()) {
        queue<CustomerRent> temp = customers;
        while (!temp.empty()) {
            CustomerRent customer = temp.front();
            temp.pop();
            stack<int> rentedVideos = customer.rentedVideos;
            while (!rentedVideos.empty()) {
                rentedFile << customer.customerID << " " << rentedVideos.top() << endl;
                rentedVideos.pop();
            }
        }
        rentedFile.close();
    } else {
        cout << "Unable to open rented videos file: " << rentedVideosFilename << endl;
    }
}


void saveRentedVideos(queue<CustomerRent>& customers, const string& filename) {
    ofstream outfile(filename);
    if (outfile.is_open()) {
        queue<CustomerRent> temp = customers;
        while (!temp.empty()) {
            CustomerRent customer = temp.front();
            temp.pop();
            stack<int> rentedVideos = customer.rentedVideos;
            while (!rentedVideos.empty()) {
                outfile << customer.customerID << " " << rentedVideos.top() << endl;
                rentedVideos.pop();
            }
        }
        outfile.close();
    } else {
        cout << "Unable to open file: " << filename << endl;
    }
}

class LinkedList {
public:
    struct Node {
        Video data;
        Node *next;

        Node(const Video &video) : data(video), next(NULL) {}
    };

private:
    Node *head;

public:
    LinkedList() : head(NULL) {}

    ~LinkedList() {
        while (head) {
            Node *temp = head;
            head = head->next;
            delete temp;
        }
    }

    void loadFromFile(const char* filename) {
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                stringstream ss(line);
                string videoID, title, genre, production, copiesStr;

                getline(ss, videoID, ',');
                getline(ss, title, ',');
                getline(ss, genre, ',');
                getline(ss, production, ',');
                getline(ss, copiesStr, ',');

                int id = stoi(videoID); // Convert string to int
                int copies = stoi(copiesStr); // Convert string to int

                Video newVideo(id, title, genre, production, copies);
                insert(newVideo); // Insert loaded video into the linked list
            }
            file.close();
            cout << "Inventory loaded from file: " << filename << endl;
        } else {
            cout << "Unable to open file: " << filename << endl;
        }
    }

    void insert(const Video &video) {
        Node *newNode = new Node(video);
        if (!head) {
            head = newNode; // If list is empty, new node becomes head
        } else {
            Node *temp = head;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = newNode; // Append new node at the end of the list
        }
    }

    void displayAllVideos() const {
        Node *temp = head;
        while (temp) {
            temp->data.showDetails();
            cout << "-------------------------\n";
            temp = temp->next;
        }
    }

    bool isEmpty() const {
        return head == NULL;
    }

    void saveToFile(const char* filename) const {
        ofstream file(filename, ios::out); // Open file in write mode
        if (file.is_open()) {
            Node *temp = head; // Start from the head of the list
            while (temp) {
                temp->data.writeToFile(file); // Write each video's data to the file
                temp = temp->next; // Move to the next node
            }
            file.close();
            cout << "Inventory saved to file: " << filename << endl;
        } else {
            cout << "Unable to open file: " << filename << endl;
            cout << "Error code: " << file.rdstate() << endl;
        }
    }

    Node *find(int videoID) const {
        Node *temp = head;
        while (temp) {
            if (temp->data.getVideoID() == videoID) {
                return temp;
            }
            temp = temp->next;
        }
        return NULL;
    }

    bool remove(int videoID) {
        Node *prev = NULL;
        Node *curr = head;

        while (curr) {
            if (curr->data.getVideoID() == videoID) {
                if (prev) {
                    prev->next = curr->next;
                } else {
                    head = curr->next;
                }
                delete curr;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    bool isVideoInStore(int videoID) const {
        Node *temp = head;
        while (temp) {
            if (temp->data.getVideoID() == videoID) {
                return true;
            }
            temp = temp->next;
        }
        return false;
    }

    bool rentVideo(int videoID) {
        Node* temp = head;
        while (temp) {
            if (temp->data.getVideoID() == videoID) {
                temp->data.rentVideo();
                return true;
            }
            temp = temp->next;
        }
        cout << "Video with ID " << videoID << " not found in the inventory." << endl;
        return false;
    }

    bool returnVideo(int videoID) {
        Node* temp = head;
        while (temp) {
            if (temp->data.getVideoID() == videoID) {
                temp->data.returnVideo();
                return true;
            }
            temp = temp->next;
        }
        cout << "Video with ID " << videoID << " not found in the inventory." << endl;
        return false;
    }

    Node* getHead() const {
        return head;
    }
    
   void showVideoDetails() const {
    int videoID;
    cout << "Enter video ID to view details: ";
    cin >> videoID;

    Node *node = find(videoID);
    if (node) {
        node->data.showDetails();
    } else {
        cout << "Video with ID " << videoID << " not found in the store." << endl;
    }
}
	bool checkVideoAvailability(int videoID) const {
	    Node *node = find(videoID);
	    if (node) {
	        if (node->data.getCopies() > 0) {
	            cout << "Video " << node->data.getTitle() << " is available for rent." << endl;
	            node->data.showDetails(); // Display video details
	            return true;
	        } else {
	            cout << "Video " << node->data.getTitle() << " is not available for rent. No copies left." << endl;
	            return false;
	        }
	    } else {
	        cout << "Video with ID " << videoID << " not found in the store." << endl;
	        return false;
	    }
	}
};

int main() {
    Video::initializeNextID();
    Customer::initializeNextID();
    
    vector<Video> videos;
    LinkedList videoList;
    videoList.loadFromFile("inventory.txt");
    
    // Populate the videos vector with the videos from the linked list
    LinkedList::Node* tempNode = videoList.getHead();
    while (tempNode != nullptr) {
        videos.push_back(tempNode->data);
        tempNode = tempNode->next;
    }

    queue<CustomerRent> customerQueue = loadCustomers("customers.txt", "rented_videos.txt");
    queue<CustomerRent> customers;

    int choice;
    do {
        cout << "\nMenu:\n";
        cout << "1. Add new video\n";
        cout << "2. Rent a Video\n";
        cout << "3. Return a Video\n";
        cout << "4. Show Video Details\n";
        cout << "5. Display all videos\n";
        cout << "6. Check Video Availability\n";
        cout << "7. Customer Maintenance\n";
        cout << "8. Remove Video\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        while (!(cin >> choice)) {

        cin.clear();

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Invalid input! Please try again with valid input: ";
    }
    

        switch (choice) {
        case 1: {
            int id = Video::getNextID();
            string title, genre, production;
            int copies;
            cout << "Enter video title: ";
            cin.ignore();
            getline(cin, title);
            cout << "Enter video genre: ";
            getline(cin, genre);
            cout << "Enter video production: ";
            getline(cin, production);
            cout << "Enter number of copies: ";
            cin >> copies;

            Video newVideo(id, title, genre, production, copies);
            videoList.insert(newVideo);
            videos.push_back(newVideo); // Add to the vector as well
            cout << "Video added successfully.\n";
            break;
        }
        case 2: {
            int customerID, videoID;
            cout << "Enter customer ID: ";
            cin >> customerID;
            cout << "Enter video ID: ";
            cin >> videoID;

            bool customerFound = false;
            queue<CustomerRent> tempQueue;
            while (!customerQueue.empty()) {
                CustomerRent customer = customerQueue.front();
                customerQueue.pop();
                if (customer.customerID == customerID) {
                    customerFound = true;
                    if (videoList.rentVideo(videoID)) {
                        customer.rentVideo(videoID);
                    }
                }
                tempQueue.push(customer);
            }
            customerQueue = tempQueue;

            if (!customerFound) {
                cout << "Customer ID not found.\n";
            }
            break;
        }
        case 3: {
            int customerID, videoID;
            cout << "Enter customer ID: ";
            cin >> customerID;
            cout << "Enter video ID: ";
            cin >> videoID;

            bool customerFound = false;
            queue<CustomerRent> tempQueue;
            while (!customerQueue.empty()) {
                CustomerRent customer = customerQueue.front();
                customerQueue.pop();
                if (customer.customerID == customerID) {
                    customerFound = true;
                    if (videoList.returnVideo(videoID)) {
                        customer.returnVideo();
                    }
                }
                tempQueue.push(customer);
            }
            customerQueue = tempQueue;

            if (!customerFound) {
                cout << "Customer ID not found.\n";
            }
            break;
        }
        case 4: {
            videoList.showVideoDetails();
            system("pause");
            break;
        }
        case 5: {
            videoList.displayAllVideos();
            system("pause");
            break;
        }
        case 6: {
            int videoID;
            cout << "Enter video ID to check availability: ";
            cin >> videoID;
            videoList.checkVideoAvailability(videoID);
            system("pause");
            break;
        }
        case 7: { // Customer maintenance operations
                CustomerRent newCustomer;
                int subChoice;

                do {
                    system("CLS");
                    std::cout << "========================================" << std::endl
                            << "|         Customer Maintenance         |" << std::endl
                            << "========================================" << std::endl
                            << "|                                      |" << std::endl
                            << "|   [1] - Add New Customer             |" << std::endl
                            << "|   [2] - Show Customer Details        |" << std::endl
                            << "|   [3] - List of Videos Rented        |" << std::endl
                            << "|         by a Customer                |" << std::endl
                            << "|   [0] - Cancel                       |" << std::endl
                            << "|                                      |" << std::endl
                            << "========================================" << std::endl << std::endl
                            << "Enter your choice: ";
                    std::cin >> subChoice;
                    std::cin.ignore();

                    switch (subChoice) {
                        case 1: { // Add New Customer
                            int id = Customer::getNextID();
                            string name, address;
                            cout << "Enter customer name: ";
                            getline(cin, name);
                            cout << "Enter customer address: ";
                            getline(cin, address);
                            CustomerRent newCustomer(id, name, address);
                            customerQueue.push(newCustomer);
                            cout << "Customer added successfully. Customer ID: " << id << std::endl;
                            system("pause");
                            break;
                        }
                        case 2: { // Show Customer Details
                            int customerId;
                            cout << "Enter customer ID: ";
                            cin >> customerId;

                            bool customerFound = false;
                            queue<CustomerRent> tempQueue = customerQueue;

                            while (!tempQueue.empty()) {
                                CustomerRent customer = tempQueue.front();
                                tempQueue.pop();

                                if (customer.customerID == customerId) {
                                    customerFound = true;
                                    cout << "Customer ID: " << customer.customerID << endl;
                                    cout << "Name: " << customer.name << endl;
                                    cout << "Address: " << customer.address << endl;
                                    break;
                                }
                            }

                            if (!customerFound) {
                                cout << "Customer with ID " << customerId << " not found." << endl;
                            }

                            system("pause");
                            break;
                        }
                        case 3: { // List Of Videos Rented By A Customer
                            int customerId;
                            cout << "Enter customer ID: ";
                            cin >> customerId;
                            CustomerRent().listRentedVideosByCustomer(customerQueue, videos, customerId);
                            system("pause");
                            break;
                        }
                        case 0: { // Cancel Customer Maintenance
                            cout << std::endl << "Returning to Menu: Press any key to proceed...";
                            cin.ignore();
                            system("CLS");
                            break;
                        }
                        default:
                            cout << endl << "Invalid Input: Press any key to try again...";
                            cin.ignore();
                            system("CLS");
                    }
                } while (subChoice != 0); // Loop until user chooses to cancel
                break;
            }
            case 8: {
                int videoID;
                cout << "Enter video ID to remove: ";
                cin >> videoID;
                if (videoList.remove(videoID)) {
                    cout << "Video removed successfully.\n";
                } else {
                    cout << "Video with ID " << videoID << " not found in the inventory.\n";
                }
                break;
            }
            case 9: { // Save data before exiting
                videoList.saveToFile("inventory.txt");
                saveCustomers(customerQueue, "customers.txt", "rented_videos.txt");
                cout << "Data saved successfully.\n";
                cout << "Exiting...\n";
                break;
            }
            default:
                cout << "Invalid choice. Try again.\n";
                break;
        }
    } while (choice != 9);

    saveCustomers(customerQueue, "customers.txt", "rented_videos.txt");
    return 0;
}
