#ifndef TASKFORM_H
#define TASKFORM_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QBoxLayout>
#include <QGroupBox>
#include <QNetworkAccessManager>
#include <QBuffer>
#include <QMessageBox>
#include <QNetworkReply>

namespace Ui {
class TaskForm;
}

class TaskForm : public QWidget
{
    Q_OBJECT

public:
    explicit TaskForm(QWidget *parent = nullptr);
    ~TaskForm();

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_comboBox_currentIndexChanged(int index);
    void handleUploadFinished(QNetworkReply* reply);

private:
    Ui::TaskForm *ui;
    QNetworkAccessManager* networkAccessManager;
    QNetworkReply * qNetworkReply;
};

#endif // TASKFORM_H
