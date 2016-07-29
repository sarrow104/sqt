#ifndef __QPASSWORDINPUT_HPP_1461663332__
#define __QPASSWORDINPUT_HPP_1461663332__

#include <QLineEdit>

class QWidget;

namespace sqt {
    class QPasswordInput : public QLineEdit
    {
        Q_OBJECT;
    public:
        QPasswordInput(QWidget * parent = 0) : QLineEdit(parent)
        {
            this->setEchoMode(QLineEdit::Password);
            this->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);
        }
        ~QPasswordInput() = default;
    };
} // namespace sqt


#endif /* __QPASSWORDINPUT_HPP_1461663332__ */
