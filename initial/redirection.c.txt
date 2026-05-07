#include <gtk/gtk.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

void log_command(const char *cmd) {
    int fd = open("logs/history.log", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd != -1) {
        write(fd, cmd, strlen(cmd));
        write(fd, "\n", 1);
        close(fd);
    }
}

void execute_piped(char *cmd1, char *cmd2, GtkWidget *output_widget) {
    int p_fds[2];
    if (pipe(p_fds) == -1) return;

    if (fork() == 0) {
        dup2(p_fds[1], STDOUT_FILENO);
        close(p_fds[0]);
        char *args[] = {strtok(cmd1, " "), NULL};
        execvp(args[0], args);
        exit(1);
    }
    
    if (fork() == 0) {
        dup2(p_fds[0], STDIN_FILENO);
        close(p_fds[1]);
        char *args[] = {strtok(cmd2, " "), NULL};
        execvp(args[0], args);
        exit(1);
    }

    close(p_fds[0]); 
    close(p_fds[1]);
    wait(NULL); 
    wait(NULL);
    
    GtkTextBuffer *tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_widget));
    gtk_text_buffer_set_text(tb, "Piped command executed successfully.", -1);
}