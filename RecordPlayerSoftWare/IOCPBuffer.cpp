// IOCPBuffer.cpp: implementation of the CIOCPBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IOCPBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIOCPBuffer::CIOCPBuffer()
{
     Init(); // Never called if the class is allocated with VirtualAlloc.. 	
}

void CIOCPBuffer::Init()
{
	m_nUsed=0;
	m_Operation=-1;
	m_pPos=NULL;
	ZeroMemory(&m_ol, sizeof(OVERLAPPED));
	ZeroMemory(&m_Buffer,sizeof(m_Buffer));
}


CIOCPBuffer::~CIOCPBuffer()
{
	// Never called if the class is allocated with VirtualAlloc.. 
	m_nUsed=0;
	//FreeBuffer();
}

/*
* Returns the pointer to the Buffer.  
*
*/
PBYTE CIOCPBuffer::GetBuffer()
{
	return (PBYTE)m_Buffer;
}

PBYTE CIOCPBuffer::GetPayLoadBuffer()
{
	return m_nUsed>MINIMUMPACKAGESIZE ? GetBuffer()+MINIMUMPACKAGESIZE : NULL;	
}

/*
* Adds a singel BYTE to the data. 
*
*/

BOOL CIOCPBuffer::AddData(BYTE data)
{
	return AddData(&data, 1);
}

BOOL CIOCPBuffer::AddData(UINT data)
{
	return AddData(reinterpret_cast<const BYTE*>(&data), sizeof(UINT));
}


BOOL CIOCPBuffer::AddData(unsigned short data)
{
	return AddData(reinterpret_cast<const BYTE*>(&data), sizeof(unsigned short));
}


/*
* Adds a stream of char to the buffer. 
*
*/
BOOL CIOCPBuffer::AddData(const char *const pData, UINT nSize)
{ 
	return AddData(reinterpret_cast<const BYTE*>(pData),nSize);
}

/*
* Adds a stream of BYTES to the buffer. 
*
*/

BOOL CIOCPBuffer::AddData(const BYTE *const pData, UINT nSize)
{
	if ( nSize > MAXIMUMPACKAGESIZE-m_nUsed )
		return FALSE;
	else
	{
		memcpy(m_Buffer + m_nUsed, pData, nSize);
		m_nUsed += nSize;
		return TRUE;
	}
}


// cheks if the Buffer is valid. 
BOOL CIOCPBuffer::IsValid()
{
	return TRUE;
}

UINT CIOCPBuffer::GetUsed()
{
	return m_nUsed;
}

// Used to indicate that we did have  a successfull Receive 
UINT CIOCPBuffer::Use(UINT nSize)
{
        m_nUsed+=nSize;
        return m_nUsed;
}

// Empty A used structure. 
void CIOCPBuffer::EmptyUsed()
{
	m_nUsed=0;
}

// Sets the current Operation. 
void CIOCPBuffer::SetOperation(int op)
{
    ZeroMemory(&m_ol, sizeof(OVERLAPPED));
    m_Operation=op;
}


// Setup the buffer for a ZeroByteRead. 
void CIOCPBuffer::SetupZeroByteRead()
{
	m_wsabuf.buf =(char*)m_Buffer;
	m_wsabuf.len = 0; 
}

// Setup Setup the buffer for a Read. 
void CIOCPBuffer::SetupRead()
{
if (m_nUsed == 0)
   {
      m_wsabuf.buf = reinterpret_cast<char*>(m_Buffer);
      m_wsabuf.len = MAXIMUMPACKAGESIZE; 
   }
   else // We have received some of the data but not all .. 
   {
      m_wsabuf.buf = reinterpret_cast<char*>(m_Buffer) + m_nUsed;
      m_wsabuf.len = MAXIMUMPACKAGESIZE - m_nUsed; 
   }
}

// Setup the buffer for a Write
void CIOCPBuffer::SetupWrite()
{
   m_wsabuf.buf = reinterpret_cast<char*>(m_Buffer);
   m_wsabuf.len = m_nUsed;
}

// Returns the WSABUF used with WsaRead and WsaWrite. 
WSABUF * CIOCPBuffer::GetWSABuffer()
{
	return  const_cast<WSABUF*>(&m_wsabuf);
}

//Get the Operation 
int CIOCPBuffer::GetOperation()
{
       return m_Operation;
}

// Saves a position into the buffer (e.g. in a CList) 
void CIOCPBuffer::SetPosition(POSITION pos)
{
	m_pPos=pos;
}

// Returns the Position of a buffer. 
POSITION CIOCPBuffer::GetPosition()
{
	return m_pPos;
}


