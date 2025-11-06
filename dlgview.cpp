#include "dlgview.h"
#include "ui_dlgview.h"
#include <QToolBar>
#include <QMenuBar>
#include <QDebug>
#include <QSettings>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include "cdlgjmp2addr.h"
#include "cdlgfind.h"

// Move to common CStrRes
const QString DlgView::IDS_MENU_VIEW = "&View";
const QString DlgView::IDS_MENU_VIEW_DEC = "&Decimal";
const QString DlgView::IDS_MENU_VIEW_HEX = "&Hexadecimal";
const QString DlgView::IDS_MENU_VIEW_LE = "&LE";
const QString DlgView::IDS_MENU_VIEW_BE = "&BE";
const QString DlgView::IDS_MENU_VIEW_BIT_INV = "&Bit inversion";
const QString DlgView::IDS_MENU_VIEW_JAMP2ADDR = "&Go to address";
const QString DlgView::IDS_MENU_FIND = "&Find";
const QString DlgView::IDS_MENU_FIND_PKTS = "&Packages";
const QString DlgView::IDS_MENU_FIND_DECT = "&Dect";
const QString DlgView::IDS_MENU_FIND_SERIES = "&Sequences";
const QString DlgView::IDS_MENU_FIND_NEXT = "&Next";
const QString DlgView::IDS_MENU_FIND_ALL = "Find &all";
const QString DlgView::IDS_DEM_DATA = "Demodulated data";
const QString DlgView::IDS_MESSAGE_SEARCH_END = "Search completed";
const QString DlgView::IDS_MESSAGE_ENTER_SEQ = "Enter the byte sequence to search.";
const QString DlgView::IDS_GRID_LST_PREAMBULA = "Preamble";
const QString DlgView::IDS_GRID_LST_SYNCHRO = "Synchronic word";
const QString DlgView::IDS_GRID_LST_HEADER = "Headline";
const QString DlgView::IDS_GRID_LST_LENGTH = "Size";
const QString DlgView::IDS_GRID_LST_DATA = "Data";
const QString DlgView::IDS_GRID_LST_CRC = "CRC";
const QString DlgView::IDS_GRID_INFO_CLOCKN = "Time slot";
const QString DlgView::IDS_GRID_INFO_LAP = "LAP";
const QString DlgView::IDS_GRID_INFO_UAP = "UAP";
const QString DlgView::IDS_GRID_INFO_NAP = "NAP";
const QString DlgView::IDS_GRID_INFO_HDR = "Data header";
const QString DlgView::IDS_GRID_INFO_AC = "Access code";
const QString DlgView::IDS_GRID_DECT_INFO_STYPE = "S field type";
const QString DlgView::IDS_GRID_DECT_INFO_RFPI = "RFPI";
const QString DlgView::IDS_GRID_DECT_6_BYTES = "S and A fields";
const QString DlgView::IDS_GRID_DECT_STYPE_RFP = "Base packet";
const QString DlgView::IDS_GRID_DECT_STYPE_PP = "Portable packet";
const QString DlgView::IDS_GRID_DECT_AFIELD_RCRC = "RCRC of field A";
const QString DlgView::IDS_GRID_DECT_RCRC_OK = "Ok!  Recieved: 0x%04X; Calculated: 0x%04X;";
const QString DlgView::IDS_GRID_DECT_RCRC_FAILED = "Error!  Recieved: 0x%04X; Calculated: 0x%04X;";
const QString DlgView::IDS_GRID_DECT_AFIELD_TYPE = "Type of field A";
const QString DlgView::IDS_GRID_DECT_ATYPE_CT = "Ct";
const QString DlgView::IDS_GRID_DECT_ATYPE_NT = "Nt";
const QString DlgView::IDS_GRID_DECT_ATYPE_QT = "Qt";
const QString DlgView::IDS_GRID_DECT_ATYPE_ESCAPE = "Escape";
const QString DlgView::IDS_GRID_DECT_ATYPE_MT = "Mt";
const QString DlgView::IDS_GRID_DECT_ATYPE_PT = "Pt";
const QString DlgView::IDS_GRID_DECT_INFO_CRYPT = "Crypt";
const QString DlgView::IDS_GRID_DECT_INFO_YES = "Yes";
const QString DlgView::IDS_GRID_DECT_INFO_NOT = "Unknown";
const QString DlgView::IDS_GRID_DECT_QT_MF_FORMAT = "Qt multi-frame number: %ld";
const QString DlgView::IDS_GRID_DECT_ATYPE_QT_SSI = "Qt::Static system info";
const QString DlgView::IDS_GRID_DECT_ATYPE_QT_ERFC1 = "Qt::Extended RF Carriers Part 1";
const QString DlgView::IDS_GRID_DECT_ATYPE_QT_FPC = "Qt::Fixed Part Capabilites";
const QString DlgView::IDS_GRID_DECT_ATYPE_QT_EFPC = "Qt::Extended Fixed Part Capabilites";
const QString DlgView::IDS_GRID_DECT_ATYPE_QT_SARILC = "Qt::SARI List Contents";
const QString DlgView::IDS_GRID_DECT_ATYPE_QT_MFN = "Qt::Multi Frame number";
const QString DlgView::IDS_GRID_DECT_ATYPE_QT_ESCAPE = "Qt::Escape";
const QString DlgView::IDS_GRID_DECT_ATYPE_QT_OBSOLETE = "Qt::Obsolete";
const QString DlgView::IDS_GRID_DECT_ATYPE_QT_ERFC2 = "Qt::Extended RF Carriers Part 2";
const QString DlgView::IDS_GRID_DECT_ATYPE_QT_RESERV = "Qt::Reserved";
const QString DlgView::IDS_GRID_DECT_ATYPE_QT_TI = "Qt::Transmit information";
const QString DlgView::IDS_GRID_DECT_ATYPE_QT_EFPC2 = "Qt::Extended Fixed Part Capabilites 2";

DlgView::DlgView(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgView)
{
    ui->setupUi(this);
    // General initialization
    CommonInit();
}
////////////////////////////////////////////////////////////////////////////////////////////
// Constructor for a user-specified file
DlgView::DlgView(QString qsDumpFile, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgView)
{
    ui->setupUi(this);
    // Name of the file to display
    m_qsDumpFile = qsDumpFile;
    // General initialization
    CommonInit();
}

