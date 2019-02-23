
#include "raytracing_the_next_week.h"

#include <si_base/core/core.h>
#include <si_base/math/math.h>
#include <si_base/core/constant.h>
#include <random>
#include <chrono>
#include <si_base/concurency/atomic.h>
#include <si_base/concurency/mutex.h>
#include <si_base/gpu/gfx_dds.h>
#include <si_base/gpu/gfx_utility.h>
#include <si_app/file/file_utility.h>
#include <si_app/file/path_storage.h>
//#include <omp.h>

namespace SI
{
namespace RayTracingTheNextWeek
{

Vfloat3 Lerp(const Vfloat3& a, const Vfloat3& b, float t){ return (1-t)*a + t*b; }

class Ray
{
public: 
	Ray() {}
	~Ray(){}
	Ray(const Vfloat3& pos, const Vfloat3& dir, float ti = 0.0f)
		: position(pos)
		, direction(dir)
		, time(ti)
	{}
	
	const Vfloat3& Pos() const{ return position; }
	const Vfloat3& Dir() const{ return direction; }
	float Time() const{ return time; }
	Vfloat3 PointAt(float t) const{ return position + t * direction; }

private:
	Vfloat3 position;
	Vfloat3 direction;
	float time;
};

float FloatUnitRand()
{
	thread_local std::random_device rd;
	thread_local std::mt19937 mt(rd());
	thread_local std::uniform_real_distribution<float> dist(0.0,1.0);

	float f = dist(mt);
	SI_ASSERT(0.0f <= f && f <= 1.0f);
	return f;
}

Vfloat3 RandomInUnitSphere()
{
	Vfloat3 o;

	do
	{
		o = Vfloat3(FloatUnitRand(), FloatUnitRand(), FloatUnitRand());
	} while(1.0f <= o.LengthSqr());

	return o;
}

Vfloat3 RandomInUnitDisk()
{
	Vfloat3 o;

	do
	{
		o = 2.0 * Vfloat3(FloatUnitRand(), FloatUnitRand(), 0) - Vfloat3(1.0f, 1.0f, 0.0f);
	} while(1.0f <= Math::Dot(o,o));

	return o;
}

class Texture
{
public:
	virtual Vfloat3 Value(float u, float v, const Vfloat3& p) const = 0;
};

class ConstantTexture : public Texture
{
public:
	ConstantTexture()
	{
	}

	ConstantTexture(const Vfloat3& c)
		: m_color(c)
	{
	}

	virtual Vfloat3 Value(float u, float v, const Vfloat3& p) const override
	{
		return m_color;
	}

	Vfloat3 m_color;
};

class CheckerTexture : public Texture
{
public:
	CheckerTexture()
	{
	}

	CheckerTexture(Texture* t0, Texture* t1)
		: m_even(t0)
		, m_odd(t1)
	{
	}

	virtual Vfloat3 Value(float u, float v, const Vfloat3& p) const override
	{
		float sines = sin(10*p.X().AsFloat()) * sin(10*p.Y().AsFloat()) * sin(10*p.Z().AsFloat());
		if(sines < 0)
		{
			return m_odd->Value(u,v,p);
		}
		else
		{
			return m_even->Value(u,v,p);
		}
	}
		
	Texture* m_even;
	Texture* m_odd;
};

class Perlin
{
public:
	Perlin()
		: m_randVector(nullptr)
		, m_permX(nullptr)
		, m_permY(nullptr)
		, m_permZ(nullptr)
	{
	}

	~Perlin()
	{
		delete[] m_randVector;
		delete[] m_permX;
		delete[] m_permY;
		delete[] m_permZ;
	}

	float Noise(const Vfloat3& p) const
	{
		Vfloat3 floorP = Math::Floor(p);
		Vfloat3 uvw = p - floorP;
		int i = (int)floorP.X();
		int j = (int)floorP.Y();
		int k = (int)floorP.Z();

		Vfloat3 c[2][2][2];
		for(int x=0; x<2; ++x)
		{
			for(int y=0; y<2; ++y)
			{
				for(int z=0; z<2; ++z)
				{
					c[x][y][z] = m_randVector[m_permX[(i+x)&255] ^ m_permY[(j+y)&255] ^ m_permZ[(k+z)&255]];
				}
			}
		}

		return TrilinearInterp(c, uvw);
	}

	float Turb(const Vfloat3& p, int depth = 7) const
	{
		float accum = 0;
		Vfloat3 tmp = p;
		float weight = 1;
		for(int i=0; i<depth; ++i)
		{
			accum += weight * Noise(tmp);
			weight *= 0.5f;
			tmp *= 2;
		}

		return fabs(accum);
	}

	void Generate(int size)
	{
		m_randVector = GenerateRandVector(size);
		m_permX = GeneratePerm(size);
		m_permY = GeneratePerm(size);
		m_permZ = GeneratePerm(size);
	}
	
private:
	static float* GenerateRanFloat(int size)
	{
		float* p = new float[size];
		for(int i=0; i<size; ++i)
		{
			p[i] = FloatUnitRand();
		}
		return p;
	}

