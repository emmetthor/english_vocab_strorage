// Qt includes
#include "mainwindow.h"
#include "getvocab.h"
#include "settings.h"
#include <QObject>
#include <QStandardItemModel>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QJsonObject>
#include <QTableView>
#include <QPushButton>
#include <QProcess>
#include <QVBoxLayout>

// IO includes
using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    qDebug() << tr("中文測試");
    // 抓取單字
    auto vocabFecher = new GetVocab(this);
    connect(vocabFecher, &GetVocab::fetchSuccess, this, [](const QString &what) {
        qDebug() << ">>" << what;
    });
    connect(vocabFecher, &GetVocab::fetchError, this, [](const QString &what) {
        qDebug() << tr(">> 有些地方出錯了: ") << what;
    });
    vocabFecher -> fetchData();
    // 讀入資料
    connect(vocabFecher, &GetVocab::fetchFinished, this, &MainWindow::loadVocab);
    // 寫入 table
    vocabTable = new QStandardItemModel(this);
    vocabTable -> setHorizontalHeaderLabels({tr("單字"), tr("詞性"), tr("意思"), tr("例句")});
    // 展示
    QTableView *vocabView = new QTableView(this);
    vocabView -> setModel(vocabTable);
    setCentralWidget(vocabView);
    resize(800, 400);
    // 新增單字
    btnAddVocab = new QPushButton("新增單字", this);
    QObject::connect(btnAddVocab, &QPushButton::clicked, this, MainWindow::openAddVocabPage);
    // push 至 github
    btnPush = new QPushButton("push to git", this);
    QObject::connect(btnPush, &QPushButton::clicked, this, MainWindow::pushToGithub);
    // layout
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout -> addWidget(btnAddVocab);
    layout -> addWidget(btnPush);
    setCentralWidget(central);
}

void MainWindow::loadVocab() {
    // 讀 vocabulary.json
    QFile vocabFile(settings::vocabularyPath);
    // 確認可否打開
    if (!vocabFile.open(QIODevice::ReadOnly)) {
        qWarning("Failed to open file");
        return;
    }
    // 定義 vocabData
    QByteArray vocabData = vocabFile.readAll();
    // 關檔
    vocabFile.close();
    // 確認可否轉檔
    QJsonDocument vocabDoc = QJsonDocument::fromJson(vocabData);
    if (!vocabDoc.isArray()) {
        qWarning("JSON is not an array");
        return;
    }
    // 遍歷資料
    QJsonArray vocabArr = vocabDoc.array();
    for (const QJsonValue &val : vocabArr) {
        if (!val.isObject()) continue;
        QJsonObject obj = val.toObject();
        // 塞入 table的row
        QList<QStandardItem *> row;
        row.append(new QStandardItem(obj.value("word").toString()));
        row.append(new QStandardItem(obj.value("parts-of-speech").toString()));
        row.append(new QStandardItem(obj.value("definition").toString()));
        row.append(new QStandardItem(obj.value("example").toString()));
        // 塞入 table
        vocabTable -> appendRow(row);
    }
}

void MainWindow::openAddVocabPage() {
    AddVocab *page = new AddVocab(this);
    connect(page, &AddVocab::addedVocab, this, &MainWindow::writeIntoJson);
    page -> exec();
}

void MainWindow::writeIntoJson(QString word, QString pos, QString definition, QString example) {
    QList<QStandardItem *> row;
    row.append(new QStandardItem(word));
    row.append(new QStandardItem(pos));
    row.append(new QStandardItem(definition));
    row.append(new QStandardItem(example));
    // 塞入 table
    vocabTable -> appendRow(row);
    // 讀 vocabulary.json
    QFile vocabFile(settings::vocabularyPath);
    // 確認可否打開
    if (!vocabFile.open(QIODevice::ReadOnly)) {
        qWarning("Failed to open file");
        return;
    }
    // 定義 vocabData
    QByteArray vocabData = vocabFile.readAll();
    // 關檔
    vocabFile.close();
    // 確認可否轉檔
    QJsonDocument vocabDoc = QJsonDocument::fromJson(vocabData);
    if (!vocabDoc.isArray()) {
        qWarning("JSON is not an array");
        return;
    }
    QJsonArray vocabArr = vocabDoc.array();
    // 建立 json
    QJsonObject obj;
    obj["word"] = word;
    obj["parts-of-speech"] = pos;
    obj["definition"] = definition;
    obj["example"] = example;
    vocabArr.append(obj);
    // 存在 json
    if (vocabFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        vocabFile.flush();
        QJsonDocument save(vocabArr);
        vocabFile.write(save.toJson(QJsonDocument::Indented));
    }
    vocabFile.close();
}

void MainWindow::pushToGithub() {
    QProcess gitProcess;

    // 先 commit（這邊假設已經 add 好了）
    gitProcess.start("git", QStringList() << "commit" << "-m" << "自動更新 vocab");
    gitProcess.waitForFinished(-1);
    qDebug() << "Commit output:" << gitProcess.readAllStandardOutput();

    // 再 push
    gitProcess.start("git", QStringList() << "push" << "origin" << "main");
    gitProcess.waitForFinished(-1);
    qDebug() << "Push output:" << gitProcess.readAllStandardOutput();

    // 你可以用 qDebug() 印出錯誤或結果做判斷
    qDebug() << "Push error:" << gitProcess.readAllStandardError();
}

MainWindow::~MainWindow()
{

}