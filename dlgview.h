#ifndef DLGVIEW_H
#define DLGVIEW_H

#include <QDialog>
#include <vector>
#include "datamodel.h"
#include "DectPktFinder.h"
#include <QTableWidget>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class DlgView; }
QT_END_NAMESPACE

#define		DEFAULT_FIND_BUFFER_SIZE	65536	// Internal buffer size used for sequence search
#define		MAX_BIT_ERRORS				0		// Maximum bit error

class DlgView : public QDialog
{
    Q_OBJECT

public:
    DlgView(QWidget *parent = nullptr);
    DlgView(QString qsDumpFile, QWidget *parent = nullptr);
    ~DlgView();
    // Synchronizing file access
    void LockFile() { m_demData.mtxDemData.lock(); }
    void UnlockFile() { m_demData.mtxDemData.unlock(); }
    QFile* GetDumpFile() { return &(m_demData.qfDumpFile); }         // Displayed file

    static const QString IDS_MENU_VIEW;
    static const QString IDS_MENU_VIEW_DEC;
    static const QString IDS_MENU_VIEW_HEX;
    static const QString IDS_MENU_VIEW_LE;
    static const QString IDS_MENU_VIEW_BE;
    static const QString IDS_MENU_VIEW_BIT_INV;
    static const QString IDS_MENU_VIEW_JAMP2ADDR;
    static const QString IDS_MENU_FIND;
    static const QString IDS_MENU_FIND_PKTS;
    static const QString IDS_MENU_FIND_BT;
    static const QString IDS_MENU_FIND_DECT;
    static const QString IDS_MENU_FIND_COMM;
    static const QString IDS_MENU_FIND_LST;
    static const QString IDS_MENU_FIND_SERIES;
    static const QString IDS_MENU_FIND_NEXT;
    static const QString IDS_MENU_FIND_ALL;
    static const QString IDS_DEM_DATA;
    static const QString IDS_MESSAGE_SEARCH_END;
    static const QString IDS_MESSAGE_ENTER_SEQ;
    static const QString IDS_GRID_LST_PREAMBULA;
    static const QString IDS_GRID_LST_SYNCHRO;
    static const QString IDS_GRID_LST_HEADER;
    static const QString IDS_GRID_LST_LENGTH;
    static const QString IDS_GRID_LST_DATA;
    static const QString IDS_GRID_LST_CRC;
    static const QString IDS_GRID_INFO_CLOCKN;
    static const QString IDS_GRID_INFO_LAP;
    static const QString IDS_GRID_INFO_UAP;
    static const QString IDS_GRID_INFO_NAP;
    static const QString IDS_GRID_INFO_HDR;
    static const QString IDS_GRID_INFO_AC;
    static const QString IDS_GRID_DECT_INFO_STYPE;
    static const QString IDS_GRID_DECT_INFO_RFPI;
    static const QString IDS_GRID_DECT_6_BYTES;
    static const QString IDS_GRID_DECT_STYPE_RFP;
    static const QString IDS_GRID_DECT_STYPE_PP;
    static const QString IDS_GRID_DECT_AFIELD_RCRC;
    static const QString IDS_GRID_DECT_RCRC_OK;
    static const QString IDS_GRID_DECT_RCRC_FAILED;
    static const QString IDS_GRID_DECT_AFIELD_TYPE;
    static const QString IDS_GRID_DECT_ATYPE_CT;
    static const QString IDS_GRID_DECT_ATYPE_NT;
    static const QString IDS_GRID_DECT_ATYPE_QT;
    static const QString IDS_GRID_DECT_ATYPE_ESCAPE;
    static const QString IDS_GRID_DECT_ATYPE_MT;
    static const QString IDS_GRID_DECT_ATYPE_PT;
    static const QString IDS_GRID_DECT_INFO_CRYPT;
    static const QString IDS_GRID_DECT_INFO_YES;
    static const QString IDS_GRID_DECT_INFO_NOT;
    static const QString IDS_GRID_DECT_QT_MF_FORMAT;
    static const QString IDS_GRID_DECT_ATYPE_QT_SSI;
    static const QString IDS_GRID_DECT_ATYPE_QT_ERFC1;
    static const QString IDS_GRID_DECT_ATYPE_QT_FPC;
    static const QString IDS_GRID_DECT_ATYPE_QT_EFPC;
    static const QString IDS_GRID_DECT_ATYPE_QT_SARILC;
    static const QString IDS_GRID_DECT_ATYPE_QT_MFN;
    static const QString IDS_GRID_DECT_ATYPE_QT_ESCAPE;
    static const QString IDS_GRID_DECT_ATYPE_QT_OBSOLETE;
    static const QString IDS_GRID_DECT_ATYPE_QT_ERFC2;
    static const QString IDS_GRID_DECT_ATYPE_QT_RESERV;
    static const QString IDS_GRID_DECT_ATYPE_QT_TI;
    static const QString IDS_GRID_DECT_ATYPE_QT_EFPC2;


protected:
    bool SaveState();           // Save state
    bool RestoreState();        // Restore state
    void UpdateMenuState();     // Refresh menu status
    void CommonInit();          // General initialization
    void UpdateFileSize();      // Update the size of the "linked" file
    void UpdateOffset();        // Update the offset of the current cell
    void Find( bool bFirst );   // General code for searching packets and sequences
    void ReleaseFindBuffer();   // Free up temporary search buffer memory
    void InitModeFind();        // Initialize attributes used when searching for data packets
    bool CreateLog();           // Create a search log file
    void CloseLog(bool bView = false);  // Close the search log file
    void StopFind();            // Complete search
    quint32 FindBytes(quint8 *pBuffer, quint32 ulSize, int *pResult);   // Search for a byte sequence
    void SetOffsetInGridData(); // Set a new offset in the data view table
    void UpdateGridInfo();      // Show the contents of the found data
    void SetGridColumnSize( QTableWidget *pTable, bool bSetColText);    // Set minimum column width
    void SaveSequence(QString qsSequence);                              // Save search string
    QString RestoreSequence();                                          // Restore the search bar
    // Set the contents of a table cell
    void SetGridCell(QTableWidget *pTable, int nRow, int nCol, QString qsText, QColor bkColor = QColor(255, 255, 212), QColor textColor = QColor(0, 0, 0));
    void GetMenuState();        // Get menu state
    void timerEvent(QTimerEvent *event) override;   // Timer event handler

protected slots:

