/**
 *  file.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_FILE_H__
#define __OPENCODE_FILE_H__

#include "port/libdefs.h"
#include "port/ioc_fdir.h"
#include "port/ioc_vector.h"

__OPENCODE_BEGIN_NAMESPACE

/**
 * Return the simple boolean attributes for the file or directory denoted
 * by the given abstract pathname, or zero if it does not exist or some
 * rv I/O error occurs.
 */
__OPENCODE_CORE_API_DECL int __M_getBooleanAttributes(const string& pathname,const string& name);

/**
 * Rename the file or directory denoted by the first abstract pathname to
 * the second abstract pathname, returning <code>true</code> if and only if
 * the operation succeeds.
 */
__OPENCODE_CORE_API_DECL bool  __M_renameTo(const string& pathname1,const string& pathname2);

/**
 * Deletes the file or directory denoted by this abstract pathname.  If
 * this pathname denotes a directory, then the directory must be empty in
 * order to be deleted.
 *
 * @return  <code>true</code> if and only if the file or directory is
 *          successfully deleted; <code>false</code> otherwise
 */
__OPENCODE_CORE_API_DECL bool __M_deleteFile(const string& pathname);

/**
 * Return the length in bytes of the file denoted by the given abstract
 * pathname, or zero if it does not exist, is a directory, or some other
 * I/O error occurs.
 */
__OPENCODE_CORE_API_DECL XLONG __M_getLength(const string& pathname);

/**
 * Return the time at which the file or directory denoted by the given
 * abstract pathname was last modified, or zero if it does not exist or
 * some other I/O error occurs.
 */
__OPENCODE_CORE_API_DECL XLONG __M_getLastModified(const string& pathname) ;

/**
 * Tests whether the file or directory denoted by this abstract pathname
 * exists.
 *
 * @return  <code>true</code> if and only if the file or directory denoted
 *          by this abstract pathname exists; <code>false</code> otherwise
 */
__OPENCODE_CORE_API_DECL bool  __M_exists(const string& pathname);

class __OPENCODE_CORE_API_DECL IOException : public BaseException
{
public:
  ~IOException();
  
  IOException();
  IOException(const string& message);
};

/**
 * An abstract representation of file and directory pathnames.
 *
 * <p> User interfaces and operating systems use system-dependent <em>pathname
 * strings</em> to name files and directories.  This class presents an
 * abstract, system-independent view of hierarchical pathnames.  An
 * <em>abstract pathname</em> has two components:
 *
 * <ol>
 * <li> An optional system-dependent <em>prefix</em> string,
 *      such as a disk-drive specifier, <code>"/"</code>&nbsp;for the UNIX root
 *      directory, or <code>"\\"</code>&nbsp;for a Microsoft Windows UNC pathname, and
 * <li> A sequence of zero or more string <em>names</em>.
 * </ol>
 *
 * Each name in an abstract pathname except for the last denotes a directory;
 * the last name may denote either a directory or a file.  The <em>empty</em>
 * abstract pathname has no prefix and an empty name sequence.
 *
 * <p> The conversion of a pathname string to or from an abstract pathname is
 * inherently system-dependent.  When an abstract pathname is converted into a
 * pathname string, each name is separated from the next by a single copy of
 * the default <em>separator character</em>.  The default name-separator
 * character is defined by the system property <code>file.separator</code>, and
 * is made available in the public static fields <code>{@link
 * #separator}</code> and <code>{@link #separatorChar}</code> of this class.
 * When a pathname string is converted into an abstract pathname, the names
 * within it may be separated by the default name-separator character or by any
 * rv name-separator character that is supported by the underlying system.
 *
 * <p> A pathname, whether abstract or in string form, may be either
 * <em>absolute</em> or <em>relative</em>.  An absolute pathname is complete in
 * that no rv information is required in order to locate the file that it
 * denotes.  A relative pathname, in contrast, must be interpreted in terms of
 * information taken from some other pathname.  By default the class 
 * always resolve relative pathnames against the
 * current user directory.  This directory is named by the system property
 * <code>user.dir</code>, and is typically the directory in which the library was loaded  
 *
 * <p> The prefix concept is used to handle root directories on UNIX platforms,
 * and drive specifiers, root directories and UNC pathnames on Microsoft Windows platforms,
 * as follows:
 *
 * <ul>
 *
 * <li> For UNIX platforms, the prefix of an absolute pathname is always
 * <code>"/"</code>.  Relative pathnames have no prefix.  The abstract pathname
 * denoting the root directory has the prefix <code>"/"</code> and an empty
 * name sequence.
 *
 * <li> For Microsoft Windows platforms, the prefix of a pathname that contains a drive
 * specifier consists of the drive letter followed by <code>":"</code> and
 * possibly followed by <code>"\"</code> if the pathname is absolute.  The
 * prefix of a UNC pathname is <code>"\\"</code>; the hostname and the share
 * name are the first two names in the name sequence.  A relative pathname that
 * does not specify a drive has no prefix.
 *
 * </ul>
 *
 * <p> Instances of the <code>File</code> class are immutable; that is, once
 * created, the abstract pathname represented by a <code>File</code> object
 * will never change.
 *
 */
