
/*
 * CodeQuest - Main Window
 * Developer: Munah Sarah Sameer
 * Purpose: Builds the application's main interface.
 */

#include "MainWindow.h"

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QFrame>
#include <QFont>

// Constructor: runs when the window is created
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("CodeQuest | Learn, Code & Quiz");
    resize(1200, 760);

    // Build the interface
    setupUI();
}

// Creates a simple page with a heading and description
static QWidget *makePage(const QString &heading,
                         const QString &description)
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(45, 40, 45, 40);
    layout->setSpacing(16);

    auto *title = new QLabel(heading);
    title->setObjectName("pageTitle");

    auto *details = new QLabel(description);
    details->setWordWrap(true);
    details->setObjectName("description");

    layout->addWidget(title);
    layout->addWidget(details);
    layout->addStretch();

    return page;
}

// Builds the sidebar and connects navigation buttons
void MainWindow::setupUI()
{
    auto *central = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar container
    auto *sidebar = new QFrame;
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(245);

    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(20, 30, 20, 25);
    sidebarLayout->setSpacing(12);

    // Application branding
    auto *logo = new QLabel("CODEQUEST");
    logo->setObjectName("logo");
    sidebarLayout->addWidget(logo);

    auto *tagline = new QLabel("Learn. Code. Achieve.");
    tagline->setObjectName("tagline");
    sidebarLayout->addWidget(tagline);

    sidebarLayout->addSpacing(30);

    // Pages appear in this stacked container
    pages = new QStackedWidget;
    pages->addWidget(createDashboard());
    pages->addWidget(createLearningCentre());
    pages->addWidget(createQuizArena());
    pages->addWidget(createResultsPage());

    // Creates a button and connects it to a page
    auto addNavigation = [this, sidebarLayout](
                             const QString &name, int pageIndex)
    {
        auto *button = new QPushButton(name);
        button->setCursor(Qt::PointingHandCursor);

        sidebarLayout->addWidget(button);

        // Lambda runs when the user clicks the button
        connect(button, &QPushButton::clicked,
                this, [this, pageIndex]()
                { pages->setCurrentIndex(pageIndex); });
    };

    addNavigation("Dashboard", 0);
    addNavigation("Learning Centre", 1);
    addNavigation("Quiz Arena", 2);
    addNavigation("Results & Reports", 3);

    sidebarLayout->addStretch();

    playerLabel = new QLabel("Player: Guest");
    playerLabel->setObjectName("playerLabel");
    sidebarLayout->addWidget(playerLabel);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(pages, 1);

    setCentralWidget(central);

    // Cream, navy and gold colour palette
    setStyleSheet(R"(
        QMainWindow, QWidget {
            background-color: #F8F5EF;
            color: #17263D;
            font-family: 'Segoe UI';
            font-size: 14px;
        }

        QFrame#sidebar {
            background-color: #17263D;
        }

        QLabel#logo {
            color: #D9B66F;
            font-size: 25px;
            font-weight: bold;
        }

        QLabel#tagline, QLabel#playerLabel {
            color: #E5E0D5;
        }

        QFrame#sidebar QPushButton {
            background-color: #253852;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 13px;
            text-align: left;
        }

        QFrame#sidebar QPushButton:hover {
            background-color: #B99755;
            color: #17263D;
        }

        QLabel#pageTitle {
            font-size: 28px;
            font-weight: bold;
            color: #17263D;
        }

        QLabel#description {
            font-size: 15px;
            color: #596579;
        }
    )");
}

// Dashboard landing page
QWidget *MainWindow::createDashboard()
{
    return makePage(
        "Welcome to CodeQuest",
        "Learn C++, Java and Python. Track your progress, "
        "complete lessons and challenge yourself with quizzes.");
}

// Learning Centre page
QWidget *MainWindow::createLearningCentre()
{
    return makePage(
        "Learning Centre",
        "Choose C++, Java or Python to begin your lessons. "
        "Detailed tutorials and practice activities are coming next.");
}

// Quiz Arena page
QWidget *MainWindow::createQuizArena()
{
    return makePage(
        "Quiz Arena",
        "Test your knowledge with 15 questions per quiz. "
        "Choose a language and difficulty level.");
}

// Results page
QWidget *MainWindow::createResultsPage()
{
    return makePage(
        "Results & Reports",
        "Review quiz scores, identify areas for improvement "
        "and export performance reports.");
}