#include "stdafx.h"
#include "Define.h"
#include <string>
using namespace std;

SYSTEMTIME		m_stStartTime;    //查询起始时间
SYSTEMTIME		m_stEndTime;     //查询结束时间


CString GetApplicationPath()
{
	TCHAR exeFullPath[MAX_PATH]; 
	GetModuleFileName( NULL, exeFullPath, MAX_PATH ); 
	CString strPathSetting = exeFullPath;
	int nFind = strPathSetting.ReverseFind('\\');
	if (nFind != -1)
		strPathSetting = strPathSetting.Left( nFind+1 );
	else
		strPathSetting = _T("");
	return strPathSetting;
}

//有多字符集转换为Unicode

WCHAR * mbcsToUnicode(const char *zFilename)
{
	int nByte;
	WCHAR *zMbcsFilename;
	int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, NULL,0)*sizeof(WCHAR);
	zMbcsFilename = (WCHAR *)malloc(nByte*sizeof(zMbcsFilename[0]));
	if( zMbcsFilename==0 )
	{
		return 0;
	}
	nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, zMbcsFilename, nByte);
	if(nByte == 0)
	{
		free(zMbcsFilename);
		zMbcsFilename = 0;
	}
	return zMbcsFilename;
}

//有Unicode转为UTF-8
char *unicodeToUtf8(const WCHAR *zWideFilename)
{
	int nByte; char *zFilename;
	nByte = WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, 0, 0, 0, 0);
	zFilename = (char *)malloc(nByte);
	if(zFilename == 0)
	{
		return 0;
	}
	nByte = WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, zFilename, nByte, 0, 0);
	if( nByte == 0 )
	{
		free(zFilename);
		zFilename = 0;
	}
	return zFilename;
}

wchar_t * UTF8ToUnicode( const char* str )
{


	int textlen ;
	wchar_t * result;
	textlen = MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 );  
	result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));  
	memset(result,0,(textlen+1)*sizeof(wchar_t));  
	MultiByteToWideChar(CP_UTF8, 0,str,-1,(LPWSTR)result,textlen );  

	return result;  


}

char * UnicodeToANSI( const wchar_t* str )
{
	char* result;
	int textlen;
	textlen = WideCharToMultiByte( CP_ACP, 0, str, -1, NULL, 0, NULL, NULL );
	result =(char *)malloc((textlen+1)*sizeof(char));
	memset( result, 0, sizeof(char) * ( textlen + 1 ) );
	WideCharToMultiByte( CP_ACP, 0, str, -1, result, textlen, NULL, NULL );
	return result;
}

CString Utf8ToAnsi(const char* srcCode)
{
   
	wchar_t* temp = UTF8ToUnicode(srcCode);
	char* res = UnicodeToANSI(temp);

	CString strResult="";
	strResult.Format("%s",res);

	free(temp);
	free(res);

	return strResult;


}