	static Vfloat3* GenerateRandVector(int size)
	{
		Vfloat3* p = new Vfloat3[size];
		for(int i=0; i<size; ++i)
		{
			Vfloat3 v(FloatUnitRand(), FloatUnitRand(), FloatUnitRand());
			p[i] = Math::Normalize(Vfloat3(-1) + 2.0f * v);
		}
		return p;
	}

	static void Permute(int* p, int n)
	{
		for(int i=n-1; 0<i; --i)
		{
			int target = int(FloatUnitRand()*(i+1));
			std::swap(p[i], p[target]);
		}
	}

	static int* GeneratePerm(int size)
	{
		int* p = new int[size];
		for(int i=0; i<size; ++i)
		{
			p[i] = i;
		}

		Permute(p, size);
		return p;
	}

	static float TrilinearInterp(Vfloat3 c[2][2][2], Vfloat3 uvw)
	{
		Vfloat3 uuvvww = uvw*uvw*(Vfloat3(3.0f) - Vfloat3(2.0f)*uvw);
		float accum = 0;
		for(int i=0; i<2; ++i)
		{
			for(int j=0; j<2; ++j)
			{
				for(int k=0; k<2; ++k)
				{
					Vfloat3 ijk((float)i, (float)j, (float)k);
					Vfloat3 weight(uvw - ijk);
					Vfloat3 tmp = (ijk*uuvvww + (Vfloat3::One() - ijk) * (Vfloat3::One()-uuvvww));
					accum += Math::HorizontalMul(tmp) * Math::Dot(c[i][j][k], weight);
				}
			}
		}

		return accum;
	}

private:
	Vfloat3* m_randVector;
	int*   m_permX;
	int*   m_permY;
	int*   m_permZ;
};

class NoiseTexture : public Texture
{
public:
	NoiseTexture()
	{
		m_perlin.Generate(256);
		m_scale = 1;
	}

	NoiseTexture(float scale)
		: m_scale(scale)
	{
		m_perlin.Generate(256);
	}

	virtual Vfloat3 Value(float u, float v, const Vfloat3& p) const
	{
		//return Vfloat3(1,1,1) * m_perlin.Noise(m_scale*p);
		//return Vfloat3(0.5f) * m_perlin.Noise(m_scale*p) + Vfloat3(0.5f);
		//return Vfloat3(0.5f) * m_perlin.Turb(m_scale*p);
		return Vfloat3(0.5f) * ( 1 + sin(m_scale*p.Z() + 10 * m_perlin.Turb(p)));
	}

	Perlin m_perlin;
	float m_scale;
};

class DdsTexture : public Texture
{
public:
	DdsTexture()
		: m_pixelByteSize(0)
	{
	}

	DdsTexture(const char* ddsPath)
	{
		Load(ddsPath);
	}

	void Load(const char* ddsFilePath)
	{
		int ret = SI::FileUtility::Load(m_texData, ddsFilePath);
		SI_ASSERT(ret==0);

		ret = SI::LoadDdsFromMemory(m_texMetaData, &m_texData[0], m_texData.size());
		SI_ASSERT(ret==0);

		if(SI::IsBlockCompression(m_texMetaData.m_format))
		{
			SI_ASSERT(0, "圧縮テクスチャは未対応.");

			m_texData.clear();
			m_texMetaData = GfxDdsMetaData();
			m_pixelByteSize = 0;
		}

		m_pixelByteSize = (uint32_t)SI::GetFormatBits(m_texMetaData.m_format) / 8;
	}

	virtual Vfloat3 Value(float u, float v, const Vfloat3& p) const override
	{
		if(!m_texMetaData.m_image) return Vfloat3(0.0f);

		uint32_t w = m_texMetaData.m_width;
		uint32_t h = m_texMetaData.m_height;
		uint32_t x = (uint32_t)(u * w);
		uint32_t y = (uint32_t)((1-v) * h);
		
		x = SI::Clamp(x, 0u, w-1u);
		y = SI::Clamp(y, 0u, h-1u);
		uint32_t bytePitch = m_texMetaData.m_pitchOrLinearSize;
		
		uint32_t index = m_pixelByteSize * x + y * bytePitch;
		SI_ASSERT(index+m_pixelByteSize <= m_texMetaData.m_imageSise);
		const void* pixelPtr = &((const uint8_t*)m_texMetaData.m_image)[index];

		Vfloat3 color(0.0f);
		switch (m_texMetaData.m_format)
		{
		case SI::GfxFormat::R8G8B8A8_Unorm:
		{
			const uint8_t* pixelPtr8 = (const uint8_t*)pixelPtr;
			color.Set((float)pixelPtr8[0], (float)pixelPtr8[1], (float)pixelPtr8[2]);
			color /= 255.0f;
			break;
		}
		default:
			SI_ASSERT(0, "未対応フォーマット.");
			break;
		}

		return color;
	}
	
	std::vector<uint8_t> m_texData;
	GfxDdsMetaData m_texMetaData;
	uint32_t m_pixelByteSize;
};

class Material;

struct HitRecord
{
	HitRecord()
		: t(0.0f)
		, position(0.0f)
		, normal(0.0f, 1.0f, 0.0f)
		, material(nullptr)
		, u(0.0f)
		, v(0.0f)
	{}

