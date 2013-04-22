#ifndef _CRYPT_UNIT
#define _CRYPT_UNIT
#include "Buffer.h"
#include <Wincrypt.h>
#include <winioctl.h>
#include <ntddndis.h>

//CString GenerateMD5Digest(const CString& SourceMessage);
//CBuffer EncryptData(const CString& Key,const CBuffer& InData);
//CBuffer DecryptData(const CString& Key,const CBuffer& InData);
//CString DataToCipherStr(const CString& Key,const CBuffer& InData);
//CBuffer CipherStrToData(const CString& Key,const CString& CipherStr);


//新版加解密函数
#define SESSION_KEY 0
#define RSA_KEY		1

extern TCHAR gb_Random_Key1[];
extern TCHAR gb_Random_Key2[];
extern TCHAR gb_Random_Key3[];
extern TCHAR gb_Random_Key4[];
extern TCHAR gb_Random_Key5[];
extern TCHAR gb_Random_Key6[];
extern TCHAR gb_Random_Key7[];
extern TCHAR gb_Random_Key8[];
extern TCHAR gb_Random_Key9[];
extern TCHAR gb_Random_Key10[];
extern TCHAR gb_Random_Key11[];
extern TCHAR gb_Random_Key12[];
extern TCHAR gb_Random_Key13[];
extern TCHAR gb_Random_Key14[];
extern TCHAR gb_Random_Key15[];
extern TCHAR gb_Random_Key16[];
extern TCHAR gb_Random_Key17[];
extern TCHAR gb_Random_Key18[];


extern TCHAR gb_XML_Const_Key[];
extern TCHAR gb_Exchg_Key[];
extern TCHAR gb_Cert_Hash_Key[];
extern TCHAR gb_Cert_Encrpt_Key[];

inline CBuffer GetMachineID();
inline CBuffer GetFirstNetCardMAC();
inline bool CheckRegKey(CBuffer& bufRegKey,CBuffer& bufCertHashkey);
inline CBuffer GenRegKey(CBuffer& bufMachineChara,CBuffer& bufCertHashkey);

//inline CBuffer GenerateSHADigest(const CBuffer& InData);
inline CBuffer GenerateMD5(CBuffer& InData,HCRYPTPROV  hCryptProv );
inline bool EncryptData(CBuffer& Key,int nKeyType,CBuffer& InData,CBuffer& OutData);
inline bool DecryptData(CBuffer& Key,int nKeyType,CBuffer& InData,CBuffer& OutData);
inline bool GenRSAKey(CBuffer& bufPubKey,CBuffer& bufPrivKey,CBuffer& bufExchgKeySeed);
inline CBuffer GenRandom(int nByteWidth);
inline CBuffer HexStrToNumber(const CString& strNumber);
inline CString NumberToHexStr(CBuffer& NumberBuf);

