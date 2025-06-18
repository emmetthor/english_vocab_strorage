// Qt includes
#include "addvocab.h"
#include <settings.h>
#include <QObject>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
// IO includes
using namespace std;

AddVocab::AddVocab(QWidget *parent) : QDialog(parent) {
    // 設標題
    setWindowTitle("新增單字");
    // 建立輸入框
    word = new QLineEdit(this);
    pos = new QLineEdit(this);
    definition = new QLineEdit(this);
    example = new QLineEdit(this);
    confirmWord = new QLineEdit(this);
    check = new QPushButton(this);
    // 建立表單
    auto *form = new QFormLayout;
    form -> addRow("單字", word);
    form -> addRow("詞性", pos);
    form -> addRow("意思", definition);
    form -> addRow("例句", example);
    form -> addRow("確認單字", confirmWord);
    form -> addWidget(check);
    setLayout(form);

    QObject::connect(check, &QPushButton::clicked, [this]() {
        sendNewVocab();
    });
}

void AddVocab::sendNewVocab() {
    QString w = word->text().trimmed();
    QString p = pos->text().trimmed();
    QString d = definition->text().trimmed();
    QString e = example->text().trimmed();
    QString c = confirmWord->text().trimmed();
    if (w != c) {
        QMessageBox::warning(this, "錯誤", "單字與確認單字輸入不同，請確認拼字");
        return;
    }
    emit addedVocab(w, p, d, e);
    accept();
}