// ANSI-1251
#include "DectPktFinder.h"

// Preamble of packet from base station
const char CDectPktFinder::ACCESS_CODE_PP[] = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,1,0,0,1,1,1,0,1,0,1};
// Preamble of packet from portable station
const char CDectPktFinder::ACCESS_CODE_RFP[] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,0,1,1,0,0,0,1,0,1,0};
// Identification packet preamble
//const char CDectPktFinder::ACCESS_CODE_IDENTITY[] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,0,1,1,0,0,0,1,0,1,0,0,1,1,0,1,1,1,0};

CDectPktFinder::CDectPktFinder(void)
{
	m_nFifoRPos = 0; 
	m_nFifoWPos = 0; 
	m_nFifoFill = 0;
	m_nPopBits = 0;
	m_nTempPos = 0;
	m_pLogFile = 0;
}

CDectPktFinder::~CDectPktFinder(void)
{
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Searching for packets in a bitstream
quint32 CDectPktFinder::Work( unsigned char *pData, quint32 nBufSize, DectPacket **pOutPacket )
{
    quint32 		nRead;				// Counter of processed bits (bytes)
    unsigned char	*pBuffer = 0; 		// Pointer to the input buffer
    char			*pBitFifo;			// Pointer to the current position of the FIFO buffer
    bool			bFind = 0;

	if ( !pData || !nBufSize )
		return -1;

	// Processed data
	pBuffer = pData;
	pBitFifo = m_pFifo + m_nFifoWPos;
	nRead = 0;
    
	while ( nRead < nBufSize && !bFind )
    {
        // Push bit to bit FIFO
        *pBitFifo = *pBuffer;
		pBuffer++;
		pBitFifo++;	
        if ( ++m_nFifoWPos >= FIFO_SIZE ) 
        {
        	m_nFifoWPos = 0; // Cyclic fill
        	pBitFifo = m_pFifo;
        }
        if ( ++m_nFifoFill >= FIFO_SIZE )
        {
			if ( ( m_Packet.nType = CheckSField() ) > 0 )
			{
                quint8			pucRCRC[8];
				
                bFind = true;
				m_Packet.u64RFPI = 0;
                // Copy 16 bytes of the packet, starting with the preamble
				CopyBits(0, m_Packet.pucBits, 128);
                // Copy field A to temporary buffer
				memcpy( pucRCRC, m_Packet.pucBits + 4, 6 );
				pucRCRC[6]=0;
				pucRCRC[7]=0;
                // RCRC transmitted in the packet
                m_Packet.wRCRC = (((quint16)m_Packet.pucBits[10]) << 8) | m_Packet.pucBits[11];
                // Calculate the RCRC of field A data
				m_Packet.wRCRC_Calc = CalcRCRC( pucRCRC ); 
				m_Packet.nAHdrField = AirToHost8( 32, 8 );
                // Data type
				m_Packet.nATailType	= m_Packet.nAHdrField >> 5; 
				if ( m_Packet.nATailType == 2 || m_Packet.nATailType == 3 ) // Nt сообщение
					m_Packet.u64RFPI = AirToHost64( 40, 40 );
                // Match found in S field
				if ( m_Packet.nType == S_FIELD_RFP )
				{
					PopBits( AC_LENGTH_RFP );
				}
				else
				if ( m_Packet.nType == S_FIELD_PP )
				{
                    // Other processing
					PopBits( AC_LENGTH_PP );
				}
			}
			else
                PopBit(); // Next bit
		}
		nRead++;
    } // End of input buffer processing cycle

    // If the package is found, return a pointer to it.
	if ( bFind )
		*pOutPacket = &m_Packet;

	return nRead;
}
//////////////////////////////////////////////////////////////////////////////////////
// RCRC Calculation
quint16 CDectPktFinder::CalcRCRC( quint8* pData )
{
    quint16	wGp = 0x0589;		/* 10000010110001001 without the leading 1 */
    quint16	wCRC;
    quint8	nNext;
	int		y, x;

	wCRC = (pData[0] << 8) | pData[1];
	y = 0;
	while( y < 6 )
	{
		nNext = pData[2 + y];
		y++;
		x = 0;
		while( x < 8)
		{
			while( !(wCRC & 0x8000) )
			{
				wCRC <<= 1;
				wCRC |= !!(nNext&0x80);
				nNext <<= 1;
				x++;
				if( x > 7 )
					break;
			}
			if( x > 7 )
				break;
			wCRC <<= 1;
			wCRC |= !!(nNext&0x80);
			nNext <<= 1;
			x++;
			wCRC ^= wGp;
		}
	}
	wCRC ^= 1;
	return wCRC;
}
//////////////////////////////////////////////////////////////////////////////////////
// Convert the specified bit range to bytes
quint8 CDectPktFinder::CopyBits(unsigned char nStartOffset, quint8 pucBytes[], quint32 nBits )
{
  quint32			i, nByte = 0;

  m_nTempPos = m_nFifoRPos + nStartOffset; //
  if ( m_nTempPos >= FIFO_SIZE ) m_nTempPos -= FIFO_SIZE;
  i = 0; 
  while( i < nBits ) 
  {
	  pucBytes[nByte] = 0;
      for (quint8 nBit = 0; nBit < 8 && i < nBits; nBit++)
	  {
		  pucBytes[nByte] |= (m_pFifo[m_nTempPos] << (7 - nBit));
		  i++;
		  if ( ++m_nTempPos >= FIFO_SIZE ) m_nTempPos = 0; 
	  }
	  nByte++;
  }
  return nByte;
}
//////////////////////////////////////////////////////////////////////////////////////
// Convert some bits to unsigned long
quint64 CDectPktFinder::AirToHost64(unsigned char nStartOffset, unsigned char nBits)
{
  quint64 nOut = 0;
  unsigned char i;
  m_nTempPos = m_nFifoRPos + nStartOffset; //
  if ( m_nTempPos >= FIFO_SIZE ) m_nTempPos -= FIFO_SIZE;
  for ( i = 0; i < nBits; i++ ) 
  {
    nOut |= ((((quint64)m_pFifo[m_nTempPos])) << (nBits - 1 - i));
    if ( ++m_nTempPos >= FIFO_SIZE ) m_nTempPos = 0; 
  }
  return nOut;
}
//////////////////////////////////////////////////////////////////////////////////////
// Convert the specified number of bits to bytes
unsigned char CDectPktFinder::AirToHost8(unsigned char nStartOffset, unsigned char nBits)
{
  unsigned char nOut = 0, i;
  m_nTempPos = m_nFifoRPos + nStartOffset; //
  if ( m_nTempPos >= FIFO_SIZE ) m_nTempPos -= FIFO_SIZE;
  for ( i = 0; i < nBits; i++ ) 
  {
    nOut |= (m_pFifo[m_nTempPos] << (7 - i));
    if ( ++m_nTempPos >= FIFO_SIZE ) m_nTempPos = 0; 
  }
  return nOut;
}
//////////////////////////////////////////////////////////////////////////////////////
// Search for a synchronization word match
int CDectPktFinder::CheckSField()
{
	int	nOut1 = S_FIELD_RFP, nOut2 = S_FIELD_PP;;
    quint32 i;

	m_nTempPos = m_nFifoRPos;

	for ( i = 0; i < AC_LENGTH_RFP; i++ ) 
	{
        // Checking for ACCESS_CODE_RFP match
		if ( m_pFifo[m_nTempPos] != ACCESS_CODE_RFP[i] ) nOut1 = 0;
		if ( m_pFifo[m_nTempPos] != ACCESS_CODE_PP[i] ) nOut2 = 0;

		if ( !nOut1 && !nOut2 ) break;

		if ( ++m_nTempPos >= FIFO_SIZE ) m_nTempPos = 0; 
	}
	
	return nOut1 | nOut2;
}
//////////////////////////////////////////////////////////////////////////////////////
// Shift the current read position in the FIFO by one bit
void CDectPktFinder::PopBit()
{
  if ( m_nFifoFill > 0 )
  {
    m_nFifoFill--;
    if ( ++m_nFifoRPos >= FIFO_SIZE ) m_nFifoRPos = 0; 
	m_nPopBits++;
  }
}
//////////////////////////////////////////////////////////////////////////////////////
// Shift the current read position in the FIFO by the specified number of bits
void CDectPktFinder::PopBits( unsigned short nBits )
{
  if ( m_nFifoFill >= nBits )
  {
    m_nFifoFill -= nBits;
    m_nFifoRPos += nBits;
    if ( m_nFifoRPos >= FIFO_SIZE ) m_nFifoRPos -= FIFO_SIZE; 
	m_nPopBits += nBits;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initialization
int CDectPktFinder::Start()
{
	m_nFifoRPos = 0; m_nFifoWPos = 0; m_nFifoFill = 0;
	m_nPopBits = 0;

	return 1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Completion of work
int CDectPktFinder::Stop()
{
	return 1;
}