/*****内联函数实现*******/
inline CBuffer GetFirstNetCardMAC()
{
	CBuffer res;
	HKEY hNetCardRootKey;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,L"Software\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards",0,KEY_READ,&hNetCardRootKey) == ERROR_SUCCESS)
	{
		CBuffer bufStr(MAX_PATH*2);
		if(RegEnumKey(hNetCardRootKey,0,(LPWSTR)bufStr.GetBufP(),bufStr.GetBufLen()/2) == ERROR_SUCCESS)
		{
			HKEY hNetCardKey;
			if(RegOpenKeyEx(hNetCardRootKey,(LPWSTR)bufStr.GetBufP(),0,KEY_READ,&hNetCardKey) == ERROR_SUCCESS)
			{
				DWORD dwValueType,dwValueLen;
				dwValueLen = bufStr.GetBufLen();
				if(RegQueryValueEx(hNetCardKey,L"ServiceName",0,&dwValueType,bufStr.GetBufP(),&dwValueLen) == ERROR_SUCCESS)
				{
					CString strAdapterUUID((LPCTSTR)bufStr.GetBufP());
					bool bDeviceExist = false;
					if(QueryDosDevice(strAdapterUUID,(LPWSTR)bufStr.GetBufP(),bufStr.GetBufLen()/2) == 0)
					{
						//创建Dos设备
						CString strDevPath = CString(L"\\\\.\\")+ strAdapterUUID;
						DefineDosDevice(DDD_RAW_TARGET_PATH,(LPTSTR)bufStr.GetBufP(),strDevPath);
					}
					else
					{
						bDeviceExist = true;
					}

					if(bDeviceExist)
					{
						HANDLE hMAC;
						CString strDevPath = CString(L"\\\\.\\")+ strAdapterUUID;
						hMAC = CreateFile(strDevPath,0,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,INVALID_HANDLE_VALUE);
						if(hMAC != INVALID_HANDLE_VALUE)
						{
							ULONG uOidCode  = OID_802_3_PERMANENT_ADDRESS;
							DWORD dwMACLen;
							if(DeviceIoControl(hMAC,IOCTL_NDIS_QUERY_GLOBAL_STATS,&uOidCode,sizeof(uOidCode),
								bufStr.GetBufP(),bufStr.GetBufLen(),&dwMACLen,NULL))
							{
								//*(bufStr.GetBufP()+dwMACLen) = 0;
								//*(bufStr.GetBufP()+dwMACLen+1) = 0;
								//bufStr.SetDataLen(dwMACLen);
								//res = NumberToHexStr(bufStr);

								bufStr.SetDataLen(dwMACLen);
								res = bufStr;

							}

							CloseHandle(hMAC);
						}
					}
				}

				RegCloseKey(hNetCardKey);
			}
			
		}

		RegCloseKey(hNetCardRootKey);
	}
	return res;
}


inline CBuffer GetMachineID()
{
	CBuffer res;
	//res.Alloc(MAX_PATH);

	//bool bException = false;
 //   BYTE szCpu[16]  = { 0 };
 //   UINT uCpuID     = 0U;

	//
	//try
	//{
 //     _asm 
 //       {
 //           mov eax, 0
 //           cpuid
 //           mov dword ptr szCpu[0], ebx
 //           mov dword ptr szCpu[4], edx
 //           mov dword ptr szCpu[8], ecx
 //           mov eax, 1
 //           cpuid
 //           mov uCpuID, edx
 //       }
	//}
	//catch(...)
	//{
	//	bException = true;
	//}

	//if(!bException)
	//{
	//	res.AppendData(szCpu,strlen((const char*)szCpu));
	//	res.AppendData(&uCpuID,sizeof(uCpuID));
	//}

	res = GetFirstNetCardMAC();
	if(res.GetDataLen()<=0)
	{
		MessageBox(NULL,L"无法读取机器特征码!",L"系统错误",MB_OK|MB_ICONSTOP);
		ExitProcess(0);
	}
	return res;
}

inline bool CheckRegKey(CBuffer& bufRegKey,CBuffer& bufCertHashkey)
{
	bool res = false;

	CBuffer bufCharaData;
	bufCharaData.AppendData(GetMachineID());
	if(bufCharaData.GetDataLen()>0)
	{
		bufCharaData.AppendData(bufCertHashkey);

		HCRYPTPROV  hCryptProv = NULL;
		HCRYPTHASH  hHash = NULL;
		
		if(CryptAcquireContext(&hCryptProv,NULL,MS_STRONG_PROV,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT|CRYPT_SILENT))
		{
			if(CryptCreateHash(hCryptProv,CALG_SHA,0,0,&hHash))
			{
				if(CryptHashData(hHash,bufCharaData.GetBufP(),bufCharaData.GetDataLen(),0))
				{
					CBuffer bufCharaDataHash(64);
					DWORD dwCharaDataHashLen = bufCharaDataHash.GetBufLen();
					if(CryptGetHashParam(hHash,HP_HASHVAL,bufCharaDataHash.GetBufP(),&dwCharaDataHashLen,0))
					{
						bufCharaDataHash.SetDataLen(dwCharaDataHashLen);
						if(bufRegKey == bufCharaDataHash)
							res = true;
					}
				}
				CryptDestroyHash(hHash);
			}
			CryptReleaseContext(hCryptProv,0);
		}

	}

	return res;
}

