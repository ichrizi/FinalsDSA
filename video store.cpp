#include <cstdlib> 
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
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

    friend class VideoStore;
};

int Video::nextID = 1;

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

            int id = atoi(videoID.c_str()); // Convert string to int
            int copies = atoi(copiesStr.c_str()); // Convert string to int

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
    ofstream file(filename, ios::out | ios::app); // Open file in append mode
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
                temp->data.rentVideo(); // Rent the video
                // After renting, update the file with the new data
                saveToFile("video_store.txt");
                return true;
            }
            temp = temp->next;
        }
        cout << "Video with ID " << videoID << " not found." << endl;
        return false;
    }
};

class VideoStore {
private:
    LinkedList inventory;

public:
    void insertVideo(const string &title, const string &genre, const string &production, int copies) {
    int id = Video::getNextID(); // Get unique ID for the new video
    Video newVideo(id, title, genre, production, copies);
    inventory.insert(newVideo);
}

    void rentVideo(int videoID) {
        LinkedList::Node *videoNode = inventory.find(videoID);
        if (videoNode) {
            videoNode->data.rentVideo();
            cout << "Video rented successfully.\n";
        } else {
            cout << "Video not found.\n";
        }
    }

    void returnVideo(int videoID) {
        LinkedList::Node *videoNode = inventory.find(videoID);
        if (videoNode) {
            videoNode->data.returnVideo();
            cout << "Video returned successfully.\n";
        } else {
            cout << "Video not found.\n";
        }
    }

    void showVideoDetails(int videoID) const {
        LinkedList::Node *videoNode = inventory.find(videoID);
        if (videoNode) {
            videoNode->data.showDetails();
        } else {
            cout << "Video not found.\n";
        }
    }

    void loadInventoryFromFile(const char* filename) {
        inventory.loadFromFile(filename);
    }

    void displayAllVideos() const {
        inventory.displayAllVideos();
    }


 bool isVideoInStore(int videoID) const {
        return inventory.isVideoInStore(videoID);
    }


    void saveToFile(const char* filename) const {
        inventory.saveToFile(filename);
    }
};

int main() {
	Video::initializeNextID();
	string title, genre, production;
	int opt;
	int videoID;
    VideoStore store;
  	store.loadInventoryFromFile("video_store.txt");
  Menu:
	cout << "\tVIDEO STORE" << endl;
	cout << "============================================" << endl;
	cout << "[1] New Video." 				<< endl;
	cout << "[2] Rent a Video." 			<< endl;
	cout << "[3] Return a Video." 			<< endl;
	cout << "[4] Show Video Details." 		<< endl;
	cout << "[5] Display all Videos." 		<< endl;
	cout << "[6] Check Video Availability." << endl;
	cout << "[7] Customer Maintenence."     << endl;
	cout << "[8] Exit/Save Program"     << endl;
	cout << "Enter a Number : ";
	cin  >> opt;
	switch(opt) {
		case 1:
			int copies;
			cout << "[1] New Video." 		 << endl;
			cout << "----------------------" << endl;
			cout << "Movie Title: ";
			cin.ignore();
			getline(cin, title);
			cout << endl;
			cout << "Genre: ";
			getline(cin, genre);
			cout << endl;
			cout << "Production: ";
			getline(cin, production);
			cout << endl;
			cout << "Number of copies: ";
			cin >> copies;
			store.insertVideo(title, genre, production, copies);
			store.saveToFile("video_store.txt");
			cout << "Sucessfully added!!" << endl;
			store.displayAllVideos();
			system("pause");
		goto Menu;
			break;
		case 2:
			int videoIDToRent;
			cout << "[2] Rent a Video." 						<< endl;
			cout << "----------------------" 					<< endl;
			cout << "Video ID of the video you wish to rent: "  << endl;
			cin  >> videoIDToRent;
			store.rentVideo(videoIDToRent);
			system("pause");
			goto Menu;
			break;
		case 3:
		
            cout << "Enter Video ID to return: ";
            cin >> videoID;
            store.returnVideo(videoID);
            system("pause");
			goto Menu;
            break;
		case 4:

            cout << "Enter Video ID to show details: ";
            cin >> videoID;
            store.showVideoDetails(videoID);
         	system("pause");
			goto Menu;
            break;
		case 5:
			//display all videos.
			cout << "\n===== All Videos in the Store =====\n";
            store.displayAllVideos();
			system("pause");
			goto Menu;
			break;
		case 6:
	
            cout << "Enter Video ID to check: ";
            cin >> videoID;
            cout << (store.isVideoInStore(videoID) ? "Video is in store.\n" : "Video is not in store.\n");
            system("pause");
			goto Menu;
            break;
		case 7:
			// customer maintence.
		 case 8:
            cout << "Exiting Program...\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
		
	
	  
    return 0;
	}