// removes nSize byte from the Buffer. 
BOOL CIOCPBuffer::Flush(UINT nBytesToRemove)
{
	if ((nBytesToRemove > MAXIMUMPACKAGESIZE) || (nBytesToRemove > m_nUsed) ) 
	{
		TRACE("ERROR BOOL CIOCPBuffer::Flush(UINT nBytesToRemove)");
			return FALSE;
	}
	m_nUsed-=nBytesToRemove;
	memmove(m_Buffer, m_Buffer + nBytesToRemove, m_nUsed);
	return TRUE;
}



// Sets the sequence number of the buffer..
void CIOCPBuffer::SetSequenceNumber(int nr)
{
        m_iSequenceNumber=nr;
}


// Gets the sequence number of the buffer. 
int CIOCPBuffer::GetSequenceNumber()
{
      return m_iSequenceNumber;
}


/*
 * CreatePackage(CString stxt)
 * Creates one package in the buffe. (Se below) 
 *
 * [SizeHeader|...String..\0]
 */

BOOL CIOCPBuffer::CreatePackage(CString stxt)
{
	
	return TRUE;
}


/*
 * CreatePackage(BYTE Type,CString stxt)
 * Creates one package in the buffe. (Se below) 
 *
 * [SizeHeader|Type|...String..\0]
 */
BOOL CIOCPBuffer::CreatePackage(BYTE Type,CString stxt)
{
	
	return TRUE;
}

/*
 * CreatePackage(BYTE Type,CString stxt)
 * Creates one package in the buffe. (Se below) 
 *
 * [SizeHeader|Type|...String..\0]
 */
BOOL CIOCPBuffer::CreatePackage(BYTE Type)
{

	//根据传输类型来进行创建包
	switch(Type)
	{
	  case FILE_TRANSFER_START:
		{
              //创建文件传输包
			EmptyUsed();
			TDGRAMHEADER mPackPage;
			memset(&mPackPage,0,sizeof(TDGRAMHEADER));

			//填写包：
			mPackPage.Tag[0]=0x00;
			mPackPage.Tag[1]=0xFF;
			mPackPage.Comand=0x03;
			mPackPage.rdata[0]=0xFF; //一个数据长度
			int datalen=1;
			memmove(mPackPage.DataLen,&datalen,sizeof(mPackPage.DataLen));
            
            AddData((PBYTE)&mPackPage,PACK_HEAD_LENGTH+datalen);	
		}
		break;
	  case FILE_TRANSFER_ABORT:
		{
			//创建文件终止传输包
			EmptyUsed();
			TDGRAMHEADER mPackPage;
			memset(&mPackPage,0,sizeof(TDGRAMHEADER));

			//填写包：
			mPackPage.Tag[0]=0x00;
			mPackPage.Tag[1]=0xFF;
			mPackPage.Comand=0x04;
			mPackPage.rdata[0]=0xFF; //一个数据长度
			int datalen=1;
			memmove(mPackPage.DataLen,&datalen,sizeof(mPackPage.DataLen));

			AddData((PBYTE)&mPackPage,PACK_HEAD_LENGTH+datalen);	

		}
		break;
	 

	}


	return TRUE;
}



/*
 * CreatePackage(BYTE Type,CString stxt)
 * Creates one package in the buffe. (Se below) 
 *
 * [SizeHeader|Type|...String..\0]
 */

BOOL CIOCPBuffer::CreatePackage(BYTE Type,UINT nData, CString stxt)
{
	switch(Type)
	{
	case FILE_TRANSFER_INFO:
		{
			//文件传输信息类型
			EmptyUsed();
			TDGRAMHEADER mPackPage;
			memset(&mPackPage,0,sizeof(TDGRAMHEADER));

			//填写包：
			mPackPage.Tag[0]=0x00;
			mPackPage.Tag[1]=0xFF;
			mPackPage.Comand=0x02;
			//填写文件字节大小
			memcpy(mPackPage.rdata,&nData,4);

			int strlen=stxt.GetLength();

			memcpy(mPackPage.rdata+4,stxt.GetBuffer(0),strlen);

			int datalen=4+strlen;
			
			memmove(mPackPage.DataLen,&datalen,sizeof(mPackPage.DataLen));

			AddData((PBYTE)&mPackPage,PACK_HEAD_LENGTH+datalen);	


		}
		break;
	case FILE_TRANSFER_START:
		{
			//创建文件传输包
			EmptyUsed();
			TDGRAMHEADER mPackPage;
			memset(&mPackPage,0,sizeof(TDGRAMHEADER));

			//填写包：
			mPackPage.Tag[0]=0x00;
			mPackPage.Tag[1]=0xFF;
			mPackPage.Comand=0x03;
			mPackPage.rdata[0]=0xFF; //一个数据长度
			int datalen=1;
			memmove(mPackPage.DataLen,&datalen,sizeof(mPackPage.DataLen));

			AddData((PBYTE)&mPackPage,PACK_HEAD_LENGTH+datalen);	
		}
		break;
	case FILE_TRANSFER_ABORT:
		{
			//创建文件终止传输包
			EmptyUsed();
			TDGRAMHEADER mPackPage;
			memset(&mPackPage,0,sizeof(TDGRAMHEADER));

			//填写包：
			mPackPage.Tag[0]=0x00;
			mPackPage.Tag[1]=0xFF;
			mPackPage.Comand=0x04;
			mPackPage.rdata[0]=0xFF; //一个数据长度
			int datalen=1;
			memmove(mPackPage.DataLen,&datalen,sizeof(mPackPage.DataLen));

			AddData((PBYTE)&mPackPage,PACK_HEAD_LENGTH+datalen);	

		}
		break;
	}


	return TRUE;
}

