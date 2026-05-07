#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>

GtkWidget *output_display;
GtkWidget *metrics_label;

void log_command(const char *cmd) {
    int fd = open("logs/history.log", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd != -1) {
        write(fd, cmd, strlen(cmd));
        write(fd, "\n", 1);
        close(fd);
    }
}

void tokenize(char *cmd, char **args) {
    int i = 0;
    char *token = strtok(cmd, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

static void run_command(GtkWidget *widget, gpointer data) {
    GtkWidget *entry = (GtkWidget *)data;
    const char *input = gtk_entry_get_text(GTK_ENTRY(entry));
    char cmd_buffer[1024];
    strncpy(cmd_buffer, input, 1024);

    if (strlen(cmd_buffer) == 0) return;
    log_command(cmd_buffer);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    if (strncmp(cmd_buffer, "cd ", 3) == 0) {
        chdir(cmd_buffer + 3);
        gtk_label_set_text(GTK_LABEL(metrics_label), "Directory changed.");
        return;
    }

    int main_pipe[2];
    pipe(main_pipe);

    pid_t pid = fork();
    if (pid == 0) {
        close(main_pipe[0]);
        dup2(main_pipe[1], STDOUT_FILENO);
        dup2(main_pipe[1], STDERR_FILENO);

        char *pipe_ptr = strchr(cmd_buffer, '|');
        if (pipe_ptr) {
            *pipe_ptr = '\0';
            char *cmd1 = cmd_buffer;
            char *cmd2 = pipe_ptr + 1;

            int internal_pipe[2];
            pipe(internal_pipe);

            if (fork() == 0) {
                dup2(internal_pipe[1], STDOUT_FILENO);
                close(internal_pipe[0]);
                char *args1[64]; tokenize(cmd1, args1);
                execvp(args1[0], args1);
                exit(1);
            } else {
                dup2(internal_pipe[0], STDIN_FILENO);
                close(internal_pipe[1]);
                char *args2[64]; tokenize(cmd2, args2);
                execvp(args2[0], args2);
                exit(1);
            }
        } 
        char *redir_ptr = strchr(cmd_buffer, '>');
        if (redir_ptr) {
            *redir_ptr = '\0';
            char *filename = strtok(redir_ptr + 1, " ");
            int file_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(file_fd, STDOUT_FILENO);
            close(file_fd);
        }

        char *args[64]; tokenize(cmd_buffer, args);
        execvp(args[0], args);
        exit(1);
    } else {
        close(main_pipe[1]);
        char buffer[8192];
        memset(buffer, 0, sizeof(buffer));
        read(main_pipe[0], buffer, sizeof(buffer) - 1);

        GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_display));
        gtk_text_buffer_set_text(text_buffer, buffer, -1);

        wait(NULL);
        gettimeofday(&end, NULL);
        double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) * 1e-6;
        char metric_text[100];
        sprintf(metric_text, "Command executed in: %.6f seconds", time_taken);
        gtk_label_set_text(GTK_LABEL(metrics_label), metric_text);
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    system("mkdir -p logs");

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "OS Final Project Shell");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 5);

    GtkWidget *button = gtk_button_new_with_label("Execute");
    g_signal_connect(button, "clicked", G_CALLBACK(run_command), entry);
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 5);

    output_display = gtk_text_view_new();
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled), output_display);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 5);

    metrics_label = gtk_label_new("Metrics: Ready");
    gtk_box_pack_start(GTK_BOX(vbox), metrics_label, FALSE, FALSE, 5);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}