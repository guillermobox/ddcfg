#!/usr/bin/python
#
# Application to edit or create a configuration file using a given spec
# file, and a user friendly gtk interface
#
# Test using the test/easy.inispec first, by doing:
#     $ python gtkconfig.py test/easy.inispec
#
from gi.repository import Gtk

win = Gtk.Window()
win.connect("delete-event", Gtk.main_quit)
win.set_wmclass("gtkconfig", "gtkconfig")
win.set_title('gtkconfig')
win.show_all()
Gtk.main()
