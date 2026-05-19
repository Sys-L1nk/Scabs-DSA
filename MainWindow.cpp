// ══════════════════════════════════════════════════════════════
//  MainWindow.cpp  — SCABS Qt GUI  (frontend only)
//  All data-structure logic is in RoomManager.cpp.
//  This file: builds widgets, drives dialogs, calls mgr.xxx(),
//  traverses DSA structures to fill QTableWidgets.
// ══════════════════════════════════════════════════════════════
#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QDateTime>
#include <QGroupBox>
#include <QScrollArea>
#include <QFont>

// ══════════════════════════════════════════════════════════════
//  Colour palette (consistent across all tabs)
// ══════════════════════════════════════════════════════════════
static const QString COL_BLUE   = "#2980b9";
static const QString COL_GREEN  = "#27ae60";
static const QString COL_ORANGE = "#e67e22";
static const QString COL_RED    = "#c0392b";
static const QString COL_PURPLE = "#8e44ad";
static const QString COL_TEAL   = "#16a085";

// ──────────────────────────────────────────────────────────────
//  Small factory: styled DSA description label
// ──────────────────────────────────────────────────────────────
static QLabel* dsaLabel(const QString& text, const QString& colour)
{
    QLabel* l = new QLabel(text);
    l->setStyleSheet(QString("color:%1; font-style:italic; font-size:12px;").arg(colour));
    return l;
}

