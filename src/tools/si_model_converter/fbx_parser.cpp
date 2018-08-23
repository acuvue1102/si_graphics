
#include "fbx_parser.h"

#include <fbxsdk.h>
#include <array>
#include <vector>
#include <unordered_map>

#include "si_base/file/file.h"
#include "si_base/core/print.h"
#include "si_base/core/assert.h"
#include "si_base/core/scope_exit.h"
#include "si_base/misc/hash.h"

#include "si_base/math/math.h"
#include "si_base/renderer/model.h"
#include "si_base/renderer/node.h"
#include "si_base/renderer/mesh.h"
#include "si_base/renderer/sub_mesh.h"
#include "si_base/renderer/material.h"

using namespace fbxsdk;

namespace
{
	/// 1/255までの誤差を無視する.
	static const double kRoundScale = 255.0;

	class HashKeyVertex
	{
	public:
		HashKeyVertex()
		{
			memset(this, 0, sizeof(HashKeyVertex));
		}
			
		void SetPosition(double x, double y, double z)
		{
			m_position = {x, y, z};
		}
			
		void SetNormal(double x, double y, double z)
		{
			m_normal = {x, y, z};
			m_hasNormal = 1;
		}
			
		void AppendUv(double x, double y)
		{
			SI_ASSERT(m_uvCount<(uint32_t)SI::ArraySize(m_uvs));
			m_uvs[m_uvCount++] = {x, y};
		}

		void AppendTangent(double x, double y, double z, double flip)
		{
			SI_ASSERT(m_tangentCount<(uint32_t)SI::ArraySize(m_tangents));
			m_tangents[m_tangentCount++] = {x, y, z, flip};
		}

		void AppendColor(double r, double g, double b, double a)
		{
			SI_ASSERT(m_colorCount<(uint32_t)SI::ArraySize(m_colors));
			m_colors[m_colorCount++] = {r, g, b, a};
		}

		void AppendToFloatArray(std::vector<float>& outArray) const
		{
			outArray.push_back((float)m_position[0]);
			outArray.push_back((float)m_position[1]);
			outArray.push_back((float)m_position[2]);
			
			if(m_hasNormal)
			{
				outArray.push_back((float)m_normal[0]);
				outArray.push_back((float)m_normal[1]);
				outArray.push_back((float)m_normal[2]);
			}

			for(size_t i=0; i<m_uvCount; ++i)
			{
				Double2 uv = m_uvs[i];
				outArray.push_back((float)uv[0]);
				outArray.push_back((float)uv[1]);
			}

			for(size_t i=0; i<m_tangentCount; ++i)
			{
				Double4 tangent = m_tangents[i];
				outArray.push_back((float)tangent[0]);
				outArray.push_back((float)tangent[1]);
				outArray.push_back((float)tangent[2]);
				outArray.push_back((float)tangent[3]);
			}

			for(size_t i=0; i<m_colorCount; ++i)
			{
				Double4 color = m_colors[i];
				outArray.push_back((float)color[0]);
				outArray.push_back((float)color[1]);
				outArray.push_back((float)color[2]);
				outArray.push_back((float)color[3]);
			}
		}

		uint32_t GetElementCount() const
		{
			uint32_t element = 3;
			
			if(m_hasNormal)
			{
				element += 3;
			}
			
			element += m_uvCount * 2;
			element += m_tangentCount * 4;
			element += m_colorCount * 4;

			return element;
		}

		void GenerateHash()
		{
			SI::Hash64Generator hashGenerator;
			Int3 positionKey = Round(m_position);
			hashGenerator.Add(positionKey);

			if(m_hasNormal)
			{
				Int3 normalKey   = Round(m_normal);
				hashGenerator.Add(normalKey);
			}

			for(size_t i=0; i<m_uvCount; ++i)
			{
				Int2 uvKey = Round(m_uvs[i]);
				hashGenerator.Add(uvKey);
			}

			for(size_t i=0; i<m_tangentCount; ++i)
			{
				Int4 tangentKey = Round(m_tangents[i]);
				hashGenerator.Add(tangentKey);
			}

			for(size_t i=0; i<m_colorCount; ++i)
			{
				Int4 colorKey = Round(m_colors[i]);
				hashGenerator.Add(colorKey);
			}

			m_hash = hashGenerator.Generate();
		}

		SI::Hash64 GetHash() const
		{
			return m_hash;
		}

