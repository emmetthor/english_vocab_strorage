#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <addvocab.h>

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardItemModel>
#include <QPushButton>
#include <QString>

class MainWindow : public QMainWindow {
    Q_OBJECT
    QPushButton *btnAddVocab;
    QPushButton *btnPush;
    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
    private slots:
        void openAddVocabPage();
        void writeIntoJson(QString word, QString pos, QString definition, QString example);
        void pushToGithub();
    private:
        QStandardItemModel *vocabTable;
        void loadVocab();
};
#endif // MAINWINDOW_H
