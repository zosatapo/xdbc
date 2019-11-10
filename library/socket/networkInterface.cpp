#include "socket/networkInterface.h"
#include "socket/networkUtils.h"

__OPENCODE_BEGIN_NAMESPACE

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
NetworkInterface::NetworkInterface() 
{
}

NetworkInterface::NetworkInterface(const string& nameValue, int indexValue, const vector<unsigned char>& ifaddrValue,const vector<InetAddress>& addrsValue) 
{
  this->name = nameValue;
  this->index = indexValue;
  this->ifaddr = ifaddrValue;

  this->addrs = addrsValue;
}

NetworkInterface::NetworkInterface(const string& nameValue, int indexValue, const vector<unsigned char>& ifaddrValue,const vector<InetAddress>& addrsValue,const string& displayNameValue) 
{
  this->name = nameValue;
  this->index = indexValue;
  this->ifaddr = ifaddrValue;
  this->displayName = displayNameValue;

  this->addrs = addrsValue;
}

NetworkInterface::NetworkInterface(const NetworkInterface& rv)
{
  this->name = rv.name;
  this->index = rv.index;
  this->ifaddr = rv.ifaddr;
  this->displayName = rv.displayName;

  this->addrs = rv.addrs;
}

NetworkInterface& NetworkInterface::operator=(const NetworkInterface& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  this->name = rv.name;
  this->index = rv.index;
  this->ifaddr = rv.ifaddr;
  this->displayName = rv.displayName;

  this->addrs = rv.addrs;

  return *this;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

const string& NetworkInterface::getName() const 
{
  return name;
}

const vector<unsigned char>&  NetworkInterface::getInterfaceAddress() const
{
  return ifaddr;
}

const vector<InetAddress>& NetworkInterface::getInetAddresses() const
{
  return addrs;
}

int NetworkInterface::getIndex() const
{
  return index;
}

const string& NetworkInterface::getDisplayName() const 
{
  return displayName;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------ 
NetworkInterface NetworkInterface::getByName(__NET_HOOK_DECLARE_MORE const string& name)  __NET_THROW_DECLARE(SocketException) 
{
  return NetworkUtils::netifGetByName(__NET_HOOK_INVOKE_MORE name);
}

NetworkInterface NetworkInterface::getByIndex(__NET_HOOK_DECLARE_MORE int index)  __NET_THROW_DECLARE(SocketException)  
{
  return NetworkUtils::netifGetByIndex(__NET_HOOK_INVOKE_MORE index);
}


NetworkInterface NetworkInterface::getByInetAddress(__NET_HOOK_DECLARE_MORE const InetAddress& addr)  __NET_THROW_DECLARE(SocketException) 
{
  return NetworkUtils::netifGetByInetAddress(__NET_HOOK_INVOKE_MORE addr);
}
  
void NetworkInterface::getNetworkInterfaces(__NET_HOOK_DECLARE_MORE vector<NetworkInterface>& ifs)  __NET_THROW_DECLARE(SocketException) 
{
  NetworkUtils::netifGetAll(__NET_HOOK_INVOKE_MORE ifs);
}

__OPENCODE_END_NAMESPACE
