
/*
 * CODEQUEST - Learn, Code & Quiz
 * Developer: Munah Sarah Sameer
 * Version: 2.0
 *
 * A desktop learning game for C++, Java and Python.
 * Built with C++17 and Qt6 Widgets.
 */

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QTextBrowser>
#include <QTextEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStackedWidget>
#include <QScrollArea>
#include <QProgressBar>
#include <QFrame>
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QVector>
#include <QStringList>
#include <QButtonGroup>
#include <QTextStream>

#include <algorithm>

#include "PlayerWelcome.h"

// Stores one quiz question.
struct Question
{
    QString language;
    QString difficulty;
    QString topic;
    QString question;
    QStringList options;
    int answer = 0;
    QString explanation;
};

// Stores one completed quiz.
struct QuizResult
{
    QString language;
    QString difficulty;
    int setNumber = 1;
    int score = 0;
    int total = 0;
    QString date;
};

// Stores one learning lesson.
struct Lesson
{
    QString language;
    QString title;
    QString explanation;
    QString example;
};

class CodeQuest : public QMainWindow
{
private:
    // Main application data.
    QString player = "Guest";
    QString avatar = "👩‍💻";

    QVector<Question> questionBank;
    QVector<Question> activeQuestions;
    QVector<QuizResult> history;
    QVector<Lesson> lessons;

    QStringList completedLessons;

    int currentQuestion = 0;
    int currentScore = 0;
    int selectedAnswer = -1;
    int currentSet = 1;

    QString activeLanguage;
    QString activeDifficulty;

    // Main interface widgets.
    QStackedWidget *pages = nullptr;
    QLabel *sidebarPlayer = nullptr;
    QLabel *dashboardGreeting = nullptr;
    QLabel *dashboardStats = nullptr;
    QLabel *quizHeading = nullptr;
    QLabel *questionText = nullptr;
    QLabel *questionCounter = nullptr;
    QLabel *quizProgressText = nullptr;
    QLabel *lessonTitle = nullptr;
    QLabel *profileText = nullptr;

    QTextBrowser *lessonContent = nullptr;
    QTextBrowser *historyContent = nullptr;

    QProgressBar *quizProgress = nullptr;

    QComboBox *languageChoice = nullptr;
    QComboBox *difficultyChoice = nullptr;
    QComboBox *setChoice = nullptr;
    QComboBox *lessonLanguage = nullptr;
    QComboBox *lessonChoice = nullptr;

    QPushButton *answerButtons[4] = {
        nullptr, nullptr, nullptr, nullptr};

    QPushButton *nextButton = nullptr;
    QPushButton *completeLessonButton = nullptr;

    QWidget *quizSelectionPage = nullptr;
    QWidget *quizGamePage = nullptr;

    // Returns the local progress file location.
    QString savePath() const
    {
        QString folder =
            QStandardPaths::writableLocation(
                QStandardPaths::AppDataLocation);

        QDir().mkpath(folder);

        return folder + "/codequest_progress.json";
    }

    // Creates a styled label.
    QLabel *makeLabel(
        const QString &text,
        const QString &objectName = "")
    {
        auto *label = new QLabel(text);
        label->setWordWrap(true);

        if (!objectName.isEmpty())
            label->setObjectName(objectName);

        return label;
    }

    // Creates a navigation button.
    QPushButton *makeButton(
        const QString &text,
        const QString &objectName = "")
    {
        auto *button = new QPushButton(text);
        button->setMinimumHeight(44);
        button->setCursor(Qt::PointingHandCursor);

        if (!objectName.isEmpty())
            button->setObjectName(objectName);

        return button;
    }

    // Displays a custom application popup.
    void showPopup(
        const QString &title,
        const QString &message)
    {
        QDialog dialog(this);
        dialog.setWindowTitle(title);
        dialog.setFixedSize(440, 300);

        auto *layout = new QVBoxLayout(&dialog);
        layout->setContentsMargins(30, 25, 30, 25);
        layout->setSpacing(18);

        auto *icon = makeLabel("◆", "popupIcon");
        icon->setAlignment(Qt::AlignCenter);

        auto *heading = makeLabel(title, "pageTitle");
        heading->setAlignment(Qt::AlignCenter);

        auto *body = makeLabel(message);
        body->setAlignment(Qt::AlignCenter);

        auto *closeButton = makeButton(
            "CONTINUE", "goldButton");

        layout->addWidget(icon);
        layout->addWidget(heading);
        layout->addWidget(body);
        layout->addStretch();
        layout->addWidget(closeButton);

        connect(
            closeButton,
            &QPushButton::clicked,
            &dialog,
            &QDialog::accept);

        dialog.setStyleSheet(styleSheet());
        dialog.exec();
    }

    // Reads quiz questions from the embedded JSON.
    void loadQuestions()
    {
        questionBank.clear();

        QFile file(":/questions.json");

        if (!file.open(QIODevice::ReadOnly))
            return;

        QJsonParseError parseError;

        QJsonDocument document =
            QJsonDocument::fromJson(
                file.readAll(), &parseError);

        if (parseError.error != QJsonParseError::NoError ||
            !document.isArray())
        {
            return;
        }

        for (const QJsonValue &value : document.array())
        {
            QJsonObject object = value.toObject();

            Question question;

            question.language =
                object.value("language").toString();

            question.difficulty =
                object.value("difficulty").toString();

            question.topic =
                object.value("topic").toString();

            question.question =
                object.value("question").toString();

            question.answer =
                object.value("answer").toInt(-1);

            question.explanation =
                object.value("explanation").toString();

            for (const QJsonValue &option :
                 object.value("options").toArray())
            {
                question.options.append(option.toString());
            }

            if (!question.question.isEmpty() &&
                question.options.size() == 4 &&
                question.answer >= 0 &&
                question.answer < 4)
            {
                questionBank.append(question);
            }
        }
    }

