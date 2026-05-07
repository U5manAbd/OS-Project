C-Shell: Custom GUI-Based Linux Shell

CS-2006 Operating Systems Project | Spring 2026

A lightweight, custom command-line interpreter built in C using the GTK 3.0 framework. This project demonstrates core Operating System concepts including process management, inter-process communication (IPC), and I/O redirection through system-level programming.

🚀 Features

GUI Wrapper: A modern GTK-based interface that captures terminal output and displays it in a dedicated text view.

Process Management: Dynamic process creation using the fork() and execvp() system calls.

Inter-Process Communication: Implementation of Pipes (|) to connect the output of one process to the input of another.

I/O Redirection: Support for Output Redirection (>) using dup2() and open().

Built-in Commands: Support for shell-specific commands like cd (Change Directory).

Performance Metrics: Real-time calculation and display of command execution time using gettimeofday().

Logging System: Automatic logging of all user commands to logs/history.log using low-level file operations.

🛠 Prerequisites

The project is designed to run on Linux (Ubuntu/Debian). You need the following installed:

Compiler: GCC

GUI Libraries: GTK+ 3.0

Environment: POSIX-compliant terminal

🏗 System Architecture

The shell operates on a Parent-Child process model:

Parent Process: Handles the GTK main loop, captures user input, and manages the GUI state.

Child Process: Created via fork(). It handles command parsing and execution.

Communication: A kernel-level pipe() is used to redirect the child’s standard output/error back to the parent for display in the GUI.

📊 Evaluation & Metrics

This project includes a built-in benchmarking tool that measures:

Execution Time: The total time elapsed from fork() to wait() for each command.

Logging: A persistent record of commands stored in /logs for auditing purposes.

👥 Contributors

Usman Abdullah - 24K-0568

Umair Ahmed - 24K-0677

Muhammad bin Humbal - 24K-2054
