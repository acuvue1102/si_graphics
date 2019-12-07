
#include "si_base/gpu/gfx_dds.h"
#include <dxgiformat.h>

namespace SI
{
	namespace
	{
		GfxFormat GetFormat(DXGI_FORMAT format)
		{
			GfxFormat kTable[] =
			{
				GfxFormat::Unknown,                          // DXGI_FORMAT_UNKNOWN	                    = 0,
				GfxFormat::R32G32B32A32_Typeless,            // DXGI_FORMAT_R32G32B32A32_TYPELESS       = 1,
				GfxFormat::R32G32B32A32_Float,				 // DXGI_FORMAT_R32G32B32A32_FLOAT          = 2,
				GfxFormat::R32G32B32A32_Uint,				 // DXGI_FORMAT_R32G32B32A32_UINT           = 3,
				GfxFormat::R32G32B32A32_Sint,				 // DXGI_FORMAT_R32G32B32A32_SINT           = 4,
				GfxFormat::R32G32B32_Typeless,				 // DXGI_FORMAT_R32G32B32_TYPELESS          = 5,
				GfxFormat::R32G32B32_Float,					 // DXGI_FORMAT_R32G32B32_FLOAT             = 6,
				GfxFormat::R32G32B32_Uint,					 // DXGI_FORMAT_R32G32B32_UINT              = 7,
				GfxFormat::R32G32B32_Sint,					 // DXGI_FORMAT_R32G32B32_SINT              = 8,
				GfxFormat::R16G16B16A16_Typeless,			 // DXGI_FORMAT_R16G16B16A16_TYPELESS       = 9,
				GfxFormat::R16G16B16A16_Float,				 // DXGI_FORMAT_R16G16B16A16_FLOAT          = 10,
				GfxFormat::R16G16B16A16_Unorm,				 // DXGI_FORMAT_R16G16B16A16_UNORM          = 11,
				GfxFormat::R16G16B16A16_Uint,				 // DXGI_FORMAT_R16G16B16A16_UINT           = 12,
				GfxFormat::R16G16B16A16_Snorm,				 // DXGI_FORMAT_R16G16B16A16_SNORM          = 13,
				GfxFormat::R16G16B16A16_Sint,				 // DXGI_FORMAT_R16G16B16A16_SINT           = 14,
				GfxFormat::R32G32_Typeless,					 // DXGI_FORMAT_R32G32_TYPELESS             = 15,
				GfxFormat::R32G32_Float,					 // DXGI_FORMAT_R32G32_FLOAT                = 16,
				GfxFormat::R32G32_Uint,						 // DXGI_FORMAT_R32G32_UINT                 = 17,
				GfxFormat::R32G32_Sint,						 // DXGI_FORMAT_R32G32_SINT                 = 18,
				GfxFormat::R32G8X24_Typeless,				 // DXGI_FORMAT_R32G8X24_TYPELESS           = 19,
				GfxFormat::D32_Float_S8X24_Uint,			 // DXGI_FORMAT_D32_FLOAT_S8X24_UINT        = 20,
				GfxFormat::R32_Float_X8X24_Typeless,		 // DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS    = 21,
				GfxFormat::X32_Typeless_G8X24_Uint,			 // DXGI_FORMAT_X32_TYPELESS_G8X24_UINT     = 22,
				GfxFormat::R10G10B10A2_Typeless,			 // DXGI_FORMAT_R10G10B10A2_TYPELESS        = 23,
				GfxFormat::R10G10B10A2_Unorm,				 // DXGI_FORMAT_R10G10B10A2_UNORM           = 24,
				GfxFormat::R10G10B10A2_Uint,				 // DXGI_FORMAT_R10G10B10A2_UINT            = 25,
				GfxFormat::R11G11B10_Float,					 // DXGI_FORMAT_R11G11B10_FLOAT             = 26,
				GfxFormat::R8G8B8A8_Typeless,				 // DXGI_FORMAT_R8G8B8A8_TYPELESS           = 27,
				GfxFormat::R8G8B8A8_Unorm,					 // DXGI_FORMAT_R8G8B8A8_UNORM              = 28,
				GfxFormat::R8G8B8A8_Unorm_SRGB,				 // DXGI_FORMAT_R8G8B8A8_UNORM_SRGB         = 29,
				GfxFormat::R8G8B8A8_Uint,					 // DXGI_FORMAT_R8G8B8A8_UINT               = 30,
				GfxFormat::R8G8B8A8_Snorm,					 // DXGI_FORMAT_R8G8B8A8_SNORM              = 31,
				GfxFormat::R8G8B8A8_Sint,					 // DXGI_FORMAT_R8G8B8A8_SINT               = 32,
				GfxFormat::R16G16_Typeless,					 // DXGI_FORMAT_R16G16_TYPELESS             = 33,
				GfxFormat::R16G16_Float,					 // DXGI_FORMAT_R16G16_FLOAT                = 34,
				GfxFormat::R16G16_Unorm,					 // DXGI_FORMAT_R16G16_UNORM                = 35,
				GfxFormat::R16G16_Uint,						 // DXGI_FORMAT_R16G16_UINT                 = 36,
				GfxFormat::R16G16_Snorm,					 // DXGI_FORMAT_R16G16_SNORM                = 37,
				GfxFormat::R16G16_Sint,						 // DXGI_FORMAT_R16G16_SINT                 = 38,
				GfxFormat::R32_Typeless,					 // DXGI_FORMAT_R32_TYPELESS                = 39,
				GfxFormat::D32_Float,						 // DXGI_FORMAT_D32_FLOAT                   = 40,
				GfxFormat::R32_Float,						 // DXGI_FORMAT_R32_FLOAT                   = 41,
				GfxFormat::R32_Uint,						 // DXGI_FORMAT_R32_UINT                    = 42,
				GfxFormat::R32_Sint,						 // DXGI_FORMAT_R32_SINT                    = 43,
				GfxFormat::R24G8_Typeless,					 // DXGI_FORMAT_R24G8_TYPELESS              = 44,
				GfxFormat::D24_Unorm_S8_Uint,				 // DXGI_FORMAT_D24_UNORM_S8_UINT           = 45,
				GfxFormat::R24_Unorm_X8_Typeless,			 // DXGI_FORMAT_R24_UNORM_X8_TYPELESS       = 46,
				GfxFormat::X24_Typeless_G8_Uint,			 // DXGI_FORMAT_X24_TYPELESS_G8_UINT        = 47,
				GfxFormat::R8G8_Typeless,					 // DXGI_FORMAT_R8G8_TYPELESS               = 48,
				GfxFormat::R8G8_Unorm,						 // DXGI_FORMAT_R8G8_UNORM                  = 49,
				GfxFormat::R8G8_Uint,						 // DXGI_FORMAT_R8G8_UINT                   = 50,
				GfxFormat::R8G8_Snorm,						 // DXGI_FORMAT_R8G8_SNORM                  = 51,
				GfxFormat::R8G8_Sint,						 // DXGI_FORMAT_R8G8_SINT                   = 52,
				GfxFormat::R16_Typeless,					 // DXGI_FORMAT_R16_TYPELESS                = 53,
				GfxFormat::R16_Float,						 // DXGI_FORMAT_R16_FLOAT                   = 54,
				GfxFormat::D16_Unorm,						 // DXGI_FORMAT_D16_UNORM                   = 55,
				GfxFormat::R16_Unorm,						 // DXGI_FORMAT_R16_UNORM                   = 56,
				GfxFormat::R16_Uint,						 // DXGI_FORMAT_R16_UINT                    = 57,
				GfxFormat::R16_Snorm,						 // DXGI_FORMAT_R16_SNORM                   = 58,
				GfxFormat::R16_Sint,						 // DXGI_FORMAT_R16_SINT                    = 59,
				GfxFormat::R8_Typeless,						 // DXGI_FORMAT_R8_TYPELESS                 = 60,
				GfxFormat::R8_Unorm,						 // DXGI_FORMAT_R8_UNORM                    = 61,
				GfxFormat::R8_Uint,							 // DXGI_FORMAT_R8_UINT                     = 62,
				GfxFormat::R8_Snorm,						 // DXGI_FORMAT_R8_SNORM                    = 63,
				GfxFormat::R8_Sint,							 // DXGI_FORMAT_R8_SINT                     = 64,
				GfxFormat::A8_Unorm,						 // DXGI_FORMAT_A8_UNORM                    = 65,
				GfxFormat::R1_Unorm,						 // DXGI_FORMAT_R1_UNORM                    = 66,
				GfxFormat::R9G9B9E5_SHAREDEXP,				 // DXGI_FORMAT_R9G9B9E5_SHAREDEXP          = 67,
				GfxFormat::R8G8_B8G8_Unorm,					 // DXGI_FORMAT_R8G8_B8G8_UNORM             = 68,
				GfxFormat::G8R8_G8B8_Unorm,					 // DXGI_FORMAT_G8R8_G8B8_UNORM             = 69,
				GfxFormat::BC1_Typeless,					 // DXGI_FORMAT_BC1_TYPELESS                = 70,
				GfxFormat::BC1_Unorm,						 // DXGI_FORMAT_BC1_UNORM                   = 71,
				GfxFormat::BC1_Unorm_SRGB,					 // DXGI_FORMAT_BC1_UNORM_SRGB              = 72,
				GfxFormat::BC2_Typeless,					 // DXGI_FORMAT_BC2_TYPELESS                = 73,
				GfxFormat::BC2_Unorm,						 // DXGI_FORMAT_BC2_UNORM                   = 74,
				GfxFormat::BC2_Unorm_SRGB,					 // DXGI_FORMAT_BC2_UNORM_SRGB              = 75,
				GfxFormat::BC3_Typeless,					 // DXGI_FORMAT_BC3_TYPELESS                = 76,
				GfxFormat::BC3_Unorm,						 // DXGI_FORMAT_BC3_UNORM                   = 77,
				GfxFormat::BC3_Unorm_SRGB,					 // DXGI_FORMAT_BC3_UNORM_SRGB              = 78,
				GfxFormat::BC4_Typeless,					 // DXGI_FORMAT_BC4_TYPELESS                = 79,
				GfxFormat::BC4_Unorm,						 // DXGI_FORMAT_BC4_UNORM                   = 80,
				GfxFormat::BC4_Snorm,						 // DXGI_FORMAT_BC4_SNORM                   = 81,
				GfxFormat::BC5_Typeless,					 // DXGI_FORMAT_BC5_TYPELESS                = 82,
				GfxFormat::BC5_Unorm,						 // DXGI_FORMAT_BC5_UNORM                   = 83,
				GfxFormat::BC5_Snorm,						 // DXGI_FORMAT_BC5_SNORM                   = 84,
				GfxFormat::B5G6R5_Unorm,					 // DXGI_FORMAT_B5G6R5_UNORM                = 85,
				GfxFormat::B5G5R5A1_Unorm,					 // DXGI_FORMAT_B5G5R5A1_UNORM              = 86,
				GfxFormat::B8G8R8A8_Unorm,					 // DXGI_FORMAT_B8G8R8A8_UNORM              = 87,
				GfxFormat::B8G8R8X8_Unorm,					 // DXGI_FORMAT_B8G8R8X8_UNORM              = 88,
				GfxFormat::R10G10B10_XR_BIAS_A2_Unorm,		 // DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM  = 89,
				GfxFormat::B8G8R8A8_Typeless,				 // DXGI_FORMAT_B8G8R8A8_TYPELESS           = 90,
				GfxFormat::B8G8R8A8_Unorm_SRGB,				 // DXGI_FORMAT_B8G8R8A8_UNORM_SRGB         = 91,
				GfxFormat::B8G8R8X8_Typeless,				 // DXGI_FORMAT_B8G8R8X8_TYPELESS           = 92,
				GfxFormat::B8G8R8X8_Unorm_SRGB,				 // DXGI_FORMAT_B8G8R8X8_UNORM_SRGB         = 93,
				GfxFormat::BC6H_Typeless,					 // DXGI_FORMAT_BC6H_TYPELESS               = 94,
				GfxFormat::BC6H_UF16,						 // DXGI_FORMAT_BC6H_UF16                   = 95,
				GfxFormat::BC6H_SF16,						 // DXGI_FORMAT_BC6H_SF16                   = 96,
				GfxFormat::BC7_Typeless,					 // DXGI_FORMAT_BC7_TYPELESS                = 97,
				GfxFormat::BC7_Unorm,						 // DXGI_FORMAT_BC7_UNORM                   = 98,
				GfxFormat::BC7_Unorm_SRGB,					 // DXGI_FORMAT_BC7_UNORM_SRGB              = 99,
			};

			if(ArraySize(kTable) < (size_t)format)
			{
				return GfxFormat::Unknown;
			}

			return kTable[(uint32_t)format];
		}
		
