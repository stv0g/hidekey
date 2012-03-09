/********************************************************************************
** Form generated from reading UI file 'hidekey.ui'
**
** Created: Fri Jan 27 00:48:57 2012
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HIDEKEY_H
#define UI_HIDEKEY_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_hidekeyClass
{
public:
    QAction *actionAbout;
    QAction *actionHelp;
    QAction *actionQuit;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QListView *listView;
    QGroupBox *groupBox;
    QFormLayout *formLayout_2;
    QLabel *label;
    QComboBox *comboBox;
    QLabel *label_2;
    QLineEdit *lineEdit;
    QLabel *label_3;
    QLineEdit *lineEdit_2;
    QDialogButtonBox *buttonBox;
    QSpacerItem *verticalSpacer;
    QMenuBar *menubar;
    QMenu *menuMenu;
    QMenu *menu;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *hidekeyClass)
    {
        if (hidekeyClass->objectName().isEmpty())
            hidekeyClass->setObjectName(QString::fromUtf8("hidekeyClass"));
        hidekeyClass->resize(624, 338);
        actionAbout = new QAction(hidekeyClass);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionHelp = new QAction(hidekeyClass);
        actionHelp->setObjectName(QString::fromUtf8("actionHelp"));
        actionQuit = new QAction(hidekeyClass);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        centralwidget = new QWidget(hidekeyClass);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        listView = new QListView(centralwidget);
        listView->setObjectName(QString::fromUtf8("listView"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(listView->sizePolicy().hasHeightForWidth());
        listView->setSizePolicy(sizePolicy);
        listView->setMaximumSize(QSize(150, 16777215));

        horizontalLayout->addWidget(listView);

        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setEnabled(true);
        groupBox->setFlat(false);
        groupBox->setCheckable(false);
        formLayout_2 = new QFormLayout(groupBox);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label);

        comboBox = new QComboBox(groupBox);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, comboBox);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_2);

        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, lineEdit);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_3);

        lineEdit_2 = new QLineEdit(groupBox);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, lineEdit_2);

        buttonBox = new QDialogButtonBox(groupBox);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        formLayout_2->setWidget(4, QFormLayout::FieldRole, buttonBox);

        verticalSpacer = new QSpacerItem(20, 5000, QSizePolicy::Minimum, QSizePolicy::Expanding);

        formLayout_2->setItem(3, QFormLayout::FieldRole, verticalSpacer);


        horizontalLayout->addWidget(groupBox);

        hidekeyClass->setCentralWidget(centralwidget);
        menubar = new QMenuBar(hidekeyClass);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 624, 25));
        menuMenu = new QMenu(menubar);
        menuMenu->setObjectName(QString::fromUtf8("menuMenu"));
        menu = new QMenu(menubar);
        menu->setObjectName(QString::fromUtf8("menu"));
        hidekeyClass->setMenuBar(menubar);
        statusbar = new QStatusBar(hidekeyClass);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        hidekeyClass->setStatusBar(statusbar);
#ifndef QT_NO_SHORTCUT
        label->setBuddy(comboBox);
        label_2->setBuddy(lineEdit);
        label_3->setBuddy(lineEdit_2);
#endif // QT_NO_SHORTCUT

        menubar->addAction(menuMenu->menuAction());
        menubar->addAction(menu->menuAction());
        menuMenu->addAction(actionQuit);
        menu->addAction(actionHelp);
        menu->addAction(actionAbout);

        retranslateUi(hidekeyClass);
        QObject::connect(actionQuit, SIGNAL(activated()), centralwidget, SLOT(close()));

        QMetaObject::connectSlotsByName(hidekeyClass);
    } // setupUi

    void retranslateUi(QMainWindow *hidekeyClass)
    {
        hidekeyClass->setWindowTitle(QApplication::translate("hidekeyClass", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("hidekeyClass", "About", 0, QApplication::UnicodeUTF8));
        actionHelp->setText(QApplication::translate("hidekeyClass", "Help", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("hidekeyClass", "Quit", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QString());
        label->setText(QApplication::translate("hidekeyClass", "Typ", 0, QApplication::UnicodeUTF8));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("hidekeyClass", "Empty", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("hidekeyClass", "String", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("hidekeyClass", "One Time Password", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("hidekeyClass", "Random", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("hidekeyClass", "Random generated", 0, QApplication::UnicodeUTF8)
        );
        label_2->setText(QApplication::translate("hidekeyClass", "User", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("hidekeyClass", "Password", 0, QApplication::UnicodeUTF8));
        menuMenu->setTitle(QApplication::translate("hidekeyClass", "Menu", 0, QApplication::UnicodeUTF8));
        menu->setTitle(QApplication::translate("hidekeyClass", "?", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class hidekeyClass: public Ui_hidekeyClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HIDEKEY_H
