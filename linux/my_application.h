#ifndef FLUTTER_MY_APPLICATION_H_
#define FLUTTER_MY_APPLICATION_H_

#include <gtk/gtk.h>
#include <flutter_linux/flutter_linux.h>

G_DECLARE_FINAL_TYPE(MyApplication, my_application, MY, APPLICATION,
                     GtkApplication)

/**
 * my_application_new:
 *
 * Creates a new Flutter-based application.
 *
 * Returns: a new #MyApplication.
 */
MyApplication *my_application_new();

void texture_method_call_handler(FlMethodChannel *channel,
                                 FlMethodCall *method_call, gpointer user_data);

extern GdkWindow *main_window;

extern FlTextureRegistrar *flutter_texture_registrar;


#endif // FLUTTER_MY_APPLICATION_H_
