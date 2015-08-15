#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ddcfg.h"
#define __DDCFG_EXPORT__ extern
#include "hash.h"


#include "spec.h"

struct st_spec * spec;

void output_configuration(GtkWidget *widget, gpointer *data)
{
	ddcfg_dump(NULL, stdout);
};

static int parse_double(const char *string, double *value)
{
	char *ptr;
	ptr = (char *) string;
	*value = strtod(string, &ptr);
	if (ptr == string)
		return 1;
	return 0;
};

static int parse_int(const char *string, int *value)
{
	char *ptr;
	ptr = (char *) string;
	*value = (int) strtol(string, &ptr, 10);
	if (ptr == string || *ptr != '\0')
		return 1;
	return 0;
};

void update_dependencies(struct st_spec_property * prop, int state)
{
	struct st_spec_section * section = prop->section->spec->sections;

	char * newstr = malloc(strlen(prop->section->name) + strlen(prop->name) + 2);
	sprintf(newstr, "%s.%s", prop->section->name, prop->name);

	while (section) {
		prop = section->properties;
		while (prop) {
			if (prop->depends_on && strcmp(prop->depends_on, newstr) == 0) {
				if (prop->widget.widget) {
					gtk_widget_set_sensitive(GTK_WIDGET(prop->widget.widget), state);
					gtk_widget_set_has_tooltip(GTK_WIDGET(prop->widget.widget), !state);
				}
			}
			prop = prop->next;
		}
		section = section->next;
	}
	free(newstr);
};

void update_boolean(GtkWidget *widget, GParamSpec *pspec, gpointer *data)
{
	struct st_spec_property * prop = (struct st_spec_property *) data;
	char * option = prop->name;
	char * section = prop->section->name;
	if (gtk_switch_get_active(GTK_SWITCH(widget)) == TRUE) {
		char * newstr = malloc(strlen(section) + strlen(option) + 2);
		sprintf(newstr, "%s.%s", section, option);
		install(newstr, "true");
		free(newstr);
		update_dependencies(prop, TRUE);
	} else {
		char * newstr = malloc(strlen(section) + strlen(option) + 2);
		sprintf(newstr, "%s.%s", section, option);
		install(newstr, "false");
		free(newstr);
		update_dependencies(prop, FALSE);
	}
};

void update_string(GtkWidget *widget, gpointer *data)
{
	struct st_spec_property * prop = (struct st_spec_property *) data;
	char * option = prop->name;
	char * section = prop->section->name;
	char * newstr = malloc(strlen(section) + strlen(option) + 2);
	sprintf(newstr, "%s.%s", section, option);
	install(newstr, gtk_entry_get_text(GTK_ENTRY(widget)));
	free(newstr);
	
	if (prop->type == INT) {
		int tmp, err;
		err = parse_int(gtk_entry_get_text(GTK_ENTRY(widget)), &tmp);
		if (err) {
			gtk_widget_show(prop->widget.alert);
			gtk_widget_set_tooltip_text(prop->widget.alert, "This property should be an integer");
		} else {
			gtk_widget_hide(prop->widget.alert);
		}
	} else if (prop->type == DOUBLE) {
		double tmp;
		int err;
		err = parse_double(gtk_entry_get_text(GTK_ENTRY(widget)), &tmp);
		if (err) {
			gtk_widget_show(prop->widget.alert);
			gtk_widget_set_tooltip_text(prop->widget.alert, "This property should be a float");
		} else {
			gtk_widget_hide(prop->widget.alert);
		}
	} else {
		int err = strlen(gtk_entry_get_text(GTK_ENTRY(widget)));
		if (err == 0) {
			gtk_widget_show(prop->widget.alert);
			gtk_widget_set_tooltip_text(prop->widget.alert, "This property should not be empty");
		} else {
			gtk_widget_hide(prop->widget.alert);
		}
	}
}

void update_path(GtkWidget *widget, gpointer *data)
{
	struct st_spec_property * prop = (struct st_spec_property *) data;
	char * option = prop->name;
	char * section = prop->section->name;
	char * newstr = malloc(strlen(section) + strlen(option) + 2);
	sprintf(newstr, "%s.%s", section, option);
	install(newstr, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget)));
	free(newstr);
}
void update_combo(GtkWidget *widget, gpointer *data)
{
	struct st_spec_property * prop = (struct st_spec_property *) data;
	char * option = prop->name;
	char * section = prop->section->name;
	char * newstr = malloc(strlen(section) + strlen(option) + 2);
	sprintf(newstr, "%s.%s", section, option);
	install(newstr, gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget)));
	free(newstr);
}

