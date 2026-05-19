#ifndef SCABS_BACKEND_H
#define SCABS_BACKEND_H

// ============================================================
//  SCABS — Smart Classroom Availability & Booking System
//  Backend Header  |  DSA Project Semester 3
//
//  Data Structures:
//   1. Linked List      — stores all Classroom objects
//   2. Hash Table       — O(1) lookup by Room ID
//   3. BST              — rooms sorted by ID
//   4. Queue            — FIFO booking requests
//   5. Stack            — undo last N actions
//   6. Priority Queue   — room recommendations
//   7. SlotNode List    — per-room timetable (replaces 2D array)
// ============================================================

#include <iostream>
#include <string>
using namespace std;

// ─── CONSTANTS ─────────────────────────────────────────────
const int DAYS      = 7;
const int SLOTS     = 8;       // 8 AM → 3 PM
const int HASH_SIZE = 101;     // prime → fewer collisions
const int MAX_UNDO  = 50;

// ─── FORWARD DECLARATIONS ──────────────────────────────────
struct Classroom;
struct Booking;

// ============================================================
//  SlotNode — one booked (day, slot) pair
//  Replaces bool timetable[DAYS][SLOTS].
//  A linked list of these = the room's weekly timetable.
// ============================================================
struct SlotNode {
    int       day;
    int       slot;
    SlotNode* next;
};

// ─── Timetable helper functions (used everywhere) ──────────

// Walk the list — return true if (day,slot) is booked — O(b)
inline bool isBooked(SlotNode* head, int day, int slot) {
    for (SlotNode* c = head; c != nullptr; c = c->next)
        if (c->day == day && c->slot == slot) return true;
    return false;
}

// Insert a new booked slot at head — O(1)
inline void bookSlot(SlotNode*& head, int day, int slot) {
    SlotNode* n = new SlotNode{ day, slot, head };
    head = n;
}

// Find and remove a booked slot — O(b)
inline void freeSlot(SlotNode*& head, int day, int slot) {
    if (!head) return;
    if (head->day == day && head->slot == slot) {
        SlotNode* d = head; head = head->next; delete d; return;
    }
    for (SlotNode* p = head; p->next; p = p->next) {
        if (p->next->day == day && p->next->slot == slot) {
            SlotNode* d = p->next; p->next = d->next; delete d; return;
        }
    }
}

// ============================================================
//  CLASSROOM  (node for Linked List & BST)
// ============================================================
struct Classroom {
    int        roomId;
    string     name;
    int        capacity;
    bool       hasAC;
    float      rating;      // 0.0 – 5.0
    string     category;    // "Good" | "Bad"

    SlotNode*  bookedSlots; // timetable as linked list

    Classroom* next;        // Linked List
    Classroom* left;        // BST
    Classroom* right;       // BST
    Classroom* hashNext;    // Hash Table chaining
};

// ============================================================
//  BOOKING  (Queue node)
// ============================================================
struct Booking {
    int      bookingId;
    int      roomId;
    int      studentId;
    int      day;
    int      slot;
    Booking* next;
};

// ============================================================
//  ACTION  (Undo Stack element)
// ============================================================
struct Action {
    string type;       // "BOOK" | "MARK_EMPTY"
    int    roomId;
    int    day;
    int    slot;
    int    bookingId;
    bool   prevState;
};

// ============================================================
//  DATA STRUCTURE 1 — LINKED LIST
// ============================================================
struct LinkedList {
    Classroom* head;
    int        count;

    LinkedList() : head(nullptr), count(0) {}

    void insertAtHead(Classroom* room) {
        room->next = head; head = room; count++;
    }

    Classroom* search(int roomId) {
        for (Classroom* c = head; c; c = c->next)
            if (c->roomId == roomId) return c;
        return nullptr;
    }

    bool remove(int roomId) {
        if (!head) return false;
        if (head->roomId == roomId) {
            Classroom* d = head; head = head->next; delete d; count--; return true;
        }
        for (Classroom* p = head; p->next; p = p->next) {
            if (p->next->roomId == roomId) {
                Classroom* d = p->next; p->next = d->next; delete d; count--; return true;
            }
        }
        return false;
    }

    void display() {
        if (!head) { cout << "  (no rooms)" << endl; return; }
        for (Classroom* c = head; c; c = c->next)
            cout << "  [" << c->roomId << "] " << c->name
                 << " | Cap: " << c->capacity
                 << " | AC: "  << (c->hasAC ? "Yes" : "No")
                 << " | Rating: " << c->rating
                 << " | " << c->category << endl;
    }
};