		bool operator==(const HashKeyVertex& rhs) const
		{
			if(m_hash != rhs.m_hash) return false;
				
			if(Round(m_position) != Round(rhs.m_position)) return false;
			if(Round(m_normal) != Round(rhs.m_normal)) return false;
				
			if(m_uvCount != m_uvCount) return false;
			for(size_t i=0; i<m_uvCount; ++i)
			{
				if(Round(m_uvs[i]) != Round(rhs.m_uvs[i])) return false;
			}
				
			if(m_tangentCount != m_tangentCount) return false;
			for(size_t i=0; i<m_tangentCount; ++i)
			{
				if(Round(m_tangents[i]) != Round(rhs.m_tangents[i])) return false;
			}
				
			if(m_colorCount != m_colorCount) return false;
			for(size_t i=0; i<m_colorCount; ++i)
			{
				if(Round(m_colors[i]) != Round(rhs.m_colors[i])) return false;
			}

			return true;
		}

		bool operator!=(const HashKeyVertex& rhs) const
		{
			return !(*this == rhs);
		}
		
	public:
		using Int4 = std::array<int64_t, 4>;
		using Int3 = std::array<int64_t, 3>;
		using Int2 = std::array<int64_t, 2>;
		using Double4 = std::array<double, 4>;
		using Double3 = std::array<double, 3>;
		using Double2 = std::array<double, 2>;
			
		static int64_t Round(double value)
		{
			return (int64_t)round(value * kRoundScale);
		}
		static Int2 Round(Double2 value)
		{
			return {Round(value[0]), Round(value[1])};
		}
		static Int3 Round(Double3 value)
		{
			return {Round(value[0]), Round(value[1]), Round(value[2])};
		}
		static Int4 Round(Double4 value)
		{
			return {Round(value[0]), Round(value[1]), Round(value[2]), Round(value[3])};
		}

	public:
		Double3 m_position;
		Double3 m_normal;
		Double2 m_uvs[8];
		Double4 m_tangents[8];
		Double4 m_colors[8];
			
		uint32_t m_hasNormal;
		uint32_t m_uvCount;
		uint32_t m_tangentCount;
		uint32_t m_colorCount;

		SI::Hash64 m_hash;
	};
	
	SI::Vfloat4 FxbVector4ToVfloat4(FbxVector4 v)
	{
		return SI::Vfloat4(
			(float)v.mData[0],
			(float)v.mData[1],
			(float)v.mData[2],
			(float)v.mData[3]);
	}

	SI::Vfloat4x4 FxbMatrixToVfloat4x4(FbxMatrix m)
	{
		return SI::Vfloat4x4(
			FxbVector4ToVfloat4( m.GetRow(0) ),
			FxbVector4ToVfloat4( m.GetRow(1) ),
			FxbVector4ToVfloat4( m.GetRow(2) ),
			FxbVector4ToVfloat4( m.GetRow(3) ) );
	}
}

namespace std
{
	// unordered_map用のオペレータ.
	template<>
	class hash<HashKeyVertex>
	{
	public:
		size_t operator () ( const HashKeyVertex &v ) const
		{
			return v.GetHash();
		}
	};
}

namespace SI
{
	ModelMetaBuffer::ModelMetaBuffer()
	{
	}

	ModelMetaBuffer::~ModelMetaBuffer()
	{
		Clear();
	}

	void ModelMetaBuffer::Clear()
	{
		for(Material* mate: m_materials)
		{
			Material::Release(mate);
		}
		for(Geometry* geo: m_geometries)
		{
			Geometry::Release(geo);
		}

		m_nodes.clear();
		m_nodeCores.clear();
		m_meshes.clear();
		m_subMeshes.clear();
		m_materials.clear();
		m_geometries.clear();
		m_vertexBuffers.clear();
		m_indexBuffers.clear();
		m_strings.clear();
	}

	void ModelMetaBuffer::Reserve(size_t count)
	{
		m_nodes.reserve(count);
		m_nodeCores.reserve(count);
		m_meshes.reserve(count);
		m_subMeshes.reserve(count);
		m_materials.reserve(count);
		m_geometries.reserve(count);
		m_vertexBuffers.reserve(count);
		m_indexBuffers.reserve(count);
		m_strings.reserve(count * 2 * 32);
	}


	/////////////////////////////////////////////////////////////////////

	FbxParser::FbxParser()
		: m_fbxManager(nullptr)
	{
	}

