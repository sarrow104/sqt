#ifndef __QDIALOG_HPP_1461663340__
#define __QDIALOG_HPP_1461663340__

#include <QDialog>

class QLabel;
class QComboBox;
class QLineEdit;
class QDialogButtonBox;

namespace sqt {
    namespace Dialog {
        class WLogin : public QDialog
        {
            Q_OBJECT;
        private:
            /*!
             * A label for the username component.
             */
            QLabel* labelUsername;

            QLabel* labelPassword;

            QComboBox* comboUsername;

            QLineEdit* editPassword;

            QDialogButtonBox* buttons;

        private:
            void setUpGUI();

        public:
            explicit WLogin(QWidget * parent = 0);

            void setUsername( QString& username );

            void setPassword( QString& password );

            void setUsernamesList( const QStringList& usernames );
        signals:

            /*!
             * A signal emitted when the login is performed.
             * username the username entered in the dialog
             * password the password entered in the dialog
             * index the number of the username selected in the combobox
             */
            void acceptLogin( QString& username, QString& password, int& indexNumber );

        public slots:
            void slotAcceptLogin();

        };
    } // namespace Dialog
} // namespace sqt


#endif /* __QDIALOG_HPP_1461663340__ */
