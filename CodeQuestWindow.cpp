
/*
 * CodeQuest - Main Application
 * Developer: Munah Sarah Sameer
 * Purpose: Learning, quizzes, progress tracking and reports.
 */

#include "CodeQuestWindow.h"

#include <QApplication>
#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStackedWidget>
#include <QScrollArea>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QRandomGenerator>
#include <QTextStream>
#include <QUrl>
#include <QDesktopServices>
#include <QTimer>
#include <QFont>
#include <algorithm>

// Creates a consistent page heading
static QLabel *heading(const QString &text)
{
    auto *label = new QLabel(text);
    label->setObjectName("heading");
    label->setWordWrap(true);
    return label;
}

// Creates a descriptive text label
static QLabel *description(const QString &text)
{
    auto *label = new QLabel(text);
    label->setObjectName("description");
    label->setWordWrap(true);
    return label;
}

// Creates a reusable white content card
static QFrame *card(QVBoxLayout **content = nullptr)
{
    auto *frame = new QFrame;
    frame->setObjectName("card");

    auto *layout = new QVBoxLayout(frame);
    layout->setContentsMargins(24, 22, 24, 22);
    layout->setSpacing(12);

    if (content)
        *content = layout;

    return frame;
}

// Creates a scrollable page with consistent spacing
static QWidget *page(QVBoxLayout **content)
{
    auto *container = new QWidget;
    auto *outer = new QVBoxLayout(container);
    outer->setContentsMargins(0, 0, 0, 0);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto *body = new QWidget;
    auto *layout = new QVBoxLayout(body);
    layout->setContentsMargins(36, 32, 36, 36);
    layout->setSpacing(20);

    scroll->setWidget(body);
    outer->addWidget(scroll);

    *content = layout;
    return container;
}

// Converts text to safe HTML for reports
static QString safe(const QString &text)
{
    return text.toHtmlEscaped();
}

// Stores progress outside the application folder
static QString progressPath()
{
    QString folder = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation);

    QDir().mkpath(folder);
    return folder + "/progress.json";
}

// Constructor: prepares the application
CodeQuestWindow::CodeQuestWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("CodeQuest | Learn, Code & Quiz");
    resize(1360, 850);
    setMinimumSize(1000, 650);

    loadLessons();
    loadQuestions();
    loadProgress();
    buildInterface();
    updateDashboard();
    updateResults();
}

// Builds the sidebar and application pages
void CodeQuestWindow::buildInterface()
{
    auto *central = new QWidget;
    auto *root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // Dark navy sidebar
    auto *sidebar = new QFrame;
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(255);

    auto *menu = new QVBoxLayout(sidebar);
    menu->setContentsMargins(20, 30, 20, 24);
    menu->setSpacing(12);

    auto *brand = new QLabel("◆ CODEQUEST");
    brand->setObjectName("brand");
    menu->addWidget(brand);

    auto *tagline = new QLabel("LEARN  •  CODE  •  ACHIEVE");
    tagline->setObjectName("tagline");
    menu->addWidget(tagline);

    menu->addSpacing(35);

    // Stack holds all pages, only one visible at a time
    pages = new QStackedWidget;
    pages->addWidget(buildDashboard());
    pages->addWidget(buildProfile());
    pages->addWidget(buildLearningCentre());
    pages->addWidget(buildQuizArena());
    pages->addWidget(buildResults());

    // Creates navigation buttons
    auto navigation = [this, menu](
                          const QString &label, int index)
    {
        auto *button = new QPushButton(label);
        button->setObjectName("navButton");
        button->setCursor(Qt::PointingHandCursor);

        menu->addWidget(button);

        connect(button, &QPushButton::clicked,
                this, [this, index]()
                {
            pages->setCurrentIndex(index);

            if (index == 0)
                updateDashboard();

            if (index == 4)
                updateResults(); });
    };

    navigation("⌂   Dashboard", 0);
    navigation("♙   Player Profile", 1);
    navigation("▤   Learning Centre", 2);
    navigation("✦   Quiz Arena", 3);
    navigation("▥   Results & Reports", 4);

    menu->addStretch();

    sidebarPlayer = new QLabel;
    sidebarPlayer->setObjectName("sidebarPlayer");
    sidebarPlayer->setWordWrap(true);
    menu->addWidget(sidebarPlayer);

    auto *footer = new QLabel("CodeQuest  •  Version 1.0");
    footer->setObjectName("tagline");
    menu->addWidget(footer);

    root->addWidget(sidebar);
    root->addWidget(pages, 1);
    setCentralWidget(central);

    // The visual theme is shared across all pages
    setStyleSheet(R"(
        QMainWindow, QScrollArea {
            background: #F7F4ED;
        }

        QWidget {
            font-family: 'Segoe UI';
            font-size: 14px;
            color: #1B2B42;
        }

        QFrame#sidebar {
            background: #17263D;
        }

        QLabel#brand {
            color: #E4C17C;
            background: transparent;
            font-size: 24px;
            font-weight: 800;
        }

        QLabel#tagline {
            color: #AAB7C8;
            background: transparent;
            font-size: 10px;
            letter-spacing: 1px;
        }

        QLabel#sidebarPlayer {
            color: #FFFFFF;
            background: #263B56;
            border-radius: 10px;
            padding: 14px;
        }

        QPushButton#navButton {
            background: transparent;
            color: #EAF0F8;
            border: none;
            border-radius: 10px;
            padding: 15px;
            text-align: left;
            font-size: 14px;
        }

        QPushButton#navButton:hover {
            background: #314760;
            color: #E4C17C;
        }

        QFrame#card {
            background: #FFFFFF;
            border: 1px solid #E7E2D9;
            border-radius: 16px;
        }

        QLabel#heading {
            background: transparent;
            color: #17263D;
            font-size: 30px;
            font-weight: 800;
        }

        QLabel#description {
            background: transparent;
            color: #66758A;
            font-size: 14px;
        }

        QLabel#statValue {
            background: transparent;
            color: #17263D;
            font-size: 29px;
            font-weight: 800;
        }

        QLabel#statName {
            background: transparent;
            color: #66758A;
            font-size: 13px;
        }

        QPushButton {
            background: #17263D;
            color: #FFFFFF;
            border: none;
            border-radius: 9px;
            padding: 12px 18px;
            font-weight: 600;
        }

        QPushButton:hover {
            background: #304969;
        }

        QPushButton:disabled {
            background: #CBD2DC;
            color: #637084;
        }

        QPushButton#goldButton {
            background: #D5AE65;
            color: #17263D;
        }

        QPushButton#goldButton:hover {
            background: #E9C886;
        }

        QLineEdit, QComboBox, QTextEdit {
            background: #FFFFFF;
            color: #17263D;
            border: 1px solid #D5DCE4;
            border-radius: 8px;
            padding: 10px;
            selection-background-color: #D5AE65;
        }

        QProgressBar {
            background: #E9EDF2;
            border: none;
            border-radius: 6px;
            height: 14px;
            text-align: center;
        }

        QProgressBar::chunk {
            background: #D5AE65;
            border-radius: 6px;
        }
    )");

    sidebarPlayer->setText("PLAYER\n" + playerName);
    pages->setCurrentIndex(0);
}
