
AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-DG_LOG_DOMAIN=\"expidus1-about\" \
	-DDATADIR=\"$(datadir)/expidus1\" \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\" \
	$(PLATFORM_CPPFLAGS)

bin_PROGRAMS = \
	expidus1-about

expidus1_about_built_sources = \
	about-dialog-ui.h

expidus1_about_SOURCES = \
	$(expidus1_about_built_sources) \
	contributors.h \
	main.c

if ENABLE_GLIBTOP

expidus1_about_SOURCES += \
	system-info.c \
	system-info.h

endif

expidus1_about_CFLAGS = \
	$(LIBEXPIDUS1UTIL_CFLAGS) \
	$(GTK3_CFLAGS) \
	$(GLIBTOP_CFLAGS) \
	$(EPOXY_CFLAGS) \
	$(GUDEV_CFLAGS) \
	$(LIBX11_CFLAGS) \
	$(PLATFORM_CFLAGS)

expidus1_about_LDADD = \
	$(top_builddir)/libexpidus1ui/libexpidus1ui-2.la \
	$(LIBEXPIDUS1UTIL_LIBS) \
	$(GLIBTOP_LIBS) \
	$(EPOXY_LIBS) \
	$(GUDEV_LIBS) \
	$(LIBX11_LIBS) \
	$(LIBX11_LDFLAGS) \
	$(GTK3_LIBS)

expidus1_about_DEPENDENCIES = \
	$(top_builddir)/libexpidus1ui/libexpidus1ui-2.la

desktopdir = $(datadir)/applications
desktop_in_files = expidus1-about.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

DISTCLEANFILES = \
	$(desktop_DATA)

if MAINTAINER_MODE

about-dialog-ui.h: $(srcdir)/expidus1-about-dialog.glade Makefile
	$(AM_V_GEN) xdt-csource --static --strip-comments --strip-content --name=expidus_about_dialog_ui $< >$@

BUILT_SOURCES = \
	$(expidus1_about_built_sources)

DISTCLEANFILES += \
	$(expidus1_about_built_sources)

endif

EXTRA_DIST = \
	$(desktop_in_files) \
	expidus1-about-dialog.glade
