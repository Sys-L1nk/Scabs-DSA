// ============================================================
//  SCABS — Qt Frontend (Modern UI Edition)
//  Integrates with scabs_backend.h via direct function calls.
//  cout from backend is captured and shown in the output panel.
//
//  Build:  qmake scabs.pro && make   (or open in Qt Creator)
// ============================================================

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QGroupBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QFont>
#include <QScrollBar>
#include <sstream>
#include <functional>

#include "scabs_backend.h"

// ─── Helper: redirect cout, run fn(), return captured text ──
static QString capture(std::function<void()> fn)
{
    std::ostringstream buf;
    std::streambuf *old = std::cout.rdbuf(buf.rdbuf());
    fn();
    std::cout.rdbuf(old);
    return QString::fromStdString(buf.str()).trimmed();
}

// ============================================================
//  MainWindow
// ============================================================
class MainWindow : public QMainWindow
{
    Q_OBJECT

    SCABS sys;
    QTextEdit *output;

    // ── Append text + separator to output panel ──────────────
    void print(const QString &text)
    {
        if (text.isEmpty())
            return;
        output->append(text);
        output->append(QString(45, '-'));
        // Auto-scroll to bottom
        output->verticalScrollBar()->setValue(
            output->verticalScrollBar()->maximum());
    }

    // ── Input dialog shortcuts ───────────────────────────────
    int askInt(const QString &msg, int lo, int hi, bool &ok)
    {
        return QInputDialog::getInt(this, "SCABS Input", msg, lo, lo, hi, 1, &ok);
    }
    QString askText(const QString &msg, bool &ok)
    {
        return QInputDialog::getText(this, "SCABS Input", msg,
                                     QLineEdit::Normal, "", &ok);
    }
    double askDouble(const QString &msg, bool &ok)
    {
        return QInputDialog::getDouble(this, "SCABS Input", msg,
                                       0.0, 0.0, 5.0, 1, &ok);
    }