	FbxParser::~FbxParser()
	{
	}

	void FbxParser::Initialize()
	{
		if(m_fbxManager) return;
		
#if 0
		{
			constexpr Hash64 hashS = GetHash64S("12wedfghjuertyuikjhgfde45");
			SI_PRINT("hashS = 0x%llx\n", hashS);
			/////////////////////////////////////////
			Hash64 hash = GetHash64("12wedfghjuertyuikjhgfde45");
			SI_PRINT("hash  = 0x%llx\n", hash);

			const char* hoge = "12wedfghjuertyuikjhgfde45";
			Hash64 hash2 = GetHash64(hoge);
			SI_PRINT("hash2 = 0x%llx\n", hash2);

			const char* hoge2 = " 12wedfghjuertyuikjhgfde45";
			++hoge2;
			Hash64 hash3 = GetHash64(hoge2);
			SI_PRINT("hash3 = 0x%llx\n", hash3);

			Hash64Generator generator;
			const char* hoge3 = " 12wedfghjuert";
			++hoge3;
			generator.Add(hoge3);
			generator.Add("yuikj");
			generator.Add("hgfde45");
			Hash64 hash4 = generator.Generate();
			SI_PRINT("hash4 = 0x%llx\n", hash4);

			Hash64 hashT = GetHash64(*this);
			SI_PRINT("hashT = 0x%llx\n", hashT);
		}
		
		{
			constexpr Hash32 hashS = GetHash32S("12wedfghjuertyuikjhgfde45");
			SI_PRINT("hashS = 0x%llx\n", hashS);
			/////////////////////////////////////////
			Hash32 hash = GetHash32("12wedfghjuertyuikjhgfde45");
			SI_PRINT("hash  = 0x%llx\n", hash);

			const char* hoge = "12wedfghjuertyuikjhgfde45";
			Hash32 hash2 = GetHash32(hoge);
			SI_PRINT("hash2 = 0x%llx\n", hash2);

			const char* hoge2 = " 12wedfghjuertyuikjhgfde45";
			++hoge2;
			Hash32 hash3 = GetHash32(hoge2);
			SI_PRINT("hash3 = 0x%llx\n", hash3);

			Hash32Generator generator;
			generator.Add("12wedfghjuert");
			generator.Add("yuikj");
			generator.Add("hgfde45");
			Hash32 hash4 = generator.Generate();
			SI_PRINT("hash4 = 0x%llx\n", hash4);

			Hash32 hashT = GetHash32(*this);
			SI_PRINT("hashT = 0x%llx\n", hashT);
		}
#endif

		m_fbxManager = FbxManager::Create();

		m_fbxManager->SetIOSettings( FbxIOSettings::Create(m_fbxManager, IOSROOT) );

		m_fbxGeometryConverter = new FbxGeometryConverter(m_fbxManager);
	}

	void FbxParser::Terminate()
	{
		if(!m_fbxManager) return;

		delete m_fbxGeometryConverter;
		m_fbxGeometryConverter = nullptr;

		m_fbxManager->Destroy();
		m_fbxManager = nullptr;
	}

