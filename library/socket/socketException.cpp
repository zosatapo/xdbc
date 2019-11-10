#include "socket/socketException.h"

__OPENCODE_BEGIN_NAMESPACE

SocketException::~SocketException()
{
}

SocketException::SocketException()
:BaseException()
{
}

SocketException::SocketException(const string& message)
:BaseException(message)
{
}

SocketException::SocketException(const string& message,XLONG errcode)
:BaseException(message,errcode)
{
}

SocketTimeoutException::~SocketTimeoutException()
{
}

SocketTimeoutException::SocketTimeoutException()
:SocketException()
{}

SocketTimeoutException::SocketTimeoutException(const string& reason)
:SocketException(reason)
{
}

UnknownHostException::~UnknownHostException()
{
}

UnknownHostException::UnknownHostException()
:SocketException()
{}

UnknownHostException::UnknownHostException(const string& reason)
:SocketException(reason)
{
}

ConnectionResetException::~ConnectionResetException()
{
}

ConnectionResetException::ConnectionResetException()
:SocketException()
{}

ConnectionResetException::ConnectionResetException(const string& reason)
:SocketException(reason)
{
}

ConnectionResetException::ConnectionResetException(const string& message,XLONG errcode)
:SocketException(message,errcode)
{
}

__OPENCODE_END_NAMESPACE
