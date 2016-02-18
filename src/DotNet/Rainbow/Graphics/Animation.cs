// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using System;
    using System.Runtime.InteropServices;

    using Diagnostics;

    public sealed partial class Animation
    {
        public const uint AnimationEnd = uint.MaxValue;

        private Sprite sprite;
        private int delay;
        private EventHandler<AnimationEventArgs> eventHandler;
        private GCHandle gcHandle;

        public Animation(Sprite sprite, uint[] frames, uint fps, int delay = 0)
        {
            this.sprite = sprite;
            this.delay = delay;
            this.gcHandle = GCHandle.Alloc(this, GCHandleType.Weak);
            NativeHandle = ctor(
                sprite.Parent.NativeHandle,
                sprite.Index,
                frames,
                (uint)frames.Length,
                fps,
                delay);
        }

        ~Animation()
        {
            gcHandle.Free();
        }

        public uint CurrentFrame => currentFrame(NativeHandle);

        public int Delay
        {
            get
            {
                return delay;
            }

            set
            {
                delay = value;
                setDelay(NativeHandle, value);
            }
        }

        public uint FrameRate
        {
            get { return frameRate(NativeHandle); }
            set { setFrameRate(NativeHandle, value); }
        }

        public bool IsStopped => isStopped(NativeHandle);

        public Sprite Sprite
        {
            get
            {
                return sprite;
            }

            set
            {
                if (value == sprite)
                {
                    return;
                }

                sprite = value;
                setSprite(NativeHandle, value.Parent.NativeHandle, value.Index);
            }
        }

        public static void PropagateEvent(IntPtr ptr, int ev, uint currentFrame)
        {
            var animation = GCHandle.FromIntPtr(ptr).Target as Animation;
            Assert.NotNull(animation);

            animation.eventHandler?.Invoke(
                animation,
                new AnimationEventArgs((AnimationEvent)ev, currentFrame));
        }

        public void JumpTo(uint frame) => jumpTo(NativeHandle, frame);

        public void Rewind() => JumpTo(0);

        public void SetEventHandler(EventHandler<AnimationEventArgs> handler)
        {
            if (handler == null)
            {
                setEventHandler(NativeHandle, IntPtr.Zero);
            }
            else
            {
                IntPtr ptr = GCHandle.ToIntPtr(gcHandle);
                setEventHandler(NativeHandle, ptr);
            }

            eventHandler = handler;
        }

        public void SetFrames(uint[] frames)
            => setFrames(NativeHandle, frames, (uint)frames.Length);

        public void Start() => start(NativeHandle);

        public void Stop() => stop(NativeHandle);
    }
}
