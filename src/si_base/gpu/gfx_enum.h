#pragma once

#include "si_base/misc/enum_flags.h"

namespace SI
{
	enum class GfxSemanticsType : uint8_t
	{
		Invalid = 0,
		Position,
		Normal,
		UV,
		Tangent,
		Color,

		Max,
	};
	static_assert((int)GfxSemanticsType::Max < (1<<4), "4bitだけ使ってる.");
	struct GfxSemantics
	{
		GfxSemanticsType m_semanticsType  : 4;
		uint8_t          m_semanticsIndex : 4;
		
		GfxSemantics()
			: m_semanticsType(GfxSemanticsType::Invalid)
			, m_semanticsIndex(0)
		{
		}

		GfxSemantics(GfxSemanticsType type, uint8_t index)
			: m_semanticsType(type)
			, m_semanticsIndex(index)
		{
		}
	};
	static_assert(sizeof(GfxSemantics)==sizeof(uint8_t), "semantics size error");

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
		Zero = 0,
		All,

		Max
	};

	enum class GfxFillMode
	{
		Wireframe = 0,
		Solid,

		Max
	};
	
	enum class GfxCullMode
	{
		None = 0,
		Front,
		Back,

		Max
	};

	enum class GfxPrimitiveTopologyType
	{
		Undefined = 0,
		Point,
		Line,
		Triangle,
		Patch,

		Max,
	};

	enum class GfxColorWriteFlag
	{
		Red     = 1 << 0,
		Green   = 1 << 1,
		Blue    = 1 << 2,
		Alpha   = 1 << 3,
		RGB     = Red | Green | Blue,
		RGBA    = Red | Green | Blue | Alpha,

		Max     = 1 << 4,
	};
	SI_DECLARE_ENUM_FLAGS(GfxColorWriteFlags, GfxColorWriteFlag);

	enum class GfxBlend
	{
		Zero = 0,
		One,
		SrcColor,
		InvSrcColor,
		SrcAlpha,
		InvSrcAlpha,
		DestAlpha,
		InvDestAlpha,
		DestColor,
		InvDestColor,
		SrcAlphaSat,
		BlendFactor,
		InvBlendFactor,
		Src1Color,
		InvSrc1Color,
		Src1Alpha,
		InvSrc1Alpha,

		Max
	};
	
	enum class GfxBlendOp
	{
		Add = 0,
		Subtract,
		RevSubtract,
		Min,
		Max,

		EnumMax
	};

	enum class GfxLogicOp
	{ 
		Clear = 0,
		Set,
		Copy,
		CopyInverted,
		Noop,
		Invert,
		And,
		Nand,
		Or,
		Nor,
		Xor,
		Equiv,
		AndReverse,
		AndInverted,
		OrReverse,
		OrInverted,

		Max
	};

	enum class GfxClearFlag
	{
		Depth   = 1 << 0,
		Stencil = 1 << 1,

		Max     = 1 << 2,
	};
	SI_DECLARE_ENUM_FLAGS(GfxClearFlags, GfxClearFlag);

	enum class GfxResourceState
	{
		Common                  = 1 << 0,
		VertexAndConstantBuffer = 1 << 1,
		IndexBuffer             = 1 << 2,
		RenderTarget            = 1 << 3,
		UnorderedAccess         = 1 << 4,
		DepthWrite              = 1 << 5,
		DepthRead               = 1 << 6,
		NonPixelShaderResource  = 1 << 7,
		PixelShaderResource     = 1 << 8,
		StreamOut               = 1 << 9,
		IndirectArgument        = 1 << 10,
		CopyDest                = 1 << 11,
		CopySource              = 1 << 12,
		ResolveDest             = 1 << 13,
		ResolveSource           = 1 << 14,		
		GenericRead             = 1 << 15,
		Predication             = 1 << 16,
		VideoDecodeRead         = 1 << 17,
		VideoDecodeWrite        = 1 << 18,
		VideoProcessRead        = 1 << 19,
		VideoProcessWrite       = 1 << 20,

		Max                     = 1 << 21,
		Pendding                = 1 << 31 // For system. contextが違う時にpenddingするための特別なstate
	};
	SI_DECLARE_ENUM_FLAGS(GfxResourceStates, GfxResourceState);

	enum class GfxResourceFlag
	{
		None	                  = 0,
		AllowRenderTarget        = 1<<0,
		AllowDepthStencil        = 1<<1,
		AllowUnorderedAccess     = 1<<2,
		DenyShaderResource       = 1<<3,
		AllowCrossAdapter        = 1<<4,
		AllowSimultaneousAccess  = 1<<5,
		VideoDecodeReferenceOnly = 1<<6,

		Max                      = 1<<7,
	};
	SI_DECLARE_ENUM_FLAGS(GfxResourceFlags, GfxResourceFlag);

	enum class GfxResourceBarrierFlag
	{
		None = 0,
		BeginOnly,
		EndOnly,

		Max,
	};

	enum class GfxComparisonFunc
	{
		Never = 0,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always,

		Max
	};

	enum class GfxTextureAddress
	{
		Wrap = 0,
		Mirror,
		Clamp,
		Boarder,
		MirrorOnce,
		
		Max
	};

	enum class GfxFilter
	{
		MinMagMipPoint = 0,
		MinMagPointMipLinear,
		MinPointMagLinearMipPoint,
		MinPointMagMipLinear,
		MinLinearMagMipPoint,
		MinLinearMagPointMipLinear,
		MinMagLinearMipPoint,
		MinMagMipLinear,
		Anisotropic,
		ComparisonMinMagMipPoint,
		ComparisonMinMagPointMipLinear,
		ComparisonMinPointMagLinearMipPoint,
		ComparisonMinPointMagMipLinear,
		ComparisonMinLinearMagMipPoint,
		ComparisonMinLinearMagPointMipLinear,
		ComparisonMinMagLinearMipPoint,
		ComparisonMinMagMipLinear,
		ComparisonAnisotropic,
		MinimumMinMagMipPoint,
		MinimumMinMagPointMipLinear,
		MinimumMinPointMagLinearMipPoint,
		MinimumMinPointMagMipLinear,
		MinimumMinLinearMagMipPoint,
		MinimumMinLinearMagPointMipLinear,
		MinimumMinMagLinearMipPoint,
		MinimumMinMagMipLinear,
		MinimumAnisotropic,
		MaximumMinMagMipPoint,
		MaximumMinMagPointMipLinear,
		MaximumMinPointMagLinearMipPoint,
		MaximumMinPointMagMipLinear,
		MaximumMinLinearMagMipPoint,
		MaximumMinLinearMagPointMipLinear,
		MaximumMinMagLinearMipPoint,
		MaximumMinMagMipLinear,
		MaximumAnisotropic,

		Max
	};

	enum class GfxDimension
	{
		Buffer = 0,
		Texture1D,
		Texture1DArray,
		Texture2D,
		Texture2DArray,
		Texture2DMS,
		Texture2DMSArray,
		Texture3D,
		TextureCube,
		TextureCubeArray,

		Max,
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
		Srv = 0,
		Uav,
		Cbv,
		Sampler,

		Max,
	};

	enum class GfxDescriptorHeapType
	{
		CbvSrvUav = 0,
		Sampler,
		Rtv,
		Dsv,

		Max
	};
 
	enum class GfxDescriptorHeapFlag
	{
		None = 0,
		ShaderVisible,

		Max
	};

	enum class GfxPrimitiveTopology
	{
		Undefined = 0,
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
		LineListAdj,
		LineStripAdj,
		TriangleListAdj,
		TriangleStripAdj,
		ControlPointPatchList1,
		ControlPointPatchList2,
		ControlPointPatchList3,
		ControlPointPatchList4,
		ControlPointPatchList5,
		ControlPointPatchList6,
		ControlPointPatchList7,
		ControlPointPatchList8,
		ControlPointPatchList9,
		ControlPointPatchList10,
		ControlPointPatchList11,
		ControlPointPatchList12,
		ControlPointPatchList13,
		ControlPointPatchList14,
		ControlPointPatchList15,
		ControlPointPatchList16,
		ControlPointPatchList17,
		ControlPointPatchList18,
		ControlPointPatchList19,
		ControlPointPatchList20,
		ControlPointPatchList21,
		ControlPointPatchList22,
		ControlPointPatchList23,
		ControlPointPatchList24,
		ControlPointPatchList25,
		ControlPointPatchList26,
		ControlPointPatchList27,
		ControlPointPatchList28,
		ControlPointPatchList29,
		ControlPointPatchList30,
		ControlPointPatchList31,
		ControlPointPatchList32,
		
		Max,
	};

	enum class GfxHeapType
	{
		Default = 0,
		Upload,
		ReadBack,
		Custom,

		Max,
	};

	enum class GfxFormat : uint8_t
	{
		Unknown = 0,
		R32G32B32A32_Typeless,
		R32G32B32A32_Float,
		R32G32B32A32_Uint,
		R32G32B32A32_Sint,
		R32G32B32_Typeless,
		R32G32B32_Float,
		R32G32B32_Uint,
		R32G32B32_Sint,
		R16G16B16A16_Typeless,
		R16G16B16A16_Float,
		R16G16B16A16_Unorm,
		R16G16B16A16_Uint,
		R16G16B16A16_Snorm,
		R16G16B16A16_Sint,
		R32G32_Typeless,
		R32G32_Float,
		R32G32_Uint,
		R32G32_Sint,
		R32G8X24_Typeless,
		D32_Float_S8X24_Uint,
		R32_Float_X8X24_Typeless,
		X32_Typeless_G8X24_Uint,
		R10G10B10A2_Typeless,
		R10G10B10A2_Unorm,
		R10G10B10A2_Uint,
		R11G11B10_Float,
		R8G8B8A8_Typeless,
		R8G8B8A8_Unorm,
		R8G8B8A8_Unorm_SRGB,
		R8G8B8A8_Uint,
		R8G8B8A8_Snorm,
		R8G8B8A8_Sint,
		R16G16_Typeless,
		R16G16_Float,
		R16G16_Unorm,
		R16G16_Uint,
		R16G16_Snorm,
		R16G16_Sint,
		R32_Typeless,
		D32_Float,
		R32_Float,
		R32_Uint,
		R32_Sint,
		R24G8_Typeless,
		D24_Unorm_S8_Uint,
		R24_Unorm_X8_Typeless,
		X24_Typeless_G8_Uint,
		R8G8_Typeless,
		R8G8_Unorm,
		R8G8_Uint,
		R8G8_Snorm,
		R8G8_Sint,
		R16_Typeless,
		R16_Float,
		D16_Unorm,
		R16_Unorm,
		R16_Uint,
		R16_Snorm,
		R16_Sint,
		R8_Typeless,
		R8_Unorm,
		R8_Uint,
		R8_Snorm,
		R8_Sint,
		A8_Unorm,
		R1_Unorm,
		R9G9B9E5_SHAREDEXP,
		R8G8_B8G8_Unorm,
		G8R8_G8B8_Unorm,
		BC1_Typeless,
		BC1_Unorm,
		BC1_Unorm_SRGB,
		BC2_Typeless,
		BC2_Unorm,
		BC2_Unorm_SRGB,
		BC3_Typeless,
		BC3_Unorm,
		BC3_Unorm_SRGB,
		BC4_Typeless,
		BC4_Unorm,
		BC4_Snorm,
		BC5_Typeless,
		BC5_Unorm,
		BC5_Snorm,
		B5G6R5_Unorm,
		B5G5R5A1_Unorm,
		B8G8R8A8_Unorm,
		B8G8R8X8_Unorm,
		R10G10B10_XR_BIAS_A2_Unorm,
		B8G8R8A8_Typeless,
		B8G8R8A8_Unorm_SRGB,
		B8G8R8X8_Typeless,
		B8G8R8X8_Unorm_SRGB,
		BC6H_Typeless,
		BC6H_UF16,
		BC6H_SF16,
		BC7_Typeless,
		BC7_Unorm,
		BC7_Unorm_SRGB,
		//AYUV,
		//Y410,
		//Y416,
		//NV12,
		//P010,
		//P016,
		//420_OPAQUE,
		//YUY2,
		//Y210,
		//Y216,
		//NV11,
		//AI44,
		//IA44,
		//P8,
		//A8P8,
		//B4G4R4A4_Unorm,
		//P208,
		//V208,
		//V408,

		Max
	};

} // namespace SI
