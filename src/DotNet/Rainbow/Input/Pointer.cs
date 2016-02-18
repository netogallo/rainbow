// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Input
{
    using System.Runtime.InteropServices;

    /// <summary>
    /// A pointer event; managed counterpart of <c>rainbow::Pointer</c>.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Pointer
    {
        private readonly uint hash;
        private readonly int x;
        private readonly int y;
        private readonly ulong timestamp;

        public uint Hash => hash;

        public int X => x;

        public int Y => y;

        public ulong Timestamp => timestamp;
    }
}
