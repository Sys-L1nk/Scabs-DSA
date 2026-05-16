#ifndef ROOMMANAGER_H
#define ROOMMANAGER_H

#include <QString>

// ─────────────────────────────────────────────
// Linked List Node: Room
// ─────────────────────────────────────────────
struct Room {
    int     roomID;
    QString roomNumber;
    int     capacity;
    bool    isAC;
    bool    isAvailable;
    Room*   next;

    Room(int id, QString num, int cap, bool ac)
        : roomID(id), roomNumber(num), capacity(cap),
          isAC(ac), isAvailable(true), next(nullptr) {}
};

// ─────────────────────────────────────────────
// Linked List Node: Booking
// ─────────────────────────────────────────────
struct Booking {
    int     bookingID;
    int     roomID;
    QString roomNumber;
    QString studentName;
    QString studentID;
    QString dateTime;
    bool    isActive;
    Booking* next;

    Booking(int bid, int rid, QString rnum, QString sname, QString sid, QString dt)
        : bookingID(bid), roomID(rid), roomNumber(rnum),
          studentName(sname), studentID(sid), dateTime(dt),
          isActive(true), next(nullptr) {}
};

// ─────────────────────────────────────────────
// Queue Node: Request
// ─────────────────────────────────────────────
struct Request {
    int     requestID;
    QString studentName;
    QString studentID;
    bool    needsAC;
    int     minCapacity;
    QString requestTime;
    Request* next;

    Request(int rid, QString sname, QString sid, bool ac, int cap, QString time)
        : requestID(rid), studentName(sname), studentID(sid),
          needsAC(ac), minCapacity(cap), requestTime(time), next(nullptr) {}
};

// ─────────────────────────────────────────────
// Stack Node: HistoryEntry
// ─────────────────────────────────────────────
struct HistoryEntry {
    QString action;       // BOOK, CANCEL, ADD_ROOM, DELETE_ROOM
    int     bookingID;
    int     roomID;
    QString roomNumber;
    QString studentName;
    QString studentID;
    QString timestamp;
    HistoryEntry* next;

    HistoryEntry(QString act, int bid, int rid, QString rnum, QString sname, QString sid, QString time)
        : action(act), bookingID(bid), roomID(rid), roomNumber(rnum),
          studentName(sname), studentID(sid), timestamp(time), next(nullptr) {}
};

// ─────────────────────────────────────────────
// RoomManager: manages all 3 DSA structures
// ─────────────────────────────────────────────
class RoomManager {
public:
    // Linked List heads
    Room*    roomHead;
    Booking* bookingHead;

    // Queue pointers
    Request* queueFront;
    Request* queueRear;

    // Stack top
    HistoryEntry* stackTop;

    // Counters
    int roomCount;
    int bookingCount;
    int queueSize;
    int nextBookingID;
    int nextRequestID;

    RoomManager();

    // Room operations (Linked List)
    void  addRoom(int id, QString num, int cap, bool ac);
    Room* findRoom(int id);
    bool  deleteRoom(int id);

    // Booking operations (Linked List)
    int  bookRoom(int roomID, QString studentName, QString studentID, QString dateTime);
    bool cancelBooking(int bookingID);

    // Queue operations
    void enqueue(QString sname, QString sid, bool ac, int cap, QString time);
    bool processNextRequest(QString dateTime);

    // Stack operations
    void pushHistory(QString action, int bid, int rid, QString rnum, QString sname, QString sid, QString time);
};

#endif // ROOMMANAGER_H