    void onDec();       // Selecting a decimal representation
    void onHex();       // Selecting hexadecimal representation
    void onLE();        // The first bit is the least significant
    void onBE();        // The first bit is the most significant
    void onBitInv();    // Bit inversion
    void onJamp2Addr(); // Go to address
    void onFindPkts();  // Search for packages
    void onDect();      // Search for packages - dect
    void onFindSeries();// Find the sequence
    void onFindNext();  // Find more
    void onFindAll();   // Find all
    void onTableDataClicked(const QModelIndex &);   // Clicking on a cell in the file content display table
private:
    Ui::DlgView *ui;
    QAction             *m_pqaDec,
                        *m_pqaHex,
                        *m_pqaLE,
                        *m_pqaBE,
                        *m_pqaBitInv,
                        *m_pqaJamp2Addr,
                        *m_pqaFindPkts,
                        *m_pqaDect,
                        *m_pqaFindSeries,
                        *m_pqaFindNext,
                        *m_pqaFindAll;
    DemData             m_demData;          // Displayed data
    RemoteModel         m_modelFile;        // Data source for the table
    QString             m_qsDumpFile;       // Name of the file being displayed
    quint64             m_u64Offset;        // Current cell offset
    // Поиск
    vector<quint8>      m_cbaFindSeq;		// Search sequence
    bool                m_bFindAll;         // Continue searching to the end of the data
    quint8              *m_pFindBuffer;		// Temporary buffer
    quint32             m_dwFindBufferSize;	// Temporary buffer size
    quint64             m_u64CurFindPos;	// Current offset in the file to search
    QFile               m_qfLogFile;        // Protocol file
    QTextStream         *m_pqtsLog;         // Streaming I/O to a log file
    quint32             m_nPackets;			// Packet (sequence) counter
    quint32             m_nPrintBytes;		// Number of bytes displayed
    CDectPktFinder      m_DectPktFinder;	// Search for Dect packages
    DectPacket          *m_pDectPacket;		// Contents of the DECT packet
    int                 m_nTimer1Sec;
    QString             m_qsCompany = "TestCompany";
};
#endif // DLGVIEW_H
