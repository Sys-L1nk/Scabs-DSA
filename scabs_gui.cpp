#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QFrame>
#include <QScrollArea>
#include <QGroupBox>
#include <QStatusBar>
#include <QFont>
#include <QColor>
#include <QPalette>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QString>
#include <QStringList>
#include <vector>

#include "scabs_backend.h"

// ==========================================
// Color Palette & Styling
// ==========================================
const QString STYLE_SHEET = R"(
QMainWindow, QWidget {
    background-color: #0f1117;
    color: #e8eaf0;
    font-family: 'Segoe UI', sans-serif;
}
QTabWidget::pane {
    border: 1px solid #2a2d3e;
    background: #13151f;
    border-radius: 8px;
}
QTabBar::tab {
    background: #1a1d2e;
    color: #8b8fa8;
    padding: 10px 22px;
    border: none;
    font-size: 13px;
    font-weight: 500;
    min-width: 130px;
}
QTabBar::tab:selected {
    background: #2563eb;
    color: #ffffff;
    border-radius: 0px;
}
QTabBar::tab:hover:!selected {
    background: #1e2235;
    color: #c8cadb;
}
QPushButton {
    background-color: #2563eb;
    color: white;
    border: none;
    border-radius: 6px;
    padding: 9px 20px;
    font-size: 13px;
    font-weight: 600;
    min-height: 36px;
}
QPushButton:hover {
    background-color: #3b76f0;
}
QPushButton:pressed {
    background-color: #1d51c8;
}
QPushButton#dangerBtn {
    background-color: #dc2626;
}
QPushButton#dangerBtn:hover {
    background-color: #ef4444;
}
QPushButton#warningBtn {
    background-color: #d97706;
}
QPushButton#warningBtn:hover {
    background-color: #f59e0b;
}
QPushButton#successBtn {
    background-color: #16a34a;
}
QPushButton#successBtn:hover {
    background-color: #22c55e;
}
QPushButton#undoBtn {
    background-color: #7c3aed;
}
QPushButton#undoBtn:hover {
    background-color: #8b5cf6;
}
QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox {
    background-color: #1a1d2e;
    border: 1px solid #2a2d3e;
    border-radius: 6px;
    padding: 8px 12px;
    color: #e8eaf0;
    font-size: 13px;
    min-height: 34px;
}
QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus {
    border: 1px solid #2563eb;
}
QComboBox::drop-down {
    border: none;
    width: 24px;
}
QComboBox::down-arrow {
    image: none;
    border-left: 5px solid transparent;
    border-right: 5px solid transparent;
    border-top: 5px solid #8b8fa8;
}
QComboBox QAbstractItemView {
    background: #1a1d2e;
    border: 1px solid #2a2d3e;
    color: #e8eaf0;
    selection-background-color: #2563eb;
}
QTableWidget {
    background-color: #13151f;
    gridline-color: #2a2d3e;
    color: #e8eaf0;
    border: none;
    font-size: 12px;
}
QTableWidget::item {
    padding: 6px 10px;
    border: none;
}
QTableWidget::item:selected {
    background-color: #2563eb;
    color: white;
}
QHeaderView::section {
    background-color: #1a1d2e;
    color: #8b8fa8;
    padding: 8px 10px;
    border: none;
    border-right: 1px solid #2a2d3e;
    font-size: 12px;
    font-weight: 600;
    text-transform: uppercase;
    letter-spacing: 0.5px;
}
QGroupBox {
    border: 1px solid #2a2d3e;
    border-radius: 8px;
    margin-top: 16px;
    padding: 10px;
    font-size: 13px;
    font-weight: 600;
    color: #8b8fa8;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 12px;
    padding: 0 6px;
    color: #8b8fa8;
}
QScrollArea {
    border: none;
    background: transparent;
}
QScrollBar:vertical {
    background: #1a1d2e;
    width: 8px;
    border-radius: 4px;
}
QScrollBar::handle:vertical {
    background: #2a2d3e;
    border-radius: 4px;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QStatusBar {
    background: #0a0c14;
    color: #8b8fa8;
    font-size: 12px;
    border-top: 1px solid #2a2d3e;
    padding: 4px 10px;
}
QLabel#titleLabel {
    font-size: 22px;
    font-weight: 700;
    color: #ffffff;
    letter-spacing: -0.5px;
}
QLabel#subtitleLabel {
    font-size: 13px;
    color: #8b8fa8;
}
QLabel#sectionLabel {
    font-size: 13px;
    font-weight: 600;
    color: #c8cadb;
}
QFrame#separator {
    background: #2a2d3e;
    max-height: 1px;
}
QFrame#card {
    background: #1a1d2e;
    border: 1px solid #2a2d3e;
    border-radius: 10px;
}
QFrame#statusCard {
    border-radius: 8px;
    padding: 10px;
}
QCheckBox {
    color: #e8eaf0;
    font-size: 13px;
    spacing: 8px;
}
QCheckBox::indicator {
    width: 18px;
    height: 18px;
    border-radius: 4px;
    border: 1px solid #2a2d3e;
    background: #1a1d2e;
}
QCheckBox::indicator:checked {
    background: #2563eb;
    border-color: #2563eb;
}
)";

