/********************************************************************************
** Form generated from reading UI file 'selectrange_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELECTRANGE_DIALOG_H
#define UI_SELECTRANGE_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_SelectRangeDialog
{
public:
    QLabel *IDC_STATIC;
    QLineEdit *IDC_LOWER_BOUND_EDIT;
    QPushButton *IDOK;
    QLabel *IDC_STATIC_2;
    QLineEdit *IDC_UPPER_BOUND_EDIT;
    QCheckBox *IDC_SELECT_SLOPE_CHECK;
    QCheckBox *IDC_INVERSE_CHECK;
    QCheckBox *IDC_TEXTURE_GRID_CHECK;
    QLabel *IDC_STATIC_3;
    QLineEdit *IDC_SLOPE_EDIT;
    QCheckBox *IDC_RANDOM_CHECK;
    QCheckBox *IDC_SELECT_CUR_TEXTURE_CHECK;

    void setupUi(QDialog *SelectRangeDialog)
    {
        if (SelectRangeDialog->objectName().isEmpty())
            SelectRangeDialog->setObjectName(QString::fromUtf8("SelectRangeDialog"));
        SelectRangeDialog->resize(298, 165);
        QFont font;
        font.setFamily(QString::fromUtf8("Liberation Sans"));
        font.setPointSize(8);
        font.setBold(false);
        font.setItalic(false);
        SelectRangeDialog->setFont(font);
        SelectRangeDialog->setEnabled(true);
        SelectRangeDialog->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint);
        SelectRangeDialog->setMinimumSize(QSize(298, 165));
        SelectRangeDialog->setMaximumSize(QSize(298, 165));
        IDC_STATIC = new QLabel(SelectRangeDialog);
        IDC_STATIC->setObjectName(QString::fromUtf8("IDC_STATIC"));
        IDC_STATIC->setGeometry(QRect(8, 8, 72, 16));
        IDC_STATIC->setVisible(true);
        IDC_STATIC->setEnabled(true);
        IDC_STATIC->setAlignment(Qt::AlignLeft);
        IDC_LOWER_BOUND_EDIT = new QLineEdit(SelectRangeDialog);
        IDC_LOWER_BOUND_EDIT->setObjectName(QString::fromUtf8("IDC_LOWER_BOUND_EDIT"));
        IDC_LOWER_BOUND_EDIT->setGeometry(QRect(80, 8, 88, 21));
        IDC_LOWER_BOUND_EDIT->setVisible(true);
        IDC_LOWER_BOUND_EDIT->setEnabled(true);
        IDOK = new QPushButton(SelectRangeDialog);
        IDOK->setObjectName(QString::fromUtf8("IDOK"));
        IDOK->setGeometry(QRect(207, 11, 80, 22));
        IDOK->setVisible(true);
        IDOK->setEnabled(true);
        IDOK->setAutoDefault(true);
        IDC_STATIC_2 = new QLabel(SelectRangeDialog);
        IDC_STATIC_2->setObjectName(QString::fromUtf8("IDC_STATIC_2"));
        IDC_STATIC_2->setGeometry(QRect(8, 34, 72, 16));
        IDC_STATIC_2->setVisible(true);
        IDC_STATIC_2->setEnabled(true);
        IDC_STATIC_2->setAlignment(Qt::AlignLeft);
        IDC_UPPER_BOUND_EDIT = new QLineEdit(SelectRangeDialog);
        IDC_UPPER_BOUND_EDIT->setObjectName(QString::fromUtf8("IDC_UPPER_BOUND_EDIT"));
        IDC_UPPER_BOUND_EDIT->setGeometry(QRect(80, 34, 88, 21));
        IDC_UPPER_BOUND_EDIT->setVisible(true);
        IDC_UPPER_BOUND_EDIT->setEnabled(true);
        IDC_SELECT_SLOPE_CHECK = new QCheckBox(SelectRangeDialog);
        IDC_SELECT_SLOPE_CHECK->setObjectName(QString::fromUtf8("IDC_SELECT_SLOPE_CHECK"));
        IDC_SELECT_SLOPE_CHECK->setGeometry(QRect(8, 69, 149, 19));
        IDC_SELECT_SLOPE_CHECK->setVisible(true);
        IDC_SELECT_SLOPE_CHECK->setEnabled(true);
        IDC_INVERSE_CHECK = new QCheckBox(SelectRangeDialog);
        IDC_INVERSE_CHECK->setObjectName(QString::fromUtf8("IDC_INVERSE_CHECK"));
        IDC_INVERSE_CHECK->setGeometry(QRect(160, 69, 104, 19));
        IDC_INVERSE_CHECK->setVisible(true);
        IDC_INVERSE_CHECK->setEnabled(true);
        IDC_TEXTURE_GRID_CHECK = new QCheckBox(SelectRangeDialog);
        IDC_TEXTURE_GRID_CHECK->setObjectName(QString::fromUtf8("IDC_TEXTURE_GRID_CHECK"));
        IDC_TEXTURE_GRID_CHECK->setGeometry(QRect(160, 93, 128, 19));
        IDC_TEXTURE_GRID_CHECK->setVisible(true);
        IDC_TEXTURE_GRID_CHECK->setEnabled(true);
        IDC_STATIC_3 = new QLabel(SelectRangeDialog);
        IDC_STATIC_3->setObjectName(QString::fromUtf8("IDC_STATIC_3"));
        IDC_STATIC_3->setGeometry(QRect(8, 110, 64, 16));
        IDC_STATIC_3->setVisible(true);
        IDC_STATIC_3->setEnabled(true);
        IDC_STATIC_3->setAlignment(Qt::AlignLeft);
        IDC_SLOPE_EDIT = new QLineEdit(SelectRangeDialog);
        IDC_SLOPE_EDIT->setObjectName(QString::fromUtf8("IDC_SLOPE_EDIT"));
        IDC_SLOPE_EDIT->setGeometry(QRect(72, 110, 48, 21));
        IDC_SLOPE_EDIT->setVisible(true);
        IDC_SLOPE_EDIT->setEnabled(false);
        IDC_RANDOM_CHECK = new QCheckBox(SelectRangeDialog);
        IDC_RANDOM_CHECK->setObjectName(QString::fromUtf8("IDC_RANDOM_CHECK"));
        IDC_RANDOM_CHECK->setGeometry(QRect(160, 124, 96, 19));
        IDC_RANDOM_CHECK->setVisible(true);
        IDC_RANDOM_CHECK->setEnabled(true);
        IDC_SELECT_CUR_TEXTURE_CHECK = new QCheckBox(SelectRangeDialog);
        IDC_SELECT_CUR_TEXTURE_CHECK->setObjectName(QString::fromUtf8("IDC_SELECT_CUR_TEXTURE_CHECK"));
        IDC_SELECT_CUR_TEXTURE_CHECK->setGeometry(QRect(160, 146, 136, 19));
        IDC_SELECT_CUR_TEXTURE_CHECK->setVisible(true);
        IDC_SELECT_CUR_TEXTURE_CHECK->setEnabled(true);

        retranslateUi(SelectRangeDialog);

        IDOK->setDefault(true);


        QMetaObject::connectSlotsByName(SelectRangeDialog);
    } // setupUi

    void retranslateUi(QDialog *SelectRangeDialog)
    {
        SelectRangeDialog->setWindowTitle(QCoreApplication::translate("SelectRangeDialog", "Select Range", nullptr));
        IDC_STATIC->setText(QCoreApplication::translate("SelectRangeDialog", "Lower bound:", nullptr));
        IDOK->setText(QCoreApplication::translate("SelectRangeDialog", "OK", nullptr));
        IDC_STATIC_2->setText(QCoreApplication::translate("SelectRangeDialog", "Upper bound:", nullptr));
        IDC_SELECT_SLOPE_CHECK->setText(QCoreApplication::translate("SelectRangeDialog", "Select by height change", nullptr));
        IDC_INVERSE_CHECK->setText(QCoreApplication::translate("SelectRangeDialog", "Invert selection", nullptr));
        IDC_TEXTURE_GRID_CHECK->setText(QCoreApplication::translate("SelectRangeDialog", "Select by texture grid", nullptr));
        IDC_STATIC_3->setText(QCoreApplication::translate("SelectRangeDialog", "Change", nullptr));
        IDC_RANDOM_CHECK->setText(QCoreApplication::translate("SelectRangeDialog", "Random", nullptr));
        IDC_SELECT_CUR_TEXTURE_CHECK->setText(QCoreApplication::translate("SelectRangeDialog", "Select only cur texture", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SelectRangeDialog: public Ui_SelectRangeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTRANGE_DIALOG_H
