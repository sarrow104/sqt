// QDebugHandler
// NOTE 目的，自定义qDebug() 对象的输出行为；
//
// NOTE 不用额外对象，<QtGlobal> 中的qDebug()函数，本来就有重载形式，可以接受
// QFile, QIODevice 甚至 QString对象，以方便调试消息"重定向"。
//
// 那么，如何达到 Debug,Release 时候不同的行为呢？
// 使用宏即可！
//
// 而且，这应该是最好的办法——因为，我指向控制"重定向"，而不想修改输出时的文字
// 内容！如果自定义函数的话，我还得摸清系统信息规律！
// 当然，如果要仔细调整行为的话，还是要调用 qInstallMessageHandler函数，以使用
// 自己的输出控制函数；
//
// 比如：
//  1. 完全忽略
//  2. 到文件
//  3. 到终端
//  4. 到特定的channel 窗口
// 参考：
//! http://stackoverflow.com/questions/14643293/how-does-qt5-redirect-qdebug-statements-to-the-qt-creator-2-6-console
//! http://stackoverflow.com/questions/22485208/redirect-qdebug-to-qtextedit
//! http://stackoverflow.com/questions/4954140/how-to-redirect-qdebug-qwarning-qcritical-etc-output 
//
// 需要注意的是，
// 1. 需要有一个 void (QtMsgType type, const QMessageLogContext &context, const QString &msg) 类型的全局函数指针；
// 2. 这个函数指针，需要在初始化 QApplication 之前，进行调用；形如：
//
//   qInstallMessageHandler(myMessageOutput);
// 比较好的使用策略是，消息注册形式——因为，作为库的使用者，不可能自己定义好几
// 个这样的函数，然后自己注册一下；
//
// 这里，最好就由库作者定义一个函数；然后该全局函数，访问特定的全局对象——或者隶属于类的static对象；
//
// 该对象，可以对Qt不同的消息级别，还有编译模式——比如是否debug等，来决定具体行为；
//
// 或者，称为监听者模式；
//
// 即，用户可以注册需要监听的对象——比如终端；比如外部文件(提供文件名)，比如具
// 体的text窗口，然后通过append()函数进行；
//
// int QtDebugHandler::regist(QtMsgType type, QString fname); --> 返回注册的槽序列号；
// int QtDebugHandler::regist(QtMsgType type, QtTextEdit * ); --> 返回注册的槽序列号；
//
// TODO
// 打印格式，也可以自定义；
// 定制打印格式化，可以使用QString::arg()，对信息进行封装
//
// 本来是想使用 QSqlFormat的；不过有少许疑问；
// 1. 是否需要数据库链接，才能使用？
// 2. 如何提取出，格式化后的串？
// 3. 绑定名字的方式有限，只能是 :identifier-name；这可能与用户提供的串……
//
// 简单期间，还是使用 QString::arg() 比较方便；
// 有如下几组信息：
//  - grade 消息级别；
//  - msg   消息本身
//  - file  消息发生的文件；
//  - line-num 消息发生的行；
//  - function 消息发生的函数；
//  - 还可以加上时间time；甚至还可以仔细定义strfstring
//
// #include <QSqlQuery>
// QString QSqlQuery::lastQuery() const
//  query.bindValue(":web_id",  r["web_id"]);
// Returns the text of the current query being used, or an empty string if there is no current query text.
// See also executedQuery().
#ifndef __QDEBUGHANDLER_HPP_1469262439__
#define __QDEBUGHANDLER_HPP_1469262439__

#include <cstdio>

#include <vector>
#include <map>
#include <stdexcept>

#include <QMap>

#include <QDebug>
#include <QtCore>
#include <QTextEdit>
#include <QDate>
#include <QTime>

#include <iostream>

namespace sqt {
    class QDebugHandler {
    private:
        QDebugHandler() {}
        ~QDebugHandler() {}

        struct MsgListener {
            enum type_t {
                type_NONE = 0,
                type_CONS = 1,
                type_FILE = 2,
                type_EDIT = 3
            };

            MsgListener()
                : m_type(type_NONE)
            {
            }

            MsgListener(std::FILE * fd)
                : m_type(type_NONE)
            {
                if (fd) {
                    m_file = fd;
                    m_type = type_FILE;
                }
            }

            MsgListener(int fd)
                : m_type(type_NONE)
            {
                // TODO console
                switch (fd) {
                case 1:
                    m_type = type_CONS;
                    m_file = stdout;
                    break;

                case 2:
                    m_type = type_CONS;
                    m_file = stderr;
                    break;
                }
            }

