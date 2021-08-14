#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_h

#include <QDialog>
#include <QtGui>

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>

class OptionsDialog : public QDialog
{
    Q_OBJECT
    public:
        OptionsDialog(QWidget *parent = 0,
                      QString browser = "",
                      QString pathDb = "",
                      bool monitoringClipboard = false,
                      int autosaveInterval = 0);
        QString defaultBrowser() const { return editDefBrowser->text(); }
        QString pathToDb() const { return editPathToDB->text(); }
        int autosaveInterval() const { return spinAutosave->value(); }
        bool monitoringClipboard() const { return chkMonitoringClipboard->isChecked(); }
    private slots:
        void okButton();
        void browseFile();
    private:
        QCheckBox *chkMonitoringClipboard;
        QDialogButtonBox *buttonBox;
        QPushButton *browse1;
        QPushButton *browse2;
        QLineEdit *editDefBrowser;
        QLineEdit *editPathToDB;
        QSpinBox *spinAutosave;
};

#endif
