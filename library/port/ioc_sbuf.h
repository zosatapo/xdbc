#ifndef __OPENCODE_PORTABLE_STRINGBUFFER_H__
#define __OPENCODE_PORTABLE_STRINGBUFFER_H__

#include "port/ioc_G_config.h"
#include "port/ioc_strings.h"

__OPENCODE_BEGIN_NAMESPACE

class  StringBuffer
{
public:
  explicit StringBuffer(const string& __sb_str)
  :__internal_cached_buffer(__sb_str){}
  	
  explicit StringBuffer(const char*  __sb_s)
  {
  	__internal_cached_buffer.assign(__sb_s,strlen(__sb_s));
  }
  
  explicit StringBuffer(const char*  __sb_s,size_t  __sb_size)
  {
  	__internal_cached_buffer.assign(__sb_s,__sb_size);
  }
  
  StringBuffer(){}
  StringBuffer(const StringBuffer& __sb_val)
  :__internal_cached_buffer(__sb_val.__internal_cached_buffer){}

//--------------------------------------------------------------------------
// Capacity methods
//--------------------------------------------------------------------------
public:
  __OPENCODE_INLINE size_t size() const
  {
  	return __internal_cached_buffer.size();
  }  
  
  __OPENCODE_INLINE size_t capacity() const
  {
  	return __internal_cached_buffer.capacity();
  } 

  __OPENCODE_INLINE void resize(size_t  __sb_size)
  {
  	 __internal_cached_buffer.resize(__sb_size);
  }  
  
  __OPENCODE_INLINE void reserve(size_t __sb_size = 0)
  { 
  	__internal_cached_buffer.reserve(__sb_size);
  }
      
  __OPENCODE_INLINE void clear()
  { 
  	__internal_cached_buffer.clear();
  }  
  
  __OPENCODE_INLINE bool empty() const
  { 
  	return __internal_cached_buffer.empty();
  }  

//--------------------------------------------------------------------------
// Element access
//--------------------------------------------------------------------------
public:
  __OPENCODE_INLINE const char& operator[] (size_t __sb_pos) const
  {
		return __internal_cached_buffer[__sb_pos];
	}
	
  __OPENCODE_INLINE char& operator[](size_t __sb_pos)
  {
  	return __internal_cached_buffer[__sb_pos];
  }
  
  __OPENCODE_INLINE const char& at(size_t __sb_pos) const
  {
  	return __internal_cached_buffer.at(__sb_pos);
  }
  
  __OPENCODE_INLINE char& at(size_t __sb_pos)
  {
  	return __internal_cached_buffer.at(__sb_pos);
  }

  __OPENCODE_INLINE const string& str() const
  {
  	return __internal_cached_buffer;
  }
  
  __OPENCODE_INLINE string toString() const
  {
  	return __internal_cached_buffer;  
	}
	
//--------------------------------------------------------------------------
// Modifiers methods (+=)
//--------------------------------------------------------------------------
public:
  __OPENCODE_INLINE StringBuffer& operator+=(const StringBuffer&  __sb_val)
  {
  	return this->append(__sb_val);
  }
  
  __OPENCODE_INLINE StringBuffer& operator+=(const string& __sb_str)
  {
  	return this->append(__sb_str);
  }
  
  __OPENCODE_INLINE StringBuffer& operator+=(const char*  __sb_s)
  {
  	return this->append(__sb_s);
  }
  
  __OPENCODE_INLINE StringBuffer& operator+=(const char&  __sb_char)
  {
  	return this->append(__sb_char);
  }

  __OPENCODE_INLINE StringBuffer& operator+=(const bool&  __sb_bool)
  {
  	return this->append(__sb_bool);
  }
  
  __OPENCODE_INLINE StringBuffer& operator+=(const short& __sb_si)
  {
  	return this->append(__sb_si);
  }
  
  __OPENCODE_INLINE StringBuffer& operator+=(const unsigned short&  __sb_usi)
  {
  	return this->append(__sb_usi);
  }

  __OPENCODE_INLINE StringBuffer& operator+=(const int&  __sb_i)
  {
  	return this->append(__sb_i);
  }
  
