
#include "si_base/renderer/gltf_loader.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLTFResourceReader.h>
#include <GLTFSDK/GLBResourceReader.h>
#include <GLTFSDK/Deserialize.h>
#include <GLTFSDK/ResourceReaderUtils.h>

#include "si_base/file/path.h"
#include "si_base/file/file_utility.h"
#include "si_base/core/assert.h"
#include "si_base/platform/windows_proxy.h"

namespace SI
{
	using namespace Microsoft;

	namespace
	{
		class StreamReader : public glTF::IStreamReader
		{
		public:
			StreamReader(const char* filePath) : m_filePath(filePath)
			{
			}

			std::shared_ptr<std::istream> GetInputStream(const std::string& filename) const override
			{
				SI_ASSERT(m_filePath);

				std::shared_ptr<std::ifstream> stream = std::make_shared<std::ifstream>(std::string(m_filePath), std::ios_base::binary);
				if (!stream || !(*stream))
				{
					SI_ASSERT(false, "failed to create file stream(%s)\n", m_filePath);
				}

				return stream;
			}

		private:
			const char* m_filePath;
		};

		Vfloat4x4 MathCast(const glTF::Matrix4& matrix)
		{
			return Vfloat4x4(matrix.values.data());
		}

		Vquat MathCast(const glTF::Quaternion& q)
		{
			return Vquat(q.x, q.y, q.w, q.z);
		}

		Vfloat3 MathCast(const glTF::Vector3& v)
		{
			return Vfloat3(v.x, v.y, v.z);
		}

		Vfloat3 MathCast(const glTF::Color3& v)
		{
			return Vfloat3(v.r, v.g, v.b);
		}

		Vfloat4 MathCast(const glTF::Color4& v)
		{
			return Vfloat4(v.r, v.g, v.b, v.a);
		}

		GfxFormat GetFormat(glTF::ComponentType component, glTF::AccessorType accessorType)
		{
#define FMT_KEY(c, a) ((uint64_t)c<<32 | (uint64_t)a)
			switch (FMT_KEY(component, accessorType))
			{
			case FMT_KEY(glTF::COMPONENT_FLOAT, glTF::TYPE_SCALAR):
				return GfxFormat::R32_Float;
			case FMT_KEY(glTF::COMPONENT_FLOAT, glTF::TYPE_VEC2):
				return GfxFormat::R32G32_Float;
			case FMT_KEY(glTF::COMPONENT_FLOAT, glTF::TYPE_VEC3):
				return GfxFormat::R32G32B32_Float;
			case FMT_KEY(glTF::COMPONENT_FLOAT, glTF::TYPE_VEC4):
				return GfxFormat::R32G32B32A32_Float;
			case FMT_KEY(glTF::COMPONENT_UNSIGNED_INT, glTF::TYPE_SCALAR):
				return GfxFormat::R32_Uint;
			case FMT_KEY(glTF::COMPONENT_UNSIGNED_INT, glTF::TYPE_VEC2):
				return GfxFormat::R32G32_Uint;
			case FMT_KEY(glTF::COMPONENT_UNSIGNED_INT, glTF::TYPE_VEC3):
				return GfxFormat::R32G32B32_Uint;
			case FMT_KEY(glTF::COMPONENT_UNSIGNED_INT, glTF::TYPE_VEC4):
				return GfxFormat::R32G32B32A32_Uint;
			case FMT_KEY(glTF::COMPONENT_SHORT, glTF::TYPE_SCALAR):
				return GfxFormat::R16_Sint;
			case FMT_KEY(glTF::COMPONENT_SHORT, glTF::TYPE_VEC2):
				return GfxFormat::R16_Sint;
			case FMT_KEY(glTF::COMPONENT_SHORT, glTF::TYPE_VEC4):
				return GfxFormat::R16G16B16A16_Sint;
			case FMT_KEY(glTF::COMPONENT_UNSIGNED_SHORT, glTF::TYPE_SCALAR):
				return GfxFormat::R16_Uint;
			case FMT_KEY(glTF::COMPONENT_UNSIGNED_SHORT, glTF::TYPE_VEC2):
				return GfxFormat::R16G16_Uint;
			case FMT_KEY(glTF::COMPONENT_UNSIGNED_SHORT, glTF::TYPE_VEC4):
				return GfxFormat::R16G16B16A16_Uint;
			case FMT_KEY(glTF::COMPONENT_BYTE, glTF::TYPE_SCALAR):
				return GfxFormat::R8_Sint;
			case FMT_KEY(glTF::COMPONENT_BYTE, glTF::TYPE_VEC2):
				return GfxFormat::R8_Sint;
			case FMT_KEY(glTF::COMPONENT_BYTE, glTF::TYPE_VEC4):
				return GfxFormat::R8G8B8A8_Sint;
			case FMT_KEY(glTF::COMPONENT_UNSIGNED_BYTE, glTF::TYPE_SCALAR):
				return GfxFormat::R8_Uint;
			case FMT_KEY(glTF::COMPONENT_UNSIGNED_BYTE, glTF::TYPE_VEC2):
				return GfxFormat::R8G8_Uint;
			case FMT_KEY(glTF::COMPONENT_UNSIGNED_BYTE, glTF::TYPE_VEC4):
				return GfxFormat::R8G8B8A8_Uint;
			default:
				break;
			}
#undef FMT_KEY

			SI_ASSERT(false);
			return GfxFormat::Unknown;
		}

