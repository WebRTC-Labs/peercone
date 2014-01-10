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

void peercone_display();

void Display(vout_display_t* vd, picture_t* picture, subpicture_t* subpicture) {
  msg_Info(vd, __func__);

  peercone_display();

  picture_Release(picture);
  VLC_UNUSED(subpicture);
}

int Control(vout_display_t* vd, int, va_list) {
  msg_Info(vd, __func__);
  return VLC_EGENERIC;
}

void peercone_init(const char* host, int port);
void peercone_close();

int Init(vlc_object_t* object) {
  vout_display_t* vd = (vout_display_t*)object;
  vd->sys = new vout_display_sys_t;
  vd->fmt.i_chroma = VLC_CODEC_I420;

  vd->pool = Pool;
  vd->display = Display;
  vd->control = Control;

  peercone_init("172.16.32.23", 8888);

  return VLC_SUCCESS;
}

void Close(vlc_object_t* object) {
  (void)object;
  peercone_close();
}

#undef ARRAY_SIZE

#include "talk/base/thread.h"
#include "talk/media/base/videocapturer.h"

#include "peercone/conductor.h"
#include "peercone/main_wnd.h"
#include "peercone/peer_connection_client.h"
#include "peercone/vlc_video_capturer.h"

extern const uint16 kDefaultServerPort;  // From defaults.[h|cc]

class CustomSocketServer : public talk_base::PhysicalSocketServer {
 public:
  CustomSocketServer(talk_base::Thread* thread, NoWindow* wnd)
      : thread_(thread), wnd_(wnd), conductor_(NULL), client_(NULL) {}
  virtual ~CustomSocketServer() {}

  void set_client(PeerConnectionClient* client) { client_ = client; }
  void set_conductor(Conductor* conductor) { conductor_ = conductor; }

  virtual bool Wait(int cms, bool process_io) {
    wnd_->HandleUICallbacks();
    return talk_base::PhysicalSocketServer::Wait(0/*cms == -1 ? 1 : cms*/,
                                                 process_io);
  }

 protected:
  talk_base::Thread* thread_;
  NoWindow* wnd_;
  Conductor* conductor_;
  PeerConnectionClient* client_;
}* socket_server_;

VlcVideoCapturer vlc_capturer;
PeerConnectionClient *client = NULL;
NoWindow* wnd;

void peercone_init(const char* host, int port) {
  wnd = new NoWindow(host, port);
}

void peercone_display() {
  if (client == NULL) {
    talk_base::AutoThread auto_thread;
    talk_base::Thread* thread = talk_base::Thread::Current();
    socket_server_ = new CustomSocketServer(thread, wnd);
    thread->set_socketserver(socket_server_);

    // Must be constructed after we set the socketserver.
    client = new PeerConnectionClient;
    talk_base::scoped_refptr<Conductor> conductor(
        new talk_base::RefCountedObject<Conductor>(client, wnd, &vlc_capturer));
    socket_server_->set_client(client);
    socket_server_->set_conductor(conductor);
    wnd->Create();
  }

  socket_server_->Wait(0, true);
}

void peercone_close() {
  talk_base::AutoThread auto_thread;
  talk_base::Thread* thread = talk_base::Thread::Current();
  thread->set_socketserver(NULL);
  delete socket_server_;
  delete client;
  delete wnd;
}
