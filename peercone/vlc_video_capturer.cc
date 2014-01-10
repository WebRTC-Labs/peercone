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

#include "peercone/vlc_video_capturer.h"

#include "talk/base/logging.h"

VlcVideoCapturer::VlcVideoCapturer() : crit_(),
                                       frame_buffer_size_(0),
                                       start_time_ns_(0),
                                       last_frame_timestamp_ns_(0) {
}

VlcVideoCapturer::~VlcVideoCapturer() {
}

cricket::CaptureState VlcVideoCapturer::Start(
    const cricket::VideoFormat& capture_format) {
  talk_base::CritScope cs(&crit_);
  if (IsRunning()) {
    LOG(LS_ERROR) << "The file video capturer is already running";
      return cricket::CS_FAILED;
  }
  SetCaptureFormat(&capture_format);
  start_time_ns_ = 1000000 *
      static_cast<int64>(talk_base::Time()); // ns to ms
  LOG(LS_INFO) << "VLC video capturer '" << GetId() << "' started";
  return cricket::CS_RUNNING;
}

bool VlcVideoCapturer::IsRunning() {
  talk_base::CritScope cs(&crit_);
  return is_running_;
}

void VlcVideoCapturer::Stop() {
  talk_base::CritScope cs(&crit_);
  if (!is_running_)
    return;

  is_running_ = false;
  SetCaptureFormat(NULL);
  LOG(LS_INFO) << "VLC video capturer '" << GetId() << "' stopped";
}

bool VlcVideoCapturer::GetPreferredFourccs(std::vector<uint32>* fourccs) {
  fourccs->clear();
  fourccs->push_back(cricket::FOURCC_I420);
  return true;
}

void VlcVideoCapturer::OnCapturedFrame(vout_display_t* vd,
                                       picture_t* picture,
                                       subpicture_t* subpicture) {
  talk_base::CritScope cs(&crit_);
  if (!is_running_)
    return;

  //TODO: Hard code fewer thingys
  captured_frame_.height = picture->format.i_height;
  captured_frame_.width = picture->format.i_width;
  captured_frame_.pixel_height = 1;
  captured_frame_.pixel_width = 1;
  captured_frame_.rotation = 0;
  captured_frame_.fourcc = cricket::FOURCC_I420;
  captured_frame_.time_stamp = picture->date; // ?
  captured_frame_.data_size = 0;
  for (uint32_t i = 0; i < picture->i_planes; ++i) {
    plane_t plane = picture->p[i];
    captured_frame_.data_size += plane.i_lines * plane.i_pitch;
  }

  const cricket::VideoFormat* fmt = GetCaptureFormat();
  if (fmt->fourcc != cricket::FOURCC_I420 ||
      fmt->height != captured_frame_.height ||
      fmt->width != captured_frame_.width) {
    // TODO: Properly handle this
    cricket::VideoFormat new_fmt(*fmt);
    new_fmt.fourcc = cricket::FOURCC_I420;
    new_fmt.width = captured_frame_.width;
    new_fmt.height = captured_frame_.height;
    SetCaptureFormat(&new_fmt);
  }

  uint32 start_read_time_ms = talk_base::Time();
  captured_frame_.elapsed_time = captured_frame_.time_stamp - start_time_ns_;

  if (frame_buffer_size_ < captured_frame_.data_size) {
    frame_buffer_size_ = captured_frame_.data_size;
    delete[] static_cast<char*>(captured_frame_.data);
    captured_frame_.data = new char[frame_buffer_size_];
  }

  //TODO: Memcpy data here
  for (uint32_t i = 0; i < picture->i_planes; ++i) {
    plane_t plane = picture->p[i];
    uint32_t plane_size = plane.i_lines * plane.i_pitch;
    uint32_t stupid_val = 255 * (i + 1) / picture->i_planes;
    memset(captured_frame_.data, 0, plane_size);
  }

  SignalFrameCaptured(this, &captured_frame_);
}