/*
 * CreatePackage(BYTE Type, BYTE key, CString stxt)
 * Creates one package in the buffe. (Se below) 
 *
 * [SizeHeader|Type|Key|...String..\0]
 */
BOOL CIOCPBuffer::CreatePackage(BYTE Type, BYTE key, CString stxt)
{
	return TRUE;
}


/*
 * CreatePackage(BYTE Type,BYTE key1, BYTE key2, BYTE key3,CString stxt)
 * Creates one package in the buffe. (Se below) 
 *
 * [SizeHeader|Type|Key1|key2|key3|...String..\0]
 */
BOOL CIOCPBuffer::CreatePackage(BYTE Type, BYTE key1, BYTE key2, BYTE key3, CString stxt)
{
	

	return TRUE;
}


/*
 * CreatePackage(BYTE Type, UINT iFilesize,UINT iBufferSize, const BYTE *const pData)
 * Creates one package in the buffe. (Se below) 
 *
 * [SizeHeader|Type|iFilesize|..Buffer data...]
 */

BOOL CIOCPBuffer::CreatePackage(BYTE Type, UINT iFilesize,UINT iBufferSize, const BYTE *const pData)
{
	
	return TRUE;
	
}




/*
 * Returns the PackageSize
 */
UINT CIOCPBuffer::GetPackageSize()
{

	//获取包的载荷数据长度
	UINT nSize=0;
	if(m_nUsed>=MINIMUMPACKAGESIZE)
	{
          PBYTE buf=GetBuffer();

		  TDGRAMHEADER mPackPage;
		  memset(&mPackPage,0,sizeof(TDGRAMHEADER));

		  memcpy(&mPackPage,buf,m_nUsed);

		  memmove(&nSize,mPackPage.DataLen,4);

		
	}
	return nSize;

}

// Gets the package Type return 255 if error. 
BYTE CIOCPBuffer::GetPackageType()
{

	//根据协议进行获取包的操作码和类型
	BYTE Type= 255;
	TDGRAMHEADER mPackData;
	memset(&mPackData,0,sizeof(TDGRAMHEADER));
	memmove(&mPackData,GetBuffer(),MINIMUMPACKAGESIZE);

	if (mPackData.Tag[0]==0x00&&mPackData.Tag[1]==0xFF)
	{
		Type=mPackData.Comand;
		return Type;
	}

	return Type;
}


// Reads the info from a package created with CreatePackage(CString stxt);
BOOL CIOCPBuffer::GetPackageInfo(CString &stxt)
{
	return TRUE;
}

// Reads the info from a package created with CreatePackage(BYTE Type,CString stxt);
BOOL CIOCPBuffer::GetPackageInfo(BYTE& Type, CString &stxt)
{

	return TRUE;
	
}

// Reads the info from a package created with CreatePackage(BYTE Type,CString stxt);
BOOL CIOCPBuffer::GetPackageInfo(BYTE& Type,UINT& nData, CString& sFilename)
{
	//获取文件信息包
	//解包
	int nSize=GetPackageSize();
	BOOL bResult=FALSE;
	if ( nSize>= 1+sizeof(UINT) && nSize<=MAXIMUMPAYLOADSIZE)
	{
		TDGRAMHEADER mPackPage;
		memset(&mPackPage,0,sizeof(TDGRAMHEADER));
		PBYTE buf=GetBuffer();

		memcpy(&mPackPage,buf,PACK_HEAD_LENGTH+nSize);

		// Get the Type
		Type=mPackPage.Comand;
        
		// Get the nSize 
		memmove(&nData,mPackPage.rdata,4);
		// Get The text
		PBYTE pData=(mPackPage.rdata+4);
		// Assumes that we already have a null termination. 
		sFilename=pData;
	    bResult=TRUE;
	}
	
	
	
	return bResult;		
	
}



BOOL CIOCPBuffer::GetPackageInfo(BYTE &Type, BYTE &key1, BYTE &key2, BYTE &key3, CString &stxt)
{
	
	return TRUE;
}


BOOL CIOCPBuffer::GetPackageInfo(BYTE &Type, BYTE &key, CString &stxt)
{
	
	return TRUE;
}


void CIOCPBuffer::DUMP()
{
   
}