            MsgListener(QTextEdit * p_edit)
                : m_type(type_NONE)
            {
                if (p_edit) {
                    m_edit = p_edit;
                    m_type = type_EDIT;
                }
            }

            void listen(const QString &msg)
            {
                switch (this->m_type) {
                case type_EDIT:
                    if (this->m_edit) {
                        this->m_edit->append(msg);
                    }
                    break;

                case type_CONS:
                case type_FILE:
                    if (this->m_file) {
                        std::fprintf(this->m_file,
                                     "%s\n",
                                     msg.toLocal8Bit().constData());
                        std::fflush(this->m_file);
                    }
                    break;

                default:
                    break;
                }
            }

            ~MsgListener()
            {
                m_type = type_NONE;
                m_file = 0;
            }

            type_t m_type;

            union {
                std::FILE * m_file;
                QTextEdit * m_edit;
            };
        };

        // 资源
        // 管理监听器资源——统一由本类，进行管理；
        // MsgListener只是使用；
        // 因为部分资源，是允许同名的！
        struct ListenerManager {
            // QtMsgType, id, MsgListener
            std::vector<std::map<int, MsgListener> > m_listener_resource;

            // FileName, ref-count, FILE*
            std::map<QString, std::pair<int, std::FILE*> > m_map2cfile;

            // id, QtMsgType
            std::map<int, QtMsgType>  m_resid2msgtype;

            // id, fname
            std::map<int, QString>    m_resid2fname;

            // id
            std::vector<int>            m_default_listener;

            enum FmtElement_t {
                fmtElement_FUNCTION,
                fmtElement_LINENUMBER,
                fmtElement_FILE,
                fmtElement_CATEGORY,
                fmtElement_MESSAGE,
                fmtElement_MSGTYPE,
                fmtElement_MSGTYPE_SHORT,
                fmtElement_DATE,
                fmtElement_TIME,
            };
            std::map<QString, FmtElement_t> m_fmtElentNameIndexs;

            QString                     m_fmt;
            QString                     m_fmt_regstr;
            QString                     m_date_fmt;
            QString                     m_time_fmt;
            std::vector<QStringRef>     m_fmt_strings;
            std::vector<FmtElement_t>   m_fmt_elements;

            int m_res_id;

            ListenerManager()
                : m_res_id(0)
            {
                this->m_fmtElentNameIndexs["function"] = fmtElement_FUNCTION;
                this->m_fmtElentNameIndexs["lineNumber"] = fmtElement_LINENUMBER;
                this->m_fmtElentNameIndexs["file"] = fmtElement_FILE;
                this->m_fmtElentNameIndexs["category"] = fmtElement_CATEGORY;
                this->m_fmtElentNameIndexs["message"] = fmtElement_MESSAGE;
                this->m_fmtElentNameIndexs["msgType"] = fmtElement_MSGTYPE;
                this->m_fmtElentNameIndexs["msgTypeShort"] = fmtElement_MSGTYPE_SHORT;
                this->m_fmtElentNameIndexs["date"] = fmtElement_DATE;
                this->m_fmtElentNameIndexs["time"] = fmtElement_TIME;

                this->m_fmt_regstr.append("{(");
                bool is_first = true;
                for (const auto &item : m_fmtElentNameIndexs) {
                    if (is_first) {
                        is_first = false;
                    }
                    else {
                        this->m_fmt_regstr.append("|");
                    }
                    this->m_fmt_regstr.append(item.first);
                }
                this->m_fmt_regstr.append(")}");

                this->setFormatStr("{message}");
                this->m_date_fmt = "yyyy-dd-MM";
                this->m_time_fmt = "hh:mm:ss.zzz";
            }

            ~ListenerManager()
            {
                this->clear();
            }

            void defaultListener(int fd = 1)
            {
                this->undefaultListener();

                m_default_listener.push_back(this->regist(QtDebugMsg, fd));
                m_default_listener.push_back(this->regist(QtInfoMsg, fd));
                m_default_listener.push_back(this->regist(QtWarningMsg, fd));
                m_default_listener.push_back(this->regist(QtCriticalMsg, fd));
                m_default_listener.push_back(this->regist(QtFatalMsg, fd));
            }

            void undefaultListener()
            {
                for (int id : m_default_listener) {
                    this->unregist(id);
                }
                m_default_listener.clear();
            }