// ══════════════════════════════════════════════════════════════
//  CONSTRUCTOR
// ══════════════════════════════════════════════════════════════
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("SCABS — Smart Classroom Availability & Booking System");
    setMinimumSize(1000, 620);

    // ── Central widget ────────────────────────────────────────
    QWidget*     central = new QWidget(this);
    QVBoxLayout* outer   = new QVBoxLayout(central);
    outer->setContentsMargins(10, 8, 10, 8);
    outer->setSpacing(6);
    setCentralWidget(central);

    // ── Title ─────────────────────────────────────────────────
    QLabel* title = new QLabel("🏫  SCABS — Smart Classroom Availability & Booking System");
    title->setStyleSheet("font-size:17px; font-weight:bold; color:#2c3e50; padding:4px 0;");
    outer->addWidget(title);

    // ── Tab widget ────────────────────────────────────────────
    tabs = new QTabWidget(this);
    outer->addWidget(tabs);

    // ══════════════════════════════════════════════════════════
    //  TAB 1 — Rooms  (DSA 1 Linked List + DSA 2 Hash Table)
    // ══════════════════════════════════════════════════════════
    {
        QWidget*     w  = new QWidget();
        QVBoxLayout* vl = new QVBoxLayout(w);

        QHBoxLayout* row = new QHBoxLayout();
        QPushButton* btnAdd  = new QPushButton("➕ Add Room");
        QPushButton* btnDel  = new QPushButton("🗑 Delete Room");
        QPushButton* btnBook = new QPushButton("📋 Book Room");
        row->addWidget(btnAdd); row->addWidget(btnDel);
        row->addWidget(btnBook); row->addStretch();
        vl->addLayout(row);

        vl->addWidget(dsaLabel(
            "DSA 1: Linked List  (roomHead → Room → … → nullptr)  |  "
            "DSA 2: Hash Table O(1) lookup  |  Rating & Category fields",
            COL_BLUE));

        roomTable = new QTableWidget(0, 7);
        roomTable->setHorizontalHeaderLabels(
            {"Room ID","Room No.","Capacity","AC","Rating","Category","Status"});
        roomTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        roomTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        roomTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        roomTable->setAlternatingRowColors(true);
        vl->addWidget(roomTable);

        tabs->addTab(w, "🏠 Rooms");

        connect(btnAdd,  &QPushButton::clicked, this, &MainWindow::onAddRoom);
        connect(btnDel,  &QPushButton::clicked, this, &MainWindow::onDeleteRoom);
        connect(btnBook, &QPushButton::clicked, this, &MainWindow::onBookRoom);
    }

    // ══════════════════════════════════════════════════════════
    //  TAB 2 — Bookings  (DSA 1 Linked List, head insert)
    // ══════════════════════════════════════════════════════════
    {
        QWidget*     w  = new QWidget();
        QVBoxLayout* vl = new QVBoxLayout(w);

        QHBoxLayout* row = new QHBoxLayout();
        QPushButton* btnCancel = new QPushButton("❌ Cancel Booking");
        row->addWidget(btnCancel); row->addStretch();
        vl->addLayout(row);

        vl->addWidget(dsaLabel(
            "DSA 1: Booking Linked List  (bookingHead → newest → … → nullptr, head-insert)",
            COL_BLUE));

        bookingTable = new QTableWidget(0, 7);
        bookingTable->setHorizontalHeaderLabels(
            {"Booking ID","Room No.","Student Name","Student ID","Date/Time","Day","Slot","Status"});
        // 8 columns
        bookingTable->setColumnCount(8);
        bookingTable->setHorizontalHeaderLabels(
            {"Booking ID","Room No.","Student Name","Student ID",
             "Date/Time","Day","Slot","Status"});
        bookingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        bookingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        bookingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        bookingTable->setAlternatingRowColors(true);
        vl->addWidget(bookingTable);

        tabs->addTab(w, "📋 Bookings");

        connect(btnCancel, &QPushButton::clicked, this, &MainWindow::onCancelBooking);
    }

    // ══════════════════════════════════════════════════════════
    //  TAB 3 — Request Queue  (DSA 4 FIFO Queue)
    // ══════════════════════════════════════════════════════════
    {
        QWidget*     w  = new QWidget();
        QVBoxLayout* vl = new QVBoxLayout(w);

        QHBoxLayout* row = new QHBoxLayout();
        QPushButton* btnJoin    = new QPushButton("➕ Join Queue");
        QPushButton* btnProcess = new QPushButton("▶ Process Next");
        row->addWidget(btnJoin); row->addWidget(btnProcess); row->addStretch();
        vl->addLayout(row);

        vl->addWidget(dsaLabel(
            "DSA 4: Queue (FIFO)  —  Enqueue at REAR, Dequeue from FRONT",
            COL_GREEN));

        queueTable = new QTableWidget(0, 6);
        queueTable->setHorizontalHeaderLabels(
            {"Req ID","Student Name","Student ID","Needs AC","Min Cap","Request Time"});
        queueTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        queueTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        queueTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        queueTable->setAlternatingRowColors(true);
        vl->addWidget(queueTable);

        tabs->addTab(w, "⏳ Request Queue");

        connect(btnJoin,    &QPushButton::clicked, this, &MainWindow::onJoinQueue);
        connect(btnProcess, &QPushButton::clicked, this, &MainWindow::onProcessQueue);
    }

    // ══════════════════════════════════════════════════════════
    //  TAB 4 — History Stack  (Linked-list LIFO, unbounded)
    // ══════════════════════════════════════════════════════════
    {
        QWidget*     w  = new QWidget();
        QVBoxLayout* vl = new QVBoxLayout(w);

        vl->addWidget(dsaLabel(
            "History Stack (LIFO, linked-list)  —  stackTop → newest → … → nullptr",
            COL_ORANGE));

        historyTable = new QTableWidget(0, 6);
        historyTable->setHorizontalHeaderLabels(
            {"Action","Room No.","Student Name","Student ID","Booking ID","Timestamp"});
        historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        historyTable->setAlternatingRowColors(true);
        vl->addWidget(historyTable);

        tabs->addTab(w, "📜 History");
    }

    // ══════════════════════════════════════════════════════════
    //  TAB 5 — Timetable  (DSA 7: 2-D Array)
    // ══════════════════════════════════════════════════════════
    {
        QWidget*     w  = new QWidget();
        QVBoxLayout* vl = new QVBoxLayout(w);

        // Buttons row
        QHBoxLayout* row = new QHBoxLayout();
        QPushButton* btnBookSlot  = new QPushButton("📌 Book Slot");
        QPushButton* btnMarkEmpty = new QPushButton("🔓 Mark Empty");
        QPushButton* btnReschedule= new QPushButton("🔄 Reschedule");
        QPushButton* btnShowTT    = new QPushButton("👁 Show Timetable");
        row->addWidget(btnBookSlot); row->addWidget(btnMarkEmpty);
        row->addWidget(btnReschedule); row->addWidget(btnShowTT);
        row->addStretch();
        vl->addLayout(row);

        vl->addWidget(dsaLabel(
            "DSA 7: 2-D Array  timetable[7 days][8 slots]  (false=free, true=booked)",
            COL_TEAL));

        // Visual 7×8 grid
        QGroupBox* gridBox = new QGroupBox("Weekly Timetable Grid  (select a room above)");
        QVBoxLayout* gbL   = new QVBoxLayout(gridBox);
        timetableGrid = new QTableWidget(DAYS, SLOTS);
        QStringList dayHdr, slotHdr;
        for (int d = 0; d < DAYS;  ++d) dayHdr  << RoomManager::DAY_NAMES[d];
        for (int s = 0; s < SLOTS; ++s) slotHdr << RoomManager::SLOT_NAMES[s];
        timetableGrid->setVerticalHeaderLabels(dayHdr);
        timetableGrid->setHorizontalHeaderLabels(slotHdr);
        timetableGrid->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        timetableGrid->setEditTriggers(QAbstractItemView::NoEditTriggers);
        timetableGrid->setFixedHeight(230);
        gbL->addWidget(timetableGrid);
        vl->addWidget(gridBox);

        // Slot-booking list
        vl->addWidget(dsaLabel("Slot Bookings (from Booking linked list, day/slot ≥ 0):", COL_TEAL));
        slotBookingTable = new QTableWidget(0, 6);
        slotBookingTable->setHorizontalHeaderLabels(
            {"Booking ID","Room No.","Student Name","Student ID","Day","Slot"});
        slotBookingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        slotBookingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        slotBookingTable->setAlternatingRowColors(true);
        vl->addWidget(slotBookingTable);

        tabs->addTab(w, "📅 Timetable");

        connect(btnBookSlot,   &QPushButton::clicked, this, &MainWindow::onBookSlot);
        connect(btnMarkEmpty,  &QPushButton::clicked, this, &MainWindow::onMarkEmpty);
        connect(btnReschedule, &QPushButton::clicked, this, &MainWindow::onReschedule);
        connect(btnShowTT,     &QPushButton::clicked, this, &MainWindow::onShowTimetable);
    }

    // ══════════════════════════════════════════════════════════
    //  TAB 6 — Undo Stack  (DSA 5: array-based)
    // ══════════════════════════════════════════════════════════
    {
        QWidget*     w  = new QWidget();
        QVBoxLayout* vl = new QVBoxLayout(w);

        QHBoxLayout* row = new QHBoxLayout();
        QPushButton* btnUndo    = new QPushButton("↩ Undo Last Action");
        QPushButton* btnRefresh = new QPushButton("🔄 Refresh");
        row->addWidget(btnUndo); row->addWidget(btnRefresh); row->addStretch();
        vl->addLayout(row);

        vl->addWidget(dsaLabel(
            QString("DSA 5: Undo Stack (array-based, capacity %1)  — top = most recent").arg(MAX_UNDO),
            COL_RED));

        undoTable = new QTableWidget(0, 5);
        undoTable->setHorizontalHeaderLabels(
            {"#","Type","Room ID","Day","Slot"});
        undoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        undoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        undoTable->setAlternatingRowColors(true);
        vl->addWidget(undoTable);

        tabs->addTab(w, "↩ Undo Stack");

        connect(btnUndo,    &QPushButton::clicked, this, &MainWindow::onUndo);
        connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::refreshUndoStack);
    }

    // ══════════════════════════════════════════════════════════
    //  TAB 7 — Recommend  (DSA 6: Priority Queue)
    // ══════════════════════════════════════════════════════════
    {
        QWidget*     w  = new QWidget();
        QVBoxLayout* vl = new QVBoxLayout(w);

        QHBoxLayout* row = new QHBoxLayout();
        QPushButton* btnRec = new QPushButton("⭐ Get Recommendations");
        row->addWidget(btnRec); row->addStretch();
        vl->addLayout(row);

        vl->addWidget(dsaLabel(
            "DSA 6: Priority Queue (sorted linked list)  —  score = rating + AC bonus + category bonus",
            COL_PURPLE));

        recommendTable = new QTableWidget(0, 6);
        recommendTable->setHorizontalHeaderLabels(
            {"Rank","Room ID","Room No.","Capacity","AC","Score"});
        recommendTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        recommendTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        recommendTable->setAlternatingRowColors(true);
        vl->addWidget(recommendTable);

        tabs->addTab(w, "⭐ Recommend");

        connect(btnRec, &QPushButton::clicked, this, &MainWindow::onRecommend);
    }

    // ══════════════════════════════════════════════════════════
    //  TAB 8 — BST Sorted Rooms  (DSA 3)
    // ══════════════════════════════════════════════════════════
    {
        QWidget*     w  = new QWidget();
        QVBoxLayout* vl = new QVBoxLayout(w);

        QHBoxLayout* row = new QHBoxLayout();
        QPushButton* btnBST = new QPushButton("🌳 Refresh (BST Inorder)");
        row->addWidget(btnBST); row->addStretch();
        vl->addLayout(row);

        vl->addWidget(dsaLabel(
            "DSA 3: Binary Search Tree (by Room ID)  —  Inorder traversal → ascending sorted list",
            COL_ORANGE));

        bstTable = new QTableWidget(0, 5);
        bstTable->setHorizontalHeaderLabels(
            {"Sorted #","Room ID","Room No.","Capacity","Rating"});
        bstTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        bstTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        bstTable->setAlternatingRowColors(true);
        vl->addWidget(bstTable);

        tabs->addTab(w, "🌳 BST Rooms");

        connect(btnBST, &QPushButton::clicked, this, &MainWindow::onRefreshBST);
    }

    // ── Status bar ────────────────────────────────────────────
    statusBar()->showMessage("Ready");

    // ── Initial render ────────────────────────────────────────
    refreshAll();
}

