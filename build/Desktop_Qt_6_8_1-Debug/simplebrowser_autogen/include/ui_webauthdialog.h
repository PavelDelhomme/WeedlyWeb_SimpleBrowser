/********************************************************************************
** Form generated from reading UI file 'webauthdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WEBAUTHDIALOG_H
#define UI_WEBAUTHDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WebAuthDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *m_headingLabel;
    QLabel *m_description;
    QWidget *layoutWidget;
    QVBoxLayout *m_mainVerticalLayout;
    QGroupBox *m_pinGroupBox;
    QLabel *m_pinLabel;
    QLineEdit *m_pinLineEdit;
    QLabel *m_confirmPinLabel;
    QLineEdit *m_confirmPinLineEdit;
    QLabel *m_pinEntryErrorLabel;

    void setupUi(QDialog *WebAuthDialog)
    {
        if (WebAuthDialog->objectName().isEmpty())
            WebAuthDialog->setObjectName("WebAuthDialog");
        WebAuthDialog->resize(563, 397);
        buttonBox = new QDialogButtonBox(WebAuthDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(20, 320, 471, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::Retry);
        m_headingLabel = new QLabel(WebAuthDialog);
        m_headingLabel->setObjectName("m_headingLabel");
        m_headingLabel->setGeometry(QRect(30, 20, 321, 16));
        m_headingLabel->setWordWrap(false);
        m_description = new QLabel(WebAuthDialog);
        m_description->setObjectName("m_description");
        m_description->setGeometry(QRect(30, 60, 491, 31));
        m_description->setWordWrap(false);
        layoutWidget = new QWidget(WebAuthDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(20, 100, 471, 171));
        m_mainVerticalLayout = new QVBoxLayout(layoutWidget);
        m_mainVerticalLayout->setObjectName("m_mainVerticalLayout");
        m_mainVerticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        m_mainVerticalLayout->setContentsMargins(0, 0, 0, 0);
        m_pinGroupBox = new QGroupBox(layoutWidget);
        m_pinGroupBox->setObjectName("m_pinGroupBox");
        m_pinGroupBox->setFlat(true);
        m_pinLabel = new QLabel(m_pinGroupBox);
        m_pinLabel->setObjectName("m_pinLabel");
        m_pinLabel->setGeometry(QRect(10, 20, 58, 16));
        m_pinLineEdit = new QLineEdit(m_pinGroupBox);
        m_pinLineEdit->setObjectName("m_pinLineEdit");
        m_pinLineEdit->setGeometry(QRect(90, 20, 113, 21));
        m_confirmPinLabel = new QLabel(m_pinGroupBox);
        m_confirmPinLabel->setObjectName("m_confirmPinLabel");
        m_confirmPinLabel->setGeometry(QRect(10, 50, 81, 16));
        m_confirmPinLineEdit = new QLineEdit(m_pinGroupBox);
        m_confirmPinLineEdit->setObjectName("m_confirmPinLineEdit");
        m_confirmPinLineEdit->setGeometry(QRect(90, 50, 113, 21));
        m_pinEntryErrorLabel = new QLabel(m_pinGroupBox);
        m_pinEntryErrorLabel->setObjectName("m_pinEntryErrorLabel");
        m_pinEntryErrorLabel->setGeometry(QRect(10, 80, 441, 16));

        m_mainVerticalLayout->addWidget(m_pinGroupBox);


        retranslateUi(WebAuthDialog);

        QMetaObject::connectSlotsByName(WebAuthDialog);
    } // setupUi

    void retranslateUi(QDialog *WebAuthDialog)
    {
        WebAuthDialog->setWindowTitle(QCoreApplication::translate("WebAuthDialog", "Dialog", nullptr));
        m_headingLabel->setText(QCoreApplication::translate("WebAuthDialog", "Heading", nullptr));
        m_description->setText(QCoreApplication::translate("WebAuthDialog", "Description", nullptr));
        m_pinGroupBox->setTitle(QString());
        m_pinLabel->setText(QCoreApplication::translate("WebAuthDialog", "PIN", nullptr));
        m_confirmPinLabel->setText(QCoreApplication::translate("WebAuthDialog", "Confirm PIN", nullptr));
        m_pinEntryErrorLabel->setText(QCoreApplication::translate("WebAuthDialog", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WebAuthDialog: public Ui_WebAuthDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WEBAUTHDIALOG_H
