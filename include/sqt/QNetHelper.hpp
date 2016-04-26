/**
 *  Copyleft(c) 549506937@qq.com
 *  author:sarrow
 *
 *  @file  sqt/QNetHelper.hpp
 *
 *  @brief QNet... Widget helper
 */

#ifndef __QNETHELPER_1460383871__
#define __QNETHELPER_1460383871__

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QtCore>

namespace sqt {
    namespace QNet {
        // 根据需要，选择操作head,get,post ...
        inline QNetworkReply * waitReply(QNetworkAccessManager& manager, const QString& url)
        {
            QEventLoop loop;
            QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
            QObject::connect(reply, &QNetworkReply::finished,
                             &loop, &QEventLoop::quit,
                             Qt::DirectConnection);
            loop.exec();
            return reply;
        }
        inline qint64 headerGetContentLength(QNetworkReply * reply)
        {
            return reply ? reply->header(QNetworkRequest::ContentLengthHeader).toLongLong() : 0;
        }

        inline QString headerGetLocation(QNetworkReply * reply)
        {
            return reply ? reply->header(QNetworkRequest::LocationHeader).toString() : QString("");
        }

        inline QString headerGetErrorString(QNetworkReply * reply)
        {
            return reply ? reply->errorString() : QString("");
        }

        // NOTE
        //
        // 断点下载；需要服务端返回支持206才行！
        inline QNetworkReply * waitReply(QNetworkAccessManager& manager,
                                         const QString& url,
                                         qint64 start_bytes,
                                         qint64 end_bytes)
        {
            QEventLoop loop;
            QNetworkRequest request = QNetworkRequest(QUrl(url));
            QString range = QString("bytes=%1-%2").arg(end_bytes, start_bytes);

#ifdef _SQT_NET_USER_AGENT_
            request.setRawHeader("User-Agent", _SQT_NET_USER_AGENT_);
#endif

            request.setRawHeader("Range", range.toLatin1());

            QNetworkReply *reply = manager.get(request);
            QObject::connect(reply, &QNetworkReply::finished,
                             &loop, &QEventLoop::quit,
                             Qt::DirectConnection);
            loop.exec();
            return reply;
        }

    } // namespace QNet
} // namespace sqt


#endif /* __QNETHELPER_1460383871__ */