  __OPENCODE_INLINE StringBuffer& operator+=(const unsigned int& __sb_ui)
  {
  	return this->append(__sb_ui);
  }

  __OPENCODE_INLINE StringBuffer& operator+=(const long & __sb_li)
  {
  	return this->append(__sb_li);
  }
  
  __OPENCODE_INLINE StringBuffer& operator+=(const unsigned long & __sb_uli)
  {
  	return this->append(__sb_uli);
  }   
                            
  __OPENCODE_INLINE StringBuffer& operator+=(const long long & __sb_lli)
  {
  	return this->append(__sb_lli);
  }
  
  __OPENCODE_INLINE StringBuffer& operator+=(const unsigned long long & __sb_ulli)
  {
  	return this->append(__sb_ulli);
  } 

  __OPENCODE_INLINE StringBuffer& operator+=(const float& __sb_f)
  {
  	return this->append(__sb_f);
  }
  
  __OPENCODE_INLINE StringBuffer& operator+=(const double&  __sb_d)
  {
  	return this->append(__sb_d);
  }
  
//--------------------------------------------------------------------------
// Modifiers methods (append)
//--------------------------------------------------------------------------
public:
  __OPENCODE_INLINE StringBuffer& append(const StringBuffer&  __sb_val)
	{
	  __internal_cached_buffer.append(__sb_val.__internal_cached_buffer);
	  return *this;
	}
  
  __OPENCODE_INLINE StringBuffer& append(const string& __sb_str)
	{
	  __internal_cached_buffer.append(__sb_str);
	  return *this;
	}

  __OPENCODE_INLINE StringBuffer& append(const char*  __sb_s)
	{
	  __internal_cached_buffer.append(__sb_s);
	  return *this;
	}

  __OPENCODE_INLINE StringBuffer& append(const char&  __sb_char)
	{
	  __internal_cached_buffer.append(&__sb_char,1);
	  return *this;
	}

  __OPENCODE_INLINE StringBuffer& append(const bool&  __sb_bool)
	{
	  if(__sb_bool)
	  {
	    __internal_cached_buffer.append("true"); 
	  }
	  else
	  {
	    __internal_cached_buffer.append("false"); 
	  }
	  
	  return *this;
	}
  
  __OPENCODE_INLINE StringBuffer& append(const short& __sb_si)
	{
	  char __internal_buf[1024] = {0};
	  sprintf(__internal_buf,"%d",__sb_si);
	  __internal_cached_buffer.append(__internal_buf);
	  
	  return *this;
	}

  __OPENCODE_INLINE StringBuffer& append(const unsigned short&  __sb_usi)
  {
    char __internal_buf[1024] = {0};
    sprintf(__internal_buf,"%u",__sb_usi);
    __internal_cached_buffer.append(__internal_buf);
    
    return *this;
  }

  __OPENCODE_INLINE StringBuffer& append(const int&  __sb_i)
  {
    char __internal_buf[1024] = {0};
    sprintf(__internal_buf,"%d",__sb_i);
    __internal_cached_buffer.append(__internal_buf);
    
    return *this;
  }

  __OPENCODE_INLINE StringBuffer& append(const unsigned int&  __sb_ui)
  {
    char __internal_buf[1024] = {0};
    sprintf(__internal_buf,"%u",__sb_ui);
    __internal_cached_buffer.append(__internal_buf);
    
    return *this;
  }

  __OPENCODE_INLINE StringBuffer& append(const long & __sb_li)
  {
    char __internal_buf[1024] = {0};
    sprintf(__internal_buf,"%ld",__sb_li);
    __internal_cached_buffer.append(__internal_buf);
    
    return *this;
  }

  __OPENCODE_INLINE StringBuffer& append(const unsigned long & __sb_uli)  
  {
    char __internal_buf[1024] = {0};
    sprintf(__internal_buf,"%lu",__sb_uli);
    __internal_cached_buffer.append(__internal_buf);
    
    return *this;
  }
                           
  __OPENCODE_INLINE StringBuffer& append(const long long & __sb_lli)
  {
    char __internal_buf[1024] = {0};
    sprintf(__internal_buf,"%lld",__sb_lli);
    __internal_cached_buffer.append(__internal_buf);
    
    return *this;
  }