	float t;
	Vfloat3 position;
	Vfloat3 normal;
	const Material* material;
	float u;
	float v;
};

class Material
{
public:
	Material(){}
	virtual ~Material(){}

	virtual bool Scatter(const Ray& ray, const HitRecord& record, Vfloat3& outAttenuation, Ray& outRay) const = 0;
};

Vfloat3 Reflect(const Vfloat3& unitV, const Vfloat3& normal)
{
	return unitV - (2.0f * Math::Dot(unitV, normal)) * normal;
}

bool Refract(const Vfloat3& unitV, const Vfloat3& normal, float niOverNt, Vfloat3& outVec)
{
	float dotVN = Math::Dot(unitV, normal);
	float discriminant = 1.0f - niOverNt * niOverNt * (1 - dotVN * dotVN);
	if(0.0f < discriminant)
	{
		outVec = niOverNt * (unitV - normal * dotVN) - normal * sqrt(discriminant);
		return true;
	}
	else
	{
		return false;
	}
}

float Schlick(float cosine, float refractiveIndex)
{
	float f0 = (1-refractiveIndex)/(1+refractiveIndex);
	f0 = f0 * f0;

	return f0 + (1-f0) * pow(1-cosine, 5);
}

class Dielectric : public Material
{
public:
	explicit Dielectric(float ri=1.0f) : refractiveIndex(ri){}
	virtual ~Dielectric(){}

	virtual bool Scatter(const Ray& ray, const HitRecord& record, Vfloat3& outAttenuation, Ray& outRay) const override
	{
		Vfloat3 normalizedRayDir = ray.Dir().Normalize();
		Vfloat3 outwardNormal;
		float niOverNt = 1.0f;
		outAttenuation = Vfloat3(1.0f);
		float cosine = 0.0f;
		float dotRayNormal = Math::Dot(normalizedRayDir, record.normal);
		if(0.0f < dotRayNormal)
		{
			outwardNormal = -record.normal;
			niOverNt = refractiveIndex;
			cosine = refractiveIndex * dotRayNormal;
		}
		else
		{
			outwardNormal = record.normal;
			niOverNt = 1.0f / refractiveIndex;
			cosine = -dotRayNormal;
		}

		Vfloat3 refracted;
		float reflectProb = 0.0f;
		if(Refract(normalizedRayDir, outwardNormal, niOverNt, refracted))
		{
			reflectProb = Schlick(cosine, refractiveIndex);
		}
		else
		{
			reflectProb = 1.0f;
		}

		if(FloatUnitRand() <= reflectProb)
		{
			Vfloat3 reflectedDir = Reflect(normalizedRayDir, record.normal);
			outRay = Ray(record.position, reflectedDir);
		}
		else
		{
			outRay = Ray(record.position, refracted);
		}

		return true;
	}

private:
	float refractiveIndex;
};

class Metal : public Material
{
public:
	Metal(const Vfloat3& a = Vfloat3(1.0f), float r = 0.0f) : albedo(a), roughness(r) {}
	virtual ~Metal(){}

	virtual bool Scatter(const Ray& ray, const HitRecord& record, Vfloat3& outAttenuation, Ray& outRay) const override
	{
		Vfloat3 reflectedDir = Reflect(ray.Dir().Normalize(), record.normal);
		
		int tryCount = 0;
		Vfloat3 fuzz = roughness * RandomInUnitSphere();
		while(Math::Dot(reflectedDir + fuzz, record.normal) <= 0.0f)
		{
			fuzz = roughness * RandomInUnitSphere();
			if(32 < ++tryCount) return false;
		}
		reflectedDir += fuzz;

		outRay = Ray(record.position, reflectedDir);
		outAttenuation = albedo;
		return true;
	}
private:
	Vfloat3 albedo;
	float roughness;
};

class Lambert : public Material
{
public:
	explicit Lambert(const Texture* albedo) : m_albedo(albedo){}
	virtual ~Lambert(){}

	virtual bool Scatter(const Ray& ray, const HitRecord& record, Vfloat3& outAttenuation, Ray& outRay) const override
	{
		Vfloat3 target = record.position + record.normal + RandomInUnitSphere();
		outRay = Ray(record.position, target - record.position, ray.Time());
		outAttenuation = m_albedo->Value(record.u, record.v, record.position);
		return true;
	}

private:
	const Texture* m_albedo;
};

class Aabb
{
public:
	Aabb()
		: m_min(0.0f)
		, m_max(1.0f)
	{
	}

	Aabb(const Vfloat3& minV, const Vfloat3& maxV)
		: m_min(minV)
		, m_max(maxV)
	{
	}
	
	Vfloat3 Min() const{ return m_min; }
	Vfloat3 Max() const{ return m_max; }

	bool Hit(const Ray& ray, float minT, float maxT) const
	{
		// Nanとかもいい感じに計算される.
		Vfloat3 tx0 = (m_min - ray.Pos()) / ray.Dir();
		Vfloat3 tx1 = (m_max - ray.Pos()) / ray.Dir();
		
		Vfloat3 t0 = Math::Min(tx0, tx1);
		Vfloat3 t1 = Math::Max(tx0, tx1);
		
		float tMin = SI::Max(Math::HorizontalMax(t0).AsFloat(), minT);
		float tMax = SI::Min(Math::HorizontalMin(t1).AsFloat(), maxT);
		if(tMax <= tMin) return false;

		return true;
	}

private:
	Vfloat3 m_min;
	Vfloat3 m_max;
};

Aabb CombineAabb(const Aabb& a, const Aabb& b)
{
	return Aabb(Math::Min(a.Min(), b.Min()), Math::Max(a.Max(), b.Max()));
}

class Hitable
{
public:
	Hitable(){}
	virtual ~Hitable(){}

