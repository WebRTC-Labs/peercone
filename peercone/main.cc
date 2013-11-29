#include "talk/base/flags.h"
#include "talk/base/thread.h"

#include "peercone/conductor.h"
#include "peercone/main_wnd.h"
#include "peercone/peer_connection_client.h"

extern const uint16 kDefaultServerPort;  // From defaults.[h|cc]

DEFINE_string(server, "localhost", "The server to connect to.");
DEFINE_int(port, kDefaultServerPort,
           "The port on which the server is listening.");

class CustomSocketServer : public talk_base::PhysicalSocketServer {
 public:
  CustomSocketServer(talk_base::Thread* thread, NoWindow* wnd)
      : thread_(thread), wnd_(wnd), conductor_(NULL), client_(NULL) {}
  virtual ~CustomSocketServer() {}

  void set_client(PeerConnectionClient* client) { client_ = client; }
  void set_conductor(Conductor* conductor) { conductor_ = conductor; }

  // Override so that we can also pump the GTK message loop.
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
};

int main(int argc, char* argv[]) {
  FlagList::SetFlagsFromCommandLine(&argc, argv, true);

  // Abort if the user specifies a port that is outside the allowed
  // range [1, 65535].
  if ((FLAG_port < 1) || (FLAG_port > 65535)) {
    printf("Error: %i is not a valid port.\n", FLAG_port);
    return -1;
  }

  NoWindow wnd(FLAG_server, FLAG_port);

  talk_base::AutoThread auto_thread;
  talk_base::Thread* thread = talk_base::Thread::Current();
  CustomSocketServer socket_server(thread, &wnd);
  thread->set_socketserver(&socket_server);

  // Must be constructed after we set the socketserver.
  PeerConnectionClient client;
  talk_base::scoped_refptr<Conductor> conductor(
      new talk_base::RefCountedObject<Conductor>(&client, &wnd));
  socket_server.set_client(&client);
  socket_server.set_conductor(conductor);
  wnd.Create();

  thread->Run();
  thread->set_socketserver(NULL);

  return 0;
}

