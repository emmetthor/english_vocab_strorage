#ifndef ADDVOCAB_H
#define ADDVOCAB_H

#include <QObject>
#include <QDialog>
#include <QLineEdit>

class AddVocab : public QDialog
{
    Q_OBJECT
    public:
        explicit AddVocab(QWidget *parent = nullptr);
    signals:
        void addedVocab(QString word, QString pos, QString definition, QString example);
    private slots:
        void sendNewVocab();
    private:
        QLineEdit *word;
        QLineEdit *pos;
        QLineEdit *definition;
        QLineEdit *example;
        QLineEdit *confirmWord;
        QPushButton *check;
};

#endif // GETVOCAB_H