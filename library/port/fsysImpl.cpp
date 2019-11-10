#include "port/fsysImpl.h"
#include "port/libnumber.h"
#include "port/logging.h"
#include "port/file.h"

#include "port/ioc_fdir.h" 
#include "port/ioc_time.h" 

__OPENCODE_BEGIN_NAMESPACE

IMPLEMENT_LIFETIME_BEGIN(GenericFileSystem)
IMPLEMENT_LIFETIME_END(GenericFileSystem)

GenericFileSystem::~GenericFileSystem() {Logging::closeFile();}
GenericFileSystem::GenericFileSystem(){}

bool  __M_renameTo(const string& pathname1,const string& pathname2)
{
  return (rename(pathname1.c_str(),pathname2.c_str() ) == 0);
}

bool  __M_exists(const string& pathname)
{
  return ((__M_getBooleanAttributes(pathname,pathname) & FILE_ATTR_EXISTS) != 0);
}

#if !defined(PORT_WINDOWS)

int   __M_getBooleanAttributes(const string& pathname,const string& name)
{
  int rv = 0;
  struct stat filestat;
  if(stat(pathname.c_str(),&filestat) == 0)
  {
    int mode = filestat.st_mode;
    int fmt = mode & S_IFMT;
    rv =  (  FILE_ATTR_EXISTS
           | ((fmt == S_IFREG) ? FILE_ATTR_REGULAR : 0)
           | ((fmt == S_IFLNK) ? FILE_ATTR_LINK : 0)
           | ((fmt == S_IFDIR) ? FILE_ATTR_DIRECTORY : 0));
  }
  bool hidden = (name.length() > 0) && (name[0] == '.');
  return rv | (hidden ? FILE_ATTR_HIDDEN : 0);
}

bool __M_deleteFile(const string& pathname)
{
  return (remove(pathname.c_str()) == 0);
}

XLONG __M_getLength(const string& pathname)
{
  struct stat filestat;
  if(stat(pathname.c_str(),&filestat) == 0)
  {
    return (XLONG)filestat.st_size;
  }

  return 0;
}