    // ── Make a styled button ─────────────────────────────────
    QPushButton *btn(const QString &label)
    {
        QPushButton *b = new QPushButton(label);
        b->setMinimumHeight(38); // Taller, modern buttons
        b->setCursor(Qt::PointingHandCursor);
        return b;
    }

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {

        setWindowTitle("SCABS — Smart Classroom Availability & Booking System");
        setMinimumSize(950, 650);

        // ── Central widget + root layout ─────────────────────
        QWidget *root = new QWidget(this);
        QHBoxLayout *layout = new QHBoxLayout(root);
        layout->setContentsMargins(20, 20, 20, 20);
        layout->setSpacing(25);
        setCentralWidget(root);

        // ════════════════════════════════════════════════════
        //  LEFT PANEL — buttons
        // ════════════════════════════════════════════════════
        QVBoxLayout *left = new QVBoxLayout();
        left->setSpacing(15);

        // App Title/Logo Label for UI structure
        QLabel *appTitle = new QLabel("SCABS Dashboard");
        appTitle->setStyleSheet("font-size: 22px; font-weight: bold; color: #BB86FC; padding-bottom: 5px;");
        left->addWidget(appTitle);

        // ── Group 1: Manage ──────────────────────────────────
        QGroupBox *manageBox = new QGroupBox("ROOM MANAGEMENT");
        QVBoxLayout *manageLayout = new QVBoxLayout(manageBox);
        manageLayout->setSpacing(8);
        manageLayout->setContentsMargins(15, 25, 15, 15);

        QPushButton *bAddRoom = btn("✚ Add Classroom");
        QPushButton *bBook = btn("📅 Book a Room");
        QPushButton *bMarkEmpty = btn("✖ Mark Class Empty");
        QPushButton *bUndo = btn("↺ Undo Last Action");
        QPushButton *bReschedule = btn("⇌ Reschedule Class");

        for (auto *b : {bAddRoom, bBook, bMarkEmpty, bUndo, bReschedule})
            manageLayout->addWidget(b);

        // ── Group 2: Query ───────────────────────────────────
        QGroupBox *queryBox = new QGroupBox("QUERIES & SEARCH");
        QVBoxLayout *queryLayout = new QVBoxLayout(queryBox);
        queryLayout->setSpacing(8);
        queryLayout->setContentsMargins(15, 25, 15, 15);

        QPushButton *bCheckAvail = btn("🔎 Check Availability");
        QPushButton *bRoomInfo = btn("ℹ Room Info & Timetable");
        QPushButton *bFreeRooms = btn("🟢 Find Free Rooms");
        QPushButton *bRecommend = btn("⭐ Recommend Rooms");

        for (auto *b : {bCheckAvail, bRoomInfo, bFreeRooms, bRecommend})
            queryLayout->addWidget(b);

        // ── Group 3: View data structures ────────────────────
        QGroupBox *viewBox = new QGroupBox("SYSTEM VIEWS");
        QVBoxLayout *viewLayout = new QVBoxLayout(viewBox);
        viewLayout->setSpacing(8);
        viewLayout->setContentsMargins(15, 25, 15, 15);

        QPushButton *bAllRooms = btn("📋 All Rooms (List)");
        QPushButton *bSorted = btn("🗂 Sorted Rooms (BST)");
        QPushButton *bQueue = btn("⏳ Booking Queue");
        QPushButton *bStack = btn("📚 Undo Stack");

        for (auto *b : {bAllRooms, bSorted, bQueue, bStack})
            viewLayout->addWidget(b);

        // ── Clear button ─────────────────────────────────────
        QPushButton *bClear = btn("🗑 Clear Terminal");
        bClear->setObjectName("clearBtn"); // Specific ID for unique styling

        left->addWidget(manageBox);
        left->addWidget(queryBox);
        left->addWidget(viewBox);
        left->addStretch();
        left->addWidget(bClear);

        // ════════════════════════════════════════════════════
        //  RIGHT PANEL — output area
        // ════════════════════════════════════════════════════
        QVBoxLayout *right = new QVBoxLayout();
        right->setSpacing(10);

        QLabel *outLabel = new QLabel("System Terminal Output");
        outLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #A0A0A0; text-transform: uppercase; letter-spacing: 1px;");

        output = new QTextEdit();
        output->setReadOnly(true);
        output->setFont(QFont("Consolas", 11)); // Better monospace font
        output->setMinimumWidth(500);

        right->addWidget(outLabel);
        right->addWidget(output);

        layout->addLayout(left);
        layout->addLayout(right, 1);

        // ── Load default rooms on startup ────────────────────
        print(capture([this]
                      {
            sys.addClassroom("LH-1",        60,  true,  4.5f);
            sys.addClassroom("LH-2",        40,  false, 3.0f);
            sys.addClassroom("CS-Lab",      30,  true,  4.8f);
            sys.addClassroom("Seminar-Hall",100, true,  4.2f);
            sys.addClassroom("Room-204",    35,  false, 2.8f); }));
        output->append("  [SYSTEM READY] 5 default rooms loaded.\n"
                       "  Room IDs start at 101.");
        output->append(QString(45, '-'));

        // ════════════════════════════════════════════════════
        //  CONNECT BUTTONS → BACKEND CALLS
        // ════════════════════════════════════════════════════

        // Clear
        connect(bClear, &QPushButton::clicked, output, &QTextEdit::clear);

        // Add Classroom
        connect(bAddRoom, &QPushButton::clicked, this, [this]
                {
            bool ok;
            QString name  = askText("Room name:", ok);           if (!ok) return;
            int     cap   = askInt("Capacity:", 1, 999, ok);     if (!ok) return;
            int     ac    = askInt("Has AC? (1=Yes, 0=No):",
                                   0, 1, ok);                    if (!ok) return;
            double  rat   = askDouble("Rating (0.0 – 5.0):", ok);if (!ok) return;
            print(capture([this, name, cap, ac, rat] {
                sys.addClassroom(name.toStdString(), cap, (bool)ac, (float)rat);
            })); });

        // Book a Room
        connect(bBook, &QPushButton::clicked, this, [this]
                {
            bool ok;
            int roomId = askInt("Room ID:", 100, 9999, ok); if (!ok) return;
            int sid    = askInt("Student ID:", 1, 99999, ok);if (!ok) return;
            int day    = askInt("Day  (1=Mon … 7=Sun):", 1, 7, ok); if (!ok) return;
            int slot   = askInt("Slot (1=8AM … 8=3PM):", 1, 8, ok); if (!ok) return;
            print(capture([this, roomId, sid, day, slot] {
                sys.bookRoom(roomId, sid, day-1, slot-1);
            })); });

        // Mark Class Empty
        connect(bMarkEmpty, &QPushButton::clicked, this, [this]
                {
            bool ok;
            int roomId = askInt("Room ID:", 100, 9999, ok); if (!ok) return;
            int day    = askInt("Day  (1–7):", 1, 7, ok);   if (!ok) return;
            int slot   = askInt("Slot (1–8):", 1, 8, ok);   if (!ok) return;
            print(capture([this, roomId, day, slot] {
                sys.markClassEmpty(roomId, day-1, slot-1);
            })); });

        // Undo
        connect(bUndo, &QPushButton::clicked, this, [this]
                { print(capture([this]
                                { sys.undoLastAction(); })); });

        // Reschedule
        connect(bReschedule, &QPushButton::clicked, this, [this]
                {
            bool ok;
            int roomId = askInt("Room ID:", 100, 9999, ok);     if (!ok) return;
            int od     = askInt("Current Day  (1–7):", 1, 7, ok);if (!ok) return;
            int os     = askInt("Current Slot (1–8):", 1, 8, ok);if (!ok) return;
            int nd     = askInt("New Day  (1–7):", 1, 7, ok);   if (!ok) return;
            int ns     = askInt("New Slot (1–8):", 1, 8, ok);   if (!ok) return;
            print(capture([this, roomId, od, os, nd, ns] {
                sys.rescheduleClass(roomId, od-1, os-1, nd-1, ns-1);
            })); });

        // Check Availability
        connect(bCheckAvail, &QPushButton::clicked, this, [this]
                {
            bool ok;
            int roomId = askInt("Room ID:", 100, 9999, ok); if (!ok) return;
            int day    = askInt("Day  (1–7):", 1, 7, ok);   if (!ok) return;
            int slot   = askInt("Slot (1–8):", 1, 8, ok);   if (!ok) return;
            print(capture([this, roomId, day, slot] {
                sys.checkAvailability(roomId, day-1, slot-1);
            })); });

        // Room Info + Timetable
        connect(bRoomInfo, &QPushButton::clicked, this, [this]
                {
            bool ok;
            int roomId = askInt("Room ID:", 100, 9999, ok); if (!ok) return;
            print(capture([this, roomId] { sys.displayRoomInfo(roomId); })); });

        // Find Free Rooms
        connect(bFreeRooms, &QPushButton::clicked, this, [this]
                {
            bool ok;
            int day  = askInt("Day  (1–7):", 1, 7, ok); if (!ok) return;
            int slot = askInt("Slot (1–8):", 1, 8, ok); if (!ok) return;
            print(capture([this, day, slot] { sys.findFreeRooms(day-1, slot-1); })); });

        // Recommend Rooms
        connect(bRecommend, &QPushButton::clicked, this, [this]
                {
            bool ok;
            int day  = askInt("Day  (1–7):", 1, 7, ok); if (!ok) return;
            int slot = askInt("Slot (1–8):", 1, 8, ok); if (!ok) return;
            print(capture([this, day, slot] { sys.recommendRooms(day-1, slot-1); })); });

        // View — Linked List
        connect(bAllRooms, &QPushButton::clicked, this, [this]
                { print(capture([this]
                                { sys.displayAllRooms(); })); });

        // View — BST
        connect(bSorted, &QPushButton::clicked, this, [this]
                { print(capture([this]
                                { sys.displaySortedByID(); })); });

        // View — Queue
        connect(bQueue, &QPushButton::clicked, this, [this]
                { print(capture([this]
                                { sys.displayBookingQueue(); })); });

        // View — Stack
        connect(bStack, &QPushButton::clicked, this, [this]
                { print(capture([this]
                                { sys.displayUndoStack(); })); });
    }
};