		constexpr uint32_t FourCC(const char c[4])
		{
			return
				((uint32_t)(uint8_t)c[0]      ) |
				((uint32_t)(uint8_t)c[1] <<  8) |
				((uint32_t)(uint8_t)c[2] << 16) |
				((uint32_t)(uint8_t)c[3] << 24);

		}
		
		static const uint32_t kDdsMagic   = FourCC("DDS ");
		static const uint32_t kDX10FourCC = FourCC("DX10");

		enum DdsPixelFormatFlag
		{
			DDPF_ALPHAPIXELS     = 0x00000001, // RGB 以外に alpha が含まれている。つまり RGBAlphaBitMask が有効。
			DDPF_ALPHA           = 0x00000002, // pixel は Alpha 成分のみ含まれている。
			DDPF_FOURCC          = 0x00000004, // fourCC が有効。
			DDPF_PALETTEINDEXED4 = 0x00000008, // Palet 16 colors (DX9 以降はたぶん使用されない)
			DDPF_PALETTEINDEXED8 = 0x00000020, // Palet 256 colors (DX10 以降は使用されない)
			DDPF_RGB             = 0x00000040, // RGBBitCount/RBitMask/GBitMask/BBitMask/RGBAlphaBitMask によってフォーマットが定義されていることを示す
			DDPF_LUMINANCE       = 0x00020000, // 1ch のデータが RGBすべてに展開される
			DDPF_BUMPDUDV        = 0x00080000, // pixel が符号付であることを示す (本来は bump 用) DX10以降は使用しない
		};