  __OPENCODE_INLINE StringBuffer& append(const unsigned long long & __sb_ulli)  
  {
    char __internal_buf[1024] = {0};
    sprintf(__internal_buf,"%llu",__sb_ulli);
    __internal_cached_buffer.append(__internal_buf);
    
    return *this;
  }

  __OPENCODE_INLINE StringBuffer& append(const float& __sb_f)
  {
    char __internal_buf[1024] = {0};
    sprintf(__internal_buf,"%g",__sb_f);
    __internal_cached_buffer.append(__internal_buf);
    
    return *this;
  }

  __OPENCODE_INLINE StringBuffer& append(const double&  __sb_d)    
  {
    char __internal_buf[1024] = {0};
    sprintf(__internal_buf,"%g",__sb_d);
    __internal_cached_buffer.append(__internal_buf);
    
    return *this;
  }  
  
//--------------------------------------------------------------------------
// Modifiers methods (insert)
//--------------------------------------------------------------------------
public:
  __OPENCODE_INLINE StringBuffer& insert(size_t __sb_pos,const StringBuffer&  __sb_val)
	{
	 __internal_cached_buffer.insert(__sb_pos,__sb_val.__internal_cached_buffer);  
	  return *this;
	}  
  
  __OPENCODE_INLINE StringBuffer& insert(size_t __sb_pos,const string& __sb_str)
	{
	  __internal_cached_buffer.insert(__sb_pos,__sb_str);  
	  return *this;
	} 
	
  __OPENCODE_INLINE StringBuffer& insert(size_t __sb_pos,const char*  __sb_s)
	{
	  __internal_cached_buffer.insert(__sb_pos,__sb_s);
	  return *this;
	} 

  __OPENCODE_INLINE StringBuffer& insert(size_t __sb_pos,const char&  __sb_char)
	{
	  __internal_cached_buffer.insert(__sb_pos,&__sb_char,1); 
	  return *this;
	} 
	
  __OPENCODE_INLINE StringBuffer& insert(size_t __sb_pos,const bool&  __sb_bool)
	{
	  if(__sb_bool)
	  {
	    __internal_cached_buffer.insert(__sb_pos,"true"); 
	  }
	  else
	  {
	    __internal_cached_buffer.insert(__sb_pos,"false"); 
	  }
	  
	  return *this;
	
	}
  
  __OPENCODE_INLINE StringBuffer& insert(size_t __sb_pos,const short& __sb_si)
  {
	  char __internal_buf[1024] = {0};
	  sprintf(__internal_buf,"%d",__sb_si);
	  __internal_cached_buffer.insert(__sb_pos,__internal_buf);
	
	  return *this;
  }
  
  __OPENCODE_INLINE StringBuffer& insert(size_t __sb_pos,const unsigned short&  __sb_usi)
  {
	  char __internal_buf[1024] = {0};
	  sprintf(__internal_buf,"%u",__sb_usi);
	  __internal_cached_buffer.insert(__sb_pos,__internal_buf);
	
	  return *this;
  }

  __OPENCODE_INLINE StringBuffer& insert(size_t __sb_pos,const int&  __sb_i)
  {
	  char __internal_buf[1024] = {0};
	  sprintf(__internal_buf,"%d",__sb_i);
	  __internal_cached_buffer.insert(__sb_pos,__internal_buf);
	
	  return *this;
	}
	
  __OPENCODE_INLINE StringBuffer& insert(size_t __sb_pos,const unsigned int&  __sb_ui)
  {
	  char __internal_buf[1024] = {0};
	  sprintf(__internal_buf,"%u",__sb_ui);
	  __internal_cached_buffer.insert(__sb_pos,__internal_buf);
	
	  return *this;
  }

  __OPENCODE_INLINE StringBuffer& insert(size_t __sb_pos,const long & __sb_li)
	{
	  char __internal_buf[1024] = {0};
	  sprintf(__internal_buf,"%ld",__sb_li);
	  __internal_cached_buffer.insert(__sb_pos,__internal_buf);
	
	  return *this;
	} 

