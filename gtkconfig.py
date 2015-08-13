#!/usr/bin/python
#
# Application to edit or create a configuration file using a given spec
# file, and a user friendly gtk interface
#
# Test using the test/easy.inispec first, by doing:
#     $ python gtkconfig.py test/easy.inispec
#

from gi.repository import Gtk, Gio

def parseline(line):
    key,_,value = line.partition(' ')
    return key, value

def parse_keys(lines):
    keys = {}
    for line in lines:
        key, value = parseline(line)
        if key in keys:
            keys[key] = keys[key] + ' ' + value
        else:
            keys[key] = value
    return keys

def clear_input(lines):
    output = []
    for line in lines:
        line = line.strip()
        if '#' in line:
            line = line[:line.index('#')]
        if line:
            output.append(line)
    return output

def group_by_properties(lines):
    section = []
    sections = []
    onsection = True

    for line in lines:
        if line.startswith('PROPERTY'):
            if section:
                sections.append(section)
            section = []
        section.append(line)

    sections.append(section)
    return sections

def group_by_sections(lines):
    section = []
    sections = []
    onsection = True

    for line in lines:
        if line.startswith('SECTION'):
            if section:
                sections.append(section)
            section = []
        section.append(line)

    sections.append(section)
    return sections

def parsespec(filename):
    fh = open(filename, 'r')
    lines = fh.readlines()
    fh.close()

    lines = clear_input(lines)
    sections = group_by_sections(lines)

    spec = []
    for section in sections:
        spec.append(SpecSection(section))
    return spec

class SpecProperty(object):
    def __init__(self, spec):
        keys = parse_keys(spec)
        self.name = keys['PROPERTY']
        self.description = keys['DESCRIPTION']
        if 'TYPE' in keys:
            self.type = keys['TYPE']
        else:
            self.type = None
        if 'VALUES' in keys:
            self.values = keys['VALUES']
        else:
            self.values = None

    def __str__(self):
        return 'PROPERTY [{0}] {1}'.format(self.name, self.type)

    def render(self):
        #row = Gtk.ListBoxRow()

        vbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=5)

        hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        #row.add(vbox)
        vbox.pack_start(hbox, True, True, 0)

        namelabel = Gtk.Label(self.name, xalign=0)
        namelabel.set_markup('<span size="x-large">{0}</span>'.format(self.name))
        hbox.pack_start(namelabel, True, True, 0)

        if self.values:
            model = Gtk.ListStore(str)
            values = map(lambda x: x.strip(), self.values.split(','))
            for val in values:
                model.append([val])
            combo = Gtk.ComboBox.new_with_model(model)
            text = Gtk.CellRendererText()
            combo.pack_start(text, True)
            combo.add_attribute(text, "text", 0)
            combo.set_entry_text_column(0)
            hbox.pack_start(combo, False, False, True)
        elif self.type == 'bool':
            switch = Gtk.Switch()
            switch.props.valign = Gtk.Align.CENTER
            hbox.pack_start(switch, False, True, 0)
        elif self.type == 'path':
            filechooserbutton = Gtk.FileChooserButton(title="Choose path for {0}".format(self.name))
            hbox.pack_start(filechooserbutton, False, True, 0)
        else:
            entry = Gtk.Entry()
            entry.set_width_chars(8)
            hbox.pack_start(entry, False, True, 0)
        desc = Gtk.Label(self.description, xalign=0)
        desc.set_line_wrap(True)
        vbox.pack_start(desc, False, False, 0)
        #return row
        return vbox

class SpecSection(object):
    def __init__(self, spec):
        self.properties = []
        self.description = ''
        self.name = None

        if spec:
            self.parse(spec)

    def __str__(self):
        s = 'SECTION [{0}]\n'.format(self.name)
        for p in self.properties:
            s += '\t' + str(p) + '\n'
        return s

    def parse(self, spec):
        while spec:
            line = spec.pop(0)
            key, value = parseline(line)
            if key == 'SECTION':
                self.name = value
            elif key == 'DESCRIPTION':
                self.description += ' ' + value
            if key == 'PROPERTY':
                spec.insert(0, line)
                break

        properties = group_by_properties(spec)
        for property in properties:
            self.properties.append(SpecProperty(property))

    def render(self):
        #list = Gtk.ListBox()
        #list.set_selection_mode(Gtk.SelectionMode.NONE)

        #row = Gtk.ListBoxRow()

        vbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)

        desc = Gtk.Label(self.description, xalign=0)
        desc.set_line_wrap(True)
        vbox.pack_start(desc, False, False, 0)

        #namelabel = Gtk.Label(self.name)
        #namelabel.set_markup('<span size="x-large" weight="bold">{0}</span>'.format(self.name))

        #vbox.pack_start(namelabel, False, True, 0)

        #row.add(namelabel)

        #list.add(row)

        for prop in self.properties:
            vbox.pack_start(prop.render(), False, False, 0)

        #return list
        vbox.set_border_width(10)
        scroll = Gtk.ScrolledWindow()
        scroll.set_policy(Gtk.PolicyType.NEVER, Gtk.PolicyType.AUTOMATIC)
        scroll.add_with_viewport(vbox)
        return scroll

class GtkConfig(Gtk.Window):
    def __init__(self, spec, specfile):
        Gtk.Window.__init__(self)
        self.set_border_width(10)
        self.set_default_size(500, 600)
        self.set_wmclass('gtkconfig', 'gtkconfig')
        self.spec = spec

        hb = Gtk.HeaderBar()
        hb.set_show_close_button(True)
        hb.props.title = specfile
        self.set_titlebar(hb)

        save = Gtk.Button()
        icon = Gio.ThemedIcon(name="media-floppy")
        image = Gtk.Image.new_from_gicon(icon, Gtk.IconSize.BUTTON)
        save.add(image)
        hb.pack_start(save)

        #stack = Gtk.Stack()
        notebook = Gtk.Notebook()

        #vbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        for spec in self.spec:
            section = spec.render()
            #stack.add_titled(section, spec.name, spec.name)
            #vbox.pack_start(section, False, False, 0)
            label = Gtk.Label(spec.name)
            notebook.append_page(section, label)
            notebook.set_scrollable(True)

        self.add(notebook)
        #switcher = Gtk.StackSwitcher()
        #switcher.set_stack(stack)

        #vbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=0)

        #vbox.pack_start(switcher, False, False, 0)
        #vbox.pack_start(stack, False, False, 0)
        #self.add(vbox)

import sys
spec = parsespec(sys.argv[1])

win = GtkConfig(spec, sys.argv[1])
win.connect("delete-event", Gtk.main_quit)
win.show_all()
Gtk.main()