	virtual bool Hit(const Ray& ray, float minT, float maxT, HitRecord& outRecord) const = 0;
	virtual bool BoundingBox(float t0, float t1, Aabb& outAabb) const = 0;
};

class Sphere : public Hitable
{
public:
	Sphere(): center(0.0f), radius(1.0f), material(nullptr) {}
	Sphere(const Vfloat3& c, float r, const Material* m) : center(c), radius(r), material(m){}

	Vfloat3 Center() const{ return center; }
	float Radius() const{ return radius; }
	void SetCenter(Vfloat3& c){ center = c; }
	void SetRadius(float r){ radius = r; }
	void Set(Vfloat3& c, float r){ SetCenter(c); SetRadius(r); }

	bool Hit(const Ray& ray, float minT, float maxT, HitRecord& outRecord) const override
	{
		Vfloat3 cp = ray.Pos() - Center();
	
		float dotDirDir = Math::Dot(ray.Dir(), ray.Dir());
		float dotDirCp  = Math::Dot(ray.Dir(), cp);
		float dotCpCp   = Math::Dot(cp, cp);
	
		float a = dotDirDir;
		float b = 2.0f*dotDirCp;
		float c = dotCpCp - Radius()*Radius();

		float discriminant = b*b - 4*a*c;
		if(discriminant <= 0.0f)
		{
			return false;
		}
		float sqrtDiscriminant = sqrt(discriminant);

		float t = (-b - sqrtDiscriminant) / (2.0f * a);
		if(minT < t && t < maxT)
		{
			outRecord.t = t;
			outRecord.position = ray.PointAt(t);
			outRecord.normal = (outRecord.position - Center()) / Radius();
			outRecord.material = material;
			GetUv(outRecord.position, outRecord.u, outRecord.v);
			return true;
		}
		
		t = (-b + sqrtDiscriminant) / (2.0f * a);
		if(minT < t && t < maxT)
		{
			outRecord.t = t;
			outRecord.position = ray.PointAt(t);
			outRecord.normal = (outRecord.position - Center()) / Radius();
			outRecord.material = material;
			GetUv(outRecord.position, outRecord.u, outRecord.v);
			return true;
		}

		return false;
	}
	
	virtual bool BoundingBox(float t0, float t1, Aabb& outAabb) const override
	{
		outAabb = Aabb(center - Vfloat3(radius), center + Vfloat3(radius));
		return true;
	}

	void GetUv(const Vfloat3& p, float& u, float& v) const
	{
		Vfloat3 unitP = Math::Normalize(p-center);
		SI_ASSERT(fabs(unitP.X().AsFloat())<=1.0f);
		SI_ASSERT(fabs(unitP.Y().AsFloat())<=1.0f);
		SI_ASSERT(fabs(unitP.Z().AsFloat())<=1.0f);

		float phi = atan2(unitP.Z().AsFloat(), unitP.X().AsFloat());
		float theta = asin(unitP.Y().AsFloat());
		u = 1.0f - (phi + SI::kPi) / (2 * SI::kPi);
		v = (theta + SI::kPi*0.5f) / SI::kPi;
	}

private:
	Vfloat3 center;
	float radius;
	const Material* material;
};

class MovingSphere : public Hitable
{
public:
	MovingSphere()
		: center0(0.0f)
		, center1(0.0f)
		, time0(0.0f)
		, time1(0.0f)
		, radius(1.0f)
		, material(nullptr)
	{}

	MovingSphere(
		const Vfloat3& c0,
		const Vfloat3& c1,
		float t0,
		float t1,
		float r,
		const Material* m)
		: center0(c0)
		, center1(c1)
		, time0(t0)
		, time1(t1)
		, radius(r)
		, material(m)
	{}

	Vfloat3 Center(float time) const
	{
		return center0 + ((time - time0)/(time1 - time0)) * (center1 - center0);
	}

	float Radius() const{ return radius; }

	bool Hit(const Ray& ray, float minT, float maxT, HitRecord& outRecord) const override
	{
		Vfloat3 cp = ray.Pos() - Center(ray.Time());
	
		float dotDirDir = Math::Dot(ray.Dir(), ray.Dir());
		float dotDirCp  = Math::Dot(ray.Dir(), cp);
		float dotCpCp   = Math::Dot(cp, cp);
	
		float a = dotDirDir;
		float b = 2.0f*dotDirCp;
		float c = dotCpCp - Radius()*Radius();

		float discriminant = b*b - 4*a*c;
		if(discriminant <= 0.0f)
		{
			return false;
		}
		float sqrtDiscriminant = sqrt(discriminant);

		float t = (-b - sqrtDiscriminant) / (2.0f * a);
		if(minT < t && t < maxT)
		{
			outRecord.t = t;
			outRecord.position = ray.PointAt(t);
			outRecord.normal = (outRecord.position - Center(ray.Time())) / Radius();
			outRecord.material = material;
			GetUv(outRecord.position, ray.Time(), outRecord.u, outRecord.v);
			return true;
		}
		
		t = (-b + sqrtDiscriminant) / (2.0f * a);
		if(minT < t && t < maxT)
		{
			outRecord.t = t;
			outRecord.position = ray.PointAt(t);
			outRecord.normal = (outRecord.position - Center(ray.Time())) / Radius();
			outRecord.material = material;
			GetUv(outRecord.position, ray.Time(), outRecord.u, outRecord.v);
			return true;
		}

		return false;
	}

