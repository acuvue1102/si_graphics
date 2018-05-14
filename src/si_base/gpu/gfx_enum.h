#pragma once

#include "si_base/misc/enum_flags.h"

namespace SI
{
	enum class GfxResourceState
	{
		kCommon                  = 1 << 0,
		kVertexAndConstantBuffer = 1 << 1,
		kIndexBuffer             = 1 << 2,
		kRenderTarget            = 1 << 3,
		kUnorderedAccess         = 1 << 4,
		kDepthWrite              = 1 << 5,
		kDepthRead               = 1 << 6,
		kNonPixelShaderResource  = 1 << 7,
		kPixelShaderResource     = 1 << 8,
		kStreamOut               = 1 << 9,
		kIndirectArgument        = 1 << 10,
		kCopyDest                = 1 << 11,
		kCopySource              = 1 << 12,
		kResolveDest             = 1 << 13,
		kResolveSource           = 1 << 14,
	};
	SI_DECLARE_ENUM_FLAGS(GfxResourceStates, GfxResourceState);

	enum class GfxResourceFlag
	{
		kNone	                  = 0,
		kAllowRenderTarget        = 1<<0,
		kAllowDepthStencil        = 1<<1,
		kAllowUnorderedAccess     = 1<<2,
		kDenyShaderResource       = 1<<3,
		kAllowCrossAdapter        = 1<<4,
		kAllowSimultaneousAccess  = 1<<5,
		kVideoDecodeReferenceOnly = 1<<6,
	};
	SI_DECLARE_ENUM_FLAGS(GfxResourceFlags, GfxResourceFlag);

	enum class GfxResourceBarrierFlag
	{
		kNone = 0,
		kBeginOnly,
		kEndOnly,

		kMax,
	};

	enum class GfxComparisonFunc
	{
		kNever,
		kLess,
		kEqual,
		kLessEqual,
		kGreater,
		kNotEqual,
		kGreaterEqual,
		kAlways,

		kMax
	};

	enum class GfxTextureAddress
	{
		kWrap = 0,
		kMirror,
		kClamp,
		kBoarder,
		kMirrorOnce,
		
		kMax
	};

	enum class GfxFilter
	{
		kMinMagMipPoint = 0,
		kMinMagPointMipLinear,
		kMinPointMagLinearMipPoint,
		kMinPointMagMipLinear,
		kMinLinearMagMipPoint,
		kMinLinearMagPointMipLinear,
		kMinMagLinearMipPoint,
		kMinMagMipLinear,
		kAnisotropic,
		kComparisonMinMagMipPoint,
		kComparisonMinMagPointMipLinear,
		kComparisonMinPointMagLinearMipPoint,
		kComparisonMinPointMagMipLinear,
		kComparisonMinLinearMagMipPoint,
		kComparisonMinLinearMagPointMipLinear,
		kComparisonMinMagLinearMipPoint,
		kComparisonMinMagMipLinear,
		kComparisonAnisotropic,
		kMinimumMinMagMipPoint,
		kMinimumMinMagPointMipLinear,
		kMinimumMinPointMagLinearMipPoint,
		kMinimumMinPointMagMipLinear,
		kMinimumMinLinearMagMipPoint,
		kMinimumMinLinearMagPointMipLinear,
		kMinimumMinMagLinearMipPoint,
		kMinimumMinMagMipLinear,
		kMinimumAnisotropic,
		kMaximumMinMagMipPoint,
		kMaximumMinMagPointMipLinear,
		kMaximumMinPointMagLinearMipPoint,
		kMaximumMinPointMagMipLinear,
		kMaximumMinLinearMagMipPoint,
		kMaximumMinLinearMagPointMipLinear,
		kMaximumMinMagLinearMipPoint,
		kMaximumMinMagMipLinear,
		kMaximumAnisotropic,

		kMax
	};

	enum class GfxDimension
	{
		kBuffer = 0,
		kTexture1D,
		kTexture1DArray,
		kTexture2D,
		kTexture2DArray,
		kTexture2DMS,
		kTexture2DMSArray,
		kTexture3D,
		kTextureCube,
		kTextureCubeArray,