MainWindow::~MainWindow() {}

// ══════════════════════════════════════════════════════════════
//  UTILITY
// ══════════════════════════════════════════════════════════════
QString MainWindow::currentTime()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
}

// ══════════════════════════════════════════════════════════════
//  REFRESH FUNCTIONS — traverse DSA structures → fill tables
// ══════════════════════════════════════════════════════════════

void MainWindow::refreshRooms()
{
    roomTable->setRowCount(0);
    Room* cur = mgr.roomHead; // DSA 1 traversal
    while (cur) {
        int r = roomTable->rowCount();
        roomTable->insertRow(r);
        roomTable->setItem(r, 0, new QTableWidgetItem(QString::number(cur->roomID)));
        roomTable->setItem(r, 1, new QTableWidgetItem(cur->roomNumber));
        roomTable->setItem(r, 2, new QTableWidgetItem(QString::number(cur->capacity)));
        roomTable->setItem(r, 3, new QTableWidgetItem(cur->isAC ? "Yes" : "No"));
        roomTable->setItem(r, 4, new QTableWidgetItem(QString::number(cur->rating, 'f', 1)));

        QTableWidgetItem* cat = new QTableWidgetItem(cur->category);
        if      (cur->category == "Good")    cat->setForeground(Qt::darkGreen);
        else if (cur->category == "Average") cat->setForeground(QColor("#e67e22"));
        else                                  cat->setForeground(Qt::red);
        roomTable->setItem(r, 5, cat);

        QTableWidgetItem* st = new QTableWidgetItem(cur->isAvailable ? "Available" : "Booked");
        st->setForeground(cur->isAvailable ? Qt::darkGreen : Qt::red);
        roomTable->setItem(r, 6, st);

        cur = cur->next; // next Linked List node
    }
}

