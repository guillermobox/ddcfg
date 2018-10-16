import gi

def me(widget, value, *args, **kwargs):
    print('Now has value', value)

def editEntry(widget):
    print(widget.get_text())

gi.require_version('Gtk', '3.0')
from gi.repository import Gtk

properties = [
        (bool, 'Activate?'),
        (str, 'Effective mass'),
        (bool, 'Output everything?'),
]

builder = Gtk.Builder()
builder.add_from_file("main.glade")

main = builder.get_object('controlHolder')
pack = builder.get_object('packHere')

for (t,d) in properties:
    if t == bool:
        builder = Gtk.Builder()
        builder.add_from_file('option-bool.glade')
        builder.connect_signals({
            'onBoolActivated': me,
        })
    elif t == str:
        builder = Gtk.Builder()
        builder.add_from_file('option-string.glade')
        builder.connect_signals({
            'onEdit': editEntry,
        })

    widget = builder.get_object('widget')
    label = builder.get_object('label')
    label.set_label(d)
    pack.add(widget)

main.show_all()

Gtk.main()