		kMax,
	};

	enum class GfxDescriptorRangeFlag
	{
		kStatic = 0,
		kDynamic,

		kMax,
	};

	enum class GfxDescriptorRangeType
	{
		kSrv = 0,
		kUav,
		kCbv,
		kSampler,

		kMax,
	};

	enum class GfxDescriptorHeapType
	{
		kCbvSrvUav = 0,
		kSampler,
		kRtv,
		kDsv,

		kMax
	};
 
	enum class GfxDescriptorHeapFlag
	{
		kNone = 0,
		kShaderVisible,

		kMax
	};

	enum class GfxPrimitiveTopology
	{
		kUndefined = 0,
		kPointList,
		kLineList,
		kLineStrip,
		kTriangleList,
		kTriangleStrip,
		kLineListAdj,
		kLineStripAdj,
		kTriangleListAdj,
		kTriangleStripAdj,
		kControlPointPatchList1,
		kControlPointPatchList2,
		kControlPointPatchList3,
		kControlPointPatchList4,
		kControlPointPatchList5,
		kControlPointPatchList6,
		kControlPointPatchList7,
		kControlPointPatchList8,
		kControlPointPatchList9,
		kControlPointPatchList10,
		kControlPointPatchList11,
		kControlPointPatchList12,
		kControlPointPatchList13,
		kControlPointPatchList14,
		kControlPointPatchList15,
		kControlPointPatchList16,
		kControlPointPatchList17,
		kControlPointPatchList18,
		kControlPointPatchList19,
		kControlPointPatchList20,
		kControlPointPatchList21,
		kControlPointPatchList22,
		kControlPointPatchList23,
		kControlPointPatchList24,
		kControlPointPatchList25,
		kControlPointPatchList26,
		kControlPointPatchList27,
		kControlPointPatchList28,
		kControlPointPatchList29,
		kControlPointPatchList30,
		kControlPointPatchList31,
		kControlPointPatchList32,
		
		kMax,
	};

	enum class GfxHeapType
	{
		kDefault = 0,
		kUpload,
		kReadBack,
		kCustom,

		kMax,
	};

