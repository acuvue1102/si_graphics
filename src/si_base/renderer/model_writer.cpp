
#include "si_base/renderer/model_writer.h"

#include <picojson/picojson.h>

#include "si_base/renderer/model.h"
#include "si_base/renderer/mesh.h"
#include "si_base/renderer/sub_mesh.h"
#include "si_base/renderer/material.h"
#include "si_base/renderer/geometry.h"
#include "si_base/math/math.h"
#include "si_base/core/print.h"
#include "si_base/file/file.h"

namespace SI
{
	class ModelWriterImpl
	{
	public:
		static void PicojsonInsert(picojson::object& obj, const char* key, const picojson::value& a)
		{
			obj.insert(std::make_pair(key, a));
		}
		
		static picojson::value ToPicojsonValue(uint32_t index)
		{
			 return picojson::value(std::to_string(index));
		}
		static picojson::value ToPicojsonValue(ObjectIndexRange range)
		{
			picojson::array picoRange;
			picoRange.push_back(ToPicojsonValue(range.m_first));
			picoRange.push_back(ToPicojsonValue(range.m_count));
			return picojson::value(picoRange);
		}
		static picojson::value ToPicojsonValue(const Vfloat4x4& m)
		{
			picojson::array picoArray;

			for(int i=0; i<4; ++i)
			{
				Vfloat4 column = m[i];
				picoArray.push_back(picojson::value((float)column[0]));
				picoArray.push_back(picojson::value((float)column[1]));
				picoArray.push_back(picojson::value((float)column[2]));
				picoArray.push_back(picojson::value((float)column[3]));
			}

			return picojson::value(picoArray);
		}
		static picojson::value GetObjectIndexRangeLayout()
		{
			picojson::array picoRange;
			picoRange.push_back(picojson::value("first"));
			picoRange.push_back(picojson::value("count"));
			return picojson::value(picoRange);
		}
		
		static bool SerializeNode(picojson::value& picoNode, const Node& node)
		{
			picojson::array picoNodeArray;
			picoNodeArray.push_back(ToPicojsonValue(node.GetNameIndex()));
			picoNodeArray.push_back(ToPicojsonValue(node.GetChildren()));
			picoNodeArray.push_back(ToPicojsonValue(node.GetCurrentNodeIndex()));
			picoNodeArray.push_back(ToPicojsonValue(node.GetParentNodeIndex()));
			picoNodeArray.push_back(ToPicojsonValue((uint32_t)node.GetNodeComponentType()));
			picoNodeArray.push_back(ToPicojsonValue(node.GetNodeComponentIndex()));

			picoNode = picojson::value(picoNodeArray);

			return true;
		}
		
		static bool SerializeNodeCore(picojson::value& picoNodeCore, const NodeCore& nodeCore)
		{
			picojson::array picoNodeCoreArray;

			picoNodeCoreArray.push_back(ToPicojsonValue(nodeCore.GetLocalMatrix()));

			picoNodeCore = picojson::value(picoNodeCoreArray);

			return true;
		}

		static bool SerializeNodeLayout(picojson::value& picoNodeLayout)
		{
			picojson::array picoNodeLayoutArray;
			picoNodeLayoutArray.push_back(picojson::value("name"));
			picoNodeLayoutArray.push_back(picojson::value("children"));
			picoNodeLayoutArray.push_back(picojson::value("current"));
			picoNodeLayoutArray.push_back(picojson::value("parent"));
			picoNodeLayoutArray.push_back(picojson::value("componentType"));
			picoNodeLayoutArray.push_back(picojson::value("component"));

			picoNodeLayout = picojson::value(picoNodeLayoutArray);

			return true;
		}
		
		static bool SerializeRootNode(picojson::value& outRootNodes, const Model& model)
		{
			return SerializeNode(outRootNodes, model.GetRootNode());
		}

		static bool SerializeNodes(picojson::value& outNodes, const Model& model)
		{
			picojson::array picoNodes;

			ConstArray<Node> nodes = model.GetNodes();
			for(uint32_t i=0; i<nodes.GetItemCount(); ++i)
			{
				const Node& n = nodes[i];
				picojson::value picoNode;
				if(!SerializeNode(picoNode, n)) return false;
				
				picoNodes.push_back(picoNode);
			}

			outNodes = picojson::value(picoNodes);

			return true;
		}
		
		static bool SerializeNodeCores(picojson::value& outNodeCores, const Model& model)
		{
			picojson::array picoNodeCores;

			ConstArray<NodeCore> nodeCores = model.GetNodeCores();
			for(uint32_t i=0; i<nodeCores.GetItemCount(); ++i)
			{
				const NodeCore& n = nodeCores[i];
				picojson::value picoNodeCore;
				if(!SerializeNodeCore(picoNodeCore, n)) return false;
				
				picoNodeCores.push_back(picoNodeCore);
			}

			outNodeCores = picojson::value(picoNodeCores);

			return true;
		}
		
		static bool SerializeNodeCoreLayout(picojson::value& outNodeCoreLayout, const Model& model)
		{
			picojson::array picoNodeCoreLayoutArray;
			picoNodeCoreLayoutArray.push_back(picojson::value("localMatrix"));

			outNodeCoreLayout = picojson::value(picoNodeCoreLayoutArray);

			return true;
		}

