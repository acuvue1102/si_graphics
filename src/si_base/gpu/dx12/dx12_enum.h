#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include "si_base/core/core.h"
#include "si_base/misc/bitwise.h"
#include "si_base/gpu/gfx_enum.h"

namespace SI
{
	inline D3D12_ROOT_PARAMETER_TYPE GetDx12RootParameterType(GfxRootDescriptorType type)
	{
		static const D3D12_ROOT_PARAMETER_TYPE kTable[] = 
		{
			D3D12_ROOT_PARAMETER_TYPE_CBV,
			D3D12_ROOT_PARAMETER_TYPE_SRV,
			D3D12_ROOT_PARAMETER_TYPE_UAV
		};
		static_assert(ArraySize(kTable) == (size_t)GfxRootDescriptorType::Max, "tableError");

		return kTable[(int)type];
	}

	inline D3D12_ROOT_DESCRIPTOR_FLAGS GetDx12RootDescriptorFlags(GfxRootDescriptorFlags flags)
	{
		static const D3D12_ROOT_DESCRIPTOR_FLAGS kTable[] = 
		{
			D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE,
			D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE,
			D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC
		};
		static_assert(((size_t)1<<(ArraySize(kTable))) == ((size_t)GfxRootDescriptorFlag::Max), "tableError");

		D3D12_ROOT_DESCRIPTOR_FLAGS dxFlags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
		uint32_t mask = flags.GetMask();
		while(mask != 0)
		{
			int msb = Bitwise::MSB32(mask);
			if(msb<0) break;
			if((int)ArraySize(kTable) <= msb) continue;

			dxFlags |= kTable[msb];
			mask &= ~((uint32_t)1<<(uint32_t)msb);
		}

		return dxFlags;
	}

	inline D3D12_SHADER_VISIBILITY GetDx12ShaderVisibility(GfxShaderVisibility v)
	{
		static const D3D12_SHADER_VISIBILITY kTable[] = 
		{
			D3D12_SHADER_VISIBILITY_ALL,
			D3D12_SHADER_VISIBILITY_VERTEX,
			D3D12_SHADER_VISIBILITY_HULL,
			D3D12_SHADER_VISIBILITY_DOMAIN,
			D3D12_SHADER_VISIBILITY_GEOMETRY,
			D3D12_SHADER_VISIBILITY_PIXEL
		};
		static_assert(ArraySize(kTable) == (size_t)GfxShaderVisibility::Max, "tableError");

		return kTable[(int)v];		
	}

	inline D3D12_DEPTH_WRITE_MASK GetDx12DepthWriteMask(GfxDepthWriteMask mask)
	{
		static const D3D12_DEPTH_WRITE_MASK kTable[] = 
		{
			D3D12_DEPTH_WRITE_MASK_ZERO,
			D3D12_DEPTH_WRITE_MASK_ALL
		};
		static_assert(ArraySize(kTable) == (size_t)GfxDepthWriteMask::Max, "tableError");

		return kTable[(int)mask];		
	}

	inline D3D12_FILL_MODE GetDx12FillMode(GfxFillMode mode)
	{
		static const D3D12_FILL_MODE kTable[] = 
		{
			D3D12_FILL_MODE_WIREFRAME,
			D3D12_FILL_MODE_SOLID,
		};
		static_assert(ArraySize(kTable) == (size_t)GfxFillMode::Max, "tableError");

		return kTable[(int)mode];
	}
	
	
	inline D3D12_CULL_MODE GetDx12CullMode(GfxCullMode mode)
	{
		static const D3D12_CULL_MODE kTable[] = 
		{
			D3D12_CULL_MODE_NONE,
			D3D12_CULL_MODE_FRONT,
			D3D12_CULL_MODE_BACK,
		};
		static_assert(ArraySize(kTable) == (size_t)GfxCullMode::Max, "tableError");

		return kTable[(int)mode];
	}