  __OPENCODE_INLINE StringBuffer& insert(size_t __sb_pos,const unsigned long & __sb_uli)
	{
	  char __internal_buf[1024] = {0};
	  sprintf(__internal_buf,"%lu",__sb_uli);
	  __internal_cached_buffer.insert(__sb_pos,__internal_buf);
	
	  return *this;
	}       
                           
  __OPENCODE_INLINE StringBuffer& insert(size_t __sb_pos,const long long & __sb_lli)
	{
	  char __internal_buf[1024] = {0};
	  sprintf(__internal_buf,"%lld",__sb_lli);
	  __internal_cached_buffer.insert(__sb_pos,__internal_buf);
	
	  return *this;
	} 
  
  __OPENCODE_INLINE StringBuffer& insert(size_t __sb_pos,const unsigned long long & __sb_ulli) 
	{
	  char __internal_buf[1024] = {0};
	  sprintf(__internal_buf,"%llu",__sb_ulli);
	  __internal_cached_buffer.insert(__sb_pos,__internal_buf);
	
	  return *this;
	} 

  __OPENCODE_INLINE StringBuffer& insert(size_t __sb_pos,const float& __sb_f)
	{
	  char __internal_buf[1024] = {0};
	  sprintf(__internal_buf,"%g",__sb_f);
	  __internal_cached_buffer.insert(__sb_pos,__internal_buf);
	
	  return *this;
	}   
  
  __OPENCODE_INLINE StringBuffer& insert(size_t __sb_pos,const double&  __sb_d)
  {
	  char __internal_buf[1024] = {0};
	  sprintf(__internal_buf,"%g",__sb_d);
	  __internal_cached_buffer.insert(__sb_pos,__internal_buf);
	
	  return *this;
  }   
 
//--------------------------------------------------------------------------
// Modifiers methods (<<)
//--------------------------------------------------------------------------
public:
  __OPENCODE_INLINE StringBuffer& operator<<(const StringBuffer& __sb_val)
  {
  	return this->append(__sb_val);
  }
  
  __OPENCODE_INLINE StringBuffer& operator<<(const string& __sb_str)
  {
  	return this->append(__sb_str);
  }
  
  __OPENCODE_INLINE StringBuffer& operator<<(const char*  __sb_s)
  {
  	return this->append(__sb_s);
  }
  
  __OPENCODE_INLINE StringBuffer& operator<<(const char&  __sb_char)
  {
  	return this->append(__sb_char);
  }

  __OPENCODE_INLINE StringBuffer& operator<<(const bool&  __sb_bool)
  {
  	return this->append(__sb_bool);
  }
  
  __OPENCODE_INLINE StringBuffer& operator<<(const short& __sb_si)
  {
  	return this->append(__sb_si);
  }
  
  __OPENCODE_INLINE StringBuffer& operator<<(const unsigned short&  __sb_usi)
  {
  	return this->append(__sb_usi);
  }

  __OPENCODE_INLINE StringBuffer& operator<<(const int&  __sb_i)
  {
  	return this->append(__sb_i);
  }
  
  __OPENCODE_INLINE StringBuffer& operator<<(const unsigned int& __sb_ui)
  {
  	return this->append(__sb_ui);
  }

  __OPENCODE_INLINE StringBuffer& operator<<(const long & __sb_li)
  {
  	return this->append(__sb_li);
  }
  
  __OPENCODE_INLINE StringBuffer& operator<<(const unsigned long & __sb_uli)
  {
  	return this->append(__sb_uli);
  }   
                            
  __OPENCODE_INLINE StringBuffer& operator<<(const long long & __sb_lli)
  {
  	return this->append(__sb_lli);
  }
  
  __OPENCODE_INLINE StringBuffer& operator<<(const unsigned long long & __sb_ulli)
  {
  	return this->append(__sb_ulli);
  } 

  __OPENCODE_INLINE StringBuffer& operator<<(const float& __sb_f)
  {
  	return this->append(__sb_f);
  }
  
