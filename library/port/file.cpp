#include "port/file.h"
#include "port/libsys.h"
#include "port/fsysImpl.h"

#include "port/ioc_fdir.h" 

__OPENCODE_BEGIN_NAMESPACE


IOException::~IOException()
{
}

IOException::IOException()
:BaseException()
{
}

IOException::IOException(const string& message)
:BaseException(message)
{
}

const char   File::separatorChar     = FILE_SEPARATORCHAR;
const char   File::separator[]       = {FILE_SEPARATOR};
const char   File::pathSeparatorChar = FILE_PATHSEPARATORCHAR;
const char   File::pathSeparator[]   = {FILE_PATHSEPARATOR};
const char   File::lineSeparator[]   = {FILE_LINESEPARATOR};

File::File(const string& pathname, int prefixLengthValue) 
{
  this->__internal_fs = &(FileSystemImpl::getInstance());
  this->path = pathname;
  this->prefixLength = prefixLengthValue;
  this->__internal_absolutepath = __internal_fs->resolve(*this);

  int index = (int)(this->path.find_last_of(separator));
  if (index < prefixLength) 
  {
    this->__internal_name = this->path.substr(prefixLength);
  }
  else
  {
    this->__internal_name = this->path.substr(index + 1);  
  }
}

File::File()
{
  this->__internal_fs = &(FileSystemImpl::getInstance());
  this->path = File::getCurrentDirectory(); 
  this->prefixLength = __internal_fs->prefixLength(this->path);
  this->__internal_absolutepath = __internal_fs->resolve(*this);   

  int index = (int)(this->path.find_last_of(separator));
  if (index < prefixLength) 
  {
    this->__internal_name = this->path.substr(prefixLength);
  }
  else
  {
    this->__internal_name = this->path.substr(index + 1);  
  }
}

File::File(const File& rv)
{
  this->__internal_fs = rv.__internal_fs;
  this->path = rv.path;
  this->prefixLength = rv.prefixLength;
  this->__internal_absolutepath = rv.__internal_absolutepath;
  this->__internal_name = rv.__internal_name;
}

File& File::operator=(const File& rv)
{
  this->__internal_fs = rv.__internal_fs;
  this->path = rv.path;
  this->prefixLength = rv.prefixLength;
  this->__internal_absolutepath = rv.__internal_absolutepath;
  this->__internal_name = rv.__internal_name;
    
  return *this;
}


File::File(const string& pathname) 
{
  if (pathname.empty()) 
  {
    System::loggingAndExit("File::<init>|pathname is empty",-1);
  }
  
  this->__internal_fs = &(FileSystemImpl::getInstance());
  this->path = __internal_fs->normalize(pathname);
  this->prefixLength = __internal_fs->prefixLength(this->path);
  this->__internal_absolutepath = __internal_fs->resolve(*this); 

  int index = (int)(this->path.find_last_of(separator));
  if (index < prefixLength) 
  {
    this->__internal_name = this->path.substr(prefixLength);
  }
  else
  {
    this->__internal_name = this->path.substr(index + 1);  
  }
}

File::File(const string& parent, const string& child)
{
  if (child.empty())
  {
    System::loggingAndExit("File::<init>|child is empty",-1);
  }
  
  this->__internal_fs = &(FileSystemImpl::getInstance());
    
  if (parent.empty())
  {
    this->path = __internal_fs->resolve(__internal_fs->getDefaultParent(),__internal_fs->normalize(child));
  }
  else
  {
    this->path = __internal_fs->resolve(__internal_fs->normalize(parent),__internal_fs->normalize(child));
  }

  this->prefixLength = __internal_fs->prefixLength(this->path);
  this->__internal_absolutepath = __internal_fs->resolve(*this); 

  int index = (int)(this->path.find_last_of(separator));
  if (index < prefixLength) 
  {
    this->__internal_name = this->path.substr(prefixLength);
  }
  else
  {
    this->__internal_name = this->path.substr(index + 1);  
  }
}

