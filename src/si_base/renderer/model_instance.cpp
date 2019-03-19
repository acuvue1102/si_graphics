
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
#if 0
		if(m_renderItems.IsValid())
		{
			RenderItem* rendeItem = m_renderItems.GetItemsAddr();
						
			GfxDevice& device = *GfxDevice::GetInstance();
			device.ReleaseGraphicsState(rendeItem->GetGraphicsState());

			SI_DELETE_ARRAY(rendeItem);
			m_renderItems.Reset();
		}
#endif
	}
	
	void ModelInstance::Setup()
	{
#if 0
		SI_ASSERT(!m_drawStageList.IsEmpty());

		ConstArray<Material*> materials  = GetMaterials();
		ConstArray<Geometry*> geometries = GetGeometries();
		ConstArray<SubMesh> subMeshArray = GetSubMeshes();
		uint32_t subMeshCount = subMeshArray.GetItemCount();

		m_drawStageList.Reserve(RendererDrawStageType_Max);
		for(uint32_t st=0; st<(uint32_t)RendererDrawStageType_Max; ++st)
		{
			RendererDrawStageType stageType = (RendererDrawStageType)st;

			for(uint32_t i=0; i<subMeshCount; ++i)
			{
				// TODO: ここでステージごとに振り分け、このステージで描画しないSubmeshならcontinueにする.


				RenderItem& renderItem = m_drawStageList.Add(stageType, i).GetRenderItem();
				const SubMesh& subMesh = subMeshArray[i];
			
				ObjectIndex materialIndex = subMesh.GetMaterialIndex();
				if(materials.GetItemCount() <= materialIndex) continue;

				ObjectIndex geometryIndex = subMesh.GetGeometryIndex();
				if(geometries.GetItemCount() <= geometryIndex) continue;
			
				const Material* material = materials[materialIndex];
				const Geometry* geometry = geometries[geometryIndex];
			
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

						for(int frameId=0; frameId<2; ++frameId)
						{
							RenderItemDescriptorHeap renderItemDescHeap = renderItem.GetDescriptorHeap(frameId, (int)s);
							renderItemDescHeap.m_constants;
						}
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
				renderItem.SetRootSignature( device.CreateRootSignature(sigDesc) );

				//GfxDescriptorHeapDesc descHeapDesc;
				//descHeapDesc.m_type            = GfxDescriptorHeapType::CbvSrvUav;
				//descHeapDesc.m_flag            = GfxDescriptorHeapFlag::ShaderVisible;
				//descHeapDesc.m_descriptorCount = 1;
				//GfxDescriptorHeap descHeap = device.CreateDescriptorHeap(descHeapDesc);

				//GfxConstantBufferViewDesc constBufViewDesc;
				//constBufViewDesc.m_buffer;
				//device.CreateConstantBufferView(descHeap, 0, constBufViewDesc);
			}
		}
#endif
	}
	
	void ModelInstance::SetupPSO(const RendererGraphicsStateDesc& desc)
	{
#if 0
		SI_ASSERT(m_renderItems.IsValid());

		ConstArray<Material*> materials  = GetMaterials();
		ConstArray<Geometry*> geometries = GetGeometries();

		ConstArray<SubMesh> subMeshArray = GetSubMeshes();

		uint32_t subMeshCount = subMeshArray.GetItemCount();
		for(uint32_t i=0; i<subMeshCount; ++i)
		{
			RenderItem& renderItem = m_renderItems[i];
			const SubMesh& subMesh = subMeshArray[i];
			
			ObjectIndex materialIndex = subMesh.GetMaterialIndex();
			if(materials.GetItemCount() <= materialIndex) continue;

			ObjectIndex geometryIndex = subMesh.GetGeometryIndex();
			if(geometries.GetItemCount() <= geometryIndex) continue;
			
			const Material* material = materials[materialIndex];
			const Geometry* geometry = geometries[geometryIndex];
			
			if(!material) continue;
			if(!geometry) continue;
			
			const GfxVertexShader& vs = material->GetVertexShader();
			const GfxPixelShader&  ps = material->GetPixelShader();

			const VertexLayout& layout = geometry->GetVertexLayout();
			
			uint32_t offset = 0;
			GfxInputElement vertexElements[kMaxVertexAttributeCount];
			for(uint8_t j=0; j<layout.m_attributeCount; ++j)
			{
				const VertexSemanticsAndFormat& vsf = layout.m_attributes[j];
				GfxInputElement& ve = vertexElements[j];
				ve = GfxInputElement(
					GetSemanticsName(vsf.m_semantics.m_semanticsType),
					vsf.m_semantics.m_semanticsIndex,
					vsf.m_format,
					0,
					offset);

				uint32_t formatBits = (uint32_t)GetFormatBits(vsf.m_format);
				SI_ASSERT(formatBits%8==0);
				
				offset += formatBits/8;
			}

			GfxDevice& device = *GfxDevice::GetInstance();

			GfxGraphicsStateDesc stateDesc;
			stateDesc.m_cullMode               = GfxCullMode::Back;
			stateDesc.m_inputElements          = vertexElements;
			stateDesc.m_inputElementCount      = (int)layout.m_attributeCount;
			stateDesc.m_rootSignature          = &renderItem.GetRootSignature();
			stateDesc.m_vertexShader           = &vs;
			stateDesc.m_pixelShader            = &ps;
			stateDesc.m_renderTargetCount      = desc.m_renderTargetCount;
			for(uint32_t j=0; j<desc.m_renderTargetCount; ++j)
			{
				stateDesc.m_rtvFormats[j]      = desc.m_rtvFormats[j];
			}
			stateDesc.m_dsvFormat              = desc.m_dsvFormat;
			stateDesc.m_depthEnable            = desc.m_depthEnable;
			stateDesc.m_depthWriteMask         = desc.m_depthWriteMask;
			stateDesc.m_depthFunc              = desc.m_depthFunc;			
			stateDesc.m_depthBias              = desc.m_depthBias;
			stateDesc.m_depthBiasClamp         = desc.m_depthBiasClamp;
			stateDesc.m_slopeScaledDepthBias   = desc.m_slopeScaledDepthBias;
			stateDesc.m_depthEnable            = desc.m_depthEnable;
			stateDesc.m_stencilEnable          = desc.m_stencilEnable;
			stateDesc.m_alphaToCoverageEnable  = desc.m_alphaToCoverageEnable;
			stateDesc.m_independentBlendEnable = desc.m_independentBlendEnable;

			renderItem.SetGraphicsState( device.CreateGraphicsState(stateDesc) );

			renderItem.SetGraphicsStateDesc( desc );
		}
#endif
	}

} // namespace SI
