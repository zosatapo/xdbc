#include "port/fsysImpl.h"
#include "port/libsys.h"
#include "port/utilities.h"
#include "port/file.h"

#include "port/libnumber.h"
#include "port/logging.h"
#include "port/ioc_fdir.h" 
#include "port/ioc_time.h" 

#if defined(PORT_WINDOWS)
#include <io.h>
#endif

__OPENCODE_BEGIN_NAMESPACE

#if defined(PORT_WINDOWS)

struct dirent 
{
  char d_name[OPENCODE_PATH_MAX];
};

typedef struct 
{
	struct dirent dirent;
	char *path;
	HANDLE handle;
	WIN32_FIND_DATA find_data;
} DIR;

DIR *opendir(const char *dirname);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);
void rewinddir(DIR *dirp);

DIR* opendir(const char *dirname)
{
  DIR *dirp = (DIR *)malloc(sizeof(DIR));
  DWORD fattr;
  char alt_dirname[4] = { 0, 0, 0, 0 };

  if (dirp == 0) 
  {
		errno = ENOMEM;
		return 0;
  }

  /*
   * Win32 accepts "\" in its POSIX stat(), but refuses to treat it
   * as a directory in FindFirstFile().  We detect this case here and
   * prepend the current drive name.
   */
  if (dirname[1] == '\0' && dirname[0] == '\\') 
  {
			alt_dirname[0] = _getdrive() + 'A' - 1;
			alt_dirname[1] = ':';
			alt_dirname[2] = '\\';
			alt_dirname[3] = '\0';
			dirname = alt_dirname;
  }

  dirp->path = (char *)malloc(strlen(dirname) + 5);
  if (dirp->path == 0) 
  {
		free(dirp);
		errno = ENOMEM;
		return 0;
  }
  strcpy(dirp->path, dirname);

  fattr = GetFileAttributes(dirp->path);
  if (fattr == ((DWORD)-1)) 
  {
		free(dirp->path);
		free(dirp);
		errno = ENOENT;
		return 0;
  } 
  else if (fattr & FILE_ATTRIBUTE_DIRECTORY == 0) 
  {
		free(dirp->path);
		free(dirp);
		errno = ENOTDIR;
		return 0;
  }

  /* Append "*.*", or possibly "\\*.*", to path */
  if (   dirp->path[1] == ':'
      && (    dirp->path[2] == '\0' 
           || (dirp->path[2] == '\\' && dirp->path[3] == '\0')
          )) 
  {
		/* No '\\' needed for cases like "Z:" or "Z:\" */
		strcat(dirp->path, "*.*");
  } 
  else 
  {
		strcat(dirp->path, "\\*.*");
  }

  dirp->handle = FindFirstFile(dirp->path, &dirp->find_data);
  if (dirp->handle == INVALID_HANDLE_VALUE) 
  {
    if (GetLastError() != ERROR_FILE_NOT_FOUND) 
    {
	    free(dirp->path);
	    free(dirp);
	    errno = EACCES;
	    return 0;
		}
  }
  return dirp;
}

struct dirent* readdir(DIR *dirp)
{
	if (dirp->handle == INVALID_HANDLE_VALUE) 
	{
		return 0;
	}
	
	strcpy(dirp->dirent.d_name, dirp->find_data.cFileName);
	
	if (!FindNextFile(dirp->handle, &dirp->find_data)) 
	{
		if (GetLastError() == ERROR_INVALID_HANDLE) 
		{
		    errno = EBADF;
		    return 0;
		}
		FindClose(dirp->handle);
		dirp->handle = INVALID_HANDLE_VALUE;
	}

  return &dirp->dirent;
}

int closedir(DIR *dirp)
{
  if (dirp->handle != INVALID_HANDLE_VALUE) 
  {
		if (!FindClose(dirp->handle))
	  {
	    errno = EBADF;
	    return -1;
		}
		dirp->handle = INVALID_HANDLE_VALUE;
  }
  free(dirp->path);
  free(dirp);
  return 0;
}

