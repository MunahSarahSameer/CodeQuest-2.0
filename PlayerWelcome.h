
#ifndef PLAYERWELCOME_H
#define PLAYERWELCOME_H

/*
 * CodeQuest - Player Registration
 * Developer: Munah Sarah Sameer
 *
 * The player enters a name and selects an avatar
 * before starting the learning game.
 */

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QFrame>
#include <QStringList>

class PlayerWelcome : public QDialog
{
public:
    explicit PlayerWelcome(QWidget *parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("CodeQuest - Player Registration");
        setFixedSize(520, 520);
        setModal(true);

        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(36, 32, 36, 32);
        mainLayout->setSpacing(16);

        // Application branding
        auto *logo = new QLabel("◆ CODEQUEST");
        logo->setObjectName("logo");
        logo->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(logo);

        auto *tagline = new QLabel(
            "LEARN  •  CODE  •  ACHIEVE");
        tagline->setObjectName("tagline");
        tagline->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(tagline);

        mainLayout->addSpacing(10);

        auto *heading = new QLabel(
            "Welcome, Future Developer!");
        heading->setObjectName("heading");
        heading->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(heading);

        auto *description = new QLabel(
            "Create your player profile to begin "
            "your coding adventure.");
        description->setAlignment(Qt::AlignCenter);
        description->setWordWrap(true);
        mainLayout->addWidget(description);

        // Player name
        mainLayout->addWidget(
            new QLabel("PLAYER NAME"));

        nameInput = new QLineEdit;
        nameInput->setPlaceholderText(
            "Enter your name here...");
        nameInput->setMaxLength(35);
        nameInput->setMinimumHeight(46);
        mainLayout->addWidget(nameInput);

        // Player avatar
        mainLayout->addWidget(
            new QLabel("CHOOSE YOUR AVATAR"));

        auto *avatarLayout = new QHBoxLayout;
        avatarLayout->setSpacing(10);

        avatarGroup = new QButtonGroup(this);

        const QStringList avatars = {
            "👩‍💻", "👨‍💻", "🧑‍🚀", "🤖"};

        for (int i = 0; i < avatars.size(); ++i)
        {
            auto *button = new QRadioButton(
                avatars.at(i));

            button->setObjectName("avatar");
            button->setMinimumHeight(70);

            avatarGroup->addButton(button, i);
            avatarLayout->addWidget(button);

            if (i == 0)
                button->setChecked(true);
        }

        mainLayout->addLayout(avatarLayout);

        // Error message appears inside the dialog.
        errorLabel = new QLabel;
        errorLabel->setObjectName("error");
        errorLabel->setAlignment(Qt::AlignCenter);
        errorLabel->setWordWrap(true);
        mainLayout->addWidget(errorLabel);

        mainLayout->addStretch();

        auto *startButton = new QPushButton(
            "START MY ADVENTURE");
        startButton->setObjectName("start");
        startButton->setMinimumHeight(52);
        mainLayout->addWidget(startButton);

        // Validate registration.
        connect(
            startButton,
            &QPushButton::clicked,
            this,
            [this, avatars]()
            {
                QString enteredName =
                    nameInput->text().trimmed();

                if (enteredName.isEmpty())
                {
                    errorLabel->setText(
                        "Please enter your player name.");
                    nameInput->setFocus();
                    return;
                }

                selectedName = enteredName;

                int avatarId =
                    avatarGroup->checkedId();

                if (avatarId < 0)
                    avatarId = 0;

                selectedAvatar =
                    avatars.at(avatarId);

                accept();
            });

        // Allow Enter to start registration.
        connect(
            nameInput,
            &QLineEdit::returnPressed,
            startButton,
            &QPushButton::click);

        setStyleSheet(R"(
            QDialog {
                background-color: #F7F4ED;
            }

            QLabel {
                color: #17263D;
                font-family: 'Segoe UI';
                font-size: 13px;
            }

            QLabel#logo {
                color: #B48B43;
                font-size: 31px;
                font-weight: bold;
            }

            QLabel#tagline {
                color: #7B8795;
                font-size: 11px;
            }

            QLabel#heading {
                color: #17263D;
                font-size: 23px;
                font-weight: bold;
            }

            QLineEdit {
                background: #FFFFFF;
                color: #17263D;
                border: 2px solid #DADFE5;
                border-radius: 10px;
                padding: 10px;
                font-size: 15px;
            }

            QLineEdit:focus {
                border: 2px solid #D5AE65;
            }

            QRadioButton#avatar {
                background: white;
                color: #17263D;
                border: 2px solid #E5E0D6;
                border-radius: 12px;
                padding: 8px;
                font-size: 23px;
            }

            QRadioButton#avatar:checked {
                background: #FFF0CF;
                border: 2px solid #D5AE65;
            }

            QLabel#error {
                color: #B42318;
                font-size: 12px;
            }

            QPushButton#start {
                background: #D5AE65;
                color: #17263D;
                border: none;
                border-radius: 11px;
                font-size: 15px;
                font-weight: bold;
            }

            QPushButton#start:hover {
                background: #E8C47C;
            }

            QPushButton#start:pressed {
                background: #C69A4B;
            }
        )");
    }

    // Returns the registered player name.
    QString playerName() const
    {
        return selectedName;
    }

    // Returns the chosen avatar.
    QString playerAvatar() const
    {
        return selectedAvatar;
    }

private:
    QLineEdit *nameInput = nullptr;
    QLabel *errorLabel = nullptr;
    QButtonGroup *avatarGroup = nullptr;

    QString selectedName;
    QString selectedAvatar;
};

#endif