    // Adds learning content for all three languages.
    void loadLessons()
    {
        lessons.clear();

        auto add = [this](
                       const QString &language,
                       const QString &title,
                       const QString &explanation,
                       const QString &example)
        {
            lessons.append({language, title, explanation, example});
        };

        // C++ lessons.
        add("C++", "Introduction to C++",
            "C++ is a compiled programming language used "
            "for desktop software, games and systems.",
            "#include <iostream>\n"
            "int main() {\n"
            "    std::cout << \"Hello, World!\";\n"
            "    return 0;\n"
            "}");

        add("C++", "Variables and Data Types",
            "Variables store information. Common types "
            "include int, double, char, bool and string.",
            "int age = 36;\n"
            "double price = 12.50;\n"
            "bool active = true;");

        add("C++", "Input and Output",
            "Use std::cout to display text and "
            "std::cin to receive user input.",
            "int age;\n"
            "std::cin >> age;\n"
            "std::cout << age;");

        add("C++", "Conditions",
            "An if statement chooses which code runs "
            "based on a condition.",
            "if (score >= 50) {\n"
            "    std::cout << \"Pass\";\n"
            "} else {\n"
            "    std::cout << \"Try again\";\n"
            "}");

        add("C++", "Loops",
            "Loops repeat instructions. For loops are "
            "useful when the number of repetitions is known.",
            "for (int i = 1; i <= 5; i++) {\n"
            "    std::cout << i;\n"
            "}");

        add("C++", "Functions",
            "Functions group reusable instructions.",
            "int add(int a, int b) {\n"
            "    return a + b;\n"
            "}");

        add("C++", "Classes and Objects",
            "Classes define properties and behaviours. "
            "Objects are instances of classes.",
            "class Student {\n"
            "public:\n"
            "    int age = 20;\n"
            "};\n"
            "Student student;");

        add("C++", "Vectors",
            "A vector is a dynamic collection of elements.",
            "#include <vector>\n"
            "std::vector<int> marks = {90, 95, 100};");

        add("C++", "References and Pointers",
            "A reference provides another name for an "
            "existing variable. A pointer stores an address.",
            "int x = 10;\n"
            "int &ref = x;\n"
            "int *ptr = &x;");

        add("C++", "Exception Handling",
            "Exceptions help programs respond to errors.",
            "try {\n"
            "    throw std::runtime_error(\"Error\");\n"
            "} catch (const std::exception &e) {\n"
            "    std::cout << e.what();\n"
            "}");

        // Java lessons.
        add("Java", "Introduction to Java",
            "Java is an object-oriented language. "
            "Java programs run on the Java Virtual Machine.",
            "public class Main {\n"
            "    public static void main(String[] args) {\n"
            "        System.out.println(\"Hello!\");\n"
            "    }\n"
            "}");

        add("Java", "Variables",
            "Java uses declared data types such as int, "
            "double, boolean and String.",
            "int age = 20;\n"
            "String name = \"Alex\";\n"
            "boolean active = true;");

        add("Java", "Conditions",
            "Conditional statements control decisions.",
            "if (score >= 50) {\n"
            "    System.out.println(\"Pass\");\n"
            "} else {\n"
            "    System.out.println(\"Fail\");\n"
            "}");

        add("Java", "Loops",
            "Java supports for, while and do-while loops.",
            "for (int i = 0; i < 5; i++) {\n"
            "    System.out.println(i);\n"
            "}");

        add("Java", "Methods",
            "Methods define actions inside classes.",
            "static int add(int a, int b) {\n"
            "    return a + b;\n"
            "}");

        add("Java", "Classes",
            "A class describes the structure and "
            "behaviour of objects.",
            "class Student {\n"
            "    String name;\n"
            "    Student(String n) { name = n; }\n"
            "}");

        add("Java", "Arrays",
            "Arrays store fixed-size groups of values.",
            "int[] scores = {80, 90, 100};\n"
            "System.out.println(scores[0]);");

        add("Java", "Inheritance",
            "Inheritance lets a class reuse and extend "
            "another class.",
            "class Animal { }\n"
            "class Dog extends Animal { }");

        add("Java", "Interfaces",
            "Interfaces describe behaviours that "
            "implementing classes must provide.",
            "interface Playable {\n"
            "    void play();\n"
            "}");

        add("Java", "Exception Handling",
            "Try-catch handles exceptions.",
            "try {\n"
            "    int x = 10 / 0;\n"
            "} catch (ArithmeticException e) {\n"
            "    System.out.println(\"Invalid division\");\n"
            "}");

        // Python lessons.
        add("Python", "Introduction to Python",
            "Python is a readable programming language "
            "used in AI, data science and automation.",
            "print(\"Hello, World!\")");

        add("Python", "Variables",
            "Python variables do not need explicit "
            "type declarations.",
            "name = \"Alex\"\n"
            "age = 20\n"
            "price = 19.99");

        add("Python", "Input and Output",
            "Use input() to receive text and print() "
            "to display results.",
            "name = input(\"Your name: \")\n"
            "print(f\"Hello, {name}\")");

        add("Python", "Conditions",
            "Python uses indentation to define blocks.",
            "if score >= 50:\n"
            "    print(\"Pass\")\n"
            "else:\n"
            "    print(\"Try again\")");

        add("Python", "Loops",
            "Use for to iterate over a sequence.",
            "for i in range(5):\n"
            "    print(i)");

        add("Python", "Functions",
            "The def keyword creates functions.",
            "def add(a, b):\n"
            "    return a + b");

        add("Python", "Lists",
            "Lists store ordered, changeable collections.",
            "scores = [80, 90, 100]\n"
            "scores.append(95)");

        add("Python", "Dictionaries",
            "Dictionaries store key-value pairs.",
            "student = {\n"
            "    \"name\": \"Alex\",\n"
            "    \"score\": 95\n"
            "}");

        add("Python", "Classes",
            "Classes organise data and behaviour.",
            "class Student:\n"
            "    def __init__(self, name):\n"
            "        self.name = name");

        add("Python", "Exception Handling",
            "Try-except handles runtime errors.",
            "try:\n"
            "    number = int(input(\"Number: \"))\n"
            "except ValueError:\n"
            "    print(\"Invalid number\")");
    }

