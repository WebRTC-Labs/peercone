/*
 * libjingle
 * Copyright 2012, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PEERCONNECTION_SAMPLES_CLIENT_LINUX_MAIN_WND_H_
#define PEERCONNECTION_SAMPLES_CLIENT_LINUX_MAIN_WND_H_

#include "talk/app/webrtc/mediastreaminterface.h"
#include "talk/media/base/videocommon.h"
#include "talk/media/base/videoframe.h"

#include "peercone/main_wnd.h"
#include "peercone/peer_connection_client.h"

class MainWndCallback {
 public:
  virtual void StartLogin(const std::string& server, int port) = 0;
  virtual void DisconnectFromServer() = 0;
  virtual void ConnectToPeer(int peer_id) = 0;
  virtual void DisconnectFromCurrentPeer() = 0;
  virtual void UIThreadCallback(int msg_id, void* data) = 0;
  virtual void Close() = 0;

 protected:
  virtual ~MainWndCallback() {}
};

namespace {
struct UIThreadCallbackData {
  explicit UIThreadCallbackData(MainWndCallback* cb, int id, void* d)
      : callback(cb), msg_id(id), data(d) {}
  MainWndCallback* callback;
  int msg_id;
  void* data;
};
}

// Pure virtual interface for the main window.
class MainWindow {
 public:
  virtual ~MainWindow() {}

  enum UI {
    CONNECT_TO_SERVER,
    LIST_PEERS,
    STREAMING,
  };

  virtual void RegisterObserver(MainWndCallback* callback) = 0;

  virtual bool IsWindow() = 0;
  virtual void MessageBox(const char* caption,
                          const char* text,
                          bool is_error) = 0;

  virtual UI current_ui() = 0;

  virtual void SwitchToConnectUI() = 0;
  virtual void SwitchToPeerList(const Peers& peers) = 0;
  virtual void SwitchToStreamingUI() = 0;

  virtual void StartLocalRenderer(webrtc::VideoTrackInterface* local_video) = 0;
  virtual void StopLocalRenderer() = 0;
  virtual void StartRemoteRenderer(
      webrtc::VideoTrackInterface* remote_video) = 0;
  virtual void StopRemoteRenderer() = 0;

  virtual void QueueUIThreadCallback(int msg_id, void* data) = 0;
};

// Implements the main UI of the peer connection client.
// This is functionally equivalent to the MainWnd class in the Windows
// implementation.
class NoWindow : public MainWindow {
 public:
  NoWindow(const char* server, int port);
  ~NoWindow();

  virtual void RegisterObserver(MainWndCallback* callback);
  virtual bool IsWindow();
  virtual void SwitchToConnectUI();
  virtual void SwitchToPeerList(const Peers& peers);
  virtual void SwitchToStreamingUI();
  virtual void MessageBox(const char* caption, const char* text, bool is_error);
  virtual MainWindow::UI current_ui();
  virtual void StartLocalRenderer(webrtc::VideoTrackInterface* local_video);
  virtual void StopLocalRenderer();
  virtual void StartRemoteRenderer(webrtc::VideoTrackInterface* remote_video);
  virtual void StopRemoteRenderer();

  virtual void QueueUIThreadCallback(int msg_id, void* data);
  void HandleUICallbacks();

  void Create();

 protected:
  class VideoRenderer : public webrtc::VideoRendererInterface {
   public:
    // VideoRendererInterface implementation
    virtual void SetSize(int width, int height) OVERRIDE;
    virtual void RenderFrame(const cricket::VideoFrame* frame) OVERRIDE;

    const uint8* image() const { return image_.get(); }

    int width() const { return width_; }

    int height() const { return height_; }

   protected:
    talk_base::scoped_ptr<uint8[]> image_;
    int width_;
    int height_;
    NoWindow* main_wnd_;
    talk_base::scoped_refptr<webrtc::VideoTrackInterface> rendered_track_;
  };

 protected:
  MainWndCallback* callback_;
  std::string server_;
  int port_;
  bool autoconnect_;
  bool autocall_;
  talk_base::scoped_ptr<VideoRenderer> local_renderer_;
  talk_base::scoped_ptr<VideoRenderer> remote_renderer_;
  talk_base::scoped_ptr<uint8> draw_buffer_;
  int draw_buffer_size_;

  talk_base::CriticalSection callback_lock_;
  std::deque<UIThreadCallbackData*> callbacks_;
};

#endif  // PEERCONNECTION_SAMPLES_CLIENT_LINUX_MAIN_WND_H_