// ============================================================
//  DATA STRUCTURE 2 — HASH TABLE  (separate chaining)
// ============================================================
struct HashTable {
    Classroom* buckets[HASH_SIZE];

    HashTable() { for (int i = 0; i < HASH_SIZE; i++) buckets[i] = nullptr; }

    int hash(int key) { return (key % HASH_SIZE + HASH_SIZE) % HASH_SIZE; }

    void insert(Classroom* room) {
        int i = hash(room->roomId);
        room->hashNext = buckets[i]; buckets[i] = room;
    }

    Classroom* search(int roomId) {
        for (Classroom* c = buckets[hash(roomId)]; c; c = c->hashNext)
            if (c->roomId == roomId) return c;
        return nullptr;
    }

    void remove(int roomId) {
        int i = hash(roomId);
        Classroom* c = buckets[i]; Classroom* p = nullptr;
        while (c) {
            if (c->roomId == roomId) {
                if (p) p->hashNext = c->hashNext; else buckets[i] = c->hashNext;
                return;
            }
            p = c; c = c->hashNext;
        }
    }
};

// ============================================================
//  DATA STRUCTURE 3 — BST  (sorted by roomId)
// ============================================================
struct BST {
    Classroom* root;
    BST() : root(nullptr) {}

    Classroom* insertNode(Classroom* node, Classroom* room) {
        if (!node) return room;
        if (room->roomId < node->roomId) node->left  = insertNode(node->left,  room);
        else if (room->roomId > node->roomId) node->right = insertNode(node->right, room);
        return node;
    }
    void insert(Classroom* room) {
        room->left = room->right = nullptr;
        root = insertNode(root, room);
    }

    void inorder(Classroom* node) {
        if (!node) return;
        inorder(node->left);
        cout << "  [" << node->roomId << "] " << node->name
             << "  Rating: " << node->rating << endl;
        inorder(node->right);
    }
    void displaySorted() { inorder(root); }

    Classroom* search(Classroom* node, int id) {
        if (!node || node->roomId == id) return node;
        return id < node->roomId ? search(node->left, id) : search(node->right, id);
    }
    Classroom* find(int id) { return search(root, id); }
};

// ============================================================
//  DATA STRUCTURE 4 — QUEUE  (FIFO booking requests)
// ============================================================
struct Queue {
    Booking* front; Booking* rear; int size;
    Queue() : front(nullptr), rear(nullptr), size(0) {}

    bool isEmpty() { return !front; }

    void enqueue(Booking* b) {
        b->next = nullptr;
        if (!rear) { front = rear = b; } else { rear->next = b; rear = b; }
        size++;
    }

    Booking* dequeue() {
        if (isEmpty()) return nullptr;
        Booking* t = front; front = front->next;
        if (!front) rear = nullptr; size--;
        return t;
    }

    Booking* peek() { return front; }

    void display() {
        if (isEmpty()) { cout << "  (empty queue)" << endl; return; }
        int pos = 1;
        for (Booking* c = front; c; c = c->next)
            cout << "  " << pos++ << ". BookingID=" << c->bookingId
                 << "  Room=" << c->roomId
                 << "  Student=" << c->studentId
                 << "  Day=" << c->day + 1
                 << "  Slot=" << c->slot + 1 << endl;
    }
};

// ============================================================
//  DATA STRUCTURE 5 — STACK  (undo, array-based)
// ============================================================
struct Stack {
    Action data[MAX_UNDO]; int top;
    Stack() : top(-1) {}

    bool isEmpty() { return top == -1; }
    bool isFull()  { return top == MAX_UNDO - 1; }

    void push(Action a) {
        if (isFull()) {
            for (int i = 0; i < MAX_UNDO - 1; i++) data[i] = data[i+1];
            data[top] = a;
        } else { data[++top] = a; }
    }

    Action pop() {
        Action e; e.type = "EMPTY";
        return isEmpty() ? e : data[top--];
    }

    void display() {
        if (isEmpty()) { cout << "  (stack empty)" << endl; return; }
        for (int i = top; i >= 0; i--)
            cout << "  [" << (top-i+1) << "] " << data[i].type
                 << "  Room=" << data[i].roomId
                 << "  Day="  << data[i].day+1
                 << "  Slot=" << data[i].slot+1 << endl;
    }
};

// ============================================================
//  DATA STRUCTURE 6 — PRIORITY QUEUE  (sorted linked list)
// ============================================================
struct PQNode { Classroom* room; float priority; PQNode* next; };

struct PriorityQueue {
    PQNode* head; int size;
    PriorityQueue() : head(nullptr), size(0) {}

    bool isEmpty() { return !head; }

