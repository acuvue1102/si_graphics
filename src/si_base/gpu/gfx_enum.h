#pragma once

namespace SI
{
	enum GfxDimension
	{
		kGfxDimension_Buffer = 0,
		kGfxDimension_Texture1D,
		kGfxDimension_Texture1DArray,
		kGfxDimension_Texture2D,
		kGfxDimension_Texture2DArray,
		kGfxDimension_Texture2DMS,
		kGfxDimension_Texture2DMSArray,
		kGfxDimension_Texture3D,
		kGfxDimension_TextureCube,
		kGfxDimension_TextureCubeArray,

		kGfxDimension_Max,
	};

	enum GfxDescriptorRangeFlag
	{
		kGfxDescriptorRangeFlag_Static = 0,
		kGfxDescriptorRangeFlag_Dynamic,

		kGfxDescriptorRangeFlag_Max,
	};

	enum GfxDescriptorRangeType
	{
		kGfxDescriptorRangeType_Srv = 0,
		kGfxDescriptorRangeType_Uav,
		kGfxDescriptorRangeType_Cbv,
		kGfxDescriptorRangeType_Sampler,

		kGfxDescriptorRangeType_Max,
	};

	enum GfxDescriptorHeapType
	{
		kGfxDescriptorHeapType_CbvSrvUav = 0,
		kGfxDescriptorHeapType_Sampler,
		kGfxDescriptorHeapType_RTV,
		kGfxDescriptorHeapType_DSV,

		kGfxDescriptorHeapType_Max
	};
 
	enum GfxDescriptorHeapFlag
	{
		kGfxDescriptorHeapFlag_None = 0,
		kGfxDescriptorHeapFlag_ShaderVisible,

		kGfxDescriptorHeapFlag_Max
	};


	enum GfxPrimitiveTopology
	{
		kGfxPrimitiveTopology_Undefined = 0,
		kGfxPrimitiveTopology_PointList,
		kGfxPrimitiveTopology_LineList,
		kGfxPrimitiveTopology_LineStrip,
		kGfxPrimitiveTopology_TriangleList,
		kGfxPrimitiveTopology_TriangleStrip,
		kGfxPrimitiveTopology_LineListAdj,
		kGfxPrimitiveTopology_LineStripAdj,
		kGfxPrimitiveTopology_TriangleListAdj,
		kGfxPrimitiveTopology_TriangleStripAdj,
		kGfxPrimitiveTopology_ControlPointPatchList1,
		kGfxPrimitiveTopology_ControlPointPatchList2,
		kGfxPrimitiveTopology_ControlPointPatchList3,
		kGfxPrimitiveTopology_ControlPointPatchList4,
		kGfxPrimitiveTopology_ControlPointPatchList5,
		kGfxPrimitiveTopology_ControlPointPatchList6,
		kGfxPrimitiveTopology_ControlPointPatchList7,
		kGfxPrimitiveTopology_ControlPointPatchList8,
		kGfxPrimitiveTopology_ControlPointPatchList9,
		kGfxPrimitiveTopology_ControlPointPatchList10,
		kGfxPrimitiveTopology_ControlPointPatchList11,
		kGfxPrimitiveTopology_ControlPointPatchList12,
		kGfxPrimitiveTopology_ControlPointPatchList13,
		kGfxPrimitiveTopology_ControlPointPatchList14,
		kGfxPrimitiveTopology_ControlPointPatchList15,
		kGfxPrimitiveTopology_ControlPointPatchList16,
		kGfxPrimitiveTopology_ControlPointPatchList17,
		kGfxPrimitiveTopology_ControlPointPatchList18,
		kGfxPrimitiveTopology_ControlPointPatchList19,
		kGfxPrimitiveTopology_ControlPointPatchList20,
		kGfxPrimitiveTopology_ControlPointPatchList21,
		kGfxPrimitiveTopology_ControlPointPatchList22,
		kGfxPrimitiveTopology_ControlPointPatchList23,
		kGfxPrimitiveTopology_ControlPointPatchList24,
		kGfxPrimitiveTopology_ControlPointPatchList25,
		kGfxPrimitiveTopology_ControlPointPatchList26,
		kGfxPrimitiveTopology_ControlPointPatchList27,
		kGfxPrimitiveTopology_ControlPointPatchList28,
		kGfxPrimitiveTopology_ControlPointPatchList29,
		kGfxPrimitiveTopology_ControlPointPatchList30,
		kGfxPrimitiveTopology_ControlPointPatchList31,
		kGfxPrimitiveTopology_ControlPointPatchList32,
		