DlgView::~DlgView()
{
    if( m_nTimer1Sec > 0 )
        killTimer( m_nTimer1Sec );

    SaveState();

    CloseLog( false );

    delete ui;
    // Free the viewport buffer
    if( m_demData.pViewBuffer ) delete m_demData.pViewBuffer;

    ReleaseFindBuffer();
}
////////////////////////////////////////////////////////////////////////////////////////////
// General initialization
void DlgView::CommonInit()
{
    m_nPrintBytes = 128;		// Number of bytes displayed
    m_bFindAll = 0;
    m_u64CurFindPos = 0;
    m_nPackets = 0;

    setWindowTitle( DlgView::IDS_DEM_DATA );

    // Create a menu
    // The menu item memory should be automatically freed after the window is closed, since the generic widget is specified.
    QMenuBar *myMenubar = new QMenuBar( this );
    if( myMenubar )
    {
        QString qsStyleSheet = myMenubar->styleSheet();

        QMenu *pmnuView  = myMenubar->addMenu( DlgView::IDS_MENU_VIEW );
        // Добавить команды в меню
        if( pmnuView )
        {
            m_pqaDec = new QAction( DlgView::IDS_MENU_VIEW_DEC , this);
            if( m_pqaDec )
            {
                m_pqaDec->setCheckable( true );
                connect( m_pqaDec, SIGNAL(triggered()), this, SLOT( onDec() ) );
                pmnuView->addAction( m_pqaDec );
            }

            m_pqaHex = new QAction( DlgView::IDS_MENU_VIEW_HEX , this);
            if( m_pqaHex )
            {
                m_pqaHex->setCheckable( true );
                connect( m_pqaHex, SIGNAL(triggered()), this, SLOT( onHex() ) );
                pmnuView->addAction( m_pqaHex );
            }

            m_pqaLE = new QAction( DlgView::IDS_MENU_VIEW_LE , this);
            if( m_pqaLE )
            {
                m_pqaLE->setCheckable( true );
                connect( m_pqaLE, SIGNAL(triggered()), this, SLOT( onLE() ) );

                pmnuView->addSeparator();
                pmnuView->addAction( m_pqaLE );
            }

            m_pqaBE = new QAction( DlgView::IDS_MENU_VIEW_BE , this);
            if( m_pqaBE )
            {
                m_pqaBE->setCheckable( true );
                connect( m_pqaBE, SIGNAL(triggered()), this, SLOT( onBE() ) );
                pmnuView->addAction( m_pqaBE );
            }

            m_pqaBitInv = new QAction( DlgView::IDS_MENU_VIEW_BIT_INV , this);
            if( m_pqaBitInv )
            {
                m_pqaBitInv->setCheckable( true );
                connect( m_pqaBitInv, SIGNAL(triggered()), this, SLOT( onBitInv() ) );

                pmnuView->addSeparator();
                pmnuView->addAction( m_pqaBitInv );
            }

            m_pqaJamp2Addr = new QAction( DlgView::IDS_MENU_VIEW_JAMP2ADDR , this);
            if( m_pqaJamp2Addr )
            {
                connect( m_pqaJamp2Addr, SIGNAL(triggered()), this, SLOT( onJamp2Addr() ) );

                pmnuView->addSeparator();
                pmnuView->addAction( m_pqaJamp2Addr );
            }
        }

        QMenu *pmnuFind  = myMenubar->addMenu( DlgView::IDS_MENU_FIND );

        if( pmnuFind )
        {
            m_pqaFindPkts = new QAction( DlgView::IDS_MENU_FIND_PKTS , this);
            if( m_pqaFindPkts )
            {
                m_pqaFindPkts->setShortcut( QKeySequence("Ctrl+F3"));
                connect( m_pqaFindPkts, SIGNAL(triggered()), this, SLOT( onFindPkts() ) );

                pmnuFind->addAction( m_pqaFindPkts );
            }

            m_pqaDect = new QAction( DlgView::IDS_MENU_FIND_DECT , this);
            if( m_pqaDect )
            {
                connect( m_pqaDect, SIGNAL(triggered()), this, SLOT( onDect() ) );

                m_pqaDect->setCheckable( true );
                pmnuFind->addAction( m_pqaDect );
            }

            m_pqaFindSeries = new QAction( DlgView::IDS_MENU_FIND_SERIES , this);
            if( m_pqaFindSeries )
            {
                m_pqaFindSeries->setShortcut( QKeySequence("Ctrl+F"));
                connect( m_pqaFindSeries, SIGNAL(triggered()), this, SLOT( onFindSeries() ) );

                pmnuFind->addSeparator();
                pmnuFind->addAction( m_pqaFindSeries );
            }

            m_pqaFindNext = new QAction( DlgView::IDS_MENU_FIND_NEXT , this);
            if( m_pqaFindNext )
            {
                m_pqaFindNext->setShortcut( QKeySequence("F3"));
                m_pqaFindNext->setEnabled( false );
                connect( m_pqaFindNext, SIGNAL(triggered()), this, SLOT( onFindNext() ) );

                pmnuFind->addSeparator();
                pmnuFind->addAction( m_pqaFindNext );
            }

            m_pqaFindAll = new QAction( DlgView::IDS_MENU_FIND_ALL , this);
            if( m_pqaFindAll )
            {
                m_pqaFindAll->setShortcut( QKeySequence("F5"));
                connect( m_pqaFindAll, SIGNAL(triggered()), this, SLOT( onFindAll() ) );

                pmnuFind->addSeparator();
                pmnuFind->addAction( m_pqaFindAll );
            }
        }

        ui->verticalLayout->insertWidget( 0, myMenubar );
    }

    RestoreState();
    UpdateMenuState();

    // Initialize the view buffer (window in data) //////////////////////////////////////////////////////////////////////////////////////
    m_demData.u32ViewBufferSize = 65536;
    m_demData.u32ViewBufferFill = 0;
    m_demData.u64ViewBufferOrg = 0;
    m_demData.pViewBuffer = new quint8[m_demData.u32ViewBufferSize];
    // Table initialization /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ui->tblvwData->setSelectionMode(QAbstractItemView::SingleSelection);

    m_modelFile.SetData( &m_demData );
    // Set up a data model
    ui->tblvwData->setModel( &m_modelFile );
    // Set default column widths
    QFontMetrics tmpFontMetric( ui->tblvwData->font() );

    int nWidthCol = tmpFontMetric.horizontalAdvance( "HH" );
    int nCol = 0, nCols = ui->tblvwData->horizontalHeader()->count();

    for( nCol = 0; nCol < nCols; nCol++ )
        ui->tblvwData->horizontalHeader()->resizeSection( nCol, nWidthCol );
    // Click on a cell
    connect( ui->tblvwData, SIGNAL( clicked(const QModelIndex &) ), this, SLOT( onTableDataClicked(const QModelIndex &) ), Qt::QueuedConnection );
    // File initialization /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if( m_qsDumpFile.length() > 0 )
    {// Open file
        m_demData.qfDumpFile.setFileName( m_qsDumpFile );
        if( m_demData.qfDumpFile.open( QIODevice::ReadOnly ) )
            m_demData.u64DataSize = m_demData.qfDumpFile.size();
    }
    // If file name is not specified or file is empty, create a new one
    if( m_qsDumpFile.length() == 0 || m_demData.u64DataSize == 0 )
    {
        if( m_demData.qfDumpFile.isOpen() ) m_demData.qfDumpFile.close();
        m_qsDumpFile = qApp->applicationDirPath();
        m_qsDumpFile += "\\demod_bin.tmp";

        m_demData.qfDumpFile.setFileName( m_qsDumpFile );
        m_demData.qfDumpFile.open( QIODevice::ReadWrite | QIODevice::Truncate );
        m_demData.u64DataSize = 0;
    }
    // Update the size of the "linked" file
    UpdateFileSize();

    m_nTimer1Sec = 0;
    m_nTimer1Sec = startTimer( 1000 );
}

