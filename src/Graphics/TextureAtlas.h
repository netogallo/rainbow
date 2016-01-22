// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTUREATLAS_H_
#define GRAPHICS_TEXTUREATLAS_H_

#include <vector>

#include "Common/DataMap.h"
#include "Graphics/Texture.h"
#include "Memory/SharedPtr.h"

class TextureManager;

/// <summary>Texture atlas loaded from an image file.</summary>
/// <remarks>
///   <list type="bullet">
///     <item>
///       Textures are loaded "upside-down", so the coordinates must be flipped.
///     </item>
///     <item>
///       iOS: Textures' dimension must be 2<sup>n</sup> by 2<sup>m</sup> for
///       some arbitrary n and m, where n, m > 6.
///     </item>
///   </list>
///   References
///   <list type="bullet">
///     <item>http://iphonedevelopment.blogspot.com/2009/05/opengl-es-from-ground-up-part-6_25.html</item>
///     <item>http://developer.android.com/guide/topics/resources/providing-resources.html</item>
///     <item>http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures</item>
///   </list>
/// </remarks>
class TextureAtlas : public RefCounted
{
public:
    explicit TextureAtlas(const char* path);
    explicit TextureAtlas(const rainbow::ISolemnlySwearThatIAmOnlyTesting& test)
        : texture_(test) {}

    auto height() const { return texture_.height(); }
    auto is_valid() const { return texture_; }
    auto size() const { return regions_.size(); }
    auto width() const { return texture_.width(); }

    /// <summary>Binds this texture.</summary>
    void bind() const { texture_.bind(); }
    void bind(unsigned int unit) const { texture_.bind(unit); }

    /// <summary>Defines a texture region.</summary>
    /// <param name="origin">Starting point of the region.</param>
    /// <param name="width">Width of the region.</param>
    /// <param name="height">Height of the region.</param>
    /// <returns>The id of the region.</returns>
    auto define(const Vec2i& origin, int width, int height) -> unsigned int;

    /// <summary>Trims the internal texture region storage.</summary>
    void trim() { regions_.shrink_to_fit(); }

    auto operator[](unsigned int i) const -> const rainbow::TextureRegion&
    {
        return regions_[i];
    }

private:
    rainbow::Texture texture_;                     ///< Texture atlas' id.
    std::vector<rainbow::TextureRegion> regions_;  ///< Defined texture regions.

    void load(TextureManager& texture_manager,
              const rainbow::Texture& texture,
              const DataMap& data);
};

#endif