  __OPENCODE_INLINE StringBuffer& operator<<(const double&  __sb_d)
  {
  	return this->append(__sb_d);
  }
 
//--------------------------------------------------------------------------
// Modifiers methods (delete)
//--------------------------------------------------------------------------
public:
  __OPENCODE_INLINE StringBuffer& erase(size_t __sb_pos,size_t  __sb_size)
  {
    __internal_cached_buffer.erase(__sb_pos,__sb_size);
    return *this;  
  }
  
//--------------------------------------------------------------------------
// Modifiers methods (replace)
//--------------------------------------------------------------------------
public:
  __OPENCODE_INLINE StringBuffer& replace(size_t __sb_pos, size_t  __sb_size, const string& __sb_str)
  {
    __internal_cached_buffer.replace(__sb_pos,__sb_size,__sb_str);
    return *this;  
  }
  
  __OPENCODE_INLINE StringBuffer& replaceAll(size_t __sb_pos,const string& __src, const string& __dst)
  {
	  if(__src != __dst )
	  {
	    size_t prevPos = __sb_pos;
	    size_t newPos  = 0;
	    
	    while((newPos = __internal_cached_buffer.find_first_of(__src,prevPos)) != string::npos)
	    {
	      prevPos = newPos;
	      __internal_cached_buffer.replace(newPos,__src.length(),__dst);
	    }
	  }
	  
	  return *this;    	  
	}
//--------------------------------------------------------------------------
// Finders methods (indexOf)
//--------------------------------------------------------------------------
public:
  __OPENCODE_INLINE size_t indexOf(const string& __sb_str) const
  {
  	return __internal_cached_buffer.find_first_of(__sb_str); 
  }   
  
  __OPENCODE_INLINE size_t indexOf(const string& __sb_str,size_t __sb_pos) const
  {
  	return __internal_cached_buffer.find_first_of(__sb_str,__sb_pos); 
  }   
  
  __OPENCODE_INLINE size_t lastIndexOf(const string& __sb_str) const
  {
  	return __internal_cached_buffer.find_last_of(__sb_str); 
  }   
  
  __OPENCODE_INLINE size_t lastIndexOf(const string& __sb_str,size_t __sb_pos) const
  {
  	return __internal_cached_buffer.find_last_of(__sb_str,__sb_pos); 
  }   

  __OPENCODE_INLINE size_t indexOf(int __sb_char) const
  {
  	return __internal_cached_buffer.find_first_of((char)__sb_char); 
  }  
  
  __OPENCODE_INLINE size_t indexOf(int __sb_char,size_t __sb_pos) const
  {
  	return __internal_cached_buffer.find_first_of((char)__sb_char,__sb_pos); 
  }  
  
  __OPENCODE_INLINE size_t lastIndexOf(int __sb_char) const
  {
  	return __internal_cached_buffer.find_last_of((char)__sb_char); 
  }  
  
  __OPENCODE_INLINE size_t lastIndexOf(int __sb_char,size_t __sb_pos) const
  {
  	return __internal_cached_buffer.find_last_of((char)__sb_char,__sb_pos); 
  }  
  
//--------------------------------------------------------------------------
// Substring methods
//--------------------------------------------------------------------------
public:   
  __OPENCODE_INLINE StringBuffer substring(size_t __sb_pos, size_t __sb_size = string::npos) const
	{
	  StringBuffer __sb_tmp;  
	  __sb_tmp += __internal_cached_buffer.substr(__sb_pos,__sb_size); 
	  return __sb_tmp;
	}  	
  
//--------------------------------------------------------------------------
// Relationship operation methods 
//--------------------------------------------------------------------------
public:
  __OPENCODE_INLINE int compare(const StringBuffer&  __sb_val) const
  {
    return __internal_cached_buffer.compare(__sb_val.__internal_cached_buffer);  
  }
  
  __OPENCODE_INLINE int compare(const string& __sb_str) const
  {
  	return __internal_cached_buffer.compare(__sb_str);  
  }
  
  __OPENCODE_INLINE int compare(const char*  __sb_s) const
  {
  	return __internal_cached_buffer.compare(__sb_s);  
  }
                  
//--------------------------------------------------------------------------
//Swap methods
//--------------------------------------------------------------------------
public:
  __OPENCODE_INLINE void swap(StringBuffer& __sb_val)
  {
  	__internal_cached_buffer.swap(__sb_val.__internal_cached_buffer);
  }
  