	inline D3D12_PRIMITIVE_TOPOLOGY_TYPE GetDx12PrimitiveTopologyType(GfxPrimitiveTopologyType topologyType)
	{
		static const D3D12_PRIMITIVE_TOPOLOGY_TYPE kTable[] = 
		{
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH
		};
		static_assert(ArraySize(kTable) == (size_t)GfxPrimitiveTopologyType::Max, "tableError");

		return kTable[(int)topologyType];
	}

	inline uint8_t GetDx12RenderTargetWriteMask(GfxColorWriteFlags writeFlags)
	{
		uint8_t mask = 0;
		if(writeFlags&GfxColorWriteFlag::Red)   mask |= D3D12_COLOR_WRITE_ENABLE_RED;
		if(writeFlags&GfxColorWriteFlag::Green) mask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
		if(writeFlags&GfxColorWriteFlag::Blue)  mask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
		if(writeFlags&GfxColorWriteFlag::Alpha) mask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;

		return mask;
	}

	inline D3D12_LOGIC_OP GetDx12LogicOp(GfxLogicOp logicOp)
	{
		static const D3D12_LOGIC_OP kTable[] = 
		{
			D3D12_LOGIC_OP_CLEAR,
			D3D12_LOGIC_OP_SET,
			D3D12_LOGIC_OP_COPY,
			D3D12_LOGIC_OP_COPY_INVERTED,
			D3D12_LOGIC_OP_NOOP,
			D3D12_LOGIC_OP_INVERT,
			D3D12_LOGIC_OP_AND,
			D3D12_LOGIC_OP_NAND,
			D3D12_LOGIC_OP_OR,
			D3D12_LOGIC_OP_NOR,
			D3D12_LOGIC_OP_XOR,
			D3D12_LOGIC_OP_EQUIV,
			D3D12_LOGIC_OP_AND_REVERSE,
			D3D12_LOGIC_OP_AND_INVERTED,
			D3D12_LOGIC_OP_OR_REVERSE,
			D3D12_LOGIC_OP_OR_INVERTED,
		};
		static_assert(ArraySize(kTable) == (size_t)GfxLogicOp::Max, "tableError");

		return kTable[(int)logicOp];
	}

	inline D3D12_BLEND GetDx12Blend(GfxBlend blend)
	{
		static const D3D12_BLEND kTable[] = 
		{
			D3D12_BLEND_ZERO,
			D3D12_BLEND_ONE,
			D3D12_BLEND_SRC_COLOR,
			D3D12_BLEND_INV_SRC_COLOR,
			D3D12_BLEND_SRC_ALPHA,
			D3D12_BLEND_INV_SRC_ALPHA,
			D3D12_BLEND_DEST_ALPHA,
			D3D12_BLEND_INV_DEST_ALPHA,
			D3D12_BLEND_DEST_COLOR,
			D3D12_BLEND_INV_DEST_COLOR,
			D3D12_BLEND_SRC_ALPHA_SAT,
			D3D12_BLEND_BLEND_FACTOR,
			D3D12_BLEND_INV_BLEND_FACTOR,
			D3D12_BLEND_SRC1_COLOR,
			D3D12_BLEND_INV_SRC1_COLOR,
			D3D12_BLEND_SRC1_ALPHA,
			D3D12_BLEND_INV_SRC1_ALPHA,
		};
		static_assert(ArraySize(kTable) == (size_t)GfxBlend::Max, "tableError");

		return kTable[(int)blend];
	}

	inline D3D12_BLEND_OP GetDx12BlendOp(GfxBlendOp blendOp)
	{
		static const D3D12_BLEND_OP kTable[] = 
		{
			D3D12_BLEND_OP_ADD,
			D3D12_BLEND_OP_SUBTRACT,
			D3D12_BLEND_OP_REV_SUBTRACT,
			D3D12_BLEND_OP_MIN,
			D3D12_BLEND_OP_MAX,
		};
		static_assert(ArraySize(kTable) == (size_t)GfxBlendOp::EnumMax, "tableError");

		return kTable[(int)blendOp];
	}

