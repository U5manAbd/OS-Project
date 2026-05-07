#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

double execute_simple(char *cmd, GtkWidget *output_widget);
void log_command(const char *cmd);
void execute_piped(char *cmd1, char *cmd2, GtkWidget *output_widget);

GtkWidget *output_view;
GtkWidget *m_label;

static void on_execute(GtkWidget *btn, gpointer data) {
    char cmd[1024];
    strncpy(cmd, gtk_entry_get_text(GTK_ENTRY(data)), 1024);
    
    if (strlen(cmd) == 0) return;

    log_command(cmd); 

    if (strncmp(cmd, "cd ", 3) == 0) {
        chdir(cmd + 3);
        gtk_label_set_text(GTK_LABEL(m_label), "Directory Changed.");
        return;
    }

    char *p = strchr(cmd, '|'); 
    if (p) {
        *p = '\0';
        execute_piped(cmd, p + 1, output_view); 
        gtk_label_set_text(GTK_LABEL(m_label), "Piped command finished.");
    } else {
        double time = execute_simple(cmd, output_view); 
        char buf[64]; 
        sprintf(buf, "Execution Time: %.6f s", time);
        gtk_label_set_text(GTK_LABEL(m_label), buf);
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    system("mkdir -p logs"); // Ensure logs directory exists

    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "OS Shell - Team Collaboration");
    gtk_window_set_default_size(GTK_WINDOW(win), 600, 450);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(win), 10);
    gtk_container_add(GTK_CONTAINER(win), vbox);

    GtkWidget *ent = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ent), "Enter command...");
    gtk_box_pack_start(GTK_BOX(vbox), ent, FALSE, FALSE, 0);

    GtkWidget *btn = gtk_button_new_with_label("Run Command");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_execute), ent);
    gtk_box_pack_start(GTK_BOX(vbox), btn, FALSE, FALSE, 0);

    output_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output_view), FALSE);
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), output_view);
    gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);

    m_label = gtk_label_new("Metrics: Ready");
    gtk_box_pack_start(GTK_BOX(vbox), m_label, FALSE, FALSE, 5);

    gtk_widget_show_all(win);
    gtk_main();
    return 0;
}