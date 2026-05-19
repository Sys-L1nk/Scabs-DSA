// ══════════════════════════════════════════════════════════════
//  RoomManager.cpp  — SCABS Backend
//  Pure C++ / Qt-string backend.  Zero UI code.
//
//  Data Structures implemented:
//   1. Linked List     — rooms (tail-insert) & bookings (head-insert)
//   2. Hash Table      — O(1) room lookup  (separate chaining, prime 101)
//   3. BST             — rooms sorted by roomID  (recursive insert/delete)
//   4. Queue (FIFO)    — waiting requests  (front dequeue / rear enqueue)
//   5. Stack (LIFO)    — undo buffer  (array-based, MAX_UNDO=50)
//   6. Priority Queue  — room recommendations  (sorted linked list by score)
//   7. 2-D Array       — per-room weekly timetable  [7 days][8 slots]
// ══════════════════════════════════════════════════════════════
#include "RoomManager.h"

// ── Static label arrays ────────────────────────────────────────
const char* RoomManager::DAY_NAMES[DAYS]  =
    { "Mon","Tue","Wed","Thu","Fri","Sat","Sun" };
const char* RoomManager::SLOT_NAMES[SLOTS] =
    { "8AM","9AM","10AM","11AM","12PM","1PM","2PM","3PM" };

// ══════════════════════════════════════════════════════════════
//  Node constructors
// ══════════════════════════════════════════════════════════════
Room::Room(int id, QString num, int cap, bool ac, float rat)
    : roomID(id), roomNumber(num), capacity(cap),
      isAC(ac), isAvailable(true), rating(rat),
      next(nullptr), hashNext(nullptr), left(nullptr), right(nullptr)
{
    for (int d = 0; d < DAYS; ++d)
        for (int s = 0; s < SLOTS; ++s)
            timetable[d][s] = false;
}

Booking::Booking(int bid, int rid, QString rnum,
                 QString sname, QString sid, QString dt, int d, int s)
    : bookingID(bid), roomID(rid), roomNumber(rnum),
      studentName(sname), studentID(sid), dateTime(dt),
      day(d), slot(s), isActive(true), next(nullptr) {}

Request::Request(int rid, QString sname, QString sid,
                 bool ac, int cap, QString time)
    : requestID(rid), studentName(sname), studentID(sid),
      needsAC(ac), minCapacity(cap), requestTime(time), next(nullptr) {}

HistoryEntry::HistoryEntry(QString act, int bid, int rid,
                           QString rnum, QString sname,
                           QString sid, QString time)
    : action(act), bookingID(bid), roomID(rid), roomNumber(rnum),
      studentName(sname), studentID(sid), timestamp(time), next(nullptr) {}

// ══════════════════════════════════════════════════════════════
//  RoomManager constructor
// ══════════════════════════════════════════════════════════════
RoomManager::RoomManager()
    : roomHead(nullptr), bookingHead(nullptr),
      queueFront(nullptr), queueRear(nullptr),
      stackTop(nullptr),
      undoTop(-1),
      roomCount(0), bookingCount(0), queueSize(0),
      nextBookingID(1001), nextRequestID(1), nextRoomID(101),
      bstRoot(nullptr)
{
    // DSA 2 — initialise hash buckets
    for (int i = 0; i < HASH_SIZE; ++i)
        hashBuckets[i] = nullptr;

    // Seed default rooms (same as original project)
    addRoom(101, "LH-1",        60, true,  4.5f);
    addRoom(102, "LH-2",        40, false, 3.0f);
    addRoom(103, "CS-Lab",      30, true,  4.8f);
    addRoom(104, "Seminar Hall",100, true,  4.2f);
    addRoom(105, "Room-204",    35, false,  2.8f);
    nextRoomID = 106; // next auto-id
}

// ══════════════════════════════════════════════════════════════
//  HELPER — categorise a room from its attributes
// ══════════════════════════════════════════════════════════════
QString RoomManager::categorize(Room* r) const
{
    if (r->isAC && r->rating >= 4.0f) return "Good";
    if (r->rating >= 3.0f)            return "Average";
    return "Bad";
}

// ══════════════════════════════════════════════════════════════
//  DSA 2 — HASH TABLE  (separate chaining, division method)
// ══════════════════════════════════════════════════════════════
int RoomManager::hashFunc(int key) const
{
    return (key % HASH_SIZE + HASH_SIZE) % HASH_SIZE;
}

