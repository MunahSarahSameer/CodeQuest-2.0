
#ifndef CODEQUESTWINDOW_H
#define CODEQUESTWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QSet>

class QStackedWidget;
class QLabel;
class QComboBox;
class QLineEdit;
class QVBoxLayout;
class QProgressBar;
class QTextEdit;
class QPushButton;

// Stores a programming lesson
struct Lesson
{
    QString language;
    QString title;
    QString explanation;
    QString code;
    QString output;
    QString practice;
};

// Stores a quiz question and its answer
struct Question
{
    QString language;
    QString difficulty;
    QString topic;
    QString text;
    QStringList options;
    int correct = 0;
    QString explanation;
};

// Stores a completed quiz
struct QuizResult
{
    QString language;
    QString difficulty;
    QString date;
    int score = 0;
    int total = 15;
    QString review;
};

// Main application class
class CodeQuestWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CodeQuestWindow(QWidget *parent = nullptr);

private:
    // Application data
    QString playerName = "Guest";
    QSet<QString> completedLessons;
    QVector<Lesson> lessons;
    QVector<Question> questionBank;
    QVector<QuizResult> history;

    // Quiz state
    QVector<Question> activeQuiz;
    QString currentLanguage;
    QString currentDifficulty;
    QString currentReview;
    int questionIndex = 0;
    int currentScore = 0;
    bool answerLocked = false;

    // Main navigation
    QStackedWidget *pages = nullptr;
    QLabel *sidebarPlayer = nullptr;

    // Dashboard widgets
    QLabel *welcomeLabel = nullptr;
    QLabel *lessonsStat = nullptr;
    QLabel *quizzesStat = nullptr;
    QLabel *averageStat = nullptr;
    QProgressBar *overallProgress = nullptr;

    // Profile widgets
    QLineEdit *nameInput = nullptr;

    // Learning Centre widgets
    QComboBox *lessonLanguage = nullptr;
    QComboBox *lessonSelector = nullptr;
    QLabel *lessonTitle = nullptr;
    QTextEdit *lessonContent = nullptr;
    QPushButton *completeButton = nullptr;

    // Quiz widgets
    QComboBox *quizLanguage = nullptr;
    QComboBox *quizDifficulty = nullptr;
    QLabel *questionLabel = nullptr;
    QLabel *quizProgressLabel = nullptr;
    QLabel *feedbackLabel = nullptr;
    QProgressBar *quizProgress = nullptr;
    QVector<QPushButton *> answerButtons;
    QPushButton *nextButton = nullptr;
    QPushButton *startButton = nullptr;

    // Results widgets
    QLabel *resultsSummary = nullptr;
    QTextEdit *resultsHistory = nullptr;

    // Interface construction
    void buildInterface();
    QWidget *buildDashboard();
    QWidget *buildProfile();
    QWidget *buildLearningCentre();
    QWidget *buildQuizArena();
    QWidget *buildResults();

    // Data and application logic
    void loadLessons();
    void loadQuestions();
    void loadProgress();
    void saveProgress();

    // Player and learning functions
    void updateDashboard();
    void updateLessonList();
    void displayLesson();
    void completeLesson();

    // Quiz functions
    void startQuiz();
    void showQuestion();
    void submitAnswer(int selected);
    void nextQuestion();
    void finishQuiz();

    // Reporting
    void updateResults();
    void exportReport();
};

#endif
