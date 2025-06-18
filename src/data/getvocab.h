#ifndef GETVOCAB_H
#define GETVOCAB_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class GetVocab : public QObject
{
    Q_OBJECT
public:
    explicit GetVocab(QObject *parent = nullptr);
    void fetchData(); // 發送請求

signals:
    void fetchSuccess(const QString &what);
    void fetchError(const QString &what);
    void fetchFinished();
    
private slots:
    void onReplyFinished();

private:
    QNetworkAccessManager *netManager;
};

#endif // GETVOCAB_H