		kGfxPrimitiveTopology_Max,
	};

	enum GfxHeapType
	{
		kGfxHeapType_Default = 0,
		kGfxHeapType_Upload,
		kGfxHeapType_ReadBack,
		kGfxHeapType_Custom,

		kGfxHeapType_Max,
	};

	enum GfxFormat
	{
		kGfxFormat_Unknown = 0,
		kGfxFormat_R32G32B32A32_Typeless,
		kGfxFormat_R32G32B32A32_Float,
		kGfxFormat_R32G32B32A32_Uint,
		kGfxFormat_R32G32B32A32_Sint,
		kGfxFormat_R32G32B32_Typeless,
		kGfxFormat_R32G32B32_Float,
		kGfxFormat_R32G32B32_Uint,
		kGfxFormat_R32G32B32_Sint,
		kGfxFormat_R16G16B16A16_Typeless,
		kGfxFormat_R16G16B16A16_Float,
		kGfxFormat_R16G16B16A16_Unorm,
		kGfxFormat_R16G16B16A16_Uint,
		kGfxFormat_R16G16B16A16_Snorm,
		kGfxFormat_R16G16B16A16_Sint,
		kGfxFormat_R32G32_Typeless,
		kGfxFormat_R32G32_Float,
		kGfxFormat_R32G32_Uint,
		kGfxFormat_R32G32_Sint,
		kGfxFormat_R32G8X24_Typeless,
		kGfxFormat_D32_Float_S8X24_Uint,
		kGfxFormat_R32_Float_X8X24_Typeless,
		kGfxFormat_X32_Typeless_G8X24_Uint,
		kGfxFormat_R10G10B10A2_Typeless,
		kGfxFormat_R10G10B10A2_Unorm,
		kGfxFormat_R10G10B10A2_Uint,
		kGfxFormat_R11G11B10_Float,
		kGfxFormat_R8G8B8A8_Typeless,
		kGfxFormat_R8G8B8A8_Unorm,
		kGfxFormat_R8G8B8A8_Unorm_SRGB,
		kGfxFormat_R8G8B8A8_Uint,
		kGfxFormat_R8G8B8A8_Snorm,
		kGfxFormat_R8G8B8A8_Sint,
		kGfxFormat_R16G16_Typeless,
		kGfxFormat_R16G16_Float,
		kGfxFormat_R16G16_Unorm,
		kGfxFormat_R16G16_Uint,
		kGfxFormat_R16G16_Snorm,
		kGfxFormat_R16G16_Sint,
		kGfxFormat_R32_Typeless,
		kGfxFormat_D32_Float,
		kGfxFormat_R32_Float,
		kGfxFormat_R32_Uint,
		kGfxFormat_R32_Sint,
		kGfxFormat_R24G8_Typeless,
		kGfxFormat_D24_Unorm_S8_Uint,
		kGfxFormat_R24_Unorm_X8_Typeless,
		kGfxFormat_X24_Typeless_G8_Uint,
		kGfxFormat_R8G8_Typeless,
		kGfxFormat_R8G8_Unorm,
		kGfxFormat_R8G8_Uint,
		kGfxFormat_R8G8_Snorm,
		kGfxFormat_R8G8_Sint,
		kGfxFormat_R16_Typeless,
		kGfxFormat_R16_Float,
		kGfxFormat_D16_Unorm,
		kGfxFormat_R16_Unorm,
		kGfxFormat_R16_Uint,
		kGfxFormat_R16_Snorm,
		kGfxFormat_R16_Sint,
		kGfxFormat_R8_Typeless,
		kGfxFormat_R8_Unorm,
		kGfxFormat_R8_Uint,
		kGfxFormat_R8_Snorm,
		kGfxFormat_R8_Sint,
		kGfxFormat_A8_Unorm,
		kGfxFormat_R1_Unorm,
		kGfxFormat_R9G9B9E5_SHAREDEXP,
		kGfxFormat_R8G8_B8G8_Unorm,
		kGfxFormat_G8R8_G8B8_Unorm,
		kGfxFormat_BC1_Typeless,
		kGfxFormat_BC1_Unorm,
		kGfxFormat_BC1_Unorm_SRGB,
		kGfxFormat_BC2_Typeless,
		kGfxFormat_BC2_Unorm,
		kGfxFormat_BC2_Unorm_SRGB,
		kGfxFormat_BC3_Typeless,
		kGfxFormat_BC3_Unorm,
		kGfxFormat_BC3_Unorm_SRGB,
		kGfxFormat_BC4_Typeless,
		kGfxFormat_BC4_Unorm,
		kGfxFormat_BC4_Snorm,
		kGfxFormat_BC5_Typeless,
		kGfxFormat_BC5_Unorm,
		kGfxFormat_BC5_Snorm,
		kGfxFormat_B5G6R5_Unorm,
		kGfxFormat_B5G5R5A1_Unorm,
		kGfxFormat_B8G8R8A8_Unorm,
		kGfxFormat_B8G8R8X8_Unorm,
		kGfxFormat_R10G10B10_XR_BIAS_A2_Unorm,
		kGfxFormat_B8G8R8A8_Typeless,
		kGfxFormat_B8G8R8A8_Unorm_SRGB,
		kGfxFormat_B8G8R8X8_Typeless,
		kGfxFormat_B8G8R8X8_Unorm_SRGB,
		kGfxFormat_BC6H_Typeless,
		kGfxFormat_BC6H_UF16,
		kGfxFormat_BC6H_SF16,
		kGfxFormat_BC7_Typeless,
		kGfxFormat_BC7_Unorm,
		kGfxFormat_BC7_Unorm_SRGB,
		//kGfxFormat_AYUV,
		//kGfxFormat_Y410,
		//kGfxFormat_Y416,
		//kGfxFormat_NV12,
		//kGfxFormat_P010,
		//kGfxFormat_P016,
		//kGfxFormat_420_OPAQUE,
		//kGfxFormat_YUY2,
		//kGfxFormat_Y210,
		//kGfxFormat_Y216,
		//kGfxFormat_NV11,
		//kGfxFormat_AI44,
		//kGfxFormat_IA44,
		//kGfxFormat_P8,
		//kGfxFormat_A8P8,
		//kGfxFormat_B4G4R4A4_Unorm,
		//kGfxFormat_P208,
		//kGfxFormat_V208,
		//kGfxFormat_V408,

