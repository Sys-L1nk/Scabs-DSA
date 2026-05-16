#include "RoomManager.h"

// Constructor: preload 4 rooms
RoomManager::RoomManager()
    : roomHead(nullptr), bookingHead(nullptr),
      queueFront(nullptr), queueRear(nullptr),
      stackTop(nullptr),
      roomCount(0), bookingCount(0), queueSize(0),
      nextBookingID(1001), nextRequestID(1)
{
    // Preload rooms (same as your original C++ code)
    addRoom(101, "A-101", 30, true);
    addRoom(102, "A-102", 50, false);
    addRoom(103, "B-201", 20, true);
    addRoom(105, "C-301", 60, true);
}

// ─── LINKED LIST: Add room at tail ───
void RoomManager::addRoom(int id, QString num, int cap, bool ac)
{
    if (findRoom(id)) return; // duplicate check

    Room* newRoom = new Room(id, num, cap, ac);

    if (!roomHead) {
        roomHead = newRoom;           // first node
    } else {
        Room* cur = roomHead;
        while (cur->next) cur = cur->next;  // traverse to tail
        cur->next = newRoom;          // attach at end
    }
    roomCount++;
}

// ─── LINKED LIST: Search room by ID ───
Room* RoomManager::findRoom(int id)
{
    Room* cur = roomHead;
    while (cur) {
        if (cur->roomID == id) return cur;
        cur = cur->next;
    }
    return nullptr; // not found
}

// ─── LINKED LIST: Delete room by ID ───
bool RoomManager::deleteRoom(int id)
{
    Room* prev = nullptr;
    Room* cur  = roomHead;

    while (cur && cur->roomID != id) {
        prev = cur;
        cur  = cur->next;
    }
    if (!cur) return false; // not found

    if (!prev) roomHead = cur->next;  // deleting head
    else       prev->next = cur->next; // bypass node

    delete cur;
    roomCount--;
    return true;
}

// ─── LINKED LIST: Book a room, insert at head ───
int RoomManager::bookRoom(int roomID, QString studentName, QString studentID, QString dateTime)
{
    Room* room = findRoom(roomID);
    if (!room || !room->isAvailable) return -1; // failed

    int bid = nextBookingID++;
    Booking* nb = new Booking(bid, roomID, room->roomNumber, studentName, studentID, dateTime);

    // Insert at head (same as your C++ code)
    nb->next     = bookingHead;
    bookingHead  = nb;

    room->isAvailable = false;
    bookingCount++;
    return bid;
}

// ─── LINKED LIST: Cancel booking ───
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

// ─── QUEUE: Enqueue request at rear ───
void RoomManager::enqueue(QString sname, QString sid, bool ac, int cap, QString time)
{
    Request* req = new Request(nextRequestID++, sname, sid, ac, cap, time);

    if (!queueRear) {
        queueFront = queueRear = req; // first item
    } else {
        queueRear->next = req;        // attach at rear
        queueRear = req;              // move rear pointer
    }
    queueSize++;
}

// ─── QUEUE: Dequeue front, auto-book matching room ───
bool RoomManager::processNextRequest(QString dateTime)
{
    if (!queueFront) return false;

    Request* req = queueFront; // peek front

    // Find a matching available room
    Room* cur = roomHead;
    while (cur) {
        if (cur->isAvailable && cur->isAC == req->needsAC && cur->capacity >= req->minCapacity) {
            // Book the room
            int bid = nextBookingID++;
            Booking* nb = new Booking(bid, cur->roomID, cur->roomNumber,
                                      req->studentName, req->studentID, dateTime);
            nb->next    = bookingHead;
            bookingHead = nb;
            cur->isAvailable = false;
            bookingCount++;

            // Dequeue front
            queueFront = queueFront->next;
            if (!queueFront) queueRear = nullptr;
            queueSize--;
            delete req;
            return true;
        }
        cur = cur->next;
    }
    return false; // no matching room found
}

// ─── STACK: Push history entry on top ───
void RoomManager::pushHistory(QString action, int bid, int rid,
                               QString rnum, QString sname, QString sid, QString time)
{
    HistoryEntry* h = new HistoryEntry(action, bid, rid, rnum, sname, sid, time);
    h->next   = stackTop;  // point to previous top
    stackTop  = h;         // new top
}
