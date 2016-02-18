// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using Diagnostics;

    /// <summary>A drawable batch of sprites.</summary>
    public sealed partial class SpriteBatch
    {
        private TextureAtlas texture;
        private TextureAtlas normal;

        public SpriteBatch(uint size)
        {
            NativeHandle = ctor(size);
        }

        /// <summary>Returns sprite count.</summary>
        public uint Count => count(NativeHandle);

        /// <summary>Gets or sets whether the batch is visible.</summary>
        public bool IsVisible
        {
            get { return isVisible(NativeHandle); }
            set { setVisible(NativeHandle, value); }
        }

        /// <summary>Gets or sets a normal map.</summary>
        public TextureAtlas Normal
        {
            get { return normal; }
            set { SetValue(ref normal, value, setNormal); }
        }

        /// <summary>Gets or sets a texture atlas.</summary>
        public TextureAtlas Texture
        {
            get { return texture; }
            set { SetValue(ref texture, value, setTexture); }
        }

        public Sprite this[int index]
        {
            get
            {
                Assert.True(index >= 0);
                Assert.True(index < Count);

                return new Sprite(this, (uint)index);
            }
        }

        /// <summary>Brings sprite to front.</summary>
        /// <remarks>Invalidates all references.</remarks>
        public void BringToFront(Sprite sprite)
        {
            Assert.Equal(sprite.Parent, this);

            bringToFront(NativeHandle, sprite.Index);
        }

        /// <summary>Clears all sprites.</summary>
        public void Clear() => clear(NativeHandle);

        /// <summary>Creates a sprite.</summary>
        /// <param name="width">Width of the sprite.</param>
        /// <param name="height">Height of the sprite.</param>
        /// <returns>
        ///   Reference to the newly created sprite, positioned at (0,0).
        /// </returns>
        public Sprite CreateSprite(uint width, uint height)
        {
            uint sprite = createSprite(NativeHandle, width, height);
            return new Sprite(this, sprite);
        }

        /// <summary>Erases a sprite from the batch.</summary>
        /// <remarks>Invalidates all references.</remarks>
        public void Erase(Sprite sprite)
        {
            Assert.Equal(sprite.Parent, this);

            erase(NativeHandle, sprite.Index);
        }

        /// <summary>Returns the first sprite with the given id.</summary>
        public Sprite FindSpriteById(int id)
        {
            uint sprite = findSpriteById(NativeHandle, id);
            return sprite == uint.MaxValue
                       ? default(Sprite)
                       : new Sprite(this, sprite) { Id = id };
        }

        /// <summary>Moves all sprites by (x,y).</summary>
        public void Move(float x, float y) => move(NativeHandle, new Vector2(x, y));

        /// <summary>Swaps two sprites' positions in the batch.</summary>
        public void Swap(Sprite left, Sprite right)
        {
            Assert.Equal(left.Parent, this);
            Assert.Equal(right.Parent, this);

            swap(NativeHandle, left.Index, right.Index);
        }
    }
}