	inline D3D12_RESOURCE_FLAGS GetDx12ResourceFlags(GfxResourceFlags flags)
	{
		static const D3D12_RESOURCE_FLAGS kTable[] = 
		{
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE,
			D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER,
			D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS,
			D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY
		};
		static_assert(((size_t)1<<(ArraySize(kTable))) == ((size_t)GfxResourceFlag::Max), "tableError");

		D3D12_RESOURCE_FLAGS dxFlags = D3D12_RESOURCE_FLAG_NONE;
		uint32_t mask = flags.GetMask();
		while(mask != 0)
		{
			int msb = Bitwise::MSB32(mask);
			if(msb<0) break;
			if((int)ArraySize(kTable) <= msb) continue;

			dxFlags |= kTable[msb];
			mask &= ~((uint32_t)1<<(uint32_t)msb);
		}

		return dxFlags;
	}

	inline D3D12_RESOURCE_STATES GetDx12ResourceStates(GfxResourceStates states)
	{
		static const D3D12_RESOURCE_STATES kTable[] = 
		{
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
			D3D12_RESOURCE_STATE_INDEX_BUFFER,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_RESOURCE_STATE_DEPTH_READ,
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_STREAM_OUT,
			D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_STATE_RESOLVE_DEST,
			D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_STATE_PREDICATION,
			D3D12_RESOURCE_STATE_VIDEO_DECODE_READ,
			D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE,
			D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ,
			D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE,
		};
		static_assert(((size_t)1<<(ArraySize(kTable))) == ((size_t)GfxResourceState::Max), "tableError");

		D3D12_RESOURCE_STATES state = (D3D12_RESOURCE_STATES)0;
		uint32_t mask = states.GetMask();
		while(mask != 0)
		{
			int msb = Bitwise::MSB32(mask);
			if(msb<0) break;
			if((int)ArraySize(kTable) <= msb) continue;

			state |= kTable[msb];
			mask &= ~((uint32_t)1<<(uint32_t)msb);
		}

		return state;
	}

	inline D3D12_RESOURCE_BARRIER_FLAGS GetDx12ResourceBarrierFlag(GfxResourceBarrierFlag f)
	{
		static const D3D12_RESOURCE_BARRIER_FLAGS kTable[] =
		{
			D3D12_RESOURCE_BARRIER_FLAG_NONE,
			D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY,
			D3D12_RESOURCE_BARRIER_FLAG_END_ONLY
		};
		static_assert(ArraySize(kTable) == (size_t)GfxResourceBarrierFlag::Max, "tableError");

		return kTable[(int)f];
	}

	inline D3D12_COMPARISON_FUNC GetDx12ComparisonFunc(GfxComparisonFunc f)
	{
		static const D3D12_COMPARISON_FUNC kTable[] =
		{
			D3D12_COMPARISON_FUNC_NEVER,
			D3D12_COMPARISON_FUNC_LESS,
			D3D12_COMPARISON_FUNC_EQUAL,
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_COMPARISON_FUNC_GREATER,
			D3D12_COMPARISON_FUNC_NOT_EQUAL,
			D3D12_COMPARISON_FUNC_GREATER_EQUAL,
			D3D12_COMPARISON_FUNC_ALWAYS
		};
		static_assert(ArraySize(kTable) == (size_t)GfxComparisonFunc::Max, "tableError");

		return kTable[(int)f];
	}

	inline D3D12_TEXTURE_ADDRESS_MODE GetDx12TextureAddress(GfxTextureAddress a)
	{
		static const D3D12_TEXTURE_ADDRESS_MODE kTable[] =
		{
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,
			D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE
		};
		static_assert(ArraySize(kTable) == (size_t)GfxTextureAddress::Max, "tableError");

		return kTable[(int)a];
	}

