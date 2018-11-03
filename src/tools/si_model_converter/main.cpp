#include <stdio.h>
#include <string>
#include <algorithm>

#include "si_base/misc/argument_parser.h"
#include "si_base/file/file.h"
#include "si_base/core/print.h"
#include "si_base/core/assert.h"
#include "si_base/renderer/model.h"
#include "si_base/renderer/model_writer.h"
#include "si_base/renderer/model_reader.h"
#include "fbx_parser.h"

#pragma comment(lib, "libfbxsdk-md.lib")


namespace
{
	static const char* kHelpMessage =
		"//////////////////////////////////////////\n"\
		"// si_model_converter                     \n"\
		"//////////////////////////////////////////\n"\
		"-h               : help.                  \n"\
		"-i <filepth>     : input fbx file.        \n"\
		"-o <filepth>     : output json file.      \n"\
		"//////////////////////////////////////////\n";

	
	int PrintHelp()
	{
		printf(kHelpMessage);
		return 0;
	}
}

int main(int argc, char* argv[])
{
	SI::ArgumentParser argParser;
	argParser.Initialize(argc, argv);
	
	if(argc<=1 || argParser.Exists("-h"))  return PrintHelp();
	
	std::string input;
	{
		const char* inputRaw = argParser.GetAsString("-i");
		if(inputRaw == nullptr)
		{
			SI_WARNING(0, "No input file.");
			return -1;
		}

		input = inputRaw;
		std::transform(input.begin(), input.end(), input.begin(), ::tolower);
		std::transform(input.begin(), input.end(), input.begin(), [](char c)->char{return (c=='/')? '\\' : c; });
	}
	
	std::string output;
	{
		const char* outputRaw = argParser.GetAsString("-o");
		if(outputRaw == nullptr)
		{
			std::string::size_type dotPos = 0;
			if((dotPos = input.find_last_of(".")) == std::string::npos)
			{
				SI_WARNING(0, "No input file extension.", input.c_str());
				return -1;
			}

			output = input.substr(0, dotPos);
			output += ".json";
		}
		else
		{
			output = outputRaw;
			std::transform(output.begin(), output.end(), output.begin(), ::tolower);
			std::transform(output.begin(), output.end(), output.begin(), [](char c)->char{return (c=='/')? '\\' : c; });
		}
	}
	
	if(!SI::File::Exists(input.c_str()))
	{
		SI_WARNING(0, "File %s does not exist.", input.c_str());
		return -1;
	}

	SI::FbxParser parser;
	parser.Initialize();

	SI::ModelParsedData parsedData;
	parser.Parse(parsedData, input.c_str());

	SI::ModelWriter writer;
	writer.Write(output.c_str(), parsedData.m_serializeData);
	parser.Terminate();

#if 0
	// 読み込みテスト.
	SI::ModelReader reader;
	SI::Model model;
	reader.Read(model, output.c_str());
#endif

	return 0;
}