		enum DdsResourceDimension
		{
			DDRD_UNKNOWN   = 0,
			DDRD_BUFFER    = 1,
			DDRD_TEXTURE1D = 2,
			DDRD_TEXTURE2D = 3,
			DDRD_TEXTURE3D = 4,
		};
		
		enum DdsFlag
		{
			DDSD_CAPS        = 0x1,      // すべての .dds ファイルで必須
			DDSD_HEIGHT      = 0x2,      // すべての .dds ファイルで必須
			DDSD_WIDTH       = 0x4,      // すべての .dds ファイルで必須
			DDSD_PITCH       = 0x8,      // ピッチが非圧縮テクスチャーに提供される場合は必須
			DDSD_PIXELFORMAT = 0x1000,   // すべての .dds ファイルで必須
			DDSD_MIPMAPCOUNT = 0x20000,  // ミップマップ テクスチャーで必須
			DDSD_LINEARSIZE	 = 0x80000,  // ピッチが圧縮テクスチャーに提供される場合は必須
			DDSD_DEPTH       = 0x800000, //	深度テクスチャーで必須
		};
		
		enum DdsMisc
		{
			DDSM_TEXTURECUBE = 0x4L,
		};
		
		enum DdsCaps2
		{
			DDSCAPS2_CUBEMAP           = 0x00000200,
			DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400,
			DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800,
			DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000,
			DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000,
			DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000,
			DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000,
			DDSCAPS2_VOLUME            = 0x00200000,
			DDSCAPS2_CUBEMAP_ALLFACES  = DDSCAPS2_CUBEMAP | 
			                             DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX |
			                             DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY |
			                             DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ,
		};

