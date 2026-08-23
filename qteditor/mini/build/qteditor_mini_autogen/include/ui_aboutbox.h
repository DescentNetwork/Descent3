/********************************************************************************
** Form generated from reading UI file 'aboutbox.ui'
**
** Created by: Qt User Interface Compiler version 5.15.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTBOX_H
#define UI_ABOUTBOX_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_AboutDialog
{
public:
    QLabel *IDC_STATIC;
    QLabel *IDC_STATIC_2;
    QPushButton *IDOK;

    void setupUi(QDialog *AboutDialog)
    {
        if (AboutDialog->objectName().isEmpty())
            AboutDialog->setObjectName(QString::fromUtf8("AboutDialog"));
        AboutDialog->resize(319, 160);
        QFont font;
        font.setFamily(QString::fromUtf8("Liberation Sans"));
        font.setPointSize(8);
        font.setBold(false);
        font.setItalic(false);
        AboutDialog->setFont(font);
        AboutDialog->setEnabled(true);
        AboutDialog->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint);
        AboutDialog->setMinimumSize(QSize(319, 160));
        AboutDialog->setMaximumSize(QSize(319, 160));
        IDC_STATIC = new QLabel(AboutDialog);
        IDC_STATIC->setObjectName(QString::fromUtf8("IDC_STATIC"));
        IDC_STATIC->setWordWrap(true);
        IDC_STATIC->setGeometry(QRect(32, 32, 89, 13));
        IDC_STATIC->setVisible(true);
        IDC_STATIC->setEnabled(true);
        IDC_STATIC->setAlignment(Qt::AlignLeft);
        IDC_STATIC_2 = new QLabel(AboutDialog);
        IDC_STATIC_2->setObjectName(QString::fromUtf8("IDC_STATIC_2"));
        IDC_STATIC_2->setWordWrap(true);
        IDC_STATIC_2->setGeometry(QRect(32, 56, 232, 40));
        IDC_STATIC_2->setVisible(true);
        IDC_STATIC_2->setEnabled(true);
        IDC_STATIC_2->setAlignment(Qt::AlignLeft);
        IDOK = new QPushButton(AboutDialog);
        IDOK->setObjectName(QString::fromUtf8("IDOK"));
        IDOK->setGeometry(QRect(224, 110, 80, 22));
        IDOK->setVisible(true);
        IDOK->setEnabled(true);
        IDOK->setAutoDefault(true);

        retranslateUi(AboutDialog);

        IDOK->setDefault(true);


        QMetaObject::connectSlotsByName(AboutDialog);
    } // setupUi

    void retranslateUi(QDialog *AboutDialog)
    {
        AboutDialog->setWindowTitle(QCoreApplication::translate("AboutDialog", "About Descent 3 Editor", nullptr));
        IDC_STATIC->setText(QCoreApplication::translate("AboutDialog", "D3Edit  version 0.0", nullptr));
        IDC_STATIC_2->setText(QCoreApplication::translate("AboutDialog", "Copyright 1996-1998 Outrage Entertainment, Inc.  All rights reserved.", nullptr));
        IDOK->setText(QCoreApplication::translate("AboutDialog", "OK", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AboutDialog: public Ui_AboutDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTBOX_H