// ── Include MOC output (needed when Q_OBJECT is in a .cpp) ──
#include "scabs_frontend.moc"

// ============================================================
//  main
// ============================================================
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // ════════════════════════════════════════════════════
    //  GLOBAL STYLESHEET (Dark Material Theme)
    // ════════════════════════════════════════════════════
    app.setStyleSheet(R"(
        /* Main Application Window */
        QMainWindow, QWidget {
            background-color: #121212;
            color: #E0E0E0;
            font-family: 'Segoe UI', 'Roboto', sans-serif;
        }
        
        /* Group Boxes */
        QGroupBox {
            background-color: #1E1E1E;
            border: 1px solid #333333;
            border-radius: 8px;
            margin-top: 1.5ex;
            font-weight: 600;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 10px;
            color: #03DAC6; /* Teal Accent */
            font-size: 11px;
            letter-spacing: 1px;
        }
        
        /* Push Buttons */
        QPushButton {
            background-color: #2D2D2D;
            color: #FFFFFF;
            border: 1px solid #3D3D3D;
            border-radius: 6px;
            padding: 6px 12px;
            font-size: 13px;
            font-weight: 500;
            text-align: left;
            padding-left: 15px;
        }
        QPushButton:hover {
            background-color: #3700B3; /* Deep Purple */
            border: 1px solid #6200EE;
        }
        QPushButton:pressed {
            background-color: #03DAC6;
            color: #000000;
        }
        
        /* Specific Override for the Clear Button */
        QPushButton#clearBtn {
            text-align: center;
            padding-left: 0;
            background-color: #CF6679; /* Soft Error Red */
            color: #000000;
            border: none;
            font-weight: bold;
        }
        QPushButton#clearBtn:hover {
            background-color: #FF7597;
        }
        QPushButton#clearBtn:pressed {
            background-color: #B00020;
            color: #FFFFFF;
        }
        
        /* Input Dialogs inherited styling */
        QLineEdit, QSpinBox, QDoubleSpinBox {
            background-color: #2D2D2D;
            color: #FFFFFF;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 5px;
        }
        
        /* Output Terminal (QTextEdit) */
        QTextEdit {
            background-color: #0D0D0D;
            color: #4AF626; /* Hacker Green for Terminal Output */
            border: 2px solid #2A2A2A;
            border-radius: 8px;
            padding: 12px;
            line-height: 1.5;
        }
        
        /* Custom Scrollbar for Terminal */
        QScrollBar:vertical {
            border: none;
            background-color: #121212;
            width: 12px;
            margin: 0px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background-color: #333333;
            min-height: 30px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #555555;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
            background: none;
        }
    )");

    MainWindow w;
    w.show();
    return app.exec();
}