	inline D3D12_FILTER GetDx12Filter(GfxFilter f)
	{
		static const D3D12_FILTER kTable[] =
		{
			D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR,
			D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
			D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR,
			D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT,
			D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
			D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_FILTER_ANISOTROPIC,
			D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
			D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
			D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
			D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
			D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
			D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
			D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
			D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
			D3D12_FILTER_COMPARISON_ANISOTROPIC,
			D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT,
			D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR,
			D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
			D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR,
			D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT,
			D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
			D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT,
			D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR,
			D3D12_FILTER_MINIMUM_ANISOTROPIC,
			D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT,
			D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR,
			D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
			D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR,
			D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT,
			D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
			D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT,
			D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR,
			D3D12_FILTER_MAXIMUM_ANISOTROPIC,
		};
		static_assert(ArraySize(kTable) == (size_t)GfxFilter::Max, "tableError");

		return kTable[(int)f];
	}

	inline D3D12_RESOURCE_DIMENSION GetDx12ResourceDimension(GfxDimension d)
	{
		static const D3D12_RESOURCE_DIMENSION kTable[] =
		{
			D3D12_RESOURCE_DIMENSION_BUFFER,    // GfxDimension::Buffer,
			D3D12_RESOURCE_DIMENSION_TEXTURE1D, // GfxDimension::Texture1D,
			D3D12_RESOURCE_DIMENSION_TEXTURE1D, // GfxDimension::Texture1DArray,
			D3D12_RESOURCE_DIMENSION_TEXTURE2D, // GfxDimension::Texture2D,
			D3D12_RESOURCE_DIMENSION_TEXTURE2D, // GfxDimension::Texture2DArray,
			D3D12_RESOURCE_DIMENSION_TEXTURE2D, // GfxDimension::Texture2DMS,
			D3D12_RESOURCE_DIMENSION_TEXTURE2D, // GfxDimension::Texture2DMSArray,
			D3D12_RESOURCE_DIMENSION_TEXTURE3D, // GfxDimension::Texture3D,
			D3D12_RESOURCE_DIMENSION_TEXTURE2D, // GfxDimension::TextureCube,
			D3D12_RESOURCE_DIMENSION_TEXTURE2D, // GfxDimension::TextureCubeArray,
		};
		static_assert(ArraySize(kTable) == (size_t)GfxDimension::Max, "tableError");

		return kTable[(int)d];
	};

	inline D3D12_SRV_DIMENSION GetDx12SrvDimension(GfxDimension d)
	{
		static const D3D12_SRV_DIMENSION kTable[] =
		{
			D3D12_SRV_DIMENSION_BUFFER,
			D3D12_SRV_DIMENSION_TEXTURE1D,
			D3D12_SRV_DIMENSION_TEXTURE1DARRAY,
			D3D12_SRV_DIMENSION_TEXTURE2D,
			D3D12_SRV_DIMENSION_TEXTURE2DARRAY,
			D3D12_SRV_DIMENSION_TEXTURE2DMS,
			D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY,
			D3D12_SRV_DIMENSION_TEXTURE3D,
			D3D12_SRV_DIMENSION_TEXTURECUBE,
			D3D12_SRV_DIMENSION_TEXTURECUBEARRAY
		};
		static_assert(ArraySize(kTable) == (size_t)GfxDimension::Max, "tableError");

		return kTable[(int)d];
	}
	
	inline D3D12_UAV_DIMENSION GetDx12UavDimension(GfxDimension d)
	{
		static const D3D12_UAV_DIMENSION kTable[] =
		{
			D3D12_UAV_DIMENSION_BUFFER,
			D3D12_UAV_DIMENSION_TEXTURE1D,
			D3D12_UAV_DIMENSION_TEXTURE1DARRAY,
			D3D12_UAV_DIMENSION_TEXTURE2D,
			D3D12_UAV_DIMENSION_TEXTURE2DARRAY,
			D3D12_UAV_DIMENSION_UNKNOWN, // TEXTURE2DMS,
			D3D12_UAV_DIMENSION_UNKNOWN, // TEXTURE2DMSARRAY,
			D3D12_UAV_DIMENSION_TEXTURE3D,
			D3D12_UAV_DIMENSION_UNKNOWN, // TEXTURECUBE,
			D3D12_UAV_DIMENSION_UNKNOWN, // TEXTURECUBEARRAY
		};
		static_assert(ArraySize(kTable) == (size_t)GfxDimension::Max, "tableError");
		SI_ASSERT(kTable[(int)d] != D3D12_UAV_DIMENSION_UNKNOWN, "UAVで使えないDimension");

		return kTable[(int)d];
	}