// ==========================================
// Helper: Day and Slot combos
// ==========================================
static QComboBox* makeDayCombo() {
    QComboBox* cb = new QComboBox;
    cb->addItems({"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"});
    return cb;
}

static QComboBox* makeSlotCombo() {
    QComboBox* cb = new QComboBox;
    cb->addItems({"08:30", "10:00", "11:30", "13:00", "14:30", "16:00", "17:30"});
    return cb;
}

static QLabel* makeLabel(const QString& text, const QString& objName = "") {
    QLabel* l = new QLabel(text);
    if (!objName.isEmpty()) l->setObjectName(objName);
    return l;
}

static QFrame* makeSeparator() {
    QFrame* f = new QFrame;
    f->setObjectName("separator");
    f->setFrameShape(QFrame::HLine);
    return f;
}

static QFrame* makeCard() {
    QFrame* f = new QFrame;
    f->setObjectName("card");
    return f;
}

// ==========================================
// Status Message Widget
// ==========================================
class StatusMessage : public QLabel {
public:
    StatusMessage(QWidget* parent = nullptr) : QLabel(parent) {
        setWordWrap(true);
        setMinimumHeight(42);
        setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        hide();
    }

    void showMsg(const QString& msg) {
        show();
        if (msg.startsWith("SUCCESS") || msg.startsWith("REPORTED") || msg.startsWith("UNDO")) {
            setStyleSheet("background:#052e16; color:#4ade80; border:1px solid #166534; border-radius:6px; padding:8px 14px; font-size:13px;");
        } else if (msg.startsWith("FAILED") || msg.startsWith("ERROR")) {
            setStyleSheet("background:#450a0a; color:#f87171; border:1px solid #991b1b; border-radius:6px; padding:8px 14px; font-size:13px;");
        } else if (msg.startsWith("NOTICE")) {
            setStyleSheet("background:#1c1917; color:#fbbf24; border:1px solid #92400e; border-radius:6px; padding:8px 14px; font-size:13px;");
        } else {
            setStyleSheet("background:#172554; color:#60a5fa; border:1px solid #1e40af; border-radius:6px; padding:8px 14px; font-size:13px;");
        }
        setText(msg);
    }
};

// ==========================================
// Main Window
// ==========================================
class SCABSWindow : public QMainWindow {
    Q_OBJECT
    SCABS backend;

public:
    SCABSWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("SCABS — Smart Classroom Availability & Booking System");
        setMinimumSize(1050, 720);
        resize(1150, 780);

        QWidget* central = new QWidget;
        QVBoxLayout* mainLayout = new QVBoxLayout(central);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        // Header
        QWidget* header = buildHeader();
        mainLayout->addWidget(header);

        // Tabs
        QTabWidget* tabs = new QTabWidget;
        tabs->setDocumentMode(true);
        tabs->addTab(buildLoadTab(),       "📂  Load Timetable");
        tabs->addTab(buildRoomInfoTab(),   "🏫  Room Info");
        tabs->addTab(buildCheckTab(),      "✅  Check Availability");
        tabs->addTab(buildRecommendTab(),  "⭐  Recommendations");
        tabs->addTab(buildBookTab(),       "📌  Book Room");
        tabs->addTab(buildCancelTab(),     "❌  Cancel Booking");
        tabs->addTab(buildReportTab(),     "🔔  Report Empty");
        tabs->addTab(buildUndoTab(),       "↩  Undo");