    // Saves player details and learning progress.
    void saveData()
    {
        QJsonObject root;

        root["player"] = player;
        root["avatar"] = avatar;

        QJsonArray completed;

        for (const QString &lesson : completedLessons)
            completed.append(lesson);

        root["completedLessons"] = completed;

        QJsonArray results;

        for (const QuizResult &result : history)
        {
            QJsonObject item;

            item["language"] = result.language;
            item["difficulty"] = result.difficulty;
            item["set"] = result.setNumber;
            item["score"] = result.score;
            item["total"] = result.total;
            item["date"] = result.date;

            results.append(item);
        }

        root["history"] = results;

        QFile file(savePath());

        if (file.open(QIODevice::WriteOnly))
        {
            file.write(
                QJsonDocument(root).toJson(
                    QJsonDocument::Indented));
        }
    }

    // Loads saved player details and results.
    void loadData()
    {
        QFile file(savePath());

        if (!file.open(QIODevice::ReadOnly))
            return;

        QJsonDocument document =
            QJsonDocument::fromJson(file.readAll());

        if (!document.isObject())
            return;

        QJsonObject root = document.object();

        player = root.value("player")
                     .toString("Guest");

        avatar = root.value("avatar")
                     .toString("👩‍💻");

        for (const QJsonValue &value :
             root.value("completedLessons").toArray())
        {
            completedLessons.append(value.toString());
        }

        for (const QJsonValue &value :
             root.value("history").toArray())
        {
            QJsonObject item = value.toObject();

            QuizResult result;

            result.language =
                item.value("language").toString();

            result.difficulty =
                item.value("difficulty").toString();

            result.setNumber =
                item.value("set").toInt(1);

            result.score =
                item.value("score").toInt();

            result.total =
                item.value("total").toInt();

            result.date =
                item.value("date").toString();

            history.append(result);
        }
    }

    // Creates the main window and sidebar.
    void buildInterface()
    {
        auto *central = new QWidget;
        setCentralWidget(central);

        auto *mainLayout = new QHBoxLayout(central);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        auto *sidebar = new QWidget;
        sidebar->setObjectName("sidebar");
        sidebar->setFixedWidth(260);

        auto *sideLayout = new QVBoxLayout(sidebar);
        sideLayout->setContentsMargins(20, 28, 20, 24);
        sideLayout->setSpacing(12);

        auto *logo = makeLabel("◆ CODEQUEST", "logo");
        sideLayout->addWidget(logo);

        sideLayout->addSpacing(18);

        sidebarPlayer = makeLabel("", "playerCard");
        sidebarPlayer->setAlignment(Qt::AlignCenter);
        sideLayout->addWidget(sidebarPlayer);

        sideLayout->addSpacing(22);

        pages = new QStackedWidget;

        auto *dashboardButton =
            makeButton("⌂   Dashboard", "navButton");

        auto *learningButton =
            makeButton("▤   Learning Centre", "navButton");

        auto *quizButton =
            makeButton("◆   Quiz Arena", "navButton");

        auto *historyButton =
            makeButton("◷   Results & History", "navButton");

        auto *profileButton =
            makeButton("◉   Player Profile", "navButton");

        sideLayout->addWidget(dashboardButton);
        sideLayout->addWidget(learningButton);
        sideLayout->addWidget(quizButton);
        sideLayout->addWidget(historyButton);
        sideLayout->addWidget(profileButton);

        sideLayout->addStretch();

        auto *credit = makeLabel(
            "CODEQUEST 2.0\n"
            "Developer: Munah Sarah Sameer");
        credit->setObjectName("sidebarCredit");
        sideLayout->addWidget(credit);

        mainLayout->addWidget(sidebar);
        mainLayout->addWidget(pages, 1);

        pages->addWidget(createDashboard());    // Page 0
        pages->addWidget(createLearningPage()); // Page 1
        pages->addWidget(createQuizPage());     // Page 2
        pages->addWidget(createHistoryPage());  // Page 3
        pages->addWidget(createProfilePage());  // Page 4

        connect(
            dashboardButton, &QPushButton::clicked,
            this, [this]()
            {
                refreshDashboard();
                pages->setCurrentIndex(0); });

        connect(
            learningButton, &QPushButton::clicked,
            this, [this]()
            { pages->setCurrentIndex(1); });

        connect(
            quizButton, &QPushButton::clicked,
            this, [this]()
            {
                updateQuizSets();
                pages->setCurrentIndex(2); });

        connect(
            historyButton, &QPushButton::clicked,
            this, [this]()
            {
                refreshHistory();
                pages->setCurrentIndex(3); });

        connect(
            profileButton, &QPushButton::clicked,
            this, [this]()
            {
                refreshProfile();
                pages->setCurrentIndex(4); });

        applyTheme();
        refreshDashboard();
    }

