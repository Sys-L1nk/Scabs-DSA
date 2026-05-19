#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QStatusBar>
#include "RoomManager.h"

// ══════════════════════════════════════════════════════════════
//  MainWindow — pure UI layer.
//  All business logic lives in RoomManager (backend).
//  This class only: builds widgets, connects signals,
//  calls mgr.xxx(), and re-renders tables.
// ══════════════════════════════════════════════════════════════
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // ── Tab 1: Rooms ──────────────────────────────────────────
    void onAddRoom();
    void onDeleteRoom();
    void onBookRoom();           // general (availability flag) booking

    // ── Tab 2: Bookings ───────────────────────────────────────
    void onCancelBooking();

    // ── Tab 3: Request Queue ──────────────────────────────────
    void onJoinQueue();
    void onProcessQueue();

    // ── Tab 4: Timetable (DSA 7) ──────────────────────────────
    void onBookSlot();
    void onMarkEmpty();
    void onReschedule();
    void onShowTimetable();      // fill timetable grid for selected room

    // ── Tab 5: Undo Stack (DSA 5) ────────────────────────────
    void onUndo();

    // ── Tab 6: Recommend (DSA 6) ─────────────────────────────
    void onRecommend();

    // ── Tab 7: Sorted Rooms (DSA 3 BST) ─────────────────────
    void onRefreshBST();

private:
    // ── Backend ───────────────────────────────────────────────
    RoomManager mgr;

    // ── Tab container ─────────────────────────────────────────
    QTabWidget* tabs;

    // ── Tab 1: Rooms ──────────────────────────────────────────
    QTableWidget* roomTable;

    // ── Tab 2: Bookings ───────────────────────────────────────
    QTableWidget* bookingTable;

    // ── Tab 3: Request Queue ──────────────────────────────────
    QTableWidget* queueTable;

    // ── Tab 4: History Stack ──────────────────────────────────
    QTableWidget* historyTable;

    // ── Tab 5: Timetable ──────────────────────────────────────
    QTableWidget* timetableGrid;   // 7 rows × 8 cols visual grid
    QTableWidget* slotBookingTable;// list of slot-bookings

    // ── Tab 6: Undo ───────────────────────────────────────────
    QTableWidget* undoTable;

    // ── Tab 7: Recommend ─────────────────────────────────────
    QTableWidget* recommendTable;

    // ── Tab 8: BST Sorted Rooms ──────────────────────────────
    QTableWidget* bstTable;

    // ── Refresh helpers (traverse DSA → fill tables) ──────────
    void refreshRooms();
    void refreshBookings();
    void refreshQueue();
    void refreshHistory();
    void refreshUndoStack();
    void refreshTimetable(int roomID);
    void refreshSlotBookings();
    void refreshAll();

    // ── Utility ───────────────────────────────────────────────
    QString currentTime();
};

#endif // MAINWINDOW_H