            void setDateFormatStr(QString fmt)
            {
                this->m_date_fmt = fmt;
            }

            void setTimeFormatStr(QString fmt)
            {
                this->m_time_fmt = fmt;
            }

            void ensureMsgLevel(QtMsgType type)
            {
                if (this->m_listener_resource.size() <= type) {
                    this->m_listener_resource.resize(type + 1);
                }
            }

            void setFormatStr(QString fmt)
            {
                this->m_fmt = fmt;
                this->m_fmt_strings.clear();
                this->m_fmt_elements.clear();

                //  - grade 消息级别；
                //  - msg   消息本身
                //  - file  消息发生的文件；
                //  - line-num 消息发生的行；
                //  - function 消息发生的函数；

                QRegularExpression re(this->m_fmt_regstr);
                QRegularExpressionMatchIterator i = re.globalMatch(this->m_fmt);
                int pos = 0;
                while (i.hasNext()) {
                    QRegularExpressionMatch match = i.next();
                    this->m_fmt_strings.push_back(this->m_fmt.midRef(pos, match.capturedStart(0) - pos));
                    this->m_fmt_elements.push_back(this->m_fmtElentNameIndexs[match.captured(1)]);
                    pos = match.capturedEnd(0);
                }
                if (pos < this->m_fmt.length()) {
                    this->m_fmt_strings.push_back(this->m_fmt.midRef(pos));
                }
            }

            void clear()
            {
                for (auto& item : m_map2cfile) {
                    if (!item.second.second) {
                        throw std::runtime_error("null FILE*");
                    }
                    if (!item.second.first) {
                        throw std::runtime_error("null ref-cnt");
                    }
                    std::fclose(item.second.second);
                    item.second.first = 0;
                    item.second.second = 0;
                }
                this->m_listener_resource.clear();
                this->m_map2cfile.clear();
                this->m_resid2fname.clear();
                this->m_resid2fname.clear();
                this->m_default_listener.clear();
                this->m_res_id = 0;
            }

            void clear(QtMsgType type)
            {
                std::vector<int> idToUnregist;
                for (const auto & item : m_listener_resource[type]) {
                    idToUnregist.push_back(item.first);
                }
                for (int id : idToUnregist) {
                    this->unregist(id);
                }
            }

            int genId(QtMsgType type)
            {
                if (m_resid2msgtype.find(m_res_id) != m_resid2msgtype.cend()) {
                    throw std::runtime_error("bad m_res_id");
                }
                m_resid2msgtype[m_res_id] = type;
                return m_res_id++;
            }

            // 1. 将 {identifier} 转换成 %1 形式
            // 2. 使用QString::arg 完成替换，注意下标……
            //
            // 使用流程：
            // 1. 设置格式串——内部完成解析、拆分
            // 2. qDebug() 转向，进入hook函数
            // 3. hook函数中，对特定的listener查看，确保处于isValid()状态
            //    此时，构造消息；将msg传入listener中
            // 4. 循环结束。
            //
            // 即，构造消息过程，只需要完成一次！如果有多个listener的话！
            // 当然，listener其实还太单调，最好是能够接收一个
            // std::function<void(QString)>对象，以便完成任意的转发，比如网络……等等；
            QString genMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
            {
                static const char * qMsgName[] =  {
                    "Debug",    // 0
                    "Warning",  // 1
                    "Critical", // 2
                    "Fatal",    // 3
                    "Info"      // 4
                };

                QString fullMsg;
                for (size_t i = 0; i != this->m_fmt_strings.size(); ++i) {
                    fullMsg.append(this->m_fmt_strings[i]);
                    if (this->m_fmt_elements.size() > i) {
                        switch (this->m_fmt_elements[i]) {
                        case fmtElement_FUNCTION:
                            fullMsg.append(context.function);
                            break;

                        case fmtElement_LINENUMBER:
                            fullMsg.append(context.category);
                            break;

                        case fmtElement_CATEGORY:
                            fullMsg.append(QString::number(context.line, 10));
                            break;

                        case fmtElement_FILE:
                            fullMsg.append(context.file);
                            break;

                        case fmtElement_MESSAGE:
                            fullMsg.append(msg);
                            break;

                        case fmtElement_MSGTYPE:
                            if (sizeof(qMsgName) / sizeof(const char *) > type) {
                                fullMsg.append(qMsgName[type]);
                            }
                            break;

                        case fmtElement_MSGTYPE_SHORT:
                            if (sizeof(qMsgName) / sizeof(const char *) > type) {
                                fullMsg.append(qMsgName[type][0]);
                            }
                            break;

                        case fmtElement_DATE:
                            fullMsg.append(QDate::currentDate().toString(this->m_date_fmt));
                            break;

                        case fmtElement_TIME:
                            fullMsg.append(QTime::currentTime().toString(this->m_time_fmt));
                            break;
                        }
                    }
                }
                return fullMsg;
            }