// Clicking on a cell in the file content display table
void DlgView::onTableDataClicked(const QModelIndex &qmiCurCell )
{
    qint64  nRow = qmiCurCell.row(), nCol = qmiCurCell.column();

    if( nRow == -1 || nCol == -1 ) return;

    m_u64Offset = nRow * m_demData.nColCount + nCol;
    UpdateOffset();
}

// Update the offset of the current cell
void DlgView::UpdateOffset()
{
    QString qsTemp;

    if( m_demData.u8DigNotation == DemData::HEX )
    {
        qsTemp = QString::asprintf("%08llX", m_u64Offset );
    }
    else
    if( m_demData.u8DigNotation == DemData::DEC )
    {
        qsTemp = QString::asprintf("%lld", m_u64Offset );
    }

    ui->leOrg->setText( qsTemp );
}

// Update the size of the "linked" file
void DlgView::UpdateFileSize()
{
    QString qsTemp;

    if( m_demData.u8DigNotation == DemData::HEX )
    {
        qsTemp = QString::asprintf("%08llX", m_demData.u64DataSize );
    }
    else
    if( m_demData.u8DigNotation == DemData::DEC )
    {
        qsTemp = QString::asprintf("%lld", m_demData.u64DataSize );
    }

    ui->leSize->setText( qsTemp );
    // Number of full and "incomplete" lines
    quint64 u64RowCount = m_demData.u64DataSize / m_demData.nColCount + ((m_demData.u64DataSize % m_demData.nColCount) ? 1 : 0);

    m_modelFile.SetRowCount( u64RowCount );
}

// Save state
bool DlgView::SaveState()
{
    QSettings qsSettings(m_qsCompany, "OUT_SCRN");

    qsSettings.beginGroup("Settings");

    qsSettings.setValue("geometry", saveGeometry());

    qsSettings.endGroup();

    return true;
}

// Restore state
bool DlgView::RestoreState()
{
    QSettings qsSettings(m_qsCompany, "OUT_SCRN");

    qsSettings.beginGroup("Settings");

    restoreGeometry(qsSettings.value("geometry").toByteArray());

    qsSettings.endGroup();

    return true;
}

// Сохранить строку поиска
void DlgView::SaveSequence( QString qsSequence )
{
    QSettings qsSettings(m_qsCompany, "OUT_SCRN");

    qsSettings.beginGroup("Settings");

    qsSettings.setValue("find_seq", qsSequence );

    qsSettings.endGroup();
}

// Восстановить строку поиска
QString DlgView::RestoreSequence()
{
    QSettings   qsSettings(m_qsCompany, "OUT_SCRN");
    QString     qsTemp;

    qsSettings.beginGroup("Settings");

    qsTemp = qsSettings.value("find_seq").toString();

    qsSettings.endGroup();

    return qsTemp;
}
// Refresh menu status
void DlgView::UpdateMenuState()
{
    m_pqaLE->setChecked( ( m_demData.u8ByteOrder == tagDemData::LE ) ? true : false );
    m_pqaBE->setChecked( ( m_demData.u8ByteOrder == tagDemData::BE ) ? true : false );

    m_pqaDec->setChecked( ( m_demData.u8DigNotation == tagDemData::DEC ) ? true : false );
    m_pqaHex->setChecked( ( m_demData.u8DigNotation == tagDemData::HEX ) ? true : false );

    m_pqaBitInv->setChecked( ( m_demData.u8BitInv > 0 ) ? true : false );

    m_pqaDect->setChecked( (m_demData.u8PktType == tagDemData::DECT) ? true : false );
    // Update file size
    UpdateFileSize();
}

// Получить состояние меню
void DlgView::GetMenuState()
{
    if( m_pqaDect->isChecked() )  m_demData.u8PktType = tagDemData::DECT;
}

// Selecting a decimal representation
void DlgView::onDec()
{
    if( m_pqaDec->isChecked() ) m_demData.u8DigNotation = tagDemData::DEC;
    UpdateMenuState();

    ui->tblvwData->horizontalHeader()->reset();
    ui->tblvwData->verticalHeader()->reset();
    // Refresh table contents
    if( !m_demData.u8IsBit )
        ui->tblvwData->reset();
}

// Selecting hexadecimal representation
void DlgView::onHex()
{
    if( m_pqaHex->isChecked() ) m_demData.u8DigNotation = tagDemData::HEX;
    UpdateMenuState();

    ui->tblvwData->horizontalHeader()->reset();
    ui->tblvwData->verticalHeader()->reset();
    // Refresh table contents
    if( !m_demData.u8IsBit )
        ui->tblvwData->reset();
}

// The first bit is the least significant
void DlgView::onLE()
{
    if( m_pqaLE->isChecked() ) m_demData.u8ByteOrder = tagDemData::LE;
    UpdateMenuState();
    // Update the package information table
    UpdateGridInfo();
}

