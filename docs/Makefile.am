AUTOMAKE_OPTIONS = 1.11

# The name of the module.
DOC_MODULE=libexpidus1ui

# The top-level SGML file.
DOC_MAIN_SGML_FILE=$(DOC_MODULE)-docs.xml

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS=--deprecated-guards="XFCE_DISABLE_DEPRECATED"

# Extra options to pass to gtkdoc-scangobj
SCANGOBJ_OPTIONS=

# The directory containing the source code. Relative to $(srcdir)
DOC_SOURCE_DIR=../libexpidus1ui

# Extra options to supply to gtkdoc-mkdb
MKDB_OPTIONS=--sgml-mode --output-format=xml

# Extra options to supply to gtkdoc-fixref
FIXXREF_OPTIONS=

# Used for dependencies
HFILE_GLOB=$(top_srcdir)/libexpidus1ui/*.h
CFILE_GLOB=$(top_srcdir)/libexpidus1ui/*.c

# Header files to ignore when scanning
IGNORE_HFILES=								\
	libexpidus1ui.h							\
	libexpidus1ui-private.h						\
	xfce-heading.h							\
	libexpidus1ui-enum-types.h						\
	libexpidus1ui-alias.h						\
	stamp-libexpidus1ui-marshal.h					\
	libexpidus1ui-marshal.h						\
	libexpidus1ui-resources.h

# Extra files to add when scanning (relative to $srcdir)
EXTRA_HFILES=

# Images to copy into HTML directory
HTML_IMAGES =

# Extra SGML files that are included by DOC_MAIN_SGML_FILE
content_files = 							\
	version.xml

# CFLAGS and LDFLAGS for compiling scan program. Only needed
# if $(DOC_MODULE).types is non-empty.
AM_CPPFLAGS =	 							\
	-I$(top_srcdir) 						\
	-I$(top_builddir) 						\
	$(LIBEXPIDUS1UTIL_CFLAGS)						\
	$(GTK3_CFLAGS)	 						\
	$(GTK_DOC_EXTRA_CFLAGS)

GTKDOC_LIBS =  								\
	$(LIBEXPIDUS1UTIL_LIBS)						\
	$(GTK3_LIBS)							\
	$(top_builddir)/libexpidus1ui/libexpidus1ui-$(LIBEXPIDUS1UI_VERSION_API).la

include $(top_srcdir)/gtk-doc.make

# Other files to distribute
EXTRA_DIST +=								\
	version.xml.in

# required for gtk-doc
dist-hook: all

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