        QWidget* tabContainer = new QWidget;
        QVBoxLayout* tcl = new QVBoxLayout(tabContainer);
        tcl->setContentsMargins(16, 12, 16, 16);
        tcl->addWidget(tabs);
        mainLayout->addWidget(tabContainer);

        setCentralWidget(central);
        statusBar()->showMessage("SCABS ready. Load a timetable CSV to begin.");
    }

private:
    // ---- Header ----
    QWidget* buildHeader() {
        QWidget* h = new QWidget;
        h->setStyleSheet("background:#0a0c14; border-bottom:1px solid #2a2d3e;");
        h->setFixedHeight(72);
        QHBoxLayout* hl = new QHBoxLayout(h);
        hl->setContentsMargins(24, 0, 24, 0);

        QLabel* icon = new QLabel("🏛");
        icon->setStyleSheet("font-size:28px;");

        QVBoxLayout* titleBlock = new QVBoxLayout;
        titleBlock->setSpacing(2);
        QLabel* title = makeLabel("SCABS", "titleLabel");
        QLabel* sub = makeLabel("Smart Classroom Availability & Booking System", "subtitleLabel");
        titleBlock->addWidget(title);
        titleBlock->addWidget(sub);

        hl->addWidget(icon);
        hl->addSpacing(10);
        hl->addLayout(titleBlock);
        hl->addStretch();

        QLabel* badge = new QLabel("v1.0");
        badge->setStyleSheet("background:#1a1d2e; color:#8b8fa8; border:1px solid #2a2d3e; border-radius:12px; padding:4px 12px; font-size:12px;");
        hl->addWidget(badge);
        return h;
    }

    // ---- Tab: Load Timetable ----
    QWidget* buildLoadTab() {
        QWidget* w = new QWidget;
        QVBoxLayout* l = new QVBoxLayout(w);
        l->setContentsMargins(20, 20, 20, 20);
        l->setSpacing(14);

        l->addWidget(makeLabel("Load Timetable from CSV", "sectionLabel"));
        l->addWidget(makeSeparator());

        QFrame* card = makeCard();
        QVBoxLayout* cl = new QVBoxLayout(card);
        cl->setContentsMargins(20, 20, 20, 20);
        cl->setSpacing(12);

        cl->addWidget(new QLabel("Select your timetable CSV file to load all room and schedule data into the system."));

        QHBoxLayout* fileRow = new QHBoxLayout;
        QLineEdit* pathEdit = new QLineEdit;
        pathEdit->setPlaceholderText("CSV file path...");
        pathEdit->setReadOnly(true);
        QPushButton* browseBtn = new QPushButton("Browse...");
        browseBtn->setFixedWidth(110);
        fileRow->addWidget(pathEdit);
        fileRow->addWidget(browseBtn);
        cl->addLayout(fileRow);

        QPushButton* loadBtn = new QPushButton("Load Timetable");
        loadBtn->setObjectName("successBtn");
        loadBtn->setFixedWidth(160);
        cl->addWidget(loadBtn);

        StatusMessage* status = new StatusMessage;
        cl->addWidget(status);

        l->addWidget(card);

        // Info box
        QFrame* info = makeCard();
        info->setStyleSheet("QFrame#card { background:#0f172a; border:1px solid #1e3a5f; }");
        QVBoxLayout* il = new QVBoxLayout(info);
        il->setContentsMargins(16, 14, 16, 14);
        QLabel* infoLabel = new QLabel(
            "<b style='color:#60a5fa'>CSV Format Expected:</b><br>"
            "<span style='color:#94a3b8; font-family:monospace;'>Program, Semester, Day, StartTime, EndTime, Course, RoomID</span><br><br>"
            "<span style='color:#94a3b8'>Days: Mo, Tu, We, Th, Fr &nbsp;|&nbsp; Times: 08:30, 10:00, 11:30, 13:00, 14:30, 16:00, 17:30</span>"
        );
        infoLabel->setTextFormat(Qt::RichText);
        infoLabel->setWordWrap(true);
        il->addWidget(infoLabel);
        l->addWidget(info);

        l->addStretch();

        // Connections
        connect(browseBtn, &QPushButton::clicked, [=]() {
            QString file = QFileDialog::getOpenFileName(this, "Select Timetable CSV", "", "CSV Files (*.csv);;All Files (*)");
            if (!file.isEmpty()) pathEdit->setText(file);
        });
        connect(loadBtn, &QPushButton::clicked, [=]() {
            QString path = pathEdit->text().trimmed();
            if (path.isEmpty()) {
                status->showMsg("ERROR: Please select a CSV file first.");
                return;
            }
            string result = backend.loadTimetableFromCSV(path.toStdString());
            status->showMsg(QString::fromStdString(result));
            statusBar()->showMessage(QString::fromStdString(result));
        });

        return w;
    }

    // ---- Tab: Room Info ----
    QWidget* buildRoomInfoTab() {
        QWidget* w = new QWidget;
        QVBoxLayout* l = new QVBoxLayout(w);
        l->setContentsMargins(20, 20, 20, 20);
        l->setSpacing(14);

        l->addWidget(makeLabel("Room Info & Schedule", "sectionLabel"));
        l->addWidget(makeSeparator());

        // Input row
        QHBoxLayout* row = new QHBoxLayout;
        QLineEdit* roomEdit = new QLineEdit;
        roomEdit->setPlaceholderText("Enter Room ID (e.g., A-6, SE Lab-B)");
        QPushButton* viewBtn = new QPushButton("View Schedule");
        viewBtn->setFixedWidth(140);
        row->addWidget(roomEdit);
        row->addWidget(viewBtn);
        l->addLayout(row);

        // Room meta card
        QFrame* metaCard = makeCard();
        metaCard->hide();
        QHBoxLayout* metaL = new QHBoxLayout(metaCard);
        metaL->setContentsMargins(16, 14, 16, 14);
        QLabel* roomMeta = new QLabel;
        roomMeta->setTextFormat(Qt::RichText);
        metaL->addWidget(roomMeta);
        l->addWidget(metaCard);

        // Timetable table
        QTableWidget* table = new QTableWidget;
        QStringList vHeaders = {"08:30", "10:00", "11:30", "13:00", "14:30", "16:00", "17:30"};
        QStringList hHeaders = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
        table->setRowCount(7);
        table->setColumnCount(5);
        table->setVerticalHeaderLabels(vHeaders);
        table->setHorizontalHeaderLabels(hHeaders);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        l->addWidget(table);

        StatusMessage* status = new StatusMessage;
        l->addWidget(status);

        connect(viewBtn, &QPushButton::clicked, [=]() {
            QString rID = roomEdit->text().trimmed();
            if (rID.isEmpty()) { status->showMsg("ERROR: Please enter a Room ID."); return; }
            Room* r = backend.getRoom(rID.toStdString());
            if (!r) { status->showMsg("ERROR: Room '" + rID + "' not found. Load timetable first."); metaCard->hide(); return; }

            metaCard->show();
            QString catColor = (QString::fromStdString(r->category) == "Good") ? "#4ade80" : "#f87171";
            roomMeta->setText(
                "<b style='color:#ffffff;font-size:15px;'>" + rID + "</b> &nbsp;"
                "<span style='background:" + catColor + ";color:#000;border-radius:4px;padding:2px 8px;font-size:11px;'>" +
                QString::fromStdString(r->category) + "</span>"
                "&nbsp;&nbsp; Capacity: <b>" + QString::number(r->capacity) + "</b>"
                " &nbsp;|&nbsp; AC: <b>" + (r->hasAC ? "Yes" : "No") + "</b>"
                " &nbsp;|&nbsp; Rating: <b>" + QString::number(r->rating, 'f', 1) + "</b>"
            );

            string days[] = {"Mon","Tue","Wed","Thu","Fri"};
            for (int d = 0; d < DAYS; d++) {
                for (int s = 0; s < 7; s++) {
                    QTableWidgetItem* item = new QTableWidgetItem;
                    if (r->timetable[d][s]) {
                        item->setText(QString::fromStdString(r->courseDetails[d][s]));
                        item->setBackground(QColor("#172554"));
                        item->setForeground(QColor("#93c5fd"));
                    } else {
                        item->setText("Free");
                        item->setBackground(QColor("#052e16"));
                        item->setForeground(QColor("#4ade80"));
                    }
                    item->setTextAlignment(Qt::AlignCenter);
                    table->setItem(s, d, item);
                }
            }
            status->hide();
        });

        return w;
    }

    // ---- Tab: Check Availability ----
    QWidget* buildCheckTab() {
        QWidget* w = new QWidget;
        QVBoxLayout* l = new QVBoxLayout(w);
        l->setContentsMargins(20, 20, 20, 20);
        l->setSpacing(14);

        l->addWidget(makeLabel("Check Room Availability", "sectionLabel"));
        l->addWidget(makeSeparator());

        QFrame* card = makeCard();
        QGridLayout* gl = new QGridLayout(card);
        gl->setContentsMargins(20, 20, 20, 20);
        gl->setSpacing(12);

        QLineEdit* roomEdit = new QLineEdit;
        roomEdit->setPlaceholderText("Room ID (e.g., A-6)");
        QComboBox* dayCombo = makeDayCombo();
        QComboBox* slotCombo = makeSlotCombo();
        QPushButton* checkBtn = new QPushButton("Check Availability");
        checkBtn->setObjectName("successBtn");

        gl->addWidget(makeLabel("Room ID:"), 0, 0);
        gl->addWidget(roomEdit, 0, 1);
        gl->addWidget(makeLabel("Day:"), 1, 0);
        gl->addWidget(dayCombo, 1, 1);
        gl->addWidget(makeLabel("Time Slot:"), 2, 0);
        gl->addWidget(slotCombo, 2, 1);
        gl->addWidget(checkBtn, 3, 0, 1, 2);
        l->addWidget(card);

        // Result widget
        QFrame* resultCard = new QFrame;
        resultCard->setObjectName("statusCard");
        resultCard->hide();
        QVBoxLayout* rl = new QVBoxLayout(resultCard);
        QLabel* resultIcon = new QLabel;
        resultIcon->setStyleSheet("font-size:40px;");
        resultIcon->setAlignment(Qt::AlignCenter);
        QLabel* resultText = new QLabel;
        resultText->setStyleSheet("font-size:16px; font-weight:600;");
        resultText->setAlignment(Qt::AlignCenter);
        QLabel* resultSub = new QLabel;
        resultSub->setStyleSheet("font-size:13px; color:#8b8fa8;");
        resultSub->setAlignment(Qt::AlignCenter);
        rl->addWidget(resultIcon);
        rl->addWidget(resultText);
        rl->addWidget(resultSub);
        l->addWidget(resultCard);
        l->addStretch();

        connect(checkBtn, &QPushButton::clicked, [=]() {
            QString rID = roomEdit->text().trimmed();
            if (rID.isEmpty()) return;
            int day = dayCombo->currentIndex();
            int slot = slotCombo->currentIndex();
            bool avail = backend.checkAvailability(rID.toStdString(), day, slot);
            Room* r = backend.getRoom(rID.toStdString());
            resultCard->show();
            if (!r) {
                resultCard->setStyleSheet("QFrame#statusCard { background:#1c0a0a; border:1px solid #991b1b; border-radius:10px; padding:20px; }");
                resultIcon->setText("❓");
                resultText->setText("Room Not Found");
                resultText->setStyleSheet("font-size:16px; font-weight:600; color:#f87171;");
                resultSub->setText("Room ID '" + rID + "' does not exist in the system.");
                return;
            }
            if (avail) {
                resultCard->setStyleSheet("QFrame#statusCard { background:#052e16; border:1px solid #166534; border-radius:10px; padding:20px; }");
                resultIcon->setText("✅");
                resultText->setText("AVAILABLE");
                resultText->setStyleSheet("font-size:16px; font-weight:600; color:#4ade80;");
                resultSub->setText("Room " + rID + " is free on " + dayCombo->currentText() + " at " + slotCombo->currentText());
            } else {
                resultCard->setStyleSheet("QFrame#statusCard { background:#450a0a; border:1px solid #991b1b; border-radius:10px; padding:20px; }");
                resultIcon->setText("🔴");
                resultText->setText("OCCUPIED");
                resultText->setStyleSheet("font-size:16px; font-weight:600; color:#f87171;");
                resultSub->setText("Room " + rID + " is booked on " + dayCombo->currentText() + " at " + slotCombo->currentText());
            }
        });

        return w;
    }

    // ---- Tab: Recommendations ----
    QWidget* buildRecommendTab() {
        QWidget* w = new QWidget;
        QVBoxLayout* l = new QVBoxLayout(w);
        l->setContentsMargins(20, 20, 20, 20);
        l->setSpacing(14);

        l->addWidget(makeLabel("Room Recommendations", "sectionLabel"));
        l->addWidget(makeSeparator());

        QFrame* card = makeCard();
        QGridLayout* gl = new QGridLayout(card);
        gl->setContentsMargins(20, 20, 20, 20);
        gl->setSpacing(12);

        QComboBox* dayCombo = makeDayCombo();
        QComboBox* slotCombo = makeSlotCombo();
        QPushButton* recBtn = new QPushButton("Find Available Rooms");
        recBtn->setObjectName("successBtn");

        gl->addWidget(makeLabel("Day:"), 0, 0);
        gl->addWidget(dayCombo, 0, 1);
        gl->addWidget(makeLabel("Time Slot:"), 1, 0);
        gl->addWidget(slotCombo, 1, 1);
        gl->addWidget(recBtn, 2, 0, 1, 2);
        l->addWidget(card);

        QTableWidget* table = new QTableWidget;
        table->setColumnCount(5);
        table->setHorizontalHeaderLabels({"Rank", "Room ID", "Category", "AC", "Rating"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setAlternatingRowColors(false);
        l->addWidget(table);

        StatusMessage* status = new StatusMessage;
        l->addWidget(status);

        connect(recBtn, &QPushButton::clicked, [=]() {
            int day = dayCombo->currentIndex();
            int slot = slotCombo->currentIndex();
            vector<Room*> rooms = backend.recommendRooms(day, slot);
            table->setRowCount(0);
            if (rooms.empty()) {
                status->showMsg("NOTICE: No rooms available at this time slot.");
                return;
            }
            status->hide();
            for (int i = 0; i < (int)rooms.size(); i++) {
                Room* r = rooms[i];
                table->insertRow(i);
                QTableWidgetItem* rank = new QTableWidgetItem("#" + QString::number(i+1));
                rank->setTextAlignment(Qt::AlignCenter);
                rank->setForeground(QColor("#f59e0b"));
                rank->setFont(QFont("", -1, QFont::Bold));

                QTableWidgetItem* rid = new QTableWidgetItem(QString::fromStdString(r->id));
                rid->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem* cat = new QTableWidgetItem(QString::fromStdString(r->category));
                cat->setTextAlignment(Qt::AlignCenter);
                if (r->category == "Good") {
                    cat->setForeground(QColor("#4ade80"));
                } else {
                    cat->setForeground(QColor("#f87171"));
                }

                QTableWidgetItem* ac = new QTableWidgetItem(r->hasAC ? "Yes" : "No");
                ac->setTextAlignment(Qt::AlignCenter);
                ac->setForeground(r->hasAC ? QColor("#4ade80") : QColor("#f87171"));

                QTableWidgetItem* rat = new QTableWidgetItem(QString::number(r->rating, 'f', 1));
                rat->setTextAlignment(Qt::AlignCenter);

                table->setItem(i, 0, rank);
                table->setItem(i, 1, rid);
                table->setItem(i, 2, cat);
                table->setItem(i, 3, ac);
                table->setItem(i, 4, rat);
            }
        });

        return w;
    }

    // ---- Tab: Book Room ----
    QWidget* buildBookTab() {
        QWidget* w = new QWidget;
        QVBoxLayout* l = new QVBoxLayout(w);
        l->setContentsMargins(20, 20, 20, 20);
        l->setSpacing(14);

        l->addWidget(makeLabel("Book a Room", "sectionLabel"));
        l->addWidget(makeSeparator());

        QFrame* card = makeCard();
        QGridLayout* gl = new QGridLayout(card);
        gl->setContentsMargins(20, 20, 20, 20);
        gl->setSpacing(12);

        QLineEdit* bookingEdit = new QLineEdit;
        bookingEdit->setPlaceholderText("e.g., BK-001");
        QLineEdit* studentEdit = new QLineEdit;
        studentEdit->setPlaceholderText("e.g., S-2024-01");
        QLineEdit* roomEdit = new QLineEdit;
        roomEdit->setPlaceholderText("e.g., A-6");
        QComboBox* dayCombo = makeDayCombo();
        QComboBox* slotCombo = makeSlotCombo();
        QPushButton* bookBtn = new QPushButton("Confirm Booking");
        bookBtn->setObjectName("successBtn");

        gl->addWidget(makeLabel("Booking ID:"), 0, 0);
        gl->addWidget(bookingEdit, 0, 1);
        gl->addWidget(makeLabel("Student ID:"), 1, 0);
        gl->addWidget(studentEdit, 1, 1);
        gl->addWidget(makeLabel("Room ID:"), 2, 0);
        gl->addWidget(roomEdit, 2, 1);
        gl->addWidget(makeLabel("Day:"), 3, 0);
        gl->addWidget(dayCombo, 3, 1);
        gl->addWidget(makeLabel("Time Slot:"), 4, 0);
        gl->addWidget(slotCombo, 4, 1);
        gl->addWidget(bookBtn, 5, 0, 1, 2);
        l->addWidget(card);

        StatusMessage* status = new StatusMessage;
        l->addWidget(status);
        l->addStretch();

        connect(bookBtn, &QPushButton::clicked, [=]() {
            QString bID = bookingEdit->text().trimmed();
            QString sID = studentEdit->text().trimmed();
            QString rID = roomEdit->text().trimmed();
            if (bID.isEmpty() || sID.isEmpty() || rID.isEmpty()) {
                status->showMsg("ERROR: Please fill in all fields.");
                return;
            }
            string result = backend.bookRoom(bID.toStdString(), rID.toStdString(), sID.toStdString(), dayCombo->currentIndex(), slotCombo->currentIndex());
            status->showMsg(QString::fromStdString(result));
            if (result.find("SUCCESS") != string::npos) {
                bookingEdit->clear(); studentEdit->clear(); roomEdit->clear();
            }
        });

        return w;
    }

    // ---- Tab: Cancel Booking ----
    QWidget* buildCancelTab() {
        QWidget* w = new QWidget;
        QVBoxLayout* l = new QVBoxLayout(w);
        l->setContentsMargins(20, 20, 20, 20);
        l->setSpacing(14);

        l->addWidget(makeLabel("Cancel a Booking", "sectionLabel"));
        l->addWidget(makeSeparator());

        QFrame* card = makeCard();
        QGridLayout* gl = new QGridLayout(card);
        gl->setContentsMargins(20, 20, 20, 20);
        gl->setSpacing(12);

        QLineEdit* roomEdit = new QLineEdit;
        roomEdit->setPlaceholderText("Room ID (e.g., A-6)");
        QComboBox* dayCombo = makeDayCombo();
        QComboBox* slotCombo = makeSlotCombo();
        QPushButton* cancelBtn = new QPushButton("Cancel Booking");
        cancelBtn->setObjectName("dangerBtn");

        gl->addWidget(makeLabel("Room ID:"), 0, 0);
        gl->addWidget(roomEdit, 0, 1);
        gl->addWidget(makeLabel("Day:"), 1, 0);
        gl->addWidget(dayCombo, 1, 1);
        gl->addWidget(makeLabel("Time Slot:"), 2, 0);
        gl->addWidget(slotCombo, 2, 1);
        gl->addWidget(cancelBtn, 3, 0, 1, 2);
        l->addWidget(card);

        StatusMessage* status = new StatusMessage;
        l->addWidget(status);
        l->addStretch();

        connect(cancelBtn, &QPushButton::clicked, [=]() {
            QString rID = roomEdit->text().trimmed();
            if (rID.isEmpty()) { status->showMsg("ERROR: Please enter a Room ID."); return; }
            string result = backend.cancelBooking(rID.toStdString(), dayCombo->currentIndex(), slotCombo->currentIndex());
            status->showMsg(QString::fromStdString(result));
        });

        return w;
    }

    // ---- Tab: Report Empty ----
    QWidget* buildReportTab() {
        QWidget* w = new QWidget;
        QVBoxLayout* l = new QVBoxLayout(w);
        l->setContentsMargins(20, 20, 20, 20);
        l->setSpacing(14);

        l->addWidget(makeLabel("Report Teacher Absent / Room Empty", "sectionLabel"));
        l->addWidget(makeSeparator());

        QFrame* info = makeCard();
        info->setStyleSheet("QFrame#card { background:#1c1100; border:1px solid #92400e; }");
        QHBoxLayout* il = new QHBoxLayout(info);
        il->setContentsMargins(14, 12, 14, 12);
        QLabel* infoL = new QLabel("⚠️  Marking a class as empty will free the room for immediate booking. This action can be undone.");
        infoL->setStyleSheet("color:#fbbf24; font-size:13px;");
        infoL->setWordWrap(true);
        il->addWidget(infoL);
        l->addWidget(info);

        QFrame* card = makeCard();
        QGridLayout* gl = new QGridLayout(card);
        gl->setContentsMargins(20, 20, 20, 20);
        gl->setSpacing(12);

        QLineEdit* roomEdit = new QLineEdit;
        roomEdit->setPlaceholderText("Room ID (e.g., A-6)");
        QComboBox* dayCombo = makeDayCombo();
        QComboBox* slotCombo = makeSlotCombo();
        QPushButton* reportBtn = new QPushButton("Mark Room as Empty");
        reportBtn->setObjectName("warningBtn");

        gl->addWidget(makeLabel("Room ID:"), 0, 0);
        gl->addWidget(roomEdit, 0, 1);
        gl->addWidget(makeLabel("Day:"), 1, 0);
        gl->addWidget(dayCombo, 1, 1);
        gl->addWidget(makeLabel("Time Slot:"), 2, 0);
        gl->addWidget(slotCombo, 2, 1);
        gl->addWidget(reportBtn, 3, 0, 1, 2);
        l->addWidget(card);

        StatusMessage* status = new StatusMessage;
        l->addWidget(status);
        l->addStretch();

        connect(reportBtn, &QPushButton::clicked, [=]() {
            QString rID = roomEdit->text().trimmed();
            if (rID.isEmpty()) { status->showMsg("ERROR: Please enter a Room ID."); return; }
            string result = backend.markClassEmpty(rID.toStdString(), dayCombo->currentIndex(), slotCombo->currentIndex());
            status->showMsg(QString::fromStdString(result));
        });

        return w;
    }

    // ---- Tab: Undo ----
    QWidget* buildUndoTab() {
        QWidget* w = new QWidget;
        QVBoxLayout* l = new QVBoxLayout(w);
        l->setContentsMargins(20, 20, 20, 20);
        l->setSpacing(14);

        l->addWidget(makeLabel("Undo Last Action", "sectionLabel"));
        l->addWidget(makeSeparator());

        QFrame* card = makeCard();
        QVBoxLayout* cl = new QVBoxLayout(card);
        cl->setContentsMargins(20, 20, 20, 20);
        cl->setSpacing(12);

        QLabel* desc = new QLabel("Click the button below to undo the most recent booking, cancellation, or empty report. Each click undoes one action.");
        desc->setWordWrap(true);
        desc->setStyleSheet("color:#8b8fa8; font-size:13px;");
        cl->addWidget(desc);

        QPushButton* undoBtn = new QPushButton("↩  Undo Last Action");
        undoBtn->setObjectName("undoBtn");
        undoBtn->setFixedWidth(200);
        undoBtn->setMinimumHeight(44);
        cl->addWidget(undoBtn);

        l->addWidget(card);

        StatusMessage* status = new StatusMessage;
        l->addWidget(status);
        l->addStretch();

        connect(undoBtn, &QPushButton::clicked, [=]() {
            string result = backend.undoLastAction();
            status->showMsg(QString::fromStdString(result));
        });

        return w;
    }
};

// ==========================================
// main()
// ==========================================
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyleSheet(STYLE_SHEET);
    app.setApplicationName("SCABS");
    app.setOrganizationName("University");

    SCABSWindow window;
    window.show();
    return app.exec();
}

#include "scabs_gui.moc"
