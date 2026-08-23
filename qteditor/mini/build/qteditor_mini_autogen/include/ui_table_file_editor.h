/********************************************************************************
** Form generated from reading UI file 'table_file_editor.ui'
**
** Created by: Qt User Interface Compiler version 5.15.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TABLE_FILE_EDITOR_H
#define UI_TABLE_FILE_EDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_TableFileEditDialog
{
public:
    QGroupBox *IDC_TABLEEDIT_MAIN_GROUP;
    QComboBox *IDC_TABLEEDIT_NAME_LIST;
    QLabel *IDC_STATIC;
    QPushButton *IDC_TABLEEDIT_BTN_PREVIOUS;
    QPushButton *IDC_TABLEEDIT_BTN_NEXT;
    QGroupBox *IDC_STATIC_2;
    QLineEdit *IDC_TABLEEDIT_SCREEN_NAME_EDIT;
    QLabel *IDC_STATIC_3;
    QLabel *IDC_STATIC_4;
    QTextEdit *IDC_TABLEEDIT_DESCRIPTION_EDIT;

    void setupUi(QDialog *IDD_TABLE_FILE_EDITOR)
    {
        if (IDD_TABLE_FILE_EDITOR->objectName().isEmpty())
            IDD_TABLE_FILE_EDITOR->setObjectName(QString::fromUtf8("IDD_TABLE_FILE_EDITOR"));
        IDD_TABLE_FILE_EDITOR->resize(505, 367);
        QFont font;
        font.setFamily(QString::fromUtf8("Liberation Sans"));
        font.setPointSize(8);
        font.setBold(false);
        font.setItalic(false);
        IDD_TABLE_FILE_EDITOR->setFont(font);
        IDD_TABLE_FILE_EDITOR->setEnabled(true);
        IDD_TABLE_FILE_EDITOR->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint);
        IDD_TABLE_FILE_EDITOR->setMinimumSize(QSize(505, 367));
        IDD_TABLE_FILE_EDITOR->setMaximumSize(QSize(505, 367));
        IDC_TABLEEDIT_MAIN_GROUP = new QGroupBox(IDD_TABLE_FILE_EDITOR);
        IDC_TABLEEDIT_MAIN_GROUP->setObjectName(QString::fromUtf8("IDC_TABLEEDIT_MAIN_GROUP"));
        IDC_TABLEEDIT_MAIN_GROUP->setGeometry(QRect(14, 13, 475, 99));
        IDC_TABLEEDIT_MAIN_GROUP->setVisible(true);
        IDC_TABLEEDIT_MAIN_GROUP->setEnabled(true);
        IDC_TABLEEDIT_MAIN_GROUP->setFlat(false);
        IDC_TABLEEDIT_NAME_LIST = new QComboBox(IDC_TABLEEDIT_MAIN_GROUP);
        IDC_TABLEEDIT_NAME_LIST->setObjectName(QString::fromUtf8("IDC_TABLEEDIT_NAME_LIST"));
        IDC_TABLEEDIT_NAME_LIST->setMaxVisibleItems(20);
        IDC_TABLEEDIT_NAME_LIST->setGeometry(QRect(89, 34, 367, 22));
        IDC_TABLEEDIT_NAME_LIST->setVisible(true);
        IDC_TABLEEDIT_NAME_LIST->setEnabled(true);
        IDC_TABLEEDIT_NAME_LIST->setEditable(false);
        IDC_STATIC = new QLabel(IDC_TABLEEDIT_MAIN_GROUP);
        IDC_STATIC->setObjectName(QString::fromUtf8("IDC_STATIC"));
        IDC_STATIC->setGeometry(QRect(14, 37, 67, 13));
        IDC_STATIC->setVisible(true);
        IDC_STATIC->setEnabled(true);
        IDC_STATIC->setAlignment(Qt::AlignLeft);
        IDC_TABLEEDIT_BTN_PREVIOUS = new QPushButton(IDC_TABLEEDIT_MAIN_GROUP);
        IDC_TABLEEDIT_BTN_PREVIOUS->setObjectName(QString::fromUtf8("IDC_TABLEEDIT_BTN_PREVIOUS"));
        IDC_TABLEEDIT_BTN_PREVIOUS->setGeometry(QRect(89, 66, 179, 22));
        IDC_TABLEEDIT_BTN_PREVIOUS->setVisible(true);
        IDC_TABLEEDIT_BTN_PREVIOUS->setEnabled(true);
        IDC_TABLEEDIT_BTN_PREVIOUS->setAutoDefault(false);
        IDC_TABLEEDIT_BTN_NEXT = new QPushButton(IDC_TABLEEDIT_MAIN_GROUP);
        IDC_TABLEEDIT_BTN_NEXT->setObjectName(QString::fromUtf8("IDC_TABLEEDIT_BTN_NEXT"));
        IDC_TABLEEDIT_BTN_NEXT->setGeometry(QRect(277, 66, 179, 22));
        IDC_TABLEEDIT_BTN_NEXT->setVisible(true);
        IDC_TABLEEDIT_BTN_NEXT->setEnabled(true);
        IDC_TABLEEDIT_BTN_NEXT->setAutoDefault(false);
        IDC_STATIC_2 = new QGroupBox(IDD_TABLE_FILE_EDITOR);
        IDC_STATIC_2->setObjectName(QString::fromUtf8("IDC_STATIC_2"));
        IDC_STATIC_2->setGeometry(QRect(16, 121, 475, 227));
        IDC_STATIC_2->setVisible(true);
        IDC_STATIC_2->setEnabled(true);
        IDC_STATIC_2->setFlat(false);
        IDC_TABLEEDIT_SCREEN_NAME_EDIT = new QLineEdit(IDC_STATIC_2);
        IDC_TABLEEDIT_SCREEN_NAME_EDIT->setObjectName(QString::fromUtf8("IDC_TABLEEDIT_SCREEN_NAME_EDIT"));
        IDC_TABLEEDIT_SCREEN_NAME_EDIT->setGeometry(QRect(97, 39, 359, 22));
        IDC_TABLEEDIT_SCREEN_NAME_EDIT->setVisible(true);
        IDC_TABLEEDIT_SCREEN_NAME_EDIT->setEnabled(true);
        IDC_STATIC_3 = new QLabel(IDC_STATIC_2);
        IDC_STATIC_3->setObjectName(QString::fromUtf8("IDC_STATIC_3"));
        IDC_STATIC_3->setGeometry(QRect(14, 43, 73, 13));
        IDC_STATIC_3->setVisible(true);
        IDC_STATIC_3->setEnabled(true);
        IDC_STATIC_3->setAlignment(Qt::AlignLeft);
        IDC_STATIC_4 = new QLabel(IDC_STATIC_2);
        IDC_STATIC_4->setObjectName(QString::fromUtf8("IDC_STATIC_4"));
        IDC_STATIC_4->setGeometry(QRect(16, 78, 61, 13));
        IDC_STATIC_4->setVisible(true);
        IDC_STATIC_4->setEnabled(true);
        IDC_STATIC_4->setAlignment(Qt::AlignLeft);
        IDC_TABLEEDIT_DESCRIPTION_EDIT = new QTextEdit(IDC_STATIC_2);
        IDC_TABLEEDIT_DESCRIPTION_EDIT->setObjectName(QString::fromUtf8("IDC_TABLEEDIT_DESCRIPTION_EDIT"));
        IDC_TABLEEDIT_DESCRIPTION_EDIT->setGeometry(QRect(16, 95, 441, 123));
        IDC_TABLEEDIT_DESCRIPTION_EDIT->setVisible(true);
        IDC_TABLEEDIT_DESCRIPTION_EDIT->setEnabled(true);
        IDC_TABLEEDIT_DESCRIPTION_EDIT->setTabChangesFocus(false);

        retranslateUi(IDD_TABLE_FILE_EDITOR);

        IDC_TABLEEDIT_BTN_PREVIOUS->setDefault(false);
        IDC_TABLEEDIT_BTN_NEXT->setDefault(false);


        QMetaObject::connectSlotsByName(IDD_TABLE_FILE_EDITOR);
    } // setupUi

    void retranslateUi(QDialog *IDD_TABLE_FILE_EDITOR)
    {
        IDD_TABLE_FILE_EDITOR->setWindowTitle(QCoreApplication::translate("TableFileEditDialog", "Table File Text Editor v1.0", nullptr));
        IDC_TABLEEDIT_MAIN_GROUP->setTitle(QCoreApplication::translate("TableFileEditDialog", "Current Generic Object", nullptr));
        IDC_STATIC->setText(QCoreApplication::translate("TableFileEditDialog", "Page Name:", nullptr));
        IDC_TABLEEDIT_BTN_PREVIOUS->setText(QCoreApplication::translate("TableFileEditDialog", " <<  Previous", nullptr));
        IDC_TABLEEDIT_BTN_NEXT->setText(QCoreApplication::translate("TableFileEditDialog", "Next  >> ", nullptr));
        IDC_STATIC_2->setTitle(QCoreApplication::translate("TableFileEditDialog", "Generic Object's Data", nullptr));
        IDC_STATIC_3->setText(QCoreApplication::translate("TableFileEditDialog", "Screen Name:", nullptr));
        IDC_STATIC_4->setText(QCoreApplication::translate("TableFileEditDialog", "Description:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TableFileEditDialog: public Ui_TableFileEditDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TABLE_FILE_EDITOR_H