File::File(const char* parent, const string& child) 
{
  if (child.empty()) 
  {
    System::loggingAndExit("File::<init>|child is empty",-1);
  }
  
  this->__internal_fs = &(FileSystemImpl::getInstance());
    
  if (parent != 0) 
  {
    string strParent(parent);
    if (strParent.empty()) 
    {
        this->path = __internal_fs->resolve(__internal_fs->getDefaultParent(),__internal_fs->normalize(child));
    } 
    else 
    {
      this->path = __internal_fs->resolve(__internal_fs->normalize(strParent),__internal_fs->normalize(child));
    }
  } 
  else 
  {
    this->path = __internal_fs->normalize(child);
  }
  
  this->prefixLength = __internal_fs->prefixLength(this->path);
  this->__internal_absolutepath = __internal_fs->resolve(*this); 

  int index = (int)(this->path.find_last_of(separator));
  if (index < prefixLength) 
  {
    this->__internal_name = this->path.substr(prefixLength);
  }
  else
  {
    this->__internal_name = this->path.substr(index + 1);  
  }
}

File::File(const File& parent, const string& child) 
{
  if (child.empty()) 
  {
    System::loggingAndExit("File::<init>|child is empty",-1);
  }
  
  this->__internal_fs = &(FileSystemImpl::getInstance());
    
  if (parent.path.empty()) 
  {
    this->path = __internal_fs->resolve(__internal_fs->getDefaultParent(),__internal_fs->normalize(child));
  } 
  else 
  {
    this->path = __internal_fs->resolve(parent.path,__internal_fs->normalize(child));
  }

  this->prefixLength = __internal_fs->prefixLength(this->path);
  this->__internal_absolutepath = __internal_fs->resolve(*this); 

  int index = (int)(this->path.find_last_of(separator));
  if (index < prefixLength) 
  {
    this->__internal_name = this->path.substr(prefixLength);
  }
  else
  {
    this->__internal_name = this->path.substr(index + 1);  
  }
}

int File::getPrefixLength() const
{
  return prefixLength;
}

const string& File::getName()  const
{
  return __internal_name;
}

string File::getParent()  const
{
	int index = (int)(this->path.find_last_of(separator));
  if (index < prefixLength) 
  {
    if ((prefixLength > 0) && (path.length() > prefixLength))
    {
      return path.substr(0, prefixLength);
    }
    return string();
  }
  
  return path.substr(0, index);
}

File File::getParentFile()  const
{
  return  File(getParent(), this->prefixLength);
}

const string& File::getPath() const
{
  return path;
}

string File::toString()  const
{
  return getPath();
} 

bool File::isAbsolute()  const
{
  return __internal_fs->isAbsolute(*this);
}

const string& File::getAbsolutePath()  const
{
  return __internal_absolutepath;
}

File File::getAbsoluteFile()  const
{
  return  File(__internal_absolutepath);
}

string File::getCanonicalPath()  const __OPENCODE_THROW_DECLARE(IOException)
{
  return __internal_fs->canonicalize(__internal_fs->resolve(*this));
}

File File::getCanonicalFile()  const __OPENCODE_THROW_DECLARE(IOException) 
{
  return  File(getCanonicalPath());
}

bool File::canRead()  const
{
  return __internal_fs->checkAccess(__internal_absolutepath, false);
}

bool File::canWrite()  const
{
  return __internal_fs->checkAccess(__internal_absolutepath, true);
}

bool File::exists()  const
{
  return ((__M_getBooleanAttributes(__internal_absolutepath,__internal_name) & FILE_ATTR_EXISTS) != 0);
}

bool File::isDirectory()  const
{
  return ((__M_getBooleanAttributes(__internal_absolutepath,__internal_name) & FILE_ATTR_DIRECTORY)!= 0);
}

bool File::isFile()  const
{
  return ((__M_getBooleanAttributes(__internal_absolutepath,__internal_name) & FILE_ATTR_REGULAR) != 0);
}

bool File::isLink()  const
{
  return ((__M_getBooleanAttributes(__internal_absolutepath,__internal_name) & FILE_ATTR_LINK) != 0);
}

bool File::isHidden()  const
{  
  return ((__M_getBooleanAttributes(__internal_absolutepath,__internal_name) & FILE_ATTR_HIDDEN) != 0);
}

XLONG File::lastModified()  const
{
  return __M_getLastModified(__internal_absolutepath);
}

