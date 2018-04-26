﻿
#include <stdio.h>
#include "base/base.h"
#include "platform/window_util.h"
#include "pipeline/pipeline.h"

#include "app/app.h"

namespace SI
{
	App::App()
		: m_pipeline(nullptr)
		, m_initialized(false)
	{
	}

	App::~App()
	{
		Terminate();
	}

	int App::Initialize(const AppConfig& config)
	{
		if(m_initialized) return 0;

		void* hWnd = WindowUtil::Initialize((int)config.m_width,	(int)config.m_height);
		if(hWnd==nullptr) return -1;

		PipelineDesc pipeDesc;
		pipeDesc.m_width  = config.m_width;
		pipeDesc.m_height = config.m_height;
		pipeDesc.m_hWnd   = hWnd;
		m_pipeline = new Pipeline();
		m_pipeline->Initialize(pipeDesc);

		if(WindowUtil::ShowWindow(hWnd, config.m_nCmdShow) != 0) return -1;

		m_initialized = true;
		return 0;
	}

	int App::Terminate()
	{
		if(!m_initialized) return 0;

		if(m_pipeline)
		{
			m_pipeline->Terminate();
			SafeDelete(m_pipeline);
		}

		if(WindowUtil::Terminate() != 0)
		{
			return -1;
		}

		m_initialized = false;
		return 0;
	}

	int App::Run()
	{
		while(true)
		{
			m_pipeline->Render();

			if(WindowUtil::MessageProc() != 0)
			{
				break;
			}
		}

		return 0;
	}

} // namespace SI