void rewinddir(DIR *dirp)
{
  if (dirp->handle != INVALID_HANDLE_VALUE) 
  {
		FindClose(dirp->handle);
  }
  dirp->handle = FindFirstFile(dirp->path, &dirp->find_data);
}

#define __fswin_islb(c)      (IsDBCSLeadByte((BYTE)(c)))

char* __fswin_cp(char *dst, char *dend, char first, char *src, char *send)
{
  char *p = src, *q = dst;
  if (first != '\0') 
  {
    if (q < dend) 
    {
      *q++ = first;
    } 
    else 
    {
      errno = ENAMETOOLONG;
      return 0;
    }
  }
  if (send - p > dend - q) 
  {
    errno = ENAMETOOLONG;
    return 0;
  }
  while (p < send) 
  {
    *q++ = *p++;
  }
  return q;
}

string __fswin_slashify(const string& p) 
{
	if ((p.length() > 0) && (p[0] != FILE_SEPARATORCHAR)) 
	{
		return string(FILE_SEPARATOR) + p;
	}
	else 
	{
		return p;
  }
}

bool __fswin_isSlash(char c) 
{
  return (c == '\\') || (c == '/');
}

bool __fswin_isLetter(char c) 
{
  return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

    
/* A normal Win32 pathname contains no duplicate slashes, except possibly
   for a UNC prefix, and does not end with a slash.  It may be the empty
   string.  Normalized Win32 pathnames have the convenient property that
   the length of the prefix almost uniquely identifies the type of the path
   and whether it is absolute or relative:

       0  relative to both drive and directory
 1  drive-relative (begins with '\\')
 2  absolute UNC (if first char is '\\'),
      else directory-relative (has form "z:foo")
 3  absolute local pathname (begins with "z:\\")
 */

int __fswin_normalizePrefix(const string& pathname, int len, StringBuffer& sb) 
{
	int src = 0;
	while ((src < len) && __fswin_isSlash(pathname.at(src))) 
	{
	  src++;
	}
	
	char c;
  if ((len - src >= 2)
	  && __fswin_isLetter(c = pathname.at(src))
	  && pathname.at(src + 1) == ':') 
	{
	  /* Remove leading slashes if followed by drive specifier.
	     This hack is necessary to support file URLs containing drive
	     specifiers (e.g., "file://c:/path").  As a side effect,
	     "/c:/path" can be used as an alternative to "c:/path". */
	  sb.append(c);
	  sb.append(':');
	  src += 2;
	} 
	else 
	{
	  src = 0;
	  if (  (len >= 2)
	      && __fswin_isSlash(pathname.at(0))
	      && __fswin_isSlash(pathname.at(1))) 
	  {
	    /* UNC pathname: Retain first slash; leave src pointed at
	     * second slash so that further slashes will be collapsed
	     * into the second slash.  The result will be a pathname
	     * beginning with "\\\\" followed (most likely) by a hostname. 
	     */
	    src = 1;
	    sb.append(FILE_SEPARATOR);
    }
  }
  
  return src;
}

/*  Normalize the given pathname, whose length is len, starting at the given
 *  offset; everything before this offset is already normal. 
 */
string __fswin_normalize(const string& pathname, int len, int off) 
{
	if (len == 0) 
	{
	  return pathname;
	}
	
	if (off < 3) 
	{
	  off = 0;	/* Avoid fencepost cases with UNC pathnames */
	}
	
	int src;
	char slash = FILE_SEPARATORCHAR;
	StringBuffer sb;
	
	if (off == 0) 
	{
	  /* Complete normalization, including prefix */
	  src = __fswin_normalizePrefix(pathname, len, sb);
	} 
	else 
	{
	  /* Partial normalization */
	  src = off;
	  sb.append(pathname.substr(0, off));
	}
	
	/* Remove redundant slashes from the remainder of the path, forcing all
	 slashes into the preferred slash */
	while (src < len) 
	{
	  char c = pathname.at(src++);
	  if (__fswin_isSlash(c)) 
	  {
	    while ((src < len) && __fswin_isSlash(pathname.at(src))) 
	    {
	      src++;
	    }
	    
	    if (src == len) 
	    {
		    /* Check for trailing separator */
        int sn = sb.size();
		    if ((sn == 2) && (sb.at(1) == ':')) 
		    {
					/* "z:\\" */
					sb.append(slash);
					break;
		    }
		    
		    if (sn == 0) 
		    {
					/* "\\" */
					sb.append(slash);
					break;
		    }
	    
		    if ((sn == 1) && (__fswin_isSlash(sb.at(0)))) 
		    {
			    /* "\\\\" is not collapsed to "\\" because "\\\\" marks
		       * the beginning of a UNC pathname.  Even though it is
		       * not, by itself, a valid UNC pathname, we leave it as
		       * is in order to be consistent with the win32 APIs,
		       * which treat this case as an invalid UNC pathname
		       * rather than as an alias for the root directory of
		       * the current drive. 
		       */
			    sb.append(slash);
			    break;
		    }
	    
	    	/* Path does not denote a root directory, so do not append trailing slash */
	   		 break;
	  	} 
	  	else 
	  	{
	    	sb.append(slash);
	  	}
		} 
		else 
		{
	   	sb.append(c);
		}
  }
  
	return sb.toString();
}

int __fswin_prefixLength(const string& pathname) 
{
	char slash = FILE_SEPARATORCHAR;
	int n = pathname.length();
	if (n == 0) 
  {
    return 0;
  }
   
  char c0 = pathname[0];
	char c1 = (n > 1) ? pathname[1] : 0;

	if (c0 == slash) 
  {
	  if (c1 == slash) 
    {
      return 2;	/* Absolute UNC pathname "\\\\foo" */
    }
    return 1;			/* Drive-relative "\\foo" */
	}

  if (__fswin_isLetter(c0) && (c1 == ':')) 
  {
	  if ((n > 2) && (pathname[2] == slash))
    {
      return 3;		/* Absolute local pathname "z:\\foo" */
    }	  
    return 2;			/* Directory-relative "z:foo" */
	}

	return 0;			/* Completely relative */
}

string __fswin_getDriveDirectory(int drive)
{
	char buf[OPENCODE_PATH_MAX + 1] = {0};
	char *p = _getdcwd(drive, buf, OPENCODE_PATH_MAX);
	if (p == 0) 
	{
		return string();
	}
	
	if (isalpha(*p) && (p[1] == ':')) 
	{
	  p += 2;
	}
	
	return string(p);
}
int __fswin_driveIndex(char d) 
{
	if ((d >= 'a') && (d <= 'z')) 
	{
		return d - 'a';
	}
	
	if ((d >= 'A') && (d <= 'Z')) 
	{
		return d - 'A';
	}
	return -1;
}

string __fswin_getDriveDirectory(char drive) 
{
	int i = __fswin_driveIndex(drive);
	if (i < 0) 
	{
		return string();
	}
	
	return  __fswin_getDriveDirectory(i + 1);
}

string __fswin_getCurrentDirectory()
{
  char currentDir[OPENCODE_PATH_MAX + 1] = {0};
  ::GetCurrentDirectory(OPENCODE_PATH_MAX,currentDir);
  string __xxx;
  __xxx.assign(currentDir);
  return string(__xxx);
}

string __fswin_getUserPath() 
{
  string __env_user_path = __fswin_getCurrentDirectory();
  return __fswin_normalize(__env_user_path,__env_user_path.size(),0);
}

string __fswin_getDrive(const string& path) 
{
  int pl = __fswin_prefixLength(path);
	return (pl == 3) ? path.substr(0, 2) : string();
}

/* Tell whether the given string contains any wildcard characters */
int __fswin_wild(const char *start)
{
	const char *p = start;
	int c;
	while (c = *p) 
	{
    if ((c == '*') || (c == '?')) 
    {
      return 1;
    }
    
    p += ((__fswin_islb(c) && p[1]) ? 2 : 1);
	}
	return 0;
}

/* Tell whether the given string contains prohibited combinations of dots.
   In the canonicalized form no path element may have dots at its end.
   Allowed canonical paths: c:\xa...dksd\..ksa\.lk    c:\...a\.b\cd..x.x
   Prohibited canonical paths: c:\..\x  c:\x.\d c:\...
*/
int __fswin_dots(char *start)
{
	char *p = start;
	while (*p) 
  {
		if ((p = strchr(p, '.')) == 0) // find next occurence of '.'
		{
			return 0; // no more dots
		}
		
		p++; // next char
		
		while ((*p) == '.') // go to the end of dots
		{
			p++;
		}
		
		if (*p && (*p != '\\')) // path element does not end with a dot
		{
			p++; // go to the next char
		}
		else 
		{
			return 1; // path element does end with a dot - prohibited
		}
  }
	return 0; // no prohibited combinations of dots found
}

/* Returns 0 on success */
int __fswin_removeFileOrDirectory(const char* path)
{
	SetFileAttributes(path, 0);
	DWORD a = GetFileAttributes(path);
	if (a == ((DWORD)-1)) 
	{
	  return 1;
	} 
	else if (a & FILE_ATTRIBUTE_DIRECTORY) 
	{
	  return !__fswin_removeFileOrDirectory(path);
	} 
	else 
	{
	  return !DeleteFile(path);
	}
}

/* Find first instance of '\\' at or following start.  Return the address of
   that byte or the address of the null terminator if '\\' is not found. */

char* __fswin_nextsep(char *start)
{
    char *p = start;
    int c;
    while ((c = *p) && (c != '\\')) {
        p += ((__fswin_islb(c) && p[1]) ? 2 : 1);
    }
    return p;
}

/* Convert a pathname to canonical form.  The input orig_path is assumed to
   have been converted to native form already, via JVM_NativePath().  This is
   necessary because _fullpath() rejects duplicate separator characters on
   Win95, though it accepts them on NT. */

int __fswin_canonicalize(const char *orig_path, char *result, int size)
{
    WIN32_FIND_DATA fd;
    HANDLE h;
    char path[1024];    /* Working copy of path */
    char *src, *dst, *dend;

    /* Reject paths that contain wildcards */
    if (__fswin_wild(orig_path)) {
        errno = EINVAL;
        return -1;
    }

    /* Collapse instances of "foo\.." and ensure absoluteness.  Note that
       contrary to the documentation, the _fullpath procedure does not require
       the drive to be available.  It also does not reliably change all
       occurrences of '/' to '\\' on Win95, so now JVM_NativePath does that. */
    if(!_fullpath(path, orig_path, sizeof(path))) {
        return -1;
    }

    /* Correction for Win95: _fullpath may leave a trailing "\\"
       on a UNC pathname */
    if ((path[0] == '\\') && (path[1] == '\\')) {
        char *p = path + strlen(path);
        if ((p[-1] == '\\') && !__fswin_islb(p[-2])) {
            p[-1] = '\0';
        }
    }

    if (__fswin_dots(path)) /* Check for phohibited combinations of dots */
        return -1;

    src = path;            /* Start scanning here */
    dst = result;        /* Place results here */
    dend = dst + size;        /* Don't go to or past here */

    /* Copy prefix, assuming path is absolute */
    if (isalpha(src[0]) && (src[1] == ':') && (src[2] == '\\')) {
        /* Drive specifier */
        *src = toupper(*src);    /* Canonicalize drive letter */
        if (!(dst = __fswin_cp(dst, dend, '\0', src, src + 2))) {
            return -1;
        }
        src += 2;
    } else if ((src[0] == '\\') && (src[1] == '\\')) {
        /* UNC pathname */
        char *p;
        p = __fswin_nextsep(src + 2);    /* Skip past host name */
        if (!*p) {
        /* A UNC pathname must begin with "\\\\host\\share",
           so reject this path as invalid if there is no share name */
            errno = EINVAL;
            return -1;
    }
    p = __fswin_nextsep(p + 1);    /* Skip past share name */
    if (!(dst = __fswin_cp(dst, dend, '\0', src, p))) {
        return -1;
    }
    src = p;
    } else {
        /* Invalid path */
        errno = EINVAL;
        return -1;
    }

    /* Windows 95/98/Me bug - FindFirstFile fails on network mounted drives */
    /* for root pathes like "E:\" . If the path has this form, we should  */
    /* simply return it, it is already canonicalized. */
    if (strlen(path) == 3 && path[1] == ':' && path[2] == '\\') {
        /* At this point we have already copied the drive specifier ("z:")*/
        /* so we need to copy "\" and the null character. */
        result[2] = '\\';
        result[3] = '\0';
        return 0;
    }

    /* At this point we have copied either a drive specifier ("z:") or a UNC
       prefix ("\\\\host\\share") to the result buffer, and src points to the
       first byte of the remainder of the path.  We now scan through the rest
       of the path, looking up each prefix in order to find the true name of
       the last element of each prefix, thereby computing the full true name of
       the original path. */
    while (*src) {
        char *p = __fswin_nextsep(src + 1);    /* Find next separator */
        char c = *p;
        __OPENCODE_ASSERT(*src == '\\');        /* Invariant */
        *p = '\0';            /* Temporarily clear separator */
    h = FindFirstFile(path, &fd);    /* Look up prefix */
    *p = c;                /* Restore separator */
    if (h != INVALID_HANDLE_VALUE) {
        /* Lookup succeeded; append true name to result and continue */
        FindClose(h);
        if (!(dst = __fswin_cp(dst, dend, '\\',
               fd.cFileName,
               fd.cFileName + strlen(fd.cFileName)))) {
        return -1;
        }
        src = p;
        continue;
    } else {
        /* If the lookup of a particular prefix fails because the file does
           not exist, because it is of the wrong type, or because access is
           denied, then we copy the remainder of the input path as-is.
           Other I/O errors cause an error return. */
        DWORD errval = GetLastError();
        if ((errval == ERROR_FILE_NOT_FOUND)
        || (errval == ERROR_DIRECTORY)
        || (errval == ERROR_PATH_NOT_FOUND)
        || (errval == ERROR_BAD_NETPATH)
        || (errval == ERROR_BAD_NET_NAME)
        || (errval == ERROR_ACCESS_DENIED)
        || (errval == ERROR_NETWORK_ACCESS_DENIED)) {
        if (!(dst = __fswin_cp(dst, dend, '\0', src, src + strlen(src)))) {
            return -1;
        }
        break;
        } else {
        return -1;
        }
    }
    }

    if (dst >= dend) {
    errno = ENAMETOOLONG;
    return -1;
    }
    *dst = '\0';
    return 0;

}

string GenericFileSystem::normalize(const string& pathname) 
{
	int n = pathname.length();
	char slash = FILE_SEPARATORCHAR;
  char altSlash = '/';
	char prev = 0;
	
	for (int i = 0; i < n; i++) 
	{
	  char c = pathname.at(i);
	  if (c == altSlash)
	  {
			return __fswin_normalize(pathname, n, (prev == slash) ? i - 1 : i);
	  }
	  
	  if ((c == slash) && (prev == slash) && (i > 1))
		{
			return __fswin_normalize(pathname, n, i - 1);
	  }
	  
	  if ((c == ':') && (i > 1))
		{
			return __fswin_normalize(pathname, n, 0);
	  }
	  prev = c;
	}
	
	if (prev == slash) 
	{
		return __fswin_normalize(pathname, n, n - 1);
	}
	
	return pathname;
}
	        
int GenericFileSystem::prefixLength(const string& pathname) 
{
	return __fswin_prefixLength(pathname);
}

string GenericFileSystem::resolve(const string& parent, const string& child) 
{
	char slash = FILE_SEPARATORCHAR;
	int pn = parent.length();
  if (pn == 0) 
  {
    return child;
  }
  
  int cn = child.length();
	if (cn == 0) 
	{
		return parent;
	}
	
	string c = child;
	if ((cn > 1) && (c[0] == slash)) 
	{
		if (c.at(1) == slash) 
		{
			/* Drop prefix when child is a UNC pathname */
			c = c.substr(2);
		} 
		else 
		{
			/* Drop prefix when child is drive-relative */
			c = c.substr(1);
		}
	}
	
	string p = parent;
	if (p[pn - 1] == slash) 
	{
		p = p.substr(0, pn - 1);
	}
	return p + __fswin_slashify(c);
}
  
string GenericFileSystem::getDefaultParent() 
{
  return "\\";
}

string GenericFileSystem::getCurrentDirectory()
{
 return __fswin_getCurrentDirectory();
}

bool GenericFileSystem::isAbsolute(const File& f) 
{
	int pl = f.getPrefixLength();
  return (((pl == 2) && (f.getPath().at(0) == FILE_SEPARATORCHAR)) || (pl == 3));
}
  
string GenericFileSystem::resolve(const File& f) 
{
  if (isAbsolute(f))
  {
    return f.getPath();
  }

	char slash  = FILE_SEPARATORCHAR;
	string path = f.getPath();
	int pl = f.getPrefixLength();	

	switch(pl)
	{
//---------------------------------
		case 0:
		{
			return __fswin_getUserPath() + __fswin_slashify(path); /* Completely relative */
		}	
		break;
//---------------------------------
    case 1:
    {
	    /* Drive-relative */
		  string up = __fswin_getUserPath();
		  string ud = __fswin_getDrive(up);
		  if (!ud.empty()) 
		  {
		  	return ud + path;
		  }
		  
		  return up + path;			/* User dir is a UNC path */
    }
    break;		
//---------------------------------				
    case 2:
    {
			if(path.at(0) == slash)
			{
				return path;
		  }
		  	
			/* Directory-relative */
		  string up = __fswin_getUserPath();
		  string ud = __fswin_getDrive(up);  
		  if ((!ud.empty()) && StringsKit::startsWith(path,ud))
		  {
		  	return up + __fswin_slashify(path.substr(2));
		  }
		  
		  char drive = path.at(0);
		  string dir = __fswin_getDriveDirectory(drive);
		  string np;
		  if (!dir.empty()) 
		  {
		    /**
		     * When resolving a directory-relative path that refers to a
		     * drive other than the current drive, insist that the caller
		     * have read permission on the result 
		     */
		    string p = drive + (':' + dir + __fswin_slashify(path.substr(2)));
		    return p;
		  }
		  return drive + ":" + __fswin_slashify(path.substr(2)); /* fake it */
    }
    break;		
//---------------------------------
    default:
    {
    	return resolve(getCurrentDirectory(), f.getPath());
    }
    break;					
  }
  
  return resolve(getCurrentDirectory(), f.getPath());
}

string GenericFileSystem::canonicalize(const string& pathname) __OPENCODE_THROW_DECLARE(IOException) 
{
  char canonicalPath[OPENCODE_PATH_MAX + 1]= {0};
  if(__fswin_canonicalize(pathname.c_str(),canonicalPath, OPENCODE_PATH_MAX) < 0) 
  {
  	__OPENCODE_THROW(IOException("Bad pathname"));
	}    
	
	return string(canonicalPath);
}

bool  GenericFileSystem::checkAccess(const string& pathname, bool write)
{
  return (_access(pathname.c_str(), (write ? 2 : 4)) == 0);
}

XLONG GenericFileSystem::getLength(int fd)
{  
  XLONG rv = 0;
	struct _stati64 sb;
	if (_fstati64(fd, &sb) == 0) 
  {
	   rv = sb.st_size;
	}
  return rv;
}

bool GenericFileSystem::createFileExclusively(const string& pathname,int filemask) __OPENCODE_THROW_DECLARE(IOException) 
{
  return (_open(pathname.c_str(),O_RDWR|O_EXCL|O_CREAT,filemask) == 0);
}
  
bool GenericFileSystem::list(const string& pathname,vector<string>& lists)
{
  DIR* ptr_dir = opendir(pathname.c_str()); 
  
  if(ptr_dir != 0)
  {
    struct dirent *ptr_dirent=0;

    /* Scan the directory */
    while ((ptr_dirent = readdir(ptr_dir)) != 0) 
    {
      if(strcmp(ptr_dirent->d_name, ".")==0  || strcmp(ptr_dirent->d_name, "..")==0)
      {
        continue;
      }
      
      lists.push_back(ptr_dirent->d_name);
      
    }//~end while ((ptr_dirent = ::readdir(ptr_dir)) != 0) 
    
    closedir(ptr_dir);
  
    return true;
  
  }//~end if(ptr_dir != 0)

  return false; 
}

bool GenericFileSystem::createDirectory(const string& pathname,int filemask) 
{
#if defined(PORT_WINDOWS)
  return  (_mkdir(pathname.c_str()) == 0);
#else
	return  (mkdir(pathname.c_str()) == 0);
#endif
}
  
bool GenericFileSystem::setLastModifiedTime(const string& pathname, XLONG time) 
{ 
  bool rv = false;

	HANDLE h = CreateFile(pathname.c_str(), GENERIC_WRITE, 0, 0, OPEN_EXISTING,
		                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, 0);
		                    
	if (h != INVALID_HANDLE_VALUE) 
	{
		LARGE_INTEGER modTime;
		FILETIME t;
		modTime.QuadPart = (time + 11644473600000L) * 10000L;
		t.dwLowDateTime = (DWORD)modTime.LowPart;
		t.dwHighDateTime = (DWORD)modTime.HighPart;
		if (SetFileTime(h, 0, 0, &t)) 
		{
		  rv = true;
		}
		CloseHandle(h);
	}

  return rv;	
}

bool GenericFileSystem::setReadOnly(const string& pathname) 
{
  bool rv = false;
  DWORD a = GetFileAttributes(pathname.c_str());
	if (a != ((DWORD)-1)) 
	{
    if (SetFileAttributes(pathname.c_str(), a | FILE_ATTRIBUTE_READONLY))
		rv = true;
	}
  
  return rv;
}

bool GenericFileSystem::listRoots(vector<File>& lists) 
{
  int ds = GetLogicalDrives();

	for (int i = 0; i < 26; i++) 
	{
	  if (((ds >> i) & 1) != 0) 
	  {
		  StringBuffer __sb;
      __sb.append((char)('A' + i));
      __sb.append(":");
      __sb.append(FILE_SEPARATOR);
      
      lists.push_back(File(__sb.toString()));
	  }
	}

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
    //counter = random() & 0xffff;
    f = File(fs, prefix + Number::toString(counter) + suffix);
  }
  while(!createFileExclusively(f.getPath()));

  return f;
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

int   __M_getBooleanAttributes(const string& pathname,const string& name)
{
  int rv = 0;

  DWORD mode = GetFileAttributes(pathname.c_str());
	if (mode != ((DWORD)-1)) 
	{
	    rv = (  FILE_ATTR_EXISTS
		        | ((mode & FILE_ATTRIBUTE_DIRECTORY) ? FILE_ATTR_DIRECTORY : FILE_ATTR_REGULAR)
		        | ((mode & FILE_ATTRIBUTE_HIDDEN) ? FILE_ATTR_HIDDEN : 0));
	 }
   return rv;
}

bool __M_deleteFile(const string& pathname)
{
  return (__fswin_removeFileOrDirectory(pathname.c_str()) == 0);
}

XLONG __M_getLength(const string& pathname)
{
  XLONG rv = 0;
	struct _stati64 sb;
	if (_stati64(pathname.c_str(), &sb) == 0) 
	{
	  rv = sb.st_size;
	}
  return rv;
}

XLONG __M_getLastModified(const string& pathname)
{
  XLONG rv = 0;
	/* Win95, Win98, WinME */
	WIN32_FIND_DATA fd;
	XLONG temp = 0;
	LARGE_INTEGER modTime;
	HANDLE h = FindFirstFile(pathname.c_str(), &fd);
	if (h != INVALID_HANDLE_VALUE) 
	{
	  FindClose(h);
	  modTime.LowPart = (DWORD) fd.ftLastWriteTime.dwLowDateTime;
	  modTime.HighPart = (LONG) fd.ftLastWriteTime.dwHighDateTime;
	  rv = modTime.QuadPart / 10000;
	  rv -= 11644473600000;
	}
	return rv;
}

#endif

__OPENCODE_END_NAMESPACE

