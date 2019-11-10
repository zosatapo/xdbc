#include "port/lineparser.h"
#include "port/ioc_sbuf.h" 

__OPENCODE_BEGIN_NAMESPACE

LineError::~LineError(){}
LineError::LineError()
:code(0),line(-1){}
  
LineError::LineError(const XLONG& errcode,int errorline,const char* errmsg)
:code(errcode),line(errorline)
{
  if(errmsg != 0)
  {
    msg = errmsg;
  }  
}  
     
LineFile::~LineFile(){}
LineFile::LineFile(FILE* fp)
:fp(fp){}
  
LineError LineFile::process(LineReader& reader,LineParser& parser, LineErrorHandler& errorHandler)
{  
  char __line[OPENCODE_MAXLINE + 1] = {0};
  XLONG __line_size = 0; 
  LineError __line_error;
  
  while( (__line_size = reader.read(fp,__line,OPENCODE_MAXLINE,__line_error)) > 0)
  {
    if(parser.parse(__line,__line_size,__line_error) != 0)
    {
      if(errorHandler.handle(__line_error) != 0)
      {
        break;
      }  
    }  
  }
  
  return __line_error;
}

DefaultLineErrorHandler::~DefaultLineErrorHandler(){}
DefaultLineErrorHandler::DefaultLineErrorHandler(){}
XLONG DefaultLineErrorHandler::handle(const LineError& error)
{
  return error.code;
} 

DefaultLineReader::~DefaultLineReader(){}
DefaultLineReader::DefaultLineReader(){}

XLONG DefaultLineReader::read(FILE* fp,char* line,XLONG size,LineError& error)
{  
  return LineFile::readLine(fp,line,size);
}

const char LineFile::keyValueSeparators[]       = "=: \t\r\n\f";
const char LineFile::strictKeyValueSeparators[] = "=:";
const char LineFile::specialSaveChars[]         = "=: \t\r\n\f#!";
const char LineFile::whiteSpaceChars[]          = " \t\r\n\f";
const char LineFile::commentLineChars[]         = "#!;";

XLONG LineFile::readLine(FILE* fp,char* line,XLONG size,bool supportContineLine)
{
  if(line == 0 || size <= 0)
  {
    return 0;
  }  

  XLONG __bytes_read = 0;
 
  while(true)
  {
    if(fgets(line,OPENCODE_MAXLINE,fp) == 0)
    {
      return 0;
    }
    LineFile::chopCRLF(line);
    //printf("LineFile::readLine[RAW] - |%s|\n",line);
    if(line[0] == '\0')
    {
      continue;
    }

    // Continue lines that end in slashes if they are not comments
    if (strchr(LineFile::commentLineChars,line[0]) == 0)
    {
      __bytes_read = strlen(line);

      while (supportContineLine && LineFile::continueLine(line))
      {
        char __nextLine[OPENCODE_MAXLINE + 1] = {0};
        if( fgets(__nextLine,OPENCODE_MAXLINE,fp) == 0)
        {
          break;
        }
         
        LineFile::chopCRLF(__nextLine);
       // printf("LineFile::readLine[RAW] - |%s|\n",__nextLine);
        if(__nextLine[0] == '\0')
        {
          continue;
        }

        size_t __nextLineLen = strlen(__nextLine);
        line[__bytes_read - 1] = '\0'; //chop the concatenation character '\\'
        strcat(line,__nextLine);
        __bytes_read +=  (__nextLineLen -1);
  
      }//~end while(supportContineLine && LineFile::continueLine(line))

      break;

    } //~end  if (strchr(LineFile::commentLineChars,firstChar) == 0)
   
  }//~end while(true)

  //printf("LineFile::readLine[RET] [%d] - |%s|\n",__bytes_read,line);
  return __bytes_read;
}

bool LineFile::continueLine(const char* line)
{
  size_t slashCount = 0;
  size_t index = strlen(line) - 1;
  while((index >= 0) && (line[index--] == '\\'))
  {
    slashCount++;
  }
  return (slashCount % 2 == 1);
}

int LineFile::chopCRLF(char* line)
{
  if(line == 0)
  {
    return 0;
  }

  size_t size = strlen(line);
  if(size <= 0)
  {
    return 0;
  }

  int count = 0;

  while( --size >= 0)
  {
    if(line[size] == '\r' || line[size] == '\n')
    {
      line[size]='\0';
      ++count;

      continue;
    }

    break;
  }

  return count;
}

string LineFile::loadConvert(const string& theString)
{
  StringBuffer __sb;
  size_t __val_size = theString.size();
  const char* __val_P = theString.c_str();

  for(size_t i = 0;i < __val_size;++i)
  {
    char aChar = __val_P[i];
    if(aChar == '\\')
    {
      if( (i+1) < __val_size)
      {
        char bChar = __val_P[i+1];

        if (bChar == 't')
        {
          aChar = '\t';
          ++i;
        }
        else if (bChar == 'r')
        {
          aChar = '\r';
          ++i;
        }
        else if (bChar == 'n')
        {
          aChar = '\n';
          ++i;
        }
        else if (bChar == 'f')
        {
          aChar = '\f';
          ++i;
        }
        else if (bChar == '\\')
        {
          aChar = '\\';
          ++i;
        }
        else
        {
          aChar = bChar;
        }

      }//~end if( (i+1) < __val_size)

    }//~end if(aChar == '\\')

    __sb << aChar;
    
  }//~end for(int i=0;i<length;++i)

  return __sb.str();
}

string  LineFile::saveConvert(const string& theString, bool escapeSpace)
{
  return theString;
}
      
__OPENCODE_END_NAMESPACE