		GfxSemantics GetSemantics(const std::string& str)
		{
			if(str == glTF::ACCESSOR_POSITION)
			{
				return GfxSemantics(GfxSemanticsType::Position, 0);
			}
			if(str == glTF::ACCESSOR_NORMAL)
			{
				return GfxSemantics(GfxSemanticsType::Normal, 0);
			}
			if(str == glTF::ACCESSOR_TANGENT)
			{
				return GfxSemantics(GfxSemanticsType::Tangent, 0);
			}
			if(str == glTF::ACCESSOR_TEXCOORD_0)
			{
				return GfxSemantics(GfxSemanticsType::UV, 0);
			}
			if(str == glTF::ACCESSOR_TEXCOORD_1)
			{
				return GfxSemantics(GfxSemanticsType::UV, 1);
			}
			if(str == glTF::ACCESSOR_COLOR_0)
			{
				return GfxSemantics(GfxSemanticsType::Color, 0);
			}

			SI_ASSERT(false);
			return GfxSemantics();
		}

		int GetId(const std::string& str)
		{
			if(str.empty())
			{
				return -1;
			}

			try
			{
				return std::stoi(str);
			}
			catch(...)
			{
				return -1;
			}
		}
	}

	class GltfLoaderImpl
	{
	public:
		GltfLoaderImpl()
		{
		}

		bool LoadBuffer(
			std::vector<uint8_t>& outBuffer,
			const glTF::Document& document,
			int bufferId)
		{
			if(bufferId<0 || document.buffers.Size()<=bufferId)
			{
				SI_ASSERT(false, "Failed to load buffer. BufferId(%d) is invalid", bufferId);
				return false;
			}

			const glTF::Buffer& gltfBuffer = document.buffers[bufferId];

			std::string::const_iterator itBegin = gltfBuffer.uri.end();
			std::string::const_iterator itEnd   = gltfBuffer.uri.end();
			if(glTF::IsUriBase64(gltfBuffer.uri, itBegin, itEnd))
			{
				// embeded
				outBuffer = std::move(glTF::Base64Decode(glTF::Base64StringView(itBegin, itEnd)));
			}
			else if(FileUtility::Load(outBuffer, gltfBuffer.uri.c_str()) != 0)
			{
				SI_ASSERT(false, "Failed to load buffer. %s is invalid URI", gltfBuffer.uri.c_str());
				return false;
			}

			return !outBuffer.empty();
		}

