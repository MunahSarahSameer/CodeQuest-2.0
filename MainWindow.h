
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class QLabel;

// Main window of the CodeQuest application
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor and destructor
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private:
    // Holds the different application pages
    QStackedWidget *pages = nullptr;

    // Displays the current player's name
    QLabel *playerLabel = nullptr;

    // Builds the main user interface
    void setupUI();

    // Creates the dashboard page
    QWidget *createDashboard();

    // Creates the learning centre page
    QWidget *createLearningCentre();

    // Creates the quiz page
    QWidget *createQuizArena();

    // Creates the results page
    QWidget *createResultsPage();
};

#endif