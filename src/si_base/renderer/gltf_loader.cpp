
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

		void LoadBuffer(GfxBuffer& outBuffer, RootScene& rootScene, const glTF::Document& document, const glTF::Buffer& gltfBuffer)
		{
			SI_ASSERT(!outBuffer.IsValid());
			GfxDevice& device = *GfxDevice::GetInstance();

			std::vector<uint8_t> bufferData;

			std::string::const_iterator itBegin = gltfBuffer.uri.end();
			std::string::const_iterator itEnd   = gltfBuffer.uri.end();
			if(glTF::IsUriBase64(gltfBuffer.uri, itBegin, itEnd))
			{
				// embeded
				bufferData = std::move(glTF::Base64Decode(glTF::Base64StringView(itBegin, itEnd)));
			}
			else if(FileUtility::Load(bufferData, gltfBuffer.uri.c_str()) != 0)
			{
				SI_ASSERT("Failed to load buffer. %s is invalid URI", gltfBuffer.uri.c_str());
				return;
			}

			SI_ASSERT(bufferData.size() == gltfBuffer.byteLength);

			GfxBufferDesc desc;
			desc.m_name = gltfBuffer.name.c_str();
			desc.m_bufferSizeInByte = gltfBuffer.byteLength;
			desc.m_resourceStates = GfxResourceState::CopyDest;
			desc.m_resourceFlags = GfxResourceFlag::None;
			outBuffer = device.CreateBuffer(desc);
			device.UploadBufferLater(
				outBuffer,
				bufferData.data(),
				bufferData.size(),
				GfxResourceState::CopyDest,
				GfxResourceState::IndexBuffer | GfxResourceState::VertexAndConstantBuffer);
		}

		void LoadBufferView(GfxVertexBufferView& outBufferView, RootScene& rootScene, const glTF::Document& document, const glTF::BufferView& gltfBufferView)
		{
			int bufferId = GetId(gltfBufferView.bufferId);

			if(0<=bufferId && bufferId<document.buffers.Size())
			{
				outBufferView.SetBuffer(rootScene.GetBuffer(bufferId));
			}
			outBufferView.SetSize(gltfBufferView.byteLength);
			outBufferView.SetOffset(gltfBufferView.byteOffset);
			outBufferView.SetStride(gltfBufferView.byteStride.HasValue()? gltfBufferView.byteStride.Get() : 0);
		}

		void LoadAccessor(Accessor& outAccessor, RootScene& rootScene, const glTF::Document& document, const glTF::Accessor& gltfAccessor)
		{
			int bufferViewId = GetId(gltfAccessor.bufferViewId);

			if(0<=bufferViewId && bufferViewId<document.bufferViews.Size())
			{
				outAccessor.SetBufferViewId(bufferViewId);
			}

			outAccessor.SetCount(gltfAccessor.count);

			GfxFormat format = GetFormat(gltfAccessor.componentType, gltfAccessor.type);
			outAccessor.SetFormat(format);
		}

		void LoadMaterial(Material& outMatrial, RootScene& rootScene, const glTF::Document& document, const glTF::Material& gltfMaterial)
		{
			outMatrial.SetName( gltfMaterial.name.c_str() );
		}

		void LoadMesh(Mesh& outMesh, RootScene& rootScene, const glTF::Document& document, const glTF::Mesh& gltfMesh)
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

				size_t vertexAttributeCount = gltfSubMesh.attributes.size();
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

		void LoadNode(Node& outNode, RootScene& rootScene, const glTF::Document& document, const glTF::Node& gltfNode)
		{
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
				outNode.SetMesh(&rootScene.GetMesh(meshId));
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
			}
		}

		void LoadScene(Scene& outScene, RootScene& rootScene, const glTF::Document& document, const glTF::Scene& gltfScene)
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

		RootScenePtr Load(const char* filePath)
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
				return RootScenePtr();
			}

			glTF::Document document;
			document = glTF::Deserialize(manifest, glTF::DeserializeFlags::IgnoreByteOrderMark);

			RootScenePtr rootScene = std::make_shared<RootScene>();

			//OpenDefaultConsole();
			size_t sceneCount      = document.scenes.Size();
			size_t nodeCount       = document.nodes.Size();
			size_t meshCount       = document.meshes.Size();
			size_t materialCount   = document.materials.Size();
			size_t bufferCount     = document.buffers.Size();
			size_t accessorCount   = document.accessors.Size();
			size_t bufferViewCount = document.bufferViews.Size();

			GfxBufferDesc desc;
			desc.m_name              = nullptr;
			desc.m_bufferSizeInByte  = 256;
			desc.m_heapType          = GfxHeapType::Default;
			desc.m_resourceStates    = GfxResourceState::IndexBuffer | GfxResourceState::VertexAndConstantBuffer;
			desc.m_resourceFlags     = GfxResourceFlag::None;
			GfxBuffer buf = GfxDevice::GetInstance()->CreateBuffer(desc);

			// 下の階層の要素からセットアップしていく.
			rootScene->AllocateBuffers(bufferCount);
			for(size_t b=0; b<bufferCount; ++b)
			{
				const glTF::Buffer& gltfBuffer = document.buffers[b];
				LoadBuffer(rootScene->GetBuffer(b), *rootScene, document, gltfBuffer);
			}

			rootScene->AllocateBufferViews(bufferViewCount);
			for(size_t b=0; b<bufferViewCount; ++b)
			{
				const glTF::BufferView& gltfBufferView = document.bufferViews[b];
				LoadBufferView(rootScene->GetBufferView(b), *rootScene, document, gltfBufferView);
			}

			rootScene->AllocateAccessors(accessorCount);
			for(size_t a=0; a<accessorCount; ++a)
			{
				const glTF::Accessor& gltfAccessor = document.accessors[a];
				LoadAccessor(rootScene->GetAccessor(a), *rootScene, document, gltfAccessor);
			}

			rootScene->AllocateMaterials(materialCount);
			for(size_t m=0; m<materialCount; ++m)
			{
				const glTF::Material& gltfMaterial = document.materials[m];
				LoadMaterial(rootScene->GetMaterial(m), *rootScene, document, gltfMaterial);
			}

			rootScene->AllocateMeshes(meshCount);
			for(size_t m=0; m<meshCount; ++m)
			{
				const glTF::Mesh& gltfMesh = document.meshes[m];
				LoadMesh(rootScene->GetMesh(m), *rootScene, document, gltfMesh);
			}

			rootScene->AllocateNodes (nodeCount);
			for(size_t n=0; n<nodeCount; ++n)
			{
				const glTF::Node& gltfNode = document.nodes[n];
				LoadNode(rootScene->GetNode(n), *rootScene, document, gltfNode);
			}

			rootScene->AllocateScenes(sceneCount);
			for(size_t s=0; s<sceneCount; ++s)
			{
				const glTF::Scene& gltfScene = document.scenes[s];
				LoadScene(rootScene->GetScene(s), *rootScene, document, gltfScene);
			}

			if (document.scenes.Size() > 0U)
			{
				std::cout << "Default Scene Index: " << document.GetDefaultScene().id << "\n\n";
			}
			else
			{
				std::cout << "\n";
			}

			// Entity Info
			std::cout << "Node Count:     " << document.nodes.Size() << "\n";
			std::cout << "Camera Count:   " << document.cameras.Size() << "\n";
			std::cout << "Material Count: " << document.materials.Size() << "\n\n";

			// Mesh Info
			std::cout << "Mesh Count: " << document.meshes.Size() << "\n";
			std::cout << "Skin Count: " << document.skins.Size() << "\n\n";

			// Texture Info
			std::cout << "Image Count:   " << document.images.Size() << "\n";
			std::cout << "Texture Count: " << document.textures.Size() << "\n";
			std::cout << "Sampler Count: " << document.samplers.Size() << "\n\n";

			// Buffer Info
			std::cout << "Buffer Count:     " << document.buffers.Size() << "\n";
			std::cout << "BufferView Count: " << document.bufferViews.Size() << "\n";
			std::cout << "Accessor Count:   " << document.accessors.Size() << "\n\n";

			// Animation Info
			std::cout << "Animation Count: " << document.animations.Size() << "\n\n";

			for (const auto& extension : document.extensionsUsed)
			{
				std::cout << "Extension Used: " << extension << "\n";
			}

			if (!document.extensionsUsed.empty())
			{
				std::cout << "\n";
			}

			for (const auto& extension : document.extensionsRequired)
			{
				std::cout << "Extension Required: " << extension << "\n";
			}

			if (!document.extensionsRequired.empty())
			{
				std::cout << "\n";
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

	RootScenePtr GltfLoader::Load(const char* filePath)
	{
		return std::move(m_impl->Load(filePath));
	}

} // namespace SI