		bool LoadBufferView(
			BufferView& outBufferView,
			const glTF::Document& document,
			int bufferViewId)
		{
			if(bufferViewId<0 || document.bufferViews.Size()<=bufferViewId)
			{
				SI_ASSERT(false, "Failed to load BufferView. BufferViewId(%d) is invalid", bufferViewId);
				return false;
			}

			const glTF::BufferView& gltfBufferView = document.bufferViews[bufferViewId];

			int bufferId = GetId(gltfBufferView.bufferId);

			if(bufferId<0 || document.buffers.Size()<=bufferId)
			{
				SI_ASSERT(false, "Failed to load BufferView. BufferId(%d) is invalid", bufferId);
				return false;
			}

			if( (gltfBufferView.byteLength<=0) || 
				(document.buffers[bufferId].byteLength < (gltfBufferView.byteOffset+gltfBufferView.byteLength)))
			{
				SI_ASSERT(false, "Failed to load BufferView. Buffer size(%u) or offset(%u) is invalid",
					(uint32_t)gltfBufferView.byteLength, (uint32_t)gltfBufferView.byteOffset);
				return false;
			}

			outBufferView.SetBufferId(bufferId);
			outBufferView.SetSize(gltfBufferView.byteLength);
			outBufferView.SetOffset(gltfBufferView.byteOffset);
			outBufferView.SetStride(gltfBufferView.byteStride);

			return true;
		}

		bool LoadBufferFromBufferView(
			std::vector<uint8_t>& outBuffer,
			const glTF::Document& document,
			int bufferViewId,
			const std::vector<std::vector<uint8_t>>& bufferDataArray)
		{
			BufferView bufferView;
			if(!LoadBufferView(bufferView, document, bufferViewId))
			{
				return false;
			}

			const std::vector<uint8_t>& buffer = bufferDataArray[bufferView.GetBufferId()];
			outBuffer.resize(bufferView.GetSize());
			memcpy_s(outBuffer.data(), outBuffer.size(), &buffer[bufferView.GetOffset()], bufferView.GetSize());

			return true;
		}

		bool LoadGfxImage(
			GfxTexture& outTexture,
			const glTF::Document& document,
			const std::vector<std::vector<uint8_t>>& bufferDataArray,
			int imageId)
		{
			if(imageId<0 || document.images.Size()<=imageId)
			{
				SI_ASSERT(false, "Failed to load image. ImageId(%d) is invalid", imageId);
				return false;
			}

			const glTF::Image& gltfImage = document.images[imageId];

			SI_ASSERT(!outTexture.IsValid());
			GfxDevice& device = *GfxDevice::GetInstance();

			std::vector<uint8_t> bufferData;
			int bufferViewId = GetId(gltfImage.bufferViewId);

			std::string::const_iterator itBegin = gltfImage.uri.end();
			std::string::const_iterator itEnd   = gltfImage.uri.end();
			if(glTF::IsUriBase64(gltfImage.uri, itBegin, itEnd))
			{
				// embeded
				bufferData = std::move(glTF::Base64Decode(glTF::Base64StringView(itBegin, itEnd)));
			}
			else if(0<=bufferViewId && bufferViewId<document.bufferViews.Size() )
			{
				if(!LoadBufferFromBufferView(bufferData,document, bufferViewId, bufferDataArray))
				{
					return false;
				}
			}
			else if(!gltfImage.uri.empty())
			{
				if(FileUtility::Load(bufferData, gltfImage.uri.c_str()) != 0)
				{
					return false;
				}
			}
			else
			{
				return false;
			}

			SI_ASSERT(!bufferData.empty());

			outTexture = device.CreateTextureWICAndUpload(gltfImage.name.c_str(), bufferData.data(), bufferData.size());

			return true;
		}

		void LoadTexture(
			TextureInfo& outTextureInfo,
			Scenes& rootScene,
			const glTF::Document& document,
			const glTF::Texture& gltfTexture)
		{
			outTextureInfo.SetImageId( GetId(gltfTexture.imageId) );
		}

		bool LoadGfxBufferFromBufferView(
			GfxBuffer& outBuffer,
			int bufferViewId,
			const glTF::Document& document,
			const std::vector<std::vector<uint8_t>>& bufferDataArray)
		{
			BufferView bufferView;
			if(!LoadBufferView(bufferView, document, bufferViewId))
			{
				return false;
			}

			int bufferId = bufferView.GetBufferId();
			if(bufferId<0 || document.buffers.Size()<=bufferId)
			{
				SI_ASSERT(false, "Failed to load buffer view. BufferId(%d) is invalid", bufferId);
				return false;
			}

			const std::vector<uint8_t>& bufferData = bufferDataArray[bufferId];

			SI_ASSERT( (bufferView.GetOffset() + bufferView.GetSize()) <= bufferData.size() );

			const glTF::Buffer& gltfBuffer = document.buffers[bufferId];

			GfxBufferDesc desc;
			desc.m_name = gltfBuffer.name.c_str();
			desc.m_bufferSizeInByte = bufferView.GetSize();
			desc.m_resourceStates = GfxResourceState::CopyDest;
			desc.m_resourceFlags = GfxResourceFlag::None;

			GfxDevice& device = *GfxDevice::GetInstance();
			outBuffer = device.CreateBuffer(desc);
			int ret = device.UploadBufferLater(
				outBuffer,
				&bufferData[bufferView.GetOffset()],
				bufferView.GetSize(),
				GfxResourceState::CopyDest,
				GfxResourceState::IndexBuffer | GfxResourceState::VertexAndConstantBuffer);

			return (ret == 0);
		}

