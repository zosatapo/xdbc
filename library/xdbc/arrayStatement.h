/**
 *  arrayStatement.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef   __OPENCODE_XDBC_ARRAYSTATEMENT_H__
#define   __OPENCODE_XDBC_ARRAYSTATEMENT_H__

#include "xdbc/xdefine.h"
#include "xdbc/statement.h"
#include "xdbc/dataTable.h"
#include "xdbc/arrayResultSet.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

class __OPENCODE_XDBC_API_DECL ArrayException
{
public:
  ArrayException(int rowOffset,const SQLException& ex)
  :rowOffset(rowOffset),sqlex(ex){}

  int getRowOffset() { return rowOffset; }
  SQLException& getException() { return sqlex;}

private:
  int rowOffset;
  SQLException sqlex;
};

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

class __OPENCODE_XDBC_API_DECL ArrayStatement: public  SQLImplementation
{
protected:
  virtual ~ArrayStatement(){}

public:
  virtual int executeUpdate(__XDBC_HOOK_DECLARE_MORE DataTable* paramTable,vector<ArrayException>* errors) __XDBC_THROW_DECLARE(SQLException) = 0;

  virtual ArrayResultSet* executeQuery(__XDBC_HOOK_DECLARE_MORE DataTable* dataTable,DataTable* paramTable) __XDBC_THROW_DECLARE(SQLException) = 0;

public:
  virtual int  getMaxArraySize() const = 0;
  virtual void setMaxArraySize(int size) = 0;
};

__OPENCODE_END_NAMESPACE

#endif
