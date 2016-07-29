/**
 *  Copyleft(c) 549506937@qq.com
 *  author:sarrow
 *
 *  @file  sqt/QFastLayout.hpp
 *
 *  @brief FastLayout for qt widget
 */
#ifndef __QFOCUSCHANGECONFIRMWATCH_H_1463299293__
#define __QFOCUSCHANGECONFIRMWATCH_H_1463299293__

#include <sqt/QFocusWatcher.h>

namespace sqt {
    namespace util {
        class QFocusChangeConfirmWatch : public FocusWatcher
        {
            Q_OBJECT;
        public:
            explicit QFocusChangeConfirmWatch(QObject* parent) : FocusWatcher(parent), m_isParentChanged(false)
            {
            }

        public:
            bool eventFilter(QObject *obj, QEvent *event);

        signals:
            void focusChanged(bool in);

        public slots:
            void parentChangedFlag();

        private:
            bool m_isParentChanged;
        };
    } // namespace utility
} // namespace sqt

#endif /* __QFOCUSCHANGECONFIRMWATCH_H_1463299293__ */