inline bool EncryptData(CBuffer& Key,int nKeyType,CBuffer& InData,CBuffer& OutData)
{
	bool res = false;

	HCRYPTPROV  hCryptProv = NULL;
	HCRYPTHASH  hHash = NULL;
	HCRYPTKEY   hCryptKey = NULL;

	if(nKeyType == SESSION_KEY)
	{
		OutData.SetData(InData);
		DWORD dwDataLen = InData.GetDataLen();

		if(CryptAcquireContext(&hCryptProv,NULL,MS_STRONG_PROV,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT|CRYPT_SILENT))
		if(CryptCreateHash(hCryptProv,CALG_SHA,0,0,&hHash))
		if(CryptHashData(hHash,Key.GetBufP(),Key.GetDataLen(),0))
		if(CryptDeriveKey(hCryptProv,CALG_3DES,hHash,0,&hCryptKey))
		{
			//加密数据可能比明文长
			CryptEncrypt(hCryptKey,0,TRUE,0,OutData.GetBufP(),&dwDataLen,OutData.GetBufLen());
			int nError = GetLastError();

			if(nError == 0)
			{
				OutData.SetDataLen(dwDataLen);
				res = true;

			}
			if(nError == 234) //加密数据缓冲区长度不够
			{
				OutData.Alloc(dwDataLen);
				OutData.SetData(InData);
				dwDataLen = InData.GetDataLen();
				
				if(CryptEncrypt(hCryptKey,0,TRUE,0,OutData.GetBufP(),&dwDataLen,OutData.GetBufLen()))
				{
					OutData.SetDataLen(dwDataLen);
					res = true;
				}
			}


		}
	}
	else if(nKeyType == RSA_KEY)
	{

		OutData.SetData(InData);
		DWORD dwDataLen = InData.GetDataLen();

		HCRYPTKEY hExchgKey = NULL;
		CBuffer bufExchgKey = HexStrToNumber(gb_Exchg_Key);

		if(CryptAcquireContext(&hCryptProv,NULL,MS_STRONG_PROV,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT|CRYPT_SILENT))
		if(CryptCreateHash(hCryptProv,CALG_SHA,0,0,&hHash))
		if(CryptHashData(hHash,bufExchgKey.GetBufP(),bufExchgKey.GetDataLen(),0))
		if(CryptDeriveKey(hCryptProv,CALG_RC4,hHash,0x00800000,&hExchgKey))
		if(CryptImportKey(hCryptProv,Key.GetBufP(),Key.GetDataLen(),hExchgKey,0,&hCryptKey))
		{
			CryptEncrypt(hCryptKey,0,TRUE,0,OutData.GetBufP(),&dwDataLen,OutData.GetBufLen());
			int nError = GetLastError();

			if(nError == 0)
			{
				OutData.SetDataLen(dwDataLen);
				res = true;

			}
			if(nError == 234) //加密数据缓冲区长度不够
			{
				OutData.Alloc(dwDataLen);
				OutData.SetData(InData);
				dwDataLen = InData.GetDataLen();
				
				if(CryptEncrypt(hCryptKey,0,TRUE,0,OutData.GetBufP(),&dwDataLen,OutData.GetBufLen()))
				{
					OutData.SetDataLen(dwDataLen);
					res = true;
				}
			}

		}

		if(hExchgKey != NULL)
			CryptDestroyKey(hExchgKey);

	}

	if(hCryptKey != NULL)
		CryptDestroyKey (hCryptKey);

	if(hHash != NULL)
		CryptDestroyHash(hHash);

	if(hCryptProv != NULL)
		CryptReleaseContext(hCryptProv,0);

	return res;
}