class FileFilter;
class FilenameFilter;
class FileSystem;

class __OPENCODE_CORE_API_DECL File
{
  /* -- Constructors -- */

private:  
  /**
   * Internal constructor for already-normalized pathname const string&s.
   */
  File(const string& pathname, int prefixLength) ;

public:  
  File();
  
  File(const File& rhs);
  File& operator = (const File& rhs);
  
public:
  /**
   * Creates a new <code>File</code> instance by converting the given
   * pathname stringinto an abstract pathname.  If the given stringis
   * the empty const string&, then the result is the empty abstract pathname.
   *
   * @param   pathname  A pathname string
   */
  File(const string& pathname) ;
  
  /** Note: The two-argument File constructors do not interpret an empty
   *  parent abstract pathname as the current user directory.  An empty parent
   *  instead causes the child to be resolved against the system-dependent
   *  directory defined by the FileSystem.getDefaultParent method.  On Unix
   *  this default is "/", while on Microsoft Windows it is "\\".  This is required for
   *  compatibility with the original behavior of this class. 
   */
  
  /**
   * Creates a new <code>File</code> instance from a parent pathname const string&
   * and a child pathname string
   *
   * <p> If <code>parent</code> is <code>null</code> then the new
   * <code>File</code> instance is created as if by invoking the
   * single-argument <code>File</code> constructor on the given
   * <code>child</code> pathname string
   *
   * <p> Otherwise the <code>parent</code> pathname stringis taken to denote
   * a directory, and the <code>child</code> pathname stringis taken to
   * denote either a directory or a file.  If the <code>child</code> pathname
   * stringis absolute then it is converted into a relative pathname in a
   * system-dependent way.  If <code>parent</code> is the empty stringthen
   * the new <code>File</code> instance is created by converting
   * <code>child</code> into an abstract pathname and resolving the result
   * against a system-dependent default directory.  Otherwise each pathname
   * stringis converted into an abstract pathname and the child abstract
   * pathname is resolved against the parent.
   *
   * @param   parent  The parent pathname string
   * @param   child   The child pathname string
   */
  File(const char* parent, const string& child) ;
  File(const string& parent, const string& child) ;
  
  /**
   * Creates a new <code>File</code> instance from a parent abstract
   * pathname and a child pathname string
   *
   * <p> If <code>parent</code> is <code>null</code> then the new
   * <code>File</code> instance is created as if by invoking the
   * single-argument <code>File</code> constructor on the given
   * <code>child</code> pathname string
   *
   * <p> Otherwise the <code>parent</code> abstract pathname is taken to
   * denote a directory, and the <code>child</code> pathname stringis taken
   * to denote either a directory or a file.  If the <code>child</code>
   * pathname stringis absolute then it is converted into a relative
   * pathname in a system-dependent way.  If <code>parent</code> is the empty
   * abstract pathname then the new <code>File</code> instance is created by
   * converting <code>child</code> into an abstract pathname and resolving
   * the result against a system-dependent default directory.  Otherwise each
   * pathname stringis converted into an abstract pathname and the child
   * abstract pathname is resolved against the parent.
   *
   * @param   parent  The parent abstract pathname
   * @param   child   The child pathname const string&
   */
  File(const File& parent, const string& child) ;
 
  
  /* -- Path-component accessors -- */ 
public:
  /**
   * Returns the length of this abstract pathname's prefix.
   * For use by FileSystem classes.
   */
  int getPrefixLength() const;
  
    
  /**
   * Returns the name of the file or directory denoted by this abstract
   * pathname.  This is just the last name in the pathname's name
   * sequence.  If the pathname's name sequence is empty, then the empty
   * stringis returned.
   *
   * @return  The name of the file or directory denoted by this abstract
   *          pathname, or the empty stringif this pathname's name sequence
   *          is empty
   */
  const string& getName()  const;
  
