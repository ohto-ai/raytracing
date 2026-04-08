#pragma once
// render.hh
// CUDA ray-tracing rendering interface.

#ifndef _OHTOAI_RENDER_H_
#define _OHTOAI_RENDER_H_

#include "cuda_scene.hh"
#include <cstdint>

namespace ohtoai {
    namespace math {

        /// CUDA ray-tracing renderer.
        /// Launches one GPU thread per pixel; all GPU-side arithmetic uses float
        /// for maximum throughput.
        /// @param cam_params  Pre-computed camera geometry (host memory).
        /// @param host_scene  Flat scene description (host memory — copied to device).
        /// @param host_output Caller-allocated RGBA buffer,
        ///                    size = image_width * image_height * 4 bytes.
        void render_cuda(const CudaCameraParams& cam_params,
                         const CudaScene&        host_scene,
                         uint8_t*                host_output);

    } // namespace math
} // namespace ohtoai

#endif // !_OHTOAI_RENDER_H_
