#pragma once

#include <cstdint>

#include "dxso_common.h"
#include "dxso_decoder.h"

#include "../d3d9/d3d9_caps.h"

namespace dxvk {

  enum class DxsoBindingType : uint32_t {
    ConstantBuffer,
    Image,
  };

  enum class DxsoConstantBufferType : uint32_t {
    Float,
    Int,
    Bool
  };

  enum DxsoConstantBuffers : uint32_t {
    VSConstantBuffer = 0,
    VSFloatConstantBuffer = 0,
    VSIntConstantBuffer = 1,
    VSBoolConstantBuffer = 2,
    VSClipPlanes     = 3,
    VSFixedFunction  = 4,
    VSVertexBlendData = 5,
    VSCount,

    PSConstantBuffer = 0,
    PSFixedFunction  = 1,
    PSShared         = 2,
    PSCount
  };

  constexpr uint32_t computeResourceSlotId(
        DxsoProgramType shaderStage,
        DxsoBindingType bindingType,
        uint32_t        bindingIndex) {
    const uint32_t stageOffset = (DxsoConstantBuffers::VSCount + caps::MaxTexturesVS) * uint32_t(shaderStage);

    if (bindingType == DxsoBindingType::ConstantBuffer)
      return bindingIndex + stageOffset;
    else // if (bindingType == DxsoBindingType::Image)
      return bindingIndex + stageOffset + (shaderStage == DxsoProgramType::PixelShader ? DxsoConstantBuffers::PSCount : DxsoConstantBuffers::VSCount);
  }

  // TODO: Intergrate into compute resource slot ID/refactor all of this?
  constexpr uint32_t getSWVPBufferSlot() {
    return DxsoConstantBuffers::VSCount + caps::MaxTexturesVS + DxsoConstantBuffers::PSCount + caps::MaxTexturesPS + 1; // From last pixel shader slot, above.
  }

  // Split sampler slots: variant = texture type (2D, 3D, cube), plus 3 if
  // depth compared. Variant 0 keeps the regular slot, the rest follow SWVP.
  constexpr uint32_t SamplerSlotVariants = 6;

  constexpr uint32_t computeSamplerSlotId(
        DxsoProgramType shaderStage,
        uint32_t        samplerIndex,
        uint32_t        variant) {
    if (!variant)
      return computeResourceSlotId(shaderStage, DxsoBindingType::Image, samplerIndex);

    const uint32_t stride    = SamplerSlotVariants - 1;
    const uint32_t stageBase = shaderStage == DxsoProgramType::PixelShader
      ? caps::MaxTexturesVS * stride : 0u;

    return getSWVPBufferSlot() + 1 + stageBase + samplerIndex * stride + variant - 1;
  }

  uint32_t RegisterLinkerSlot(DxsoSemantic semantic);

}
