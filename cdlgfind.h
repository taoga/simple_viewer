#ifndef CDLGFIND_H
#define CDLGFIND_H

#include <QDialog>

namespace Ui {
class CDlgFind;
}

class CDlgFind : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgFind(QWidget *parent = nullptr);
    ~CDlgFind();
    // Return the sequence entered by the user
    QString GetSequence();
    void SetSequence( QString strSeq );
    //
    bool IsHex();
    void SetHex( bool bSet );
    // Return/set the number of bytes to display
    QString GetViewBytes();
    void SetViewBytes( QString strViewBytes );
    //
    bool IsBits();
    void SetBits( bool bSet );

private:
    Ui::CDlgFind *ui;
};

#endif // CDLGFIND_H
