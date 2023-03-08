#include <GL/gl.h>
#include <assert.h>
#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include <flutter_linux/flutter_linux.h>
#include <glib-object.h>
#include <glib.h>

#include "my_application.h"

static GdkGLContext *context = nullptr;

static bool flutter_texture_populate(FlTextureGL *texture, uint32_t *target,
                                     uint32_t *name, uint32_t *width,
                                     uint32_t *height) {
  GLuint tex;
  unsigned char *buf;
  int x, y;

  fprintf(stderr, "Populating Flutter texture %p\n", texture);

  assert(context != nullptr);
  gdk_gl_context_make_current(context);

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  buf = (unsigned char *)malloc(*width * *height * 4);
  for (x = 0; x < *width; x++) {
    for (y = 0; y < *height; y++) {
      int p = 4 * (x + y * *width);
      buf[p + 0] = x % 255;
      buf[p + 1] = 0;
      buf[p + 2] = y % 255;
      buf[p + 3] = 255;
    }
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, buf);

  gdk_gl_context_clear_current();

  fprintf(stderr, "Populated Flutter texture %p with GL texture %u\n", texture,
          tex);

  *target = GL_TEXTURE_2D;
  *name = tex;
  return true;
}

G_DECLARE_FINAL_TYPE(FlRenderTextureGL, fl_render_texture_gl, FL,
                     RENDER_TEXTURE_GL, FlTextureGL)

struct _FlRenderTextureGL {
  FlTextureGL parent_instance;
};

G_DEFINE_TYPE(FlRenderTextureGL, fl_render_texture_gl, fl_texture_gl_get_type())

static gboolean fl_render_texture_gl_populate(FlTextureGL *texture,
                                              uint32_t *target, uint32_t *name,
                                              uint32_t *width, uint32_t *height,
                                              GError **error) {
  if (!flutter_texture_populate(texture, target, name, width, height)) {
    *error =
        g_error_new_literal(g_quark_from_static_string("gl_widget_flutter"), 0,
                            "populate texture failed");
    return false;
  } else {
    return true;
  }
}

extern "C" void fl_render_texture_gl_unref(FlRenderTextureGL *self) {
  g_object_unref(self);
}

static void fl_render_texture_gl_class_init(FlRenderTextureGLClass *klass) {
  FL_TEXTURE_GL_CLASS(klass)->populate = fl_render_texture_gl_populate;
}

static void fl_render_texture_gl_init(__attribute__((unused))
                                      FlRenderTextureGL *self) {}

static FlMethodResponse *new_texture() {
  FlRenderTextureGL *fl_texture;
  g_autoptr(GError) error = nullptr;

  fprintf(stderr, "create Flutter texture\n");

  fl_texture =
      (FlRenderTextureGL *)g_object_new(fl_render_texture_gl_get_type(), NULL);
  if (!fl_texture_registrar_register_texture(flutter_texture_registrar,
                                             (FlTexture *)fl_texture)) {
    assert(false);
  }

  assert(context == nullptr);
  context = gdk_window_create_gl_context(main_window, &error);
  assert(context != nullptr);
  gdk_gl_context_set_required_version(context, 0, 0);
  gdk_gl_context_set_use_es(context, -1);
  if (!gdk_gl_context_realize(context, &error)) {
    assert(false);
  }
  gdk_gl_context_clear_current();

  fprintf(stderr, "created Flutter texture %p\n", fl_texture);

  fl_texture_registrar_mark_texture_frame_available(flutter_texture_registrar,
                                                    (FlTexture *)fl_texture);

  g_autoptr(FlValue) result = fl_value_new_int((int64_t)fl_texture);
  return FL_METHOD_RESPONSE(fl_method_success_response_new(result));
}

void texture_method_call_handler(FlMethodChannel *channel,
                                 FlMethodCall *method_call,
                                 gpointer user_data) {
  g_autoptr(FlMethodResponse) response = nullptr;
  if (strcmp(fl_method_call_get_name(method_call), "newTexture") == 0) {
    response = new_texture();
  } else {
    response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }

  g_autoptr(GError) error = nullptr;
  if (!fl_method_call_respond(method_call, response, &error)) {
    g_warning("Failed to send response: %s", error->message);
  }
}
