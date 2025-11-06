#ifndef CDLGJMP2ADDR_H
#define CDLGJMP2ADDR_H

#include <QDialog>

namespace Ui {
class CDlgJmp2Addr;
}

class CDlgJmp2Addr : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgJmp2Addr(QWidget *parent = nullptr);
    ~CDlgJmp2Addr();

    void setHex(bool bIsHex);   // Вводить шестнадцатиричное число
    quint64 getAddr();          // Return the address

private:
    Ui::CDlgJmp2Addr *ui;
};

#endif // CDLGJMP2ADDR_H
