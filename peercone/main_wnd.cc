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

#include "peercone/main_wnd.h"

#include <stddef.h>

#include "talk/base/common.h"
#include "talk/base/logging.h"
#include "talk/base/stringutils.h"

#include "peercone/defaults.h"

using talk_base::sprintfn;

NoWindow::NoWindow(const char* server, int port)
    : port_(port), server_(server) {}

NoWindow::~NoWindow() {}

void NoWindow::RegisterObserver(MainWndCallback* callback) {
  puts(__func__);
  callback_ = callback;
}

bool NoWindow::IsWindow() { return true; }

void NoWindow::MessageBox(const char* caption,
                          const char* text,
                          bool is_error) {
  fprintf(is_error ? stderr : stdout,
          "NoWindow::MessageBox(%s): %s\n",
          caption,
          text);
}

MainWindow::UI NoWindow::current_ui() { return STREAMING; }

void NoWindow::StartLocalRenderer(webrtc::VideoTrackInterface* local_video) {
  local_renderer_.reset(new VideoRenderer);
}

void NoWindow::StopLocalRenderer() { local_renderer_.reset(); }

void NoWindow::StartRemoteRenderer(webrtc::VideoTrackInterface* remote_video) {
  remote_renderer_.reset(new VideoRenderer);
}

void NoWindow::StopRemoteRenderer() { remote_renderer_.reset(); }

void NoWindow::QueueUIThreadCallback(int msg_id, void* data) {
  talk_base::CritScope cs(&callback_lock_);
  callbacks_.push_front(new UIThreadCallbackData(callback_, msg_id, data));
}

void NoWindow::HandleUICallbacks() {
  while (true) {
    UIThreadCallbackData* cb_data;
    {
      talk_base::CritScope cs(&callback_lock_);
      if (callbacks_.empty())
        return;
      cb_data = callbacks_.front();
      callbacks_.pop_front();
    }
    cb_data->callback->UIThreadCallback(cb_data->msg_id, cb_data->data);
    free(cb_data);
  }
}

void NoWindow::Create() { callback_->StartLogin(server_, port_); }
void NoWindow::SwitchToConnectUI() {}
void NoWindow::SwitchToPeerList(const Peers& peers) {
  if (peers.empty())
    return;
  int id = peers.begin()->first;
  callback_->ConnectToPeer(id);
}

void NoWindow::SwitchToStreamingUI() { LOG(INFO) << __FUNCTION__; }

void NoWindow::VideoRenderer::SetSize(int width, int height) {}
void NoWindow::VideoRenderer::RenderFrame(const cricket::VideoFrame* frame) {}

