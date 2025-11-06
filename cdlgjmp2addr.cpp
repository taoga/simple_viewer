#include "cdlgjmp2addr.h"
#include "ui_cdlgjmp2addr.h"

CDlgJmp2Addr::CDlgJmp2Addr(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgJmp2Addr)
{
    ui->setupUi(this);
    ui->leAddr->setFocus();
}

CDlgJmp2Addr::~CDlgJmp2Addr()
{
    delete ui;
}
///////////////////////////////////////////////////////////////////////////////////////
// Enter a hexadecimal number
void CDlgJmp2Addr::setHex( bool bIsHex )
{
    ui->chboxHex->setChecked( bIsHex );
}
///////////////////////////////////////////////////////////////////////////////////////
// Return the address
quint64 CDlgJmp2Addr::getAddr()
{
    if( ui->chboxHex->isChecked() )
        return ui->leAddr->text().toLongLong( nullptr, 16 );    // HEX
    else
        return ui->leAddr->text().toLongLong();                 // DEC
}