	enum class GfxFormat
	{
		kUnknown = 0,
		kR32G32B32A32_Typeless,
		kR32G32B32A32_Float,
		kR32G32B32A32_Uint,
		kR32G32B32A32_Sint,
		kR32G32B32_Typeless,
		kR32G32B32_Float,
		kR32G32B32_Uint,
		kR32G32B32_Sint,
		kR16G16B16A16_Typeless,
		kR16G16B16A16_Float,
		kR16G16B16A16_Unorm,
		kR16G16B16A16_Uint,
		kR16G16B16A16_Snorm,
		kR16G16B16A16_Sint,
		kR32G32_Typeless,
		kR32G32_Float,
		kR32G32_Uint,
		kR32G32_Sint,
		kR32G8X24_Typeless,
		kD32_Float_S8X24_Uint,
		kR32_Float_X8X24_Typeless,
		kX32_Typeless_G8X24_Uint,
		kR10G10B10A2_Typeless,
		kR10G10B10A2_Unorm,
		kR10G10B10A2_Uint,
		kR11G11B10_Float,
		kR8G8B8A8_Typeless,
		kR8G8B8A8_Unorm,
		kR8G8B8A8_Unorm_SRGB,
		kR8G8B8A8_Uint,
		kR8G8B8A8_Snorm,
		kR8G8B8A8_Sint,
		kR16G16_Typeless,
		kR16G16_Float,
		kR16G16_Unorm,
		kR16G16_Uint,
		kR16G16_Snorm,
		kR16G16_Sint,
		kR32_Typeless,
		kD32_Float,
		kR32_Float,
		kR32_Uint,
		kR32_Sint,
		kR24G8_Typeless,
		kD24_Unorm_S8_Uint,
		kR24_Unorm_X8_Typeless,
		kX24_Typeless_G8_Uint,
		kR8G8_Typeless,
		kR8G8_Unorm,
		kR8G8_Uint,
		kR8G8_Snorm,
		kR8G8_Sint,
		kR16_Typeless,
		kR16_Float,
		kD16_Unorm,
		kR16_Unorm,
		kR16_Uint,
		kR16_Snorm,
		kR16_Sint,
		kR8_Typeless,
		kR8_Unorm,
		kR8_Uint,
		kR8_Snorm,
		kR8_Sint,
		kA8_Unorm,
		kR1_Unorm,
		kR9G9B9E5_SHAREDEXP,
		kR8G8_B8G8_Unorm,
		kG8R8_G8B8_Unorm,
		kBC1_Typeless,
		kBC1_Unorm,
		kBC1_Unorm_SRGB,
		kBC2_Typeless,
		kBC2_Unorm,
		kBC2_Unorm_SRGB,
		kBC3_Typeless,
		kBC3_Unorm,
		kBC3_Unorm_SRGB,
		kBC4_Typeless,
		kBC4_Unorm,
		kBC4_Snorm,
		kBC5_Typeless,
		kBC5_Unorm,
		kBC5_Snorm,
		kB5G6R5_Unorm,
		kB5G5R5A1_Unorm,
		kB8G8R8A8_Unorm,
		kB8G8R8X8_Unorm,
		kR10G10B10_XR_BIAS_A2_Unorm,
		kB8G8R8A8_Typeless,
		kB8G8R8A8_Unorm_SRGB,
		kB8G8R8X8_Typeless,
		kB8G8R8X8_Unorm_SRGB,
		kBC6H_Typeless,
		kBC6H_UF16,
		kBC6H_SF16,
		kBC7_Typeless,
		kBC7_Unorm,
		kBC7_Unorm_SRGB,
		//kAYUV,
		//kY410,
		//kY416,
		//kNV12,
		//kP010,
		//kP016,
		//k420_OPAQUE,
		//kYUY2,
		//kY210,
		//kY216,
		//kNV11,
		//kAI44,
		//kIA44,
		//kP8,
		//kA8P8,
		//kB4G4R4A4_Unorm,
		//kP208,
		//kV208,
		//kV408,

		kMax
	};

	// enumで表現したいが、重複して指定できるenumなので、Gfx Wrapperではclassにする.
	//class GfxResourceState
	//{
	//public:
	//	GfxResourceState(int stateFlag = 0)
	//		: m_stateFlags(stateFlag)
	//	{
	//	}

	//	~GfxResourceState()
	//	{
	//	}

	//	inline int GetStateFlags() const{ return m_stateFlags; }
	//	inline void SetStateFlags(int stateFlags){ m_stateFlags = stateFlags; }
	//	inline void MergeStateFlags(const GfxResourceState& state){ m_stateFlags |= state.GetStateFlags();  }
	//	
	//	inline GfxResourceState& operator|=(const GfxResourceState& a)
	//	{
	//		MergeStateFlags(a);
	//		return *this;
	//	}

	//	// 各ResourceStateタイプ. 組み合わせれる.
	//	static const GfxResourceState kCommon; // present
	//	static const GfxResourceState kVertexAndConstantBuffer;
	//	static const GfxResourceState kIndexBuffer;
	//	static const GfxResourceState kRenderTarget;
	//	static const GfxResourceState kUnorderedAccess;
	//	static const GfxResourceState kDepthWrite;
	//	static const GfxResourceState kDepthRead;
	//	static const GfxResourceState kNonPixelShaderResource;
	//	static const GfxResourceState kPixelShaderResource;
	//	static const GfxResourceState kStreamOut;
	//	static const GfxResourceState kIndirectArgument;
	//	static const GfxResourceState kCopyDest;
	//	static const GfxResourceState kCopySource;
	//	static const GfxResourceState kResolveDest;
	//	static const GfxResourceState kResolveSource;

	//private:
	//	int m_stateFlags;
	//};
	//
	//inline GfxResourceState operator|(const GfxResourceState& a, const GfxResourceState& b)
	//{
	//	return GfxResourceState(a.GetStateFlags() | b.GetStateFlags());
	//}


} // namespace SI