		static bool SerializeStrings(picojson::value& outStrings, const Model& model)
		{
			picojson::array picoStrings;
			
			ConstArray<LongObjectIndex> strings = model.GetStrings();
			ConstArray<char> stringPool = model.GetStringPool();
			for(uint32_t i=0; i<strings.GetItemCount(); ++i)
			{
				LongObjectIndex offset = strings[i];
				if(offset==kInvalidLongObjectIndex)
				{
					picoStrings.push_back(picojson::value(""));
					continue;
				}

				const char* str = &stringPool[offset];				
				picoStrings.push_back(picojson::value(str));
			}

			outStrings = picojson::value(picoStrings);

			return true;
		}

		static bool SerializeModel(picojson::object& picoModel, const char* modelName, const Model& model)
		{
			picoModel.insert( std::make_pair("version",  "0.01") );
			picoModel.insert( std::make_pair("modelName", modelName) );

			{
				// modelDataに一旦入れる.
				picojson::object picoModelData;
				
				// node layout
				{
					picojson::value picoRootNodeLayout;
					if(!SerializeNodeLayout(picoRootNodeLayout)) return false;			
					picoModelData.insert( std::make_pair("nodeLayout", picoRootNodeLayout) );
				}

				// root node
				{
					picojson::value picoRootNode;
					if(!SerializeRootNode(picoRootNode, model)) return false;			
					picoModelData.insert( std::make_pair("rootNode", picoRootNode) );
				}

				// nodes
				{
					picojson::value picoNodes;
					if(!SerializeNodes(picoNodes, model)) return false;
					picoModelData.insert( std::make_pair("nodes", picoNodes) );
				}

				// nodeCores
				{
					picojson::value picoNodeCoreLayout;
					if(!SerializeNodeCoreLayout(picoNodeCoreLayout, model)) return false;
					picoModelData.insert( std::make_pair("nodeCoreLayout", picoNodeCoreLayout) );
				}

				// nodeCores
				{
					picojson::value picoNodeCores;
					if(!SerializeNodeCores(picoNodeCores, model)) return false;
					picoModelData.insert( std::make_pair("nodeCores", picoNodeCores) );
				}

				// strings
				{
					picojson::value picoStrings;
					if(!SerializeStrings(picoStrings, model)) return false;
					picoModelData.insert( std::make_pair("strings",  picoStrings) );
				}
				
				picoModel.insert( std::make_pair("modelData", picoModelData) );
			}

			return true;
		}

		static std::string MakeJsonReadable(const std::string& json)
		{
			std::string o;

			int tab = 0;
			int arrayNest = 0;
			for(char c : json)
			{
				bool newLine = false;
				if(c=='{')
				{
					++tab;
					newLine = true;
				}
				else if(c=='}')
				{
					--tab;
					//newLine = true;
					o.push_back('\n');
					for(int i=0; i<tab; ++i)
					{
						o.push_back('\t');
					}
				}
				else if(c=='[')
				{
					//if(arrayNest==1)
					//{
					//	o.push_back('\n');
					//	for(int i=0; i<tab; ++i)
					//	{
					//		o.push_back('\t');
					//	}
					//}

					++tab;
					++arrayNest;
				}
				else if(c==']')
				{
					--tab;
					--arrayNest;
				}
				else if(c==',')
				{
					if(arrayNest==0)
					{
						newLine = true;
					}
				}

				o.push_back(c);
				
				if(newLine)
				{
					o.push_back('\n');
					for(int i=0; i<tab; ++i)
					{
						o.push_back('\t');
					}
				}
			}
			
			o.push_back('\n');
			return o;
		}
		
		bool Write(const char* path, const Model& model)
		{
			std::string modelName = path;
			size_t pos;
			if((pos = modelName.rfind("\\")) != std::string::npos) modelName = modelName.substr(pos+1);
			if((pos = modelName.rfind(".")) != std::string::npos) modelName = modelName.substr(0, pos);

			// jsonにシリアライズ.
			std::string json;
			{
				picojson::object picoModel;
				if(!SerializeModel(picoModel, modelName.c_str(), model)) return false;
				json = picojson::value(picoModel).serialize();

				// 見にくいので整形する.
				json = MakeJsonReadable(json);
			}

			// ファイルに出力.
			{
				File f;
				int ret = f.Open(path, SI::FileAccessType::Write);
				if(ret!=0)
				{
					SI_WARNING("file(%s) can't be loaded.", path);
					return false;
				}


				// utf-8 with BOM
				uint8_t bom[3] = {0xEF, 0xBB, 0xBF};
				f.Write(bom, sizeof(bom));

				f.Write(json.c_str(), json.size());

				f.Close();
			}

			return true;
		}
	};

	/////////////////////////////////////////////////////////////////////////////

	ModelWriter::ModelWriter()
		: m_impl(SI_NEW(ModelWriterImpl))
	{
	}
	
	ModelWriter::~ModelWriter()
	{
		SI_DELETE(m_impl);
	}

	bool ModelWriter::Write(const char* path, const Model& model)
	{
		return m_impl->Write(path, model);
	}

} // namespace SI
