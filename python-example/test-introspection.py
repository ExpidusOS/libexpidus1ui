#!/usr/bin/env python3

# You can generate python documentation with the following commands:
# g-ir-doc-tool --language Python -o ./html ../libexpidus1ui/libexpidus1ui-2.0.gir
# yelp ./html/

import gi.repository
# Set the search path to use the newly generated introspection files
gi.require_version('GIRepository', '2.0')
from gi.repository import GIRepository
GIRepository.Repository.prepend_search_path('../libexpidus1ui/')
# randomly search for libexpidus1util's gir and typelib
GIRepository.Repository.prepend_search_path('/usr/local/share/gir-1.0/')
GIRepository.Repository.prepend_search_path('/usr/local/lib/girepository-1.0/')
# Now import 4ui
gi.require_version('Libexpidus1ui', '2.0')
from gi.repository import Libexpidus1ui
# and Gtk3
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk

# see if it works
titled_dialog = Libexpidus1ui.TitledDialog(subtitle="    Hello Expidus Python Developer!    ")
titled_dialog.connect("delete-event", Gtk.main_quit)
titled_dialog.show()
Gtk.main()
