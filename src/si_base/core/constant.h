#pragma once

#include <cstdint>
#include <float.h>

namespace SI
{
	static const int8_t    kInt8Max   =  INT8_MAX;
	static const int8_t    kInt8Min   =  INT8_MIN;
	static const uint8_t   kUint8Max  =  UINT8_MAX;
	static const int16_t   kInt16Max  =  INT16_MAX;
	static const int16_t   kInt16Min  =  INT16_MIN;
	static const uint16_t  kUint16Max =  UINT16_MAX;
	
	static const int32_t   kInt32Max  =  INT32_MAX;
	static const int32_t   kInt32Min  =  INT32_MIN;
	static const uint32_t  kUint32Max =  UINT32_MAX;

	static const int64_t   kInt64Max  =  INT64_MAX;
	static const int64_t   kInt64Min  =  INT64_MIN;
	static const uint64_t  kUint64Max =  UINT64_MAX;
	static const float     kFltMax    =  FLT_MAX;
	static const float     kFltMin    =  FLT_MIN;	
	static const double    kDblMax    =  DBL_MAX;
	static const double    kDblMin    =  DBL_MIN;

	static const float kPi     = 3.14159265358979f;
} // namespace SI
