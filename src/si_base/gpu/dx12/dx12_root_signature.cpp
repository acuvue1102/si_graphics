﻿
#include "si_base/gpu/dx12/dx12_root_signature.h"

#include <comdef.h>
#include "si_base/core/core.h"
#include "si_base/core/scope_exit.h"
#include "si_base/memory/pool_allocator.h"
#include "si_base/gpu/gfx_root_signature.h"
#include "si_base/gpu/dx12/dx12_enum.h"
#include "si_base/gpu/dx12/dx12_device.h"

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

	int BaseRootSignature::Initialize(BaseDevice& device, const GfxRootSignatureDesc& desc)
	{
		ID3D12Device& d3dDevice = *device.GetComPtrDevice().Get();
		
		PoolAllocatorEx* tempAllocator = device.GetTempAllocator();
		uint32_t parameterCount = desc.m_tableCount + desc.m_rootDescriptorCount;
		D3D12_ROOT_PARAMETER1* parameters = (0<parameterCount)? tempAllocator->NewArray<D3D12_ROOT_PARAMETER1>((size_t)parameterCount) : nullptr;
		for(uint32_t t=0; t<desc.m_tableCount; ++t)
		{
			D3D12_ROOT_PARAMETER1& outParameter = parameters[t];
			D3D12_ROOT_DESCRIPTOR_TABLE1& outTable = outParameter.DescriptorTable;
			const GfxDescriptorHeapTable& inTable = desc.m_tables[t];

			outParameter.ParameterType    = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			outParameter.ShaderVisibility = GetDx12ShaderVisibility(inTable.m_visibility);
			
			D3D12_DESCRIPTOR_RANGE1* ranges = (0<inTable.m_rangeCount)? tempAllocator->NewArray<D3D12_DESCRIPTOR_RANGE1>((size_t)inTable.m_rangeCount) : nullptr;

			for(uint32_t r=0; r<inTable.m_rangeCount; ++r)
			{
				D3D12_DESCRIPTOR_RANGE1&  outRange = ranges[r];
				const GfxDescriptorRange&  inRange = inTable.m_ranges[r];
				
				outRange.RangeType           = GetDx12DescriptorRangeType(inRange.m_rangeType);
				outRange.NumDescriptors      = inRange.m_descriptorCount;
				outRange.BaseShaderRegister  = inRange.m_shaderRegisterIndex;
				outRange.RegisterSpace       = 0;

				if(inRange.m_rangeType==GfxDescriptorRangeType::Sampler)
				{
					outRange.Flags           = GetDx12DescriptorRangeFlags(inRange.m_rangeFlags);//D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
				}
				else
				{
					outRange.Flags           = GetDx12DescriptorRangeFlags(inRange.m_rangeFlags);
				}
				outRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			}
			
			outTable.NumDescriptorRanges = inTable.m_rangeCount;
			outTable.pDescriptorRanges   = ranges;
		}
		
		for(uint32_t d=0; d<desc.m_rootDescriptorCount; ++d)
		{
			const GfxRootDescriptor& inDescriptor = desc.m_rootDescriptors[d];
			D3D12_ROOT_PARAMETER1& outParameter = parameters[desc.m_tableCount + d];
			D3D12_ROOT_DESCRIPTOR1& outDescriptor = outParameter.Descriptor;
			
			outParameter.ParameterType    = GetDx12RootParameterType(inDescriptor.m_type); 
			outParameter.ShaderVisibility = GetDx12ShaderVisibility(inDescriptor.m_visibility);
			outDescriptor.ShaderRegister = inDescriptor.m_shaderRegisterIndex;
			outDescriptor.RegisterSpace  = inDescriptor.m_registerSpace;
			outDescriptor.Flags          = GetDx12RootDescriptorFlags(inDescriptor.m_flags);
		}

		// このスコープを抜けると全て開放するように...
		SI_SCOPE_EXIT(
			for(uint32_t i=0; i<desc.m_tableCount; ++i)
			{
				tempAllocator->DeleteArray(parameters[i].DescriptorTable.pDescriptorRanges);
			}
			tempAllocator->DeleteArray(parameters);
		);
		
		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc1 = rootSignatureDesc.Desc_1_1;
		rootSignatureDesc1.NumParameters     = parameterCount;
		rootSignatureDesc1.pParameters       = parameters;
		rootSignatureDesc1.NumStaticSamplers = 0;
		rootSignatureDesc1.pStaticSamplers   = nullptr;
		rootSignatureDesc1.Flags             = GetDx12RootSignatureFlags(desc.m_flags);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> errCode;
		HRESULT hr = D3D12SerializeVersionedRootSignature(
			&rootSignatureDesc,
			&signature,
			&errCode);
		if(FAILED(hr))
		{
			const char* errString = (const char*)errCode->GetBufferPointer();
			SI_ASSERT(0, "error D3D12SerializeRootSignature\n%s", errString);
			return -1;
		}

		hr = d3dDevice.CreateRootSignature(
			0,
			signature->GetBufferPointer(),
			signature->GetBufferSize(),
			IID_PPV_ARGS(&m_rootSignature));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateRootSignature\n%s", _com_error(hr).ErrorMessage());
			return -1;
		}
		
		if(desc.m_name)
		{
			wchar_t wName[64];
			wName[0] = 0;
			size_t num = 0;
			errno_t ret = mbstowcs_s(&num, wName, desc.m_name, ArraySize(wName));
			if(ret == 0)
			{
				m_rootSignature->SetName(wName);
			}
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