		bool LoadAccessor(
			Accessor& outAccessor,
			Scenes& rootScene,
			const glTF::Document& document,
			const glTF::Accessor& gltfAccessor,
			const std::vector<std::vector<uint8_t>>& bufferDataArray)
		{
			int bufferViewId = GetId(gltfAccessor.bufferViewId);

			if(!LoadGfxBufferFromBufferView(
				outAccessor.GetBuffer(),
				bufferViewId,
				document,
				bufferDataArray))
			{
				return false;
			}

			outAccessor.SetCount((uint32_t)gltfAccessor.count);

			GfxFormat format = GetFormat(gltfAccessor.componentType, gltfAccessor.type);
			outAccessor.SetFormat(format);
			return true;
		}

		void LoadMaterial(Material& outMaterial, Scenes& rootScene, const glTF::Document& document, const glTF::Material& gltfMaterial)
		{
			outMaterial.SetName( gltfMaterial.name.c_str() );

			outMaterial.SetBaseColorTextureId( GetId(gltfMaterial.metallicRoughness.baseColorTexture.textureId) );
			outMaterial.SetNormalTextureId( GetId(gltfMaterial.normalTexture.textureId));
			outMaterial.SetMetallicRoughnessTextureId( GetId(gltfMaterial.metallicRoughness.metallicRoughnessTexture.textureId) );
			outMaterial.SetEmissiveTextureId( GetId(gltfMaterial.emissiveTexture.textureId) );

			outMaterial.SetBaseColorFactor( MathCast(gltfMaterial.metallicRoughness.baseColorFactor) );
			outMaterial.SetNormalFactor( gltfMaterial.normalTexture.scale );
			outMaterial.SetMetallicFactor( gltfMaterial.metallicRoughness.metallicFactor );
			outMaterial.SetRoughnessFactor( gltfMaterial.metallicRoughness.roughnessFactor );
			outMaterial.SetEmissiveFactor( MathCast(gltfMaterial.emissiveFactor));

			outMaterial.Setup();
		}

		void LoadMesh(Mesh& outMesh, Scenes& rootScene, const glTF::Document& document, const glTF::Mesh& gltfMesh)
		{
			outMesh.SetName( gltfMesh.name.c_str() );
			
			size_t subMeshCount = gltfMesh.primitives.size();
			outMesh.ReserveSubmeshes(subMeshCount);
			for(size_t sm=0; sm<subMeshCount; ++sm)
			{
				const glTF::MeshPrimitive& gltfSubMesh = gltfMesh.primitives[sm];

				SubMesh* subMesh = outMesh.CreateNewSubMesh();

				SI::GfxPrimitiveTopology topology = SI::GfxPrimitiveTopology::TriangleList;
				switch(gltfSubMesh.mode)
				{
				case glTF::MESH_POINTS:
					topology = SI::GfxPrimitiveTopology::PointList;
					break;
				case glTF::MESH_LINES:
					topology = SI::GfxPrimitiveTopology::LineList;
					break;
				case glTF::MESH_LINE_LOOP:
					topology = SI::GfxPrimitiveTopology::LineStrip;
					SI_ASSERT(false, "unsupported topology: glTF::MESH_LINE_LOOP.");
					break;
				case glTF::MESH_LINE_STRIP:
					topology = SI::GfxPrimitiveTopology::LineStrip;
					break;
				case glTF::MESH_TRIANGLES:
					topology = SI::GfxPrimitiveTopology::TriangleList;
					break;
				case glTF::MESH_TRIANGLE_STRIP:
					topology = SI::GfxPrimitiveTopology::TriangleStrip;
					break;
				case glTF::MESH_TRIANGLE_FAN:
					topology = SI::GfxPrimitiveTopology::TriangleStrip;
					SI_ASSERT(false, "unsupported topology: glTF::MESH_TRIANGLE_FAN.");
					break;
				}

				subMesh->SetTopology(topology);

				int materialId = GetId(gltfSubMesh.materialId);
				if(0 <= materialId && materialId < document.materials.Size())
				{
					subMesh->SetMaterialId(materialId);
				}
				else
				{
					SI_ASSERT("material id is invalid.");
				}

				int indicesAccessorId = GetId(gltfSubMesh.indicesAccessorId);
				if(0 <= indicesAccessorId && indicesAccessorId < document.accessors.Size())
				{
					subMesh->SetIndicesAccessorId(indicesAccessorId);
				}

				uint32_t vertexAttributeCount = (uint32_t)gltfSubMesh.attributes.size();
				subMesh->ReserveVertexAttribute(vertexAttributeCount);
				for(auto itr=gltfSubMesh.attributes.begin(); itr!=gltfSubMesh.attributes.end(); ++itr)
				{
					GfxSemantics semantics  = GetSemantics(itr->first);
					int accessorId          = GetId(itr->second);

					VertexAttribute vertexAttribute;
					vertexAttribute.m_semantics = semantics;
					if(0<=accessorId && accessorId<document.accessors.Size())
					{
						vertexAttribute.m_accessorId = accessorId;
					}

					subMesh->AddVertexAttribute(vertexAttribute);
				}
			}
		}

