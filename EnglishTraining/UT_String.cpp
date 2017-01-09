#include "stdafx.h"
#include <windows.h>

wchar_t* MB2WC(char const* _sIn, DWORD _CodePage/* = CP_ACP*/){
  if(!_sIn)return NULL;
  int InLen_=(int)strlen(_sIn);
//  if(!InLen_)return NULL;
  InLen_++;
  int res_=MultiByteToWideChar(_CodePage,(_CodePage==CP_UTF7 || _CodePage==CP_UTF8)?0:MB_PRECOMPOSED,_sIn,InLen_,NULL,0);
  if(!res_ && _CodePage!=CP_ACP)
    if((res_=MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,_sIn,InLen_,NULL,0))>0)
      _CodePage=CP_ACP;
  if(res_<= 0)
    return NULL;
  wchar_t* ret_=new wchar_t[res_];
  if(!ret_)return NULL;
  ZeroMemory(ret_,res_*sizeof(wchar_t));
  if(!MultiByteToWideChar(_CodePage,(_CodePage==CP_UTF7 || _CodePage==CP_UTF8)?0:MB_PRECOMPOSED,_sIn,InLen_,ret_,res_)){
    delete[] ret_;
    ret_=NULL;
  }
  return ret_; }

bool MB2WC(char const* _sIn, wchar_t** _sOut, DWORD _CodePage/* = CP_ACP*/){
  if(!_sOut)return false;
  *_sOut=MB2WC(_sIn,_CodePage);
  return *_sOut!=NULL; }

bool MB2WC(char const* _sIn, wchar_t* _sOut, size_t& _OutLen, DWORD _CodePage/* = CP_ACP*/){
  if(!_sOut || !_OutLen)return false;
  ZeroMemory(_sOut,sizeof(wchar_t)*_OutLen);
  wchar_t* tmp_=MB2WC(_sIn,_CodePage);
  if(!tmp_)return false;
  size_t TmpLen_=wcslen(tmp_);
  bool ret_=false;
  if(TmpLen_<=_OutLen-1)
    ret_=(wcsncat_s(_sOut,_OutLen,tmp_,TmpLen_)==0);
  else
    _OutLen=TmpLen_+1;
  delete[] tmp_;
  return ret_; }

char* WC2MB(wchar_t const* _sIn, DWORD _CodePage/* = CP_ACP */, int _Len/* = -1 */){
  if(!_sIn)return NULL;
  int InLen_=_Len!=-1?_Len:(int)wcslen(_sIn);
//  if(!InLen_)return NULL;
  InLen_++;
  int rc_=WideCharToMultiByte(_CodePage,(_CodePage==CP_UTF7 || _CodePage==CP_UTF8)?0:WC_COMPOSITECHECK,_sIn,InLen_,NULL,0,NULL,NULL);
  if(!rc_ && _CodePage!=CP_ACP)
    if((rc_=WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,_sIn,InLen_,NULL,0,NULL,NULL))>0)
      _CodePage=CP_ACP;
  if(rc_<=0)
    return NULL;
  char* ret_=new char[rc_];
  if(!ret_)
    return NULL;
  ZeroMemory(ret_,rc_);
  if(!WideCharToMultiByte(_CodePage,(_CodePage==CP_UTF7 || _CodePage==CP_UTF8)?0:WC_COMPOSITECHECK,_sIn,InLen_,ret_,rc_,NULL,NULL)){
    delete[] ret_;
    ret_=NULL;
  }
  return ret_; }

bool WC2MB(wchar_t const* _sIn, char** _sOut, DWORD _CodePage){
  if(!_sOut)return false;
  *_sOut=WC2MB(_sIn,_CodePage);
  return *_sOut!=NULL; }

bool WC2MB(wchar_t const* _sIn, char* _sOut, size_t& _OutLen, DWORD _CodePage){
  if(!_sOut || !_OutLen)return false;
  ZeroMemory(_sOut,_OutLen);
  char* s_=WC2MB(_sIn,_CodePage);
  if(!s_)
    return false;
  size_t len_=strlen(s_);
  bool r_=false;
  if(len_<=_OutLen-1)
    r_=(strncat_s(_sOut,_OutLen,s_,len_)==0);
  else
    _OutLen=len_+1;
  delete[] s_;
  return r_; }

int CompareWideCharString(const wchar_t* _s1, const wchar_t* _s2, DWORD _Len){
  int nRet=CompareStringW(LOCALE_USER_DEFAULT,NORM_IGNORECASE,_s1,_Len,_s2,_Len);
  return nRet-CSTR_EQUAL; }  // Convert to strcmp convention. This really is documented.

size_t MakeString(wchar_t const* _Str, wchar_t** _pVar){
  if(!_Str)return 0;
  if(_pVar && *_pVar){
    delete[] *_pVar;
    *_pVar=NULL;
  }
  size_t ret_=(DWORD)wcslen(_Str);
  if(!ret_)return 0;
  if(!_pVar)return ret_;
//  *_pVar=(wchar_t*)malloc((ret_+2)*sizeof(wchar_t));
  *_pVar=new wchar_t[ret_+1];
  ZeroMemory(*_pVar,(ret_+1)*sizeof(wchar_t));
  // Trim from the right
  while(ret_>0 && _Str[ret_-1]==wchar_t(' '))ret_--; 
  // Trim from left
  wchar_t* Pos_=(wchar_t*)_Str; 
  while(Pos_ && *Pos_ && Pos_[0]==wchar_t(' ')) {
    Pos_++;
    ret_--;
  }
  if(Pos_ && *Pos_)wcsncat_s(*_pVar,ret_,Pos_,_TRUNCATE);
  return ret_; }

//////////////////////////////////////////////////////////////////////////

_Success_(return==true) bool IsFileExists(_In_z_ _Const_ const wchar_t* _File){
    if(_File && _File[0]){
        DWORD res_=GetFileAttributesW(_File);
        return (res_&INVALID_FILE_ATTRIBUTES)!=INVALID_FILE_ATTRIBUTES && (res_&FILE_ATTRIBUTE_DIRECTORY)!=FILE_ATTRIBUTE_DIRECTORY &&
            (res_&FILE_ATTRIBUTE_DEVICE)!=FILE_ATTRIBUTE_DEVICE && (res_&FILE_ATTRIBUTE_VIRTUAL)!=FILE_ATTRIBUTE_VIRTUAL;
    }
    return false;
}

_Success_(return==true) bool IsFileExists(_In_z_ _Const_ const char* _File){
    wchar_t s_[MAX_PATH]={}; MultiByteToWideChar(CP_ACP, 0, _File, -1, s_, MAX_PATH); return IsFileExists(s_);
}
