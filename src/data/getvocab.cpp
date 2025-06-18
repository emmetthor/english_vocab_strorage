// Qt includes
#include "getvocab.h"
#include <settings.h>
#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
// IO includes
using namespace std;

QString basePath = QCoreApplication::applicationDirPath();

GetVocab::GetVocab(QObject *parent) : QObject(parent) {
    netManager = new QNetworkAccessManager(this);
}

void GetVocab::fetchData() {
    // 傳送請求並接收
    QUrl databaseUrl("https://raw.githubusercontent.com/emmetthor/english_vocab_strorage/refs/heads/main/DATA.json");
    QNetworkRequest databaseRequest(databaseUrl);
    QNetworkReply *databaseReply = netManager -> get(databaseRequest);
    // 監聽回傳
    QObject::connect(databaseReply, &QNetworkReply::finished, this, &GetVocab::onReplyFinished);
}

void GetVocab::onReplyFinished() {
    // 讀入 databaseReply
    QNetworkReply *databaseReply = qobject_cast<QNetworkReply *>(sender());
    if (!databaseReply) return;
    //建立 data
    QByteArray data = databaseReply->readAll(); 
    databaseReply->deleteLater();
    // 檢查格式轉換
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        emit fetchError("轉檔錯誤 " + parseError.errorString());
        return;
    }
    emit fetchSuccess(tr("已將 RAW 轉成 JSON 檔案"));
    // data/vocabulary.json 位址
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cdUp();
    settings::vocabularyPath = dir.filePath("vocabulary.json");
    // 找到 .json
    QFile vocab(settings::vocabularyPath);
    // 判斷可否開啟
    if (!vocab.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit fetchError(tr("無法開啟 \"vocabulary.json\" ") + vocab.errorString());
        return;
    }
    // 寫入
    vocab.write(jsonDoc.toJson(QJsonDocument::Indented));
    emit fetchSuccess(tr("已讀入單字，共 ") + QString::number(jsonDoc.array().size()) + tr(" 項"));
    // 關檔
    vocab.close();
    // 可以讀入表格資料
    emit fetchFinished();
}