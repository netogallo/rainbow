// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/SpriteBatch.h"

namespace
{
	const unsigned int kStaleBuffer       = 1u << 0;
	const unsigned int kStalePosition     = 1u << 1;
	const unsigned int kStaleFrontBuffer  = 1u << 2;
	const unsigned int kStaleMask         = 0xffffu;
	const unsigned int kIsHidden          = 1u << 16;
	const unsigned int kIsMirrored        = 1u << 17;

	Vec2f transform_srt(const Vec2f &p,
	                    const Vec2f &s_sin_r,
	                    const Vec2f &s_cos_r,
	                    const Vec2f &center)
	{
		return Vec2f(s_cos_r.x * p.x - s_sin_r.y * p.y + center.x,
		             s_sin_r.x * p.x + s_cos_r.y * p.y + center.y);
	}

	Vec2f transform_st(const Vec2f &p, const Vec2f &scale, const Vec2f &center)
	{
		return Vec2f(scale.x * p.x + center.x, scale.y * p.y + center.y);
	}
}

Sprite& Sprite::Ref::operator*() const
{
	return batch_->sprites()[i_];
}

Sprite* Sprite::Ref::operator->() const
{
	return &batch_->sprites()[i_];
}

Sprite::Sprite(const unsigned int w, const unsigned int h, const SpriteBatch *p)
    : width_(w), height_(h), state_(0), angle_(0.0f), pivot_(0.5f, 0.5f),
      scale_(1.0f, 1.0f), parent_(p), vertex_array_(nullptr),
      normal_map_(nullptr) {}

Sprite::Sprite(Sprite&& s)
    : width_(s.width_), height_(s.height_), state_(s.state_), angle_(s.angle_),
      pivot_(s.pivot_), center_(s.center_), position_(s.position_),
      scale_(s.scale_), parent_(s.parent_), vertex_array_(s.vertex_array_),
      normal_map_(s.normal_map_)
{
	s.vertex_array_ = nullptr;
	s.parent_ = nullptr;
}

bool Sprite::is_hidden() const
{
	return (state_ & kIsHidden) == kIsHidden;
}

bool Sprite::is_mirrored() const
{
	return (state_ & kIsMirrored) == kIsMirrored;
}

void Sprite::set_color(const Colorb c)
{
	R_ASSERT(vertex_array_, "Missing vertex array buffer");

	vertex_array_[0].color = c;
	vertex_array_[1].color = c;
	vertex_array_[2].color = c;
	vertex_array_[3].color = c;
	state_ |= kStaleFrontBuffer;
}

void Sprite::set_normal(const unsigned int id)
{
	R_ASSERT(normal_map_, "Missing normal map buffer");

	const Texture &normal = parent_->normal()[id];
	if (state_ & kIsMirrored)
	{
		normal_map_[0] = normal.vx[1];
		normal_map_[1] = normal.vx[0];
		normal_map_[2] = normal.vx[3];
		normal_map_[3] = normal.vx[2];
	}
	else
	{
		normal_map_[0] = normal.vx[0];
		normal_map_[1] = normal.vx[1];
		normal_map_[2] = normal.vx[2];
		normal_map_[3] = normal.vx[3];
	}
	state_ |= kStaleFrontBuffer;
}

void Sprite::set_pivot(const Vec2f &pivot)
{
	R_ASSERT(pivot.x >= 0.0f && pivot.x <= 1.0f &&
	         pivot.y >= 0.0f && pivot.y <= 1.0f,
	         "Invalid pivot point");

	Vec2f diff = pivot;
	diff -= pivot_;
	if (diff.is_zero())
		return;

	diff.x *= width_ * scale_.x;
	diff.y *= height_ * scale_.y;
	center_ += diff;
	position_ += diff;
	pivot_ = pivot;
}

void Sprite::set_position(const Vec2f &position)
{
	position_ = position;
	state_ |= kStalePosition;
}

void Sprite::set_rotation(const float r)
{
	angle_ = r;
	state_ |= kStaleBuffer;
}

void Sprite::set_scale(const Vec2f &f)
{
	R_ASSERT(f.x > 0.0f && f.y > 0.0f,
	         "Can't scale with a factor of zero or less");

	scale_ = f;
	state_ |= kStaleBuffer;
}