		kGfxFormat_Max
	};

	// enumで表現したいが、重複して指定できるenumなので、Gfx Wrapperではclassにする.
	class GfxResourceState
	{
	public:
		GfxResourceState(int stateFlag = 0)
			: m_stateFlags(stateFlag)
		{
		}

		~GfxResourceState()
		{
		}

		inline int GetStateFlags() const{ return m_stateFlags; }
		inline void SetStateFlags(int stateFlags){ m_stateFlags = stateFlags; }
		inline void MergeStateFlags(const GfxResourceState& state){ m_stateFlags |= state.GetStateFlags();  }
		
		inline GfxResourceState& operator|=(const GfxResourceState& a)
		{
			MergeStateFlags(a);
			return *this;
		}

		// 各ResourceStateタイプ. 組み合わせれる.
		static const GfxResourceState kCommon; // present
		static const GfxResourceState kVertexAndConstantBuffer;
		static const GfxResourceState kIndexBuffer;
		static const GfxResourceState kRenderTarget;
		static const GfxResourceState kUnorderedAccess;
		static const GfxResourceState kDepthWrite;
		static const GfxResourceState kDepthRead;
		static const GfxResourceState kNonPixelShaderResource;
		static const GfxResourceState kPixelShaderResource;
		static const GfxResourceState kStreamOut;
		static const GfxResourceState kIndirectArgument;
		static const GfxResourceState kCopyDest;
		static const GfxResourceState kCopySource;
		static const GfxResourceState kResolveDest;
		static const GfxResourceState kResolveSource;

	private:
		int m_stateFlags;
	};
	
	inline GfxResourceState operator|(const GfxResourceState& a, const GfxResourceState& b)
	{
		return GfxResourceState(a.GetStateFlags() | b.GetStateFlags());
	}


} // namespace SI
