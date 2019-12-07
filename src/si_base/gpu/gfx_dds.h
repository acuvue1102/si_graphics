#pragma once

#include "si_base/core/core.h"
#include "si_base/gpu/gfx_enum.h"

namespace SI
{
	struct GfxDdsMetaData
	{
		const void*  m_image      = nullptr; // headerを取り除いたimage.
		size_t       m_imageSize  = 0;
		uint32_t     m_width      = 1;
		uint32_t     m_height     = 1;
		uint32_t     m_depth      = 1;
		uint32_t     m_pitchOrLinearSize = 0; // 無圧縮の時はスキャンラインのバイトサイズ. 圧縮時はトップレベルのバイトサイズ.
		uint32_t     m_arraySize  = 1;
		uint32_t     m_mipLevel   = 1;
		GfxFormat    m_format     = GfxFormat::Unknown;
		GfxDimension m_dimension  = GfxDimension::Max;
	};

	// ddsを読んで、ヘッダー情報とイメージ部分のバッファに分ける.
	int LoadDdsFromMemory(
		GfxDdsMetaData&  outDdsMeta,
		const void*      ddsBuffer,
		size_t           ddsBufferSize);

} // namespace SI
