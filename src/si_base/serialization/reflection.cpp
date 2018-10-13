
#include "si_base/serialization/reflection.h"

namespace SI
{
	const ReflectionMember* ReflectionType::FindMember(const char* memberName, SI::Hash64 memberNameHash) const
	{
		if(memberNameHash==0){ memberNameHash = SI::GetHash64(memberName); }
		SI::Hash64 h = SI::GetHash64(memberName);
		SI_ASSERT(memberNameHash == h);// SI::GetHash64(memberName));

		uint32_t memberCount = GetMemberCount();
		for(uint32_t i=0; i<memberCount; ++i)
		{
			const ReflectionMember* member = GetMember(i);
			if(member->GetNameHash() != memberNameHash) continue;
			SI_ASSERT(strcmp(memberName, member->GetName())==0);

			return member;
		}

		return nullptr;
	}
}