	inline D3D12_DESCRIPTOR_RANGE_FLAGS GetDx12DescriptorRangeFlags(GfxDescriptorRangeFlags flag)
	{
		static const D3D12_DESCRIPTOR_RANGE_FLAGS kTable[] = 
		{
			D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
			D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE,
			D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE,
			D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE,
			D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC,
		};
		static_assert(((size_t)1<<(ArraySize(kTable))) == ((size_t)GfxDescriptorRangeFlag::Max), "tableError");

		D3D12_DESCRIPTOR_RANGE_FLAGS dxFlag = (D3D12_DESCRIPTOR_RANGE_FLAGS)0;
		uint32_t mask = flag.GetMask();
		while(mask != 0)
		{
			int msb = Bitwise::MSB32(mask);
			if(msb<0) break;
			if((int)ArraySize(kTable) <= msb) continue;

			dxFlag |= kTable[msb];
			mask &= ~((uint32_t)1<<(uint32_t)msb);
		}

		return dxFlag;
	}

	inline D3D12_DESCRIPTOR_RANGE_TYPE GetDx12DescriptorRangeType(GfxDescriptorRangeType type)
	{
		static const D3D12_DESCRIPTOR_RANGE_TYPE kTable[] =
		{
			D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
			D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
			D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
			D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
		};
		static_assert(ArraySize(kTable) == (size_t)GfxDescriptorRangeType::Max, "tableError");

		return kTable[(int)type];
	}

	inline D3D12_DESCRIPTOR_HEAP_TYPE GetDx12DescriptorHeapType(GfxDescriptorHeapType type)
	{
		static const D3D12_DESCRIPTOR_HEAP_TYPE kTable[] =
		{
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		};
		static_assert(ArraySize(kTable) == (size_t)GfxDescriptorHeapType::Max, "tableError");

		return kTable[(int)type];
	}
	
	inline D3D12_DESCRIPTOR_HEAP_FLAGS GetDx12DescriptorHeapFlag(GfxDescriptorHeapFlag flag)
	{
		static const D3D12_DESCRIPTOR_HEAP_FLAGS kTable[] =
		{
			D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		};
		static_assert(ArraySize(kTable) == (size_t)GfxDescriptorHeapFlag::Max, "tableError");

		return kTable[(int)flag];
	}

	inline D3D12_PRIMITIVE_TOPOLOGY GetDx12PrimitiveTopology(GfxPrimitiveTopology topology)
	{
		D3D12_PRIMITIVE_TOPOLOGY kTable[] =
		{
			D3D_PRIMITIVE_TOPOLOGY_UNDEFINED,
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
			D3D_PRIMITIVE_TOPOLOGY_LINELIST,
			D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
			D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
			D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST,
			D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST,
		};
		static_assert(ArraySize(kTable)==(size_t)GfxPrimitiveTopology::Max, "TableError");

		return kTable[(int)topology];
	}

	inline D3D12_HEAP_TYPE GetDx12HeapType(GfxHeapType type)
	{
		D3D12_HEAP_TYPE kTable[] =
		{
			D3D12_HEAP_TYPE_DEFAULT,
			D3D12_HEAP_TYPE_UPLOAD,
			D3D12_HEAP_TYPE_READBACK,
			D3D12_HEAP_TYPE_CUSTOM
		};
		static_assert(ArraySize(kTable) == (size_t)GfxHeapType::Max, "tableError");

		return kTable[(int)type];
	}

