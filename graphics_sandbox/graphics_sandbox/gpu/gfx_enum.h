#pragma once

namespace SI
{
	// enumで表現したいが、重複して指定できるenumなので、Gfx Wrapperではclassにする.
	class GfxResourceState
	{
	public:
		GfxResourceState(int stateFlag = 0)
			: m_stateFlags(stateFlag)
		{
		}

		~GfxResourceState()
		{
		}

		inline int GetStateFlags() const{ return m_stateFlags; }
		inline void SetStateFlags(int stateFlags){ m_stateFlags = stateFlags; }
		inline void MergeStateFlags(const GfxResourceState& state){ m_stateFlags |= state.GetStateFlags();  }
		
		inline GfxResourceState& operator|=(const GfxResourceState& a)
		{
			MergeStateFlags(a);
			return *this;
		}

		// 各ResourceStateタイプ. 組み合わせれる.
		static const GfxResourceState kCommon; // present
		static const GfxResourceState kVertexAndConstantBuffer;
		static const GfxResourceState kIndexBuffer;
		static const GfxResourceState kRenderTarget;
		static const GfxResourceState kUnorderedAccess;
		static const GfxResourceState kDepthWrite;
		static const GfxResourceState kDepthRead;
		static const GfxResourceState kNonPixelShaderResource;
		static const GfxResourceState kPixelShaderResource;
		static const GfxResourceState kStreamOut;
		static const GfxResourceState kIndirectArgument;
		static const GfxResourceState kCopyDest;
		static const GfxResourceState kCopySource;
		static const GfxResourceState kResolveDest;
		static const GfxResourceState kResolveSource;

	private:
		int m_stateFlags;
	};
	
	inline GfxResourceState operator|(const GfxResourceState& a, const GfxResourceState& b)
	{
		return GfxResourceState(a.GetStateFlags() | b.GetStateFlags());
	}


} // namespace SI