            void dispatchMsg(QtMsgType type, const QMessageLogContext &context, const QString &msg)
            {
                if (this->m_listener_resource.size() > type) {
                    QString fullMsg;
                    bool    has_genMsg = false;
                    for (auto& item : m_listener_resource[type]) {
                        if (!has_genMsg) {
                            fullMsg = genMessage(type, context, msg);
                            has_genMsg = true;
                        }
                        item.second.listen(fullMsg);
                    }
                }
            }

            template<typename T>
            int registAndGenId(QtMsgType type, T value)
            {
                this->ensureMsgLevel(type);
                int listener_id = this->genId(type);
                m_listener_resource[type][listener_id] = MsgListener(value);
                return listener_id;
            }

            int regist(QtMsgType type, QString fname)
            {
                int listener_id = -1;
                auto it = m_map2cfile.find(fname);
                std::FILE * curFile = 0;
                if (it == m_map2cfile.end()) {
                    curFile = std::fopen(fname.toLocal8Bit().constData(), "wb");
                    if (curFile) {
                        m_map2cfile.insert(it, std::make_pair(fname, std::make_pair(1, curFile)) );
                    }
                }
                else {
                    curFile = it->second.second;
                    it->second.first++; // add refer-count
                }

                if (curFile) {
                    listener_id = registAndGenId(type, curFile);
                    m_resid2fname[listener_id] = fname;
                }

                return listener_id;
            }

            int regist(QtMsgType type, int fd)
            {
                return this->registAndGenId(type, fd);
            }

            int regist(QtMsgType type, QTextEdit * p_edit)
            {
                return this->registAndGenId(type, p_edit);
            }

            bool unregist(int resId)
            {
                bool has_success = false;
                auto it = m_resid2msgtype.find(resId);
                if (it == m_resid2msgtype.end()) {
                    return has_success;
                }
                m_listener_resource[it->second].erase(resId);

                auto it_id2fname = m_resid2fname.find(resId);
                if (it_id2fname != m_resid2fname.end()) {
                    auto & file_cnt = m_map2cfile[it_id2fname->second];
                    if (file_cnt.first <= 0) {
                        throw std::runtime_error("bad ref-count");
                    }
                    if (--file_cnt.first == 0) {
                        std::fclose(file_cnt.second);
                        m_map2cfile.erase(it_id2fname->second);
                    }
                }
                has_success = true;
                return has_success;
            }
        };

    protected:
        static ListenerManager& getListenerManager() {
            static ListenerManager s_listenerManager;
            return s_listenerManager;
        }

    public:
        void clear()
        {
            getListenerManager().clear();
        }
        void clear(QtMsgType type)
        {
            getListenerManager().clear(type);
        }
        static int regist(QtMsgType type, QString fname)
        {
            return getListenerManager().regist(type, fname);
        }
        static int regist(QtMsgType type, int fd)
        {
            return getListenerManager().regist(type, fd);
        }
        static int regist(QtMsgType type, QTextEdit * p_edit)
        {
            return getListenerManager().regist(type, p_edit);
        }

        static void unregist(int id)
        {
            getListenerManager().unregist(id);
        }

        static void install()
        {
            qInstallMessageHandler(QDebugHandler::myMessageOutput);
        }
        static void uninstall()
        {
            qInstallMessageHandler(0);
        }
        static void setDateFormatStr(QString fmt)
        {
            getListenerManager().setDateFormatStr(fmt);
        }

        static void setTimeFormatStr(QString fmt)
        {
            getListenerManager().setTimeFormatStr(fmt);
        }

        static void setFormatStr(QString fmt)
        {
            getListenerManager().setFormatStr(fmt);
        }

    private:
        static void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
        {
            getListenerManager().dispatchMsg(type, context, msg);
        }

    }; // class QDebugHandler
} // namespace sqt


#endif /* __QDEBUGHANDLER_HPP_1469262439__ */
