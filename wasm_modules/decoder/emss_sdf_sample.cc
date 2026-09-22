// Copyright (c) 2020 Samsung Electronics Inc.
// Licensed under the MIT license.

#include "emss_sdf_sample.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <utility>

#include "sample_data.h"

using ElementaryMediaStreamSource =
    samsung::wasm::ElementaryMediaStreamSource;
using ElementaryMediaStreamSourceListener =
    samsung::wasm::ElementaryMediaStreamSourceListener;
using ElementaryMediaTrack = samsung::wasm::ElementaryMediaTrack;
using ElementaryMediaTrackListener =
    samsung::wasm::ElementaryMediaTrackListener;
using HTMLMediaElement = samsung::html::HTMLMediaElement;
using HTMLMediaElementListener = samsung::html::HTMLMediaElementListener;
using Seconds = samsung::wasm::Seconds;
using SessionId = samsung::wasm::SessionId;

static constexpr char kVideoTagId[] = "video-element";

TrackDataPump::TrackDataPump(ElementaryMediaTrack video_track)
    : video_track_(std::move(video_track)),
      pump_worker_([this]() { this->PumpPackets(); }),
      last_reported_running_time_(0),
      session_id_(video_track_.GetSessionId().value) {
  video_track_.SetListener(this);
}

TrackDataPump::~TrackDataPump() {
  messages_.PushTerminate();
  pump_worker_.detach();
}

void TrackDataPump::UpdateTime(Seconds new_time) {
  if (last_reported_running_time_ + kWorkerUpdateThreshold > new_time) {
    return;
  }

  last_reported_running_time_ = new_time;

  messages_.PushBufferToPts(
      new_time + kBufferAhead,
      session_id_);
}

void TrackDataPump::OnTrackOpen() {
  messages_.PushBufferToPts(
      last_reported_running_time_ + kBufferAhead,
      session_id_);
}

void TrackDataPump::OnTrackClosed(
    ElementaryMediaTrack::CloseReason) {
  messages_.Flush();
}

void TrackDataPump::OnSeek(Seconds new_time) {
  last_reported_running_time_ = new_time;
  messages_.PushSeekTo(new_time);
}

void TrackDataPump::OnSessionIdChanged(SessionId session_id) {
  session_id_ = session_id;
}

TrackDataPump::WorkerMessageQueue::Message::Message(
    Type type,
    Seconds time,
    SessionId session_id)
    : type(type),
      time(time),
      session_id(session_id) {}

void TrackDataPump::WorkerMessageQueue::Flush() {
  std::unique_lock<std::mutex> lock(messages_mutex_);
  FlushWhileLocked();
}

TrackDataPump::WorkerMessageQueue::Message
TrackDataPump::WorkerMessageQueue::Pop() {
  std::unique_lock<std::mutex> lock(messages_mutex_);

  while (message_queue_.empty()) {
    messages_changed_.wait(lock);
  }

  Message result = message_queue_.front();
  message_queue_.pop();

  return result;
}

void TrackDataPump::WorkerMessageQueue::PushBufferToPts(
    Seconds time,
    SessionId session_id) {
  {
    std::lock_guard<std::mutex> lock(messages_mutex_);

    message_queue_.emplace(
        WorkerMessageQueue::Message::Type::kSetBufferToPts,
        time,
        session_id);
  }

  messages_changed_.notify_one();
}

void TrackDataPump::WorkerMessageQueue::PushSeekTo(
    Seconds time) {
  {
    std::lock_guard<std::mutex> lock(messages_mutex_);

    FlushWhileLocked();

    message_queue_.emplace(
        WorkerMessageQueue::Message::Type::kSeekTo,
        time,
        0);
  }

  messages_changed_.notify_one();
}

void TrackDataPump::WorkerMessageQueue::PushTerminate() {
  {
    std::lock_guard<std::mutex> lock(messages_mutex_);

    FlushWhileLocked();

    message_queue_.emplace(
        WorkerMessageQueue::Message::Type::kTerminate,
        Seconds{0},
        0);
  }

  messages_changed_.notify_one();
}