inline bool DecryptData(CBuffer& Key,int nKeyType,CBuffer& InData,CBuffer& OutData)
{
	bool res = false;

	HCRYPTPROV  hCryptProv = NULL;
	HCRYPTHASH  hHash = NULL;
	HCRYPTKEY   hCryptKey = NULL;

	int nError;
	if(nKeyType == SESSION_KEY)
	{
		OutData.SetData(InData);
		DWORD dwDataLen = InData.GetDataLen();

		if(CryptAcquireContext(&hCryptProv,NULL,MS_STRONG_PROV,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT|CRYPT_SILENT))
		if(CryptCreateHash(hCryptProv,CALG_SHA,0,0,&hHash))
		if(CryptHashData(hHash,Key.GetBufP(),Key.GetDataLen(),0))
		if(CryptDeriveKey(hCryptProv,CALG_3DES,hHash,0,&hCryptKey))
		if(CryptDecrypt(hCryptKey,0,TRUE,0,OutData.GetBufP(),&dwDataLen))
		{
			OutData.SetDataLen(dwDataLen);
			res = true;
		}
		else
		{
			OutData.ClearData();
			res= false;
		}
	}
	else if(nKeyType == RSA_KEY)
	{
		OutData.SetData(InData);
		DWORD dwDataLen = InData.GetDataLen();

		if(CryptAcquireContext(&hCryptProv,NULL,MS_STRONG_PROV,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT|CRYPT_SILENT))
		if(CryptImportKey(hCryptProv,Key.GetBufP(),Key.GetDataLen(),NULL,0,&hCryptKey))
		if(CryptDecrypt(hCryptKey,0,TRUE,0,OutData.GetBufP(),&dwDataLen))
		{
			OutData.SetDataLen(dwDataLen);
			res = true;
		}
		nError = GetLastError();

	}

	if(hCryptKey != NULL)
		CryptDestroyKey(hCryptKey);

	if(hHash != NULL)
		CryptDestroyHash(hHash);

	if(hCryptProv != NULL)
		CryptReleaseContext(hCryptProv,0);

	return res;
}

inline bool GenRSAKey(CBuffer& bufPubKey,CBuffer& bufPrivKey,CBuffer& bufExchgKeySeed)
{
	bool res =false;

	bufPubKey.Alloc(1024);
	bufPrivKey.Alloc(1024);

	HCRYPTPROV  hCryptProv = NULL;
	HCRYPTKEY hRSAKey = NULL;
	HCRYPTKEY hExchgKey = NULL;
	HCRYPTHASH  hHash = NULL;


	//int nError;
	if(CryptAcquireContext(&hCryptProv,NULL,MS_STRONG_PROV,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT|CRYPT_SILENT))
	{
		if(CryptCreateHash(hCryptProv,CALG_SHA,0,0,&hHash))
		{
			if(CryptHashData(hHash,bufExchgKeySeed.GetBufP(),bufExchgKeySeed.GetDataLen(),0))
			if(CryptDeriveKey(hCryptProv,CALG_RC4,hHash,0x00800000,&hExchgKey))
			{
				DWORD dwKeyFlag = 0x200;
				dwKeyFlag = (dwKeyFlag<<16);
				dwKeyFlag |= CRYPT_EXPORTABLE;

				if(CryptGenKey(hCryptProv,AT_KEYEXCHANGE,dwKeyFlag,&hRSAKey))
				{
					DWORD dwKeyPubLen = bufPubKey.GetBufLen();
					DWORD dwKeyPrivLen = bufPrivKey.GetBufLen();

					if( CryptExportKey(hRSAKey,NULL,PUBLICKEYBLOB,0,bufPubKey.GetBufP(),&dwKeyPubLen) 
						&& CryptExportKey(hRSAKey,hExchgKey,PRIVATEKEYBLOB,0,bufPrivKey.GetBufP(),&dwKeyPrivLen))
					{
						bufPubKey.SetDataLen(dwKeyPubLen);
						bufPrivKey.SetDataLen(dwKeyPrivLen);

						res = true;
						HCRYPTKEY hImpKey = NULL;
						if(CryptImportKey(hCryptProv,bufPrivKey.GetBufP(),bufPrivKey.GetDataLen(),hExchgKey,0,&hImpKey))
						{
							CryptDestroyKey(hImpKey);
						}
					}
					//else
					//{
					//	nError =GetLastError();
					//}
					
					int nError =GetLastError();
					
					CryptDestroyKey(hRSAKey);
				}
				CryptDestroyKey(hExchgKey);
			}

			CryptDestroyHash(hHash);
		}
		CryptReleaseContext(hCryptProv,0);
	}



	return res;
}

