// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Audio
{
    using System;

    public sealed partial class Sound
    {
        public Sound(IntPtr nativeHandle)
        {
            NativeHandle = nativeHandle;
        }
    }
}