	void GetUv(const Vfloat3& p, float t, float& u, float& v) const
	{
		Vfloat3 unitP = Math::Normalize(p-Center(t));
		SI_ASSERT(fabs(unitP.X().AsFloat())<=1.0f);
		SI_ASSERT(fabs(unitP.Y().AsFloat())<=1.0f);
		SI_ASSERT(fabs(unitP.Z().AsFloat())<=1.0f);

		float phi = atan2(unitP.Z().AsFloat(), unitP.X().AsFloat());
		float theta = asin(unitP.Y().AsFloat());
		u = 1.0f - (phi + SI::kPi) / (2 * SI::kPi);
		v = (theta + SI::kPi*0.5f) / SI::kPi;
	}
	
	virtual bool BoundingBox(float t0, float t1, Aabb& outAabb) const override
	{
		Vfloat3 c0 = Center(t0);
		Vfloat3 c1 = Center(t1);
		Vfloat3 r(radius);
		Aabb aabb0(c0-r, c0+r);
		Aabb aabb1(c1-r, c1+r);
		outAabb = CombineAabb(aabb0, aabb1);
		return true;
	}

private:
	Vfloat3 center0;
	Vfloat3 center1;
	float time0;
	float time1;
	float radius;
	const Material* material;
};

class BvhNode : public Hitable
{
public:
	BvhNode()
		: m_left(nullptr)
		, m_right(nullptr)
		, m_owner(false)
	{}

	~BvhNode()
	{
		if(m_owner)
		{
			delete m_left;
			delete m_right;
		}
	}

	static int CompareAabb(int axis, const void* a, const void* b)
	{
		const Hitable* hitable0 = *(const Hitable**)a;
		const Hitable* hitable1 = *(const Hitable**)b;
		Aabb aabb0;
		Aabb aabb1;
		bool ret0 = hitable0->BoundingBox(0, 0, aabb0);
		bool ret1 = hitable1->BoundingBox(0, 0, aabb1);
		SI_ASSERT(ret0 && ret1);

		if(aabb0.Min()[axis] < aabb1.Min()[axis])
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}

	void BuildBvh(const Hitable** list, int listCount, float t0, float t1, int axisIndex=0)
	{
		SI_ASSERT(m_left==nullptr);
		SI_ASSERT(m_right==nullptr);

		int axis = axisIndex % 3;
		if(axis==0)
		{
			std::qsort(list, listCount, sizeof(Hitable*), [](const void* a, const void* b)->int{ return CompareAabb(0, a, b); });
		}
		else if(axis==1)
		{
			std::qsort(list, listCount, sizeof(Hitable*), [](const void* a, const void* b)->int{ return CompareAabb(1, a, b); });
		}
		else
		{
			std::qsort(list, listCount, sizeof(Hitable*), [](const void* a, const void* b)->int{ return CompareAabb(2, a, b); });
		}

		if(listCount==1)
		{
			m_left = m_right = list[0];
		}
		else if(listCount==2)
		{
			m_left = list[0];
			m_right = list[1];
		}
		else
		{
			m_left  = new BvhNode();
			m_right = new BvhNode();
			m_owner = true;
			
			int leftCount = listCount/2;
			((BvhNode*)m_left)->BuildBvh (list,             leftCount,             t0, t1, axis+1);
			((BvhNode*)m_right)->BuildBvh(list + leftCount, listCount - leftCount, t0, t1, axis+1);
		}
		
		Aabb leftAabb;
		Aabb rightAabb;
		bool ret0 = m_left->BoundingBox(t0, t1, leftAabb);
		bool ret1 = m_right->BoundingBox(t0, t1, rightAabb);
		SI_ASSERT(ret0 && ret1);

		m_aabb = CombineAabb(leftAabb, rightAabb);
	}
	
	bool Hit(const Ray& ray, float minT, float maxT, HitRecord& outRecord) const override
	{
		if(!m_aabb.Hit(ray, minT, maxT)) return false;
		
		HitRecord leftRecord;
		HitRecord rightRecord;
		
		bool hitLeft  = m_left? m_left->Hit(ray, minT, maxT, leftRecord) : false;
		bool hitRight = m_right? m_right->Hit(ray, minT, maxT, rightRecord) : false;

		if(hitLeft && hitRight)
		{
			outRecord = (leftRecord.t < rightRecord.t)? leftRecord : rightRecord;
			return true;
		}
		else if(hitLeft)
		{
			outRecord = leftRecord;
			return true;
		}
		else if(hitRight)
		{
			outRecord = rightRecord;
			return true;
		}

		return false;
	}