	int FbxParser::Parse(Model& outModel, const char* fbxPath)
	{
		FbxImporter* fbxImporter = FbxImporter::Create(m_fbxManager, "fbxImporter");
		SI_SCOPE_EXIT( fbxImporter->Destroy(); );

		if(!fbxImporter->Initialize(fbxPath, -1, m_fbxManager->GetIOSettings()))
		{
			SI_WARNING(0, "FbxImporter::Initialize() failed (%s).\n", fbxImporter->GetStatus().GetErrorString());
			return -1;
		}

		FbxScene* fbxScene = FbxScene::Create(m_fbxManager,"myScene");
		SI_SCOPE_EXIT( fbxScene->Destroy(); );

		if(!fbxImporter->Import(fbxScene))
		{
			SI_WARNING(0, "FbxImporter::Import() failed (%s).\n", fbxImporter->GetStatus().GetErrorString());
			return -1;
		}

		// 三角形化する.
		m_fbxGeometryConverter->Triangulate(fbxScene, true);

		FbxNode* fbxRootNode = fbxScene->GetRootNode();
		if(!fbxRootNode)
		{
			SI_WARNING(0, "No Root Node.\n");
			return -1;
		}

		// parseしてmetaBuffer内に必要なデータを集める.
		ModelMetaBuffer meta;
		meta.Reserve(fbxScene->GetNodeCount());
		Node rootNode;
		ParseNode(rootNode, meta, *fbxRootNode);

		// コピー.
		outModel.GetRootNode() = rootNode;
		if(!meta.m_nodes.empty())
		{
			outModel.AllocateNodes     ((ObjectIndex)meta.m_nodes.size());
			outModel.GetNodes().CopyFromArray(&meta.m_nodes[0], (uint32_t)meta.m_nodes.size());
		}
		
		if(!meta.m_nodeCores.empty())
		{
			outModel.AllocateNodeCores ((ObjectIndex)meta.m_nodeCores.size());
			outModel.GetNodeCores().CopyFromArray(&meta.m_nodeCores[0], (uint32_t)meta.m_nodeCores.size());
		}
		
		if(!meta.m_meshes.empty())
		{
			outModel.AllocateMeshes    ((ObjectIndex)meta.m_meshes.size());
			outModel.GetMeshes().CopyFromArray(&meta.m_meshes[0], (uint32_t)meta.m_meshes.size());
		}
		
		if(!meta.m_subMeshes.empty())
		{
			outModel.AllocateSubMeshes ((ObjectIndex)meta.m_subMeshes.size());
			outModel.GetSubMeshes().CopyFromArray(&meta.m_subMeshes[0], (uint32_t)meta.m_subMeshes.size());
		}
		
		if(!meta.m_geometries.empty())
		{
			outModel.AllocateGeometries((ObjectIndex)meta.m_geometries.size());
			outModel.GetGeometries().CopyFromArray(&meta.m_geometries[0], (uint32_t)meta.m_geometries.size());
		}
		
		if(!meta.m_materials.empty())
		{
			outModel.AllocateMaterials ((ObjectIndex)meta.m_materials.size());
			outModel.GetMaterials().CopyFromArray(&meta.m_materials[0], (uint32_t)meta.m_materials.size());
		}

		if(!meta.m_strings.empty())
		{
			outModel.AllocateStrings   ((ObjectIndex)meta.m_strings.size());
			outModel.GetStrings().CopyFromArray(&meta.m_strings[0], (uint32_t)meta.m_strings.size());
		}

		if(!meta.m_stringPool.empty())
		{
			outModel.AllocateStringPool   ((ObjectIndex)meta.m_stringPool.size());
			outModel.GetStringPool().CopyFromArray(&meta.m_stringPool[0], (uint32_t)meta.m_stringPool.size());
		}

		return 0;
	}
	
	void FbxParser::ParseNode(
		Node& outNode,
		ModelMetaBuffer& outMeta,
		FbxNode& node)
	{
		int childCount = node.GetChildCount();
		if(childCount <= 0) return;

		SI_ASSERT(outMeta.m_nodes.size() < 0xfffe);
		SI::ObjectIndex firstIndex = (SI::ObjectIndex)outMeta.m_nodes.size();

		outNode.m_children.m_count = childCount;
		outNode.m_children.m_first = firstIndex;
		
		outMeta.m_nodes.resize(firstIndex + childCount);
		outMeta.m_nodeCores.resize(firstIndex + childCount);
		for(int i=0; i<childCount; ++i)
		{
			Node& child = outMeta.m_nodes[firstIndex + i];
			NodeCore& childCore = outMeta.m_nodeCores[firstIndex + i];
			child.m_current = firstIndex + (SI::ObjectIndex)i;
			child.m_parent = outNode.m_current;

			FbxNode* fbxChildNode = node.GetChild(i);
			const char* name = fbxChildNode->GetName();
			if(name)
			{
				child.m_name = (ObjectIndex)outMeta.m_strings.size();

				outMeta.m_strings.push_back((LongObjectIndex)outMeta.m_stringPool.size());

				size_t len = strlen(name) + 1; // 終端もコピー
				outMeta.m_stringPool.reserve(outMeta.m_stringPool.size() + len);
				for(size_t j=0; j<len; ++j) outMeta.m_stringPool.push_back(name[j]);
			}
			else
			{
				child.m_name = kInvalidObjectIndex;
			}

			FbxMesh* mesh = fbxChildNode->GetMesh();
			if(mesh)
			{
				ParseMesh(child, outMeta, *mesh);
			}

			FbxAMatrix m = fbxChildNode->EvaluateLocalTransform();
			childCore.m_localMatrix = FxbMatrixToVfloat4x4(m);
			childCore.m_isMatrixChanged = true;

			ParseNode(child, outMeta, *fbxChildNode);
		}
	}
	
