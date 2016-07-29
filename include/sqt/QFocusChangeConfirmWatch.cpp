#include "QFocusChangeConfirmWatch.h"

#include <QMetaMethod>

#include <QDebug>
#include <QLineEdit>
#include <QComboBox>

namespace sqt {
    namespace util {

        bool QFocusChangeConfirmWatch::eventFilter(QObject *obj, QEvent *event)
        {
            //! http://stackoverflow.com/questions/22838682/checking-that-value-of-ui-elements-has-been-changed

            // QMetaMethod slot = metaObject()->method(
            //     metaObject()->indexOfSlot("textChanged(QString)"));
            // QWidget* w = qobject_cast<QWidget*>(this->parent());
            // const QMetaObject * mo = w->metaObject();
            // if (mo->userProperty().isValid() && mo->userProperty().hasNotifySignal()) {
            //     connect(w, mo->userProperty().notifySignal(), this, slot);
            // }

            Q_UNUSED(obj);
            if (event->type() == QEvent::FocusIn) {
                if (this->parent()) {
                    this->m_isParentChanged = false;
                    if (QLineEdit * w = qobject_cast<QLineEdit*>(this->parent())) {
                        qDebug() << w << __func__ << "connect";
                        connect(w, &QLineEdit::textChanged, this, &QFocusChangeConfirmWatch::parentChangedFlag);
                    }
                    else if (QComboBox * w = qobject_cast<QComboBox*>(this->parent())) {
                        qDebug() << w << __func__ << "connect";
                        connect(w, &QComboBox::currentTextChanged, this, &QFocusChangeConfirmWatch::parentChangedFlag);
                    }
                }
                emit focusChanged(true);
            }
            else if (event->type() == QEvent::FocusOut) {
                if (this->parent()) {
                    if (QLineEdit * w = qobject_cast<QLineEdit*>(this->parent())) {
                        qDebug() << w << __func__ << "disconnect";
                        disconnect(w, &QLineEdit::textChanged, this, &QFocusChangeConfirmWatch::parentChangedFlag);
                    }
                    else if (QComboBox * w = qobject_cast<QComboBox*>(this->parent())) {
                        qDebug() << w << __func__ << "disconnect";
                        disconnect(w, &QComboBox::currentTextChanged, this, &QFocusChangeConfirmWatch::parentChangedFlag);
                    }
                }
                if (this->m_isParentChanged) {
                    this->m_isParentChanged = false;
                    emit focusChanged(false);
                }
            }

            return false;
        }

        void QFocusChangeConfirmWatch::parentChangedFlag()
        {
            this->m_isParentChanged = true;
        }
    } // namespace utility
} // namespace sqt
