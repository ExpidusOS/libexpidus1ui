AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-DG_LOG_DOMAIN=\"libexpidus1ui-glade\" \
	$(PLATFORM_CPPFLAGS)

SUBDIRS = \
	icons

if HAVE_GLADEUI2
##
## Glade module
##
glademoduledir = \
	$(GLADEUI2_MODULEDIR)

glademodule_LTLIBRARIES = \
	libexpidus1uiglade2.la

libexpidus1uiglade2_la_SOURCES  = \
	libexpidus1ui-glade.c

libexpidus1uiglade2_la_CFLAGS = \
	$(GLADEUI2_CFLAGS) \
	$(LIBEXPIDUS1UTIL_CFLAGS) \
	$(PLATFORM_CFLAGS)

libexpidus1uiglade2_la_LDFLAGS = \
	-no-undefined \
	-avoid-version \
	-module \
	$(PLATFORM_LDFLAGS)

libexpidus1uiglade2_la_LIBADD = \
	$(top_builddir)/libexpidus1ui/libexpidus1ui-2.la \
	$(LIBEXPIDUS1UTIL_LIBS) \
	$(GLADEUI2_LIBS)

##
## Glade catalog xml file
##
gladecatalogdir = \
	$(GLADEUI2_CATALOGDIR)
gladecatalog_DATA = \
	libexpidus1ui-2.xml
@INTLTOOL_XML_NOMERGE_RULE@
endif

CLEANFILES = \
	libexpidus1ui-2.xml

EXTRA_DIST = \
	libexpidus1ui-2.xml.in

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