  __OPENCODE_INLINE void swap(string& __sb_str)
  {
  	__internal_cached_buffer.swap(__sb_str);
  }

protected:
  string __internal_cached_buffer;  
};

__OPENCODE_INLINE const StringBuffer operator+(const StringBuffer& __sb_val,const StringBuffer& __sb_rv)
{
  StringBuffer __sb_tmp(__sb_val);
  __sb_tmp += __sb_rv;  
  return __sb_tmp;
}

__OPENCODE_INLINE const StringBuffer operator+(const StringBuffer& __sb_val,const string& __sb_str)
{
  StringBuffer __sb_tmp(__sb_val);
  __sb_tmp += __sb_str;  
  return __sb_tmp;
}

__OPENCODE_INLINE const StringBuffer operator+(const StringBuffer& __sb_val,const char*  __sb_s)
{
  StringBuffer __sb_tmp(__sb_val);
  __sb_tmp += __sb_s;  
  return __sb_tmp;
}

__OPENCODE_INLINE const StringBuffer operator+(const StringBuffer& __sb_val,const char&  __sb_char)
{
  StringBuffer __sb_tmp(__sb_val);
  __sb_tmp += __sb_char;  
  return __sb_tmp;
}

__OPENCODE_INLINE const StringBuffer operator+(const StringBuffer& __sb_val,const bool& __sb_bool)
{
  StringBuffer __sb_tmp(__sb_val);
  __sb_tmp += __sb_bool;  
  return __sb_tmp;
}

__OPENCODE_INLINE const StringBuffer operator+(const StringBuffer& __sb_val,const short& __sb_si)
{
  StringBuffer __sb_tmp(__sb_val);
  __sb_tmp += __sb_si;  
  return __sb_tmp;
}

__OPENCODE_INLINE const StringBuffer operator+(const StringBuffer& __sb_val,const unsigned short& __sb_usi)
{
  StringBuffer __sb_tmp(__sb_val);
  __sb_tmp += __sb_usi;  
  return __sb_tmp;
}

__OPENCODE_INLINE const StringBuffer operator+(const StringBuffer& __sb_val,const int& __sb_i)
{
  StringBuffer __sb_tmp(__sb_val);
  __sb_tmp += __sb_i;  
  return __sb_tmp;
}

__OPENCODE_INLINE const StringBuffer operator+(const StringBuffer& __sb_val,const unsigned int& __sb_ui)
{
  StringBuffer __sb_tmp(__sb_val);
  __sb_tmp += __sb_ui;  
  return __sb_tmp;
}

__OPENCODE_INLINE const StringBuffer operator+(const StringBuffer& __sb_val,const long& __sb_li)
{
  StringBuffer __sb_tmp(__sb_val);
  __sb_tmp += __sb_li;  
  return __sb_tmp;
}

__OPENCODE_INLINE const StringBuffer operator+(const StringBuffer& __sb_val,const unsigned long& __sb_uli)
{
  StringBuffer __sb_tmp(__sb_val);
  __sb_tmp += __sb_uli;  
  return __sb_tmp;
}

__OPENCODE_INLINE const StringBuffer operator+(const StringBuffer& __sb_val,const long long& __sb_lli)
{
  StringBuffer __sb_tmp(__sb_val);
  __sb_tmp += __sb_lli;  
  return __sb_tmp;
}

__OPENCODE_INLINE const StringBuffer operator+(const StringBuffer& __sb_val,const unsigned long long& __sb_ulli)
{
  StringBuffer __sb_tmp(__sb_val);
  __sb_tmp += __sb_ulli;  
  return __sb_tmp;
}
      
__OPENCODE_INLINE const StringBuffer operator+(const StringBuffer& __sb_val,const float& __sb_f)
{
  StringBuffer __sb_tmp(__sb_val);
  __sb_tmp += __sb_f;  
  return __sb_tmp;
}

__OPENCODE_INLINE const StringBuffer operator+(const StringBuffer& __sb_val,const double& __sb_d)
{
  StringBuffer __sb_tmp(__sb_val);
  __sb_tmp += __sb_d;  
  return __sb_tmp;
}

__OPENCODE_END_NAMESPACE

#endif

