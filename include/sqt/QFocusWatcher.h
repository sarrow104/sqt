/**
 *  Copyleft(c) 549506937@qq.com
 *  author:sarrow
 *
 *  @file  sqt/QFastLayout.hpp
 *
 *  @brief FastLayout for qt widget
 */
#ifndef __QFOCUSWATCHER_H_1463296084__
#define __QFOCUSWATCHER_H_1463296084__

#include <QWidget>
#include <QEvent>

namespace sqt {
    namespace util {
        class FocusWatcher : public QObject
        {
            Q_OBJECT;
        public:
            explicit FocusWatcher(QObject* parent = nullptr) : QObject(parent)
            {
                if (parent)
                    parent->installEventFilter(this);
            }
            virtual bool eventFilter(QObject *obj, QEvent *event) override
            {
                Q_UNUSED(obj);
                if (event->type() == QEvent::FocusIn)
                    emit focusChanged(true);
                else if (event->type() == QEvent::FocusOut)
                    emit focusChanged(false);

                return false;
            }

            signals:
                void focusChanged(bool in);
        };
    } // namespace utility
} // namespace sqt


#endif /* __QFOCUSWATCHER_H_1463296084__ */
