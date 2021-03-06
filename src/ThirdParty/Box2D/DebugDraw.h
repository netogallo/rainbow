// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_DEBUGDRAW_H_
#define THIRDPARTY_BOX2D_DEBUGDRAW_H_

#include <vector>

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
#include <Box2D/Common/b2Draw.h>
#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif

namespace b2
{
    class DebuggableWorld
    {
    public:
        DebuggableWorld() : ptm_(32.0f) {}

        auto GetPTM() const { return ptm_; }
        void SetPTM(float ptm) { ptm_ = ptm; }

        virtual void DrawDebugData() = 0;

    protected:
        float ptm_;
    };

    class DebugDraw : public b2Draw
    {
    public:
        static void Draw();

        DebugDraw();

        void Add(DebuggableWorld*);
        void Remove(DebuggableWorld*);

        void DrawAllWorlds();

        // Implement b2Draw.

        void DrawPolygon(const b2Vec2* vertices,
                         int32 vertex_count,
                         const b2Color& color) override;
        void DrawSolidPolygon(const b2Vec2* vertices,
                              int32 vertex_count,
                              const b2Color& color) override;
        void DrawCircle(const b2Vec2& center,
                        float32 radius,
                        const b2Color& color) override;
        void DrawSolidCircle(const b2Vec2& center,
                             float32 radius,
                             const b2Vec2& axis,
                             const b2Color& color) override;
        void DrawSegment(const b2Vec2& p1,
                         const b2Vec2& p2,
                         const b2Color& color) override;
        void DrawTransform(const b2Transform& xf) override;

    private:
        static const size_t kMaxNumWorlds = 6;

        struct Vertex
        {
            b2Color color;
            b2Vec2 vertex;

            Vertex(const b2Color& c, const b2Vec2& v) : color(c), vertex(v) {}
        };

        float32 ptm_;
        std::vector<Vertex> lines_;
        std::vector<Vertex> triangles_;
        DebuggableWorld* worlds_[kMaxNumWorlds];
    };
}

#endif