		void LoadNode( int nodeId, Scenes& rootScene, const glTF::Document& document )
		{
			const glTF::Node& gltfNode = document.nodes[nodeId];
			Node& outNode = rootScene.GetNode(nodeId);

			outNode.SetId(nodeId);
			outNode.SetName(gltfNode.name.c_str());

			if(gltfNode.GetTransformationType() == glTF::TRANSFORMATION_MATRIX)
			{
				outNode.SetMatrix(MathCast(gltfNode.matrix));
			}
			else if(gltfNode.GetTransformationType() == glTF::TRANSFORMATION_TRS)
			{
				Vfloat3   t(MathCast(gltfNode.translation));
				Vfloat3x3 r(MathCast(gltfNode.rotation));
				Vfloat3x3 s(Vfloat3x3::Scale(MathCast(gltfNode.scale)));

				Vfloat4x4 trs(r*s, t);
				outNode.SetMatrix(trs);
			}

			int meshId = GetId(gltfNode.meshId);
			if(0 <= meshId && meshId < document.meshes.Size())
			{
				outNode.SetMeshId(meshId);
			}

			size_t nodeCount = gltfNode.children.size();
			outNode.ReserveNodes(nodeCount);
			for(size_t n=0; n<nodeCount; ++n)
			{
				int childNodeId = GetId(gltfNode.children[n]);
				if(childNodeId<0 || document.nodes.Size() <= childNodeId)
				{
					SI_ASSERT(false);
					continue;
				}

				outNode.AddNodeId(childNodeId);
				rootScene.GetNode(nodeId).SetParentId(nodeId);
			}
		}

		void LoadScene(Scene& outScene, Scenes& rootScene, const glTF::Document& document, const glTF::Scene& gltfScene)
		{
			outScene.SetName( gltfScene.name.c_str() );

			size_t nodeCount = gltfScene.nodes.size();
			outScene.ReserveNodes(nodeCount);
			for(size_t n=0; n<nodeCount; ++n)
			{
				int nodeId = GetId(gltfScene.nodes[n]);
				if(nodeId < 0 || document.nodes.Size() <= nodeId) continue;

				outScene.AddNodeId(nodeId);
			}
		}

