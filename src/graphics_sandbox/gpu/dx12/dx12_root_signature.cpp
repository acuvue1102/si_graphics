
#include <comdef.h>
#include "base/base.h"
#include "gpu/dx12/dx12_root_signature.h"

#if SI_USE_DX12
namespace SI
{
	BaseRootSignature::BaseRootSignature()
	{
	}

	BaseRootSignature::~BaseRootSignature()
	{
		Terminate();
	}

	int BaseRootSignature::Initialize(ID3D12Device& device)
	{
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.NumParameters     = 0;
		rootSignatureDesc.pParameters       = nullptr;
		rootSignatureDesc.NumStaticSamplers = 0;
		rootSignatureDesc.pStaticSamplers   = nullptr;
		rootSignatureDesc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> errCode;
		HRESULT hr = D3D12SerializeRootSignature(
			&rootSignatureDesc,
			D3D_ROOT_SIGNATURE_VERSION_1,
			&signature,
			&errCode);
		if(FAILED(hr))
		{
			const char* errString = (const char*)errCode->GetBufferPointer();
			SI_ASSERT(0, "error D3D12SerializeRootSignature\n%s", errString);
			return -1;
		}

		hr = device.CreateRootSignature(
			0,
			signature->GetBufferPointer(),
			signature->GetBufferSize(),
			IID_PPV_ARGS(&m_rootSignature));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateRootSignature", _com_error(hr).ErrorMessage());
			return -1;
		}

		return 0;
	}

	int BaseRootSignature::Terminate()
	{
		m_rootSignature.Reset();
		return 0;
	}

} // namespace SI

#endif // SI_USE_DX12
