// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Audio
{
    using System;

    public struct Channel
    {
        public Channel(IntPtr nativeHandle)
        {
            NativeHandle = nativeHandle;
        }

        public IntPtr NativeHandle { get; }
    }
}
