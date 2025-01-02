#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <libnotify/notify.h>
#include <wayland-client.h>

// Global variables for tracking user activity
time_t last_active_time;
time_t idle_timeout = 300;  // Idle timeout in seconds
time_t short_break_timeout = 1200; // Short break after 30 minutes
struct wl_display *display;
struct wl_registry *registry;
int is_wayland = 1;

// Placeholder for Wayland idle time fetching
time_t get_wayland_idle_time() {
    // Replace with real Wayland idle query
    // Currently, just a placeholder
    return 0;  // Default idle time
}

// Function to send notifications
void send_notification(const char *title, const char *message) {
    if (!notify_is_initted()) {
        notify_init("Break Reminder");
    }
    NotifyNotification *notif = notify_notification_new(title, message, NULL);
    notify_notification_show(notif, NULL);
    g_object_unref(G_OBJECT(notif));
}

// Function to check activity and manage break reminders
void check_activity(int signum) {
    time_t current_time = time(NULL);
    time_t idle_time = 0;

    // Get idle time based on the environment (Wayland or fallback)
    if (is_wayland) {
        idle_time = get_wayland_idle_time();
    } else {
        idle_time = 0;  // Placeholder for X11 or other environment idle detection
    }

    // If idle timeout is reached, remind user to take a break
    if (idle_time >= idle_timeout) {
        send_notification("Take a Break!", "You have been active for too long. Time to rest.");
    }

    if (current_time - last_active_time >= short_break_timeout) {
        send_notification("Short Break", "You've been working for 30 minutes. Please take a short break.");
        last_active_time = current_time;  // Reset timer after reminder
    }

    alarm(60);  // Set the alarm to trigger in 1 minute
}

int main() {
    // Initialize last active time
    last_active_time = time(NULL);

    // Detect if running on Wayland
    char *display_env = getenv("WAYLAND_DISPLAY");
    if (display_env) {
        is_wayland = 1;  // We're on Wayland
    } else {
        is_wayland = 0;  // Not on Wayland
    }

    signal(SIGALRM, check_activity);
    alarm(60);  // Start checking every minute

    // Main loop for checking activity
    while (1) {
        // Poll for idle time (Wayland-specific or fallback here)
        printf("Monitoring user activity...\n");
        sleep(10);  // Sleep before checking again
    }

    return 0;
}
