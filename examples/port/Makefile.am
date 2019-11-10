DEFAULT_INCLUDES          = @opencode_DEFAULT_INCLUDES@
INCLUDES                  = -I$(top_srcdir)/library
AM_LDFLAGS                = -L$(top_srcdir)/library/port -lwiport  @opencode_LDFLAGS@
AM_CXXFLAGS               = @opencode_DEFS@ @opencode_CXXFLAGS@
                          
noinst_PROGRAMS           = asn1parser_demo  calendar_demo   codec_demo     date_demo       file_demo          math_demo  \
                            number_demo      utilities_demo  unicode_demo   process_demo    profile_demo       properties_demo \
                            random_demo      syslib_demo     system_demo    tokenizer_demo  featuresDemo

asn1parser_demo_SOURCES   = asn1parser_demo.cpp
calendar_demo_SOURCES     = calendar_demo.cpp
codec_demo_SOURCES        = codec_demo.cpp
date_demo_SOURCES         = date_demo.cpp
file_demo_SOURCES         = file_demo.cpp
math_demo_SOURCES         = math_demo.cpp
number_demo_SOURCES       = number_demo.cpp
unicode_demo_SOURCES      = unicode_demo.cpp
utilities_demo_SOURCES    = utilities_demo.cpp
process_demo_SOURCES      = process_demo.cpp
profile_demo_SOURCES      = profile_demo.cpp
properties_demo_SOURCES   = properties_demo.cpp
random_demo_SOURCES       = random_demo.cpp
syslib_demo_SOURCES       = syslib_demo.cpp
system_demo_SOURCES       = system_demo.cpp
tokenizer_demo_SOURCES    = tokenizer_demo.cpp
featuresDemo_SOURCES      = featuresDemo.cpp

EXTRA_DIST                = schema.db schema.ini schema.properties


