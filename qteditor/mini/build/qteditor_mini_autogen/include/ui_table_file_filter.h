/********************************************************************************
** Form generated from reading UI file 'table_file_filter.ui'
**
** Created by: Qt User Interface Compiler version 5.15.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TABLE_FILE_FILTER_H
#define UI_TABLE_FILE_FILTER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_TableFileFilterDialog
{
public:
    QLabel *IDC_STATIC;
    QLineEdit *IDC_OUTPUTFILE_EDIT;
    QLabel *IDC_STATIC_2;
    QListWidget *IDC_TABLEFILTER_LEVEL_LIST;
    QPushButton *IDC_TABLEFILTER_BTN_ADDLEVEL;
    QPushButton *IDC_TABLEFILTER_BTN_REMOVELEVEL;
    QLabel *IDC_STATIC_3;
    QLabel *IDC_TABLEFILTER_NUMPAGESTEXT;
    QTableWidget *IDC_TABLEFILTER_DATA_LIST;
    QPushButton *IDC_TABLEFILTER_BTN_ADDPAGE;
    QPushButton *IDC_TABLEFILTER_BTN_REMOVEPAGE;
    QPushButton *IDC_TABLEFILTER_BTN_CREATENEWTABLEFILE;
    QLabel *IDC_STATIC_4;
    QLabel *IDC_TABLEFILTER_STATUSTEXT;

    void setupUi(QDialog *IDD_TABLE_FILE_FILTER)
    {
        if (IDD_TABLE_FILE_FILTER->objectName().isEmpty())
            IDD_TABLE_FILE_FILTER->setObjectName(QString::fromUtf8("IDD_TABLE_FILE_FILTER"));
        IDD_TABLE_FILE_FILTER->resize(470, 453);
        QFont font;
        font.setFamily(QString::fromUtf8("Liberation Sans"));
        font.setPointSize(8);
        font.setBold(false);
        font.setItalic(false);
        IDD_TABLE_FILE_FILTER->setFont(font);
        IDD_TABLE_FILE_FILTER->setEnabled(true);
        IDD_TABLE_FILE_FILTER->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint);
        IDD_TABLE_FILE_FILTER->setMinimumSize(QSize(470, 453));
        IDD_TABLE_FILE_FILTER->setMaximumSize(QSize(470, 453));
        IDC_STATIC = new QLabel(IDD_TABLE_FILE_FILTER);
        IDC_STATIC->setObjectName(QString::fromUtf8("IDC_STATIC"));
        IDC_STATIC->setGeometry(QRect(11, 11, 221, 13));
        IDC_STATIC->setVisible(true);
        IDC_STATIC->setEnabled(true);
        IDC_STATIC->setAlignment(Qt::AlignLeft);
        IDC_OUTPUTFILE_EDIT = new QLineEdit(IDD_TABLE_FILE_FILTER);
        IDC_OUTPUTFILE_EDIT->setObjectName(QString::fromUtf8("IDC_OUTPUTFILE_EDIT"));
        IDC_OUTPUTFILE_EDIT->setGeometry(QRect(11, 26, 295, 22));
        IDC_OUTPUTFILE_EDIT->setVisible(true);
        IDC_OUTPUTFILE_EDIT->setEnabled(true);
        IDC_STATIC_2 = new QLabel(IDD_TABLE_FILE_FILTER);
        IDC_STATIC_2->setObjectName(QString::fromUtf8("IDC_STATIC_2"));
        IDC_STATIC_2->setGeometry(QRect(11, 65, 154, 13));
        IDC_STATIC_2->setVisible(true);
        IDC_STATIC_2->setEnabled(true);
        IDC_STATIC_2->setAlignment(Qt::AlignLeft);
        IDC_TABLEFILTER_LEVEL_LIST = new QListWidget(IDD_TABLE_FILE_FILTER);
        IDC_TABLEFILTER_LEVEL_LIST->setObjectName(QString::fromUtf8("IDC_TABLEFILTER_LEVEL_LIST"));
        IDC_TABLEFILTER_LEVEL_LIST->setGeometry(QRect(11, 79, 295, 70));
        IDC_TABLEFILTER_LEVEL_LIST->setVisible(true);
        IDC_TABLEFILTER_LEVEL_LIST->setEnabled(true);
        IDC_TABLEFILTER_LEVEL_LIST->setSelectionMode(QAbstractItemView::SingleSelection);
        IDC_TABLEFILTER_BTN_ADDLEVEL = new QPushButton(IDD_TABLE_FILE_FILTER);
        IDC_TABLEFILTER_BTN_ADDLEVEL->setObjectName(QString::fromUtf8("IDC_TABLEFILTER_BTN_ADDLEVEL"));
        IDC_TABLEFILTER_BTN_ADDLEVEL->setGeometry(QRect(324, 79, 134, 26));
        IDC_TABLEFILTER_BTN_ADDLEVEL->setVisible(true);
        IDC_TABLEFILTER_BTN_ADDLEVEL->setEnabled(true);
        IDC_TABLEFILTER_BTN_ADDLEVEL->setAutoDefault(false);
        IDC_TABLEFILTER_BTN_REMOVELEVEL = new QPushButton(IDD_TABLE_FILE_FILTER);
        IDC_TABLEFILTER_BTN_REMOVELEVEL->setObjectName(QString::fromUtf8("IDC_TABLEFILTER_BTN_REMOVELEVEL"));
        IDC_TABLEFILTER_BTN_REMOVELEVEL->setGeometry(QRect(324, 118, 134, 26));
        IDC_TABLEFILTER_BTN_REMOVELEVEL->setVisible(true);
        IDC_TABLEFILTER_BTN_REMOVELEVEL->setEnabled(true);
        IDC_TABLEFILTER_BTN_REMOVELEVEL->setAutoDefault(false);
        IDC_STATIC_3 = new QLabel(IDD_TABLE_FILE_FILTER);
        IDC_STATIC_3->setObjectName(QString::fromUtf8("IDC_STATIC_3"));
        IDC_STATIC_3->setGeometry(QRect(11, 161, 89, 13));
        IDC_STATIC_3->setVisible(true);
        IDC_STATIC_3->setEnabled(true);
        IDC_STATIC_3->setAlignment(Qt::AlignLeft);
        IDC_TABLEFILTER_NUMPAGESTEXT = new QLabel(IDD_TABLE_FILE_FILTER);
        IDC_TABLEFILTER_NUMPAGESTEXT->setObjectName(QString::fromUtf8("IDC_TABLEFILTER_NUMPAGESTEXT"));
        IDC_TABLEFILTER_NUMPAGESTEXT->setGeometry(QRect(107, 161, 59, 13));
        IDC_TABLEFILTER_NUMPAGESTEXT->setVisible(true);
        IDC_TABLEFILTER_NUMPAGESTEXT->setEnabled(true);
        IDC_TABLEFILTER_NUMPAGESTEXT->setAlignment(Qt::AlignLeft);
        IDC_TABLEFILTER_DATA_LIST = new QTableWidget(IDD_TABLE_FILE_FILTER);
        IDC_TABLEFILTER_DATA_LIST->setObjectName(QString::fromUtf8("IDC_TABLEFILTER_DATA_LIST"));
        IDC_TABLEFILTER_DATA_LIST->setGeometry(QRect(11, 180, 448, 209));
        IDC_TABLEFILTER_DATA_LIST->setVisible(true);
        IDC_TABLEFILTER_DATA_LIST->setEnabled(true);
        IDC_TABLEFILTER_DATA_LIST->setSelectionBehavior(QAbstractItemView::SelectRows);
        IDC_TABLEFILTER_DATA_LIST->setSortingEnabled(true);
        IDC_TABLEFILTER_BTN_ADDPAGE = new QPushButton(IDD_TABLE_FILE_FILTER);
        IDC_TABLEFILTER_BTN_ADDPAGE->setObjectName(QString::fromUtf8("IDC_TABLEFILTER_BTN_ADDPAGE"));
        IDC_TABLEFILTER_BTN_ADDPAGE->setGeometry(QRect(11, 401, 102, 22));
        IDC_TABLEFILTER_BTN_ADDPAGE->setVisible(true);
        IDC_TABLEFILTER_BTN_ADDPAGE->setEnabled(true);
        IDC_TABLEFILTER_BTN_ADDPAGE->setAutoDefault(false);
        IDC_TABLEFILTER_BTN_REMOVEPAGE = new QPushButton(IDD_TABLE_FILE_FILTER);
        IDC_TABLEFILTER_BTN_REMOVEPAGE->setObjectName(QString::fromUtf8("IDC_TABLEFILTER_BTN_REMOVEPAGE"));
        IDC_TABLEFILTER_BTN_REMOVEPAGE->setGeometry(QRect(122, 401, 102, 22));
        IDC_TABLEFILTER_BTN_REMOVEPAGE->setVisible(true);
        IDC_TABLEFILTER_BTN_REMOVEPAGE->setEnabled(true);
        IDC_TABLEFILTER_BTN_REMOVEPAGE->setAutoDefault(false);
        IDC_TABLEFILTER_BTN_CREATENEWTABLEFILE = new QPushButton(IDD_TABLE_FILE_FILTER);
        IDC_TABLEFILTER_BTN_CREATENEWTABLEFILE->setObjectName(QString::fromUtf8("IDC_TABLEFILTER_BTN_CREATENEWTABLEFILE"));
        IDC_TABLEFILTER_BTN_CREATENEWTABLEFILE->setGeometry(QRect(285, 401, 173, 22));
        IDC_TABLEFILTER_BTN_CREATENEWTABLEFILE->setVisible(true);
        IDC_TABLEFILTER_BTN_CREATENEWTABLEFILE->setEnabled(true);
        IDC_TABLEFILTER_BTN_CREATENEWTABLEFILE->setAutoDefault(false);
        IDC_STATIC_4 = new QLabel(IDD_TABLE_FILE_FILTER);
        IDC_STATIC_4->setObjectName(QString::fromUtf8("IDC_STATIC_4"));
        IDC_STATIC_4->setGeometry(QRect(11, 436, 51, 13));
        IDC_STATIC_4->setVisible(true);
        IDC_STATIC_4->setEnabled(true);
        IDC_STATIC_4->setAlignment(Qt::AlignLeft);
        IDC_TABLEFILTER_STATUSTEXT = new QLabel(IDD_TABLE_FILE_FILTER);
        IDC_TABLEFILTER_STATUSTEXT->setObjectName(QString::fromUtf8("IDC_TABLEFILTER_STATUSTEXT"));
        IDC_TABLEFILTER_STATUSTEXT->setGeometry(QRect(56, 436, 403, 13));
        IDC_TABLEFILTER_STATUSTEXT->setVisible(true);
        IDC_TABLEFILTER_STATUSTEXT->setEnabled(true);
        IDC_TABLEFILTER_STATUSTEXT->setAlignment(Qt::AlignLeft);

        retranslateUi(IDD_TABLE_FILE_FILTER);

        IDC_TABLEFILTER_BTN_ADDLEVEL->setDefault(false);
        IDC_TABLEFILTER_BTN_REMOVELEVEL->setDefault(false);
        IDC_TABLEFILTER_BTN_ADDPAGE->setDefault(false);
        IDC_TABLEFILTER_BTN_REMOVEPAGE->setDefault(false);
        IDC_TABLEFILTER_BTN_CREATENEWTABLEFILE->setDefault(false);


        QMetaObject::connectSlotsByName(IDD_TABLE_FILE_FILTER);
    } // setupUi

    void retranslateUi(QDialog *IDD_TABLE_FILE_FILTER)
    {
        IDD_TABLE_FILE_FILTER->setWindowTitle(QCoreApplication::translate("TableFileFilterDialog", "Table File Filter v1.0", nullptr));
        IDC_STATIC->setText(QCoreApplication::translate("TableFileFilterDialog", "Location of TableParse Output File:", nullptr));
        IDC_STATIC_2->setText(QCoreApplication::translate("TableFileFilterDialog", "Level filters:", nullptr));
        IDC_TABLEFILTER_BTN_ADDLEVEL->setText(QCoreApplication::translate("TableFileFilterDialog", "Add level(s)...", nullptr));
        IDC_TABLEFILTER_BTN_REMOVELEVEL->setText(QCoreApplication::translate("TableFileFilterDialog", "Remove level(s)", nullptr));
        IDC_STATIC_3->setText(QCoreApplication::translate("TableFileFilterDialog", "Pages to include:", nullptr));
        IDC_TABLEFILTER_NUMPAGESTEXT->setText(QCoreApplication::translate("TableFileFilterDialog", "0", nullptr));
        IDC_TABLEFILTER_BTN_ADDPAGE->setText(QCoreApplication::translate("TableFileFilterDialog", "Add Page...", nullptr));
        IDC_TABLEFILTER_BTN_REMOVEPAGE->setText(QCoreApplication::translate("TableFileFilterDialog", "Remove Page", nullptr));
        IDC_TABLEFILTER_BTN_CREATENEWTABLEFILE->setText(QCoreApplication::translate("TableFileFilterDialog", "Create Filtered Table File...", nullptr));
        IDC_STATIC_4->setText(QCoreApplication::translate("TableFileFilterDialog", "Status:", nullptr));
        IDC_TABLEFILTER_STATUSTEXT->setText(QCoreApplication::translate("TableFileFilterDialog", "Waiting for user...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TableFileFilterDialog: public Ui_TableFileFilterDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TABLE_FILE_FILTER_H