		ScenesPtr Load(const char* filePath)
		{
			String ext = PathUtility::GetExt(filePath).ToLower();

			std::unique_ptr<StreamReader> streamReader = std::make_unique<StreamReader>(filePath);
			String manifest;

			std::unique_ptr<glTF::GLTFResourceReader> resourceReader;
			if(ext==".gltf")
			{
				std::stringstream manifestStream;

				std::shared_ptr<std::istream> gltfStream = streamReader->GetInputStream(filePath); // Pass a UTF-8 encoded filename to GetInputString
				std::unique_ptr<glTF::GLTFResourceReader> gltfResourceReader = std::make_unique<glTF::GLTFResourceReader>(std::move(streamReader));

				// Read the contents of the glTF file into a string using a std::stringstream
				manifestStream << gltfStream->rdbuf();
				manifest = manifestStream.str();

				resourceReader = std::move(gltfResourceReader);
			}
			else if(ext==".glb")
			{
				std::shared_ptr<std::istream> glbStream = streamReader->GetInputStream(filePath);
				std::unique_ptr<glTF::GLBResourceReader> glbResourceReader = std::make_unique<glTF::GLBResourceReader>(std::move(streamReader), std::move(glbStream));

				manifest = glbResourceReader->GetJson(); // Get the manifest from the JSON chunk

				resourceReader = std::move(glbResourceReader);
			}

			if(!resourceReader)
			{
				return ScenesPtr();
			}

			glTF::Document document;
			document = glTF::Deserialize(manifest, glTF::DeserializeFlags::IgnoreByteOrderMark);

			ScenesPtr rootScene = Scenes::Create();

			size_t sceneCount       = document.scenes.Size();
			size_t nodeCount        = document.nodes.Size();
			size_t meshCount        = document.meshes.Size();
			size_t materialCount    = document.materials.Size();
			size_t bufferCount      = document.buffers.Size();
			size_t accessorCount    = document.accessors.Size();
			size_t bufferViewCount  = document.bufferViews.Size();
			size_t textureInfoCount = document.textures.Size();
			size_t imageCount       = document.images.Size();


			// 下の階層の要素からセットアップしていく.
			std::vector<std::vector<uint8_t>> bufferDataArray;
			bufferDataArray.resize(bufferCount);
			for(int b=0; b<(int)bufferCount; ++b)
			{
				std::vector<uint8_t>& bufferData = bufferDataArray[b];
				LoadBuffer(bufferData, document, b);
			}

			rootScene->AllocateImages(imageCount);
			for(size_t i=0; i<imageCount; ++i)
			{
				LoadGfxImage(rootScene->GetImage((uint32_t)i), document, bufferDataArray, (int)i);
			}

			rootScene->AllocateTextureInfos(textureInfoCount);
			for(size_t t=0; t<textureInfoCount; ++t)
			{
				const glTF::Texture& gltfTexture = document.textures[t];
				LoadTexture(rootScene->GetTextureInfo((uint32_t)t), *rootScene, document, gltfTexture);
			}

			rootScene->AllocateAccessors(accessorCount);
			for(size_t a=0; a<accessorCount; ++a)
			{
				const glTF::Accessor& gltfAccessor = document.accessors[a];
				LoadAccessor(rootScene->GetAccessor((uint32_t)a), *rootScene, document, gltfAccessor, bufferDataArray);
			}

			rootScene->AllocateMaterials(materialCount);
			for(size_t m=0; m<materialCount; ++m)
			{
				const glTF::Material& gltfMaterial = document.materials[m];
				LoadMaterial(rootScene->GetMaterial((uint32_t)m), *rootScene, document, gltfMaterial);
			}

			rootScene->AllocateMeshes(meshCount);
			for(size_t m=0; m<meshCount; ++m)
			{
				const glTF::Mesh& gltfMesh = document.meshes[m];
				LoadMesh(rootScene->GetMesh((uint32_t)m), *rootScene, document, gltfMesh);
			}

			rootScene->AllocateNodes (nodeCount);
			for(size_t n=0; n<nodeCount; ++n)
			{
				LoadNode((int)n, *rootScene, document);
			}

			rootScene->AllocateScenes(sceneCount);
			for(size_t s=0; s<sceneCount; ++s)
			{
				const glTF::Scene& gltfScene = document.scenes[s];
				LoadScene(rootScene->GetScene((uint32_t)s), *rootScene, document, gltfScene);
			}

			return rootScene;
		}
	};

	GltfLoader::GltfLoader()
		: m_impl(new GltfLoaderImpl)
	{
	}

	GltfLoader::~GltfLoader()
	{
		delete m_impl;
	}

	ScenesPtr GltfLoader::Load(const char* filePath)
	{
		return std::move(m_impl->Load(filePath));
	}

} // namespace SI
