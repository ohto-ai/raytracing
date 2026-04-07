#pragma once
// render.hh
// Unified rendering interface for both the CPU (render.cpp) and
// CUDA (render.cu) back-ends.

#ifndef _OHTOAI_RENDER_H_
#define _OHTOAI_RENDER_H_

#include "cuda_scene.hh"
#include <cstdint>

namespace ohtoai {
    namespace math {

        /// CPU multi-threaded ray-tracing renderer.
        /// @param cam_params  Pre-computed camera geometry.
        /// @param host_scene  Flat scene description (host memory).
        /// @param host_output Caller-allocated RGBA buffer,
        ///                    size = image_width * image_height * 4 bytes.
        void render_cpu(const CudaCameraParams& cam_params,
                        const CudaScene&        host_scene,
                        uint8_t*                host_output);

#ifdef OHTOAI_ENABLE_CUDA
        /// CUDA ray-tracing renderer.
        /// Identical algorithm to render_cpu; parallelism provided by GPU threads.
        /// @param cam_params  Pre-computed camera geometry.
        /// @param host_scene  Flat scene description (host memory – copied to device).
        /// @param host_output Caller-allocated RGBA buffer,
        ///                    size = image_width * image_height * 4 bytes.
        void render_cuda(const CudaCameraParams& cam_params,
                         const CudaScene&        host_scene,
                         uint8_t*                host_output);
#endif // OHTOAI_ENABLE_CUDA

    } // namespace math
} // namespace ohtoai

#endif // !_OHTOAI_RENDER_H_