    // Builds the dashboard.
    QWidget *createDashboard()
    {
        auto *page = new QWidget;
        auto *layout = new QVBoxLayout(page);

        layout->setContentsMargins(45, 35, 45, 35);
        layout->setSpacing(20);

        layout->addWidget(
            makeLabel("YOUR LEARNING DASHBOARD", "eyebrow"));

        dashboardGreeting =
            makeLabel("", "pageTitle");

        layout->addWidget(dashboardGreeting);

        layout->addWidget(
            makeLabel(
                "Build your coding skills one lesson "
                "and one challenge at a time."));

        auto *statsCard = new QFrame;
        statsCard->setObjectName("card");

        auto *statsLayout = new QVBoxLayout(statsCard);
        statsLayout->setContentsMargins(25, 25, 25, 25);

        dashboardStats = makeLabel("");
        dashboardStats->setObjectName("stats");

        statsLayout->addWidget(dashboardStats);
        layout->addWidget(statsCard);

        layout->addWidget(
            makeLabel("EXPLORE YOUR LEARNING PATH", "sectionTitle"));

        auto *cards = new QGridLayout;
        cards->setSpacing(15);

        const QStringList languages = {
            "C++", "Java", "Python"};

        const QStringList descriptions = {
            "Systems and object-oriented programming",
            "Object-oriented software development",
            "AI, data science and automation"};

        for (int i = 0; i < languages.size(); ++i)
        {
            auto *card = new QFrame;
            card->setObjectName("card");

            auto *cardLayout = new QVBoxLayout(card);
            cardLayout->setContentsMargins(20, 20, 20, 20);

            cardLayout->addWidget(
                makeLabel(languages[i], "sectionTitle"));

            cardLayout->addWidget(
                makeLabel(descriptions[i]));

            auto *button =
                makeButton("START LEARNING", "goldButton");

            cardLayout->addWidget(button);

            QString language = languages[i];

            connect(
                button, &QPushButton::clicked,
                this, [this, language]()
                {
                    lessonLanguage->setCurrentText(language);
                    pages->setCurrentIndex(1); });

            cards->addWidget(card, 0, i);
        }

        layout->addLayout(cards);
        layout->addStretch();

        return page;
    }

    // Builds the learning centre.
    QWidget *createLearningPage()
    {
        auto *page = new QWidget;
        auto *layout = new QVBoxLayout(page);

        layout->setContentsMargins(45, 35, 45, 35);
        layout->setSpacing(16);

        layout->addWidget(
            makeLabel("LEARNING CENTRE", "eyebrow"));

        layout->addWidget(
            makeLabel("Explore Programming", "pageTitle"));

        layout->addWidget(
            makeLabel(
                "Read the explanation, study the code "
                "and mark each lesson complete."));

        auto *selection = new QHBoxLayout;

        lessonLanguage = new QComboBox;
        lessonLanguage->addItems({"C++", "Java", "Python"});

        lessonChoice = new QComboBox;

        selection->addWidget(lessonLanguage);
        selection->addWidget(lessonChoice, 1);

        layout->addLayout(selection);

        lessonTitle =
            makeLabel("", "sectionTitle");

        layout->addWidget(lessonTitle);

        lessonContent = new QTextBrowser;
        lessonContent->setObjectName("lessonBrowser");

        layout->addWidget(lessonContent, 1);

        completeLessonButton =
            makeButton("MARK LESSON COMPLETE", "goldButton");

        layout->addWidget(completeLessonButton);

        connect(
            lessonLanguage,
            &QComboBox::currentTextChanged,
            this,
            [this]()
            {
                updateLessonChoices();
            });

        connect(
            lessonChoice,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this]()
            {
                displayLesson();
            });

        connect(
            completeLessonButton,
            &QPushButton::clicked,
            this,
            [this]()
            {
                QString key =
                    lessonLanguage->currentText() +
                    "|" +
                    lessonChoice->currentText();

                if (!completedLessons.contains(key))
                {
                    completedLessons.append(key);
                    saveData();
                }

                displayLesson();

                showPopup(
                    "Lesson Completed!",
                    "Excellent work! Your learning "
                    "progress has been saved.");
            });

        updateLessonChoices();

