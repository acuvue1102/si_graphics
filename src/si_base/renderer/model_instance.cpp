
#include "si_base/renderer/model_instance.h"
#include "si_base/renderer/render_item.h"
#include "si_base/gpu/gfx_utility.h"
#include "si_base/gpu/gfx_input_layout.h"

namespace SI
{
	ModelInstance::ModelInstance()
		: m_model( std::make_shared<Model>() )
	{
	}

	ModelInstance::ModelInstance(ModelPtr& model)
		: m_model(model)
	{
	}

	ModelInstance::~ModelInstance()
	{
		if(m_renderItems.IsValid())
		{
			RenderItem* rendeItem = m_renderItems.GetItemsAddr();
			SI_DELETE_ARRAY(rendeItem);
			m_renderItems.Reset();
		}
	}
	
	void ModelInstance::SetupRenderItem()
	{
		Array<Material*> materials  = GetMaterials();
		Array<Geometry*> geometries = GetGeometries();

		Array<SubMesh> subMeshArray = GetSubMeshes();
		uint32_t subMeshCount = subMeshArray.GetItemCount();
		m_renderItems.Setup(SI_NEW_ARRAY(RenderItem, subMeshCount), subMeshCount);

		for(uint32_t i=0; i<subMeshCount; ++i)
		{
			RenderItem& rendeItem = m_renderItems[i];
			SubMesh& subMesh = subMeshArray[i];
			
			ObjectIndex materialIndex = subMesh.GetMaterialIndex();
			if(materials.GetItemCount() <= materialIndex) continue;

			ObjectIndex geometryIndex = subMesh.GetGeometryIndex();
			if(geometries.GetItemCount() <= geometryIndex) continue;
			
			Material* material = materials[materialIndex];
			Geometry* geometry = geometries[geometryIndex];
			
			if(!material) continue;
			if(!geometry) continue;
			
			const GfxVertexShader& vs = material->GetVertexShader();
			const GfxPixelShader&  ps = material->GetPixelShader();
			
			GfxDescriptorRange shaderRanges[(int)GfxShaderType::Max * (int)GfxDescriptorRangeType::Max];
			GfxDescriptorHeapTable heapTables[(int)GfxShaderType::Max * (int)GfxDescriptorRangeType::Max];
			uint8_t shaderTableMap[(int)GfxShaderType::Max][(int)GfxDescriptorRangeType::Max];
			memset(shaderTableMap, 0xff, sizeof(shaderTableMap));

			uint8_t tableCount = 0;
			const GfxShader* shaders[] ={ &vs, &ps };
			for(size_t s=0; s<ArraySize(shaders); ++s)
			{
				const GfxShader* shader = shaders[s];
				if(!shader) continue;
				if(!shader->IsValid()) continue;

				GfxShaderType shaderType = shader->GetType();
				GfxShaderVisibility visibility = GetShaderVisibility(shaderType);

				const GfxShaderBindingResouceCount& binding = shader->GetBindingResourceCount();
				
				if(0<binding.m_constantCount)
				{
					static const GfxDescriptorRangeType kRangeType = GfxDescriptorRangeType::Cbv;
					GfxDescriptorRange& range = shaderRanges[tableCount];
					range.Set(
						kRangeType,
						binding.m_constantCount,
						0,
						GfxDescriptorRangeFlag::DataStaticWhileSetAtExecute | GfxDescriptorRangeFlag::DescriptorsVolatile);
					GfxDescriptorHeapTable& table = heapTables[tableCount];
					table.m_rangeCount = 1;
					table.m_ranges = &range;
					table.m_visibility = visibility;

					shaderTableMap[(int)shaderType][(int)kRangeType] = tableCount;
					++tableCount;
				}
				
				if(0<binding.m_srvBufferCount)
				{
					static const GfxDescriptorRangeType kRangeType = GfxDescriptorRangeType::Srv;
					GfxDescriptorRange& range = shaderRanges[tableCount];
					range.Set(
						kRangeType,
						binding.m_srvBufferCount,
						0,
						GfxDescriptorRangeFlag::DataStaticWhileSetAtExecute | GfxDescriptorRangeFlag::DescriptorsVolatile);
					GfxDescriptorHeapTable& table = heapTables[tableCount];
					table.m_rangeCount = 1;
					table.m_ranges = &range;
					table.m_visibility = visibility;
					shaderTableMap[(int)shaderType][(int)kRangeType] = tableCount;
					++tableCount;
				}
				
				if(0<binding.m_uavBufferCount)
				{
					static const GfxDescriptorRangeType kRangeType = GfxDescriptorRangeType::Uav;
					GfxDescriptorRange& range = shaderRanges[tableCount];
					range.Set(
						kRangeType,
						binding.m_uavBufferCount,
						0,
						GfxDescriptorRangeFlag::DataVolatile | GfxDescriptorRangeFlag::DescriptorsVolatile);
					GfxDescriptorHeapTable& table = heapTables[tableCount];
					table.m_rangeCount = 1;
					table.m_ranges = &range;
					table.m_visibility = visibility;
					shaderTableMap[(int)shaderType][(int)kRangeType] = tableCount;
					++tableCount;
				}
				
				if(0<binding.m_samplerCount)
				{
					static const GfxDescriptorRangeType kRangeType = GfxDescriptorRangeType::Sampler;
					GfxDescriptorRange& range = shaderRanges[tableCount];
					range.Set(
						GfxDescriptorRangeType::Sampler,
						binding.m_samplerCount,
						0,
						GfxDescriptorRangeFlag::DescriptorsVolatile);
					GfxDescriptorHeapTable& table = heapTables[tableCount];
					table.m_rangeCount = 1;
					table.m_ranges = &range;
					table.m_visibility = visibility;
					shaderTableMap[(int)shaderType][(int)kRangeType] = tableCount;
					++tableCount;
				}
			}

			GfxRootSignatureDesc sigDesc;
			sigDesc.m_name = "sig";
			sigDesc.m_tableCount = tableCount;
			sigDesc.m_tables = heapTables;

			GfxDevice& device = *GfxDevice::GetInstance();
			rendeItem.SetRootSignature( device.CreateRootSignature(sigDesc) );

			
			static const GfxInputElement kVertexElements[] =
			{
				{"POSITION",  0, GfxFormat::R32G32B32_Float,  0, 0},
				{"NORMAL",    0, GfxFormat::R32G32B32_Float,  0, 12},
				{"TEXCOORD",  0, GfxFormat::R32G32_Float,     0, 24},
			};

#if 0
			GfxGraphicsStateDesc stateDesc;
			stateDesc.m_cullMode           = GfxCullMode::Back;
			stateDesc.m_inputElements      = kVertexElements;
			stateDesc.m_inputElementCount  = (int)ArraySize(kVertexElements);
			stateDesc.m_rootSignature      = &m_rootSignatures[0].GetRootSignature();
			stateDesc.m_vertexShader       = &m_lambertVS;
			stateDesc.m_pixelShader        = &m_lambertPS;
			stateDesc.m_rtvFormats[0]      = GfxFormat::R8G8B8A8_Unorm;
			stateDesc.m_dsvFormat          = GfxFormat::D32_Float;
			stateDesc.m_depthEnable        = true;
			stateDesc.m_depthFunc          = GfxComparisonFunc::LessEqual;
			stateDesc.m_depthWriteMask     = GfxDepthWriteMask::All;
			m_graphicsStates = device.CreateGraphicsState(stateDesc);
#endif
		}
	}

} // namespace SI
