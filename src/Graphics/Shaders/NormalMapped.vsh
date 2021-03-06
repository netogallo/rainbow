// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

//#version 100

#ifdef GL_ES
#   ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#   else
precision mediump float;
#   endif
#else
#   define lowp
#endif

uniform mat4 mvp_matrix;

attribute vec4 color;
attribute vec2 normal;
attribute vec2 texcoord;
attribute vec2 vertex;

varying lowp vec4 v_color;
varying vec2 v_normal;
varying vec2 v_texcoord;

void main()
{
    v_color = color;
    v_normal = normal;
    v_texcoord = texcoord;
    gl_Position = mvp_matrix * vec4(vertex, 0.0, 1.0);
}
