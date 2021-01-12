# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:

if ENABLE_KEYBOARD_LIBRARY

AM_CPPFLAGS = \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	-DDATADIR=\"$(datadir)\" \
	-DG_LOG_DOMAIN=\"libexpidus1kbd-private\" \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\" \
	-DPREFIX=\"$(prefix)\" \
	$(PLATFORM_CPPFLAGS)

libexpidus1kbd_headers = \
	xfce-shortcuts-provider.h \
	xfce-shortcuts-grabber.h \
	xfce-shortcut-dialog.h \
	xfce-shortcuts.h \
	xfce-shortcuts-xfwm4.h

libexpidus1kbd_built_sources = \
	xfce-shortcuts-marshal.c \
	xfce-shortcuts-marshal.h

libexpidus1kbd_sources = \
	$(libexpidus1kbd_headers) \
	$(libexpidus1kbd_built_sources) \
	xfce-shortcuts-provider.c \
	xfce-shortcuts-grabber.c \
	xfce-shortcut-dialog.c \
	xfce-shortcuts.c \
	xfce-shortcuts-xfwm4.c


lib_LTLIBRARIES = libexpidus1kbd-private-3.la

libexpidus1kbd3_private_includedir = \
	$(includedir)/expidus1/libexpidus1kbd-private-3/libexpidus1kbd-private

libexpidus1kbd3_private_include_HEADERS = \
	$(libexpidus1kbd_headers)

libexpidus1kbd_private_3_la_SOURCES = \
	$(libexpidus1kbd_sources)

libexpidus1kbd_private_3_la_CFLAGS = \
	$(PLATFORM_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(GTK3_CFLAGS) \
	$(LIBX11_CFLAGS) \
	$(LIBEXPIDUS1UTIL_CFLAGS) \
	$(ESCONF_CFLAGS) \
	$(PLATFORM_CFLAGS)

libexpidus1kbd_private_3_la_LDFLAGS = \
	-export-dynamic \
	-version-info $(LIBEXPIDUS1KBD_PRIVATE_VERINFO) \
	-export-symbols-regex "^[^_].*" \
	-no-undefined \
	$(LIBX11_LDFLAGS) \
	$(PLATFORM_LDFLAGS)

libexpidus1kbd_private_3_la_LIBADD = \
	$(top_builddir)/libexpidus1ui/libexpidus1ui-2.la \
	$(GLIB_LIBS) \
	$(GTK3_LIBS) \
	$(LIBX11_LIBS) \
	$(LIBEXPIDUS1UTIL_LIBS) \
	$(ESCONF_LIBS)

settingsdir = $(sysconfdir)/xdg/expidus1/esconf/xfce-perchannel-xml
settings_DATA = expidus1-keyboard-shortcuts.xml

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libexpidus1kbd-private-3.pc


if MAINTAINER_MODE
xfce-shortcuts-marshal.h: xfce-shortcuts-marshal.list Makefile
	$(AM_V_GEN) glib-genmarshal --prefix=_xfce_shortcuts_marshal --internal --header $< > $@

xfce-shortcuts-marshal.c: xfce-shortcuts-marshal.list Makefile xfce-shortcuts-marshal.h
	$(AM_V_GEN) echo "#include \"xfce-shortcuts-marshal.h\"" > $@ \
	&& glib-genmarshal --prefix=_xfce_shortcuts_marshal --body $< >> $@

DISTCLEANFILES = \
	$(libexpidus1kbd_built_sources)

BUILT_SOURCES = \
	$(libexpidus1kbd_built_sources)
endif

EXTRA_DIST = \
	$(settings_DATA) \
	xfce-shortcuts-marshal.list

# required for gtk-doc
dist-hook: all

endif # ENABLE_KEYBOARD_LIBRARY
