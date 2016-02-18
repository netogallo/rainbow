// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using Interop;

    /// <summary>Font atlas loaded from file.</summary>
    public sealed partial class FontAtlas
    {
        public FontAtlas(string path, float pt)
        {
            NativeHandle = ctor(path.GetUTF8Bytes(), pt);
            IsValid = isValid(NativeHandle);
            LineHeight = getHeight(NativeHandle);
        }

        public bool IsValid { get; }

        public int LineHeight { get; }
    }
}
