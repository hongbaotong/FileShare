#include "stdafx.h"
#include "CryptUnit.h"
#include "atlenc.h"


//CString GenerateMD5Digest(const CString& SourceMessage)
//{
//	HCRYPTPROV  hCryptProv;
//	HCRYPTHASH  hHash;
//	DWORD Len;
//	BYTE TempBuf[256],TempBuf2[256];
//	CString strDigest;
//
//	if(CryptAcquireContext(&hCryptProv,NULL,NULL,PROV_RSA_FULL,0))
//	{
//		if(CryptCreateHash(hCryptProv,CALG_MD5,0,0,&hHash))
//		{
//			if(CryptHashData(hHash,(BYTE*)(LPCTSTR(SourceMessage)),SourceMessage.GetLength(),0))
//			{
//				Len=sizeof(TempBuf);
//				if(CryptGetHashParam(hHash,HP_HASHVAL,TempBuf,&Len,0))
//				{
//					for(int i=0;i<Len;i++)
//					{
//						sprintf((char*)TempBuf2,"%02x",(unsigned int)TempBuf[i]);
//						strDigest=strDigest+(TCHAR*)TempBuf2;
//					}
//					return strDigest; 
//				}
//			}
//			CryptDestroyHash(hHash);
//		}
//		CryptReleaseContext(hCryptProv,0);
//	}
//	int h=GetLastError();
//	return L"";
//}
//CBuffer EncryptData(const CString& Key,const CBuffer& InData)
//{
//	HCRYPTPROV  hCryptProv;
//	HCRYPTHASH  hHash;
//	HCRYPTKEY   hCryptKey;
//
//	CBuffer Result((CBuffer&)InData);
//	DWORD DataLen=Result.GetDataLen();
//
//	if(!CryptAcquireContext(&hCryptProv,NULL,NULL,PROV_RSA_FULL,0))
//		if(!CryptAcquireContext(&hCryptProv,NULL,NULL,PROV_RSA_FULL,CRYPT_NEWKEYSET))
//			return CBuffer(0,NULL);
//
//	if(CryptCreateHash(hCryptProv,CALG_MD5,0,0,&hHash))
//	if(CryptHashData(hHash,(BYTE*)(LPCTSTR(Key)),Key.GetLength(),0))
//	if(CryptDeriveKey(hCryptProv,CALG_RC4,hHash,0,&hCryptKey))
//	if(CryptEncrypt(hCryptKey,0,TRUE,0,Result.GetBufP(),&DataLen,Result.GetDataLen()))
//		return Result;
//
//	int h=GetLastError();
//	return CBuffer(0,NULL);
//}
//CBuffer DecryptData(const CString& Key,const CBuffer& InData)
//{
//	HCRYPTPROV  hCryptProv;
//	HCRYPTHASH  hHash;
//	HCRYPTKEY   hCryptKey;
//
//	CBuffer Result((CBuffer&)InData);
//	DWORD DataLen=Result.GetBufLen();
//    int i=Key.GetLength();
//
//	if(CryptAcquireContext(&hCryptProv,NULL,NULL,PROV_RSA_FULL,0))
//	if(CryptCreateHash(hCryptProv,CALG_MD5,0,0,&hHash))
//	if(CryptHashData(hHash,(BYTE*)(LPCTSTR(Key)),Key.GetLength(),0))
//	if(CryptDeriveKey(hCryptProv,CALG_RC4,hHash,0,&hCryptKey))
//	if(CryptDecrypt(hCryptKey,0,TRUE,0,Result.GetBufP(),&DataLen))
//		return Result;
//
//	int h=GetLastError();
//	return CBuffer(0,NULL);
//}
//CString DataToCipherStr(const CString& Key,const CBuffer& InData)
//{
//	if(Key==L""||InData.GetBufLen()==0)
//		return L"";
//	CBuffer CipherData=EncryptData(Key,InData);
//	int ResultLen=Base64EncodeGetRequiredLength(CipherData.GetDataLen());
//	LPSTR TempStr=new char[ResultLen];
//	CString ResultStr;
//	if(Base64Encode(CipherData.GetBufP(),CipherData.GetDataLen(),TempStr,&ResultLen))
//	{
//		TempStr[ResultLen]=0;
//		ResultStr=TempStr;
//	}
//	delete TempStr;
//	return ResultStr;
//}
//CBuffer CipherStrToData(const CString& Key,const CString& CipherStr)
//{
//	if(Key==L""||CipherStr==L"")
//		return CBuffer(NULL,0);
//	
//	int BufSize=Base64DecodeGetRequiredLength(CipherStr.GetLength());
//	BYTE *TempBuf=new BYTE[BufSize];
//
//	if(Base64Decode((LPCSTR)(LPCTSTR)CipherStr,CipherStr.GetLength(),TempBuf,&BufSize))
//	{
//		CBuffer CipherData(TempBuf,BufSize);
//		delete TempBuf;
//		return DecryptData(Key,CipherData);	
//	}
//	else
//	{
//		delete TempBuf;
//		return CBuffer(0,NULL);
//	}
//}