    float score(Classroom* r) {
        float p = r->rating;
        if (r->hasAC)              p += 2.0f;
        if (r->category == "Good") p += 3.0f;
        return p;
    }

    void insert(Classroom* room) {
        PQNode* n = new PQNode(); n->room = room; n->priority = score(room); n->next = nullptr;
        if (!head || n->priority > head->priority) { n->next = head; head = n; }
        else {
            PQNode* c = head;
            while (c->next && c->next->priority >= n->priority) c = c->next;
            n->next = c->next; c->next = n;
        }
        size++;
    }

    Classroom* extractMax() {
        if (isEmpty()) return nullptr;
        PQNode* t = head; Classroom* r = t->room; head = head->next; delete t; size--;
        return r;
    }

    void clear() { while (!isEmpty()) extractMax(); }

    void display() {
        int rank = 1;
        for (PQNode* c = head; c; c = c->next)
            cout << "  #" << rank++ << " [" << c->room->roomId << "] "
                 << c->room->name << "  Score=" << c->priority
                 << "  Cat=" << c->room->category << endl;
    }
};

// ============================================================
//  SCABS  — Main system class
//  All public methods return nothing; they print via cout.
//  The frontend captures cout to display output in the GUI.
// ============================================================
class SCABS {
private:
    LinkedList    roomList;
    HashTable     hashTable;
    BST           bst;
    Queue         bookingQueue;
    Stack         undoStack;
    PriorityQueue pq;

    int nextRoomId;
    int nextBookingId;

    string dayNames[DAYS]   = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
    string slotNames[SLOTS] = {"8AM","9AM","10AM","11AM","12PM","1PM","2PM","3PM"};

    string categorize(Classroom* r) {
        return (r->hasAC && r->rating >= 3.5f) ? "Good" : "Bad";
    }

public:
    SCABS() : nextRoomId(101), nextBookingId(1001) {}

    // ── Add classroom ────────────────────────────────────────
    void addClassroom(string name, int capacity, bool hasAC, float rating) {
        Classroom* room    = new Classroom();
        room->roomId       = nextRoomId++;
        room->name         = name;
        room->capacity     = capacity;
        room->hasAC        = hasAC;
        room->rating       = rating;
        room->category     = categorize(room);
        room->bookedSlots  = nullptr;
        room->next = room->left = room->right = room->hashNext = nullptr;

        roomList.insertAtHead(room);
        hashTable.insert(room);
        bst.insert(room);

        cout << "  Room added: [" << room->roomId << "] " << name
             << "  Category: " << room->category << endl;
    }

    // ── Book room ────────────────────────────────────────────
    bool bookRoom(int roomId, int studentId, int day, int slot) {
        Classroom* room = hashTable.search(roomId);
        if (!room)                   { cout << "  Room not found.\n";  return false; }
        if (day  < 0 || day  >= DAYS)  { cout << "  Invalid day.\n";    return false; }
        if (slot < 0 || slot >= SLOTS) { cout << "  Invalid slot.\n";   return false; }
        if (isBooked(room->bookedSlots, day, slot)) {
            cout << "  Slot already booked!\n"; return false;
        }

        bookSlot(room->bookedSlots, day, slot);

        Booking* b   = new Booking();
        b->bookingId = nextBookingId++;
        b->roomId    = roomId; b->studentId = studentId;
        b->day       = day;   b->slot      = slot; b->next = nullptr;
        bookingQueue.enqueue(b);

        Action a; a.type = "BOOK"; a.roomId = roomId;
        a.day = day; a.slot = slot; a.bookingId = b->bookingId; a.prevState = false;
        undoStack.push(a);

        cout << "  Booking confirmed! ID=" << b->bookingId << "\n";
        return true;
    }

    // ── Mark slot empty ──────────────────────────────────────
    void markClassEmpty(int roomId, int day, int slot) {
        Classroom* room = hashTable.search(roomId);
        if (!room) { cout << "  Room not found.\n"; return; }
        if (!isBooked(room->bookedSlots, day, slot)) {
            cout << "  Slot is already free.\n"; return;
        }

        freeSlot(room->bookedSlots, day, slot);

        Action a; a.type = "MARK_EMPTY"; a.roomId = roomId;
        a.day = day; a.slot = slot; a.bookingId = -1; a.prevState = true;
        undoStack.push(a);
        cout << "  Slot marked as empty. Room now available.\n";
    }

