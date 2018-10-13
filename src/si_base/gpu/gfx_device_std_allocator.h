#pragma once
#include <memory>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "si_base/gpu/gfx_device.h"
#include "si_base/memory/pool_allocator.h"

namespace SI
{
#if 1
	template<class T>
	class GfxStdDeviceTempAllocator {
	public:
		using value_type = T;
		
		GfxStdDeviceTempAllocator() { }
		GfxStdDeviceTempAllocator(const GfxStdDeviceTempAllocator& x) { }
	
		template<class U>
		GfxStdDeviceTempAllocator(const GfxStdDeviceTempAllocator<U>& x) { }
	
		// メモリ確保
		T* allocate(std::size_t n)
		{
			return reinterpret_cast<T*>( SI_DEVICE_TEMP_ALLOCATOR().Allocate(n * sizeof(T), alignof(T) ) );
		}

		// メモリ解放
		void deallocate(T* p, std::size_t n)
		{
			SI_DEVICE_TEMP_ALLOCATOR().Deallocate(p);
		}
	};
	


	///////////////////////////////////////////////////////////////////////////////////////////////
	
	template<class T>
	class GfxStdDeviceObjAllocator {
	public:
		using value_type = T;

		GfxStdDeviceObjAllocator() { }
		GfxStdDeviceObjAllocator(const GfxStdDeviceObjAllocator& x) { }
	
		template<class U>
		GfxStdDeviceObjAllocator(const GfxStdDeviceObjAllocator<U>& x) { }
		
		// メモリ確保
		T* allocate(std::size_t n)
		{
			return reinterpret_cast<T*>( SI_DEVICE_OBJ_ALLOCATOR().Allocate(n * sizeof(T), alignof(T) ) );
		}

		// メモリ解放
		void deallocate(T* p, std::size_t n)
		{
			SI_DEVICE_OBJ_ALLOCATOR().Deallocate(p);
		}
	};
#else
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
	


	///////////////////////////////////////////////////////////////////////////////////////////////
	
	template<class T>
	class GfxStdDeviceObjAllocator : public std::allocator<T> {
	public:
		GfxStdDeviceObjAllocator() { }
		GfxStdDeviceObjAllocator(const GfxStdDeviceObjAllocator& x) { }
	
		template<class U>
		GfxStdDeviceObjAllocator(const GfxStdDeviceObjAllocator<U>& x) { }
	
		std::allocator<T>::pointer allocate(
			std::allocator<T>::size_type n,
			std::allocator<T>::const_pointer hint=0)
		{ 
			return (std::allocator<T>::pointer) SI_DEVICE_OBJ_ALLOCATOR().Allocate(n * sizeof(T), alignof(T));
		}
	
		void deallocate(
			std::allocator<T>::pointer ptr,
			std::allocator<T>::size_type n)
		{
			SI_DEVICE_OBJ_ALLOCATOR().Deallocate(ptr);
		}
	
		template<class U>
		struct rebind
		{
			typedef GfxStdDeviceObjAllocator<U> other;
		};
	};
#endif
	
	template <typename T> using GfxTempVector = std::vector<T, SI::GfxStdDeviceTempAllocator<T>>;
	template <typename T> using GfxObjVector = std::vector<T, SI::GfxStdDeviceObjAllocator<T>>;

} // namespace SI
