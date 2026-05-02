#pragma once
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

const int DAYS = 5;
const int SLOTS = 8;

struct Room {
    string id;
    int capacity;
    bool hasAC;
    float rating;
    string category;
    bool timetable[DAYS][SLOTS];
    string courseDetails[DAYS][SLOTS];

    Room(string id, int cap, bool ac, float rat)
        : id(id), capacity(cap), hasAC(ac), rating(rat) {
        for (int i = 0; i < DAYS; i++)
            for (int j = 0; j < SLOTS; j++) {
                timetable[i][j] = false;
                courseDetails[i][j] = "Free";
            }
    }
};

struct BookingRequest {
    string bookingID, roomID, studentID;
    int day, slot;
};

enum ActionType { BOOK, CANCEL, MARK_EMPTY };

struct Action {
    ActionType type;
    string roomID;
    int day, slot;
    string previousDetail;
};

// ---- Linked List ----
struct LLNode {
    Room* room;
    LLNode* next;
    LLNode(Room* r) : room(r), next(nullptr) {}
};

class RoomLinkedList {
public:
    LLNode* head;
    RoomLinkedList() : head(nullptr) {}
    void insert(Room* r) {
        LLNode* n = new LLNode(r);
        n->next = head;
        head = n;
    }
};

// ---- BST ----
struct BSTNode {
    Room* room;
    BSTNode* left, *right;
    BSTNode(Room* r) : room(r), left(nullptr), right(nullptr) {}
};

class RoomBST {
public:
    BSTNode* root;
    RoomBST() : root(nullptr) {}
    BSTNode* insert(BSTNode* node, Room* r) {
        if (!node) return new BSTNode(r);
        if (r->id < node->room->id) node->left = insert(node->left, r);
        else if (r->id > node->room->id) node->right = insert(node->right, r);
        return node;
    }
};

// ---- Queue ----
struct QNode {
    BookingRequest req;
    QNode* next;
    QNode(BookingRequest r) : req(r), next(nullptr) {}
};

class BookingQueue {
public:
    QNode* front, *rear;
    BookingQueue() : front(nullptr), rear(nullptr) {}
    void enqueue(BookingRequest r) {
        QNode* t = new QNode(r);
        if (!rear) { front = rear = t; return; }
        rear->next = t; rear = t;
    }
    BookingRequest dequeue() {
        if (!front) return {"","","",-1,-1};
        QNode* t = front;
        front = front->next;
        if (!front) rear = nullptr;
        BookingRequest r = t->req;
        delete t;
        return r;
    }
    bool isEmpty() { return front == nullptr; }
};

// ---- Stack ----
struct StackNode {
    Action act;
    StackNode* next;
    StackNode(Action a) : act(a), next(nullptr) {}
};

class UndoStack {
public:
    StackNode* top;
    UndoStack() : top(nullptr) {}
    void push(Action a) {
        StackNode* t = new StackNode(a);
        t->next = top; top = t;
    }
    Action pop() {
        if (!top) return {BOOK,"",-1,-1,""};
        StackNode* t = top;
        Action a = t->act;
        top = top->next;
        delete t;
        return a;
    }
    bool isEmpty() { return top == nullptr; }
};

// ---- Priority Queue ----
class PriorityQueue {
    Room* heap[100];
    int size;
    bool isHigherPriority(Room* a, Room* b) {
        if (a->category == "Good" && b->category == "Bad") return true;
        if (a->category == "Bad" && b->category == "Good") return false;
        if (a->hasAC && !b->hasAC) return true;
        if (!a->hasAC && b->hasAC) return false;
        return a->rating > b->rating;
    }
    void heapifyUp(int i) {
        while (i > 0 && isHigherPriority(heap[i], heap[(i-1)/2])) {
            swap(heap[i], heap[(i-1)/2]); i = (i-1)/2;
        }
    }
    void heapifyDown(int i) {
        int m = i, l = 2*i+1, r = 2*i+2;
        if (l < size && isHigherPriority(heap[l], heap[m])) m = l;
        if (r < size && isHigherPriority(heap[r], heap[m])) m = r;
        if (i != m) { swap(heap[i], heap[m]); heapifyDown(m); }
    }
public:
    PriorityQueue() : size(0) {}
    void insert(Room* r) { if (size < 100) { heap[size] = r; heapifyUp(size++); } }
    Room* extractMax() {
        if (!size) return nullptr;
        Room* mx = heap[0];
        heap[0] = heap[--size];
        heapifyDown(0);
        return mx;
    }
    bool isEmpty() { return size == 0; }
};

// ---- Hash Table ----
class RoomHashTable {
    static const int TABLE_SIZE = 50;
    LLNode* table[TABLE_SIZE];
    int hashFunc(string id) {
        int s = 0; for (char c : id) s += c;
        return s % TABLE_SIZE;
    }
public:
    RoomHashTable() { for (int i = 0; i < TABLE_SIZE; i++) table[i] = nullptr; }
    void insert(Room* r) {
        int idx = hashFunc(r->id);
        LLNode* n = new LLNode(r);
        n->next = table[idx]; table[idx] = n;
    }
    Room* get(string id) {
        int idx = hashFunc(id);
        LLNode* curr = table[idx];
        while (curr) { if (curr->room->id == id) return curr->room; curr = curr->next; }
        return nullptr;
    }
};

// ==========================================
// SCABS Backend Controller
// ==========================================
class SCABS {
private:
    RoomLinkedList roomsList;
    RoomBST roomTree;
    RoomHashTable roomMap;
    BookingQueue bookingQ;
    UndoStack historyStack;