		struct DdsPixelFormat
		{
			uint32_t m_size;
			uint32_t m_flags;
			uint32_t m_fourCC;
			uint32_t m_RGBBitCount;
			uint32_t m_RBitMask;
			uint32_t m_GBitMask;
			uint32_t m_BBitMask;
			uint32_t m_ABitMask;
		};

		struct DdsHeader
		{
			uint32_t       m_size;
			uint32_t       m_flags;
			uint32_t       m_height;
			uint32_t       m_width;
			uint32_t       m_pitchOrLinearSize;
			uint32_t       m_depth;
			uint32_t       m_mipMapCount;
			uint32_t       m_reserved1[11];
			DdsPixelFormat m_pixelFormat;
			uint32_t       m_caps;
			uint32_t       m_caps2;
			uint32_t       m_caps3;
			uint32_t       m_caps4;
			uint32_t       m_reserved2;
		};
		
		struct DdsHeader10
		{
		  DXGI_FORMAT    m_format;
		  uint32_t       m_resourceDimension;
		  uint32_t       m_miscFlag;
		  uint32_t       m_arraySize;
		  uint32_t       m_miscFlags2;
		};
		
		struct DdsHeaderLayout
		{
			uint32_t       m_magic;
			DdsHeader      m_header;
		};

		struct DdsHeaderLayout10
		{
			uint32_t       m_magic;
			DdsHeader      m_header;
			DdsHeader10    m_header10;
		};