// The first bit is the most significant
void DlgView::onBE()
{
    if( m_pqaBE->isChecked() ) m_demData.u8ByteOrder = tagDemData::BE;
    UpdateMenuState();
    // Update the package information table
    UpdateGridInfo();
}

// Bit inversion
void DlgView::onBitInv()
{
    if( m_pqaBitInv->isChecked() ) m_demData.u8BitInv = 1; else m_demData.u8BitInv = 0;

    ui->tblvwData->reset();

}

// Go to address
void DlgView::onJamp2Addr()
{
    CDlgJmp2Addr tmpDlg;

    tmpDlg.setHex( m_demData.u8DigNotation == tagDemData::HEX );

    if( tmpDlg.exec() == QDialog::Accepted )
    {
        quint64 u64Addr = tmpDlg.getAddr();
        if( u64Addr < m_demData.u64DataSize )
        {
            quint64 u64Row = u64Addr / m_demData.nColCount; // Line number
            int     nCol = u64Addr % m_demData.nColCount;   // Column number

            QModelIndex idxCell = ui->tblvwData->model()->index(u64Row, nCol);
            // Select the cell with the address
            ui->tblvwData->scrollTo( idxCell );
            ui->tblvwData->selectionModel()->select( idxCell, QItemSelectionModel::ClearAndSelect );

            m_u64Offset = u64Addr;

            UpdateOffset();
        }
    }
}

// Search for packages
void DlgView::onFindPkts()
{
    // Get menu state
    GetMenuState();
    Find( true );
}

// Search for packages - dect
void DlgView::onDect()
{
    if( m_pqaDect->isChecked() ) m_demData.u8PktType = tagDemData::DECT;
    UpdateMenuState();
}

// Find the sequence
void DlgView::onFindSeries()
{
    QString csTemp, csFormat;
    // Show a dialog box to request the information you are looking for
    CDlgFind	tmpDlg;

    m_demData.u8PktType = tagDemData::BYTES;
    // Previous sequence
    csTemp = RestoreSequence();
    tmpDlg.SetSequence( csTemp );

    tmpDlg.SetHex( m_demData.u8DigNotation == tagDemData::HEX );
    // Number of bytes displayed
    csTemp = QString::asprintf("%d", m_nPrintBytes );
    tmpDlg.SetViewBytes( csTemp );
    tmpDlg.SetBits( m_demData.u8IsBit == 1 );

    if( tmpDlg.exec() == QDialog::Accepted )
    {
        bool	bUseHex = tmpDlg.IsHex();
        int		nDigit = 0;
        QStringList     query;
        QRegExp         qrexpTemp("(\\,|\\ |\\;)"); //RegEx for ':' or ','

        m_cbaFindSeq.clear();

        m_demData.u8IsBit = (tmpDlg.IsBits()) ? 1 : 0;

        csTemp = tmpDlg.GetViewBytes();
        nDigit = csTemp.toInt();
        if( nDigit > 0 && nDigit < (DEFAULT_FIND_BUFFER_SIZE / 8) ) m_nPrintBytes = (quint32)nDigit;

        csTemp = tmpDlg.GetSequence();

        if( csTemp.length() > 0 )
        {
            // Parse a string
            query.clear();
            query = csTemp.split( qrexpTemp );

            quint32 nValue = 0, nMax = 0;

            for( int i = 0; i < query.size(); i++ )
            {
                if( bUseHex )
                    nValue = query[i].toUInt( 0, 16 );
                else
                    nValue = query[i].toUInt();

                if( nValue > nMax  ) nMax = nValue;

                m_cbaFindSeq.push_back( nValue & 0xFF );
            }

            if( nMax <= 1 ) m_demData.u8IsBit = true;
        }

        if ( m_cbaFindSeq.size() > 0 )
        {
            // Preserve sequence
            SaveSequence( csTemp );

            Find( true );
        }
        else
        {
            QMessageBox::information( 0, "Find", IDS_MESSAGE_ENTER_SEQ );
        }
    }
}

// Find next
void DlgView::onFindNext()
{
    Find( false );
}

