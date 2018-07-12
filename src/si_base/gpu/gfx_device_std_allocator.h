﻿#pragma once
#include <memory>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "si_base/gpu/gfx_device.h"
#include "si_base/memory/pool_allocator.h"

namespace SI
{
	template<class T>
	class GfxStdDeviceTempAllocator : public std::allocator<T> {
	public:
		GfxStdDeviceTempAllocator() { }
		GfxStdDeviceTempAllocator(const GfxStdDeviceTempAllocator& x) { }
	
		template<class U>
		GfxStdDeviceTempAllocator(const GfxStdDeviceTempAllocator<U>& x) { }
	
		std::allocator<T>::pointer allocate(
			std::allocator<T>::size_type n,
			std::allocator<T>::const_pointer hint=0)
		{ 
			return (std::allocator<T>::pointer) SI_DEVICE_TEMP_ALLOCATOR().Allocate(n * sizeof(T), alignof(T));
		}
	
		void deallocate(
			std::allocator<T>::pointer ptr,
			std::allocator<T>::size_type n)
		{
			SI_DEVICE_TEMP_ALLOCATOR().Deallocate(ptr);
		}
	
		template<class U>
		struct rebind
		{
			typedef GfxStdDeviceTempAllocator<U> other;
		};
	};
	
	template <typename T> using GfxTempVector = std::vector<T, SI::GfxStdDeviceTempAllocator<T>>;

} // namespace SI