	bool BoundingBox(float t0, float t1, Aabb& outAabb) const
	{
		outAabb = m_aabb;
		return true;
	}

private:
	const Hitable* m_left;
	const Hitable* m_right;
	Aabb m_aabb;
	bool m_owner;
};

class HitableList : public Hitable
{
public:
	HitableList() : m_bvh(nullptr)
	{
	}

	HitableList(int c) : m_list(c), m_bvh(nullptr)
	{
		Clear();
	}

	virtual ~HitableList()
	{
		delete m_bvh;
		m_bvh = nullptr;
	}

	void Reserve(int c)
	{
		m_list.clear();
		m_list.resize(c);
		Clear();
		
		delete m_bvh;
		m_bvh = nullptr;
	}

	void Clear()
	{
		if(!m_list.empty())
		{
			std::fill(m_list.begin(), m_list.end(), nullptr);
		}
	}

	bool Set(int id, const Hitable* h)
	{
		if(m_list.size()<=id) return false;
		if(id<0) return false;

		m_list[id] = h;
		return true;
	}
	
	bool Build(float t0, float t1)
	{
		SI_ASSERT(m_bvh==nullptr);

		m_bvh = new BvhNode();
		m_bvh->BuildBvh(&m_list[0], (int)m_list.size(), t0, t1);

		return true;
	}
	
	bool Hit(const Ray& ray, float minT, float maxT, HitRecord& outRecord) const override
	{
		if(m_bvh)
		{
			return m_bvh->Hit(ray, minT, maxT, outRecord);
		}

		bool hitAnything = false;
		float closestT = maxT;
		HitRecord tmpRecord;

		for(int i=0; i<m_list.size(); ++i)
		{
			const Hitable* h = m_list[i];
			if(h==nullptr) continue;

			if(h->Hit(ray, minT, closestT, tmpRecord))
			{
				outRecord = tmpRecord;
				closestT = tmpRecord.t;
				hitAnything = true;
			}
		}
		return hitAnything;
	}

	bool BoundingBox(float t0, float t1, Aabb& outAabb) const
	{
		if(m_list.empty()) return false;

		if(!m_list[0]->BoundingBox(t0, t1, outAabb)) return false;

		Aabb tmpAabb;
		for(int i=1; i<m_list.size(); ++i)
		{
			if(!m_list[i]->BoundingBox(t0, t1, tmpAabb)) return false;

			outAabb = CombineAabb(outAabb, tmpAabb);
		}

		return true;
	}

protected:
	std::vector<const Hitable*> m_list;

	BvhNode *m_bvh;
};

class TwoPerlinSphere : public HitableList
{
public:
	TwoPerlinSphere()
	{
		m_noiseTexture = new NoiseTexture(4);

		char ddsFilePath[260];
		sprintf_s(ddsFilePath, "%stexture\\test_texture_rgba8.dds", SI_PATH_STORAGE().GetAssetDirPath());
		//sprintf_s(ddsFilePath, "%stexture\\grid.dds", SI_PATH_STORAGE().GetAssetDirPath());
		m_ddsTexture = new DdsTexture(ddsFilePath);

		m_lambert0 = new Lambert(m_noiseTexture);
		m_lambert1 = new Lambert(m_ddsTexture);

		Reserve(2);
		Set(0, new Sphere(Vfloat3(0, -1000, 0), 1000, m_lambert0));
		Set(1, new Sphere(Vfloat3(0, 2, 0), 2, m_lambert1));
	}

