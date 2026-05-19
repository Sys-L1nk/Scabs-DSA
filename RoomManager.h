#ifndef ROOMMANAGER_H
#define ROOMMANAGER_H

#include <QString>

// ══════════════════════════════════════════════════════════════
//  CONSTANTS
// ══════════════════════════════════════════════════════════════
static const int DAYS      = 7;
static const int SLOTS     = 8;        // 8 AM – 3 PM
static const int HASH_SIZE = 101;      // prime → fewer collisions
static const int MAX_UNDO  = 50;

// ──────────────────────────────────────────────────────────────
//  DSA 1 / 2 / 3 node — Classroom  (Room)
//  Used by: Linked List  |  Hash Table  |  BST
// ──────────────────────────────────────────────────────────────
struct Room {
    // Core fields
    int     roomID;
    QString roomNumber;
    int     capacity;
    bool    isAC;
    bool    isAvailable;   // global availability flag (simple booking)
    float   rating;        // 0.0 – 5.0
    QString category;      // "Good" | "Average" | "Bad"

    // DSA 7 — 2-D timetable array  [day 0-6][slot 0-7]
    bool timetable[DAYS][SLOTS];

    // DSA 1 — Linked List pointer
    Room* next;

    // DSA 2 — Hash Table chaining pointer
    Room* hashNext;

    // DSA 3 — BST pointers
    Room* left;
    Room* right;

    Room(int id, QString num, int cap, bool ac, float rat = 3.0f);
};

// ──────────────────────────────────────────────────────────────
//  DSA 4 node — Booking  (Queue element)
// ──────────────────────────────────────────────────────────────
struct Booking {
    int     bookingID;
    int     roomID;
    QString roomNumber;
    QString studentName;
    QString studentID;
    QString dateTime;
    int     day;           // timetable day  (0-6), -1 if general booking
    int     slot;          // timetable slot (0-7), -1 if general booking
    bool    isActive;
    Booking* next;

    Booking(int bid, int rid, QString rnum,
            QString sname, QString sid, QString dt,
            int d = -1, int s = -1);
};

// ──────────────────────────────────────────────────────────────
//  DSA 4 node — Request  (waiting-queue element)
// ──────────────────────────────────────────────────────────────
struct Request {
    int     requestID;
    QString studentName;
    QString studentID;
    bool    needsAC;
    int     minCapacity;
    QString requestTime;
    Request* next;

    Request(int rid, QString sname, QString sid,
            bool ac, int cap, QString time);
};

// ──────────────────────────────────────────────────────────────
//  DSA 5 — Undo Action record  (stored in array-based stack)
// ──────────────────────────────────────────────────────────────
struct UndoAction {
    QString type;       // "BOOK_SLOT" | "MARK_EMPTY" | "CANCEL"
    int     roomID;
    int     day;
    int     slot;
    int     bookingID;
    bool    prevState;  // timetable cell value before action
};

// ──────────────────────────────────────────────────────────────
//  DSA 6 node — Priority Queue element
// ──────────────────────────────────────────────────────────────
struct PQNode {
    Room*   room;
    float   priority;
    PQNode* next;
};

// ──────────────────────────────────────────────────────────────
//  History Stack node  (linked-list based, LIFO, unbounded)
// ──────────────────────────────────────────────────────────────
struct HistoryEntry {
    QString action;
    int     bookingID;
    int     roomID;
    QString roomNumber;
    QString studentName;
    QString studentID;
    QString timestamp;
    HistoryEntry* next;

    HistoryEntry(QString act, int bid, int rid,
                 QString rnum, QString sname, QString sid, QString time);
};

// ══════════════════════════════════════════════════════════════
//  RoomManager
//  Pure backend — no Qt widgets, no UI code.
//  Exposes all DSA structures as public members so MainWindow
//  can traverse them for display without coupling logic.
// ══════════════════════════════════════════════════════════════
class RoomManager {
public:
    // ── DSA 1 — Linked List ────────────────────────────────────
    Room*    roomHead;
    Booking* bookingHead;