void TrackDataPump::WorkerMessageQueue::FlushWhileLocked() {
  std::queue<Message> tmp;
  message_queue_.swap(tmp);
}

// static
size_t TrackDataPump::GetClosestKeyframeIndex(Seconds time) {
  auto keyframe = std::find_if(
      sample_data::kVideoPackets.crbegin(),
      sample_data::kVideoPackets.crend(),
      [time](const samsung::wasm::ElementaryMediaPacket& packet) {
        return packet.is_key_frame && packet.pts < time;
      });

  if (keyframe == sample_data::kVideoPackets.crend()) {
    return 0;
  }

  return static_cast<size_t>(
      &(*keyframe) - sample_data::kVideoPackets.data());
}

void TrackDataPump::PumpPackets() {
  typedef WorkerMessageQueue::Message Message;

  bool ended = false;
  unsigned int packet_idx = 0;
  SessionId session_id = 0;

  while (true) {
    Message message = messages_.Pop();

    switch (message.type) {
      case Message::Type::kSetBufferToPts:
        session_id = message.session_id;

        while (
            packet_idx < sample_data::kVideoPackets.size() &&
            sample_data::kVideoPackets[packet_idx].pts <
                message.time) {
          samsung::wasm::ElementaryMediaPacket packet =
              sample_data::kVideoPackets[packet_idx];

          packet.session_id = session_id;

          video_track_.AppendPacket(packet);

          ++packet_idx;
        }

        if (!ended &&
            packet_idx == sample_data::kVideoPackets.size()) {
          ended = true;
          video_track_.AppendEndOfTrack(session_id);
        }

        break;

      case Message::Type::kSeekTo:
        ended = false;
        packet_idx = GetClosestKeyframeIndex(message.time);
        break;

      case Message::Type::kTerminate:
        return;
    }
  }
}

void SamplePlayer::SetUp(
    ElementaryMediaStreamSource::RenderingMode rendering_mode) {
  media_element_.reset(
      new HTMLMediaElement(kVideoTagId));

  media_element_->SetListener(this);

  source_.reset(
      new ElementaryMediaStreamSource(
          ElementaryMediaStreamSource::LatencyMode::kNormal,
          rendering_mode));

  source_->SetListener(this);

  media_element_->SetSrc(source_.get());
}

void SamplePlayer::OnSourceClosed() {
  source_->SetDuration(sample_data::kStreamDuration);

  auto add_track_result =
      source_->AddTrack(sample_data::kVideoTrackConfig);

  if (!add_track_result) {
    std::cout
        << "Cannot add a video track!"
        << std::endl;
    return;
  }

  ElementaryMediaTrack video_track =
      std::move(add_track_result.value);

  track_data_pump_ =
      CreateTrackDataPump(std::move(video_track));

  source_->Open(
      [](samsung::wasm::OperationResult result) {
        if (result !=
            samsung::wasm::OperationResult::kSuccess) {
          std::cout
              << "Cannot open ElementaryMediaStreamSource."
              << std::endl;
        }
      });
}

void SamplePlayer::OnPlaybackPositionChanged(
    Seconds new_time) {
  if (track_data_pump_) {
    track_data_pump_->UpdateTime(new_time);
  }
}

void SamplePlayer::OnCanPlay() {
  if (!media_element_->IsPaused()) {
    return;
  }

  media_element_->Play(
      [](samsung::wasm::OperationResult result) {
        if (result !=
            samsung::wasm::OperationResult::kSuccess) {
          std::cout
              << "Cannot play."
              << std::endl;
        }
      });
}

std::unique_ptr<TrackDataPump>
SamplePlayer::CreateTrackDataPump(
    ElementaryMediaTrack&& video_track) {
  return std::unique_ptr<TrackDataPump>(
      new TrackDataPump(std::move(video_track)));
}