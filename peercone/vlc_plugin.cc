#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_vout_display.h>

#ifndef MODULE_STRING
#define MODULE_STRING "peercone"
#endif

#define DOMAIN "peercone"
#define _(str) dgettext(DOMAIN, str)
#define N_(str) (str)

static int Init(vlc_object_t*);
static void Close(vlc_object_t*);

// VLC module declaration
vlc_module_begin()

set_shortname(N_("peercone"))
set_description(N_("PeerConnection interface"))
set_capability("vout display", 0)
set_callbacks(Init, Close)
set_category(CAT_VIDEO)
set_subcategory(SUBCAT_VIDEO_VOUT)
add_string("host", "127.0.0.1", "Target", "Where should we connect?", false)

vlc_module_end()

struct vout_display_sys_t {
  FILE* f;
  bool is_first;
  bool is_yuv4mpeg2;

  picture_pool_t* pool;
};

picture_pool_t* Pool(vout_display_t* vd, unsigned count) {
  msg_Info(vd, __func__);
  if (!vd->sys->pool)
    vd->sys->pool = picture_pool_NewFromFormat(&vd->fmt, count);
  return vd->sys->pool;
}

void Display(vout_display_t* vd, picture_t* picture, subpicture_t* subpicture) {
  msg_Info(vd, __func__);

  picture_Release(picture);
  VLC_UNUSED(subpicture);
}

int Control(vout_display_t* vd, int, va_list) {
  msg_Info(vd, __func__);
  return VLC_EGENERIC;
}

int Init(vlc_object_t* object) {
  vout_display_t* vd = (vout_display_t*)object;
  vd->sys = new vout_display_sys_t;
  vd->fmt.i_chroma = VLC_CODEC_I420;

  vd->pool = Pool;
  vd->display = Display;
  vd->control = Control;

  return VLC_SUCCESS;
}

void Close(vlc_object_t* object) { (void)object; }