    // ── DSA 4 — Queue (waiting requests) ──────────────────────
    Request* queueFront;
    Request* queueRear;

    // ── History Stack (linked-list, LIFO) ─────────────────────
    HistoryEntry* stackTop;

    // ── DSA 5 — Undo Stack (array-based) ──────────────────────
    UndoAction undoData[MAX_UNDO];
    int        undoTop;          // -1 = empty

    // ── Counters / ID generators ───────────────────────────────
    int roomCount;
    int bookingCount;
    int queueSize;
    int nextBookingID;
    int nextRequestID;
    int nextRoomID;    // auto-assigned when caller passes 0

    // ── Day / slot label arrays (part of DSA 7 display) ───────
    static const char* DAY_NAMES[DAYS];
    static const char* SLOT_NAMES[SLOTS];

    // ── Constructor ────────────────────────────────────────────
    RoomManager();

    // ── DSA 1 — Linked List : Room operations ─────────────────
    void  addRoom(int id, QString num, int cap, bool ac, float rating = 3.0f);
    Room* findRoom(int id);
    bool  deleteRoom(int id);

    // ── DSA 2 — Hash Table ────────────────────────────────────
    Room* hashSearch(int roomID);

    // ── DSA 3 — BST ───────────────────────────────────────────
    // Returns rooms in sorted order (inorder traversal)
    // Caller must pre-allocate array of size roomCount
    void bstInorder(Room** outArray, int& idx) const;

    // ── DSA 4 — Booking Queue (general / legacy) ───────────────
    int  bookRoom(int roomID, QString studentName,
                  QString studentID, QString dateTime);
    bool cancelBooking(int bookingID);

    // ── DSA 4 — Timetable slot booking ────────────────────────
    bool bookSlot(int roomID, int studentIDnum, int day, int slot,
                  QString studentName, QString studentID, QString dateTime);
    bool markSlotEmpty(int roomID, int day, int slot);
    bool rescheduleSlot(int roomID,
                        int oldDay, int oldSlot,
                        int newDay, int newSlot);

    // ── DSA 4 — Waiting Request Queue ─────────────────────────
    void enqueue(QString sname, QString sid,
                 bool ac, int cap, QString time);
    bool processNextRequest(QString dateTime);

    // ── DSA 5 — Undo Stack ────────────────────────────────────
    bool undoLastAction();
    bool isUndoEmpty() const { return undoTop == -1; }

    // ── DSA 6 — Priority Queue (recommend rooms) ───────────────
    // Fills outArray (caller allocates) with rooms sorted best-first
    // for the given day/slot.  Returns count placed.
    int recommendRooms(int day, int slot, Room** outArray, float* scores, int maxOut);

    // ── History Stack ─────────────────────────────────────────
    void pushHistory(QString action, int bid, int rid,
                     QString rnum, QString sname,
                     QString sid, QString time);

private:
    // ── DSA 2 internal ────────────────────────────────────────
    Room* hashBuckets[HASH_SIZE];
    int   hashFunc(int key) const;
    void  hashInsert(Room* r);
    void  hashRemove(int roomID);

    // ── DSA 3 internal ────────────────────────────────────────
    Room* bstRoot;
    Room* bstInsertNode(Room* node, Room* room);
    void  bstInorderHelper(Room* node, Room** out, int& idx) const;
    Room* bstDeleteNode(Room* node, int roomID);
    Room*  bstMin(Room* node) const;

    // ── DSA 5 internal ────────────────────────────────────────
    void undoPush(UndoAction a);
    UndoAction undoPop();

    // ── DSA 6 internal ────────────────────────────────────────
    float pqScore(Room* r) const;

    // ── Category helper ───────────────────────────────────────
    QString categorize(Room* r) const;
};

#endif // ROOMMANAGER_H
