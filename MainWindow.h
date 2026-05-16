#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QStatusBar>
#include "RoomManager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Room slots
    void onAddRoom();
    void onDeleteRoom();
    void onBookRoom();

    // Booking slots
    void onCancelBooking();

    // Queue slots
    void onJoinQueue();
    void onProcessQueue();

private:
    // The DSA backend
    RoomManager mgr;

    // Tabs
    QTabWidget*   tabs;

    // Tables (one per tab)
    QTableWidget* roomTable;
    QTableWidget* bookingTable;
    QTableWidget* queueTable;
    QTableWidget* historyTable;

    // Helper: refresh all tables from DSA structures
    void refreshRooms();
    void refreshBookings();
    void refreshQueue();
    void refreshHistory();
    void refreshAll();

    // Helper: current timestamp as QString
    QString currentTime();
};

#endif // MAINWINDOW_H