GtkWidget * render_property(struct st_spec_property * prop)
{
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *name, *description;
	GtkWidget * control;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	name = gtk_label_new("");
	char * markup = g_markup_printf_escaped("<span size=\"x-large\">\%s</span>", prop->name);
	gtk_label_set_markup(GTK_LABEL(name), markup);
	g_free(markup);
	description = gtk_label_new(prop->description);
	gtk_label_set_line_wrap(GTK_LABEL(description), TRUE);
	g_object_set(description, "xalign", 0, 0, NULL);

	GtkWidget * image = gtk_image_new_from_icon_name("dialog-warning", GTK_ICON_SIZE_MENU);
	gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), name, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), NULL, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), description, FALSE, FALSE, 0);

	if (prop->type == BOOL) {
		control = gtk_switch_new();
		g_signal_connect(control, "notify::active", G_CALLBACK(update_boolean), prop);
		if (ddcfg_is_defined(prop->section->name, prop->name)) {
			if (ddcfg_bool(prop->section->name, prop->name)) {
				gtk_switch_set_active(GTK_SWITCH(control), TRUE);
			}
		}
	} else if (prop->values != NULL) {
		int number_values, i;
		char ** possible_values = ddcfg_parselist(prop->values, &number_values);
		char * defined;
		control = gtk_combo_box_text_new();
		for (i = 0; i < number_values; i++) {
			gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(control), NULL, possible_values[i]);
		}
		if (ddcfg_is_defined(prop->section->name, prop->name)) {
			defined = ddcfg_get(prop->section->name, prop->name);
			for (i = 0; i < number_values; i++) {
				if (strcmp(possible_values[i], defined) == 0)
					break;
			}
			if (i != number_values)
				gtk_combo_box_set_active(GTK_COMBO_BOX(control), i);
		} else {
			gtk_combo_box_set_active(GTK_COMBO_BOX(control), 0);
		}
		g_signal_connect(control, "changed", G_CALLBACK(update_combo), prop);
	} else if (prop->type == PATH) {
		control = gtk_file_chooser_button_new("Choose file", GTK_FILE_CHOOSER_ACTION_OPEN);
		char * defined;
		if (ddcfg_is_defined(prop->section->name, prop->name)) {
			defined = ddcfg_get(prop->section->name, prop->name);
			gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(control), defined);
		}
		g_signal_connect(control, "file-set", G_CALLBACK(update_path), prop);
	} else {
		control = gtk_entry_new();
		g_signal_connect(control, "changed", G_CALLBACK(update_string), prop);
		gtk_entry_set_width_chars(GTK_ENTRY(control), 16);
		gtk_entry_set_alignment(GTK_ENTRY(control), 1);
		if (ddcfg_is_defined(prop->section->name, prop->name)) {
			gtk_entry_set_text(GTK_ENTRY(control), ddcfg_get(prop->section->name, prop->name));
		}
	}

	gtk_box_pack_end(GTK_BOX(hbox), control, FALSE, FALSE, 0);

	if (prop->depends_on) {
		char * key = prop->depends_on;
		if (ddcfg_bool(NULL, key) == 0) {
			char tooltip[256];
			sprintf(tooltip, "Activate <b>%s</b> to use this property", key);
			gtk_widget_set_sensitive(vbox, FALSE);
			gtk_widget_set_tooltip_markup(vbox, tooltip);
		}
	}

	struct st_gtk_property pw = {
		.widget = vbox,
		.control = control,
		.alert = image
	};
	prop->widget = pw;

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
	g_signal_connect(savebtn, "clicked", G_CALLBACK(output_configuration), NULL);

	gtk_header_bar_pack_start(GTK_HEADER_BAR(header), savebtn);

	explorer = gtk_notebook_new();
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(explorer), TRUE);

	struct st_spec_section *section;

	for (section = spec->sections; section != NULL; section = section->next) {
		GtkWidget * warning = gtk_image_new_from_icon_name("dialog-warning", GTK_ICON_SIZE_MENU);
		GtkWidget * labeltext = gtk_label_new(section->name);
		GtkWidget * label;
		label = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
		gtk_widget_set_tooltip_text(warning, "There are errors in this section");
		gtk_box_pack_start(GTK_BOX(label), warning, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(label), labeltext, FALSE, FALSE, 0);
		gtk_widget_show_all(label);
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
	gtk_window_set_default_size(GTK_WINDOW(window), 600, 800);
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
