#include <stdio.h>
#include <gtk/gtk.h>

#include "ddcfg.h"
#define __DDCFG_EXPORT__ extern
#include "spec.h"


struct st_spec * spec;

GtkWidget * render_property(struct st_spec_property * prop)
{
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *name, *description;
	GtkWidget * control;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	name = gtk_label_new("");
	char * markup = g_markup_printf_escaped("<span size=\"x-large\">\%s</span>", prop->name);
	gtk_label_set_markup(GTK_LABEL(name), markup);
	g_free(markup);
	description = gtk_label_new(prop->description);
	gtk_label_set_line_wrap(GTK_LABEL(description), TRUE);
	g_object_set(description, "xalign", 0, 0, NULL);
	gtk_box_pack_start(GTK_BOX(hbox), name, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), NULL, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), description, FALSE, FALSE, 0);

	if (prop->type == BOOL) {
		control = gtk_switch_new();
	} else if (prop->values != NULL) {
		int number_values, i;
		char ** possible_values = ddcfg_parselist(prop->values, &number_values);
		control = gtk_combo_box_text_new();
		for (i = 0; i < number_values; i++) {
			gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(control), NULL, possible_values[i]);
		}
		gtk_combo_box_set_active(GTK_COMBO_BOX(control), 0);
	} else if (prop->type == PATH) {
		control = gtk_file_chooser_button_new("Choose file", GTK_FILE_CHOOSER_ACTION_OPEN);
	} else {
		control = gtk_entry_new();
		gtk_entry_set_width_chars(GTK_ENTRY(control), 8);
	}
	gtk_box_pack_end(GTK_BOX(hbox), control, FALSE, FALSE, 0);

	return vbox;
};

GtkWidget * render_section(struct st_spec_section * section)
{
	struct st_spec_property * prop;
	GtkWidget * description;
	GtkWidget * list;

	list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_set_border_width(GTK_CONTAINER(list), 10);
	description = gtk_label_new(section->description);
	g_object_set(description, "xalign", 0, 0, NULL);

	gtk_widget_set_hexpand(description, TRUE);
	gtk_widget_set_halign(description, GTK_ALIGN_START);
	gtk_label_set_line_wrap(GTK_LABEL(description), TRUE);

	gtk_box_pack_start(GTK_BOX(list), description, FALSE, FALSE, 0);

	GtkWidget * sep;
	sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(list), sep, FALSE, FALSE, 0);


	for (prop = section->properties; prop != NULL; prop = prop->next) {
		GtkWidget * content = render_property(prop);
		gtk_box_pack_start(GTK_BOX(list), content, FALSE, FALSE, 0);
	};

	GtkWidget * scroll;
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
	gtk_container_add(GTK_CONTAINER(scroll), list);

	return scroll;
};

static void activate (GtkApplication* app, gpointer user_data)
{
	GtkWidget *window;
	GtkWidget *header;
	GtkWidget *explorer;
	GtkWidget *container;
	
	container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	header = gtk_header_bar_new();
	gtk_header_bar_set_title(GTK_HEADER_BAR(header), (char *)user_data);
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);

	GtkWidget * savebtn;
	savebtn = gtk_button_new();
	GtkWidget * image = gtk_image_new_from_icon_name("media-floppy", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(savebtn), image);

	gtk_header_bar_pack_start(GTK_HEADER_BAR(header), savebtn);

	explorer = gtk_notebook_new();
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(explorer), TRUE);

	struct st_spec_section *section;

	for (section = spec->sections; section != NULL; section = section->next) {
		GtkWidget * label = gtk_label_new(section->name);
		GtkWidget * content = render_section(section);
		gtk_notebook_append_page(GTK_NOTEBOOK(explorer), content, label);
	}

	gtk_box_pack_start(GTK_BOX(container), header, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(container), explorer, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(explorer), 10);

	window = gtk_application_window_new(app);
	gtk_window_set_wmclass(GTK_WINDOW(window), "gtkconfig", "gtkconfig");

	gtk_container_add(GTK_CONTAINER(window), container);

	gtk_window_set_title(GTK_WINDOW(window), "ddcfg GTK Wizard");
	gtk_window_set_default_size(GTK_WINDOW(window), 500, 600);
	gtk_widget_show_all(window);
}

int main(int argc, char *argv[])
{
	int arg, status;

	if (argc < 1) {
		fprintf(stderr, "Please provide a spec file as first argument\n");
		return 1;
	}

	spec = new_spec_from_file(argv[1]);
	status = parse_spec(spec);

	if (status) {
		fprintf(stderr, "Problem reading specfile! Aborting\n");
		return 1;
	}

	for (arg = 2; arg < argc; arg++) {
		fprintf(stdout, "Reading file %s as configuration file\n", argv[arg]);
		status = ddcfg_parse(argv[arg]);
		if (status) {
			fprintf(stderr, "Error reading configuration file! Aborting\n");
			return 1;
		}
	}

	GtkApplication *app;
	app = gtk_application_new(NULL, G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), argv[1]);
	status = g_application_run(G_APPLICATION(app), 0, NULL);
	g_object_unref(app);

	return status;
}
