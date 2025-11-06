// ANSI-1251
#pragma once
#include <QFile>

#define AC_LENGTH_RFP			32
#define AC_LENGTH_PP			32
// S-field types
#define S_FIELD_RFP				1
#define S_FIELD_PP				2

#define	AF_TA					0x07	// The first 3 bits of the A field header
#define	AF_TA_NT				0x06	// identities information

#define MAX_FIFO_SIZE			128

#pragma pack(1)
typedef struct tagDectPacket
{
    quint8		nType;						// Packet preamble type
    quint8		nAHdrField, nATailType;     // Field header A, field type A
    quint64     u64RFPI;					// Device ID
    quint32		wRCRC;						// RCRC transmitted in the packet
    quint32		wRCRC_Calc;					// RCRC calculated from the received packet data
    quint8		pucBits[MAX_FIFO_SIZE];
}DectPacket;
#pragma pack()

class CDectPktFinder
{
public:
	CDectPktFinder(void);
	~CDectPktFinder(void);
    // Initialization
	int Start();
    // Completion of work
	int Stop();
    // Searching for packets in a bitstream
    quint32 Work( unsigned char *pData, quint32 nBufSize, DectPacket **pOutPacket );
    void SetLogFile( QFile	*pLogFile ) { m_pLogFile = pLogFile; };

    static const unsigned int 		FIFO_SIZE = MAX_FIFO_SIZE;	// !Temporary, you need to set the maximum packet size

protected:
    static const char ACCESS_CODE_RFP[AC_LENGTH_RFP];			// Preamble of packet from base station
    static const char ACCESS_CODE_PP[AC_LENGTH_PP];				// Preamble of packet from portable station

    char            				m_pFifo[FIFO_SIZE], m_pPktData[FIFO_SIZE];			// bit FIFO, packet bits
    unsigned short  				m_nFifoRPos, m_nFifoWPos, m_nFifoFill, m_nTempPos;	// FIFO current fill counters
    DectPacket						m_Packet;											// New package
    quint64                         m_nPopBits;											// Number of bits processed
    QFile                           *m_pLogFile;

    // Shift the current read position in the FIFO by one bit
	void PopBit();
    // Shift the current read position in the FIFO by the specified number of bits
	void PopBits( unsigned short nBits );
    // Search for a synchronization word match
	int CheckSField();
    // Convert the specified number of bits to bytes
	unsigned char AirToHost8(unsigned char nStartOffset, unsigned char nBits);
    quint64 AirToHost64(unsigned char nStartOffset, unsigned char nBits);
    // Convert the specified bit range to bytes
    quint8 CopyBits(unsigned char nStartOffset, quint8 pucBytes[], quint32 nBits );
    // RCRC Calculation
    quint16 CalcRCRC( quint8* pData );
};