void MainWindow::refreshBookings()
{
    bookingTable->setRowCount(0);
    Booking* cur = mgr.bookingHead; // DSA 1 traversal (head = newest)
    while (cur) {
        int r = bookingTable->rowCount();
        bookingTable->insertRow(r);
        bookingTable->setItem(r, 0, new QTableWidgetItem(QString::number(cur->bookingID)));
        bookingTable->setItem(r, 1, new QTableWidgetItem(cur->roomNumber));
        bookingTable->setItem(r, 2, new QTableWidgetItem(cur->studentName));
        bookingTable->setItem(r, 3, new QTableWidgetItem(cur->studentID));
        bookingTable->setItem(r, 4, new QTableWidgetItem(cur->dateTime));
        bookingTable->setItem(r, 5, new QTableWidgetItem(
            cur->day  >= 0 ? QString(RoomManager::DAY_NAMES[cur->day])  : "-"));
        bookingTable->setItem(r, 6, new QTableWidgetItem(
            cur->slot >= 0 ? QString(RoomManager::SLOT_NAMES[cur->slot]): "-"));

        QTableWidgetItem* st = new QTableWidgetItem(cur->isActive ? "Active" : "Cancelled");
        st->setForeground(cur->isActive ? Qt::darkGreen : Qt::red);
        bookingTable->setItem(r, 7, st);

        cur = cur->next;
    }
}

void MainWindow::refreshQueue()
{
    queueTable->setRowCount(0);
    Request* cur = mgr.queueFront; // DSA 4: front → rear
    while (cur) {
        int r = queueTable->rowCount();
        queueTable->insertRow(r);
        queueTable->setItem(r, 0, new QTableWidgetItem(QString::number(cur->requestID)));
        queueTable->setItem(r, 1, new QTableWidgetItem(cur->studentName));
        queueTable->setItem(r, 2, new QTableWidgetItem(cur->studentID));
        queueTable->setItem(r, 3, new QTableWidgetItem(cur->needsAC ? "Yes" : "No"));
        queueTable->setItem(r, 4, new QTableWidgetItem(QString::number(cur->minCapacity)));
        queueTable->setItem(r, 5, new QTableWidgetItem(cur->requestTime));
        cur = cur->next;
    }
}

void MainWindow::refreshHistory()
{
    historyTable->setRowCount(0);
    HistoryEntry* cur = mgr.stackTop; // LIFO: top = newest
    while (cur) {
        int r = historyTable->rowCount();
        historyTable->insertRow(r);

        QTableWidgetItem* act = new QTableWidgetItem(cur->action);
        if      (cur->action == "BOOK"       || cur->action == "BOOK_SLOT") act->setForeground(Qt::darkGreen);
        else if (cur->action == "CANCEL")                                    act->setForeground(Qt::red);
        else if (cur->action == "ADD_ROOM")                                  act->setForeground(Qt::darkBlue);
        else if (cur->action == "DELETE_ROOM")                               act->setForeground(Qt::darkRed);
        else if (cur->action == "QUEUE_BOOK")                                act->setForeground(QColor("#16a085"));
        historyTable->setItem(r, 0, act);

        historyTable->setItem(r, 1, new QTableWidgetItem(cur->roomNumber));
        historyTable->setItem(r, 2, new QTableWidgetItem(cur->studentName));
        historyTable->setItem(r, 3, new QTableWidgetItem(cur->studentID));
        historyTable->setItem(r, 4, new QTableWidgetItem(
            cur->bookingID > 0 ? QString::number(cur->bookingID) : "-"));
        historyTable->setItem(r, 5, new QTableWidgetItem(cur->timestamp));

        cur = cur->next;
    }
}

void MainWindow::refreshUndoStack()
{
    undoTable->setRowCount(0);
    // DSA 5: read array from top downwards (top = most recent)
    for (int i = mgr.undoTop; i >= 0; --i) {
        const UndoAction& a = mgr.undoData[i];
        int r = undoTable->rowCount();
        undoTable->insertRow(r);
        undoTable->setItem(r, 0, new QTableWidgetItem(QString::number(mgr.undoTop - i + 1)));
        undoTable->setItem(r, 1, new QTableWidgetItem(a.type));
        undoTable->setItem(r, 2, new QTableWidgetItem(QString::number(a.roomID)));
        undoTable->setItem(r, 3, new QTableWidgetItem(
            a.day  >= 0 ? QString(RoomManager::DAY_NAMES[a.day])  : "-"));
        undoTable->setItem(r, 4, new QTableWidgetItem(
            a.slot >= 0 ? QString(RoomManager::SLOT_NAMES[a.slot]): "-"));
    }
}

void MainWindow::refreshTimetable(int roomID)
{
    // Clear grid
    for (int d = 0; d < DAYS; ++d)
        for (int s = 0; s < SLOTS; ++s)
            timetableGrid->setItem(d, s, new QTableWidgetItem(""));

    Room* room = mgr.findRoom(roomID);
    if (!room) return;

    // Fill DSA 7 — 2D array
    for (int d = 0; d < DAYS; ++d) {
        for (int s = 0; s < SLOTS; ++s) {
            bool booked = room->timetable[d][s];
            QTableWidgetItem* cell = new QTableWidgetItem(booked ? "■ Booked" : "○ Free");
            cell->setTextAlignment(Qt::AlignCenter);
            if (booked) {
                cell->setForeground(Qt::white);
                cell->setBackground(QColor("#c0392b"));
            } else {
                cell->setForeground(Qt::darkGreen);
                cell->setBackground(QColor("#eafaf1"));
            }
            timetableGrid->setItem(d, s, cell);
        }
    }
}