	void FbxParser::ParseMesh(
		Node& outNode,
		ModelMetaBuffer& outMeta,
		FbxMesh& fbxMesh)
	{
		m_fbxGeometryConverter->SplitMeshPerMaterial(&fbxMesh, true);

		FbxNode* meshNode = fbxMesh.GetNode();
		int nodeAttributeCount = meshNode->GetNodeAttributeCount();
		int subMeshCount = 0;
		Mesh mesh;
		mesh.m_submeshIndeces.m_first = (SI::ObjectIndex)outMeta.m_subMeshes.size();

		for(int i=0; i<nodeAttributeCount; ++i)
		{
			FbxNodeAttribute* nodeAttr = meshNode->GetNodeAttributeByIndex(i);
			if(!nodeAttr) continue;
			if(nodeAttr->GetAttributeType() != FbxNodeAttribute::eMesh) continue;
			//if(nodeAttr == &fbxMesh) continue; // 分割前のsubmesh

			FbxMesh* fbxSubMesh = (FbxMesh*)nodeAttr;
			
			SubMesh subMesh;
			bool ret = ParseSubMesh(subMesh, outMeta, *fbxSubMesh);
			if(!ret) continue;

			outMeta.m_subMeshes.push_back(subMesh);
			
			++subMeshCount;
		}

		if(0 < subMeshCount)
		{
			mesh.m_submeshIndeces.m_count = (uint16_t)subMeshCount;

			outNode.m_nodeComponentType = SI::NodeType::Mesh;
			outNode.m_nodeComponent     = (SI::ObjectIndex)outMeta.m_meshes.size();
			
			outMeta.m_meshes.push_back(mesh);
		}
	}
	
