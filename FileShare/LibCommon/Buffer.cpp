#include "stdafx.h"
#include "buffer.h"
//#include "common.h"
#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p){delete(p);  (p)=NULL;} }

#endif
CBuffer::CBuffer()
{
	InitMember();
}

CBuffer::~CBuffer()
{
	Release();
}

VOID CBuffer::InitMember()
{
	m_pBuf = NULL;
	m_dwBufLen = 0;
	m_dwDataLen = 0;
}

CBuffer::CBuffer(INT dwBufLen)
{
	InitMember();
	Alloc(dwBufLen);
}

CBuffer::CBuffer(const VOID* pData,INT dwDataLen)
{	
	InitMember();
	SetData(pData,dwDataLen);
}
CBuffer::CBuffer(CBuffer& src)
{
	InitMember();
	SetData(src);
}

BOOL CBuffer::Attach(const VOID* pData,INT dwDataLen,INT dwBufLen)
{
	BOOL res = FALSE;
	if(pData&&dwBufLen>0)
	{
		Release();
		m_pBuf = (BYTE*)pData;
		m_dwBufLen = dwDataLen;
		m_dwDataLen = dwBufLen;
		res = TRUE;
	}
	return res;
}

VOID CBuffer::Detch()
{
	InitMember();
}

CBuffer CBuffer::operator +(CBuffer& right)
{
	CBuffer TempBuf(GetDataLen()+right.GetDataLen());
	if(TempBuf.GetBufLen()>0)
	{
		memcpy(TempBuf.GetBufP(),GetBufP(),GetDataLen());
		memcpy(TempBuf.GetBufP()+GetDataLen(),right.GetBufP(),right.GetDataLen());
		TempBuf.SetDataLen(GetDataLen()+right.GetDataLen());
	}
	return TempBuf;
}
bool CBuffer::operator ==(CBuffer& right)
{
	bool res = false;

	if(m_dwDataLen == right.GetDataLen())
	{
		res= true;
		for(int i=0;i<m_dwDataLen;++i)
		{
			BYTE* pRightBuf =right.GetBufP();
			if(m_pBuf[i] != pRightBuf[i])
			{
				res = false;
				break;
			}
		}
	}

	return res;
}

VOID CBuffer::operator =(const CBuffer& right)
{
	SetData(right);
}

BOOL CBuffer::Alloc(INT dwBufLen)
{
	BOOL res = TRUE;
	Release();
	if(dwBufLen > 0)
	{
		m_pBuf = new BYTE[dwBufLen];
		if(m_pBuf)
		{
			m_dwBufLen = dwBufLen;
			memset(m_pBuf,0,m_dwBufLen);
		}
		else
		{
			InitMember();
			res = FALSE;
		}
	}
	return res;
}

BOOL CBuffer::ReAlloc(INT dwBufLen)
{
	BOOL res = FALSE;

	if(dwBufLen>m_dwDataLen)
	{
		BYTE* pNewBuf = new BYTE[dwBufLen];
		if(m_dwDataLen>0)
			memcpy(pNewBuf,m_pBuf,m_dwDataLen);
		delete m_pBuf;
		m_pBuf = pNewBuf;
		m_dwBufLen = dwBufLen;
		res = TRUE;

	}

	return res;
}

VOID CBuffer::Release()
{
	if(m_pBuf)
	{
		delete m_pBuf;		
	}
	InitMember();
}

BYTE* CBuffer::GetBufP() const
{
	return m_pBuf;
}

INT CBuffer::GetBufLen() const
{
	return m_dwBufLen;
}

VOID CBuffer::ClearData()
{
	if(GetBufP())
	{
		memset(GetBufP(),0,GetBufLen());
		SetDataLen(0);
	}
		
}

BOOL CBuffer::SetData(const CBuffer& src)
{
	return	SetData(src.GetBufP(),src.GetDataLen());
}