void MainWindow::refreshSlotBookings()
{
    slotBookingTable->setRowCount(0);
    Booking* cur = mgr.bookingHead;
    while (cur) {
        if (cur->isActive && cur->day >= 0 && cur->slot >= 0) {
            int r = slotBookingTable->rowCount();
            slotBookingTable->insertRow(r);
            slotBookingTable->setItem(r, 0, new QTableWidgetItem(QString::number(cur->bookingID)));
            slotBookingTable->setItem(r, 1, new QTableWidgetItem(cur->roomNumber));
            slotBookingTable->setItem(r, 2, new QTableWidgetItem(cur->studentName));
            slotBookingTable->setItem(r, 3, new QTableWidgetItem(cur->studentID));
            slotBookingTable->setItem(r, 4, new QTableWidgetItem(
                QString(RoomManager::DAY_NAMES[cur->day])));
            slotBookingTable->setItem(r, 5, new QTableWidgetItem(
                QString(RoomManager::SLOT_NAMES[cur->slot])));
        }
        cur = cur->next;
    }
}

void MainWindow::refreshAll()
{
    refreshRooms();
    refreshBookings();
    refreshQueue();
    refreshHistory();
    refreshUndoStack();
    refreshSlotBookings();

    statusBar()->showMessage(
        QString("Rooms: %1  |  Bookings: %2  |  Queue: %3  |  Undo depth: %4")
            .arg(mgr.roomCount)
            .arg(mgr.bookingCount)
            .arg(mgr.queueSize)
            .arg(mgr.undoTop + 1));
}

// ══════════════════════════════════════════════════════════════
//  SLOT — Add Room  (DSA 1 + 2 + 3)
// ══════════════════════════════════════════════════════════════
void MainWindow::onAddRoom()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Add Classroom");
    QFormLayout form(&dlg);

    QSpinBox*       idInput  = new QSpinBox();  idInput->setRange(1, 9999); idInput->setValue(mgr.nextRoomID);
    QLineEdit*      numInput = new QLineEdit();  numInput->setPlaceholderText("e.g. LH-3");
    QSpinBox*       capInput = new QSpinBox();  capInput->setRange(1, 500); capInput->setValue(30);
    QComboBox*      acInput  = new QComboBox(); acInput->addItems({"Yes", "No"});
    QDoubleSpinBox* ratInput = new QDoubleSpinBox();
    ratInput->setRange(0.0, 5.0); ratInput->setSingleStep(0.1); ratInput->setValue(3.0);

    form.addRow("Room ID:",    idInput);
    form.addRow("Room Name/No.:", numInput);
    form.addRow("Capacity:",   capInput);
    form.addRow("Has AC:",     acInput);
    form.addRow("Rating (0-5):", ratInput);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&btns);
    connect(&btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    int     id  = idInput->value();
    QString num = numInput->text().trimmed();
    int     cap = capInput->value();
    bool    ac  = (acInput->currentIndex() == 0);
    float   rat = static_cast<float>(ratInput->value());

    if (num.isEmpty()) { QMessageBox::warning(this, "Error", "Room name is required."); return; }
    if (mgr.findRoom(id)) { QMessageBox::warning(this, "Error", "Room ID already exists."); return; }

    mgr.addRoom(id, num, cap, ac, rat);
    mgr.pushHistory("ADD_ROOM", 0, id, num, "-", "-", currentTime());
    refreshAll();
    statusBar()->showMessage(QString("Room %1 (%2) added.").arg(id).arg(num));
}

// ══════════════════════════════════════════════════════════════
//  SLOT — Delete Room  (DSA 1 + 2 + 3)
// ══════════════════════════════════════════════════════════════
void MainWindow::onDeleteRoom()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Delete Room");
    QFormLayout form(&dlg);

    QComboBox* combo = new QComboBox();
    Room* cur = mgr.roomHead;
    while (cur) {
        combo->addItem(
            QString("[%1] %2").arg(cur->roomID).arg(cur->roomNumber),
            cur->roomID);
        cur = cur->next;
    }
    if (combo->count() == 0) {
        QMessageBox::information(this, "Empty", "No rooms to delete.");
        return;
    }
    form.addRow("Room:", combo);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&btns);
    connect(&btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    int   id = combo->currentData().toInt();
    Room* r  = mgr.findRoom(id); // O(1) via DSA 2 Hash Table
    if (!r) { QMessageBox::warning(this, "Error", "Room not found."); return; }

    mgr.pushHistory("DELETE_ROOM", 0, id, r->roomNumber, "-", "-", currentTime());
    mgr.deleteRoom(id);
    refreshAll();
    statusBar()->showMessage(QString("Room %1 deleted.").arg(id));
}