void RoomManager::hashInsert(Room* r)
{
    int idx = hashFunc(r->roomID);
    r->hashNext       = hashBuckets[idx];
    hashBuckets[idx]  = r;
}

Room* RoomManager::hashSearch(int roomID)
{
    int idx = hashFunc(roomID);
    Room* cur = hashBuckets[idx];
    while (cur) {
        if (cur->roomID == roomID) return cur;
        cur = cur->hashNext;
    }
    return nullptr;
}

void RoomManager::hashRemove(int roomID)
{
    int   idx  = hashFunc(roomID);
    Room* cur  = hashBuckets[idx];
    Room* prev = nullptr;
    while (cur) {
        if (cur->roomID == roomID) {
            if (prev) prev->hashNext = cur->hashNext;
            else      hashBuckets[idx] = cur->hashNext;
            return;
        }
        prev = cur;
        cur  = cur->hashNext;
    }
}

// ══════════════════════════════════════════════════════════════
//  DSA 3 — BST  (by roomID, recursive)
// ══════════════════════════════════════════════════════════════
Room* RoomManager::bstInsertNode(Room* node, Room* room)
{
    if (!node) return room;
    if (room->roomID < node->roomID)
        node->left  = bstInsertNode(node->left,  room);
    else if (room->roomID > node->roomID)
        node->right = bstInsertNode(node->right, room);
    return node;
}

void RoomManager::bstInorderHelper(Room* node, Room** out, int& idx) const
{
    if (!node) return;
    bstInorderHelper(node->left,  out, idx);
    out[idx++] = node;
    bstInorderHelper(node->right, out, idx);
}

void RoomManager::bstInorder(Room** outArray, int& idx) const
{
    idx = 0;
    bstInorderHelper(bstRoot, outArray, idx);
}

Room* RoomManager::bstMin(Room* node) const
{
    while (node && node->left) node = node->left;
    return node;
}

Room* RoomManager::bstDeleteNode(Room* node, int roomID)
{
    if (!node) return nullptr;
    if (roomID < node->roomID) {
        node->left  = bstDeleteNode(node->left,  roomID);
    } else if (roomID > node->roomID) {
        node->right = bstDeleteNode(node->right, roomID);
    } else {
        // Found — handle 3 cases
        if (!node->left)  return node->right;
        if (!node->right) return node->left;
        // Two children: replace with inorder successor
        Room* succ = bstMin(node->right);
        node->roomID     = succ->roomID;
        node->roomNumber = succ->roomNumber;
        node->capacity   = succ->capacity;
        node->isAC       = succ->isAC;
        node->isAvailable= succ->isAvailable;
        node->rating     = succ->rating;
        node->category   = succ->category;
        for (int d = 0; d < DAYS; ++d)
            for (int s = 0; s < SLOTS; ++s)
                node->timetable[d][s] = succ->timetable[d][s];
        node->right = bstDeleteNode(node->right, succ->roomID);
    }
    return node;
}

// ══════════════════════════════════════════════════════════════
//  DSA 1 — LINKED LIST : Room operations
// ══════════════════════════════════════════════════════════════
void RoomManager::addRoom(int id, QString num, int cap, bool ac, float rating)
{
    if (findRoom(id)) return; // duplicate guard

    Room* r    = new Room(id, num, cap, ac, rating);
    r->category = categorize(r);

    // ── Linked List: tail insert (preserves insertion order) ──
    if (!roomHead) {
        roomHead = r;
    } else {
        Room* cur = roomHead;
        while (cur->next) cur = cur->next;
        cur->next = r;
    }
    roomCount++;

    // ── DSA 2: Hash Table insert ──
    hashInsert(r);

    // ── DSA 3: BST insert ──
    r->left = r->right = nullptr;
    bstRoot = bstInsertNode(bstRoot, r);
}

Room* RoomManager::findRoom(int id)
{
    // Use DSA 2 (O(1) average) for lookup
    return hashSearch(id);
}

bool RoomManager::deleteRoom(int id)
{
    // Remove from Linked List
    Room* prev = nullptr;
    Room* cur  = roomHead;
    while (cur && cur->roomID != id) { prev = cur; cur = cur->next; }
    if (!cur) return false;

    if (!prev) roomHead   = cur->next;
    else       prev->next = cur->next;

    // Remove from Hash Table
    hashRemove(id);

    // Remove from BST (value-copy approach in bstDeleteNode)
    bstRoot = bstDeleteNode(bstRoot, id);

    delete cur;
    roomCount--;
    return true;
}

