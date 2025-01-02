#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <libnotify/notify.h>

// Global variables for tracking user activity
time_t last_active_time;
time_t idle_timeout = 300;  // Idle timeout in seconds 
time_t short_break_timeout = 1200; // Short break after 30 minutes

void send_notification(const char *title, const char *message) {
    if (!notify_is_initted()) {
        notify_init("Break Reminder");
    }
    NotifyNotification *notif = notify_notification_new(title, message, NULL);
    notify_notification_show(notif, NULL);
    g_object_unref(G_OBJECT(notif));
}

void check_activity(int signum) {
    time_t current_time = time(NULL);
    // If idle timeout is reached, remind user to take a break
    if (current_time - last_active_time >= idle_timeout) {
        send_notification("Take a Break!", "You have been active for too long. Time to rest.");
    }

    if (current_time - last_active_time >= short_break_timeout) {
        send_notification("Short Break", "You've been working for 30 minutes. Please take a short break.");
        last_active_time = current_time;  // Reset timer after reminder
    }

    alarm(60);  // Set the alarm to trigger in 1 minute
}

int main() {
    // Set up initial values
    last_active_time = time(NULL);
  
    signal(SIGALRM, check_activity);
    alarm(60);  // Start checking every minute

    while (1) {
        // Simulate activity check here (e.g., capture keyboard/mouse activity)
        // You can integrate with Wayland idle notifications if necessary
        printf("Monitoring user activity...\n");
        sleep(10);  // Sleep for 10 seconds before checking again
    }

    return 0;
}