	inline DXGI_FORMAT GetDx12Format(GfxFormat format)
	{
		DXGI_FORMAT kTable[] =
		{
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_R32G32B32A32_TYPELESS,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R32G32B32A32_UINT           ,
			DXGI_FORMAT_R32G32B32A32_SINT           ,
			DXGI_FORMAT_R32G32B32_TYPELESS          ,
			DXGI_FORMAT_R32G32B32_FLOAT             ,
			DXGI_FORMAT_R32G32B32_UINT              ,
			DXGI_FORMAT_R32G32B32_SINT              ,
			DXGI_FORMAT_R16G16B16A16_TYPELESS       ,
			DXGI_FORMAT_R16G16B16A16_FLOAT          ,
			DXGI_FORMAT_R16G16B16A16_UNORM          ,
			DXGI_FORMAT_R16G16B16A16_UINT           ,
			DXGI_FORMAT_R16G16B16A16_SNORM          ,
			DXGI_FORMAT_R16G16B16A16_SINT           ,
			DXGI_FORMAT_R32G32_TYPELESS             ,
			DXGI_FORMAT_R32G32_FLOAT                ,
			DXGI_FORMAT_R32G32_UINT                 ,
			DXGI_FORMAT_R32G32_SINT                 ,
			DXGI_FORMAT_R32G8X24_TYPELESS           ,
			DXGI_FORMAT_D32_FLOAT_S8X24_UINT        ,
			DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    ,
			DXGI_FORMAT_X32_TYPELESS_G8X24_UINT     ,
			DXGI_FORMAT_R10G10B10A2_TYPELESS        ,
			DXGI_FORMAT_R10G10B10A2_UNORM           ,
			DXGI_FORMAT_R10G10B10A2_UINT            ,
			DXGI_FORMAT_R11G11B10_FLOAT             ,
			DXGI_FORMAT_R8G8B8A8_TYPELESS           ,
			DXGI_FORMAT_R8G8B8A8_UNORM              ,
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB         ,
			DXGI_FORMAT_R8G8B8A8_UINT               ,
			DXGI_FORMAT_R8G8B8A8_SNORM              ,
			DXGI_FORMAT_R8G8B8A8_SINT               ,
			DXGI_FORMAT_R16G16_TYPELESS             ,
			DXGI_FORMAT_R16G16_FLOAT                ,
			DXGI_FORMAT_R16G16_UNORM                ,
			DXGI_FORMAT_R16G16_UINT                 ,
			DXGI_FORMAT_R16G16_SNORM                ,
			DXGI_FORMAT_R16G16_SINT                 ,
			DXGI_FORMAT_R32_TYPELESS                ,
			DXGI_FORMAT_D32_FLOAT                   ,
			DXGI_FORMAT_R32_FLOAT                   ,
			DXGI_FORMAT_R32_UINT                    ,
			DXGI_FORMAT_R32_SINT                    ,
			DXGI_FORMAT_R24G8_TYPELESS              ,
			DXGI_FORMAT_D24_UNORM_S8_UINT           ,
			DXGI_FORMAT_R24_UNORM_X8_TYPELESS       ,
			DXGI_FORMAT_X24_TYPELESS_G8_UINT        ,
			DXGI_FORMAT_R8G8_TYPELESS               ,
			DXGI_FORMAT_R8G8_UNORM                  ,
			DXGI_FORMAT_R8G8_UINT                   ,
			DXGI_FORMAT_R8G8_SNORM                  ,
			DXGI_FORMAT_R8G8_SINT                   ,
			DXGI_FORMAT_R16_TYPELESS                ,
			DXGI_FORMAT_R16_FLOAT                   ,
			DXGI_FORMAT_D16_UNORM                   ,
			DXGI_FORMAT_R16_UNORM                   ,
			DXGI_FORMAT_R16_UINT                    ,
			DXGI_FORMAT_R16_SNORM                   ,
			DXGI_FORMAT_R16_SINT                    ,
			DXGI_FORMAT_R8_TYPELESS                 ,
			DXGI_FORMAT_R8_UNORM                    ,
			DXGI_FORMAT_R8_UINT                     ,
			DXGI_FORMAT_R8_SNORM                    ,
			DXGI_FORMAT_R8_SINT                     ,
			DXGI_FORMAT_A8_UNORM                    ,
			DXGI_FORMAT_R1_UNORM                    ,
			DXGI_FORMAT_R9G9B9E5_SHAREDEXP          ,
			DXGI_FORMAT_R8G8_B8G8_UNORM             ,
			DXGI_FORMAT_G8R8_G8B8_UNORM             ,
			DXGI_FORMAT_BC1_TYPELESS                ,
			DXGI_FORMAT_BC1_UNORM                   ,
			DXGI_FORMAT_BC1_UNORM_SRGB              ,
			DXGI_FORMAT_BC2_TYPELESS                ,
			DXGI_FORMAT_BC2_UNORM                   ,
			DXGI_FORMAT_BC2_UNORM_SRGB              ,
			DXGI_FORMAT_BC3_TYPELESS                ,
			DXGI_FORMAT_BC3_UNORM                   ,
			DXGI_FORMAT_BC3_UNORM_SRGB              ,
			DXGI_FORMAT_BC4_TYPELESS                ,
			DXGI_FORMAT_BC4_UNORM                   ,
			DXGI_FORMAT_BC4_SNORM                   ,
			DXGI_FORMAT_BC5_TYPELESS                ,
			DXGI_FORMAT_BC5_UNORM                   ,
			DXGI_FORMAT_BC5_SNORM                   ,
			DXGI_FORMAT_B5G6R5_UNORM                ,
			DXGI_FORMAT_B5G5R5A1_UNORM              ,
			DXGI_FORMAT_B8G8R8A8_UNORM              ,
			DXGI_FORMAT_B8G8R8X8_UNORM              ,
			DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  ,
			DXGI_FORMAT_B8G8R8A8_TYPELESS           ,
			DXGI_FORMAT_B8G8R8A8_UNORM_SRGB         ,
			DXGI_FORMAT_B8G8R8X8_TYPELESS           ,
			DXGI_FORMAT_B8G8R8X8_UNORM_SRGB         ,
			DXGI_FORMAT_BC6H_TYPELESS               ,
			DXGI_FORMAT_BC6H_UF16                   ,
			DXGI_FORMAT_BC6H_SF16                   ,
			DXGI_FORMAT_BC7_TYPELESS                ,
			DXGI_FORMAT_BC7_UNORM                   ,
			DXGI_FORMAT_BC7_UNORM_SRGB              ,
			//DXGI_FORMAT_AYUV                        ,
			//DXGI_FORMAT_Y410                        ,
			//DXGI_FORMAT_Y416                        ,
			//DXGI_FORMAT_NV12                        ,
			//DXGI_FORMAT_P010                        ,
			//DXGI_FORMAT_P016                        ,
			//DXGI_FORMAT_420_OPAQUE                  ,
			//DXGI_FORMAT_YUY2                        ,
			//DXGI_FORMAT_Y210                        ,
			//DXGI_FORMAT_Y216                        ,
			//DXGI_FORMAT_NV11                        ,
			//DXGI_FORMAT_AI44                        ,
			//DXGI_FORMAT_IA44                        ,
			//DXGI_FORMAT_P8                          ,
			//DXGI_FORMAT_A8P8                        ,
			//DXGI_FORMAT_B4G4R4A4_UNORM              ,
			//DXGI_FORMAT_P208                        ,
			//DXGI_FORMAT_V208                        ,
			//DXGI_FORMAT_V408                        ,
		};
		static_assert(SI::ArraySize(kTable) == (size_t)GfxFormat::Max, "tableError");

		return kTable[(int)format];
	}

} // namespace SI

#endif // SI_USE_DX12