		GfxFormat GetFormat( const DdsPixelFormat& pixelFormat )
		{
			if (pixelFormat.m_flags & DDPF_RGB)
			{
				switch (pixelFormat.m_RGBBitCount)
				{
				case 32:
					if (pixelFormat.m_RBitMask == 0x000000ff &&
						pixelFormat.m_GBitMask == 0x0000ff00 &&
						pixelFormat.m_BBitMask == 0x00ff0000 &&
						pixelFormat.m_ABitMask == 0xff000000 )
					{
						return GfxFormat::R8G8B8A8_Unorm;
					}
					
					if (pixelFormat.m_RBitMask == 0x00ff0000 &&
						pixelFormat.m_GBitMask == 0x0000ff00 &&
						pixelFormat.m_BBitMask == 0x000000ff &&
						pixelFormat.m_ABitMask == 0xff000000 )
					{
						return GfxFormat::B8G8R8A8_Unorm;
					}
					
					if (pixelFormat.m_RBitMask == 0x00ff0000 &&
						pixelFormat.m_GBitMask == 0x0000ff00 &&
						pixelFormat.m_BBitMask == 0x000000ff &&
						pixelFormat.m_ABitMask == 0x00000000 )
					{
						return GfxFormat::B8G8R8X8_Unorm;
					}
					
					if (pixelFormat.m_RBitMask == 0x3ff00000 &&
						pixelFormat.m_GBitMask == 0x000ffc00 &&
						pixelFormat.m_BBitMask == 0x000003ff &&
						pixelFormat.m_ABitMask == 0xc0000000 )
					{
						return GfxFormat::R10G10B10A2_Unorm;
					}
					
					if (pixelFormat.m_RBitMask == 0x0000ffff &&
						pixelFormat.m_GBitMask == 0xffff0000 &&
						pixelFormat.m_BBitMask == 0x00000000 &&
						pixelFormat.m_ABitMask == 0x00000000 )
					{
						return GfxFormat::R16G16_Unorm;
					}
					
					if (pixelFormat.m_RBitMask == 0xffffffff &&
						pixelFormat.m_GBitMask == 0x00000000 &&
						pixelFormat.m_BBitMask == 0x00000000 &&
						pixelFormat.m_ABitMask == 0x00000000 )
					{
						return GfxFormat::R32_Float;
					}
					break;

				case 16:
					if (pixelFormat.m_RBitMask == 0x7c00 &&
						pixelFormat.m_GBitMask == 0x03e0 &&
						pixelFormat.m_BBitMask == 0x001f &&
						pixelFormat.m_ABitMask == 0x8000 )
					{
						return GfxFormat::B5G5R5A1_Unorm;
					}

					if (pixelFormat.m_RBitMask == 0xf800 &&
						pixelFormat.m_GBitMask == 0x07e0 &&
						pixelFormat.m_BBitMask == 0x001f &&
						pixelFormat.m_ABitMask == 0x0000 )
					{
						return GfxFormat::B5G6R5_Unorm;
					}

					//if (pixelFormat.m_RBitMask == 0x0f00 &&
					//	pixelFormat.m_GBitMask == 0x00f0 &&
					//	pixelFormat.m_BBitMask == 0x000f &&
					//	pixelFormat.m_ABitMask == 0xf000 )
					//{
					//	return GfxFormat::B4G4R4A4;
					//}

					break;
				}
			}
			else if (pixelFormat.m_flags & DDPF_LUMINANCE)
			{
				if (pixelFormat.m_RGBBitCount == 8)
				{
					if (pixelFormat.m_RBitMask == 0x000000ff &&
						pixelFormat.m_GBitMask == 0x00000000 &&
						pixelFormat.m_BBitMask == 0x00000000 &&
						pixelFormat.m_ABitMask == 0x00000000 )
					{
						return GfxFormat::R8_Unorm;
					}
				}

				if (pixelFormat.m_RGBBitCount == 16)
				{
					if (pixelFormat.m_RBitMask == 0x0000ffff &&
						pixelFormat.m_GBitMask == 0x00000000 &&
						pixelFormat.m_BBitMask == 0x00000000 &&
						pixelFormat.m_ABitMask == 0x00000000 )
					{
						return GfxFormat::R16_Unorm;
					}

					if (pixelFormat.m_RBitMask == 0x000000ff &&
						pixelFormat.m_GBitMask == 0x00000000 &&
						pixelFormat.m_BBitMask == 0x00000000 &&
						pixelFormat.m_ABitMask == 0x0000ff00 ) // 元々AチャンネルにGのマスクがあるみたい...
					{
						return GfxFormat::R8G8_Unorm;
					}
				}
			}
			else if (pixelFormat.m_flags & DDPF_ALPHA)
			{
				if (pixelFormat.m_RGBBitCount == 8)
				{
					return GfxFormat::A8_Unorm;
				}
			}
			else if (pixelFormat.m_flags & DDPF_FOURCC)
			{
				switch( pixelFormat.m_fourCC )
				{
				case FourCC("DXT1"):
					return GfxFormat::BC1_Unorm;

				case FourCC("DXT2"): // pre-mulitplied alpha
				case FourCC("DXT3"):
					return GfxFormat::BC2_Unorm;

				case FourCC("DXT4"): // pre-mulitplied alpha
				case FourCC("DXT5"):
					return GfxFormat::BC3_Unorm;

				case FourCC("ATI1"):
				case FourCC("BC4U"):
					return GfxFormat::BC4_Unorm;

				case FourCC("BC4S"):
					return GfxFormat::BC4_Snorm;

				case FourCC("ATI2"):
				case FourCC("BC5U"):
					return GfxFormat::BC5_Unorm;
					
				case FourCC("BC5S"):
					return GfxFormat::BC5_Snorm;
					
				case FourCC("RGBG"):
					return GfxFormat::R8G8_B8G8_Unorm;
				
				case FourCC("GRGB"):
					return GfxFormat::G8R8_G8B8_Unorm;
					
				//case FourCC("YUY2"):
				//	return GfxFormat::YUY2;

				case 36: // D3DFMT_A16B16G16R16
					return GfxFormat::R16G16B16A16_Unorm;

				case 110: // D3DFMT_Q16W16V16U16
					return GfxFormat::R16G16B16A16_Snorm;

				case 111: // D3DFMT_R16F
					return GfxFormat::R16_Float;

				case 112: // D3DFMT_G16R16F
					return GfxFormat::R16G16_Float;

				case 113: // D3DFMT_A16B16G16R16F
					return GfxFormat::R16G16B16A16_Float;
					//return GfxFormat::A16B16G16R16_Float;

				case 114: // D3DFMT_R32F
					return GfxFormat::R32_Float;

				case 115: // D3DFMT_G32R32F
					return GfxFormat::R32G32_Float; // G32B32ではないようだ.

				case 116: // D3DFMT_A32B32G32R32F
					return GfxFormat::R32G32B32A32_Float;
				}
			}

			return GfxFormat::Unknown;
		}
	}