// Find all
void DlgView::onFindAll()
{
    m_bFindAll = true;
    Find( false );
}
/////////////////////////////////////////////////////////////////////////////////
/// General code for searching packets and sequences
void DlgView::Find( bool bFirst )
{
    if ( m_demData.u8PktType == tagDemData::BYTES && m_cbaFindSeq.size() == 0 )
    {
        // Message - nothing to search for
        return;
    }

    if( bFirst)
    {
        m_bFindAll = false;

        UpdateGridInfo();   // If there is no data, the table will be destroyed.
        // Create a log file
        CreateLog();
        if ( m_demData.u8PktType == tagDemData::DECT )
        {
            m_DectPktFinder.SetLogFile( &m_qfLogFile );
        }
    }

    // Block file I/O while searching
    if ( m_demData.qfDumpFile.isOpen() && m_demData.u64DataSize > 0 )
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );

        m_pDectPacket = 0;

        if ( bFirst )
        {
            InitModeFind();
            if ( !m_pFindBuffer || m_dwFindBufferSize < DEFAULT_FIND_BUFFER_SIZE )
            {
                ReleaseFindBuffer();
                m_dwFindBufferSize = DEFAULT_FIND_BUFFER_SIZE;
                m_pFindBuffer = new quint8[m_dwFindBufferSize];
            }
        }

        while ( m_pFindBuffer && m_u64CurFindPos < m_demData.u64DataSize )
        {
            // Read a data block
            quint64 u64Rem = m_demData.u64DataSize - m_u64CurFindPos;
            quint32	nWorked = 0;
            int		nCmpResult = -1;

            if ( u64Rem > m_dwFindBufferSize )
                u64Rem = m_dwFindBufferSize;

            m_demData.mtxDemData.lock();

            m_demData.qfDumpFile.seek( m_u64CurFindPos );

            u64Rem = m_demData.qfDumpFile.read( (char*)m_pFindBuffer, u64Rem );

            m_demData.mtxDemData.unlock();

            if( m_demData.u8IsBit && m_demData.u8BitInv )
            {
                for( quint32 dwIdx = 0; dwIdx < (quint32)u64Rem; dwIdx++ )
                    m_pFindBuffer[dwIdx] = !m_pFindBuffer[dwIdx];
            }

            if ( m_demData.u8PktType == tagDemData::DECT )
            {
                nWorked = m_DectPktFinder.Work( m_pFindBuffer, (quint32)u64Rem, &m_pDectPacket );
                m_u64CurFindPos += nWorked;

                if ( nWorked != u64Rem && m_pDectPacket )
                {
                    // Select packet bits in the table
                    // The packet header is the first bit of the filled FIFO.
                    m_demData.u64StartSelPos = m_u64CurFindPos - CDectPktFinder::FIFO_SIZE;
                    quint32 nSel = 0;
                    if ( m_pDectPacket->nType == S_FIELD_RFP )
                        nSel = AC_LENGTH_RFP;
                    else
                    if ( m_pDectPacket->nType == S_FIELD_PP )
                        nSel = AC_LENGTH_PP;
                    m_demData.u64EndSelPos = m_demData.u64StartSelPos + nSel;
                    // Update offset value
                    m_u64Offset = m_demData.u64StartSelPos;

                    if( !m_bFindAll )
                    {
                        // Update offset value
                        SetOffsetInGridData();
                        // Show package contents in a separate table
                        UpdateGridInfo();
                        break;
                    }
                    else
                        // Show package contents in a separate table
                        UpdateGridInfo();
                }
            }
            else
            {
                nWorked = FindBytes( m_pFindBuffer, (quint32)u64Rem, &nCmpResult );
                m_u64CurFindPos += nWorked;

                if ( !nCmpResult )
                {
                    m_demData.u64StartSelPos = m_u64CurFindPos;
                    m_demData.u64EndSelPos = m_demData.u64StartSelPos + m_cbaFindSeq.size();
                    // Update offset value
                    m_u64Offset = m_demData.u64StartSelPos;
                    // Продолжить искать со следующего байта
                    m_u64CurFindPos++;
                    if( !m_bFindAll )
                    {
                        // Update offset value
                        SetOffsetInGridData();
                        // Show package contents in a separate table
                        UpdateGridInfo();
                        break;
                    }
                    else
                        // Show package contents in a separate table
                        UpdateGridInfo();
                }
                // The sequence search may not reach the end of the data, since there will not be the required amount of data for comparison.
                if ( u64Rem + m_u64CurFindPos >= m_demData.u64DataSize )
                {
                    m_u64CurFindPos = m_demData.u64DataSize;
                    break;
                }
            }
        }

        QApplication::restoreOverrideCursor();

        if ( m_u64CurFindPos >= m_demData.u64DataSize )
        {
            // Search completed
            StopFind();
            QMessageBox::information( 0, "Search", IDS_MESSAGE_SEARCH_END );
        }
        else
        {
            // Allow search to continue
            m_pqaFindNext->setEnabled( true );
        }
    } // end of file and data size check
}
// Free up temporary search buffer memory
void DlgView::ReleaseFindBuffer()
{
    if ( m_pFindBuffer )
    {
        delete m_pFindBuffer;
        m_pFindBuffer = 0;
    }
}

/////////////////////////////////////////////////////////////////////////////////
// Initialize attributes used when searching for data packets
void DlgView::InitModeFind()
{
    m_u64CurFindPos = 0;
    m_demData.u64StartSelPos = 0;
    m_demData.u64EndSelPos = 0;
    m_pqaFindNext->setEnabled( false );
    // Initialize the package search class
    if ( m_demData.u8PktType == tagDemData::DECT )
        m_DectPktFinder.Start();
}

///////////////////////////////////////////////////////////////////////////////////
// Create a search log file
bool DlgView::CreateLog()
{
    bool	bResult = false;
    QString	csOutFile;

    CloseLog();

    // Generate a unique file name
    QDateTime		tmpDateTime = QDateTime::currentDateTime();
    csOutFile = QString::asprintf("OUT_SCRN_%02d_%02d_%02d_%02d_%02d_%02d.log", tmpDateTime.date().year(), tmpDateTime.date().month(), tmpDateTime.date().day(),
            tmpDateTime.time().hour(), tmpDateTime.time().minute(), tmpDateTime.time().second() );

    m_qfLogFile.setFileName( csOutFile );
    m_qfLogFile.open( QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text );
    m_nPackets = 0;
    m_pqtsLog =  new QTextStream( &m_qfLogFile );

    return bResult;
}
///////////////////////////////////////////////////////////////////////////////////
// Close the search log file
void DlgView::CloseLog( bool bView )
{
    if( m_pqtsLog )
    {
        delete m_pqtsLog;
        m_pqtsLog = 0;
    }
    if( m_qfLogFile.isOpen() )
    {
        quint64	u64Length = m_qfLogFile.size();
        QString	csName = m_qfLogFile.fileName();

        m_qfLogFile.close();

        if( u64Length == 0 )	// Delete empty file
            QFile::remove( csName );
        else
        {
            if( bView )
                QDesktopServices::openUrl( QUrl(csName) );
        }
    }
}
// Complete search
void DlgView::StopFind()
{
    // Search completed
    InitModeFind();

    if ( m_demData.u8PktType == tagDemData::DECT)
        m_DectPktFinder.Stop();

    UpdateGridInfo();   // If there is no data, the table will be destroyed.
    m_pqaFindNext->setEnabled( false );

    ReleaseFindBuffer();

    CloseLog( true );
}

///////////////////////////////////////////////////////////////////////////
// Search for a byte sequence
quint32 DlgView::FindBytes( quint8 *pBuffer, quint32 ulSize, int *pResult )
{
    quint32	dwSeqSize = (quint32)m_cbaFindSeq.size(), dwPos = 0, u32BitErrors = 0, u32Idx = 0;
    quint8	*pSeq = m_cbaFindSeq.data(), *pSrc = pBuffer;

    *pResult = -1;
    if ( dwSeqSize > 1 && pSeq )
    {
        while ( dwPos + dwSeqSize <= ulSize )
        {
            // Compare
            u32BitErrors = 0;
            u32Idx = 0;
            quint8 *pCur = pSrc;
            while( u32Idx < dwSeqSize )
            {
                if( *pCur != pSeq[u32Idx] ) u32BitErrors++;
                if( u32BitErrors > MAX_BIT_ERRORS ) break;
                u32Idx++;
                pCur++;
            }

            if( u32BitErrors <= MAX_BIT_ERRORS ) *pResult = 0;

            if( (*pResult) == 0 || dwPos + dwSeqSize > ulSize )
                break;

            dwPos++;
            pSrc++;
        }
    }
    else
        return ulSize;

    return dwPos;
}

