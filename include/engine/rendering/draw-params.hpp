#pragma once

#include <engine/core/common.hpp>

#include <GL/glew.h>

struct DrawParams {
    enum BlendFunc {
        BLEND_FUNC_NONE,
        BLEND_FUNC_ZERO = GL_ZERO,
        BLEND_FUNC_ONE = GL_ONE,
        BLEND_FUNC_SRC_ALPHA = GL_SRC_ALPHA,
        BLEND_FUNC_ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
        BLEND_FUNC_ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA,
        BLEND_FUNC_DST_ALPHA = GL_DST_ALPHA
    };

    enum BufferUsage {
        USAGE_STATIC_DRAW = GL_STATIC_DRAW,
        USAGE_STREAM_DRAW = GL_STREAM_DRAW,
        USAGE_DYNAMIC_DRAW = GL_DYNAMIC_DRAW,

        USAGE_STATIC_COPY = GL_STATIC_COPY,
        USAGE_STREAM_COPY = GL_STREAM_COPY,
        USAGE_DYNAMIC_COPY = GL_DYNAMIC_COPY,

        USAGE_STATIC_READ = GL_STATIC_READ,
        USAGE_STREAM_READ = GL_STREAM_READ,
        USAGE_DYNAMIC_READ = GL_DYNAMIC_READ,
    };

    enum SamplerFilter {
        FILTER_NEAREST = GL_NEAREST,
        FILTER_LINEAR = GL_LINEAR,
        FILTER_NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
        FILTER_LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
        FILTER_NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
        FILTER_LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
    };

    enum SamplerWrapMode {
        WRAP_CLAMP = GL_CLAMP_TO_EDGE,
        WRAP_REPEAT = GL_REPEAT,
        WRAP_CLAMP_MIRROR = GL_MIRROR_CLAMP_TO_EDGE,
        WRAP_REPEAT_MIRROR = GL_MIRRORED_REPEAT,
    };

    enum PixelFormat {
        FORMAT_R,
        FORMAT_RG,
        FORMAT_RGB,
        FORMAT_RGBA,
        FORMAT_DEPTH,
        FORMAT_DEPTH_AND_STENCIL,
    };

    enum FaceCullMode {
        FACE_CULL_NONE,
        FACE_CULL_BACK = GL_BACK,
        FACE_CULL_FRONT = GL_FRONT,
        FACE_CULL_FRONT_AND_BACK = GL_FRONT_AND_BACK,
    };

    enum DrawFunc {
        DRAW_FUNC_NEVER = GL_NEVER,
        DRAW_FUNC_ALWAYS = GL_ALWAYS,
        DRAW_FUNC_LESS = GL_LESS,
        DRAW_FUNC_GREATER = GL_GREATER,
        DRAW_FUNC_LEQUAL = GL_LEQUAL,
        DRAW_FUNC_GEQUAL = GL_GEQUAL,
        DRAW_FUNC_EQUAL = GL_EQUAL,
        DRAW_FUNC_NOT_EQUAL = GL_NOTEQUAL,
    };

    enum FramebufferAttachment {
        ATTACHMENT_COLOR = GL_COLOR_ATTACHMENT0,
        ATTACHMENT_DEPTH = GL_DEPTH_ATTACHMENT,
        ATTACHMENT_STENCIL = GL_STENCIL_ATTACHMENT,
    };

    enum StencilOp {
        STENCIL_KEEP = GL_KEEP,
        STENCIL_ZERO = GL_ZERO,
        STENCIL_REPLACE = GL_REPLACE,
        STENICL_INCR = GL_INCR,
        STENCIL_INCR_WRAP = GL_INCR_WRAP,
        STENCIL_DECR_WRAP = GL_DECR_WRAP,
        STENCIL_DECR = GL_DECR,
        STENCIL_INVERT = GL_INVERT,
    };

    enum CubeMapFace {
        CUBE_MAP_POSITIVE_X = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        CUBE_MAP_NEGATIVE_X = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        CUBE_MAP_POSITIVE_Y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        CUBE_MAP_NEGATIVE_Y	= GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        CUBE_MAP_POSITIVE_Z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        CUBE_MAP_NEGATIVE_Z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    enum TextureType {
        TEXTURE_TYPE_2D = GL_TEXTURE_2D,
        TEXTURE_TYPE_CUBE_MAP = GL_TEXTURE_CUBE_MAP
    };

    enum FeedbackBufferMode {
        BUFFER_INTERLEAVED_ATTRIBS = GL_INTERLEAVED_ATTRIBS,
        BUFFER_SEPARATE_ATTRIBS = GL_SEPARATE_ATTRIBS
    };

    enum FaceCullMode faceCullMode = FACE_CULL_NONE;
    enum DrawFunc depthFunc = DRAW_FUNC_ALWAYS;

    enum BlendFunc sourceBlend = BLEND_FUNC_NONE;
    enum BlendFunc destBlend = BLEND_FUNC_NONE;

    bool writeDepth = true;
    bool stencilTest = false;
    bool scissorTest = false;
    bool discardRasterizer = false;

    uint32 numDrawBuffers = 1;

    enum DrawFunc stencilFunc = DRAW_FUNC_ALWAYS;
    uint32 stencilTestMask = 0;
    uint32 stencilWriteMask = 0;
    int32 stencilComparisonVal = 0;
    enum StencilOp stencilFail = STENCIL_KEEP;
    enum StencilOp stencilPassDepthFail = STENCIL_KEEP;
    enum StencilOp stencilPass = STENCIL_KEEP;

    uint32 scissorStartX = 0;
    uint32 scissorStartY = 0;
    uint32 scissorWidth = 0;
    uint32 scissorHeight = 0;
};