BOOL  CBuffer::SetData(const VOID* pData,INT dwDataLen)
{
	BOOL res = FALSE;
	if(pData!=NULL && dwDataLen>0)
	{
		if(!GetBufP()||GetBufLen()< dwDataLen)
		{
			Alloc(dwDataLen);
		}
		ClearData();
		if(pData&&dwDataLen>0&&GetBufLen()>=dwDataLen)
		{
			memcpy(m_pBuf,pData,dwDataLen);
			SetDataLen(dwDataLen);
			res = TRUE;
		}
	}
	else
	{
		ClearData();
		res = TRUE;
	}
	return res;
}

BOOL CBuffer::SetDataLen(INT dwDatalen)
{
	BOOL res = FALSE;
	if(dwDatalen <= m_dwBufLen )
	{
		m_dwDataLen = dwDatalen;
		res =TRUE;
	}
	return res;
}

INT CBuffer::GetDataLen() const
{
	return m_dwDataLen;
}

//BOOL CBuffer::IsFull()
//{
//	return m_dwDataLen==m_dwBufLen?TRUE:FALSE;
//}
//
//BOOL CBuffer::IsEmpty()
//{
//	return m_dwDataLen>0?false:true;
//}

std::string CBuffer::toStdString()
{
	std::string res;
	if(GetDataLen()>0)
	{
		CBuffer TempBuf(GetDataLen()+1);
		TempBuf.SetData(GetBufP(),GetDataLen());
		res = std::string((CHAR*)TempBuf.GetBufP());
	}
	return res;
}

//CString CBuffer::toCString(bool bWidechar)
//{
//	if(GetDataLen()>0)
//	{
//		if(bWidechar)
//		{
//			CBuffer TempBuf(GetDataLen()+2);
//			TempBuf.SetData(GetBufP(),GetDataLen());
//			//PaddData(GetDataLen()+2,0);
//
//			return CString((wchar_t*)TempBuf.GetBufP());
//		}
//		else
//		{
//			CBuffer bufUTF16(GetDataLen()*2+2);
//			int nCovert = MultiByteToWideChar(CP_ACP,0,(LPSTR)GetBufP(),GetDataLen(),(LPWSTR)bufUTF16.GetBufP(),bufUTF16.GetBufLen()/2);
//			if(nCovert>0)
//			{
//				bufUTF16.SetDataLen(nCovert*2);
//				bufUTF16.PaddData(bufUTF16.GetDataLen()+2,0);
//
//				return CString((wchar_t*)bufUTF16.GetBufP());
//			}
//
//		//	return CString(TempBuf.GetBufP());
//		}
//	}
//	return CString("");
//}
CString CBuffer::toCString(bool bWidechar)
{
	if(GetDataLen()>0)
	{
		CBuffer TempBuf(GetDataLen()+2);
		TempBuf.SetData(GetBufP(),GetDataLen());
		TempBuf.PaddData(TempBuf.GetBufLen(),0);

		if(bWidechar)
		{
			return CString((wchar_t*)TempBuf.GetBufP());
		}
		else
		{
			return CString(TempBuf.GetBufP());
		}
	}
	return CString("");
}

bool CBuffer::toCString(UINT CodePage,CString& strRes)
{
	bool res = false;
	//转码
	CBuffer bufUTF16(GetDataLen()*2+2);
	int nLenTo = MultiByteToWideChar(CodePage,0,(LPCSTR)GetBufP(),GetDataLen(),(LPWSTR)bufUTF16.GetBufP(),bufUTF16.GetBufLen()/2);
	if(nLenTo>0)
	{
		strRes = (LPWSTR)bufUTF16.GetBufP();
		res = true;
	}
	return res;
}

VOID CBuffer::CutHeadData(INT dwCutLen)
{
	//if(dwCutLen >= GetDataLen())
	//	SetDataLen(0);
	//else
	//{
	//	memmove(GetBufP(),GetBufP()+dwCutLen,GetDataLen() - dwCutLen);
	//	SetDataLen(GetDataLen() - dwCutLen);
	//}
	CutData(0,dwCutLen);
}