inline CBuffer GenerateMD5(CBuffer& InData,HCRYPTPROV  hCryptProv = NULL)
{
	CBuffer res(16);

	//HCRYPTPROV  hCryptProv = NULL;
	HCRYPTHASH  hHash = NULL;

	bool bContextCreate = false;
	if(hCryptProv == NULL)
	{
		bContextCreate =true;
		CryptAcquireContext(&hCryptProv,NULL,MS_STRONG_PROV,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT|CRYPT_SILENT);
	}

	if(hCryptProv!=NULL)
	{
		if(CryptCreateHash(hCryptProv,CALG_MD5,0,0,&hHash))
		{
			if(CryptHashData(hHash,InData.GetBufP(),InData.GetDataLen(),0))
			{
				DWORD dwDataLen = res.GetBufLen();
				if(CryptGetHashParam(hHash,HP_HASHVAL,res.GetBufP(),&dwDataLen,0))
					res.SetDataLen(dwDataLen);
			}
			CryptDestroyHash(hHash);
		}

		if(bContextCreate)
			CryptReleaseContext(hCryptProv,0);
	}
	return res;
}

inline CBuffer HexStrToNumber(const CString& strNumber)
{
	CBuffer res(strNumber.GetLength());

	if(strNumber.GetLength()%2 == 0)
	{
		LPCTSTR pChBuf  =(LPCTSTR)strNumber;
		TCHAR ChgBuf[4];
		for(int i=0;i<strNumber.GetLength();i+=2)
		{
			memset(ChgBuf,0,sizeof(ChgBuf));
			ChgBuf[0] = pChBuf[i];
			ChgBuf[1] = pChBuf[i+1];
			TCHAR* pEndChar;
			ULONG uChgValue = wcstoul(ChgBuf,&pEndChar,16);
			if(pEndChar == (ChgBuf + 2))
			{
				BYTE  byChgValue = uChgValue & 0xff;
				res.AppendData(&byChgValue,sizeof(byChgValue));
			}
			else
			{
				res.SetDataLen(0);
				break;
			}
	 
		}
	}
	//CString strNumRe = NumberToHexStr(res);
	//if(strNumber == strNumRe)
	//{
	//	int s= 0;
	//}

	return res;
}


inline CString NumberToHexStr(CBuffer& NumberBuf)
{
	CString res;

	BYTE* pNumBerBuf = NumberBuf.GetBufP();
	for(int i=0;i<NumberBuf.GetDataLen();++i)
	{
		res.Format(L"%s%02.2X",CString(res),pNumBerBuf[i]);
	}
	return res;
}

inline CBuffer GenRandom(int nByteWidth)
{
	CBuffer res(nByteWidth);
	HCRYPTPROV  hCryptProv = NULL;
	if(CryptAcquireContext(&hCryptProv,NULL,NULL,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT|CRYPT_SILENT))
	{
		if(CryptGenRandom(hCryptProv,res.GetBufLen(),res.GetBufP()))
		{
			res.SetDataLen(res.GetBufLen());
		}
		CryptReleaseContext(hCryptProv,0);
	}
	//BYTE* pRandBuf = res.GetBufP();
	//srand( (unsigned)time( NULL ) );
	//for(int i=0;i<nByteWidth;++i)
	//{
	//	pRandBuf[i] = (rand()%256);
	//	
	//}
	//res.SetDataLen(nByteWidth);
	return res;
}



inline CBuffer GenRegKey(CBuffer& bufMachineChara,CBuffer& bufCertHashkey)
{
	CBuffer res;

	if(bufMachineChara.GetDataLen()>0)
	{
		CBuffer bufHash;
		bufHash.AppendData(bufMachineChara);
		bufHash.AppendData(bufCertHashkey);

		HCRYPTPROV  hCryptProv = NULL;
		HCRYPTHASH  hHash = NULL;
		
		if(CryptAcquireContext(&hCryptProv,NULL,MS_STRONG_PROV,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT|CRYPT_SILENT))
		{
			if(CryptCreateHash(hCryptProv,CALG_SHA,0,0,&hHash))
			{
				if(CryptHashData(hHash,bufHash.GetBufP(),bufHash.GetDataLen(),0))
				{
					res.Alloc(64);
					DWORD dwRegKeyLen = res.GetBufLen();
					if(CryptGetHashParam(hHash,HP_HASHVAL,res.GetBufP(),&dwRegKeyLen,0))
					{
						res.SetDataLen(dwRegKeyLen);
					}
				}
				CryptDestroyHash(hHash);
			}
			CryptReleaseContext(hCryptProv,0);
		}
	}

	return res;

}