	virtual ~TwoPerlinSphere()
	{
		delete m_list[1];
		delete m_list[0];
		
		delete m_lambert0;
		delete m_lambert1;
		delete m_noiseTexture;
		delete m_ddsTexture;
	}

private:
	DdsTexture* m_ddsTexture;
	NoiseTexture* m_noiseTexture;
	Lambert* m_lambert0;
	Lambert* m_lambert1;
};

class ManySpheres : public HitableList
{
public:
	ManySpheres(float time0, float time1)
	{
		float timeDif = time1 - time0;
		textures.push_back( new ConstantTexture(Vfloat3(0.2f, 0.3f, 0.1f)) );
		textures.push_back( new ConstantTexture(Vfloat3(0.9f, 0.9f, 0.9f)) );
		textures.push_back( new CheckerTexture(textures[0], textures[1]) ); // checker
	
		textures.push_back( new ConstantTexture(Vfloat3(1.0f, 0.5f, 0.5f)) );
		textures.push_back( new ConstantTexture(Vfloat3(0.5f, 0.5f, 0.5f)) );
		textures.push_back( new ConstantTexture(Vfloat3(0.2f, 0.2f, 0.9f)) );

		static const Lambert lamberts[] =
		{
			Lambert(textures[2]),
			Lambert(textures[3]),
			Lambert(textures[4]),
			Lambert(textures[5]),
		};

		static const Metal metals[] =
		{
			Metal(Vfloat3(1.0f, 1.0f, 1.0f), 0.0f),
			Metal(Vfloat3(1.0f, 0.5f, 0.5f), 0.5f),
		};

		static const Dielectric dielectrics[] = 
		{
			Dielectric(1.5f),
			Dielectric(2.4f),
		};

		static const Sphere spheres[] = 
		{
			Sphere(Vfloat3(0.0f, -10000.f, 0.0f), 10000.0f, &lamberts[0]),
			Sphere(Vfloat3(-4.0f, 1.0f, -1.0f), 1.0f, &dielectrics[0]),
			Sphere(Vfloat3(0.0f, 1.0f, -1.0f), 1.0f, &lamberts[1]),
			Sphere(Vfloat3(4.0f, 1.0f, -1.0f), 1.0f, &metals[0]),
		};
		int sphereCount = (int)(sizeof(spheres)/sizeof(spheres[0]));
		
		int randCount = 6;
		int allRandCount = (2 * randCount + 1) * (2 * randCount + 1);
		randMaterials.reserve(allRandCount);
		randSpheres.reserve(allRandCount);

	#if 1
		for(int z=-randCount; z<=randCount; ++z)
		{
			if(z==-1) continue;

			for(int x=-randCount; x<=randCount; ++x)
			{
				Material* m = nullptr;
				Vfloat3 center = Vfloat3((float)x+0.9f*FloatUnitRand(), 0.2f, (float)z+0.9f*FloatUnitRand());
				if((center - Vfloat3(4,0.2f,0)).Length() < 0.9f) continue;
				if((center - Vfloat3(-4,0.2f,0)).Length() < 0.9f) continue;
				if((center - Vfloat3(0,0.2f,0)).Length() < 0.9f) continue;

				float randKey = FloatUnitRand();
				if(randKey < 0.8f)
				{
					textures.push_back( new ConstantTexture(RandomInUnitSphere()*RandomInUnitSphere()) );
					m = new Lambert(textures.back());
					randMaterials.push_back(m);
					randSpheres.push_back(new MovingSphere(center, center+Vfloat3(0, 0.5f*timeDif*FloatUnitRand(), 0), time0, time1, 0.2f, m));
				}
				else if(randKey < 0.9f)
				{
					m = new Metal(0.5f * (Vfloat3(1) + RandomInUnitSphere()), 0.5f * FloatUnitRand());
					randMaterials.push_back(m);
					randSpheres.push_back(new Sphere(center, 0.2f, m));
				}
				else
				{
					m = new Dielectric(1.5f);
					randMaterials.push_back(m);
					randSpheres.push_back(new Sphere(center, 0.2f, m));
				}
			}
		}
	#endif

		Reserve(sphereCount + (int)randSpheres.size());
		for(int i=0; i<sphereCount; ++i)
		{
			Set(i, &spheres[i]);
		}
		for(size_t i=0; i<randSpheres.size(); ++i)
		{
			Hitable* s = randSpheres[i];
			Set((int)i + sphereCount, s);
		}
	}

	virtual ~ManySpheres()
	{
		for(Hitable* s : randSpheres)
		{
			delete s;
		}
		randSpheres.clear();
		
		for(Material* m : randMaterials)
		{
			delete m;
		}
		randMaterials.clear();
		
		for(Texture* t : textures)
		{
			delete t;
		};
		textures.clear();
	}

private:
	std::vector<Texture*> textures;
	std::vector<Material*> randMaterials;
	std::vector<Hitable*> randSpheres;
};

class Camera
{
public:
	Camera(
		Vfloat3 camPos,
		Vfloat3 target,
		Vfloat3 vUp,
		float vFov,
		float aspect,
		float aparture,
		float focusDist,
		float t0,
		float t1)
	{
		time0 = t0;
		time1 = t1;

		lensRadius = 0.5f * aparture;
		float theta = vFov * kPi / 180.0f;
		float halfHeight = tan(0.5f * theta);
		float halfWidth = aspect * halfHeight;
		origin = camPos;

		w = (camPos - target).Normalize();
		u = (Math::Cross(vUp, w)).Normalize();
		v = -Math::Cross(w,u);

		lowerLeftCorner = origin - halfWidth * focusDist * u - halfHeight * focusDist * v - focusDist * w;
		horizontal = 2.0f * halfWidth * focusDist * u;
		vertical = 2.0f * halfHeight * focusDist * v;
	}

