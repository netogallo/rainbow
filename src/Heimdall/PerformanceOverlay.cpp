// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_HEIMDALL

#include "Heimdall/PerformanceOverlay.h"

#include <algorithm>

#include "Graphics/Renderer.h"
#include "Graphics/SceneGraph.h"
#include "Heimdall/Style.h"

#if defined(_MSC_VER) && _MSC_VER <= 1800
#	define snprintf(...) sprintf_s(__VA_ARGS__)
#endif

namespace
{
	const int k1024ToPowerOf[]{1, 1024, 1048576, 1073741824};
	const size_t kFrameDataLength = 600;
	const char kStringAxisLabels[] =
	    "ms   %cB\n67  %3d\n50  %3d\n33  %3d\n16  %3d";
	const char kStringAxisLabelsF[] =
	    "ms   %cB\n67  %.1f\n50  %.1f\n33  %.1f\n16  %.1f";
	const char kStringAxisLabelsM[] = "MGTPEZY";
	const char kStringHidePerfData[] = "Hide performance data";
	const char kStringShowPerfData[] = "Show performance data";

	float spacing(const Vec2i& res)
	{
		return res.y / 20;
	}

	Colorb colorb(const NVGcolor &c)
	{
		return Colorb(c.r * 255, c.g * 255, c.b * 255, c.a * 255);
	}

	NVGcolor frame_line_color()
	{
		return nvgRGBf(0.12941176f, 0.58823529f, 0.95294118f);  // #2196f3
	}

	NVGcolor graph_line_color()
	{
		return nvgRGBAf(1.0f, 1.0f, 1.0f, 0.8f);
	}

	NVGcolor vmem_line_color()
	{
		return nvgRGBf(0.29803921f, 0.68627451f, 0.31372549f);  // #4caf50
	}

	template<size_t I, typename T>
	void plot(NVGcontext *vg,
	          float x1,
	          const float y0,
	          const float dx,
	          const float dy,
	          const NVGcolor color,
	          const T &data)
	{
		nvgBeginPath(vg);
		nvgMoveTo(vg, x1, y0 - std::get<I>(data.front()) * dy);
		for (auto&& p : data)
		{
			nvgLineTo(vg, x1, y0 - std::get<I>(p) * dy);
			x1 -= dx;
		}
		nvgStrokeColor(vg, color);
		nvgStroke(vg);
	}
}

namespace heimdall
{
	PerformanceOverlay::PerformanceOverlay()
	    : node_(new rainbow::SceneNode()), vmem_top_(0.0f), perf_graph_(nvg_)
	{
		node_->enabled = false;
	}

	void PerformanceOverlay::init_button(const Vec2f &p,
	                                     SharedPtr<FontAtlas> font)
	{
		button_.set_color(color::InactiveFont());
		button_.set_font(std::move(font));
		button_.set_position(p);
		button_.set_text(kStringShowPerfData);
		button_.set_action([this] {
			node_->enabled = !node_->enabled;
			if (node_->enabled)
			{
				button_.set_color(color::NormalFont());
				button_.set_text(kStringHidePerfData);
			}
			else
			{
				button_.set_color(color::InactiveFont());
				button_.set_text(kStringShowPerfData);
			}
		});
	}

	void PerformanceOverlay::init_graph(SharedPtr<FontAtlas> font)
	{
		const Vec2i& res = Renderer::Get()->resolution();
		const float x = spacing(res);
		const float x0 = res.x - x * 3 + x / 5;
		const float y0 = x + font->height() * (2.0f / 3.0f);
		const float y1 = y0 + x * 4;

		labels_.set_color(colorb(frame_line_color()));
		labels_.set_font(font);
		labels_.set_position(Vec2f(x0, y1));
		labels_.set_text(kStringAxisLabels);

		const Colorb &color = colorb(vmem_line_color());
		for (int i = 0; i <= 4; ++i)
			labels_.set_color(color, i * 7 + 4, 3);

		const Vec2i offset(0, font->height() - x);
		for (int i = 1; i < 4; ++i)
			labels_.set_offset(i * offset, i * 7 + 7, 7);

		node_->add_child(labels_.as_label())
		     ->add_child(&perf_graph_);
	}

	void PerformanceOverlay::update(const unsigned long dt)
	{
		if (!node_->enabled)
			return;

		if (frame_data_.size() == kFrameDataLength)
			frame_data_.pop_back();
		const auto &usage = TextureManager::Get()->memory_usage();
		frame_data_.emplace_front(dt, usage.used);

		const auto &data = frame_data_;
		const float vmem_top = rainbow::next_pow2(std::ceil(usage.peak));
		nvg_.enqueue([&data, vmem_top](NVGcontext *vg) {
			const Vec2i& res = Renderer::Get()->resolution();
			const float x = spacing(res);
			const float width = res.x - x * 4;
			const float height = x * 4;
			const float y = res.y - x - height;

			nvgBeginPath(vg);
			nvgRect(vg, x, y, width, height);
			for (int i = 1; i < 4; ++i)
			{
				const float y1 = y + x * i;
				nvgMoveTo(vg, x, y1);
				nvgLineTo(vg, x + width, y1);
			}
			nvgStrokeColor(vg, graph_line_color());
			nvgStrokeWidth(vg, 2.0f);
			nvgStroke(vg);

			const float x1 = x + width - 1;
			const float by = y + height;
			const float dx = width / static_cast<float>(kFrameDataLength);
			plot<0>(vg, x1, by, dx, x / 16, frame_line_color(), data);
			plot<1>(vg, x1, by, dx, height / vmem_top, vmem_line_color(), data);
		});

		if (vmem_top != vmem_top_)
		{
			const int order = std::min<int>(
			    (vmem_top == 0 ? 0 : std::floor(std::log10(vmem_top)) / 3),
			    rainbow::array_size(k1024ToPowerOf) - 1);
			const float dy = vmem_top / (k1024ToPowerOf[order] * 4);
			if (dy < 1.0f)
			{
				snprintf(const_cast<char*>(labels_.text()),
				         sizeof(kStringAxisLabelsF),
				         kStringAxisLabelsF,
				         kStringAxisLabelsM[order],
				         dy * 4,
				         dy * 3,
				         dy * 2,
				         dy);
			}
			else
			{
				const int dY = static_cast<int>(dy);
				snprintf(const_cast<char*>(labels_.text()),
				         sizeof(kStringAxisLabels),
				         kStringAxisLabels,
				         kStringAxisLabelsM[order],
				         dY * 4,
				         dY * 3,
				         dY * 2,
				         dY);
			}
			labels_.set_needs_update(Label::kStaleBuffer);
			vmem_top_ = vmem_top;
		}
	}
}

#endif