// ══════════════════════════════════════════════════════════════
//  SLOT — Book Room (general / availability-flag booking)
// ══════════════════════════════════════════════════════════════
void MainWindow::onBookRoom()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Book a Room");
    QFormLayout form(&dlg);

    QComboBox* roomCombo = new QComboBox();
    Room* cur = mgr.roomHead;
    while (cur) {
        if (cur->isAvailable)
            roomCombo->addItem(
                QString("[%1] %2  Cap:%3  AC:%4  Rating:%5  %6")
                    .arg(cur->roomID).arg(cur->roomNumber)
                    .arg(cur->capacity).arg(cur->isAC ? "Yes":"No")
                    .arg(cur->rating, 0, 'f', 1).arg(cur->category),
                cur->roomID);
        cur = cur->next;
    }
    if (roomCombo->count() == 0) {
        QMessageBox::information(this, "No Rooms",
            "No rooms available. Join the request queue instead.");
        return;
    }

    QLineEdit* nameInput = new QLineEdit(); nameInput->setPlaceholderText("Student full name");
    QLineEdit* sidInput  = new QLineEdit(); sidInput->setPlaceholderText("e.g. F2022-001");

    form.addRow("Room:",         roomCombo);
    form.addRow("Student Name:", nameInput);
    form.addRow("Student ID:",   sidInput);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&btns);
    connect(&btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    int     roomID = roomCombo->currentData().toInt();
    QString name   = nameInput->text().trimmed();
    QString sid    = sidInput->text().trimmed();

    if (name.isEmpty() || sid.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all fields."); return;
    }

    int bid = mgr.bookRoom(roomID, name, sid, currentTime());
    if (bid == -1) { QMessageBox::warning(this, "Error", "Booking failed."); return; }

    Room* r = mgr.findRoom(roomID);
    mgr.pushHistory("BOOK", bid, roomID, r ? r->roomNumber : "-", name, sid, currentTime());

    QMessageBox::information(this, "Booked",
        QString("Room booked!\nBooking ID: %1").arg(bid));
    refreshAll();
}

// ══════════════════════════════════════════════════════════════
//  SLOT — Cancel Booking
// ══════════════════════════════════════════════════════════════
void MainWindow::onCancelBooking()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Cancel Booking");
    QFormLayout form(&dlg);

    QComboBox* combo = new QComboBox();
    Booking* cur = mgr.bookingHead;
    while (cur) {
        if (cur->isActive)
            combo->addItem(
                QString("#%1 — %2 (%3)").arg(cur->bookingID)
                    .arg(cur->roomNumber).arg(cur->studentName),
                cur->bookingID);
        cur = cur->next;
    }
    if (combo->count() == 0) {
        QMessageBox::information(this, "No Bookings", "No active bookings to cancel.");
        return;
    }
    form.addRow("Select Booking:", combo);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&btns);
    connect(&btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    int bid = combo->currentData().toInt();

    // Find booking details before cancelling (for history)
    Booking* b = mgr.bookingHead;
    while (b && b->bookingID != bid) b = b->next;
    if (b) mgr.pushHistory("CANCEL", bid, b->roomID,
                            b->roomNumber, b->studentName,
                            b->studentID, currentTime());

    mgr.cancelBooking(bid);

    // Auto-process waiting queue if anyone is waiting
    if (mgr.queueSize > 0) mgr.processNextRequest(currentTime());

    refreshAll();
    statusBar()->showMessage(QString("Booking %1 cancelled.").arg(bid));
}

// ══════════════════════════════════════════════════════════════
//  SLOT — Join Request Queue  (DSA 4)
// ══════════════════════════════════════════════════════════════
void MainWindow::onJoinQueue()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Join Request Queue");
    QFormLayout form(&dlg);

    QLineEdit* nameInput = new QLineEdit(); nameInput->setPlaceholderText("Student full name");
    QLineEdit* sidInput  = new QLineEdit(); sidInput->setPlaceholderText("e.g. F2022-456");
    QComboBox* acInput   = new QComboBox(); acInput->addItems({"Yes", "No"});
    QSpinBox*  capInput  = new QSpinBox();  capInput->setRange(1, 500); capInput->setValue(30);

    form.addRow("Student Name:", nameInput);
    form.addRow("Student ID:",   sidInput);
    form.addRow("Needs AC:",     acInput);
    form.addRow("Min Capacity:", capInput);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&btns);
    connect(&btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    QString name = nameInput->text().trimmed();
    QString sid  = sidInput->text().trimmed();
    bool    ac   = (acInput->currentIndex() == 0);
    int     cap  = capInput->value();

    if (name.isEmpty() || sid.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all fields."); return;
    }

    mgr.enqueue(name, sid, ac, cap, currentTime());
    QMessageBox::information(this, "Queued",
        QString("%1 added to queue.\nCurrent position: %2").arg(name).arg(mgr.queueSize));
    refreshAll();
}

// ══════════════════════════════════════════════════════════════
//  SLOT — Process Next Request  (DSA 4)
// ══════════════════════════════════════════════════════════════
void MainWindow::onProcessQueue()
{
    if (mgr.queueSize == 0) {
        QMessageBox::information(this, "Queue Empty", "No pending requests.");
        return;
    }
    bool ok = mgr.processNextRequest(currentTime());
    if (ok)
        QMessageBox::information(this, "Success", "Request processed — room auto-booked.");
    else
        QMessageBox::warning(this, "No Match",
            "No available room matches the front request's requirements.");
    refreshAll();
}

