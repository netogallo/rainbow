// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef CONFUOCO_CODECS_OGGVORBISAUDIOFILE_H_
#define CONFUOCO_CODECS_OGGVORBISAUDIOFILE_H_

#include "Platform/Macros.h"
#if !defined(RAINBOW_JS) && defined(RAINBOW_SDL) && !defined(USE_FMOD_STUDIO)
#   define USE_OGGVORBISAUDIOFILE
#endif

#ifdef USE_OGGVORBISAUDIOFILE

#ifdef RAINBOW_OS_MACOS
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#   pragma GCC diagnostic ignored "-Wunused-variable"
#endif
#include <vorbis/vorbisfile.h>
#ifdef RAINBOW_OS_MACOS
#   pragma GCC diagnostic pop
#endif

#include "ConFuoco/AudioFile.h"

namespace ConFuoco
{
    /// Ogg Vorbis implementation of AudioFile.
    class OggVorbisAudioFile final : public AudioFile
    {
    public:
        OggVorbisAudioFile(File &&file);
        virtual ~OggVorbisAudioFile();

    private:
        OggVorbis_File vf;
        vorbis_info *vi;

        /* Implement AudioFile */

        int channels_impl() const override;
        int rate_impl() const override;
        size_t read_impl(char **dst) override;
        size_t read_impl(char *dst, const size_t size) override;
        void rewind_impl() override;
    };
}

#endif  // USE_OGGVORBISAUDIOFILE
#endif  // CONFUOCO_OGGVORBISAUDIOFILE_H_
