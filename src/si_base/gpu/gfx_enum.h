#pragma once

#include "si_base/misc/enum_flags.h"

namespace SI
{
	enum class GfxShaderVisibility
	{
		All	= 0,
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,

		Max,
	};

	enum class GfxDepthWriteMask
	{
		kZero = 0,
		kAll,

		kMax
	};

	enum class GfxFillMode
	{
		kWireframe = 0,
		kSolid,

		kMax
	};
	
	enum class GfxCullMode
	{
		kNone = 0,
		kFront,
		kBack,

		kMax
	};

	enum class GfxPrimitiveTopologyType
	{
		kUndefined = 0,
		kPoint,
		kLine,
		kTriangle,
		kPatch,

		kMax,
	};

	enum class GfxColorWriteFlag
	{
		kRed     = 1 << 0,
		kGreen   = 1 << 1,
		kBlue    = 1 << 2,
		kAlpha   = 1 << 3,
		kRGB     = kRed | kGreen | kBlue,
		kRGBA    = kRed | kGreen | kBlue | kAlpha,

		kMax     = 1 << 4,
	};
	SI_DECLARE_ENUM_FLAGS(GfxColorWriteFlags, GfxColorWriteFlag);

	enum class GfxBlend
	{
		kZero = 0,
		kOne,
		kSrcColor,
		kInvSrcColor,
		kSrcAlpha,
		kInvSrcAlpha,
		kDestAlpha,
		kInvDestAlpha,
		kDestColor,
		kInvDestColor,
		kSrcAlphaSat,
		kBlendFactor,
		kInvBlendFactor,
		kSrc1Color,
		kInvSrc1Color,
		kSrc1Alpha,
		kInvSrc1Alpha,

		kMax
	};
	
	enum class GfxBlendOp
	{
		kAdd = 0,
		kSubtract,
		kRevSubtract,
		kMin,
		kMax,

		kEnumMax
	};

	enum class GfxLogicOp
	{ 
		kClear = 0,
		kSet,
		kCopy,
		kCopyInverted,
		kNoop,
		kInvert,
		kAnd,
		kNand,
		kOr,
		kNor,
		kXor,
		kEquiv,
		kAndReverse,
		kAndInverted,
		kOrReverse,
		kOrInverted,

		kMax
	};

	enum class GfxClearFlag
	{
		kDepth   = 1 << 0,
		kStencil = 1 << 1,

		kMax     = 1 << 2,
	};
	SI_DECLARE_ENUM_FLAGS(GfxClearFlags, GfxClearFlag);

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
		kGenericRead             = 1 << 15,
		kPredication             = 1 << 16,
		kVideoDecodeRead         = 1 << 17,
		kVideoDecodeWrite        = 1 << 18,
		kVideoProcessRead        = 1 << 19,
		kVideoProcessWrite       = 1 << 20,

		kMax                     = 1 << 21,
		kPendding                = 1 << 31 // For system. contextが違う時にpenddingするための特別なstate
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

		kMax                      = 1<<7,
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
		kNever = 0,
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
		None                                        = 1<<0,
		DescriptorsVolatile                         = 1<<1,
		DataVolatile                                = 1<<2,
		DataStaticWhileSetAtExecute                 = 1<<3,
		DataStatic                                  = 1<<4,

		Volatile                                    = DescriptorsVolatile | DataVolatile,

		Max                                         = 1<<5,
	};
	SI_DECLARE_ENUM_FLAGS(GfxDescriptorRangeFlags, GfxDescriptorRangeFlag);

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

} // namespace SI
