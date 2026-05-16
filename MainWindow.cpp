#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QDateTime>
#include <QGroupBox>

// ════════════════════════════════════════════
//  CONSTRUCTOR  — build the entire window
// ════════════════════════════════════════════
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Room Management System — DSA Project");
    setMinimumSize(860, 560);

    // ── Central widget + outer layout ──
    QWidget*     central = new QWidget(this);
    QVBoxLayout* outer   = new QVBoxLayout(central);
    outer->setContentsMargins(10, 10, 10, 10);
    outer->setSpacing(8);
    setCentralWidget(central);

    // ── Title label ──
    QLabel* title = new QLabel("🏠  Room Management System", this);
    title->setStyleSheet("font-size:18px; font-weight:bold; color:#2c3e50; padding:4px 0;");
    outer->addWidget(title);

    // ── Tab widget ──
    tabs = new QTabWidget(this);
    outer->addWidget(tabs);

    // ────────────────────────────────────────
    // TAB 1 — Rooms  (Linked List)
    // ────────────────────────────────────────
    QWidget*     roomTab    = new QWidget();
    QVBoxLayout* roomLayout = new QVBoxLayout(roomTab);

    // Buttons row
    QHBoxLayout* roomBtns = new QHBoxLayout();
    QPushButton* btnAddRoom    = new QPushButton("➕ Add Room");
    QPushButton* btnDeleteRoom = new QPushButton("🗑 Delete Room");
    QPushButton* btnBookRoom   = new QPushButton("📋 Book Room");
    roomBtns->addWidget(btnAddRoom);
    roomBtns->addWidget(btnDeleteRoom);
    roomBtns->addWidget(btnBookRoom);
    roomBtns->addStretch();
    roomLayout->addLayout(roomBtns);

    // DSA label
    QLabel* lbl1 = new QLabel("DSA: Linked List  —  roomHead → Room → Room → nullptr");
    lbl1->setStyleSheet("color: #2980b9; font-style:italic; font-size:12px;");
    roomLayout->addWidget(lbl1);

    // Table
    roomTable = new QTableWidget(0, 5);
    roomTable->setHorizontalHeaderLabels({"Room ID", "Room No.", "Capacity", "AC", "Status"});
    roomTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    roomTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    roomTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    roomLayout->addWidget(roomTable);

    tabs->addTab(roomTab, "🏠 Rooms");

    // ────────────────────────────────────────
    // TAB 2 — Bookings  (Linked List)
    // ────────────────────────────────────────
    QWidget*     bookTab    = new QWidget();
    QVBoxLayout* bookLayout = new QVBoxLayout(bookTab);

    QHBoxLayout* bookBtns = new QHBoxLayout();
    QPushButton* btnCancel = new QPushButton("❌ Cancel Booking");
    bookBtns->addWidget(btnCancel);
    bookBtns->addStretch();
    bookLayout->addLayout(bookBtns);

    QLabel* lbl2 = new QLabel("DSA: Linked List  —  bookingHead → Booking → Booking → nullptr  (head insert)");
    lbl2->setStyleSheet("color: #2980b9; font-style:italic; font-size:12px;");
    bookLayout->addWidget(lbl2);

    bookingTable = new QTableWidget(0, 6);
    bookingTable->setHorizontalHeaderLabels({"Booking ID", "Room No.", "Student Name", "Student ID", "Date/Time", "Status"});
    bookingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    bookingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    bookingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bookLayout->addWidget(bookingTable);

    tabs->addTab(bookTab, "📋 Bookings");

    // ────────────────────────────────────────
    // TAB 3 — Request Queue  (Queue FIFO)
    // ────────────────────────────────────────
    QWidget*     queueTab    = new QWidget();
    QVBoxLayout* queueLayout = new QVBoxLayout(queueTab);

    QHBoxLayout* queueBtns = new QHBoxLayout();
    QPushButton* btnJoinQueue    = new QPushButton("➕ Join Queue");
    QPushButton* btnProcessQueue = new QPushButton("▶ Process Next");
    queueBtns->addWidget(btnJoinQueue);
    queueBtns->addWidget(btnProcessQueue);
    queueBtns->addStretch();
    queueLayout->addLayout(queueBtns);

    QLabel* lbl3 = new QLabel("DSA: Queue (FIFO)  —  Enqueue at REAR, Dequeue from FRONT");
    lbl3->setStyleSheet("color: #27ae60; font-style:italic; font-size:12px;");
    queueLayout->addWidget(lbl3);

    queueTable = new QTableWidget(0, 6);
    queueTable->setHorizontalHeaderLabels({"Req ID", "Student Name", "Student ID", "Needs AC", "Min Cap", "Request Time"});
    queueTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    queueTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    queueTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    queueLayout->addWidget(queueTable);

    tabs->addTab(queueTab, "⏳ Request Queue");

    // ────────────────────────────────────────
    // TAB 4 — History Stack  (Stack LIFO)
    // ────────────────────────────────────────
    QWidget*     histTab    = new QWidget();
    QVBoxLayout* histLayout = new QVBoxLayout(histTab);

    QLabel* lbl4 = new QLabel("DSA: Stack (LIFO)  —  stackTop → latest entry  (most recent shown first)");
    lbl4->setStyleSheet("color: #e67e22; font-style:italic; font-size:12px;");
    histLayout->addWidget(lbl4);

    historyTable = new QTableWidget(0, 5);
    historyTable->setHorizontalHeaderLabels({"Action", "Room No.", "Student Name", "Student ID", "Timestamp"});
    historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    histLayout->addWidget(historyTable);

    tabs->addTab(histTab, "📜 History Stack");

    // ── Status bar ──
    statusBar()->showMessage("Ready");

    // ── Connect buttons to slots ──
    connect(btnAddRoom,      &QPushButton::clicked, this, &MainWindow::onAddRoom);
    connect(btnDeleteRoom,   &QPushButton::clicked, this, &MainWindow::onDeleteRoom);
    connect(btnBookRoom,     &QPushButton::clicked, this, &MainWindow::onBookRoom);
    connect(btnCancel,       &QPushButton::clicked, this, &MainWindow::onCancelBooking);
    connect(btnJoinQueue,    &QPushButton::clicked, this, &MainWindow::onJoinQueue);
    connect(btnProcessQueue, &QPushButton::clicked, this, &MainWindow::onProcessQueue);

    // ── Initial render ──
    refreshAll();
}