/////////////////////////////////////////////////////////////////////////////////
// Set a new offset in the data view table
void DlgView::SetOffsetInGridData()
{
    UpdateOffset();
    quint64 u64End = m_u64Offset + (m_demData.u64EndSelPos - m_demData.u64StartSelPos);
    if( u64End >= m_demData.u64DataSize ) u64End = m_demData.u64DataSize - 1;

    // Set focus to the found block
    quint64 u64Row = u64End / m_demData.nColCount; // Line number
    int     nCol = u64End % m_demData.nColCount;   // Column number

    QModelIndex idxCell = ui->tblvwData->model()->index(u64Row, nCol);
    ui->tblvwData->scrollTo( idxCell );
    // Select the cell with the first bit of the found sequence
    u64Row = m_u64Offset / m_demData.nColCount; // Line number
    nCol = m_u64Offset % m_demData.nColCount;   // Column number

    idxCell = ui->tblvwData->model()->index(u64Row, nCol);
    ui->tblvwData->selectionModel()->select( idxCell, QItemSelectionModel::ClearAndSelect );

    ui->tblvwData->reset();
}
///////////////////////////////////////////////////////////////////////////
// Show the contents of the found data
void DlgView::UpdateGridInfo()
{
    int                 i;
    QString             csLine, csFrag;

    if( m_demData.u64StartSelPos == m_demData.u64EndSelPos )
    {
        int nRows = ui->tblwgtExtInfo->rowCount();
        if( nRows > 0)
        {
            nRows--;
            while( nRows >= 0 )
            {
                ui->tblwgtExtInfo->removeRow( nRows );
                nRows--;
            }
        }
        return;
    }

    if ( m_demData.u8PktType == tagDemData::DECT && m_pDectPacket )
    {
        QString csTemp, csFormat;
        quint8	nCrypt = 0;

        m_nPackets++;

        // Show additional information about the found package
        ui->tblwgtExtInfo->horizontalHeader()->hide();
        ui->tblwgtExtInfo->verticalHeader()->hide();
        // Set the number of lines to display if you want to add one more under the incomplete line and one under the heading
        ui->tblwgtExtInfo->setRowCount( 6 );
        // Add one column for addresses
        ui->tblwgtExtInfo->setColumnCount( 2 );
        // Offset, current packet number
        if( m_demData.u8DigNotation == tagDemData::HEX )
            csLine = QString::asprintf("%08llX (%d):\n",  m_u64Offset, m_nPackets );
        else
            csLine = QString::asprintf("%08llu (%d):\n",  m_u64Offset, m_nPackets );
        if( m_pqtsLog ) *m_pqtsLog << csLine;
        csLine.clear();

        // Type S fields
        csLine = IDS_GRID_DECT_INFO_STYPE;
        SetGridCell( ui->tblwgtExtInfo, 0, 0, csLine, m_demData.fixCellBkGround );

        csLine += ": ";
        csTemp.clear();
        if ( m_pDectPacket->nType == S_FIELD_RFP )
            csTemp = IDS_GRID_DECT_STYPE_RFP;
        else
        if ( m_pDectPacket->nType == S_FIELD_PP )
            csTemp = IDS_GRID_DECT_STYPE_PP;
        SetGridCell( ui->tblwgtExtInfo, 0, 1, csTemp );

        csLine += csTemp;
        csLine += "\n";
        if( m_pqtsLog ) *m_pqtsLog << csLine;


        // The meaning of RFPI
        csLine = IDS_GRID_DECT_INFO_RFPI;
        SetGridCell( ui->tblwgtExtInfo, 1, 0, csLine, m_demData.fixCellBkGround );

        csLine += ": ";

        csTemp = QString::asprintf("0x%010llX", m_pDectPacket->u64RFPI );
        SetGridCell( ui->tblwgtExtInfo, 1, 1, csTemp );

        csLine += csTemp;
        csLine += "\n";
        if( m_pqtsLog ) *m_pqtsLog << csLine;
        csLine.clear();

        // Contents of fields S and A
        csLine = IDS_GRID_DECT_6_BYTES;
        SetGridCell( ui->tblwgtExtInfo, 2, 0, csLine, m_demData.fixCellBkGround );
        csTemp.clear();
        for ( i = 0; i < 12; i++ )
        {
            csFormat = QString::asprintf( "%02X ", m_pDectPacket->pucBits[i] );
            csTemp += csFormat;
        }
        SetGridCell( ui->tblwgtExtInfo, 2, 1, csTemp );
        // Save to log
        csLine += ": ";
        csLine += csTemp;
        csLine += "\n";
        if( m_pqtsLog ) *m_pqtsLog << csLine;
        csLine.clear();

        // Checksum of field A
        csLine = IDS_GRID_DECT_AFIELD_RCRC;
        SetGridCell( ui->tblwgtExtInfo, 3, 0, csLine, m_demData.fixCellBkGround );

        if ( m_pDectPacket->wRCRC == m_pDectPacket->wRCRC_Calc )
            csFormat = IDS_GRID_DECT_RCRC_OK;
        else
            csFormat = IDS_GRID_DECT_RCRC_FAILED;
        csTemp = QString::asprintf( csFormat.toStdString().data(),  m_pDectPacket->wRCRC, m_pDectPacket->wRCRC_Calc );

        csLine += ": ";
        csLine += csTemp;
        csLine += "\n";
        if( m_pqtsLog ) *m_pqtsLog << csLine;
        csLine.clear();

        SetGridCell( ui->tblwgtExtInfo, 3, 1, csTemp );
        // Message Type A Fields
        csLine = IDS_GRID_DECT_AFIELD_TYPE;
        SetGridCell( ui->tblwgtExtInfo, 4, 0, csLine, m_demData.fixCellBkGround );

        csLine += ": ";
        csTemp.clear();
        if ( m_pDectPacket->nATailType == 0 || m_pDectPacket->nATailType == 1 )
        {
            // Ct message
            csTemp = IDS_GRID_DECT_ATYPE_CT;
            csLine += csTemp;
        }
        else
        if ( m_pDectPacket->nATailType == 2 || m_pDectPacket->nATailType == 3 )
        {
            // Nt message
            csTemp = IDS_GRID_DECT_ATYPE_NT;
            csLine += csTemp;
        }
        else
        if ( m_pDectPacket->nATailType == 4 )
        {
            // Qt message
            // Determine if encryption is used
            // The first byte after the A field header
            quint8 nQh = (m_pDectPacket->pucBits[5] >> 4);
            if ( nQh == 0 || nQh == 1 )
                csTemp = IDS_GRID_DECT_ATYPE_QT_SSI;
            else
            if ( nQh == 2 )
                csTemp = IDS_GRID_DECT_ATYPE_QT_ERFC1;
            else
            if ( nQh == 3 )
                csTemp = IDS_GRID_DECT_ATYPE_QT_FPC;
            else
            if ( nQh == 4 )
            {
                csTemp = IDS_GRID_DECT_ATYPE_QT_EFPC;
                // Extended fixed part capabilites
                if ( m_pDectPacket->pucBits[5] & 2 )
                    nCrypt = 1;
            }
            else
            if ( nQh == 5 )
                csTemp = IDS_GRID_DECT_ATYPE_QT_SARILC;
            else
            if ( nQh == 6 )
            {
                if ( ((m_pDectPacket->pucBits[5] & 0x0F) == 0x0F) &&
                    m_pDectPacket->pucBits[6] == 0x0F )
                {
                    csFormat = IDS_GRID_DECT_QT_MF_FORMAT;
                    csTemp = QString::asprintf( csFormat.toStdString().data(), (m_pDectPacket->pucBits[7] << 16) |
                        (m_pDectPacket->pucBits[8] << 8) | m_pDectPacket->pucBits[9] );
                }
                else
                    csTemp = IDS_GRID_DECT_ATYPE_QT_MFN;
            }
            else
            if ( nQh == 7 )
                csTemp = IDS_GRID_DECT_ATYPE_QT_ESCAPE;
            else
            if ( nQh == 8 )
                csTemp = IDS_GRID_DECT_ATYPE_QT_OBSOLETE;
            else
            if ( nQh == 9 )
                csTemp = IDS_GRID_DECT_ATYPE_QT_ERFC2;
            else
            if ( nQh == 10 )
                csTemp = IDS_GRID_DECT_ATYPE_QT_RESERV;
            else
            if ( nQh == 11 )
                csTemp = IDS_GRID_DECT_ATYPE_QT_TI;
            else
            if ( nQh == 12 )
                csTemp = IDS_GRID_DECT_ATYPE_QT_EFPC2;
            else
            if ( nQh == 13 || nQh == 14 || nQh == 15 )
                csTemp = IDS_GRID_DECT_ATYPE_QT_RESERV;
            else
                csTemp = IDS_GRID_DECT_ATYPE_QT;

            csLine += csTemp;
        }
        else
        if ( m_pDectPacket->nATailType == 5 )
        {
            // Qt message
            csTemp = IDS_GRID_DECT_ATYPE_ESCAPE;
            csLine += csTemp;
        }
        else
        if ( m_pDectPacket->nATailType == 6 || (m_pDectPacket->nATailType == 7 && m_pDectPacket->nType == S_FIELD_PP) )
        {
            // Mt message
            csTemp = IDS_GRID_DECT_ATYPE_MT;
            csLine += csTemp;
        }
        else
        {
            // Pt message
            csTemp = IDS_GRID_DECT_ATYPE_PT;
            csLine += csTemp;
        }
        SetGridCell( ui->tblwgtExtInfo, 4, 1, csTemp );
        csLine += "\n";
        if( m_pqtsLog ) *m_pqtsLog << csLine;
        csLine.clear();

        // Using encryption
        csLine = IDS_GRID_DECT_INFO_CRYPT;
        SetGridCell( ui->tblwgtExtInfo, 5, 0, csLine, m_demData.fixCellBkGround );

        csLine += ": ";
        csTemp.clear();

        if ( nCrypt )
            csTemp = IDS_GRID_DECT_INFO_YES;
        else
            csTemp = IDS_GRID_DECT_INFO_NOT;
        SetGridCell( ui->tblwgtExtInfo, 5, 1, csTemp );

        csLine += csTemp;
        csLine += "\n";
        csLine += "\n";
        if( m_pqtsLog ) *m_pqtsLog << csLine;
        csLine.clear();

        QRect clRect = ui->tblwgtExtInfo->rect();
        ui->tblwgtExtInfo->horizontalHeader()->resizeSection( 0, (int)((double)(clRect.width()) * 0.12 ) );
        ui->tblwgtExtInfo->horizontalHeader()->resizeSection( 1, (int)((double)(clRect.width()) * 0.85 ) );
    }
    else
    if( m_demData.u8IsBit )
    {
        m_nPackets++;
        // Generate a table to display the sequence of bytes assembled from the found bits
        quint64     u64CurOffset =  m_u64Offset;
        quint32		u32Bits = m_nPrintBytes * 8, u32Bit = 0, u32Idx = 0;

        if( u64CurOffset + u32Bits > m_demData.u64DataSize )
            u32Bits = (quint32)(m_demData.u64DataSize - u64CurOffset);		// Not enough bits to display, limit

        if( u32Bits > m_dwFindBufferSize ) u32Bits = m_dwFindBufferSize;	// Limit the size of the read buffer
        // Prepare an array to store bytes
        vector<quint8> vecOut;
        vecOut.resize( (u32Bits / 8) + 1, 0 );

        quint8	*pBitFifo = 0;
        // Read data
        m_demData.qfDumpFile.seek( u64CurOffset );
        u32Bits = m_demData.qfDumpFile.read( (char*)m_pFindBuffer, u32Bits );//m_pcfOutFile->Read( m_pFindBuffer, (UINT)u32Bits );

        if( m_demData.u8BitInv )
        {
            for( quint32 dwIdx = 0; dwIdx < u32Bits; dwIdx++ )
                m_pFindBuffer[dwIdx] = !m_pFindBuffer[dwIdx];
        }

        pBitFifo = m_pFindBuffer;

        quint8	u8Bit = 0, u8CurByte = 0, nCurBit = 0;
        // Transform data
        u32Bit = 0;
        u32Idx = 0;

        if( m_demData.u8DigNotation == tagDemData::HEX )//m_nDigitMode == HEX_MODE )
            csLine = QString::asprintf("%08llX (%d):\n",  u64CurOffset, m_nPackets );
        else
            csLine = QString::asprintf("%08llu (%d):\n",  u64CurOffset, m_nPackets );

        if( m_pqtsLog ) *m_pqtsLog << csLine;
        csLine.clear();

        while( u32Bit < u32Bits )
        {
            u8Bit = (*pBitFifo) & 0x01;
            csFrag = QString::asprintf( "%d ", u8Bit );
            csLine += csFrag;

            nCurBit = u32Bit % 8;

            if( m_demData.u8ByteOrder == tagDemData::LE )
            {
                u8CurByte |= (u8Bit << nCurBit);
            }
            else
            {
                nCurBit = 7 - nCurBit;
                u8CurByte |= (u8Bit << nCurBit);
            }
            u32Bit++;
            if( !(u32Bit % 8) )
            {
                csLine += " ";

                vecOut[u32Idx] = u8CurByte;	// Save the value to the buffer
                u8CurByte = 0;
                u32Idx++;
                if( u32Idx >= vecOut.size() ) vecOut.resize( u32Idx + 1 ); // Control the buffer size

                if( !(u32Idx % 4) )
                {
                    csLine += "\n";
                    if( m_pqtsLog ) *m_pqtsLog << csLine;
                    csLine.clear();
                }

            }
            pBitFifo++; // Next bit
        }
        // Write the rest
        csLine += "\n";
        if( m_pqtsLog ) *m_pqtsLog << csLine;
        csLine.clear();

        int nCols = m_demData.nColCount + 1, nRows = (u32Idx / m_demData.nColCount) + 1;	// Address column and 32 bytes
        if( (u32Idx % m_demData.nColCount) > 0 ) nRows++;			// Display partial string
        // Generate a table for display
        // Hide column and row headers
        ui->tblwgtExtInfo->horizontalHeader()->hide();
        ui->tblwgtExtInfo->verticalHeader()->hide();
        ui->tblwgtExtInfo->setColumnCount( nCols );
        // Set the number of lines to display if you want to add one more under the incomplete line and one under the heading
        ui->tblwgtExtInfo->setRowCount( nRows + 1 );
        // Set column widths
        SetGridColumnSize( ui->tblwgtExtInfo, true );
        QString csTemp;
        // Load data into a table
        quint32 u32Bytes = u32Idx;

        u32Idx = 0;
        while( u32Idx < u32Bytes )
        {
            nCols = u32Idx % 32;
            nRows = u32Idx / 32;

            if( nCols == 0 )
            {
                if( m_demData.u8DigNotation == tagDemData::HEX )
                    csTemp = QString::asprintf( "%03X", u32Idx );
                else
                    csTemp = QString::asprintf( "%03d", u32Idx );

                SetGridCell( ui->tblwgtExtInfo, nRows + 1, 0, csTemp, QColor(255,255,255) );
            }

            if( m_demData.u8DigNotation == tagDemData::HEX )//m_nDigitMode == HEX_MODE )
                csTemp = QString::asprintf( "%02X", vecOut[u32Idx] );
            else
                csTemp = QString::asprintf( "%03d", vecOut[u32Idx] );

            SetGridCell( ui->tblwgtExtInfo, nRows + 1, nCols + 1, csTemp );

            csLine += csTemp;
            csLine += " ";

            u32Idx++;

            if( !(u32Idx % 16) )
            {
                csLine += "\n";

                if( m_pqtsLog ) *m_pqtsLog << csLine;
                csLine.clear();
            }
        }
        csLine += "\n\n";
        if( m_pqtsLog ) *m_pqtsLog << csLine;
        csLine.clear();

        // Clear unused cells
        u32Bytes = ui->tblwgtExtInfo->rowCount() - 1;
        u32Bytes *= m_demData.nColCount;

        csTemp.clear();
        while( u32Idx < u32Bytes )
        {
            nCols = u32Idx % 32;
            nRows = u32Idx / 32;

            SetGridCell( ui->tblwgtExtInfo, nRows + 1, nCols + 1, csTemp );

            u32Idx++;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////
// Set minimum column width
void DlgView::SetGridColumnSize( QTableWidget *pTable, bool bSetColText )
{
    if( !pTable ) return;
    QString	csText;

    if ( m_demData.u8DigNotation == tagDemData::HEX )
        csText = QString::asprintf( "%08X:", 0xFF );
    else
        csText = QString::asprintf( "%010d:", 0xFF );

    QFontMetrics tmpFontMetric( pTable->font() );
    pTable->horizontalHeader()->resizeSection( 0, tmpFontMetric.horizontalAdvance(csText) );

    if( m_demData.u8DigNotation == tagDemData::HEX )
        csText = QString::asprintf( "%03X", 0xFF );
    else
        csText = QString::asprintf( "%03d", 0xFF );

    int nWidth = tmpFontMetric.horizontalAdvance( csText );

    for ( int i = 1; i < m_demData.nColCount + 1; i++ )
    {
        pTable->horizontalHeader()->resizeSection( i, nWidth );
        if( bSetColText )
        {
            if( m_demData.u8DigNotation == tagDemData::HEX )
                csText = QString::asprintf( "%02X", i - 1 );
            else
                csText = QString::asprintf( "%02d", i - 1 );

            QTableWidgetItem *pItem = pTable->item( 0, i );
            if( pItem )
            {
                pItem->setText( csText );
                pTable->setItem( 0, i, pItem );
            }
            else
                pTable->setItem( 0, i, new QTableWidgetItem(csText) );
        }
    }
}
// Set the contents of a table cell
void DlgView::SetGridCell( QTableWidget *pTable, int nRow, int nCol, QString qsText, QColor bkColor, QColor textColor )
{
    Q_UNUSED(pTable)
    QTableWidgetItem *pItem = ui->tblwgtExtInfo->item( nRow, nCol );
    if( !pItem )  pItem = new QTableWidgetItem();
    if( pItem )
    {
        pItem->setText(qsText);
        pItem->setBackground( QBrush( bkColor ) );
        pItem->setForeground( textColor );
        ui->tblwgtExtInfo->setItem( nRow, nCol, pItem );
    }
}

void DlgView::timerEvent(QTimerEvent *event)
 {
    Q_UNUSED(event)
    // Check if the file has changed
    if( m_demData.qfDumpFile.isOpen() )
    {
        m_demData.mtxDemData.lock();
        quint64 u64DataSize = m_demData.qfDumpFile.size();

        if ( u64DataSize != m_demData.u64DataSize )
        {
            m_demData.u64DataSize = u64DataSize;
            m_demData.mtxDemData.unlock();
            // Update file size and number of rows in the model
            UpdateFileSize();
        }
        else
            m_demData.mtxDemData.unlock();
    }
 }
