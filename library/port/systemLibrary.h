/** 
 *  systemLibrary.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
 
#ifndef __OPENCODE_SYSTEMLIBRARY_H__
#define __OPENCODE_SYSTEMLIBRARY_H__

#include "port/libdefs.h"
#include "port/file.h"
#include "port/ioc_dll.h" 

__OPENCODE_BEGIN_NAMESPACE

class __OPENCODE_CORE_API_DECL SystemLibrary
{
public:
  ~SystemLibrary() ;

  SystemLibrary();

private:
  SystemLibrary(const OPENCODE_TYPE_SOLIBRARY slh ,const string& pathname,XLONG openTime);

public:
  static SystemLibrary  getInstance(const string& pathname) __OPENCODE_THROW_DECLARE(IOException);

  OPENCODE_TYPE_SOSYMBOL findSymbol(const string& sym) const;

  void close();

  bool isOpened()const;
  XLONG getOpenTime() const;
  XLONG getCloseTime() const;
  const string& getPathname()const;

private:
  OPENCODE_TYPE_SOLIBRARY m_slh;

  string pathname; 
  XLONG openTime;
  XLONG closeTime;
};

__OPENCODE_CORE_API_DECL bool operator==(const SystemLibrary& lhs,const SystemLibrary& rhs);
__OPENCODE_CORE_API_DECL bool operator==(const SystemLibrary& lhs,const string& rhs);

__OPENCODE_END_NAMESPACE

#endif