	bool FbxParser::ParseSubMesh(
		SubMesh& outSubMesh,
		ModelMetaBuffer& outMeta,
		FbxMesh& fbxSubMesh)
	{
		outSubMesh.m_geometryIndex = (uint16_t)outMeta.m_geometries.size();
		
		VertexLayout vertexLayout;

		///////////////////////////////////////////////////////////
		// index
		int indexCount = fbxSubMesh.GetPolygonVertexCount();
		if(indexCount<=0)
		{
			SI_WARNING("インデックスがない");
			return false;
		}
		SI_ASSERT(indexCount == fbxSubMesh.GetPolygonCount()*3, "前もって三角形化しているからpolygonCount*3のはず.");
		int* indeces = fbxSubMesh.GetPolygonVertices();
		
		
		///////////////////////////////////////////////////////////
		// position
		const FbxVector4* positions = fbxSubMesh.GetControlPoints();
		int positionCount = fbxSubMesh.GetControlPointsCount();
		if(positionCount<=0)
		{
			SI_WARNING("頂点位置がない");
			return false;
		}
		uint8_t posAttr = vertexLayout.m_attributeCount++;
		vertexLayout.m_attributes[posAttr].m_semantics = GfxSemantics(GfxSemanticsType::Position, 0);
		vertexLayout.m_attributes[posAttr].m_format    = GfxFormat::R32G32B32_Float;
		vertexLayout.m_stride += 3 * sizeof(float);

		///////////////////////////////////////////////////////////
		// normal
		FbxArray<FbxVector4> normals;		
		bool hasNormal = fbxSubMesh.GetPolygonVertexNormals(normals);
		hasNormal &= 0<normals.GetCount();
		if(hasNormal)
		{
			uint8_t normalAttr = vertexLayout.m_attributeCount++;
			if(ArraySize(vertexLayout.m_attributes) <= normalAttr){ SI_ASSERT(0); return false; }
			vertexLayout.m_attributes[normalAttr].m_semantics = GfxSemantics(GfxSemanticsType::Normal, 0);
			vertexLayout.m_attributes[normalAttr].m_format    = GfxFormat::R32G32B32_Float;
			vertexLayout.m_stride += 3 * sizeof(float);
		}
		
		
		///////////////////////////////////////////////////////////
		// uv
		FbxStringList uvsetNames;
		fbxSubMesh.GetUVSetNames(uvsetNames);

		std::array<FbxArray<FbxVector2>, 8> uvArrays;
		int uvCount = uvsetNames.GetCount();
		uvCount = SI::Min(uvCount, (int)uvArrays.size());
		for(int i=0; i<uvCount; ++i)
		{
			FbxString uvsetName = uvsetNames[i];
			fbxSubMesh.GetPolygonVertexUVs(uvsetName.Buffer(), uvArrays[i]);
			
			uint8_t uvAttr = vertexLayout.m_attributeCount++;
			if(ArraySize(vertexLayout.m_attributes) <= uvAttr){ SI_ASSERT(0); return false; }
			vertexLayout.m_attributes[uvAttr].m_semantics = GfxSemantics(GfxSemanticsType::UV, i);
			vertexLayout.m_attributes[uvAttr].m_format    = GfxFormat::R32G32_Float;
			vertexLayout.m_stride += 2 * sizeof(float);
		}
		
		///////////////////////////////////////////////////////////
		// tangent
		std::array<const FbxLayerElementArrayTemplate<FbxVector4>*, 8> tangentArrays;
		int tangentCount = fbxSubMesh.GetElementTangentCount();
		tangentCount = SI::Min(tangentCount, (int)tangentArrays.size());
		for(int i=0; i<tangentCount; ++i)
		{
			const FbxGeometryElementTangent* fbxTangent= fbxSubMesh.GetElementTangent(i);

			FbxLayerElementArrayTemplate<FbxVector4>& directArray = fbxTangent->GetDirectArray();
			tangentArrays[i] = &directArray;
			
			uint8_t tangentAttr = vertexLayout.m_attributeCount++;
			if(ArraySize(vertexLayout.m_attributes) <= tangentAttr){ SI_ASSERT(0); return false; }
			vertexLayout.m_attributes[tangentAttr].m_semantics = GfxSemantics(GfxSemanticsType::Tangent, i);
			vertexLayout.m_attributes[tangentAttr].m_format    = GfxFormat::R32G32B32A32_Float;
			vertexLayout.m_stride += 4 * sizeof(float);
		}

		
		///////////////////////////////////////////////////////////
		// color
		std::array<const FbxLayerElementArrayTemplate<FbxColor>*, 8> colorArrays;
		int colorCount = fbxSubMesh.GetElementVertexColorCount();
		colorCount = SI::Min(colorCount, (int)colorArrays.size());
		for(int i=0; i<colorCount; ++i)
		{
			const FbxGeometryElementVertexColor* fbxColor= fbxSubMesh.GetElementVertexColor(i);

			FbxLayerElementArrayTemplate<FbxColor>& directArray = fbxColor->GetDirectArray();
			colorArrays[i] = &directArray;
			
			uint8_t colorAttr = vertexLayout.m_attributeCount++;
			if(ArraySize(vertexLayout.m_attributes) <= colorAttr){ SI_ASSERT(0); return false; }
			vertexLayout.m_attributes[colorAttr].m_semantics = GfxSemantics(GfxSemanticsType::Color, i);
			vertexLayout.m_attributes[colorAttr].m_format    = GfxFormat::R32G32B32A32_Float;
			vertexLayout.m_stride += 4 * sizeof(float);
		}
		
		// positionとnormalの数が合っていないので頂点バッファとインデックスバッファを再構築する.
		std::vector<uint32_t>      newIndexArray;
		std::vector<HashKeyVertex> newVertexArray;
		{
			std::unordered_map<HashKeyVertex, uint32_t> hashVertexTable;
			
			newIndexArray.reserve(indexCount);
			newVertexArray.reserve(indexCount);

			for(int i=0; i<indexCount; ++i)
			{
				HashKeyVertex vertex;

				// position
				int positionId = indeces[i];
				FbxVector4 pos = positions[positionId];
				vertex.SetPosition(pos[0], pos[1], pos[2]);

				// normal
				if(hasNormal)
				{
					FbxVector4 normal = normals[i];
					vertex.SetNormal(normal[0], normal[1], normal[2]);
				}

				// uv
				for(int j=0; j<uvCount; ++j)
				{
					FbxArray<FbxVector2>& uvs = uvArrays[j];
					FbxVector2 uv = uvs[i];
					vertex.AppendUv(uv[0], uv[1]);
				}

				// tangent
				for(int j=0; j<tangentCount; ++j)
				{
					const FbxLayerElementArrayTemplate<FbxVector4>& tangents = *tangentArrays[j];
					FbxVector4 tangent = tangents[i];
					vertex.AppendTangent(tangent[0], tangent[1], tangent[2], tangent[3]);
				}

				// color
				for(int j=0; j<colorCount; ++j)
				{
					const FbxLayerElementArrayTemplate<FbxColor>& colors = *colorArrays[j];
					FbxColor color = colors[i];
					vertex.AppendColor(color[0], color[1], color[2], color[3]);
				}

				vertex.GenerateHash();

				auto itr = hashVertexTable.find(vertex);
				if(itr == hashVertexTable.end())
				{
					uint32_t newIndex = (uint32_t)newVertexArray.size();

					newVertexArray.push_back(vertex);
					newIndexArray.push_back(newIndex);
					hashVertexTable.insert( std::make_pair(vertex, newIndex) );
				}
				else
				{
					// 同じキーが見つかったので、インデックスを使いまわす.
					uint32_t newIndex = itr->second;
					newIndexArray.push_back(newIndex);
				}
			}
		}

		if(newVertexArray.empty())
		{
			SI_ASSERT("再構成された頂点がない");
			return false;
		}


		///////////////////////////////////////////////////////////
		// vertexBufferを構築
		{
			std::vector<float> vertexBuffer;

			uint32_t elementCount = newVertexArray[0].GetElementCount();
			vertexBuffer.reserve( elementCount * newVertexArray.size() );
			
			for(const auto& v : newVertexArray)
			{
				v.AppendToFloatArray(vertexBuffer);
			}
		
			outMeta.m_vertexBuffers.push_back(std::move(vertexBuffer));
		}
		
		
		///////////////////////////////////////////////////////////
		// indexBufferをコピー
		uint32_t maxIndex = 0;
		{
			maxIndex = *std::max_element(newIndexArray.begin(), newIndexArray.end());			
			outMeta.m_indexBuffers.push_back(std::move(newIndexArray));
		}
		
		std::vector<float>& resultVertexBuffer = outMeta.m_vertexBuffers.back();
		std::vector<uint32_t>& resultIndexBuffer = outMeta.m_indexBuffers.back();

		VerboseGeometry* geometry = VerboseGeometry::Create();
		geometry->m_rawVertexBuffer.Setup((uint8_t*)&resultVertexBuffer[0], uint32_t(resultVertexBuffer.size() * sizeof(float)));
		geometry->m_rawIndexBuffer.Setup((uint8_t*)&resultIndexBuffer[0], uint32_t(resultIndexBuffer.size() * sizeof(uint32_t)));
		geometry->m_vertexLayout = vertexLayout;
		geometry->m_is16bitIndex = false;
		outMeta.m_geometries.push_back(geometry); // 一個追加.


		//////////////////////////////////////////////////////////////////////////////////////
		
		int elementMaterialCount = fbxSubMesh.GetElementMaterialCount(); // SplitMeshPerMaterial呼び出してるので1のはず...
		if(0 < elementMaterialCount)
		{
			FbxGeometryElementMaterial* fbxMaterial = fbxSubMesh.GetElementMaterial();
			std::string materialName = fbxMaterial->GetName();

			ObjectIndex foundMaterialIndex = kInvalidObjectIndex;
			size_t createdMaterialCount = outMeta.m_materials.size();
			for(size_t i=0; i<createdMaterialCount; ++i)
			{
				Material* mate = outMeta.m_materials[i];
				//if(materialName == mate->m_name)
				if( mate->m_name != kInvalidObjectIndex &&
					outMeta.m_strings[mate->m_name] != kInvalidLongObjectIndex &&
					materialName == &outMeta.m_stringPool[outMeta.m_strings[mate->m_name]])
				{
					foundMaterialIndex = (ObjectIndex)i;
					break;
				}
			}

			if(foundMaterialIndex != kInvalidObjectIndex)
			{
				outSubMesh.m_materialIndex = foundMaterialIndex;
			}
			else
			{
				outSubMesh.m_materialIndex = (uint16_t)outMeta.m_materials.size();

				Material* material = Material::Create();
				material->m_name = (ObjectIndex)outMeta.m_strings.size();

				outMeta.m_strings.push_back((LongObjectIndex)outMeta.m_stringPool.size());

				size_t len = materialName.size()+1;
				outMeta.m_stringPool.reserve(outMeta.m_stringPool.size() + len);
				for(size_t j=0; j<len; ++j) outMeta.m_stringPool.push_back(materialName[j]);

				outMeta.m_materials.push_back(material);
			}
		}

		return true;
	}

} // namespace SI
