#ifndef __OPENCODE_PORTABLE_STLMAP_H__
#define __OPENCODE_PORTABLE_STLMAP_H__

#include "port/ioc_G_config.h"

#include <map>
using OPENCODE_STL_NS::map;
using OPENCODE_STL_NS::pair;
   	
#ifndef OPENCODE_HAS_STL_HASHMAP
#  define OPENCODE_MAP  map
#else
#  include <ext/hash_map>
   using  OPENCODE_STL_HASHMAP_NS::hash_map;
#  define OPENCODE_MAP  hash_map

   namespace OPENCODE_STL_HASHMAP_NS 
   {
     template<> struct hash< std::string >
     {
       size_t operator()( const std::string& x ) const
       {
         return hash< const char* >()( x.c_str() );
       }
     };
   }

#endif

#endif