  /**
   * Returns the pathname stringof this abstract pathname's parent, or
   * <code>null</code> if this pathname does not name a parent directory.
   *
   * <p> The <em>parent</em> of an abstract pathname consists of the
   * pathname's prefix, if any, and each name in the pathname's name
   * sequence except for the last.  If the name sequence is empty then
   * the pathname does not name a parent directory.
   *
   * @return  The pathname stringof the parent directory named by this
   *          abstract pathname, or <code>null</code> if this pathname
   *          does not name a parent
   */
  string getParent()  const;
  
  /**
   * Returns the abstract pathname of this abstract pathname's parent,
   * or <code>null</code> if this pathname does not name a parent
   * directory.
   *
   * <p> The <em>parent</em> of an abstract pathname consists of the
   * pathname's prefix, if any, and each name in the pathname's name
   * sequence except for the last.  If the name sequence is empty then
   * the pathname does not name a parent directory.
   *
   * @return  The abstract pathname of the parent directory named by this
   *          abstract pathname, or <code>null</code> if this pathname
   *          does not name a parent
   *
   */
  File getParentFile()  const;
  
  /**
   * Converts this abstract pathname into a pathname string  The resulting
   * string uses the {@link #separator default name-separator character} to
   * separate the names in the name sequence.
   *
   * @return  The string form of this abstract pathname
   */
  const string& getPath() const;
  
   /**
   * Returns the pathname stringof this abstract pathname.  This is just the
   * stringreturned by the <code>{@link #getPath}</code> method.
   *
   * @return  The stringform of this abstract pathname
   */
  string toString()  const;
  
  /* -- Path operations -- */
public:  
  /**
   * Tests whether this abstract pathname is absolute.  The definition of
   * absolute pathname is system dependent.  On UNIX systems, a pathname is
   * absolute if its prefix is <code>"/"</code>.  On Microsoft Windows systems, a
   * pathname is absolute if its prefix is a drive specifier followed by
   * <code>"\\"</code>, or if its prefix is <code>"\\"</code>.
   *
   * @return  <code>true</code> if this abstract pathname is absolute,
   *          <code>false</code> otherwise
   */
  bool isAbsolute()  const;
  
  /**
   * Returns the absolute pathname stringof this abstract pathname.
   *
   * <p> If this abstract pathname is already absolute, then the pathname
   * stringis simply returned as if by the <code>{@link #getPath}</code>
   * method.  If this abstract pathname is the empty abstract pathname then
   * the pathname stringof the current user directory, which is named by the
   * system property <code>user.dir</code>, is returned.  Otherwise this
   * pathname is resolved in a system-dependent way.  On UNIX systems, a
   * relative pathname is made absolute by resolving it against the current
   * user directory.  On Microsoft Windows systems, a relative pathname is made absolute
   * by resolving it against the current directory of the drive named by the
   * pathname, if any; if not, it is resolved against the current user
   * directory.
   *
   * @return  The absolute pathname stringdenoting the same file or
   *          directory as this abstract pathname
   */
  const string& getAbsolutePath()  const;
  
  /**
   * Returns the absolute form of this abstract pathname.  Equivalent to
   * <code>new&nbsp;File(this->{@link #getAbsolutePath}())</code>.
   *
   * @return  The absolute abstract pathname denoting the same file or
   *          directory as this abstract pathname
   */
  File getAbsoluteFile()  const;
  
