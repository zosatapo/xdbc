/** 
 *  stringBuffer.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
 
#ifndef __OPENCODE_STRINGBUFFER_H__
#define __OPENCODE_STRINGBUFFER_H__

#include "port/libdefs.h"

__OPENCODE_BEGIN_NAMESPACE


class __OPENCODE_EXPORT_DECL StringBuffer
{
//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
public:
  explicit StringBuffer(const string& rv);
  explicit StringBuffer(const char* rv);
  explicit StringBuffer(const char* rv,size_t __n);

  StringBuffer();
  StringBuffer(const StringBuffer& str);

//--------------------------------------------------------------------------
// Capacity methods
//--------------------------------------------------------------------------
public:
  size_t size() const; 
  
  size_t capacity() const;
  
  void resize(size_t __n);  
  
  void reserve(size_t __res_arg = 0);
      
  void clear();
  
  bool empty() const;

//--------------------------------------------------------------------------
// Element access
//--------------------------------------------------------------------------
public:
  const char& operator[] (size_t pos) const;
  char& operator[](size_t pos);
  
  const char& at(size_t pos) const;
  char& at(size_t pos);

  const string& str() const;

  string toString() const;   

//--------------------------------------------------------------------------
// Modifiers methods (+=)
//--------------------------------------------------------------------------
public:
  StringBuffer& operator+=(const StringBuffer& rv);
  StringBuffer& operator+=(const string& rv);
  StringBuffer& operator+=(const char*  rv);
  StringBuffer& operator+=(const char&  rv);

  StringBuffer& operator+=(const bool& rv);
  
  StringBuffer& operator+=(const short& rv);
  StringBuffer& operator+=(const unsigned short& rv);

  StringBuffer& operator+=(const int& rv);
  StringBuffer& operator+=(const unsigned int& rv);

  StringBuffer& operator+=(const long & rv);
  StringBuffer& operator+=(const unsigned long & rv);   
                            
  StringBuffer& operator+=(const long long & rv);
  StringBuffer& operator+=(const unsigned long long & rv);   

  StringBuffer& operator+=(const float& rv);
  StringBuffer& operator+=(const double& rv);
  
//--------------------------------------------------------------------------
// Modifiers methods (append)
//--------------------------------------------------------------------------
public:
  StringBuffer& append(const StringBuffer& rv);
  StringBuffer& append(const string& rv);
  StringBuffer& append(const char*  rv);
  StringBuffer& append(const char&  rv);

  StringBuffer& append(const bool& rv);
  
  StringBuffer& append(const short& rv);
  StringBuffer& append(const unsigned short& rv);

  StringBuffer& append(const int& rv);
  StringBuffer& append(const unsigned int& rv);

  StringBuffer& append(const long & rv);
  StringBuffer& append(const unsigned long & rv);   
                           
  StringBuffer& append(const long long & rv);
  StringBuffer& append(const unsigned long long & rv);   

  StringBuffer& append(const float& rv);
  StringBuffer& append(const double& rv);    
  
//--------------------------------------------------------------------------
// Modifiers methods (insert)
//--------------------------------------------------------------------------
public:
  StringBuffer& insert(size_t pos,const StringBuffer& rv);
  StringBuffer& insert(size_t pos,const string& rv);
  StringBuffer& insert(size_t pos,const char*  rv);
  StringBuffer& insert(size_t pos,const char&  rv);

  StringBuffer& insert(size_t pos,const bool& rv);
  
  StringBuffer& insert(size_t pos,const short& rv);
  StringBuffer& insert(size_t pos,const unsigned short& rv);

  StringBuffer& insert(size_t pos,const int& rv);
  StringBuffer& insert(size_t pos,const unsigned int& rv);

  StringBuffer& insert(size_t pos,const long & rv);
  StringBuffer& insert(size_t pos,const unsigned long & rv);   
                           
  StringBuffer& insert(size_t pos,const long long & rv);
  StringBuffer& insert(size_t pos,const unsigned long long & rv);  

  StringBuffer& insert(size_t pos,const float& rv);
  StringBuffer& insert(size_t pos,const double& rv);  
 
//--------------------------------------------------------------------------
// Modifiers methods (<<)
//--------------------------------------------------------------------------
public:
  StringBuffer& operator<<(const StringBuffer& rv);
  StringBuffer& operator<<(const string& rv);
  StringBuffer& operator<<(const char*  rv);
  StringBuffer& operator<<(const char&  rv);

  StringBuffer& operator<<(const bool& rv);
  
  StringBuffer& operator<<(const short& rv);
  StringBuffer& operator<<(const unsigned short& rv);

  StringBuffer& operator<<(const int& rv);
  StringBuffer& operator<<(const unsigned int& rv);

  StringBuffer& operator<<(const long & rv);
  StringBuffer& operator<<(const unsigned long & rv);   
                           
  StringBuffer& operator<<(const long long & rv);
  StringBuffer& operator<<(const unsigned long long & rv);  
    
  StringBuffer& operator<<(const float& rv);
  StringBuffer& operator<<(const double& rv);
 
//--------------------------------------------------------------------------
// Modifiers methods (delete)
//--------------------------------------------------------------------------
public:
  StringBuffer& erase(size_t pos,size_t size);
  
//--------------------------------------------------------------------------
// Modifiers methods (replace)
//--------------------------------------------------------------------------
public:
  StringBuffer& replace(size_t pos, size_t size, const string& rv);
  StringBuffer& replaceAll(size_t pos,const string& __src, const string& __dst);  
  
//--------------------------------------------------------------------------
// Finders methods (indexOf)
//--------------------------------------------------------------------------
public:
  size_t indexOf(const string& rv) const;
  size_t indexOf(const string& rv,size_t pos) const;
  
  size_t lastIndexOf(const string& rv) const;
  size_t lastIndexOf(const string& rv,size_t pos) const;  

  size_t indexOf(int ch) const;
  size_t indexOf(int ch,size_t pos) const;
  
  size_t lastIndexOf(int ch) const;
  size_t lastIndexOf(int ch,size_t pos) const;  
  
//--------------------------------------------------------------------------
// Substring methods
//--------------------------------------------------------------------------
public:   
  StringBuffer substring(size_t pos, size_t rv = string::npos) const;
  
//--------------------------------------------------------------------------
// Relationship operation methods 
//--------------------------------------------------------------------------
public:
  int compare(const StringBuffer& rv) const;
  int compare(const string& rv) const;
  int compare(const char* rv) const;
                  
//--------------------------------------------------------------------------
//Swap methods
//--------------------------------------------------------------------------
public:
  void swap(StringBuffer& sh);
  void swap(string& str);

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
private:
  string __internal_cached_buffer;  
};

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
__OPENCODE_EXPORT_DECL const StringBuffer operator+(const StringBuffer& lv,const StringBuffer& rv);
__OPENCODE_EXPORT_DECL const StringBuffer operator+(const StringBuffer& lv,const string& rv);
__OPENCODE_EXPORT_DECL const StringBuffer operator+(const StringBuffer& lv,const char*  rv);
__OPENCODE_EXPORT_DECL const StringBuffer operator+(const StringBuffer& lv,const char&  rv);

__OPENCODE_EXPORT_DECL const StringBuffer operator+(const StringBuffer& lv,const bool& rv);

__OPENCODE_EXPORT_DECL const StringBuffer operator+(const StringBuffer& lv,const short& rv);
__OPENCODE_EXPORT_DECL const StringBuffer operator+(const StringBuffer& lv,const unsigned short& rv);

__OPENCODE_EXPORT_DECL const StringBuffer operator+(const StringBuffer& lv,const int& rv);
__OPENCODE_EXPORT_DECL const StringBuffer operator+(const StringBuffer& lv,const unsigned int& rv);

__OPENCODE_EXPORT_DECL const StringBuffer operator+(const StringBuffer& lv,const long & rv);
__OPENCODE_EXPORT_DECL const StringBuffer operator+(const StringBuffer& lv,const unsigned long & rv);
  
__OPENCODE_EXPORT_DECL const StringBuffer operator+(const StringBuffer& lv,const long long& rv);
__OPENCODE_EXPORT_DECL const StringBuffer operator+(const StringBuffer& lv,const unsigned long long& rv);

__OPENCODE_EXPORT_DECL const StringBuffer operator+(const StringBuffer& lv,const float& rv);
__OPENCODE_EXPORT_DECL const StringBuffer operator+(const StringBuffer& lv,const double& rv);

__OPENCODE_END_NAMESPACE


#endif
