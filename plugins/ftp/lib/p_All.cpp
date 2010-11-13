#include <all_far.h>
#pragma hdrstop

#include <fstdlib.h>
#include "ftp_Plugin.h"

#if !defined(__USE_STD_FUNCIONS__)
#define __FP_INFO_FUNCTIONS__ 1
#endif

FTPInterface* FTP_Info = NULL;
HMODULE       FTP_Module = NULL;

/*******************************************************************
   INTERFACE
 *******************************************************************/
extern "C" FTPPluginInterface* WINAPI FTPQueryInterface(FTPInterface* Info)
{
	if(Info->Magic != FTP_INTERFACE_MAGIC ||
	        Info->SizeOf != sizeof(FTPInterface))
		return NULL;

	FTP_Info = Info;
	return FTPPluginGetInterface();
}

/*******************************************************************
   INTERFACE
 *******************************************************************/
#if defined(__BORLAND)
BOOL WINAPI DllEntryPoint(HINSTANCE hinst, DWORD reason, LPVOID ptr)
{
	BOOL rc;
	FTP_Module = (HMODULE)hinst;
	rc = FTP_PluginStartup(reason);

	if(reason == DLL_PROCESS_DETACH)
		FTP_Info = NULL;

	return rc;
}
#else
#if defined(__MSOFT) || defined(__GNU)
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID ptr)
{
	BOOL rc = FTP_PluginStartup(reason);

	if(reason == DLL_PROCESS_DETACH)
		FTP_Info = NULL;

	return rc;
}
#else
#error "Define plugin DLL entry point procedure for your  compiller"
#endif
#endif

/*******************************************************************
   CPP MEMORY
 *******************************************************************/
#if defined( __DEBUG__ )
void WINAPI _RTLCheck(LPCSTR fnm)
{
	if(!FTP_Info)
	{
		MessageBox(NULL,
		           "FTP_Info is not defined in FTP subplugin",
		           fnm ? fnm : "Error...", MB_OK);
		//abort();
	}
}
#endif


void __cdecl operator delete(void *ptr)           { RTLCheck("delete")      FTP_Info->Del(ptr); }
void *__cdecl operator new(size_t sz)             { RTLCheck("new")         return FTP_Info->Alloc(sz); }

#if defined(__BORLANDC__)
#if sizeof(size_t) < sizeof(long)
void *operator new(unsigned long sz)            { RTLCheck("new(long)")   return FTP_Info->Alloc(sz); }
#endif
#endif

#if defined( __HWIN__ ) || defined( __QNX__ )
void __cdecl operator delete[](void *ptr)      { RTLCheck("delete[]")    FTP_Info->Del(ptr); }
void *__cdecl operator new[](size_t sz)        { RTLCheck("new[]")       return FTP_Info->Alloc(sz); }
#endif

LPVOID WINAPI _Alloc(SIZE_T sz)              { RTLCheck("_Alloc")      return FTP_Info->Alloc(sz); }
void   WINAPI _Del(LPVOID ptr)               { RTLCheck("_Del")        FTP_Info->Del(ptr); }
LPVOID WINAPI _Realloc(LPVOID ptr,SIZE_T sz) { RTLCheck("_Realloc")    return FTP_Info->Realloc(ptr,sz); }
SIZE_T WINAPI _PtrSize(LPVOID ptr)           { RTLCheck("_PtrSize")    return FTP_Info->PtrSize(ptr); }
BOOL   WINAPI _HeapCheck(void)               { RTLCheck("_HeapCheck")  return FTP_Info->HeapCheck(); }

/*******************************************************************
   String functions
 *******************************************************************/
#if defined( __FP_INFO_FUNCTIONS__ )
int   WINAPI strLen(LPCSTR str)            { RTLCheck("strLen")  return FTP_Info->strLen(str); }
int   WINAPI StrCmp(LPCSTR str,LPCSTR str1,int maxlen, BOOL isCaseSens) { RTLCheck("StrCmp")  return FTP_Info->StrCmp(str,str1,maxlen,isCaseSens); }
char* WINAPI StrCpy(char *dest,LPCSTR src,int dest_sz) { RTLCheck("StrCpy")  return FTP_Info->StrCpy(dest,src,dest_sz); }
#else
int   WINAPI strLen(LPCSTR s)              { return s ? strlen(s) : 0; }

int WINAPI StrCmp(LPCSTR str,LPCSTR str1,int maxlen /*= -1*/, BOOL isCaseSens /*= FALSE*/)
{
	if(!str)  return (str1 == NULL) ? 0 : (-1);

	if(!str1) return 1;

	int rc;
	rc = CompareString(LOCALE_USER_DEFAULT,
	                   isCaseSens ? 0 : NORM_IGNORECASE,
	                   str,maxlen,str1, maxlen);
	return rc-2;
}

char *WINAPI StrCpy(char *dest,LPCSTR src,int dest_sz)
{
	if(!dest)        return NULL;

	if(dest_sz == 0) return dest;

	if(!src)         { *dest = 0; return dest; }

	if(dest_sz != -1)
	{
		strncpy(dest,src,dest_sz-1);
		dest[dest_sz-1] = 0;
	}
	else
		strcpy(dest,src);

	return dest;
}
#endif

char *WINAPI StrDup(LPCSTR m)
{
	char *rc;
	rc = (char*)_Alloc(strLen(m)+1);
	Assert(rc);
	return StrCpy(rc,m);
}

/*******************************************************************
   Format output
 *******************************************************************/
#if defined( __FP_INFO_FUNCTIONS__ )
int WINAPI    VSNprintf(char *Buff,size_t cn,LPCSTR Fmt,va_list arglist)  { RTLCheck("VSNprintf") return FTP_Info->VSNprintf(Buff,cn,Fmt,arglist); }
int WINAPI    VSprintf(char *Buff,LPCSTR Fmt,va_list arglist)             { RTLCheck("VSprintf")  return FTP_Info->VSprintf(Buff,Fmt,arglist); }
#else
int WINAPI VSprintf(char *Buff,LPCSTR Fmt,va_list arglist)             { return vsprintf(Buff,Fmt,arglist); }
int WINAPI VSNprintf(char *Buff,size_t cn,LPCSTR Fmt,va_list arglist)  { return vsnprintf(Buff,cn,Fmt,arglist); }
#endif

int _cdecl Sprintf(char *Buff,LPCSTR Fmt,...)
{
	va_list a;
	int     res;
	va_start(a, Fmt);
	res = VSprintf(Buff,Fmt,a);
	va_end(a);
	return res;
}

int _cdecl SNprintf(char *Buff,size_t cn,LPCSTR Fmt,...)
{
	va_list a;
	int     res;
	va_start(a, Fmt);
	res = VSNprintf(Buff,cn,Fmt,a);
	va_end(a);
	return res;
}
