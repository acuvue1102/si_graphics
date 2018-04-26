
#include "gpu/gfx_config.h"

#if SI_USE_DX12

#include <d3d12.h>
#include "gpu/gfx_enum.h"

namespace SI
{
	const GfxResourceState GfxResourceState::kCommon                 ( D3D12_RESOURCE_STATE_COMMON );
	const GfxResourceState GfxResourceState::kVertexAndConstantBuffer( D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER );
	const GfxResourceState GfxResourceState::kIndexBuffer            ( D3D12_RESOURCE_STATE_INDEX_BUFFER );
	const GfxResourceState GfxResourceState::kRenderTarget           ( D3D12_RESOURCE_STATE_RENDER_TARGET );
	const GfxResourceState GfxResourceState::kUnorderedAccess        ( D3D12_RESOURCE_STATE_UNORDERED_ACCESS );
	const GfxResourceState GfxResourceState::kDepthWrite             ( D3D12_RESOURCE_STATE_DEPTH_WRITE );
	const GfxResourceState GfxResourceState::kDepthRead              ( D3D12_RESOURCE_STATE_DEPTH_READ );
	const GfxResourceState GfxResourceState::kNonPixelShaderResource ( D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE );
	const GfxResourceState GfxResourceState::kPixelShaderResource    ( D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE );
	const GfxResourceState GfxResourceState::kStreamOut              ( D3D12_RESOURCE_STATE_STREAM_OUT );
	const GfxResourceState GfxResourceState::kIndirectArgument       ( D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT );
	const GfxResourceState GfxResourceState::kCopyDest               ( D3D12_RESOURCE_STATE_COPY_DEST );
	const GfxResourceState GfxResourceState::kCopySource             ( D3D12_RESOURCE_STATE_COPY_SOURCE );
	const GfxResourceState GfxResourceState::kResolveDest            ( D3D12_RESOURCE_STATE_RESOLVE_DEST );
	const GfxResourceState GfxResourceState::kResolveSource          ( D3D12_RESOURCE_STATE_RESOLVE_SOURCE );

} // namespace SI

#endif // SI_USE_DX12