// ══════════════════════════════════════════════════════════════
//  DSA 4 — QUEUE : General (legacy) booking  (head-insert list)
// ══════════════════════════════════════════════════════════════
int RoomManager::bookRoom(int roomID, QString studentName,
                          QString studentID, QString dateTime)
{
    Room* room = findRoom(roomID);
    if (!room || !room->isAvailable) return -1;

    int      bid = nextBookingID++;
    Booking* nb  = new Booking(bid, roomID, room->roomNumber,
                               studentName, studentID, dateTime);
    nb->next    = bookingHead;
    bookingHead = nb;

    room->isAvailable = false;
    bookingCount++;
    return bid;
}

bool RoomManager::cancelBooking(int bookingID)
{
    Booking* cur = bookingHead;
    while (cur) {
        if (cur->bookingID == bookingID && cur->isActive) {
            cur->isActive = false;
            Room* room = findRoom(cur->roomID);
            if (room) room->isAvailable = true;
            return true;
        }
        cur = cur->next;
    }
    return false;
}

// ══════════════════════════════════════════════════════════════
//  DSA 7 — 2-D TIMETABLE : Slot booking
// ══════════════════════════════════════════════════════════════
bool RoomManager::bookSlot(int roomID, int /*studentIDnum*/,
                           int day, int slot,
                           QString studentName, QString studentID,
                           QString dateTime)
{
    if (day < 0 || day >= DAYS || slot < 0 || slot >= SLOTS) return false;

    Room* room = findRoom(roomID);
    if (!room) return false;
    if (room->timetable[day][slot]) return false; // already booked

    // Mark 2-D array
    room->timetable[day][slot] = true;

    // Create booking record and head-insert into booking list
    int      bid = nextBookingID++;
    Booking* nb  = new Booking(bid, roomID, room->roomNumber,
                               studentName, studentID, dateTime, day, slot);
    nb->next    = bookingHead;
    bookingHead = nb;
    bookingCount++;

    // DSA 5 — push undo action
    UndoAction a;
    a.type      = "BOOK_SLOT";
    a.roomID    = roomID;
    a.day       = day;
    a.slot      = slot;
    a.bookingID = bid;
    a.prevState = false;
    undoPush(a);

    return true;
}

bool RoomManager::markSlotEmpty(int roomID, int day, int slot)
{
    if (day < 0 || day >= DAYS || slot < 0 || slot >= SLOTS) return false;
    Room* room = findRoom(roomID);
    if (!room) return false;
    if (!room->timetable[day][slot]) return false; // already empty

    room->timetable[day][slot] = false;

    UndoAction a;
    a.type      = "MARK_EMPTY";
    a.roomID    = roomID;
    a.day       = day;
    a.slot      = slot;
    a.bookingID = -1;
    a.prevState = true;
    undoPush(a);

    return true;
}

bool RoomManager::rescheduleSlot(int roomID,
                                  int oldDay, int oldSlot,
                                  int newDay, int newSlot)
{
    if (oldDay  < 0 || oldDay  >= DAYS || oldSlot < 0 || oldSlot >= SLOTS) return false;
    if (newDay  < 0 || newDay  >= DAYS || newSlot < 0 || newSlot >= SLOTS) return false;

    Room* room = findRoom(roomID);
    if (!room) return false;
    if (!room->timetable[oldDay][oldSlot]) return false; // nothing to move
    if ( room->timetable[newDay][newSlot]) return false; // destination occupied

    room->timetable[oldDay][oldSlot] = false;
    room->timetable[newDay][newSlot] = true;
    return true;
}

// ══════════════════════════════════════════════════════════════
//  DSA 4 — QUEUE : Waiting request queue
// ══════════════════════════════════════════════════════════════
void RoomManager::enqueue(QString sname, QString sid,
                           bool ac, int cap, QString time)
{
    Request* req = new Request(nextRequestID++, sname, sid, ac, cap, time);
    if (!queueRear) {
        queueFront = queueRear = req;
    } else {
        queueRear->next = req;
        queueRear = req;
    }
    queueSize++;
}

