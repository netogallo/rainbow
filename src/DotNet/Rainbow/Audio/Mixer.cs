// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Audio
{
    using Interop;

    public sealed partial class Mixer
    {
        private Mixer()
        {
            // Intentionally empty, private constructor.
        }

        #region Sound management
        public static Sound LoadSound(string path) => new Sound(loadSound(path.GetUTF8Bytes()));

        public static Sound LoadStream(string path) => new Sound(loadStream(path.GetUTF8Bytes()));

        public static void Release(Sound sound) => sound.Dispose();
        #endregion

        #region Playback
        public static bool IsPaused(Channel channel) => isPaused(channel.NativeHandle);

        public static bool IsPlaying(Channel channel) => isPlaying(channel.NativeHandle);

        public static void SetLoopCount(Channel channel, int count)
            => setLoopCount(channel.NativeHandle, count);

        public static void SetVolume(Channel channel, float volume)
            => setVolume(channel.NativeHandle, volume);

        public static void SetWorldPosition(Channel channel, Vector2 pos)
            => setWorldPosition(channel.NativeHandle, pos);

        public static void Pause(Channel channel) => pause(channel.NativeHandle);

        public static Channel Play(Channel channel)
            => new Channel(playChannel(channel.NativeHandle));

        public static Channel Play(Sound sound) => Play(sound, Vector2.Zero);

        public static Channel Play(Sound sound, Vector2 worldPosition)
            => new Channel(playSound(sound.NativeHandle, worldPosition));

        public static void Stop(Channel channel) => stop(channel.NativeHandle);
        #endregion
    }
}