  /**
   * Returns the canonical pathname stringof this abstract pathname.
   *
   * <p> A canonical pathname is both absolute and unique.  The precise
   * definition of canonical form is system-dependent.  This method first
   * converts this pathname to absolute form if necessary, as if by invoking the
   * {@link #getAbsolutePath} method, and then maps it to its unique form in a
   * system-dependent way.  This typically involves removing redundant names
   * such as <tt>"."</tt> and <tt>".."</tt> from the pathname, resolving
   * symbolic links (on UNIX platforms), and converting drive letters to a
   * standard case (on Microsoft Windows platforms).
   *
   * <p> Every pathname that denotes an existing file or directory has a
   * unique canonical form.  Every pathname that denotes a nonexistent file
   * or directory also has a unique canonical form.  The canonical form of
   * the pathname of a nonexistent file or directory may be different from
   * the canonical form of the same pathname after the file or directory is
   * created.  Similarly, the canonical form of the pathname of an existing
   * file or directory may be different from the canonical form of the same
   * pathname after the file or directory is deleted.
   *
   * @return  The canonical pathname stringdenoting the same file or
   *          directory as this abstract pathname
   *
   * @throws  IOException
   *          If an I/O error occurs, which is possible because the
   *          construction of the canonical pathname may require
   *          filesystem queries
   */
  string getCanonicalPath()  const __OPENCODE_THROW_DECLARE(IOException);
  
  /**
   * Returns the canonical form of this abstract pathname.  Equivalent to
   * <code>new&nbsp;File(this->{@link #getCanonicalPath}())</code>.
   *
   * @return  The canonical pathname stringdenoting the same file or
   *          directory as this abstract pathname
   *
   * @throws  IOException
   *          If an I/O error occurs, which is possible because the
   *          construction of the canonical pathname may require
   *          filesystem queries
   */
  File getCanonicalFile()  const __OPENCODE_THROW_DECLARE(IOException) ;
  
  /* -- Attribute accessors -- */
public:  
  /**
   * Tests whether the application can read the file denoted by this
   * abstract pathname.
   *
   * @return  <code>true</code> if and only if the file specified by this
   *          abstract pathname exists <em>and</em> can be read by the
   *          application; <code>false</code> otherwise
   */
  bool canRead()  const;
  
  /**
   * Tests whether the application can modify to the file denoted by this
   * abstract pathname.
   *
   * @return  <code>true</code> if and only if the file system actually
   *          contains a file denoted by this abstract pathname <em>and</em>
   *          the application is allowed to write to the file;
   *          <code>false</code> otherwise.
   */
  bool canWrite()  const;
  
  /**
   * Tests whether the file or directory denoted by this abstract pathname
   * exists.
   *
   * @return  <code>true</code> if and only if the file or directory denoted
   *          by this abstract pathname exists; <code>false</code> otherwise
   */
  bool exists()  const;
  
  /**
   * Tests whether the file denoted by this abstract pathname is a
   * directory.
   *
   * @return <code>true</code> if and only if the file denoted by this
   *          abstract pathname exists <em>and</em> is a directory;
   *          <code>false</code> otherwise
   */
  bool isDirectory()  const;
  
  /**
   * Tests whether the file denoted by this abstract pathname is a normal
   * file.  A file is <em>normal</em> if it is not a directory and, in
   * addition, satisfies rv system-dependent criteria.  Any non-directory
   * file created by a Java application is guaranteed to be a normal file.
   *
   * @return  <code>true</code> if and only if the file denoted by this
   *          abstract pathname exists <em>and</em> is a normal file;
   *          <code>false</code> otherwise
   */
  bool isFile()  const;

  /**
   * Tests whether the file denoted by this abstract pathname is a link (or shortcut). 
   *
   * @return  <code>true</code> if and only if the file denoted by this
   *          abstract pathname exists <em>and</em> is a link;
   *          <code>false</code> otherwise
   */
  bool isLink()  const;

  /**
   * Tests whether the file named by this abstract pathname is a hidden
   * file.  The exact definition of <em>hidden</em> is system-dependent.  On
   * UNIX systems, a file is considered to be hidden if its name begins with
   * a period character (<code>'.'</code>).  On Microsoft Windows systems, a file is
   * considered to be hidden if it has been marked as such in the filesystem.
   *
   * @return  <code>true</code> if and only if the file denoted by this
   *          abstract pathname is hidden according to the conventions of the
   *          underlying platform
   */
  bool isHidden()  const;
  
