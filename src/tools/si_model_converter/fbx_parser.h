#pragma once

#include <vector>
#include <string>

#include "si_base/renderer/geometry.h"
#include "si_base/renderer/mesh.h"
#include "si_base/renderer/sub_mesh.h"
#include "si_base/renderer/material.h"

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

	struct ModelMetaBuffer
	{
		std::vector<Node>                   m_nodes;
		std::vector<NodeCore>               m_nodeCores;
		std::vector<Mesh>                   m_meshes;
		std::vector<SubMesh>                m_subMeshes;
		std::vector<Material*>              m_materials;
		std::vector<Geometry*>              m_geometries;
		std::vector<std::vector<float>>     m_vertexBuffers;
		std::vector<std::vector<uint32_t>>  m_indexBuffers;
		std::vector<LongObjectIndex>        m_strings;
		std::vector<char>                   m_stringPool;
		
		ModelMetaBuffer();
		~ModelMetaBuffer();
		
		void Clear();
		void Reserve(size_t count);
	};

	class FbxParser
	{
	public:
		FbxParser();
		~FbxParser();

		void Initialize();
		void Terminate();

		int Parse(Model& outModel, const char* fbxPath);

	private:

		void ParseNode(
			Node& outNode,
			ModelMetaBuffer& outMeta,
			fbxsdk::FbxNode& node);
		
		void FbxParser::ParseMesh(
			Node& outNode,
			ModelMetaBuffer& outMeta,
			fbxsdk::FbxMesh& mesh);
		
		bool FbxParser::ParseSubMesh(
			SubMesh& outSubMesh,
			ModelMetaBuffer& outMeta,
			fbxsdk::FbxMesh& fbxSubMesh);

	private:
		fbxsdk::FbxManager*           m_fbxManager;
		fbxsdk::FbxGeometryConverter* m_fbxGeometryConverter;
	};
} // namespace SI