inline CBuffer CalcHashValue(CBuffer& bufSrc,CBuffer& bufPostfix)
{
	CBuffer res;

	if(bufSrc.GetDataLen()>0)
	{
		CBuffer bufHash;
		bufHash.AppendData(bufSrc);
		bufHash.AppendData(bufPostfix);

		HCRYPTPROV  hCryptProv = NULL;
		HCRYPTHASH  hHash = NULL;
		
		if(CryptAcquireContext(&hCryptProv,NULL,MS_STRONG_PROV,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT|CRYPT_SILENT))
		{
			if(CryptCreateHash(hCryptProv,CALG_SHA,0,0,&hHash))
			{
				if(CryptHashData(hHash,bufHash.GetBufP(),bufHash.GetDataLen(),0))
				{
					res.Alloc(64);
					DWORD dwRegKeyLen = res.GetBufLen();
					if(CryptGetHashParam(hHash,HP_HASHVAL,res.GetBufP(),&dwRegKeyLen,0))
					{
						res.SetDataLen(dwRegKeyLen);
					}
				}
				CryptDestroyHash(hHash);
			}
			CryptReleaseContext(hCryptProv,0);
		}


	}


	return res;
}


inline CBuffer CalcHMACSHA1(CBuffer& bfKey,CBuffer& bfMessage)
{
	CBuffer res;

	DWORD dwError;

	HCRYPTPROV  hCryptProv = NULL;
	if(CryptAcquireContext(&hCryptProv,NULL,MS_ENHANCED_PROV,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT|CRYPT_SILENT))
	{
		//产生Key
		HCRYPTHASH hHashKey;
		if(CryptCreateHash(hCryptProv,CALG_SHA1,0,0,&hHashKey))
		{
			if(CryptHashData(hHashKey,bfKey.GetBufP(),bfKey.GetDataLen(),0))
			{
				HCRYPTKEY hKey;
				//if(CryptDeriveKey(hCryptProv,CALG_HMAC,hHashKey,0,&hKey))
				
				PUBLICKEYSTRUC headImpKey={0};
				headImpKey.bType =  PLAINTEXTKEYBLOB;
				headImpKey.aiKeyAlg = CALG_HMAC;
				headImpKey.bVersion =  CUR_BLOB_VERSION;
				CBuffer bfImpKeyHeadBody(&headImpKey,sizeof(headImpKey));
				bfImpKeyHeadBody.AppendData(bfKey);
				CRYPT_INTEGER_BLOB bolbImp;
				bolbImp.cbData = bfImpKeyHeadBody.GetDataLen();
				bolbImp.pbData = bfImpKeyHeadBody.GetBufP();
				
				if(CryptImportKey(hCryptProv,(BYTE*)&bolbImp,sizeof(bolbImp),NULL,0,&hKey))
				{
					//创建Message Hash
					HCRYPTHASH hHashMessage;
					if(CryptCreateHash(hCryptProv,CALG_HMAC,hKey,0,&hHashMessage))
					{
						HMAC_INFO infHMAC={0};
						infHMAC.HashAlgid = CALG_SHA1;
						if(CryptSetHashParam(hHashMessage,HP_HMAC_INFO,(const BYTE*)&infHMAC,0))
						{
							if(CryptHashData(hHashMessage,bfMessage.GetBufP(),bfMessage.GetDataLen(),0))
							{
								res.Alloc(128);
								DWORD dwLenMsgHash = res.GetBufLen();
								if(CryptGetHashParam(hHashMessage,HP_HASHVAL,res.GetBufP(),&dwLenMsgHash,0))
								{
									res.SetDataLen(dwLenMsgHash);
								}

							}
						}
						//else
						//{
						//	dwError = GetLastError();
						//}
						CryptDestroyHash(hHashMessage);
					}

					CryptDestroyKey(hKey);
				}

			}

			CryptDestroyHash(hHashKey);
		}


		CryptReleaseContext(hCryptProv,0);
	}

	return res;
}
#endif