void Sprite::set_texture(const unsigned int id)
{
	R_ASSERT(vertex_array_, "Missing vertex array buffer");

	const Texture &tx = parent_->texture()[id];
	if (state_ & kIsMirrored)
	{
		vertex_array_[0].texcoord = tx.vx[1];
		vertex_array_[1].texcoord = tx.vx[0];
		vertex_array_[2].texcoord = tx.vx[3];
		vertex_array_[3].texcoord = tx.vx[2];
	}
	else
	{
		vertex_array_[0].texcoord = tx.vx[0];
		vertex_array_[1].texcoord = tx.vx[1];
		vertex_array_[2].texcoord = tx.vx[2];
		vertex_array_[3].texcoord = tx.vx[3];
	}
	state_ |= kStaleFrontBuffer;
}

void Sprite::set_vertex_array(SpriteVertex *array)
{
	vertex_array_ = array;
	state_ |= kStaleMask;
}

void Sprite::hide()
{
	if (is_hidden())
		return;

	vertex_array_[0].position = Vec2f::Zero;
	vertex_array_[1].position = Vec2f::Zero;
	vertex_array_[2].position = Vec2f::Zero;
	vertex_array_[3].position = Vec2f::Zero;
	state_ |= kIsHidden | kStaleFrontBuffer;
}

void Sprite::mirror()
{
	R_ASSERT(vertex_array_, "Missing vertex array buffer");

	std::swap(vertex_array_[0].texcoord, vertex_array_[1].texcoord);
	std::swap(vertex_array_[2].texcoord, vertex_array_[3].texcoord);
	if (normal_map_)
	{
		std::swap(normal_map_[0], normal_map_[1]);
		std::swap(normal_map_[2], normal_map_[3]);
	}
	state_ |= kStaleFrontBuffer;
	state_ ^= kIsMirrored;
}

void Sprite::move(const Vec2f &delta)
{
	position_ += delta;
	state_ |= kStalePosition;
}

void Sprite::rotate(const float r)
{
	angle_ += r;
	state_ |= kStaleBuffer;
}

void Sprite::show()
{
	if (!is_hidden())
		return;

	state_ &= ~kIsHidden;
	state_ |= kStaleMask;
}

bool Sprite::update()
{
	if ((state_ & kStaleMask) == 0)
		return false;

	if (is_hidden())
	{
		state_ &= ~kStaleMask;
		return true;
	}

	R_ASSERT(vertex_array_, "Missing vertex array buffer");

	if (state_ & kStaleBuffer)
	{
		if (state_ & kStalePosition)
			center_ = position_;

		Vec2f origin[4];
		origin[0].x = width_ * -pivot_.x;
		origin[0].y = height_ * -(1 - pivot_.y);
		origin[1].x = origin[0].x + width_;
		origin[1].y = origin[0].y;
		origin[2].x = origin[1].x;
		origin[2].y = origin[1].y + height_;
		origin[3].x = origin[0].x;
		origin[3].y = origin[2].y;

		if (!rainbow::is_equal(angle_, 0.0f))
		{
			const float cos_r = cosf(-angle_);
			const float sin_r = sinf(-angle_);

			const Vec2f s_sin_r(scale_.x * sin_r, scale_.y * sin_r);
			const Vec2f s_cos_r(scale_.x * cos_r, scale_.y * cos_r);

			vertex_array_[0].position =
			    transform_srt(origin[0], s_sin_r, s_cos_r, center_);
			vertex_array_[1].position =
			    transform_srt(origin[1], s_sin_r, s_cos_r, center_);
			vertex_array_[2].position =
			    transform_srt(origin[2], s_sin_r, s_cos_r, center_);
			vertex_array_[3].position =
			    transform_srt(origin[3], s_sin_r, s_cos_r, center_);
		}
		else
		{
			vertex_array_[0].position =
			    transform_st(origin[0], scale_, center_);
			vertex_array_[1].position =
			    transform_st(origin[1], scale_, center_);
			vertex_array_[2].position =
			    transform_st(origin[2], scale_, center_);
			vertex_array_[3].position =
			    transform_st(origin[3], scale_, center_);
		}
	}
	else if (state_ & kStalePosition)
	{
		position_ -= center_;
		vertex_array_[0].position += position_;
		vertex_array_[1].position += position_;
		vertex_array_[2].position += position_;
		vertex_array_[3].position += position_;
		center_ += position_;
		position_ = center_;
	}
	state_ &= ~kStaleMask;
	return true;
}
