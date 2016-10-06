// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Input
{
    using System.Runtime.InteropServices;

    [StructLayout(LayoutKind.Sequential)]
    public struct ControllerButtonEvent
    {
        private uint id;
        private ControllerButton button;
        private ulong timestamp;

        public uint Id => id;

        public ControllerButton Button => button;

        public ulong Timestamp => timestamp;
    }
}