XLONG __M_getLastModified(const string& pathname)
{
  struct stat filestat;
  if(stat(pathname.c_str(),&filestat) == 0)
  {
    return  ((XLONG)filestat.st_mtime) * 1000;
  }

  return 0;
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

/**
 * Convert the given pathname string to normal form.  If the string is
 * already in normal form then it is simply returned.
 */
string __fsux_normalize(const string& pathname, int len, int off)
{
  if (len == 0) 
  {
    return pathname;
  }  

  int n = len;
  while ((n > 0) && (pathname[n - 1] == '/')) 
  {
    n--;
  }  
  if (n == 0) 
  {
    return "/";
  }
  
  string  sb ;
  if (off > 0) 
  {
    sb.append(pathname.substr(0, off));
  }

  char prevChar = 0;
  for (int i = off; i < n; i++) 
  {
    char c = pathname[i];
    if ((prevChar == '/') && (c == '/')) 
    {
      continue;
    }
    sb+=c;
    prevChar = c;
  }

  return sb;  
}

/** 
 * Check the given name sequence to see if it can be further collapsed.
 *  Return zero if not, otherwise return the number of names in the sequence. 
 */
int __fsux_collapsible(char *names)
{
  char *p = names;
  int dots = 0, n = 0;
  
  while (*p) 
  {
    if ((p[0] == '.') && ((p[1] == '\0')
        || (p[1] == '/')
        || ((p[1] == '.') && ((p[2] == '\0')
            || (p[2] == '/'))))) 
    {
      dots = 1;
    }
    n++;
    while (*p) 
    {
      if (*p == '/') 
      {
        p++;
        break;
      }
      p++;
    }
  }
  return (dots ? n : 0);
}

/** 
 * Split the names in the given name sequence,
 * replacing slashes with nulls and filling in the given index array 
 */
void __fsux_splitNames(char *names, char **ix)
{
  char *p = names;
  int i = 0;
  
  while (*p) 
  {
    ix[i++] = p++;
    while (*p) 
    {
      if (*p == '/') 
      {
        *p++ = '\0';
        break;
      }
      p++;
    }
  }
}

/** 
 * Join the names in the given name sequence, ignoring names whose index
 *  entries have been cleared and replacing nulls with slashes as needed 
 */
void __fsux_joinNames(char *names, int nc, char **ix)
{
  int i;
  char *p;
  
  for (i = 0, p = names; i < nc; i++) 
  {
    if (!ix[i]) continue;
    if (i > 0) 
    {
      p[-1] = '/';
    }
    if (p == ix[i]) 
    {
      p += strlen(p) + 1;
    } 
    else 
    {
      char *q = ix[i];
      while ((*p++ = *q++));
    }
  }
  *p = '\0';
}

/** 
 *  Collapse "." and ".." names in the given path wherever possible.
 *  A "." name may always be eliminated; a ".." name may be eliminated if it
 *  follows a name that is neither "." nor "..".  This is a syntactic operation
 *  that performs no filesystem queries, so it should only be used to cleanup
 *  after invoking the realpath() procedure. 
 */
void __fsux_collapse(char *path)
{
  char *names = (path[0] == '/') ? path + 1 : path; /* Preserve first '/' */
  int nc;
  char **ix;
  int i, j;
  char *q;
  
  nc = __fsux_collapsible(names);
  if (nc < 2) return;    /* Nothing to do */
  
  ix = (char **)malloc(nc * sizeof(char *));
  __fsux_splitNames(names, ix);
  
  for (i = 0; i < nc; i++) 
  {
    int dots = 0;
    
    /* Find next occurrence of "." or ".." */
    do 
    {
      char *p = ix[i];
      if (p[0] == '.') 
      {
        if (p[1] == '\0') 
        {
          dots = 1;
          break;
        }
        if ((p[1] == '.') && (p[2] == '\0')) 
        {
          dots = 2;
          break;
        }
      }
      i++;
    } while (i < nc);
    
    if (i >= nc) break;
    
    /* At this point i is the index of either a "." or a "..", so take the
    appropriate action and then continue the outer loop */
    if (dots == 1) 
    {
      /* Remove this instance of "." */
      ix[i] = 0;
    }
    else 
    {
      /* If there is a preceding name, remove both that name and this
       instance of ".."; otherwise, leave the ".." as is */
      for (j = i - 1; j >= 0; j--) 
      {
        if (ix[j]) break;
      }
      if (j < 0) continue;
      ix[j] = 0;
      ix[i] = 0;
    }
    /* i will be incremented at the top of the loop */
  }

  __fsux_joinNames(names, nc, ix);
  free(ix);
}

string GenericFileSystem::normalize(const string& pathname) 
{
  int n = (int)pathname.length();
  char prevChar = 0;
  for (int i = 0; i < n; ++i) 
  {
     char c = pathname[i];
     if ((prevChar == FILE_SEPARATORCHAR) && (c == FILE_SEPARATORCHAR))
     {
       return __fsux_normalize(pathname, n, i - 1);
     }     
     prevChar = c;
   }

   if (prevChar == FILE_SEPARATORCHAR) 
   {
     return __fsux_normalize(pathname, n, n - 1);
   }
      
   return pathname;
}
  
int GenericFileSystem::prefixLength(const string& pathname) 
{
  if (pathname.length() == 0) 
  {
    return 0;
  }

  return (pathname[0] == '/') ? 1 : 0;
}

string GenericFileSystem::resolve(const string& parent, const string& child) 
{
  if (child.empty()) 
  {
    return parent;
  }  

  if (child[0] == '/') 
  {
    if (parent.compare("/") == 0)
    {
      return child;
    }
    return parent + child;
  }
 
  if (parent.compare("/") == 0)
  {
     return parent + child;
  }  
  
  return parent + "/" + child;
}
  
string GenericFileSystem::getDefaultParent() 
{
  return "/";
}

string GenericFileSystem::getCurrentDirectory()
{
  char currentDir[OPENCODE_PATH_MAX+1] = {0};
  getcwd(currentDir,OPENCODE_PATH_MAX+1);

  return string(currentDir);
}

bool GenericFileSystem::isAbsolute(const File& f) 
{
  return (f.getPrefixLength() != 0);
}
  
string GenericFileSystem::resolve(const File& f) 
{
  if (isAbsolute(f))
  {
    return f.getPath();
  }

  return resolve(getCurrentDirectory(), f.getPath());
}

string GenericFileSystem::canonicalize(const string& pathname) __OPENCODE_THROW_DECLARE(IOException) 
{
  char resolved[OPENCODE_PATH_MAX + 1] = {0};
  if(realpath(pathname.c_str(),resolved)!=0)
  {
    __fsux_collapse(resolved);
 
    return string(resolved);
  } 
  else
  {
    /** 
     * Something's bogus in the original path, so remove names from the end
     * until either some subpath works or we run out of names 
     */
    char *p, *end, *r = 0;
    char path[OPENCODE_PATH_MAX + 1] = {0};
    strncpy(path, pathname.c_str(), sizeof(path));
    
    if (path[OPENCODE_PATH_MAX] != '\0') 
    {
      __OPENCODE_THROW(IOException("[IOException] bad pathname "));
    }
    
    end = path + strlen(path);
    
    for (p = end; p > path;) 
    {
    
      /* Skip last element */
      while ((--p > path) && (*p != '/'));
      
      if (p == path) break;
    
      /* Try realpath() on this subpath */
      *p = '\0';
      r = realpath(path, resolved);
      *p = (p == end) ? '\0' : '/';
    
      if (r != 0) 
      {
        /* The subpath has a canonical path */
        break;
      }
      else if (errno == ENOENT || errno == ENOTDIR || errno == EACCES) 
      {
        continue;
      }
      else 
      {
        __OPENCODE_THROW(IOException("[IOException] bad pathname "));
      }
    }
    
    if (r != 0) 
    {   
      /* Append unresolved subpath to resolved subpath */
      int rn = (int)strlen(r);
      if (rn + strlen(p) >=OPENCODE_PATH_MAX ) 
      {
        /* Buffer overflow */
        __OPENCODE_THROW(IOException("[IOException] bad pathname "));
      }
      if ((rn > 0) && (r[rn - 1] == '/') && (*p == '/')) 
      {
        /* Avoid duplicate slashes */
        p++;
      }
      strcpy(r + rn, p);
      __fsux_collapse(r);

      return string(resolved);
    }
    else 
    {
      /* Nothing resolved, so just return the original path */
      strcpy(resolved, path);
      __fsux_collapse(resolved);

      return string(resolved);
    }
  }
}

bool  GenericFileSystem::checkAccess(const string& pathname, bool write)
{
  return (access(pathname.c_str(), (write ? W_OK : R_OK)) == 0);
}

XLONG GenericFileSystem::getLength(int fd)
{
  struct stat filestat;
  if(fstat(fd,&filestat) == 0)
  {
    return (XLONG)filestat.st_size;
  }

  return 0;
}

bool GenericFileSystem::createFileExclusively(const string& pathname,int filemask) __OPENCODE_THROW_DECLARE(IOException) 
{
  return (open(pathname.c_str(),O_RDWR|O_EXCL|O_CREAT,filemask) == 0);
}
  
bool GenericFileSystem::list(const string& pathname,vector<string>& lists)
{
  DIR* ptr_dir = ::opendir(pathname.c_str()); 
  
  if(ptr_dir != 0)
  {
    struct dirent *ptr_dirent=0;

    /* Scan the directory */
    while ((ptr_dirent = ::readdir(ptr_dir)) != 0) 
    {
      if(strcmp(ptr_dirent->d_name, ".")==0  || strcmp(ptr_dirent->d_name, "..")==0)
      {
        continue;
      }
      
      lists.push_back(ptr_dirent->d_name);
      
    }//~end while ((ptr_dirent = ::readdir(ptr_dir)) != 0) 
    
    ::closedir(ptr_dir);
  
    return true;
  
  }//~end if(ptr_dir != 0)

  return false; 
}

bool GenericFileSystem::createDirectory(const string& pathname,int filemask) 
{
  return (mkdir(pathname.c_str(), filemask) == 0);
}
  
bool GenericFileSystem::setLastModifiedTime(const string& pathname, XLONG time) 
{
  struct timeval tv[2];
  struct stat sb;
  if (stat(pathname.c_str(), &sb) == 0)
  {
    tv[0].tv_sec = sb.st_atime;
    tv[0].tv_usec = 0; 
  } 

  /* Change last-modified time */
  tv[1].tv_sec = time / 1000;
  tv[1].tv_usec = (time % 1000) * 1000;
  return (utimes(pathname.c_str(), tv) >= 0);
}

bool GenericFileSystem::setReadOnly(const string& pathname) 
{
  struct stat sb;
  if (stat(pathname.c_str(), &sb) == 0)
  {
    int mode = sb.st_mode;
    if (chmod(pathname.c_str(), mode & ~(S_IWUSR | S_IWGRP | S_IWOTH)) >= 0)
    {
      return true;
    }   
  }

  return false;
}

bool GenericFileSystem::listRoots(vector<File>& lists) 
{
  lists.push_back(File("/"));  
  return true;
}

File GenericFileSystem::createTempFile(const string& prefix,const string& suffix,const File* directory) __OPENCODE_THROW_DECLARE(IOException)
{
  File fs;
  if(directory == 0)
  {
    fs = File("/tmp");
  }  
  else
  {
    fs = *directory; 
  }

  size_t counter = 0;
  File f;

  do
  {
    counter = random() & 0xffff;
    f = File(fs, prefix + Number::toString(counter) + suffix);
  }
  while(!createFileExclusively(f.getPath()));

  return f;
}

#endif

__OPENCODE_END_NAMESPACE

