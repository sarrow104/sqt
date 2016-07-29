#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGridLayout>
#include <QDebug>

#include "QDialog.hpp"
#include "QFastLayout.hpp"
#include "QPasswordInput.hpp"

// 用户自定义的行为，可以通过function传入……
namespace sqt {
    namespace Dialog {
        WLogin::WLogin(QWidget *parent) :
            QDialog(parent)
        {
            setUpGUI();
            setWindowTitle( tr("User Login") );
            setModal( true );
        }

        void WLogin::setUpGUI(){

            // initialize the username combo box so that it is editable
            comboUsername = new QComboBox( this );
            comboUsername->setEditable( true );
            // initialize the password field so that it does not echo
            // characters
            editPassword = new sqt::QPasswordInput(this);

            // initialize the labels
            labelUsername = new QLabel( this );
            labelPassword = new QLabel( this );
            labelUsername->setText( tr( "Username" ) );
            labelUsername->setBuddy( comboUsername );
            labelPassword->setText( tr( "Password" ) );
            labelPassword->setBuddy( editPassword );

            // initialize buttons
            buttons = new QDialogButtonBox( this );
            buttons->addButton( QDialogButtonBox::Ok );
            buttons->addButton( QDialogButtonBox::Cancel );
            buttons->button( QDialogButtonBox::Ok )->setText( tr("Login") );
            buttons->button( QDialogButtonBox::Cancel )->setText( tr("Abort") );

            // connects slots
            connect( buttons->button( QDialogButtonBox::Cancel ),
                    &QPushButton::clicked,
                    this,
                    &WLogin::close
                   );

            connect( buttons->button( QDialogButtonBox::Ok ),
                    &QPushButton::clicked,
                    this,
                    &WLogin::slotAcceptLogin );

            // set up the layout
            QGridLayout* formGridLayout = new QGridLayout( this );
            // place components into the dialog
            formGridLayout->addWidget( labelUsername,   0, 0 );
            formGridLayout->addWidget( comboUsername,   0, 1 );
            formGridLayout->addWidget( labelPassword,   1, 0 );
            formGridLayout->addWidget( editPassword,    1, 1 );
            formGridLayout->addWidget( buttons,         2, 0, 1, 2 );

            setLayout( formGridLayout );
        }

        void WLogin::setUsername(QString &username){
            bool found = false;
            for( int i = 0; i < comboUsername->count() && ! found ; i++ )
                if( comboUsername->itemText( i ) == username ){
                    comboUsername->setCurrentIndex( i );
                    found = true;
                }

            if( ! found ){
                int index = comboUsername->count();
                qDebug() << "Select username " << index;
                comboUsername->addItem( username );

                comboUsername->setCurrentIndex( index );
            }

            // place the focus on the password field
            editPassword->setFocus();
        }

        void WLogin::setPassword(QString &password){
            editPassword->setText( password );
        }

        void WLogin::slotAcceptLogin(){
            QString username = comboUsername->currentText();
            QString password = editPassword->text();
            int index = comboUsername->currentIndex();

            emit acceptLogin( username,
                             password,
                             index
                            );
            close();
        }

        void WLogin::setUsernamesList(const QStringList &usernames){
            comboUsername->addItems( usernames );
        }
    } // namespace Dialog
} // namespace sqt