MainWindow::~MainWindow() {}

// ════════════════════════════════════════════
//  HELPER — current date/time as QString
// ════════════════════════════════════════════
QString MainWindow::currentTime()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
}

// ════════════════════════════════════════════
//  REFRESH FUNCTIONS
//  These traverse the DSA structures and
//  fill the QTableWidgets row by row
// ════════════════════════════════════════════

void MainWindow::refreshRooms()
{
    roomTable->setRowCount(0); // clear

    // Traverse Linked List: roomHead → ... → nullptr
    Room* cur = mgr.roomHead;
    while (cur) {
        int row = roomTable->rowCount();
        roomTable->insertRow(row);

        roomTable->setItem(row, 0, new QTableWidgetItem(QString::number(cur->roomID)));
        roomTable->setItem(row, 1, new QTableWidgetItem(cur->roomNumber));
        roomTable->setItem(row, 2, new QTableWidgetItem(QString::number(cur->capacity)));
        roomTable->setItem(row, 3, new QTableWidgetItem(cur->isAC ? "Yes" : "No"));

        QTableWidgetItem* status = new QTableWidgetItem(cur->isAvailable ? "Available" : "Booked");
        status->setForeground(cur->isAvailable ? Qt::darkGreen : Qt::red);
        roomTable->setItem(row, 4, status);

        cur = cur->next; // move to next node
    }
}

void MainWindow::refreshBookings()
{
    bookingTable->setRowCount(0);

    // Traverse Linked List: bookingHead → ... → nullptr
    Booking* cur = mgr.bookingHead;
    while (cur) {
        int row = bookingTable->rowCount();
        bookingTable->insertRow(row);

        bookingTable->setItem(row, 0, new QTableWidgetItem(QString::number(cur->bookingID)));
        bookingTable->setItem(row, 1, new QTableWidgetItem(cur->roomNumber));
        bookingTable->setItem(row, 2, new QTableWidgetItem(cur->studentName));
        bookingTable->setItem(row, 3, new QTableWidgetItem(cur->studentID));
        bookingTable->setItem(row, 4, new QTableWidgetItem(cur->dateTime));

        QTableWidgetItem* status = new QTableWidgetItem(cur->isActive ? "Active" : "Cancelled");
        status->setForeground(cur->isActive ? Qt::darkGreen : Qt::red);
        bookingTable->setItem(row, 5, status);

        cur = cur->next;
    }
}

