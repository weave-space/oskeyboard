#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <gtk4-layer-shell/gtk4-layer-shell.h>
#include "keymap.h"

GtkWidget *window = NULL;
struct libevdev *dev;
struct libevdev_uinput *uidev;
bool shift_active = false;
GList *key_buttons = NULL;

static void apply_css() {
  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_resource(provider, "/org/spaces/oskeyboard/style.css");
  gtk_style_context_add_provider_for_display(
    gdk_display_get_default(),
    GTK_STYLE_PROVIDER(provider),
    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
  );
  g_object_unref(provider);
}

void press_key(int key_code) {
  if (!uidev) return;
  libevdev_uinput_write_event(uidev, EV_KEY, key_code, 1);
  libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
  libevdev_uinput_write_event(uidev, EV_KEY, key_code, 0);
  libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
}

static void update_labels() {
  for (GList *l = key_buttons; l != NULL; l = l->next) {
    GtkButton *btn = GTK_BUTTON(l->data);
    const char *current = gtk_button_get_label(btn);
    
    for (int i = 0; i < KEY_MAP_SIZE; i++) {
      if (shift_active && g_strcmp0(current, key_map[i].label) == 0) {
        gtk_button_set_label(btn, key_map[i].shift_label);
        break;
      } else if (!shift_active && g_strcmp0(current, key_map[i].shift_label) == 0) {
        gtk_button_set_label(btn, key_map[i].label);
        break;
      }
    }
  }
}

static void on_key_clicked(GtkButton *button, gpointer user_data) {
  const char *label = gtk_button_get_label(button);

  if (g_strcmp0(label, "Shift") == 0) {
    shift_active = !shift_active;
    update_labels();
    return;
  }

  for (int i = 0; i < KEY_MAP_SIZE; i++) {
    if (g_strcmp0(label, key_map[i].label) == 0 || g_strcmp0(label, key_map[i].shift_label) == 0) {
      if (shift_active) {
        libevdev_uinput_write_event(uidev, EV_KEY, KEY_LEFTSHIFT, 1);
        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
      }

      press_key(key_map[i].key_code);

      if (shift_active) {
        libevdev_uinput_write_event(uidev, EV_KEY, KEY_LEFTSHIFT, 0);
        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        shift_active = false; 
        update_labels(); 
      }
      return;
    }
  }
}

void add_key(GtkGrid *grid, const char *label, int col, int width) {
  GtkWidget *button = gtk_button_new_with_label(label);
  gtk_widget_set_hexpand(button, TRUE);
  gtk_widget_set_halign(button, GTK_ALIGN_FILL);
  gtk_widget_set_size_request(button, 94 * width, 94);

  g_signal_connect(button, "clicked", G_CALLBACK(on_key_clicked), NULL);
  gtk_grid_attach(grid, button, col, 0, width, 1);
  key_buttons = g_list_append(key_buttons, button);
}

void init_evdev_keyboard() {
  dev = libevdev_new();
  libevdev_set_name(dev, "OSKeyboard");
  libevdev_enable_event_type(dev, EV_KEY);
  
  for (int i = 0; i < KEY_MAP_SIZE; i++) {
    libevdev_enable_event_code(dev, EV_KEY, key_map[i].key_code, NULL);
  }

  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (fd < 0) {
    fprintf(stderr, "Failed to open /dev/uinput: %s\n", strerror(errno));
    return; 
  }

  int err = libevdev_uinput_create_from_device(dev, fd, &uidev);
  if (err != 0) {
    fprintf(stderr, "Failed to create uinput device: %s\n", strerror(-err));
    close(fd);
    return;
  }
}

static void activate(GtkApplication *app, gpointer user_data) {
  if (window) {
    if (gtk_widget_get_visible(window)) {
      gtk_widget_set_visible(window, FALSE);
    } else {
      gtk_window_present(GTK_WINDOW(window));
    }
    return;
  }

  apply_css();

  GtkBuilder *builder = gtk_builder_new_from_resource("/org/spaces/oskeyboard/keyboard.ui");
  window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
  
  gtk_layer_init_for_window(GTK_WINDOW(window));
  gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_BOTTOM);
  gtk_layer_auto_exclusive_zone_enable(GTK_WINDOW(window));
  gtk_layer_set_keyboard_mode(GTK_WINDOW(window), FALSE);
  gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
  gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
  gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);
  
  gtk_window_set_application(GTK_WINDOW(window), app);

  GtkGrid *rows[] = {
    GTK_GRID(gtk_builder_get_object(builder, "kbd_row_0")),
    GTK_GRID(gtk_builder_get_object(builder, "kbd_row_1")),
    GTK_GRID(gtk_builder_get_object(builder, "kbd_row_2")),
    GTK_GRID(gtk_builder_get_object(builder, "kbd_row_3")),
    GTK_GRID(gtk_builder_get_object(builder, "kbd_row_4"))
  };

  int current_key = 0;
  int row_counts[] = {14, 14, 13, 12, 6};

  for (int r = 0; r < 5; r++) {
    int current_col = 0;
    for (int c = 0; c < row_counts[r]; c++) {
      if (current_key >= KEY_MAP_SIZE) break;

      const char *label = key_map[current_key].label;
      int width = 1;

      if (g_strcmp0(label, "Space") == 0) width = 7;
      else if (g_strcmp0(label, "Backspace") == 0) width = 2;
      else if (g_strcmp0(label, "Enter") == 0) width = 3;
      else if (g_strcmp0(label, "Shift") == 0) width = 3;
      else if (g_strcmp0(label, "Alt") == 0) width = 2;
      else if (g_strcmp0(label, "Ctrl") == 0) width = 3;
      else if (g_strcmp0(label, "Caps Lock") == 0) width = 2;

      add_key(rows[r], label, current_col, width);
      current_col += width;
      current_key++;
    }
  }

  init_evdev_keyboard();
  gtk_window_present(GTK_WINDOW(window));
  g_object_unref(builder);
}

int main(int argc, char **argv) {
  GtkApplication *app = gtk_application_new("org.spaces.oskeyboard", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