    int getDayIndex(string day) {
        if (day=="Mo") return 0; if (day=="Tu") return 1;
        if (day=="We") return 2; if (day=="Th") return 3;
        if (day=="Fr") return 4; return -1;
    }
    int getSlotIndex(string time) {
        if (time=="08:30") return 0; if (time=="10:00") return 1;
        if (time=="11:30") return 2; if (time=="13:00") return 3;
        if (time=="14:30") return 4; if (time=="16:00") return 5;
        if (time=="17:30") return 6; return -1;
    }

public:
    void categorizeRoom(Room* r) {
        r->category = (r->hasAC && r->rating >= 4.0) ? "Good" : "Bad";
    }

    void addClassroom(string id, int cap, bool ac, float rating) {
        if (roomMap.get(id)) return;
        Room* r = new Room(id, cap, ac, rating);
        categorizeRoom(r);
        roomsList.insert(r);
        roomTree.root = roomTree.insert(roomTree.root, r);
        roomMap.insert(r);
    }

    // Returns status message
    string loadTimetableFromCSV(string filename) {
        ifstream file(filename);
        if (!file.is_open()) return "ERROR: Could not open " + filename;
        string line;
        getline(file, line);
        int count = 0;
        while (getline(file, line)) {
            stringstream ss(line);
            string program, sem, day, start, end, course, roomID;
            getline(ss, program, ','); getline(ss, sem, ',');
            getline(ss, day, ','); getline(ss, start, ',');
            getline(ss, end, ','); getline(ss, course, ',');
            getline(ss, roomID, ',');
            if (!roomID.empty() && roomID.back() == '\r') roomID.pop_back();
            addClassroom(roomID, 50, true, 4.2);
            int dayIdx = getDayIndex(day);
            int slotIdx = getSlotIndex(start);
            if (dayIdx != -1 && slotIdx != -1) {
                Room* r = roomMap.get(roomID);
                if (r) {
                    r->timetable[dayIdx][slotIdx] = true;
                    r->courseDetails[dayIdx][slotIdx] = course + " (" + program + ")";
                    count++;
                }
            }
        }
        file.close();
        return "SUCCESS: " + to_string(count) + " timetable records loaded.";
    }

    bool checkAvailability(string roomID, int day, int slot) {
        Room* r = roomMap.get(roomID);
        if (!r) return false;
        return !r->timetable[day][slot];
    }

    string bookRoom(string bID, string rID, string sID, int day, int slot) {
        BookingRequest req = {bID, rID, sID, day, slot};
        bookingQ.enqueue(req);
        string result;
        while (!bookingQ.isEmpty()) {
            BookingRequest req2 = bookingQ.dequeue();
            Room* r = roomMap.get(req2.roomID);
            if (r && checkAvailability(r->id, req2.day, req2.slot)) {
                r->timetable[req2.day][req2.slot] = true;
                string prev = r->courseDetails[req2.day][req2.slot];
                r->courseDetails[req2.day][req2.slot] = "Booked by " + req2.studentID;
                historyStack.push({BOOK, r->id, req2.day, req2.slot, prev});
                result = "SUCCESS: Booking confirmed for Room " + r->id;
            } else {
                result = "FAILED: Room " + req2.roomID + " is currently occupied.";
            }
        }
        return result;
    }

    string cancelBooking(string rID, int day, int slot) {
        Room* r = roomMap.get(rID);
        if (r && r->timetable[day][slot]) {
            string prev = r->courseDetails[day][slot];
            r->timetable[day][slot] = false;
            r->courseDetails[day][slot] = "Free";
            historyStack.push({CANCEL, r->id, day, slot, prev});
            return "SUCCESS: Booking cancelled for Room " + rID;
        }
        return "FAILED: No active booking found to cancel.";
    }

    string markClassEmpty(string rID, int day, int slot) {
        Room* r = roomMap.get(rID);
        if (r && r->timetable[day][slot]) {
            string prev = r->courseDetails[day][slot];
            r->timetable[day][slot] = false;
            r->courseDetails[day][slot] = "Free (Reported Empty)";
            historyStack.push({MARK_EMPTY, r->id, day, slot, prev});
            return "REPORTED: Room " + rID + " marked empty. Available for booking.";
        }
        return "NOTICE: Room is already free.";
    }

    string undoLastAction() {
        if (historyStack.isEmpty()) return "NOTICE: Nothing to undo.";
        Action last = historyStack.pop();
        Room* r = roomMap.get(last.roomID);
        if (!r) return "ERROR: Room not found.";
        if (last.type == BOOK) {
            r->timetable[last.day][last.slot] = false;
            r->courseDetails[last.day][last.slot] = last.previousDetail;
            return "UNDO: Reverted booking for Room " + r->id;
        } else {
            r->timetable[last.day][last.slot] = true;
            r->courseDetails[last.day][last.slot] = last.previousDetail;
            return "UNDO: Restored schedule for Room " + r->id + " (" + last.previousDetail + ")";
        }
    }

    // Returns list of rooms sorted by priority for a given slot
    vector<Room*> recommendRooms(int day, int slot) {
        PriorityQueue pq;
        LLNode* curr = roomsList.head;
        while (curr) {
            if (!curr->room->timetable[day][slot]) pq.insert(curr->room);
            curr = curr->next;
        }
        vector<Room*> result;
        while (!pq.isEmpty()) result.push_back(pq.extractMax());
        return result;
    }

    Room* getRoom(string id) { return roomMap.get(id); }

    // Get all rooms
    vector<Room*> getAllRooms() {
        vector<Room*> rooms;
        LLNode* curr = roomsList.head;
        while (curr) { rooms.push_back(curr->room); curr = curr->next; }
        return rooms;
    }
};