// ══════════════════════════════════════════════════════════════
//  SLOT — Book Slot  (DSA 7: 2-D array)
// ══════════════════════════════════════════════════════════════
void MainWindow::onBookSlot()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Book Timetable Slot");
    QFormLayout form(&dlg);

    QComboBox* roomCombo = new QComboBox();
    Room* cur = mgr.roomHead;
    while (cur) {
        roomCombo->addItem(
            QString("[%1] %2").arg(cur->roomID).arg(cur->roomNumber),
            cur->roomID);
        cur = cur->next;
    }

    QComboBox* dayCombo  = new QComboBox();
    QComboBox* slotCombo = new QComboBox();
    for (int d = 0; d < DAYS;  ++d) dayCombo->addItem(RoomManager::DAY_NAMES[d]);
    for (int s = 0; s < SLOTS; ++s) slotCombo->addItem(RoomManager::SLOT_NAMES[s]);

    QLineEdit* nameInput = new QLineEdit(); nameInput->setPlaceholderText("Student name");
    QLineEdit* sidInput  = new QLineEdit(); sidInput->setPlaceholderText("Student ID");

    form.addRow("Room:",         roomCombo);
    form.addRow("Day:",          dayCombo);
    form.addRow("Slot:",         slotCombo);
    form.addRow("Student Name:", nameInput);
    form.addRow("Student ID:",   sidInput);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&btns);
    connect(&btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    int     roomID = roomCombo->currentData().toInt();
    int     day    = dayCombo->currentIndex();
    int     slot   = slotCombo->currentIndex();
    QString name   = nameInput->text().trimmed();
    QString sid    = sidInput->text().trimmed();

    if (name.isEmpty() || sid.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all fields."); return;
    }

    bool ok = mgr.bookSlot(roomID, 0, day, slot, name, sid, currentTime());
    if (!ok) {
        QMessageBox::warning(this, "Slot Taken",
            "That slot is already booked or room not found."); return;
    }

    Room* r = mgr.findRoom(roomID);
    mgr.pushHistory("BOOK_SLOT", mgr.nextBookingID - 1, roomID,
                    r ? r->roomNumber : "-", name, sid, currentTime());

    QMessageBox::information(this, "Slot Booked",
        QString("Slot booked: %1  %2  %3")
            .arg(RoomManager::DAY_NAMES[day])
            .arg(RoomManager::SLOT_NAMES[slot])
            .arg(r ? r->roomNumber : ""));

    refreshTimetable(roomID);
    refreshAll();
}

// ══════════════════════════════════════════════════════════════
//  SLOT — Mark Slot Empty  (DSA 7 + DSA 5 undo)
// ══════════════════════════════════════════════════════════════
void MainWindow::onMarkEmpty()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Mark Slot as Empty");
    QFormLayout form(&dlg);

    QComboBox* roomCombo = new QComboBox();
    Room* cur = mgr.roomHead;
    while (cur) {
        roomCombo->addItem(
            QString("[%1] %2").arg(cur->roomID).arg(cur->roomNumber),
            cur->roomID);
        cur = cur->next;
    }

    QComboBox* dayCombo  = new QComboBox();
    QComboBox* slotCombo = new QComboBox();
    for (int d = 0; d < DAYS;  ++d) dayCombo->addItem(RoomManager::DAY_NAMES[d]);
    for (int s = 0; s < SLOTS; ++s) slotCombo->addItem(RoomManager::SLOT_NAMES[s]);

    form.addRow("Room:", roomCombo);
    form.addRow("Day:",  dayCombo);
    form.addRow("Slot:", slotCombo);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&btns);
    connect(&btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    int roomID = roomCombo->currentData().toInt();
    int day    = dayCombo->currentIndex();
    int slot   = slotCombo->currentIndex();

    bool ok = mgr.markSlotEmpty(roomID, day, slot);
    if (!ok)
        QMessageBox::warning(this, "Error",
            "Slot is already free, or room not found.");
    else {
        Room* r = mgr.findRoom(roomID);
        mgr.pushHistory("MARK_EMPTY", -1, roomID,
                        r ? r->roomNumber : "-", "-", "-", currentTime());
        QMessageBox::information(this, "Done", "Slot marked as empty.");
        refreshTimetable(roomID);
        refreshAll();
    }
}

// ══════════════════════════════════════════════════════════════
//  SLOT — Reschedule Slot  (DSA 7)
// ══════════════════════════════════════════════════════════════
void MainWindow::onReschedule()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Reschedule Class");
    QFormLayout form(&dlg);

    QComboBox* roomCombo = new QComboBox();
    Room* cur = mgr.roomHead;
    while (cur) {
        roomCombo->addItem(
            QString("[%1] %2").arg(cur->roomID).arg(cur->roomNumber),
            cur->roomID);
        cur = cur->next;
    }

    QComboBox *odCombo = new QComboBox(), *osCombo = new QComboBox();
    QComboBox *ndCombo = new QComboBox(), *nsComb = new QComboBox();
    for (int d = 0; d < DAYS;  ++d) { odCombo->addItem(RoomManager::DAY_NAMES[d]);
                                       ndCombo->addItem(RoomManager::DAY_NAMES[d]); }
    for (int s = 0; s < SLOTS; ++s) { osCombo->addItem(RoomManager::SLOT_NAMES[s]);
                                       nsComb->addItem(RoomManager::SLOT_NAMES[s]); }

    form.addRow("Room:",     roomCombo);
    form.addRow("Old Day:",  odCombo);
    form.addRow("Old Slot:", osCombo);
    form.addRow("New Day:",  ndCombo);
    form.addRow("New Slot:", nsComb);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&btns);
    connect(&btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    int roomID = roomCombo->currentData().toInt();
    bool ok = mgr.rescheduleSlot(roomID,
                                  odCombo->currentIndex(), osCombo->currentIndex(),
                                  ndCombo->currentIndex(), nsComb->currentIndex());
    if (!ok)
        QMessageBox::warning(this, "Error",
            "Old slot not booked, new slot occupied, or invalid inputs.");
    else {
        Room* r = mgr.findRoom(roomID);
        mgr.pushHistory("RESCHEDULE", -1, roomID,
                        r ? r->roomNumber : "-", "-", "-", currentTime());
        QMessageBox::information(this, "Rescheduled", "Class rescheduled successfully.");
        refreshTimetable(roomID);
        refreshAll();
    }
}

