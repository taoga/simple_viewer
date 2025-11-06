#include "cdlgfind.h"
#include "ui_cdlgfind.h"

CDlgFind::CDlgFind(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgFind)
{
    ui->setupUi(this);
}

CDlgFind::~CDlgFind()
{
    delete ui;
}

QString CDlgFind::GetSequence()
{
    return ui->teSequence->toPlainText();
}

void CDlgFind::SetSequence(QString strSeq)
{
    ui->teSequence->setText( strSeq );
}

bool CDlgFind::IsHex()
{
    return ui->checkHEX->isChecked();
}

void CDlgFind::SetHex(bool bSet)
{
    ui->checkHEX->setChecked( bSet );
}

QString CDlgFind::GetViewBytes()
{
    return ui->leBytesCount->text();
}

void CDlgFind::SetViewBytes(QString strViewBytes)
{
    ui->leBytesCount->setText( strViewBytes );
}

bool CDlgFind::IsBits()
{
    return ui->checkIsBits->isChecked();
}

void CDlgFind::SetBits(bool bSet)
{
    ui->checkIsBits->setChecked( bSet );
}
