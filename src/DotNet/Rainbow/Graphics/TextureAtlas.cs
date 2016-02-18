// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using Diagnostics;
    using Interop;

    /// <summary>Texture atlas loaded from an image.</summary>
    public sealed partial class TextureAtlas
    {
        public TextureAtlas(string path, float scale = 1.0f)
        {
            NativeHandle = ctor(path.GetUTF8Bytes(), scale);
            Height = getHeight(NativeHandle);
            IsValid = isValid(NativeHandle);
            Width = getWidth(NativeHandle);
        }

        public uint Count { get; private set; }

        public int Height { get; }

        public bool IsValid { get; }

        public int Width { get; }

        public uint this[int index]
        {
            get
            {
                Assert.True(index >= 0);
                Assert.True(index < Count);

                return getTextureRegion(NativeHandle, (uint)index);
            }
        }

        /// <summary>Adds a texture region.</summary>
        /// <param name="x">Starting point of the region (x-coordinate).</param>
        /// <param name="y">Starting point of the region (y-coordinate).</param>
        /// <param name="width">Width of the region.</param>
        /// <param name="height">Height of the region.</param>
        /// <returns>The id of the region.</returns>
        public uint AddRegion(int x, int y, int width, int height)
        {
            ++Count;
            return addRegion(NativeHandle, x, y, width, height);
        }

        /// <summary>
        /// Replaces the current set of texture regions with the set in the
        /// specified array.
        /// </summary>
        public void SetRegions(int[] rectangles)
        {
            Count = (uint)rectangles.Length;
            setRegions(NativeHandle, rectangles, rectangles.Length);
        }
    }
}