VOID CBuffer::CutTailData(INT dwCutLen)
{
	CutData(GetDataLen()>dwCutLen?GetDataLen() - dwCutLen:0,dwCutLen);
}

BOOL CBuffer::CutData(INT dwStart,INT dwCutLen)
{
	BOOL res = FALSE;
	if(dwStart < GetDataLen())
	{
		INT dwMoveLen = dwStart + dwCutLen < GetDataLen()?GetDataLen() - (dwStart + dwCutLen ):0;
		if(dwMoveLen>0)
			memmove(GetBufP()+dwStart,GetBufP()+dwStart+dwCutLen,dwMoveLen);
		SetDataLen(dwStart+dwMoveLen);
		res = TRUE;
	}
	return res;
}

CBuffer CBuffer::GetSubData(INT dwStart,INT dwLen)
{
	CBuffer res;
	if(dwStart < GetDataLen())
	{
		dwLen = dwStart+dwLen< GetDataLen()?dwLen:GetDataLen() - dwStart;
		res.SetData(GetBufP()+dwStart,dwLen);
	}
	return res;
}

bool CBuffer::AppendData(VOID* pData,INT dwDataLen)
{
	if(pData==NULL || dwDataLen<=0)
		return false;

	bool res =false;
	if((dwDataLen + m_dwDataLen) > m_dwBufLen)
	{
		BYTE* pBufNew = new BYTE[dwDataLen+ m_dwDataLen];
		if(pBufNew != NULL)
		{
			if(m_pBuf != NULL)
				memcpy(pBufNew,m_pBuf,m_dwDataLen);
			SAFE_DELETE(m_pBuf);
			m_pBuf = pBufNew;
			m_dwBufLen = dwDataLen+ m_dwDataLen;

			memcpy(m_pBuf + m_dwDataLen,pData,dwDataLen);
			m_dwDataLen += dwDataLen;
			res = true;

		}
	}
	else
	{
		memcpy(m_pBuf + m_dwDataLen,pData,dwDataLen);
		m_dwDataLen += dwDataLen;
		res = true;
	}
	return res;
}

bool CBuffer::AppendData(CBuffer& DataToAdd)
{
	return AppendData(DataToAdd.GetBufP(),DataToAdd.GetDataLen());
}

void CBuffer::InsertData(INT nPos,VOID* pData,INT dwDataLen)
{
	if(pData==NULL || dwDataLen<=0 || (nPos<0&&nPos>m_dwDataLen))
		return ;

	//计算总长度并分配缓冲区
	DWORD dwResLen = m_dwDataLen +  dwDataLen;
	if(m_dwBufLen<dwResLen)
		ReAlloc(dwResLen);

	//移动原有数据为插入数据准备空间
	memmove(m_pBuf+nPos+dwDataLen,m_pBuf+nPos,m_dwDataLen-nPos);
	//复制插入数据
	memcpy(m_pBuf+nPos,pData,dwDataLen);
	SetDataLen(dwResLen);

}

void CBuffer::InsertData(INT nPos,CBuffer& DataToAdd)
{
	InsertData(nPos,DataToAdd.GetBufP(),DataToAdd.GetDataLen());
}

bool CBuffer::PaddData(int nToSize,BYTE byValue)
{
	bool res =false;
	if(nToSize>m_dwDataLen)
	{
		if(nToSize>m_dwBufLen)
		{
			BYTE* pBufNew = new BYTE[nToSize];
			if(pBufNew != NULL)
			{
				if(m_pBuf != NULL)
					memcpy(pBufNew,m_pBuf,m_dwDataLen);
				SAFE_DELETE(m_pBuf);
				m_pBuf = pBufNew;
				m_dwBufLen = nToSize;

				memset(m_pBuf+m_dwDataLen,byValue,nToSize - m_dwDataLen);
				m_dwDataLen = nToSize;
				res = true;
			}
		}
		else
		{
			memset(m_pBuf+m_dwDataLen,byValue,nToSize - m_dwDataLen);
			m_dwDataLen = nToSize;
			res = true;
		}

	}
	return res;
}