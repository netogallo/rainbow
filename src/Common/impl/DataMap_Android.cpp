// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/DataMap.h"

#include <android/asset_manager.h>

#include "FileSystem/Path.h"

using rainbow::DataMapAndroid;
using rainbow::filesystem::Path;

DataMapAndroid::DataMapAndroid(const Path& path)
    : off_(0), asset_(File::open(path))
{
}

auto DataMapAndroid::data() const -> const uint8_t*
{
    return static_cast<const uint8_t*>(AAsset_getBuffer(asset_)) + off_;
}

auto DataMapAndroid::size() const -> size_t
{
    return AAsset_getLength(asset_);
}