// ══════════════════════════════════════════════════════════════
//  SLOT — Show Timetable for a room  (DSA 7 visualisation)
// ══════════════════════════════════════════════════════════════
void MainWindow::onShowTimetable()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Select Room");
    QFormLayout form(&dlg);

    QComboBox* roomCombo = new QComboBox();
    Room* cur = mgr.roomHead;
    while (cur) {
        roomCombo->addItem(
            QString("[%1] %2").arg(cur->roomID).arg(cur->roomNumber),
            cur->roomID);
        cur = cur->next;
    }
    form.addRow("Room:", roomCombo);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&btns);
    connect(&btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    int roomID = roomCombo->currentData().toInt();
    Room* r    = mgr.findRoom(roomID);
    if (!r) return;

    refreshTimetable(roomID);
    tabs->setCurrentIndex(4); // switch to Timetable tab

    statusBar()->showMessage(
        QString("Timetable for Room [%1] %2  (■=Booked  ○=Free)")
            .arg(roomID).arg(r->roomNumber));
}

// ══════════════════════════════════════════════════════════════
//  SLOT — Undo Last Action  (DSA 5)
// ══════════════════════════════════════════════════════════════
void MainWindow::onUndo()
{
    if (mgr.isUndoEmpty()) {
        QMessageBox::information(this, "Undo", "Nothing to undo.");
        return;
    }
    bool ok = mgr.undoLastAction();
    if (ok) {
        QMessageBox::information(this, "Undo", "Last action undone.");
        refreshAll();
    } else {
        QMessageBox::warning(this, "Undo", "Undo failed — room may no longer exist.");
    }
}

// ══════════════════════════════════════════════════════════════
//  SLOT — Recommend Rooms  (DSA 6: Priority Queue)
// ══════════════════════════════════════════════════════════════
void MainWindow::onRecommend()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Recommend Rooms");
    QFormLayout form(&dlg);

    QComboBox* dayCombo  = new QComboBox();
    QComboBox* slotCombo = new QComboBox();
    dayCombo->addItem("(All — ignore time)");
    slotCombo->addItem("(All — ignore time)");
    for (int d = 0; d < DAYS;  ++d) dayCombo->addItem(RoomManager::DAY_NAMES[d]);
    for (int s = 0; s < SLOTS; ++s) slotCombo->addItem(RoomManager::SLOT_NAMES[s]);

    form.addRow("Day:",  dayCombo);
    form.addRow("Slot:", slotCombo);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&btns);
    connect(&btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    // index 0 = "All", index 1..7 = actual day (0-6)
    int day  = dayCombo->currentIndex()  - 1; // -1 = all
    int slot = slotCombo->currentIndex() - 1;

    Room*  outRooms[100];
    float  outScores[100];
    int    count = mgr.recommendRooms(day, slot, outRooms, outScores, 100);

    recommendTable->setRowCount(0);
    for (int i = 0; i < count; ++i) {
        Room* r = outRooms[i];
        int   row = recommendTable->rowCount();
        recommendTable->insertRow(row);
        recommendTable->setItem(row, 0, new QTableWidgetItem(QString::number(i + 1)));
        recommendTable->setItem(row, 1, new QTableWidgetItem(QString::number(r->roomID)));
        recommendTable->setItem(row, 2, new QTableWidgetItem(r->roomNumber));
        recommendTable->setItem(row, 3, new QTableWidgetItem(QString::number(r->capacity)));
        recommendTable->setItem(row, 4, new QTableWidgetItem(r->isAC ? "Yes" : "No"));
        recommendTable->setItem(row, 5, new QTableWidgetItem(
            QString::number(outScores[i], 'f', 2)));
    }

    tabs->setCurrentIndex(6); // switch to Recommend tab
    statusBar()->showMessage(
        QString("Priority Queue: %1 room(s) ranked for %2 %3")
            .arg(count)
            .arg(day  >= 0 ? RoomManager::DAY_NAMES[day]   : "any day")
            .arg(slot >= 0 ? RoomManager::SLOT_NAMES[slot] : "any slot"));
}

// ══════════════════════════════════════════════════════════════
//  SLOT — BST Inorder (DSA 3)
// ══════════════════════════════════════════════════════════════
void MainWindow::onRefreshBST()
{
    bstTable->setRowCount(0);

    if (mgr.roomCount == 0) return;

    Room** sorted = new Room*[mgr.roomCount];
    int    idx    = 0;
    mgr.bstInorder(sorted, idx); // DSA 3 inorder traversal

    for (int i = 0; i < idx; ++i) {
        Room* r = sorted[i];
        int   row = bstTable->rowCount();
        bstTable->insertRow(row);
        bstTable->setItem(row, 0, new QTableWidgetItem(QString::number(i + 1)));
        bstTable->setItem(row, 1, new QTableWidgetItem(QString::number(r->roomID)));
        bstTable->setItem(row, 2, new QTableWidgetItem(r->roomNumber));
        bstTable->setItem(row, 3, new QTableWidgetItem(QString::number(r->capacity)));
        bstTable->setItem(row, 4, new QTableWidgetItem(
            QString::number(r->rating, 'f', 1)));
    }
    delete[] sorted;

    statusBar()->showMessage(
        QString("BST Inorder: %1 rooms displayed in ascending Room ID order").arg(idx));
}