        return page;
    }

    // Updates the lessons for the selected language.
    void updateLessonChoices()
    {
        lessonChoice->blockSignals(true);
        lessonChoice->clear();

        for (const Lesson &lesson : lessons)
        {
            if (lesson.language ==
                lessonLanguage->currentText())
            {
                lessonChoice->addItem(lesson.title);
            }
        }

        lessonChoice->blockSignals(false);
        displayLesson();
    }

    // Displays the selected lesson and code example.
    void displayLesson()
    {
        if (!lessonChoice || !lessonContent)
            return;

        for (const Lesson &lesson : lessons)
        {
            if (lesson.language !=
                lessonLanguage->currentText())
                continue;

            if (lesson.title !=
                lessonChoice->currentText())
                continue;

            lessonTitle->setText(lesson.title);

            QString html =
                "<h2>" + lesson.title.toHtmlEscaped() +
                "</h2><p>" +
                lesson.explanation.toHtmlEscaped() +
                "</p><h3>Code Example</h3>"
                "<pre style='background:#17263D;"
                "color:#F8E7BE;padding:20px;"
                "white-space:pre-wrap;'>" +
                lesson.example.toHtmlEscaped() +
                "</pre>";

            lessonContent->setHtml(html);

            QString key =
                lesson.language + "|" + lesson.title;

            completeLessonButton->setText(
                completedLessons.contains(key)
                    ? "✓ LESSON COMPLETED"
                    : "MARK LESSON COMPLETE");

            return;
        }
    }

    // Builds the quiz selection and question interface.
    QWidget *createQuizPage()
    {
        auto *page = new QWidget;
        auto *outer = new QVBoxLayout(page);

        outer->setContentsMargins(45, 35, 45, 35);
        outer->setSpacing(18);

        outer->addWidget(
            makeLabel("QUIZ ARENA", "eyebrow"));

        outer->addWidget(
            makeLabel("Test Your Knowledge", "pageTitle"));

        auto *quizStack = new QStackedWidget;

        quizSelectionPage = new QWidget;
        quizGamePage = new QWidget;

        // Quiz selection screen.
        auto *selectLayout =
            new QVBoxLayout(quizSelectionPage);

        selectLayout->setSpacing(16);

        selectLayout->addWidget(
            makeLabel(
                "Choose a language, difficulty "
                "and question set."));

        selectLayout->addWidget(
            makeLabel("PROGRAMMING LANGUAGE", "eyebrow"));

        languageChoice = new QComboBox;
        languageChoice->addItems({"C++", "Java", "Python"});
        selectLayout->addWidget(languageChoice);

        selectLayout->addWidget(
            makeLabel("DIFFICULTY LEVEL", "eyebrow"));

        difficultyChoice = new QComboBox;
        difficultyChoice->addItems({"Beginner", "Intermediate", "Advanced"});
        selectLayout->addWidget(difficultyChoice);

        selectLayout->addWidget(
            makeLabel("QUESTION SET", "eyebrow"));

        setChoice = new QComboBox;
        selectLayout->addWidget(setChoice);

        auto *startButton =
            makeButton("START QUIZ", "goldButton");

        selectLayout->addWidget(startButton);
        selectLayout->addStretch();

        // Question screen.
        auto *gameLayout =
            new QVBoxLayout(quizGamePage);

        gameLayout->setSpacing(16);

        quizHeading =
            makeLabel("", "sectionTitle");

        gameLayout->addWidget(quizHeading);

        questionCounter = makeLabel("");
        gameLayout->addWidget(questionCounter);

        quizProgress = new QProgressBar;
        quizProgress->setTextVisible(false);
        quizProgress->setRange(0, 100);
        gameLayout->addWidget(quizProgress);

        quizProgressText = makeLabel("");
        gameLayout->addWidget(quizProgressText);

        questionText =
            makeLabel("", "questionText");

        gameLayout->addWidget(questionText);

        for (int i = 0; i < 4; ++i)
        {
            answerButtons[i] =
                makeButton("", "answerButton");

            gameLayout->addWidget(answerButtons[i]);

            int answerIndex = i;

            connect(
                answerButtons[i],
                &QPushButton::clicked,
                this,
                [this, answerIndex]()
                {
                    selectedAnswer = answerIndex;

                    for (int j = 0; j < 4; ++j)
                    {
                        answerButtons[j]->setProperty(
                            "selected",
                            j == answerIndex);

                        answerButtons[j]->style()->unpolish(answerButtons[j]);

                        answerButtons[j]->style()->polish(answerButtons[j]);
                    }
                });
        }

        nextButton =
            makeButton("SUBMIT ANSWER", "goldButton");

        gameLayout->addWidget(nextButton);
        gameLayout->addStretch();

        quizStack->addWidget(quizSelectionPage);
        quizStack->addWidget(quizGamePage);

        outer->addWidget(quizStack, 1);

        connect(
            languageChoice,
            &QComboBox::currentTextChanged,
            this,
            [this]()
            {
                updateQuizSets();
            });

        connect(
            difficultyChoice,
            &QComboBox::currentTextChanged,
            this,
            [this]()
            {
                updateQuizSets();
            });

        connect(
            startButton,
            &QPushButton::clicked,
            this,
            [this, quizStack]()
            {
                if (startQuiz())
                    quizStack->setCurrentIndex(1);
            });

        connect(
            nextButton,
            &QPushButton::clicked,
            this,
            [this, quizStack]()
            {
                if (selectedAnswer < 0)
                {
                    showPopup(
                        "Choose an Answer",
                        "Please select one of the four "
                        "answers before continuing.");
                    return;
                }

                const Question &question =
                    activeQuestions[currentQuestion];

                bool correct =
                    selectedAnswer == question.answer;

                if (correct)
                    currentScore++;

                QString feedback =
                    correct
                        ? "Correct answer!\n\n"
                        : "Not quite!\n\n";

                feedback +=
                    "Correct answer: " +
                    question.options[question.answer] +
                    "\n\n" +
                    question.explanation;

                showPopup(
                    correct ? "Well Done!" : "Keep Learning!",
                    feedback);

                currentQuestion++;

                if (currentQuestion >=
                    activeQuestions.size())
                {
                    finishQuiz();
                    quizStack->setCurrentIndex(0);
                }
                else
                {
                    displayQuestion();
                }
            });

        updateQuizSets();

        return page;
    }

    // Shows the available sets for a difficulty.
    void updateQuizSets()
    {
        if (!languageChoice ||
            !difficultyChoice ||
            !setChoice)
            return;

        int available = 0;

        for (const Question &question : questionBank)
        {
            if (question.language ==
                    languageChoice->currentText() &&
                question.difficulty ==
                    difficultyChoice->currentText())
            {
                available++;
            }
        }

        setChoice->clear();

        // Each set receives a different portion of the bank.
        // More original questions can be added to JSON later.
        int setCount = std::min(3, available);

        for (int i = 0; i < setCount; ++i)
        {
            int count = available / setCount;

            if (i < available % setCount)
                count++;

            setChoice->addItem(
                QString("Set %1 — %2 questions")
                    .arg(i + 1)
                    .arg(count),
                i + 1);
        }
    }

    // Starts a distinct question set.
    bool startQuiz()
    {
        activeLanguage = languageChoice->currentText();
        activeDifficulty = difficultyChoice->currentText();

        if (setChoice->currentIndex() < 0)
        {
            showPopup(
                "No Questions",
                "No questions are available "
                "for this selection.");
            return false;
        }

        currentSet =
            setChoice->currentData().toInt();

        QVector<Question> pool;

        for (const Question &question : questionBank)
        {
            if (question.language == activeLanguage &&
                question.difficulty == activeDifficulty)
            {
                pool.append(question);
            }
        }

        if (pool.isEmpty())
            return false;

        // Split the pool into separate sets.
        // Questions are not repeated across these sets.
        int setCount = std::min(3, int(pool.size()));

        int base = pool.size() / setCount;
        int remainder = pool.size() % setCount;

        int start = 0;

        for (int i = 0; i < currentSet - 1; ++i)
        {
            start += base + (i < remainder ? 1 : 0);
        }

        int count =
            base + (currentSet - 1 < remainder ? 1 : 0);

        activeQuestions.clear();

        for (int i = start; i < start + count; ++i)
            activeQuestions.append(pool[i]);

        // Shuffle only within the selected set.
        for (int i = activeQuestions.size() - 1;
             i > 0; --i)
        {
            int j = QRandomGenerator::global()
                        ->bounded(i + 1);

            activeQuestions.swapItemsAt(i, j);
        }

        currentQuestion = 0;
        currentScore = 0;
        selectedAnswer = -1;

        displayQuestion();

        return true;
    }

    // Displays the next quiz question.
    void displayQuestion()
    {
        if (currentQuestion >= activeQuestions.size())
            return;

        const Question &question =
            activeQuestions[currentQuestion];

        quizHeading->setText(
            activeLanguage + " • " +
            activeDifficulty + " • Set " +
            QString::number(currentSet));

        questionCounter->setText(
            QString("QUESTION %1 OF %2")
                .arg(currentQuestion + 1)
                .arg(activeQuestions.size()));

        quizProgress->setValue(
            (currentQuestion * 100) /
            activeQuestions.size());

        quizProgressText->setText(
            QString("Current score: %1")
                .arg(currentScore));

        questionText->setText(question.question);

        selectedAnswer = -1;

        for (int i = 0; i < 4; ++i)
        {
            answerButtons[i]->setText(
                QString("%1. %2")
                    .arg(QChar('A' + i))
                    .arg(question.options[i]));

            answerButtons[i]->setProperty(
                "selected", false);

            answerButtons[i]->style()->unpolish(answerButtons[i]);

            answerButtons[i]->style()->polish(answerButtons[i]);
        }

        nextButton->setText(
            currentQuestion == activeQuestions.size() - 1
                ? "FINISH QUIZ"
                : "SUBMIT ANSWER");
    }

    // Saves the final score and shows results.
    void finishQuiz()
    {
        QuizResult result;

        result.language = activeLanguage;
        result.difficulty = activeDifficulty;
        result.setNumber = currentSet;
        result.score = currentScore;
        result.total = activeQuestions.size();
        result.date =
            QDateTime::currentDateTime()
                .toString("dd MMM yyyy, hh:mm");

        history.append(result);
        saveData();

        int percentage =
            result.total > 0
                ? (result.score * 100) / result.total
                : 0;

        QString award;

        if (percentage >= 90)
            award = "🏆 Outstanding Performance!";
        else if (percentage >= 70)
            award = "⭐ Great Work!";
        else if (percentage >= 50)
            award = "✓ Good Progress!";
        else
            award = "Keep Practising!";

        showPopup(
            "Quiz Completed!",
            award +
                "\n\n" +
                QString("Score: %1 / %2\nResult: %3%")
                    .arg(result.score)
                    .arg(result.total)
                    .arg(percentage));

        refreshDashboard();
        refreshHistory();
    }

    // Builds the results page.
    QWidget *createHistoryPage()
    {
        auto *page = new QWidget;
        auto *layout = new QVBoxLayout(page);

        layout->setContentsMargins(45, 35, 45, 35);
        layout->setSpacing(16);

        layout->addWidget(
            makeLabel("PERFORMANCE TRACKER", "eyebrow"));

        layout->addWidget(
            makeLabel("Results & History", "pageTitle"));

        historyContent = new QTextBrowser;
        layout->addWidget(historyContent, 1);

        auto *exportButton =
            makeButton("EXPORT HTML REPORT", "goldButton");

        layout->addWidget(exportButton);

        connect(
            exportButton,
            &QPushButton::clicked,
            this,
            [this]()
            {
                QString path =
                    QFileDialog::getSaveFileName(
                        this,
                        "Save CodeQuest Report",
                        "CodeQuest_Report.html",
                        "HTML Files (*.html)");

                if (path.isEmpty())
                    return;

                QFile file(path);

                if (!file.open(QIODevice::WriteOnly |
                               QIODevice::Text))
                {
                    showPopup(
                        "Export Failed",
                        "The report could not be saved.");
                    return;
                }

                QString report =
                    "<!DOCTYPE html><html><head>"
                    "<meta charset='UTF-8'>"
                    "<title>CodeQuest Report</title>"
                    "<style>body{font-family:Arial;"
                    "padding:40px;color:#17263D}"
                    "table{border-collapse:collapse;width:100%}"
                    "th,td{border:1px solid #ddd;"
                    "padding:12px;text-align:left}"
                    "th{background:#D5AE65}</style>"
                    "</head><body>"
                    "<h1>CodeQuest Performance Report</h1>"
                    "<p>Player: " +
                    player.toHtmlEscaped() +
                    "</p><table><tr>"
                    "<th>Date</th><th>Language</th>"
                    "<th>Difficulty</th><th>Set</th>"
                    "<th>Score</th></tr>";

                for (const QuizResult &result : history)
                {
                    report +=
                        "<tr><td>" +
                        result.date.toHtmlEscaped() +
                        "</td><td>" +
                        result.language.toHtmlEscaped() +
                        "</td><td>" +
                        result.difficulty.toHtmlEscaped() +
                        "</td><td>" +
                        QString::number(result.setNumber) +
                        "</td><td>" +
                        QString("%1/%2")
                            .arg(result.score)
                            .arg(result.total) +
                        "</td></tr>";
                }

                report +=
                    "</table><p>Developed by "
                    "Munah Sarah Sameer</p>"
                    "</body></html>";

                file.write(report.toUtf8());
                file.close();

                showPopup(
                    "Report Exported",
                    "Your HTML report has been saved.");
            });

        return page;
    }

    // Refreshes the results history.
    void refreshHistory()
    {
        if (!historyContent)
            return;

        QString html =
            "<h2>Completed Quizzes</h2>";

        if (history.isEmpty())
        {
            html +=
                "<p>No completed quizzes yet. "
                "Start a challenge to see your results.</p>";
        }

        for (int i = history.size() - 1; i >= 0; --i)
        {
            const QuizResult &result = history[i];

            html +=
                "<div style='background:#F7F4ED;"
                "padding:14px;margin-bottom:10px;"
                "border-radius:10px;'>"
                "<b>" +
                result.language.toHtmlEscaped() +
                " — " +
                result.difficulty.toHtmlEscaped() +
                " — Set " +
                QString::number(result.setNumber) +
                "</b><p>Score: " +
                QString("%1/%2")
                    .arg(result.score)
                    .arg(result.total) +
                "</p><small>" +
                result.date.toHtmlEscaped() +
                "</small></div>";
        }

        historyContent->setHtml(html);
    }

    // Builds the player profile page.
    QWidget *createProfilePage()
    {
        auto *page = new QWidget;
        auto *layout = new QVBoxLayout(page);

        layout->setContentsMargins(45, 35, 45, 35);
        layout->setSpacing(20);

        layout->addWidget(
            makeLabel("PLAYER PROFILE", "eyebrow"));

        layout->addWidget(
            makeLabel("Your Coding Journey", "pageTitle"));

        profileText = makeLabel("", "stats");
        layout->addWidget(profileText);

        auto *changePlayer =
            makeButton("CHANGE PLAYER", "goldButton");

        layout->addWidget(changePlayer);

        connect(
            changePlayer,
            &QPushButton::clicked,
            this,
            [this]()
            {
                PlayerWelcome dialog(this);

                if (dialog.exec() != QDialog::Accepted)
                    return;

                player = dialog.playerName();
                avatar = dialog.playerAvatar();

                // New player starts with fresh progress.
                history.clear();
                completedLessons.clear();

                saveData();
                refreshDashboard();
                refreshProfile();
                refreshHistory();

                showPopup(
                    "Welcome to CodeQuest!",
                    "Your new player profile is ready.");
            });

        layout->addStretch();

        return page;
    }

    // Updates the player profile information.
    void refreshProfile()
    {
        if (!profileText)
            return;

        profileText->setText(
            avatar + "\n\n" +
            "Player: " + player + "\n\n" +
            "Completed Lessons: " +
            QString::number(completedLessons.size()) +
            "\n\nCompleted Quizzes: " +
            QString::number(history.size()));
    }

    // Updates the dashboard after each activity.
    void refreshDashboard()
    {
        if (!sidebarPlayer)
            return;

        sidebarPlayer->setText(
            avatar + "\n" + player);

        if (dashboardGreeting)
        {
            dashboardGreeting->setText(
                "Welcome back, " + player + "!");
        }

        if (dashboardStats)
        {
            int totalCorrect = 0;
            int totalQuestions = 0;

            for (const QuizResult &result : history)
            {
                totalCorrect += result.score;
                totalQuestions += result.total;
            }

            int accuracy =
                totalQuestions > 0
                    ? (totalCorrect * 100) /
                          totalQuestions
                    : 0;

            dashboardStats->setText(
                QString(
                    "Lessons Completed: %1 / %2\n\n"
                    "Quizzes Completed: %3\n\n"
                    "Overall Quiz Accuracy: %4%")
                    .arg(completedLessons.size())
                    .arg(lessons.size())
                    .arg(history.size())
                    .arg(accuracy));
        }
    }

    // Applies the premium CodeQuest theme.
    void applyTheme()
    {
        setStyleSheet(R"(
            QMainWindow, QWidget {
                background: #F7F4ED;
                color: #17263D;
                font-family: 'Segoe UI';
                font-size: 14px;
            }

            QWidget#sidebar {
                background: #17263D;
            }

            QLabel#logo {
                color: #D5AE65;
                font-size: 25px;
                font-weight: bold;
            }

            QLabel#playerCard {
                background: #243852;
                color: #FFFFFF;
                padding: 20px;
                border-radius: 14px;
                font-size: 19px;
                font-weight: bold;
            }

            QLabel#sidebarCredit {
                color: #B8C4D2;
                font-size: 11px;
            }

            QLabel#eyebrow {
                color: #B48B43;
                font-size: 12px;
                font-weight: bold;
            }

            QLabel#pageTitle {
                color: #17263D;
                font-size: 29px;
                font-weight: bold;
            }

            QLabel#sectionTitle {
                color: #17263D;
                font-size: 21px;
                font-weight: bold;
            }

            QLabel#questionText {
                background: #FFFFFF;
                color: #17263D;
                border-radius: 14px;
                padding: 24px;
                font-size: 20px;
                font-weight: bold;
            }

            QLabel#stats {
                color: #17263D;
                font-size: 20px;
                font-weight: bold;
                padding: 10px;
            }

            QLabel#popupIcon {
                color: #B48B43;
                font-size: 42px;
            }

            QFrame#card {
                background: #FFFFFF;
                border: 1px solid #E5E0D6;
                border-radius: 16px;
            }

            QPushButton {
                background: #FFFFFF;
                color: #17263D;
                border: 1px solid #D9DEE5;
                border-radius: 10px;
                padding: 10px 14px;
                text-align: left;
            }

            QPushButton:hover {
                border: 2px solid #D5AE65;
            }

            QPushButton#navButton {
                background: #243852;
                color: #FFFFFF;
                border: none;
                text-align: left;
                padding-left: 18px;
                font-weight: bold;
            }

            QPushButton#navButton:hover {
                background: #344B67;
            }

            QPushButton#goldButton {
                background: #D5AE65;
                color: #17263D;
                border: none;
                text-align: center;
                font-weight: bold;
            }

            QPushButton#goldButton:hover {
                background: #E9C780;
            }

            QPushButton#answerButton {
                background: #FFFFFF;
                color: #17263D;
                text-align: left;
                min-height: 48px;
                font-size: 15px;
            }

            QPushButton#answerButton[selected="true"] {
                background: #FFF0CF;
                border: 2px solid #B48B43;
            }

            QComboBox {
                background: #FFFFFF;
                color: #17263D;
                border: 1px solid #D9DEE5;
                border-radius: 9px;
                padding: 12px;
                min-height: 24px;
            }

            QTextBrowser {
                background: #FFFFFF;
                color: #17263D;
                border: 1px solid #E5E0D6;
                border-radius: 12px;
                padding: 15px;
                font-size: 15px;
            }

            QProgressBar {
                background: #E4E8EE;
                border: none;
                border-radius: 7px;
                height: 13px;
            }

            QProgressBar::chunk {
                background: #D5AE65;
                border-radius: 7px;
            }
        )");
    }

public:
    // Prepares the complete desktop application.
    explicit CodeQuest(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        setWindowTitle(
            "CodeQuest 2.0 | Learn, Code & Quiz");

        resize(1350, 850);
        setMinimumSize(1000, 650);

        loadLessons();
        loadQuestions();
        loadData();
        buildInterface();
    }

    // Opens player registration if required.
    bool preparePlayer()
    {
        if (player.trimmed().isEmpty() ||
            player == "Guest")
        {
            PlayerWelcome welcome;

            if (welcome.exec() != QDialog::Accepted)
                return false;

            player = welcome.playerName();
            avatar = welcome.playerAvatar();

            saveData();
            refreshDashboard();
        }

        return true;
    }
};

// Application entry point.
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("CodeQuest");
    app.setOrganizationName("Munah Sarah Sameer");

    CodeQuest window;

    // Register the player before opening the game.
    if (!window.preparePlayer())
        return 0;

    window.show();

    return app.exec();
}