bool RoomManager::processNextRequest(QString dateTime)
{
    if (!queueFront) return false;

    Request* req = queueFront;
    Room* cur    = roomHead;
    while (cur) {
        if (cur->isAvailable &&
            cur->isAC       == req->needsAC &&
            cur->capacity   >= req->minCapacity)
        {
            int      bid = nextBookingID++;
            Booking* nb  = new Booking(bid, cur->roomID, cur->roomNumber,
                                       req->studentName, req->studentID, dateTime);
            nb->next    = bookingHead;
            bookingHead = nb;
            cur->isAvailable = false;
            bookingCount++;

            // Dequeue
            queueFront = queueFront->next;
            if (!queueFront) queueRear = nullptr;
            queueSize--;
            delete req;

            pushHistory("QUEUE_BOOK", bid, cur->roomID,
                        cur->roomNumber, nb->studentName,
                        nb->studentID, dateTime);
            return true;
        }
        cur = cur->next;
    }
    return false; // no matching room
}

// ══════════════════════════════════════════════════════════════
//  DSA 5 — UNDO STACK  (array-based, fixed MAX_UNDO)
// ══════════════════════════════════════════════════════════════
void RoomManager::undoPush(UndoAction a)
{
    if (undoTop == MAX_UNDO - 1) {
        // Shift left — drop oldest entry
        for (int i = 0; i < MAX_UNDO - 1; ++i)
            undoData[i] = undoData[i + 1];
        undoData[undoTop] = a;
    } else {
        undoData[++undoTop] = a;
    }
}

UndoAction RoomManager::undoPop()
{
    UndoAction empty;
    empty.type = "EMPTY";
    empty.roomID = -1;
    if (undoTop == -1) return empty;
    return undoData[undoTop--];
}

bool RoomManager::undoLastAction()
{
    if (isUndoEmpty()) return false;

    UndoAction a = undoPop();
    Room* room   = findRoom(a.roomID);
    if (!room) return false;

    if (a.day >= 0 && a.day < DAYS && a.slot >= 0 && a.slot < SLOTS)
        room->timetable[a.day][a.slot] = a.prevState;

    // Also update booking list if applicable
    if (a.bookingID > 0 && a.type == "BOOK_SLOT") {
        Booking* b = bookingHead;
        while (b) {
            if (b->bookingID == a.bookingID) { b->isActive = false; break; }
            b = b->next;
        }
    }
    return true;
}

// ══════════════════════════════════════════════════════════════
//  DSA 6 — PRIORITY QUEUE  (sorted linked list by score)
//  Returns up to maxOut rooms, best first, for given day/slot.
// ══════════════════════════════════════════════════════════════
float RoomManager::pqScore(Room* r) const
{
    float p = r->rating;
    if (r->isAC)             p += 2.0f;
    if (r->category == "Good")    p += 3.0f;
    else if (r->category == "Average") p += 1.0f;
    return p;
}

int RoomManager::recommendRooms(int day, int slot,
                                 Room** outArray, float* scores,
                                 int maxOut)
{
    // Build a temporary sorted linked list (Priority Queue)
    PQNode* pqHead = nullptr;
    int pqSize = 0;

    Room* cur = roomHead;
    while (cur) {
        // Include room if slot is free (or day/slot invalid = all rooms)
        bool slotOk = (day < 0 || slot < 0) ? true
                                              : !cur->timetable[day][slot];
        if (slotOk) {
            PQNode* node = new PQNode();
            node->room     = cur;
            node->priority = pqScore(cur);
            node->next     = nullptr;

            // Insert in descending priority order
            if (!pqHead || node->priority > pqHead->priority) {
                node->next = pqHead;
                pqHead     = node;
            } else {
                PQNode* p = pqHead;
                while (p->next && p->next->priority >= node->priority)
                    p = p->next;
                node->next = p->next;
                p->next    = node;
            }
            pqSize++;
        }
        cur = cur->next;
    }

    // Extract up to maxOut
    int count = 0;
    while (pqHead && count < maxOut) {
        PQNode* tmp = pqHead;
        outArray[count] = tmp->room;
        scores[count]   = tmp->priority;
        count++;
        pqHead = pqHead->next;
        delete tmp;
    }
    // Free remainder
    while (pqHead) {
        PQNode* tmp = pqHead;
        pqHead = pqHead->next;
        delete tmp;
    }
    return count;
}

// ══════════════════════════════════════════════════════════════
//  HISTORY STACK  (linked-list, LIFO, unbounded)
// ══════════════════════════════════════════════════════════════
void RoomManager::pushHistory(QString action, int bid, int rid,
                               QString rnum, QString sname,
                               QString sid, QString time)
{
    HistoryEntry* h = new HistoryEntry(action, bid, rid, rnum, sname, sid, time);
    h->next  = stackTop;
    stackTop = h;
}
