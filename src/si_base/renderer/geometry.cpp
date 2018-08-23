
#include "si_base/renderer/geometry.h"

namespace SI
{
	Geometry* Geometry::Create()
	{
		return new Geometry();
	}

	void Geometry::Release(Geometry*& geometry)
	{
		delete geometry;
		geometry = nullptr;
	}
	
	VerboseGeometry* VerboseGeometry::Create()
	{
		return new VerboseGeometry();
	}

} // namespace SI
