#include "datamodel.h"
#include <QDebug>
#include <QBrush>

RemoteModel::RemoteModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_pDemData = 0;
    m_nRowCount = 0;
}

RemoteModel::~RemoteModel()
{
}

void RemoteModel::SetRowCount(quint64 nRowCount )
{
    beginResetModel();

    m_nRowCount = nRowCount;

    endResetModel();
}

int RemoteModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_nRowCount;
}

int RemoteModel::columnCount(const QModelIndex & /*parent*/) const
{
    if( m_pDemData )
        return m_pDemData->nColCount;

    return 0;
}

QVariant RemoteModel::data(const QModelIndex &index, int role) const
{
    if( !m_pDemData || !m_pDemData->pViewBuffer || !m_pDemData->u64DataSize ) return QVariant();

    int     nNewRow = index.row();
    int     nNewCol = index.column();
    quint64 i64Org = nNewRow*m_pDemData->nColCount + nNewCol;
    QString qsTemp;

    if( i64Org >= m_pDemData->u64DataSize ) return QVariant();  // Out of bounds data, may be on the last line, return "empty"

    // Check that the index is in the window, if necessary, load new data
    if( m_pDemData->u32ViewBufferFill == 0 || i64Org < m_pDemData->u64ViewBufferOrg || i64Org > m_pDemData->u64ViewBufferOrg + m_pDemData->u32ViewBufferFill )
    {
        m_pDemData->mtxDemData.lock();

        qint64 i64Start = i64Org - m_pDemData->u32ViewBufferSize / 2;   // The first index must be in the middle of the buffer.
        if( i64Start < 0 ) i64Start = 0;    // Data out of bounds

        qint64 i64DataSize = m_pDemData->u64DataSize - i64Start; // Number of bytes from offset to end of file
        if( i64DataSize > m_pDemData->u32ViewBufferSize ) i64DataSize = m_pDemData->u32ViewBufferSize;

        m_pDemData->u32ViewBufferFill = 0;
        if( m_pDemData->qfDumpFile.isOpen() )
        {
            m_pDemData->u64ViewBufferOrg = (quint32)i64Start;
            m_pDemData->qfDumpFile.seek( i64Start );
            m_pDemData->u32ViewBufferFill = m_pDemData->qfDumpFile.read((char*)m_pDemData->pViewBuffer, i64DataSize );
            // Replace with Intel
            quint8 u8Max = 0;
            for( quint32 dwIdx = 0; dwIdx < m_pDemData->u32ViewBufferFill; dwIdx++ )
                if( m_pDemData->pViewBuffer[dwIdx] > u8Max ) u8Max = m_pDemData->pViewBuffer[dwIdx];

            if( u8Max <= 1 )
                m_pDemData->u8IsBit = 1;
            else
                m_pDemData->u8IsBit = 0;
        }

        m_pDemData->mtxDemData.unlock();
    }

    qint64 orgInBuf = i64Org - m_pDemData->u64ViewBufferOrg; // Data offset in the buffer
    quint8  u8Byte = m_pDemData->pViewBuffer[orgInBuf];

    if( m_pDemData->u8IsBit && m_pDemData->u8BitInv )
        u8Byte = !u8Byte;   // Invert

    if(role == Qt::DisplayRole)
    {
        if( m_pDemData->u8DigNotation == tagDemData::DEC )
            return QString::number( u8Byte );
        else
        if( m_pDemData->u8DigNotation == tagDemData::HEX )
        {
            if( m_pDemData->u8IsBit )
                qsTemp = QString::asprintf("%X", u8Byte );
            else
                qsTemp = QString::asprintf("%02X", u8Byte );
            return qsTemp;
        }
    }
    else
    if( role == Qt::DecorationRole && nNewCol == 0 )
    { // The icon was displayed here
    }
    else
    if( role == Qt::BackgroundRole )
    {
        if( m_pDemData->u64StartSelPos != m_pDemData->u64EndSelPos && i64Org >= m_pDemData->u64StartSelPos && i64Org <= m_pDemData->u64EndSelPos )
        {
            // Initialize the package search class
            if ( m_pDemData->u8PktType == tagDemData::BLUETOOTH )
            {
                if( i64Org < m_pDemData->u64StartSelPos + 38 )
                    return QColor(184, 235, 171); // Green
                else
                if( i64Org < m_pDemData->u64StartSelPos + 62 )
                    return QBrush( m_pDemData->selLapBkGround );
                else
                if( i64Org < m_pDemData->u64StartSelPos + 72 )
                        return QColor(184, 235, 171); // Green
                else
                if( i64Org < m_pDemData->u64StartSelPos + 126 )
                        return QBrush( m_pDemData->selHdrBkGround ); // Blue
            }
            else
            if ( m_pDemData->u8PktType == tagDemData::LST )
            {
                if( i64Org < m_pDemData->u64StartSelPos + 40 )
                    return QBrush( m_pDemData->selLapBkGround );
                else
                if( i64Org < m_pDemData->u64StartSelPos + 56 )
                    return QBrush( m_pDemData->selHdrBkGround );
                else
                    return QBrush( QColor(184, 235, 171) ); // Highlight in green
            }
            else
                return QBrush( QColor(184, 235, 171) );     // Highlight in green
        }
        else
        {
            return QBrush( QColor(255, 255, 212) );
        }
    }

    return QVariant();
}

QVariant RemoteModel::headerData(int section,
                               Qt::Orientation  orientation,
                               int role) const
{
    if( !m_pDemData ) return QVariant();

    QString qsTemp;

    if (role == Qt::DisplayRole)
    {
        if(orientation == Qt::Horizontal)
        {
            if( m_pDemData->u8DigNotation == tagDemData::DEC )
                return QString::number( section ); // Column number
            else
            if( m_pDemData->u8DigNotation == tagDemData::HEX )
            {
                qsTemp = QString::asprintf("%02X", section );
                return qsTemp;
            }
        }
        else if(orientation == Qt::Vertical)
        {
            if( m_pDemData->u8DigNotation == tagDemData::DEC )
                return QString::number( section * m_pDemData->nColCount ); // Line number
            else
            if( m_pDemData->u8DigNotation == tagDemData::HEX )
            {
                qsTemp = QString::asprintf("%08X", section * m_pDemData->nColCount );
                return qsTemp;
            }
        }
    }
    else
    if( role == Qt::TextAlignmentRole )
        return Qt::AlignLeft;

    return QVariant();
}