XLONG File::length()  const
{
  return __M_getLength(__internal_absolutepath);
}

bool File::createNewFile()  const __OPENCODE_THROW_DECLARE(IOException)
{
  return __internal_fs->createFileExclusively(path);
}

bool File::deleteFile()  const
{
  return __M_deleteFile(__internal_absolutepath);
}

void File::list(vector<string>& lists)  const
{
  __internal_fs->list(__internal_absolutepath,lists);
}

void File::list(const FilenameFilter& filter,vector<string>& lists)  const
{
  vector<string> names;
  __internal_fs->list(__internal_absolutepath,names);
  
  int size = (int)(names.size());
  
  if(size == 0) 
  {
    return;
  }
  
  for (int i = 0 ; i < size ; ++i) 
  {
    if (filter.accept(*this, names[i])) 
    {
      lists.push_back(names[i]);
    }
  }
}

void File::listFiles(vector<File>& files)  const
{
  vector<string> names;
  __internal_fs->list(__internal_absolutepath,names);
  
  int size = (int)(names.size());
  
  for (int i = 0; i < size; ++i) 
  {
    files.push_back(File(this->path, names[i]));
  }
}

void File::listFiles(const FilenameFilter& filter,vector<File>& files)  const
{
  vector<string> names;
  __internal_fs->list(__internal_absolutepath,names);
  
  int size = (int)(names.size());  
  
  for (int i = 0 ; i < size ; ++i) 
  {
    if (filter.accept(*this, names[i])) 
    {
      files.push_back(File(this->path,names[i]));
    }
  }
}

void File::listFiles(const FileFilter& filter,vector<File>& files)  const
{

  vector<string> names;
  __internal_fs->list(__internal_absolutepath,names);
  
  int size = (int)(names.size());  
  
  for (int i = 0 ; i < size ; ++i) 
  {
    File f(this->path,names[i]);
    if (filter.accept(f)) 
    {
      files.push_back(f);
    }
  }
}

bool File::mkdir(int filemask)  const
{
  return __internal_fs->createDirectory(__internal_absolutepath,filemask);
}

bool File::mkdirs(int filemask)  const
{
  if (exists()) 
  {
    return false;
  }
  
  if (mkdir(filemask)) 
  {
    return true;
  }
  
  File canonFile;
  __OPENCODE_TRY 
  {
    canonFile = getCanonicalFile();
  } 
  __OPENCODE_CATCH (IOException& e) 
  {
    return false;
  }
  
  string parent = canonFile.getParent();
  
  return (!parent.empty()) && (File(parent).mkdirs() && canonFile.mkdir(filemask));
}

bool File::renameTo(const string& dest)  const
{
  return __M_renameTo(__internal_absolutepath, dest);
}

bool File::setLastModified(XLONG time)  const
{
  if (time < 0) 
  {
    LOGGING_WARN("File::setLastModified|Negative time"); 
    return false;
  }

  return __internal_fs->setLastModifiedTime(__internal_absolutepath, time);
}

bool File::setReadOnly()  const
{
  return __internal_fs->setReadOnly(__internal_absolutepath);
}

void File::listRoots(vector<File>& lists)  const
{
  __internal_fs->listRoots(lists);
}

File File::createTempFile(const string& prefix, const string& suffix,const File* directory) __OPENCODE_THROW_DECLARE(IOException)
{
  FileSystem* __internal_fs = &(FileSystemImpl::getInstance());
  return __internal_fs->createTempFile(prefix,suffix,directory);
}

File File::createTempFile(const string& prefix, const string& suffix) __OPENCODE_THROW_DECLARE(IOException)
{
  FileSystem* __internal_fs = &(FileSystemImpl::getInstance());
  return createTempFile(prefix, suffix, 0);
}

string  File::getCurrentDirectory()
{
  FileSystem* __internal_fs = &(FileSystemImpl::getInstance());
  return  __internal_fs->getCurrentDirectory();
}

string  File::getDefaultParent()
{
  FileSystem* __internal_fs = &(FileSystemImpl::getInstance());
  return  __internal_fs->getDefaultParent();
}

__OPENCODE_END_NAMESPACE
