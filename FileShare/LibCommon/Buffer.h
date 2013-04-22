#ifndef _BUFFER_
#define _BUFFER_
#include <string>
class CBuffer
{
public:
	CBuffer();
	~CBuffer();
	CBuffer(INT dwBufLen);
	CBuffer(const VOID* pData,INT dwDataLen);
	CBuffer(CBuffer& src);

	BOOL Attach(const VOID* pData,INT dwDataLen,INT dwBufLen);
	VOID Detch();

	bool operator ==(CBuffer& right);

	CBuffer operator +(CBuffer& right);
	bool AppendData(VOID* pData,INT dwDataLen);
	bool AppendData(CBuffer& DataToAdd);
	void InsertData(INT nPos,VOID* pData,INT dwDataLen);
	void InsertData(INT nPos,CBuffer& DataToAdd);
	
	VOID operator =(const CBuffer& right);
	BOOL Alloc(INT dwBufLen);
	BOOL ReAlloc(INT dwBufLen);//保留原数据
	VOID Release();
	BYTE* GetBufP() const;
	INT GetBufLen() const;
	BOOL SetData(const CBuffer& src);
	BOOL  SetData(const VOID* pData,INT dwDataLen);
	VOID ClearData();
	BOOL SetDataLen(INT dwDatalen);
	INT GetDataLen() const;
	inline BOOL IsFull()
	{
		return m_dwDataLen==m_dwBufLen?TRUE:FALSE;
	}
	inline BOOL IsEmpty()
	{
		return m_dwDataLen>0?false:true;
	}

	bool PaddData(int nToSize,BYTE byValue);

	CBuffer GetSubData(INT dwStart,INT dwLen);

	BOOL CutData(INT dwStart,INT dwCutLen);
	VOID CutHeadData(INT dwCutLen);
	VOID CutTailData(INT dwCutLen);

	std::string toStdString();
	CString toCString(bool bWidechar = true);
	bool toCString(UINT CodePage,CString& strRes);
protected:
	VOID InitMember();

protected:
	BYTE* m_pBuf;
	INT m_dwBufLen;
	INT m_dwDataLen;
};
#endif