  /**
   * Returns the time that the file denoted by this abstract pathname was
   * last modified.
   *
   * @return  A <code>XLONG</code> value representing the time the file was
   *          last modified, measured in milliseconds since the epoch
   *          (00:00:00 GMT, January 1, 1970), or <code>0L</code> if the
   *          file does not exist or if an I/O error occurs
   */
  XLONG lastModified()  const;
  
  /**
   * Returns the length of the file denoted by this abstract pathname.
   * The return value is unspecified if this pathname denotes a directory.
   *
   * @return  The length, in bytes, of the file denoted by this abstract
   *          pathname, or <code>0L</code> if the file does not exist
   */
  XLONG length()  const; 
  
  /* -- File operations -- */
public:  
  /**
   * Atomically creates a new, empty file named by this abstract pathname if
   * and only if a file with this name does not yet exist.  The check for the
   * existence of the file and the creation of the file if it does not exist
   * are a single operation that is atomic with respect to all rv
   * filesystem activities that might affect the file.
   *
   * @return  <code>true</code> if the named file does not exist and was
   *          successfully created; <code>false</code> if the named file
   *          already exists
   *
   * @throws  IOException
   *          If an I/O error occurred
   */
  bool createNewFile()  const __OPENCODE_THROW_DECLARE(IOException);
  
  /**
   * Deletes the file or directory denoted by this abstract pathname.  If
   * this pathname denotes a directory, then the directory must be empty in
   * order to be deleted.
   *
   * @return  <code>true</code> if and only if the file or directory is
   *          successfully deleted; <code>false</code> otherwise
   */
  bool deleteFile()  const;
  
  /**
   * Returns an array of const string&s naming the files and directories in the
   * directory denoted by this abstract pathname.
   *
   * <p> If this abstract pathname does not denote a directory, then this
   * method returns <code>null</code>.  Otherwise an array of const string&s is
   * returned, one for each file or directory in the directory.  Names
   * denoting the directory itself and the directory's parent directory are
   * not included in the result.  Each stringis a file name rather than a
   * complete path.
   *
   * <p> There is no guarantee that the name const string&s in the resulting array
   * will appear in any specific order; they are not, in particular,
   * guaranteed to appear in alphabetical order.
   *
   * @return  An array of const string&s naming the files and directories in the
   *          directory denoted by this abstract pathname.  The array will be
   *          empty if the directory is empty or if
   *          this abstract pathname does not denote a directory, or if an
   *          I/O error occurs.
   */
  void list(vector<string>& lists)  const;
  
  /**
   * Returns an array of const string&s naming the files and directories in the
   * directory denoted by this abstract pathname that satisfy the specified
   * filter.  The behavior of this method is the same as that of the
   * <code>{@link #list()}</code> method, except that the const string&s in the
   * returned array must satisfy the filter.  If the given
   * <code>filter</code> is <code>null</code> then all names are accepted.
   * Otherwise, a name satisfies the filter if and only if the value
   * <code>true</code> results when the <code>{@link
   * FilenameFilter#accept}</code> method of the filter is invoked on this
  * abstract pathname and the name of a file or directory in the directory
  * that it denotes.
  *
  * @param  filter  A filename filter
  *
  * @return  An array of const string&s naming the files and directories in the
  *          directory denoted by this abstract pathname that were accepted
  *          by the given <code>filter</code>.  The array will be empty if
  *          the directory is empty or if no names were accepted by the
  *          filter.  Returns <code>null</code> if this abstract pathname
  *          does not denote a directory, or if an I/O error occurs.
  */
  void list(const FilenameFilter& filter,vector<string>& lists)  const;
  