    // ── Undo last action ─────────────────────────────────────
    void undoLastAction() {
        if (undoStack.isEmpty()) { cout << "  Nothing to undo.\n"; return; }
        Action a = undoStack.pop();
        Classroom* room = hashTable.search(a.roomId);
        if (!room) { cout << "  Cannot undo — room no longer exists.\n"; return; }

        if (a.prevState) bookSlot(room->bookedSlots, a.day, a.slot);
        else             freeSlot(room->bookedSlots, a.day, a.slot);

        cout << "  Undid: " << a.type << "  Room " << a.roomId
             << "  Day " << a.day+1 << "  Slot " << a.slot+1 << "\n";
    }

    // ── Recommend rooms ──────────────────────────────────────
    void recommendRooms(int day, int slot) {
        pq.clear();
        for (Classroom* c = roomList.head; c; c = c->next)
            if (!isBooked(c->bookedSlots, day, slot)) pq.insert(c);

        if (pq.isEmpty()) { cout << "  No rooms free at that time.\n"; return; }

        cout << "\n  ═══ Recommended Rooms (Best First) ═══\n";
        int rank = 1;
        while (!pq.isEmpty()) {
            Classroom* r = pq.extractMax();
            cout << "  #" << rank++ << " [" << r->roomId << "] " << r->name
                 << "  Cap=" << r->capacity
                 << "  AC=" << (r->hasAC ? "Yes" : "No")
                 << "  Rating=" << r->rating
                 << "  " << r->category << "\n";
        }
    }

    // ── Display room info + timetable ────────────────────────
    void displayRoomInfo(int roomId) {
        Classroom* room = hashTable.search(roomId);
        if (!room) { cout << "  Room not found.\n"; return; }

        cout << "\n  ═══ Room Info ═══\n";
        cout << "  ID: " << room->roomId << "  Name: " << room->name << "\n";
        cout << "  Capacity: " << room->capacity
             << "  AC: " << (room->hasAC ? "Yes" : "No") << "\n";
        cout << "  Rating: " << room->rating
             << "  Category: " << room->category << "\n";

        cout << "\n  Timetable  ([ ] free  [X] booked)\n";
        cout << "       ";
        for (int s = 0; s < SLOTS; s++) cout << slotNames[s] << "\t";
        cout << "\n";
        for (int d = 0; d < DAYS; d++) {
            cout << "  " << dayNames[d] << "  ";
            for (int s = 0; s < SLOTS; s++)
                cout << (isBooked(room->bookedSlots, d, s) ? "[X]\t" : "[ ]\t");
            cout << "\n";
        }
    }

    // ── Reschedule ───────────────────────────────────────────
    void rescheduleClass(int roomId, int od, int os, int nd, int ns) {
        Classroom* room = hashTable.search(roomId);
        if (!room) { cout << "  Room not found.\n"; return; }
        if (!isBooked(room->bookedSlots, od, os)) {
            cout << "  Old slot is not booked.\n"; return;
        }
        if (isBooked(room->bookedSlots, nd, ns)) {
            cout << "  New slot is already occupied.\n"; return;
        }
        freeSlot(room->bookedSlots, od, os);
        bookSlot(room->bookedSlots, nd, ns);
        cout << "  Class rescheduled successfully.\n";
    }

    // ── Find free rooms ──────────────────────────────────────
    void findFreeRooms(int day, int slot) {
        cout << "\n  Free rooms on " << dayNames[day]
             << " at " << slotNames[slot] << ":\n";
        bool found = false;
        for (Classroom* c = roomList.head; c; c = c->next) {
            if (!isBooked(c->bookedSlots, day, slot)) {
                cout << "  [" << c->roomId << "] " << c->name
                     << "  " << c->category << "\n";
                found = true;
            }
        }
        if (!found) cout << "  No free rooms.\n";
    }

    // ── Check availability ───────────────────────────────────
    void checkAvailability(int roomId, int day, int slot) {
        Classroom* room = hashTable.search(roomId);
        if (!room) { cout << "  Room not found.\n"; return; }
        bool free = !isBooked(room->bookedSlots, day, slot);
        cout << "  Room " << roomId << " on "
             << dayNames[day] << " " << slotNames[slot]
             << " is " << (free ? "FREE" : "BOOKED") << "\n";
    }

    // ── Display helpers ──────────────────────────────────────
    void displayAllRooms()      { cout << "\n  All Rooms:\n";             roomList.display(); }
    void displaySortedByID()    { cout << "\n  Rooms Sorted by ID:\n";    bst.displaySorted(); }
    void displayBookingQueue()  { cout << "\n  Booking Queue (FIFO):\n";  bookingQueue.display(); }
    void displayUndoStack()     { cout << "\n  Undo Stack:\n";            undoStack.display(); }
    void displayPriorityQueue() { cout << "\n  Priority Queue:\n";        pq.display(); }
};

#endif // SCABS_BACKEND_H