void MainWindow::refreshQueue()
{
    queueTable->setRowCount(0);

    // Traverse Queue: queueFront → ... → queueRear → nullptr
    Request* cur = mgr.queueFront;
    while (cur) {
        int row = queueTable->rowCount();
        queueTable->insertRow(row);

        queueTable->setItem(row, 0, new QTableWidgetItem(QString::number(cur->requestID)));
        queueTable->setItem(row, 1, new QTableWidgetItem(cur->studentName));
        queueTable->setItem(row, 2, new QTableWidgetItem(cur->studentID));
        queueTable->setItem(row, 3, new QTableWidgetItem(cur->needsAC ? "Yes" : "No"));
        queueTable->setItem(row, 4, new QTableWidgetItem(QString::number(cur->minCapacity)));
        queueTable->setItem(row, 5, new QTableWidgetItem(cur->requestTime));

        cur = cur->next;
    }
}

void MainWindow::refreshHistory()
{
    historyTable->setRowCount(0);

    // Traverse Stack: stackTop → ... → nullptr  (LIFO: top is newest)
    HistoryEntry* cur = mgr.stackTop;
    while (cur) {
        int row = historyTable->rowCount();
        historyTable->insertRow(row);

        QTableWidgetItem* action = new QTableWidgetItem(cur->action);
        if      (cur->action == "BOOK")        action->setForeground(Qt::darkGreen);
        else if (cur->action == "CANCEL")      action->setForeground(Qt::red);
        else if (cur->action == "ADD_ROOM")    action->setForeground(Qt::darkBlue);
        else if (cur->action == "DELETE_ROOM") action->setForeground(Qt::darkRed);
        historyTable->setItem(row, 0, action);

        historyTable->setItem(row, 1, new QTableWidgetItem(cur->roomNumber));
        historyTable->setItem(row, 2, new QTableWidgetItem(cur->studentName));
        historyTable->setItem(row, 3, new QTableWidgetItem(cur->studentID));
        historyTable->setItem(row, 4, new QTableWidgetItem(cur->timestamp));

        cur = cur->next;
    }
}

void MainWindow::refreshAll()
{
    refreshRooms();
    refreshBookings();
    refreshQueue();
    refreshHistory();

    // Update status bar
    statusBar()->showMessage(
        QString("Rooms: %1  |  Bookings: %2  |  Queue: %3  |  History entries: %4")
            .arg(mgr.roomCount)
            .arg(mgr.bookingCount)
            .arg(mgr.queueSize)
            .arg("see stack")
    );
}

// ════════════════════════════════════════════
//  SLOT: Add Room
// ════════════════════════════════════════════
void MainWindow::onAddRoom()
{
    // Simple input dialog
    QDialog dlg(this);
    dlg.setWindowTitle("Add Room");
    QFormLayout form(&dlg);

    QSpinBox*  idInput  = new QSpinBox();  idInput->setRange(1, 9999);
    QLineEdit* numInput = new QLineEdit();  numInput->setPlaceholderText("e.g. D-401");
    QSpinBox*  capInput = new QSpinBox();  capInput->setRange(1, 500);
    QComboBox* acInput  = new QComboBox(); acInput->addItems({"Yes", "No"});

    form.addRow("Room ID:",    idInput);
    form.addRow("Room No.:",   numInput);
    form.addRow("Capacity:",   capInput);
    form.addRow("AC:",         acInput);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&btns);
    connect(&btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    int     id  = idInput->value();
    QString num = numInput->text().trimmed();
    int     cap = capInput->value();
    bool    ac  = (acInput->currentIndex() == 0);

    if (num.isEmpty()) { QMessageBox::warning(this, "Error", "Room number is required."); return; }
    if (mgr.findRoom(id)) { QMessageBox::warning(this, "Error", "Room ID already exists."); return; }

    mgr.addRoom(id, num, cap, ac);
    mgr.pushHistory("ADD_ROOM", 0, id, num, "-", "-", currentTime());
    refreshAll();
}

// ════════════════════════════════════════════
//  SLOT: Delete Room
// ════════════════════════════════════════════
void MainWindow::onDeleteRoom()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Delete Room");
    QFormLayout form(&dlg);

    QSpinBox* idInput = new QSpinBox(); idInput->setRange(1, 9999);
    form.addRow("Room ID:", idInput);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form.addRow(&btns);
    connect(&btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    int id = idInput->value();
    Room* r = mgr.findRoom(id);
    if (!r) { QMessageBox::warning(this, "Error", "Room not found."); return; }

    mgr.pushHistory("DELETE_ROOM", 0, id, r->roomNumber, "-", "-", currentTime());
    mgr.deleteRoom(id);
    refreshAll();
}