  /**
  * Returns an array of abstract pathnames denoting the files in the
  * directory denoted by this abstract pathname.
  *
  * <p> If this abstract pathname does not denote a directory, then this
  * method returns <code>null</code>.  Otherwise an array of
  * <code>File</code> objects is returned, one for each file or directory in
  * the directory.  Pathnames denoting the directory itself and the
  * directory's parent directory are not included in the result.  Each
  * resulting abstract pathname is constructed from this abstract pathname
  * using the <code>{@link #File(base.File, const string&)
  * File(File,&nbsp;const string&)}</code> constructor.  Therefore if this pathname
  * is absolute then each resulting pathname is absolute; if this pathname
  * is relative then each resulting pathname will be relative to the same
  * directory.
  *
  * <p> There is no guarantee that the name const string&s in the resulting array
  * will appear in any specific order; they are not, in particular,
  * guaranteed to appear in alphabetical order.
  *
  * @return  An array of abstract pathnames denoting the files and
  *          directories in the directory denoted by this abstract
  *          pathname.  The array will be empty if the directory is
  *          empty.  Returns <code>null</code> if this abstract pathname
  *          does not denote a directory, or if an I/O error occurs.
  */
  
  void listFiles(vector<File>& files)  const;
  
  /**
  * Returns an array of abstract pathnames denoting the files and
  * directories in the directory denoted by this abstract pathname that
  * satisfy the specified filter.  The behavior of this method is the
  * same as that of the <code>{@link #listFiles()}</code> method, except
  * that the pathnames in the returned array must satisfy the filter.
  * If the given <code>filter</code> is <code>null</code> then all
  * pathnames are accepted.  Otherwise, a pathname satisfies the filter
  * if and only if the value <code>true</code> results when the
  * <code>{@link FilenameFilter#accept}</code> method of the filter is
  * invoked on this abstract pathname and the name of a file or
  * directory in the directory that it denotes.
  *
  * @param  filter  A filename filter
  *
  * @return  An array of abstract pathnames denoting the files and
  *          directories in the directory denoted by this abstract
  *          pathname.  The array will be empty if the directory is
  *          empty.  Returns <code>null</code> if this abstract pathname
  *          does not denote a directory, or if an I/O error occurs.
  */
  
  void listFiles(const FilenameFilter& filter,vector<File> & files)  const;
  
  /**
  * Returns an array of abstract pathnames denoting the files and
  * directories in the directory denoted by this abstract pathname that
  * satisfy the specified filter.  The behavior of this method is the
  * same as that of the <code>{@link #listFiles()}</code> method, except
  * that the pathnames in the returned array must satisfy the filter.
  * If the given <code>filter</code> is <code>null</code> then all
  * pathnames are accepted.  Otherwise, a pathname satisfies the filter
  * if and only if the value <code>true</code> results when the
  * <code>{@link FileFilter#accept(base.File)}</code> method of
  * the filter is invoked on the pathname.
  *
  * @param  filter  A file filter
  *
  * @return  An array of abstract pathnames denoting the files and
  *          directories in the directory denoted by this abstract
  *          pathname.  The array will be empty if the directory is
  *          empty.  Returns <code>null</code> if this abstract pathname
  *          does not denote a directory, or if an I/O error occurs.
  */
  void listFiles(const FileFilter& filter,vector<File>& files)  const;
  
  /**
  * Creates the directory named by this abstract pathname.
  *
  * @return  <code>true</code> if and only if the directory was
  *          created; <code>false</code> otherwise
  */
  bool mkdir(int filemask = 0666)  const;
  
  /**
  * Creates the directory named by this abstract pathname, including any
  * necessary but nonexistent parent directories.  Note that if this
  * operation fails it may have succeeded in creating some of the necessary
  * parent directories.
  *
  * @return  <code>true</code> if and only if the directory was created,
  *          along with all necessary parent directories; <code>false</code>
  *          otherwise
  *
  */
  bool mkdirs(int filemask = 0666)  const;
  
  /**
  * Renames the file denoted by this abstract pathname.
  * 
  * <p> Whether or not this method can move a file from one filesystem 
  * to another is platform-dependent.  The return value should always 
  * be checked to make sure that the rename operation was successful.
  *
  * @param  dest  The new abstract pathname for the named file
  * 
  * @return  <code>true</code> if and only if the renaming succeeded;
  *          <code>false</code> otherwise
  *
  */
  bool renameTo(const string& dest)  const;
  
  /**
  * Sets the last-modified time of the file or directory named by this
  * abstract pathname.
  *
  * <p> All platforms support file-modification times to the nearest second,
  * but some provide more precision.  The argument will be truncated to fit
  * the supported precision.  If the operation succeeds and no intervening
  * operations on the file take place, then the next invocation of the
  * <code>{@link #lastModified}</code> method will return the (possibly
  * truncated) <code>time</code> argument that was passed to this method.
  *
  * @param  time  The new last-modified time, measured in milliseconds since
  *               the epoch (00:00:00 GMT, January 1, 1970)
  *
  * @return <code>true</code> if and only if the operation succeeded;
  *          <code>false</code> otherwise
  */
  bool setLastModified(XLONG time)  const;
  