	Ray CalcRay(float s, float t)
	{
		Vfloat3 rd = lensRadius * RandomInUnitDisk();
		Vfloat3 offset = u * rd.X() + v * rd.Y();
		float time = time0 + FloatUnitRand() * (time1 - time0);
		return Ray(origin + offset, lowerLeftCorner + s*horizontal + t*vertical - (origin + offset), time);
	}

private:
	Vfloat3 origin;
	Vfloat3 lowerLeftCorner;
	Vfloat3 horizontal;
	Vfloat3 vertical;
	Vfloat3 u,v,w;
	float time0;
	float time1;
	float lensRadius;
};

bool CheckIfRayHitsSphere(const Ray& ray, const Sphere& sphere, float& outT)
{
	Vfloat3 cp = ray.Pos() - sphere.Center();
	
	float dotDirDir = Math::Dot(ray.Dir(), ray.Dir());
	float dotDirCp  = Math::Dot(ray.Dir(), cp);
	float dotCpCp   = Math::Dot(cp, cp);
	
	float a = dotDirDir;
	float b = 2.0f*dotDirCp;
	float c = dotCpCp - sphere.Radius()*sphere.Radius();

	float discriminant = b*b - 4*a*c;
	if(discriminant < 0.0f)
	{
		outT = -1.0f;
		return false;
	}

	outT = (-b - sqrt(discriminant)) / (2.0f * a);
	return true;
}

Vfloat3 RayToColor(const Ray& r)
{
	Vfloat3 unitDir = (r.Dir()).Normalize();

	float t = 0.5f*unitDir.Y() + 0.5f;
	return Lerp(Vfloat3(1.0f), Vfloat3(0.5f, 0.7f, 1.0f), t);
}

Vfloat3 RayToColor(const Ray& r, const Hitable& world, int bounce = 0)
{
	HitRecord record;
	if(world.Hit(r, 0.001f, FLT_MAX, record))
	{
		Ray scatteredRay;
		Vfloat3 attenuation;
		if( bounce < 64 && record.material->Scatter(r, record, attenuation, scatteredRay) )
		{
			return attenuation * RayToColor(scatteredRay, world, bounce+1);
		}
		else
		{
			return Vfloat3(0.0f);
		}
	}
	else
	{
		Vfloat3 unitDir = (r.Dir()).Normalize();

		float t = 0.5f*unitDir.Y() + 0.5f;
		return Lerp(Vfloat3(1.0f), Vfloat3(0.5f, 0.7f, 1.0f), t);
	}
}

std::vector<float> GenerateRaytracingTextureData(uint32_t width, uint32_t height)
{
	auto start = std::chrono::system_clock::now();
	SI_SCOPE_EXIT( SI_PRINT("GenerateRaytracingTextureData=%dms\n", (int)std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::system_clock::now() - start)).count()); );

	const uint32_t pixelSize = 4;
	const uint32_t rowPitch = width * pixelSize;
	const uint32_t textureSize = rowPitch * height;

	const uint32_t sampleUPerPixel = 16;
	const uint32_t sampleVPerPixel = 16;
	const uint32_t samplePerPixel = sampleUPerPixel * sampleVPerPixel;

	std::vector<float> data(textureSize);
	float* pData = &data[0];

#if 0
	Vfloat3 cameraPos(4,2,4);
	float vFov = 90;
	Vfloat3 cameraTarget(0.0f, 0.0f, -1.0f);
	float focusDist = (cameraTarget - cameraPos).Length();
#elif 1
	Vfloat3 cameraPos(4,2,4);
	float vFov = 60;
	Vfloat3 cameraTarget(0.0f, 2.0f, 0.0f);
	float focusDist = (cameraTarget - cameraPos).Length();
#else
	Vfloat3 cameraPos(13,2,3);
	float vFov = 20;
	Vfloat3 cameraTarget(0.0f, 0.0f, 0.0f);
	float focusDist = 10;
#endif
	float apature = 0.05f;

	float time0 = 0;
	float time1 = 0.00001f;
	float timeDif = time1 - time0;

	Camera camera(
		cameraPos,
		cameraTarget,
		Vfloat3(0.0f, 1.0f ,0.0f),  // vup
		vFov,
		(float)width/(float)height,
		apature, // aparture
		focusDist,
		time0,
		time1);

#if 0
	ManySpheres world(time0, time1);
#else
	TwoPerlinSphere world;
#endif
	world.Build(time0, time1);

	auto func = [&](uint32_t n)
	{
		uint32_t x = n % rowPitch;
		uint32_t y = n / rowPitch;
		Vfloat3 color(0.0f);
		
		for (uint32_t us = 0; us < sampleUPerPixel; ++us)
		{
			float u = (float)x / (float)rowPitch;
			u += ((float)us + 0.5f) / (float)sampleUPerPixel / (float)width;

			for (uint32_t vs = 0; vs < sampleVPerPixel; ++vs)
			{
				float v = (float)y / (float)height;
				v += ((float)vs + 0.5f) / (float)sampleVPerPixel / (float)height;
		
				Ray ray = camera.CalcRay(u, v);
				Vfloat3 sampleColor = RayToColor(ray, world);

				color += sampleColor / (float)samplePerPixel;
			}
		}

		color = Math::Sqrt(color); // gamma

		pData[n]     = color.X();   // R
		pData[n + 1] = color.Y();	// G
		pData[n + 2] = color.Z();	// B
		pData[n + 3] = 1.0f;	    // A
	};

#if 1
	std::atomic<int> pix = 0;
	auto funcEntry = [&]()
	{
		for (int pp = pix.fetch_add((int)pixelSize); pp < (int)textureSize; pp = pix.fetch_add((int)pixelSize))
		{
			func(pp);
		}
	};
	std::vector<std::thread> threads;
	// スレッド立てて
	for(int i=0; i<7; ++i)
	{
		threads.emplace_back(funcEntry);
	}

	// 一緒に仕事する.
	funcEntry();

	// 待つ
	for(auto& t : threads)
	{
		t.join();
	}
	threads.clear();
#else
	//#pragma omp parallel for
	for (int32_t pix = 0; pix < (int)textureSize; pix += pixelSize)
	{
		func(pix);
	}
#endif

	return std::move(data);
}

} // namespace RayTracingTheNextWeek
} // namespace SI
