#ifndef PEERCONE_VLC_VIDEO_CAPTURER_H_
#define PEERCONE_VLC_VIDEO_CAPTURER_H_

#include "talk/media/base/videocapturer.h"
#include "talk/base/criticalsection.h"

#undef ARRAY_SIZE
#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_vout_display.h>

class VlcVideoCapturer : public cricket::VideoCapturer {
 public:
  VlcVideoCapturer();
  virtual ~VlcVideoCapturer();

  // Override virtual methods of parent class VideoCapturer.
  virtual cricket::CaptureState Start(
      const cricket::VideoFormat& capture_format);
  virtual void Stop();
  virtual bool IsRunning();
  virtual bool IsScreencast() const { return false; }

  void OnCapturedFrame(vout_display_t* vd,
                       picture_t* picture,
                       subpicture_t* subpicture);


protected:
  virtual bool GetPreferredFourccs(std::vector<uint32>* fourccs);

private:
  talk_base::CriticalSection crit_;
  // The number of bytes allocated buffer for captured_frame_.data.
  uint32 frame_buffer_size_;
  cricket::CapturedFrame captured_frame_;
  int64 start_time_ns_;  // Time when the file video capturer starts.
  int64 last_frame_timestamp_ns_;  // Timestamp of last read frame.
  bool is_running_;
};

#endif  // PEERCONE_VLC_VIDEO_CAPTURER_H_