  /**
  * Marks the file or directory named by this abstract pathname so that
  * only read operations are allowed.  After invoking this method the file
  * or directory is guaranteed not to change until it is either deleted or
  * marked to allow write access.  Whether or not a read-only file or
  * directory may be deleted depends upon the underlying system.
  *
  * @return <code>true</code> if and only if the operation succeeded;
  *          <code>false</code> otherwise
  */
  bool setReadOnly()  const;
  

  /* -- Filesystem interface -- */
public:  
  /**
  * List the available filesystem roots.
  *
  * <p> A particular Java platform may support zero or more
  * hierarchically-organized file systems.  Each file system has a
  * <code>root</code> directory from which all rv files in that file
  * system can be reached.  Windows platforms, for example, have a root
  * directory for each active drive; UNIX platforms have a single root
  * directory, namely <code>"/"</code>.  The set of available filesystem
  * roots is affected by various system-level operations such the insertion
  * or ejection of removable media and the disconnecting or unmounting of
  * physical or virtual disk drives.
  *
  * <p> This method returns an array of <code>File</code> objects that
  * denote the root directories of the available filesystem roots.  It is
  * guaranteed that the canonical pathname of any file physically present on
  * the local machine will begin with one of the roots returned by this
  * method.
  *
  * <p> The canonical pathname of a file that resides on some other machine
  * and is accessed via a remote-filesystem protocol such as SMB or NFS may
  * or may not begin with one of the roots returned by this method.  If the
  * pathname of a remote file is syntactically indistinguishable from the
  * pathname of a local file then it will begin with one of the roots
  * returned by this method.  Thus, for example, <code>File</code> objects
  * denoting the root directories of the mapped network drives of a Windows
  * platform will be returned by this method, while <code>File</code>
  * objects containing UNC pathnames will not be returned by this method.
  *
  * @return  An array of <code>File</code> objects denoting the available
  *          filesystem roots, or <code>null</code> if the set of roots
  *          could not be determined.  The array will be empty if there are
  *          no filesystem roots.
  *
  */
  void listRoots(vector<File> & lists)  const;
  

  /* -- Temporary files -- */
  
public:  
  /**
  * <p> Creates a new empty file in the specified directory, using the
  * given prefix and suffix strings to generate its name. 
  *
  * <p> To create the new file, the prefix and the suffix may first be
  * adjusted to fit the limitations of the underlying platform.  If the
  * prefix is too long then it will be truncated, but its first three
  * characters will always be preserved.  If the suffix is too long then it
  * too will be truncated, but if it begins with a period character
  * (<code>'.'</code>) then the period and the first three characters
  * following it will always be preserved.  Once these adjustments have been
  * made the name of the new file will be generated by concatenating the
  * prefix, five or more internally-generated characters, and the suffix.
  *
  * <p> If the <code>directory</code> argument is <code>null</code> then the
  * system-dependent default temporary-file directory will be used.  The
  * default temporary-file directory is specified by the system property
  * <code>base.tmpdir</code>.  On UNIX systems the default value of this
  * property is typically <code>"/tmp"</code> or <code>"/var/tmp"</code>; on
  * Microsoft Windows systems it is typically <code>"c:\\temp"</code>.  A different
  * value may be given to this system property when the Java virtual machine
  * is invoked, but programmatic changes to this property are not guaranteed
  * to have any effect upon the the temporary directory used by this method.
  *
  * @param  prefix     The prefix stringto be used in generating the file's
  *                    name; must be at least three characters long
  *
  * @param  suffix     The suffix stringto be used in generating the file's
  *                    name; may be <code>null</code>, in which case the
  *                    suffix <code>".tmp"</code> will be used
  *
  * @param  directory  The directory in which the file is to be created, or
  *                    <code>null</code> if the default temporary-file
  *                    directory is to be used
  *
  * @return  An abstract pathname denoting a newly-created empty file
  *
  * @throws  IOException  If a file could not be created
  */
  static File createTempFile(const string& prefix, const string& suffix,const File* directory) __OPENCODE_THROW_DECLARE(IOException);
  