//新版加解密函数
TCHAR gb_Random_Key1[] = L"6158483C4C6D1C06281E6B654C712A25";
TCHAR gb_Random_Key2[] = L"522E1543505B7F7C667C237A2F402773";
TCHAR gb_Random_Key3[] = L"22547E556A47542C29541F7C0D04064E";
TCHAR gb_Random_Key4[] = L"12242C452D4136502D141B7D40093C6B";
TCHAR gb_Random_Key5[] = L"324A1457472D0B00716C177E1E4D1B45";
TCHAR gb_Random_Key6[] = L"53440825313778193513110028132727";
TCHAR gb_Random_Key7[] = L"3BA9FE52CBB1FD4E2FC351F880237CDA";
TCHAR gb_Random_Key8[] = L"834B353D605470474F33730939344369";
TCHAR gb_Random_Key9[] = L"93437A25440C023D55126C0B473C5315";
TCHAR gb_Random_Key10[] = L"A33F1D5936684B795802690C4E405B2C";
TCHAR gb_Random_Key11[] = L"29D7508C533CB7D15119D9842EDFAECA";

TCHAR gb_XML_Const_Key[] = L"518D2F349A7D419CC9F218B2EE9157E6";

TCHAR gb_Random_Key12[] = L"535A1C6D2C3A577D63725C0F3C4E4E7E";
TCHAR gb_Random_Key13[] = L"34563F211E1620396662591143515614";
TCHAR gb_Random_Key14[] = L"527C2833380175692A3A55122116356E";
TCHAR gb_Random_Key15[] = L"644C56237A7C560D2E7A5013541B6B0C";
TCHAR gb_Random_Key16[] = L"A84BCE71AF72E1DDC17A513AE69A8424";
TCHAR gb_Random_Key17[] = L"73426D2557620C627511471666647F04";
TCHAR gb_Random_Key18[] = L"375B3D5346613C444139391A58347621";


TCHAR gb_Exchg_Key[] = L"732B6E2E3A397862671E37487F647C67";

TCHAR gb_Random_Key19[] = L"05293D5D010A42460731331C0F7B300A";
TCHAR gb_Random_Key20[] = L"151D2579561E1D781001281F2407484D";
TCHAR gb_Random_Key21[] = L"356D536919197E1D14402420570C7D6B";
TCHAR gb_Random_Key22[] = L"850F5E2F24601C085A081C233C54645C";
TCHAR gb_Random_Key23[] = L"555F0C1F675B7D2D5E4817246F591A79";
TCHAR gb_Random_Key24[] = L"952C0C292204032F2440112627205462";



TCHAR gb_Cert_Hash_Key[] = L"5259F2C856AE0F5E097363B6491E6006";

TCHAR gb_Random_Key25[] = L"F552743B3C6F585F67180D270464323C";
TCHAR gb_Random_Key26[] = L"66785D4D565B2D0F2B70092861291117";
TCHAR gb_Random_Key27[] = L"56480B3D18550F332F30042A152E4734";
TCHAR gb_Random_Key28[] = L"7768681D1D4B517C372F7B2C7D383270";
TCHAR gb_Random_Key29[] = L"56602D05000363733D0F742E0B40421D";

TCHAR gb_Cert_Encrpt_Key[] = L"F78AD06FEA62724C5467DBE834EA25CC";

TCHAR gb_Random_Key30[] = L"3602384B0C4A015E03576D316F08290D";
TCHAR gb_Random_Key31[] = L"C927A06A99D67584BD518AF2B3F16669";
TCHAR gb_Random_Key32[] = L"D6477D3D2B2B19574F2F5F3534567323";
TCHAR gb_Random_Key33[] = L"0DDB0C5CB7F3C76435FC688FC742D9AD";
TCHAR gb_Random_Key34[] = L"C7652B37284E007E186654381F23622A";
TCHAR gb_Random_Key35[] = L"573559266B4961231C26503953281747";
TCHAR gb_Random_Key36[] = L"2707367C34161E695F2E4D3A036B491A";
TCHAR gb_Random_Key37[] = L"172D1F0E4E0173192206493B612F2874";










