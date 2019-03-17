/*
 * Copyright (c) 2019 WangBin <wbsecg1 at gmail.com>
 */
#include "mdk/c/MediaInfo.h"
#include "mdk/MediaInfo.h"
#include "mdk/VideoFormat.h"
#include "MediaInfoInternal.h"
#include <cassert>

void from_abi(const VideoCodecParameters& in, mdkVideoCodecParameters& out)
{
    out.codec = in.codec.data();
    out.codec_tag = in.codec_tag;
    out.extra_data = in.extra.data();
    out.extra_data_size = (int)in.extra.size();
    out.bit_rate = in.bit_rate;
    out.profile = in.profile;
    out.level = in.level;
    out.frame_rate = in.frame_rate;
    out.format = (int)in.format;
    out.format_name = VideoFormat(in.format).name();
    out.width = in.width;
    out.height = in.height;
    out.b_frames = in.b_frames;
}

void from_abi(const VideoStreamInfo& in, mdkVideoStreamInfo& out)
{
    out.index = in.index;
    out.start_time = in.start_time;
    out.duration = in.duration;
    out.frames = in.frames;
    out.priv = &in;
}

void from_abi(const MediaInfo& in, mdkMediaInfo& out)
{
    out.start_time = in.start_time;
    out.duration = in.duration;
    out.bit_rate = in.bit_rate;
    out.format = in.format.data();
    out.streams = in.streams;
    //out.audio = &in.audio[0];
    out.nb_audio = (int)in.audio.size();
    //out.video = &in.video[0];
    out.nb_video = (int)in.video.size();

    out.priv = &in;
}

void MediaInfoToC(const MediaInfo& abi, MediaInfoInternal* out)
{
    if (!out)
        return;
    out->abi = abi;
    from_abi(out->abi, out->info);
}

extern "C" {

void MDK_VideoStreamCodecParametersGet(const mdkVideoStreamInfo* info, mdkVideoCodecParameters* p)
{
    auto abi = reinterpret_cast<const VideoStreamInfo*>(info->priv);
    from_abi(abi->codec, *p);
}

bool MDK_MediaMetadata(const mdkMediaInfo* info, mdkStringMapEntry* entry)
{
    assert(entry && "entry can not be null");
    auto abi = reinterpret_cast<const MediaInfo*>(info->priv);
    auto it = abi->metadata.cend();
    if (entry->priv) {
        auto pit = (decltype(it)*)entry->priv;
        it = *pit;
        it++;
    } else if (entry->key) {
        it = abi->metadata.find(entry->key);
    } else {
        it = abi->metadata.cbegin();
    }
    if (it == abi->metadata.cend())
        return false;
    entry->key = it->first.data();
    entry->value = it->second.data();
    entry->priv = &it;
    return true;
}

} // extern "C"