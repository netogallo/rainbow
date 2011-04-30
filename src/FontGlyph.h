/// Font glyph structure for storing advance and a textured sprite.

/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef FONTGLYPH_H_
#define FONTGLYPH_H_

#include "Common/SpriteVertex.h"

struct FontGlyph
{
	unsigned short int advance;
	short int left;
	#ifdef FONTATLAS_KERNING
		short int kern[95];
	#endif
	SpriteVertex quad[4];
};

#endif