// ════════════════════════════════════════════
//  SLOT: Book Room
// ════════════════════════════════════════════
void MainWindow::onBookRoom()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Book a Room");
    QFormLayout form(&dlg);

    // Show only available rooms in a dropdown
    QComboBox* roomCombo = new QComboBox();
    Room* cur = mgr.roomHead;
    while (cur) {
        if (cur->isAvailable)
            roomCombo->addItem(
                QString("%1 — %2 (Cap:%3, AC:%4)")
                    .arg(cur->roomID).arg(cur->roomNumber)
                    .arg(cur->capacity).arg(cur->isAC ? "Yes" : "No"),
                cur->roomID   // store roomID as data
            );
        cur = cur->next;
    }

    if (roomCombo->count() == 0) {
        QMessageBox::information(this, "No Rooms", "No rooms available right now. Join the queue instead.");
        return;
    }

    QLineEdit* nameInput = new QLineEdit(); nameInput->setPlaceholderText("e.g. Ali Hassan");
    QLineEdit* sidInput  = new QLineEdit(); sidInput->setPlaceholderText("e.g. F2021-123");

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
        QMessageBox::warning(this, "Error", "Please fill all fields.");
        return;
    }

    int bid = mgr.bookRoom(roomID, name, sid, currentTime());
    if (bid == -1) { QMessageBox::warning(this, "Error", "Booking failed."); return; }

    Room* r = mgr.findRoom(roomID);
    mgr.pushHistory("BOOK", bid, roomID, r ? r->roomNumber : "-", name, sid, currentTime());

    QMessageBox::information(this, "Success", QString("Booking confirmed!\nBooking ID: %1").arg(bid));
    refreshAll();
}

// ════════════════════════════════════════════
//  SLOT: Cancel Booking
// ════════════════════════════════════════════
void MainWindow::onCancelBooking()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Cancel Booking");
    QFormLayout form(&dlg);

    // Show only active bookings
    QComboBox* combo = new QComboBox();
    Booking* cur = mgr.bookingHead;
    while (cur) {
        if (cur->isActive)
            combo->addItem(
                QString("#%1 — %2 (%3)").arg(cur->bookingID).arg(cur->roomNumber).arg(cur->studentName),
                cur->bookingID
            );
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

    if (b) mgr.pushHistory("CANCEL", bid, b->roomID, b->roomNumber, b->studentName, b->studentID, currentTime());

    mgr.cancelBooking(bid);

    // Auto-process queue if someone is waiting
    if (mgr.queueSize > 0) mgr.processNextRequest(currentTime());

    refreshAll();
}

// ════════════════════════════════════════════
//  SLOT: Join Queue
// ════════════════════════════════════════════
void MainWindow::onJoinQueue()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Join Request Queue");
    QFormLayout form(&dlg);

    QLineEdit* nameInput = new QLineEdit(); nameInput->setPlaceholderText("e.g. Sara Khan");
    QLineEdit* sidInput  = new QLineEdit(); sidInput->setPlaceholderText("e.g. F2022-456");
    QComboBox* acInput   = new QComboBox(); acInput->addItems({"Yes", "No"});
    QSpinBox*  capInput  = new QSpinBox();  capInput->setRange(1, 500);

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
        QMessageBox::warning(this, "Error", "Please fill all fields.");
        return;
    }

    mgr.enqueue(name, sid, ac, cap, currentTime());
    QMessageBox::information(this, "Queued", QString("%1 added to the queue.\nPosition: %2").arg(name).arg(mgr.queueSize));
    refreshAll();
}

// ════════════════════════════════════════════
//  SLOT: Process Next in Queue
// ════════════════════════════════════════════
void MainWindow::onProcessQueue()
{
    if (mgr.queueSize == 0) {
        QMessageBox::information(this, "Queue Empty", "No requests in the queue.");
        return;
    }

    bool ok = mgr.processNextRequest(currentTime());

    if (ok)
        QMessageBox::information(this, "Success", "Request processed — room auto-booked from queue.");
    else
        QMessageBox::warning(this, "No Match", "No available room matches the front request's requirements.");

    refreshAll();
}