	int LoadDdsFromMemory(
		GfxDdsMetaData&  outDdsMeta,
		const void*      ddsBuffer,
		size_t           ddsBufferSize)
	{
		if(ddsBufferSize <= sizeof(DdsHeaderLayout))
		{
			SI_WARNING(0, "DDS ERROR: Not dds format.");
			return -1;
		}

		const DdsHeaderLayout& layout = *static_cast<const DdsHeaderLayout*>(ddsBuffer);

		if(layout.m_magic != kDdsMagic)
		{
			SI_WARNING(0, "DDS ERROR: Not dds format.");
			return -1;
		}
				
		const DdsHeader& header = layout.m_header;
		
		if(header.m_size != sizeof(DdsHeader))
		{
			SI_WARNING(0, "DDS ERROR: Not dds format.");
			return -1;
		}

		if(header.m_pixelFormat.m_size != sizeof(DdsPixelFormat))
		{
			SI_WARNING(0, "DDS ERROR: Not dds format.");
			return -1;
		}
		
		static const uint32_t kRequiredFlag = DDSD_CAPS | DDSD_WIDTH | DDSD_PIXELFORMAT;
		if((header.m_flags & kRequiredFlag) != kRequiredFlag)
		{
			SI_WARNING(0, "DDS ERROR: Not dds format.");
		}

		size_t       offset    = sizeof(DdsHeaderLayout);
		uint32_t     width     = header.m_width;
		uint32_t     height    = (header.m_flags & DDSD_HEIGHT)? Max(header.m_height, 1u) : 1;
		uint32_t     depth     = (header.m_flags & DDSD_DEPTH)?  Max(header.m_depth,  1u) : 1;
		uint32_t     arraySize = 1;
		GfxFormat    format    = GfxFormat::Unknown;
		GfxDimension dimension = GfxDimension::Max;
		uint32_t     mipLevel  = Max(header.m_mipMapCount, 1u);

		if( (header.m_pixelFormat.m_flags & DDPF_FOURCC) &&
			(header.m_pixelFormat.m_fourCC == kDX10FourCC) )
		{
			if(ddsBufferSize <= sizeof(DdsHeaderLayout10))
			{
				SI_WARNING(0, "DDS ERROR: Not dds DX10 format.");
				return -1;
			}
			offset = sizeof(DdsHeaderLayout10);

			const DdsHeaderLayout10& layout10 = *static_cast<const DdsHeaderLayout10*>(ddsBuffer);
			
			format = GetFormat( layout10.m_header10.m_format );			
			if(format == GfxFormat::Unknown)
			{
				SI_WARNING(0, "DDS ERROR: Not dds DX10 format.");
				return -1;
			}
			
			arraySize = layout10.m_header10.m_arraySize;

			switch ( layout10.m_header10.m_resourceDimension )
			{
			case DDRD_TEXTURE1D:
				if(height != 1 || depth != 1)
				{
					SI_WARNING(0, "DDS ERROR: Not 1d dds format.");
					return -1;
				}
				dimension = (1<arraySize)? GfxDimension::Texture1DArray : GfxDimension::Texture1D;
				break;

			case DDRD_TEXTURE2D:
				if(depth != 1)
				{
					SI_WARNING(0, "DDS ERROR: Not 2d dds format.");
					return -1;
				}

				if(layout10.m_header10.m_miscFlag & DDSM_TEXTURECUBE)
				{
					dimension = (1<arraySize)? GfxDimension::TextureCubeArray : GfxDimension::TextureCube;
				}
				else
				{
					dimension = (1<arraySize)? GfxDimension::Texture2DArray : GfxDimension::Texture2D;
				}
				break;

			case DDRD_TEXTURE3D:
				if (!(layout10.m_header.m_flags & DDSD_DEPTH))
				{
					SI_WARNING(0, "DDS ERROR: Not 3d dds format.");
					return -1;
				}

				if (0 < arraySize)
				{
					SI_WARNING(0, "DDS ERROR: Not 3d dds format.");
					return -1;
				}
				
				dimension = GfxDimension::Texture3D;
				break;

			default:
				SI_WARNING(0, "DDS ERROR: Not dds format.");
				return -1;
			}
		}
		else
		{
			format = GetFormat( header.m_pixelFormat );
			
			if(format == GfxFormat::Unknown)
			{
				SI_WARNING(0, "DDS ERROR: Not dds format.");
				return -1;
			}
			
			if (header.m_flags & DDSD_DEPTH)
			{
				dimension = GfxDimension::Texture3D;
			}
			else 
			{
				if (header.m_caps2 & DDSCAPS2_CUBEMAP)
				{
					// We require all six faces to be defined
					if ((header.m_caps2 & DDSCAPS2_CUBEMAP_ALLFACES ) != DDSCAPS2_CUBEMAP_ALLFACES)
					{
						SI_WARNING(0, "DDS ERROR: Not cube dds format.");
						return -1;
					}

					dimension = GfxDimension::TextureCube;
				}
				else
				{
					dimension = GfxDimension::Texture2D;
				}
			}
		}

		SI_ASSERT(dimension != GfxDimension::Max);


		outDdsMeta.m_image      = (const void*)((uintptr_t)ddsBuffer + offset);
		outDdsMeta.m_imageSize  = ddsBufferSize - offset;
		outDdsMeta.m_width      = width;
		outDdsMeta.m_height     = height;
		outDdsMeta.m_depth      = depth;
		outDdsMeta.m_pitchOrLinearSize = header.m_pitchOrLinearSize;
		outDdsMeta.m_arraySize  = arraySize;
		outDdsMeta.m_mipLevel   = mipLevel;
		outDdsMeta.m_format     = format;
		outDdsMeta.m_dimension  = dimension;

		return 0;
	}

} // namespace SI
