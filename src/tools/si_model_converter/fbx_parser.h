#pragma once

#include <vector>
#include <string>

#include "si_base/renderer/model.h"
#include "si_base/renderer/geometry.h"
#include "si_base/renderer/mesh.h"
#include "si_base/renderer/sub_mesh.h"
#include "si_base/renderer/material.h"
#include "si_base/memory/linear_allocator.h"

namespace fbxsdk
{
	class FbxManager;
	class FbxImporter;
	class FbxNode;
	class FbxMesh;
	class FbxGeometryConverter;
}

namespace SI
{
	class Model;
	class Node;
	class NodeCore;
	class Mesh;
	class SubMesh;
	class Geometry;
	class Material;

	struct ModelParsedData
	{
		ModelSerializeData                   m_serializeData;

		std::vector<NodeSerializeData>       m_nodes;
		std::vector<NodeCoreSerializeData>   m_nodeCores;
		std::vector<MeshSerializeData>       m_meshes;
		std::vector<SubMeshSerializeData>    m_subMeshes;
		std::vector<MaterialSerializeData>   m_materials;
		std::vector<GeometrySerializeData>   m_geometries;
		std::vector<std::vector<float>>      m_vertexBuffers;
		std::vector<std::vector<uint32_t>>   m_indexBuffers;
		std::vector<LongObjectIndex>         m_strings;
		std::vector<char>                    m_stringPool;
		LinearAllocator                      m_tempAllocator;
		
		void Clear();
		void Reserve(size_t count);
		void UpdateSerializeData();
	};

	class FbxParser
	{
	public:
		FbxParser();
		~FbxParser();

		void Initialize();
		void Terminate();
		
		int Parse(ModelParsedData& outData, const char* path);

	private:
		void ParseNode(
			NodeSerializeData& outNode,
			ModelParsedData& outparsedData,
			fbxsdk::FbxNode& node);
		
		void FbxParser::ParseMesh(
			NodeSerializeData& outNode,
			ModelParsedData& outparsedData,
			fbxsdk::FbxMesh& mesh,
			ObjectIndex nodeIndex);
		
		bool FbxParser::ParseSubMesh(
			SubMeshSerializeData& outSubMesh,
			ModelParsedData& outparsedData,
			fbxsdk::FbxMesh& fbxSubMesh);

	private:
		fbxsdk::FbxManager*           m_fbxManager;
		fbxsdk::FbxGeometryConverter* m_fbxGeometryConverter;
	};
} // namespace SI