  /**
   * Creates an empty file in the default temporary-file directory, using
   * the given prefix and suffix to generate its name.  Invoking this method
   * is equivalent to invoking <code>{@link #createTempFile(const string&,
   * const string&,const base.File*)
   * createTempFile(prefix,&nbsp;suffix,&nbsp;null)}</code>.
   *
   * @param  prefix     The prefix stringto be used in generating the file's
   *                    name; must be at least three characters long
   *
   * @param  suffix     The suffix stringto be used in generating the file's
   *                    name; may be <code>null</code>, in which case the
   *                    suffix <code>".tmp"</code> will be used
   *
   * @return  An abstract pathname denoting a newly-created empty file
   *
   *
   * @throws  IOException  If a file could not be created
   *
   */
  static File createTempFile(const string& prefix, const string& suffix) __OPENCODE_THROW_DECLARE(IOException);

 /**
  *  return the absolute pathname of current working directory
  */
  static  string getCurrentDirectory();

 /**
  * Return the parent pathname string to be used when the parent-directory
  * argument in one of the two-argument File constructors is the empty
  * pathname.
  */
  static string getDefaultParent() ;

private:
  /**
   * This abstract pathname's normalized pathname string.  A normalized
   * pathname string uses the default name-separator character and does not
   * contain any duplicate or redundant separators.
   */
  mutable string path;

  /**
   * The length of this abstract pathname's prefix, or zero if it has no
   * prefix.
   */
  mutable int prefixLength;
  
  mutable string __internal_absolutepath;
  mutable string __internal_name;
  mutable FileSystem* __internal_fs;
  
public:

  /**
   * The system-dependent default name-separator character. 
   * On UNIX systems the value of this  field is <code>'/'</code>;
   */
  const static char   separatorChar ;
  
  /**
   * The system-dependent default name-separator character, represented as a
   * string for convenience.  This string contains a single character, namely
   * <code>{@link #separatorChar}</code>.
   */ 
  const static char separator[2];

  /**
   * The system-dependent path-separator character.  This field is
   * initialized to contain the first character of the value of the system
   * property <code>path.separator</code>.  This character is used to
   * separate filenames in a sequence of files given as a <em>path list</em>.
   * On UNIX systems, this character is <code>':'</code>
   */
  const static  char pathSeparatorChar; 

  /**
   * The system-dependent path-separator character, represented as a string
   * for convenience.  This string contains a single character, namely
   * <code>{@link #pathSeparatorChar}</code>.
   */
  const static char pathSeparator[2];  
  
  
  /**
   * The system-dependent line-separator character, represented as a string
   * for convenience.
   */
  const static char lineSeparator[3];  
};

/**
 * A filter for abstract pathnames.
 *
 * <p> Instances of this interface may be passed to the <code>{@link
 * File#listFiles(FileFilter) listFiles(FileFilter)}</code> method
 * of the <code>{File}</code> class.
 *
 */
class __OPENCODE_CORE_API_DECL FileFilter 
{
public:
  /**
   * Tests whether or not the specified abstract pathname should be
   * included in a pathname list.
   *
   * @param  pathname  The abstract pathname to be tested
   * @return  <code>true</code> if and only if <code>pathname</code>
   *          should be included
   */
  virtual bool accept(const File& pathname) const = 0;

};

/**
 * Instances of classes that implement this interface are used to
 * filter filenames. These instances are used to filter directory
 * listings in the <code>list</code> method of class
 * <code>File</code>, and by the Abstract Window Toolkit's file
 * dialog component.
 *
 */
class __OPENCODE_CORE_API_DECL FilenameFilter 
{
public:
 /**
  * Tests if a specified file should be included in a file list.
  *
  * @param   dir    the directory in which the file was found.
  * @param   name   the name of the file.
  * @return  <code>true</code> if and only if the name should be
  * included in the file list; <code>false</code> otherwise.
  */
  virtual bool accept(const File& dir, const string& name) const = 0;
};

__OPENCODE_END_NAMESPACE

#endif
