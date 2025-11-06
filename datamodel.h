#ifndef REMOTEMODEL_H
#define REMOTEMODEL_H

#include <QAbstractTableModel>
#include <QFile>
#include <QMutex>
#include <QColor>

typedef struct tagDemData
{
    tagDemData()
    {
        nColCount = 32;
        u8DigNotation = HEX;
        u8ByteOrder = LE;
        u8BitInv = 0;
        u8PktType = BLUETOOTH;
        u64DataSize = 0;
        pViewBuffer = 0;
        u64ViewBufferOrg = 0;
        u32ViewBufferSize = 0;
        u32ViewBufferFill = 0;
        u8IsBit = 0;
        // Start and end of the found packet
        u64StartSelPos = 0;
        u64EndSelPos = 0;
        // Default colors
        fixCellBkGround = QColor(240, 245, 255);		// Headers and footers
        defCellBkGround = QColor(0xFF, 0xFF, 0xE0);     // Yellow
        selCellBkGround = QColor(184, 235, 171);		// Green
        selLapBkGround = QColor(191, 223, 255);         // Blue
        selHdrBkGround = QColor(226, 198, 255);         // Violet
        redBkGround = QColor(254, 143, 101);			// Red
    }

    enum {DEC = 0, HEX} ;
    enum {LE = 0, BE} ;
    enum {BLUETOOTH = 0, DECT, COMM, LST, BYTES}; // Search options

    int         nColCount;          // Number of columns
    quint8      u8DigNotation;      // Number system
    quint8      u8ByteOrder;        // Byte order
    quint8      u8BitInv;           // Bit inversion
    quint8      u8PktType;          // Type of packages sought
    quint8      u8IsBit;            // Binary data sign
    QFile       qfDumpFile;         // Displayed file
    quint64     u64DataSize;        // File size
    quint8      *pViewBuffer;       // Displayed data (window)
    quint64     u64ViewBufferOrg;   // Window offset in data
    quint32     u32ViewBufferSize, u32ViewBufferFill;   // Buffer full, Max buffer size
    quint64     u64StartSelPos, u64EndSelPos;           // Start and end of the found packet
    // Default colors: background, selected cell
    QColor      defCellBkGround, selCellBkGround, fixCellBkGround;
    QColor      selLapBkGround,  selHdrBkGround, redBkGround;
    QMutex      mtxDemData;         // Mutex for synchronizing access to data
}DemData;

class RemoteModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit RemoteModel(QObject *parent = 0);
    ~RemoteModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void SetData( DemData *pDemData ) { m_pDemData = pDemData; }
    void SetRowCount( quint64 nRowCount );
protected:
    DemData             *m_pDemData;
    quint64             m_nRowCount;
};

#endif // REMOTEMODEL_H
