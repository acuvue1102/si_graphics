
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
#include <si_base/file/file_utility.h>
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
		o = 2.0 * Vfloat3(FloatUnitRand(), FloatUnitRand(), FloatUnitRand()) - Vfloat3::One();
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
		SI_ASSERT(index+m_pixelByteSize <= m_texMetaData.m_imageSize);
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
	virtual Vfloat3 Emitted(float u, float v, const Vfloat3& p) const{ return Vfloat3(0.0f); }
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

class DiffuseLight : public Material
{
public:
	DiffuseLight(const Texture* texture)
		: m_emit(texture)
	{
	}
	
	virtual bool Scatter(const Ray& ray, const HitRecord& record, Vfloat3& outAttenuation, Ray& outRay) const override
	{
		return false;
	}
	
	virtual Vfloat3 Emitted(float u, float v, const Vfloat3& p) const override
	{
		return m_emit->Value(u,v,p);
	}

private:
	const Texture* m_emit;
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

class Isotropic :public Material
{
public:
	Isotropic(const Texture* albedo)
		: m_albedo(albedo)
	{
	}
	
	virtual bool Scatter(const Ray& ray, const HitRecord& record, Vfloat3& outAttenuation, Ray& outRay) const override
	{
		outRay = Ray(record.position, RandomInUnitSphere(), ray.Time());
		outAttenuation = m_albedo->Value(record.u, record.v, record.position);
		return true;
	}

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

class ConstantMedium : public Hitable
{
public:
	ConstantMedium(Hitable* hitable, float density, const Isotropic* phaseFunction)
		: m_hitable(hitable)
		, m_density(density)
		, m_phaseFunction(phaseFunction)
	{}

	virtual bool Hit(const Ray& ray, float minT, float maxT, HitRecord& outRecord) const override
	{
		HitRecord rec0, rec1;
		if(!m_hitable->Hit(ray, -FLT_MAX, FLT_MAX, rec0)){ return false; }
		if(!m_hitable->Hit(ray, rec0.t+0.0001f, FLT_MAX, rec1)){ return false; }
		
		if(rec0.t < minT) rec0.t = minT;
		if(maxT < rec1.t) rec1.t = maxT;

		if(rec1.t <= rec0.t) return false;

		if(rec0.t < 0) rec0.t = 0;

		float distance = (rec1.t - rec0.t) * ray.Dir().Length().AsFloat();
		float hitDistance = -(1/m_density) * log(FloatUnitRand());

		if(distance <= hitDistance) return false;

		outRecord.t        = rec0.t + hitDistance / ray.Dir().Length().AsFloat();
		outRecord.position = ray.PointAt(outRecord.t);
		outRecord.normal   = Vfloat3(1,0,0); // arbitary
		outRecord.material = m_phaseFunction;

		return true;
	}

	virtual bool BoundingBox(float t0, float t1, Aabb& outAabb) const override
	{
		return m_hitable->BoundingBox(t0, t1, outAabb);
	}

	Hitable* m_hitable;
	float m_density;
	const Material* m_phaseFunction;
};


class Translate : public Hitable
{
public:
	Translate(
		Hitable* hitable,
		const Vfloat3& offset)
		: m_hitable(hitable)
		, m_offset(offset)
	{}

	virtual bool Hit(const Ray& ray, float minT, float maxT, HitRecord& outRecord) const override
	{
		Ray movedRay(ray.Pos() - m_offset, ray.Dir(), ray.Time());
		if(m_hitable->Hit(movedRay, minT, maxT, outRecord))
		{
			outRecord.position += m_offset;
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual bool BoundingBox(float t0, float t1, Aabb& outAabb) const override
	{
		if(m_hitable->BoundingBox(t0, t1, outAabb))
		{
			outAabb = Aabb(outAabb.Min() + m_offset, outAabb.Max() + m_offset);
			return true;
		}
		else
		{
			return false;
		}
	}

private:
	Hitable* m_hitable;
	Vfloat3 m_offset;
};

class RotateY : public Hitable
{
public:
	RotateY(Hitable* hitable, float angle)
		: m_hitable(hitable)
	{
		m_sinTheta = sin(angle);
		m_cosTheta = cos(angle);
		m_hasAabb = m_hitable->BoundingBox(0, 1, m_aabb); 
		
		Vfloat3 points[8] =
		{
			m_aabb.Min(),
			Vfloat3(m_aabb.Max().X(), m_aabb.Min().Y(), m_aabb.Min().Z()),
			Vfloat3(m_aabb.Min().X(), m_aabb.Max().Y(), m_aabb.Min().Z()),
			Vfloat3(m_aabb.Max().X(), m_aabb.Max().Y(), m_aabb.Min().Z()),
			Vfloat3(m_aabb.Min().X(), m_aabb.Min().Y(), m_aabb.Max().Z()),
			Vfloat3(m_aabb.Max().X(), m_aabb.Min().Y(), m_aabb.Max().Z()),
			Vfloat3(m_aabb.Min().X(), m_aabb.Max().Y(), m_aabb.Max().Z()),
			m_aabb.Max(),
		};
		
		Vfloat3 minPos(FLT_MAX);
		Vfloat3 maxPos(-FLT_MAX);
		for(const Vfloat3& p : points)
		{
			Vfloat3 rotatedP = CalcRotateY(p, -m_sinTheta, m_cosTheta); // 逆回転
			minPos = Math::Min(minPos, rotatedP);
			maxPos = Math::Max(maxPos, rotatedP);
		}

		m_aabb = Aabb(minPos, maxPos);
	}

	static Vfloat3 CalcRotateY(const Vfloat3& v, float sinTheta, float cosTheta)
	{
		return Vfloat3(
			cosTheta*v.X() - sinTheta*v.Z(),
			v.Y(),
			sinTheta*v.X() + cosTheta*v.Z());
	}

	virtual bool Hit(const Ray& ray, float minT, float maxT, HitRecord& outRecord) const override
	{
		Vfloat3 pos = ray.Pos();
		Vfloat3 dir = ray.Dir();		
		pos = CalcRotateY(pos, m_sinTheta, m_cosTheta);
		dir = CalcRotateY(dir, m_sinTheta, m_cosTheta);

		Ray rotatedRay(pos, dir, ray.Time());
		if(m_hitable->Hit(rotatedRay, minT, maxT, outRecord))
		{
			outRecord.position = CalcRotateY(outRecord.position, -m_sinTheta, m_cosTheta); // 逆回転
			outRecord.normal   = CalcRotateY(outRecord.normal,   -m_sinTheta, m_cosTheta); // 逆回転
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual bool BoundingBox(float t0, float t1, Aabb& outAabb) const override
	{
		if(m_hasAabb)
		{
			outAabb = m_aabb;
			return true;
		}
		else
		{
			return false;
		}
	}

private:
	Hitable* m_hitable;
	float m_sinTheta;
	float m_cosTheta;
	bool m_hasAabb;
	Aabb m_aabb;
};

class XYRect : public Hitable
{
public:
	XYRect(float x0, float x1, float y0, float y1, float z, const Material* m)
		: m_x0(x0)
		, m_x1(x1)
		, m_y0(y0)
		, m_y1(y1)
		, m_z(z)
		, m_material(m)
	{}

	bool Hit(const Ray& ray, float minT, float maxT, HitRecord& outRecord) const override
	{
		float t = (m_z - ray.Pos().Z().AsFloat()) / ray.Dir().Z().AsFloat();
		if(t<minT || maxT<t) return false;

		Vfloat3 xyz = ray.PointAt(t);
		float x = xyz.X().AsFloat();
		float y = xyz.Y().AsFloat();
		if(x < m_x0 || m_x1 < x) return false;
		if(y < m_y0 || m_y1 < y) return false;
		
		outRecord.u = (x - m_x0) / (m_x1 - m_x0);
		outRecord.v = (y - m_y0) / (m_y1 - m_y0);
		outRecord.t = t;
		outRecord.material = m_material;
		outRecord.position = xyz;
		outRecord.normal = Vfloat3(0,0,1);

		return true;
	}
	
	virtual bool BoundingBox(float t0, float t1, Aabb& outAabb) const override
	{
		outAabb = Aabb(Vfloat3(m_x0, m_y0, m_z-0.001f), Vfloat3(m_x1, m_y1, m_z+0.001f));
		return true;
	}

private:
	float m_x0, m_x1;
	float m_y0, m_y1;
	float m_z;
	const Material* m_material;
};

class XZRect : public Hitable
{
public:
	XZRect(float x0, float x1, float z0, float z1, float y, const Material* m)
		: m_x0(x0)
		, m_x1(x1)
		, m_z0(z0)
		, m_z1(z1)
		, m_y(y)
		, m_material(m)
	{}

	bool Hit(const Ray& ray, float minT, float maxT, HitRecord& outRecord) const override
	{
		float t = (m_y - ray.Pos().Y().AsFloat()) / ray.Dir().Y().AsFloat();
		if(t<minT || maxT<t) return false;

		Vfloat3 xyz = ray.PointAt(t);
		float x = xyz.X().AsFloat();
		float z = xyz.Z().AsFloat();
		if(x < m_x0 || m_x1 < x) return false;
		if(z < m_z0 || m_z1 < z) return false;
		
		outRecord.u = (x - m_x0) / (m_x1 - m_x0);
		outRecord.v = (z - m_z0) / (m_z1 - m_z0);
		outRecord.t = t;
		outRecord.material = m_material;
		outRecord.position = xyz;
		outRecord.normal = Vfloat3(0,1,0);

		return true;
	}
	
	virtual bool BoundingBox(float t0, float t1, Aabb& outAabb) const override
	{
		outAabb = Aabb(Vfloat3(m_x0, m_y-0.001f, m_z0), Vfloat3(m_x1, m_y+0.001f, m_z1));
		return true;
	}

private:
	float m_x0, m_x1;
	float m_z0, m_z1;
	float m_y;
	const Material* m_material;
};

class YZRect : public Hitable
{
public:
	YZRect(float y0, float y1, float z0, float z1, float x, const Material* m)
		: m_y0(y0)
		, m_y1(y1)
		, m_z0(z0)
		, m_z1(z1)
		, m_x(x)
		, m_material(m)
	{}

	bool Hit(const Ray& ray, float minT, float maxT, HitRecord& outRecord) const override
	{
		float t = (m_x - ray.Pos().X().AsFloat()) / ray.Dir().X().AsFloat();
		if(t<minT || maxT<t) return false;

		Vfloat3 xyz = ray.PointAt(t);
		float y = xyz.Y().AsFloat();
		float z = xyz.Z().AsFloat();
		if(y < m_y0 || m_y1 < y) return false;
		if(z < m_z0 || m_z1 < z) return false;
		
		outRecord.u = (y - m_y0) / (m_y1 - m_y0);
		outRecord.v = (z - m_z0) / (m_z1 - m_z0);
		outRecord.t = t;
		outRecord.material = m_material;
		outRecord.position = xyz;
		outRecord.normal = Vfloat3(1,0,0);

		return true;
	}
	
	virtual bool BoundingBox(float t0, float t1, Aabb& outAabb) const override
	{
		outAabb = Aabb(Vfloat3(m_x-0.001f, m_y0, m_z0), Vfloat3(m_x+0.001f, m_y1, m_z1));
		return true;
	}

private:
	float m_y0, m_y1;
	float m_z0, m_z1;
	float m_x;
	const Material* m_material;
};

class FlipNormal : public Hitable
{
public:
	FlipNormal(const Hitable* h)
		: m_hitable(h)
	{
	}	

	bool Hit(const Ray& ray, float minT, float maxT, HitRecord& outRecord) const override
	{
		if(m_hitable->Hit(ray, minT, maxT, outRecord))
		{
			outRecord.normal = -outRecord.normal;
			return true;
		}
		else
		{
			return false;
		}
	}
	
	virtual bool BoundingBox(float t0, float t1, Aabb& outAabb) const override
	{
		return m_hitable->BoundingBox(t0, t1, outAabb);
	}

private:
	const Hitable* m_hitable;
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
			outRecord.normal = Math::Normalize(outRecord.position - Center());// / Radius();
			outRecord.material = material;
			GetUv(outRecord.position, outRecord.u, outRecord.v);
			return true;
		}
		
		t = (-b + sqrtDiscriminant) / (2.0f * a);
		if(minT < t && t < maxT)
		{
			outRecord.t = t;
			outRecord.position = ray.PointAt(t);
			outRecord.normal = Math::Normalize(outRecord.position - Center());// / Radius();
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
			outRecord.normal = Math::Normalize(outRecord.position - Center(ray.Time()));// / Radius();
			outRecord.material = material;
			GetUv(outRecord.position, ray.Time(), outRecord.u, outRecord.v);
			return true;
		}
		
		t = (-b + sqrtDiscriminant) / (2.0f * a);
		if(minT < t && t < maxT)
		{
			outRecord.t = t;
			outRecord.position = ray.PointAt(t);
			outRecord.normal = Math::Normalize(outRecord.position - Center(ray.Time()));// / Radius();
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

	HitableList(int c) : m_hitables(c), m_bvh(nullptr)
	{
		Clear();
	}

	virtual ~HitableList()
	{
		delete m_bvh;
		m_bvh = nullptr;
	}

	void Clear()
	{
		if(!m_hitables.empty())
		{
			std::fill(m_hitables.begin(), m_hitables.end(), nullptr);
		}
	}
	
	bool Build(float t0, float t1)
	{
		SI_ASSERT(m_bvh==nullptr);

		m_bvh = new BvhNode();
		m_bvh->BuildBvh(&m_hitables[0], (int)m_hitables.size(), t0, t1);

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

		for(int i=0; i<m_hitables.size(); ++i)
		{
			const Hitable* h = m_hitables[i];
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
		if(m_hitables.empty()) return false;

		if(!m_hitables[0]->BoundingBox(t0, t1, outAabb)) return false;

		Aabb tmpAabb;
		for(int i=1; i<m_hitables.size(); ++i)
		{
			if(!m_hitables[i]->BoundingBox(t0, t1, tmpAabb)) return false;

			outAabb = CombineAabb(outAabb, tmpAabb);
		}

		return true;
	}

protected:
	std::vector<const Hitable*> m_hitables;
	BvhNode *m_bvh;
};

class SceneBase : public HitableList
{
public:
	SceneBase()
	{
	}

	virtual ~SceneBase()
	{
		for(const Hitable* hitable : m_hitables)
		{
			delete hitable;
		}
		m_hitables.clear();

		for(const Hitable* hitable : m_coreHitables)
		{
			delete hitable;
		}
		m_coreHitables.clear();

		for(Material* material : m_materials)
		{
			delete material;
		}
		m_materials.clear();

		for(const Texture* texture : m_textures)
		{
			delete texture;
		}
		m_textures.clear();
	}

protected:
	std::vector<Texture*>  m_textures;
	std::vector<Material*> m_materials;
	std::vector<Hitable*>  m_coreHitables; // Hitableとして登録しないが、作成したいHitable(FlipNormal用)
};

class Box : public SceneBase
{
public:
	Box(const Vfloat3& p0, const Vfloat3& p1, const Material* material)
		: m_p0(p0)
		, m_p1(p1)
		, m_material(material)
	{
		float p0x = m_p0.X().AsFloat();
		float p0y = m_p0.Y().AsFloat();
		float p0z = m_p0.Z().AsFloat();
		float p1x = m_p1.X().AsFloat();
		float p1y = m_p1.Y().AsFloat();
		float p1z = m_p1.Z().AsFloat();
		
		m_hitables.push_back( new XYRect(p0x, p1x, p0y, p1y, p1z, material ) );
		m_coreHitables.push_back( new XYRect(p0x, p1x, p0y, p1y, p0z, material ) );
		m_hitables.push_back( new FlipNormal(m_coreHitables.back()) );
		
		m_hitables.push_back( new XZRect(p0x, p1x, p0z, p1z, p1y, material ) );
		m_coreHitables.push_back( new XZRect(p0x, p1x, p0z, p1z, p0y, material ) );
		m_hitables.push_back( new FlipNormal(m_coreHitables.back()) );
		
		m_hitables.push_back( new YZRect(p0y, p1y, p0z, p1z, p1x, material ) );
		m_coreHitables.push_back( new YZRect(p0y, p1y, p0z, p1z, p0x, material ) );
		m_hitables.push_back( new FlipNormal(m_coreHitables.back()) );
	}
	
	virtual bool BoundingBox(float t0, float t1, Aabb& outAabb) const override
	{
		outAabb = Aabb(m_p0, m_p1);
		return true;
	}

private:
	Vfloat3 m_p0;
	Vfloat3 m_p1;
	const Material* m_material;
};

class CornellBox : public SceneBase
{
public:
	CornellBox()
	{
		m_textures.push_back( new ConstantTexture(Vfloat3(0.65f, 0.05f, 0.05f)) );
		m_textures.push_back( new ConstantTexture(Vfloat3(0.73f, 0.73f, 0.73f)) );
		m_textures.push_back( new ConstantTexture(Vfloat3(0.12f, 0.45f, 0.15f)) );
		m_textures.push_back( new ConstantTexture(Vfloat3(15, 15, 15)) );
		m_textures.push_back( new ConstantTexture(Vfloat3(1, 1, 1)) );
		m_textures.push_back( new ConstantTexture(Vfloat3(0, 0, 0)) );
		
		m_materials.push_back( new Lambert(m_textures[0]) );
		const Material* red = m_materials.back();
		m_materials.push_back( new Lambert(m_textures[1]) );
		const Material* white = m_materials.back();		
		m_materials.push_back( new Lambert(m_textures[2]) );
		const Material* green = m_materials.back();
		m_materials.push_back( new DiffuseLight(m_textures[3]) );
		const Material* light = m_materials.back();
		m_materials.push_back( new Isotropic(m_textures[4]) );
		const Material* isotopic0 = m_materials.back();
		m_materials.push_back( new Isotropic(m_textures[5]) );
		const Material* isotopic1 = m_materials.back();
		
		m_coreHitables.push_back( new YZRect(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, green ) );
		m_hitables.push_back( new FlipNormal(m_coreHitables.back()) );
		
		m_hitables.push_back( new YZRect(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, red ) );
		
		m_hitables.push_back( new XZRect(213.0f, 343.0f, 227.0f, 332.0f, 554.0f, light ) );
		
		m_coreHitables.push_back( new XZRect(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, white ) );
		m_hitables.push_back( new FlipNormal(m_coreHitables.back()) );
		
		m_hitables.push_back( new XZRect(0.0f, 555.0f, 0.0f, 555.0f, 0.0f, white ) );

		m_coreHitables.push_back( new XYRect(0.0f, 555.0f, 0.0f, 555.0f, 555.0f, white ) );
		m_hitables.push_back( new FlipNormal(m_coreHitables.back()) );
		
#if 0
		m_coreHitables.push_back( new Box(Vfloat3(0, 0, 0), Vfloat3(165, 165, 165), white) );
		m_coreHitables.push_back( new RotateY(m_coreHitables.back(), -18*SI::kPi/180.0f) );
		m_hitables.push_back( new Translate(m_coreHitables.back(), Vfloat3(130, 0, 65)) );
		
		m_coreHitables.push_back( new Box(Vfloat3(0, 0, 0), Vfloat3(165, 330, 165), white) );
		m_coreHitables.push_back( new RotateY(m_coreHitables.back(), 15*SI::kPi/180.0f) );
		m_hitables.push_back( new Translate(m_coreHitables.back(), Vfloat3(265, 0, 295)) );
#else
		m_coreHitables.push_back( new Box(Vfloat3(0, 0, 0), Vfloat3(165, 165, 165), white) );
		m_coreHitables.push_back( new RotateY(m_coreHitables.back(), -18*SI::kPi/180.0f) );
		m_coreHitables.push_back( new Translate(m_coreHitables.back(), Vfloat3(130, 0, 65)) );
		m_hitables.push_back( new ConstantMedium(m_coreHitables.back(), 0.01f, (Isotropic*)isotopic0) );
		
		m_coreHitables.push_back( new Box(Vfloat3(0, 0, 0), Vfloat3(165, 330, 165), white) );
		m_coreHitables.push_back( new RotateY(m_coreHitables.back(), 15*SI::kPi/180.0f) );
		m_coreHitables.push_back( new Translate(m_coreHitables.back(), Vfloat3(265, 0, 295)) );
		m_hitables.push_back( new ConstantMedium(m_coreHitables.back(), 0.01f, (Isotropic*)isotopic1) );
#endif
	}
};

class FinalScene : public SceneBase
{
public:
	FinalScene()
	{
		m_textures.push_back( new ConstantTexture(Vfloat3(0.73f, 0.73f, 0.73f)) );
		const Texture* whiteTex = m_textures.back();
		m_textures.push_back( new ConstantTexture(Vfloat3(0.48f, 0.83f, 0.53f)) );
		const Texture* groundTex = m_textures.back();
		m_textures.push_back( new ConstantTexture(Vfloat3(7,7,7)) );
		const Texture* lightTex = m_textures.back();
		m_textures.push_back( new ConstantTexture(Vfloat3(0.7f, 0.3f, 0.1f)) );
		const Texture* movingTex = m_textures.back();
		m_textures.push_back( new ConstantTexture(Vfloat3(0.2f, 0.4f, 0.9f)) );
		const Texture* fogTex0 = m_textures.back();
		m_textures.push_back( new ConstantTexture(Vfloat3(1.0f, 1.0f, 1.0f)) );
		const Texture* fogTex1 = m_textures.back();
		char ddsFilePath[260];
		sprintf_s(ddsFilePath, "%stexture\\test_texture_rgba8.dds", SI_PATH_STORAGE().GetAssetDirPath());
		m_textures.push_back( new DdsTexture(ddsFilePath) );
		const Texture* ddsTex = m_textures.back();
		m_textures.push_back( new NoiseTexture(0.1f) );
		const Texture* noiseTex = m_textures.back();
		
		m_materials.push_back( new Lambert(whiteTex) );
		const Material* white = m_materials.back();
		m_materials.push_back( new Lambert(groundTex) );
		const Material* ground = m_materials.back();
		m_materials.push_back( new DiffuseLight(lightTex) );
		const Material* light = m_materials.back();
		m_materials.push_back( new Lambert(movingTex) );
		const Material* movingMat = m_materials.back();
		m_materials.push_back( new Dielectric(1.5f) );
		const Material* dielectric= m_materials.back();
		m_materials.push_back( new Metal(Vfloat3(0.8f, 0.8f, 0.9f), 10.0f) );
		const Material* metal= m_materials.back();
		m_materials.push_back( new Isotropic(fogTex0) );
		const Isotropic* isotropic0 = (Isotropic*)m_materials.back();
		m_materials.push_back( new Isotropic(fogTex1) );
		const Isotropic* isotropic1 = (Isotropic*)m_materials.back();
		m_materials.push_back( new Lambert(ddsTex) );
		const Material* imageMat = m_materials.back();
		m_materials.push_back( new Lambert(noiseTex) );
		const Material* noiseMat = m_materials.back();

		const int groundBoxCount = 20;
		for(int i=0; i<groundBoxCount; ++i)
		{
			for(int j=0; j<groundBoxCount; ++j)
			{
				float w = 100.0f;
				Vfloat3 p0(
					-1000.0f + i*w,
					0.0f,
					-1000.0f + j*w);
				Vfloat3 p1(
					p0.X().AsFloat() + w,
					100.0f * (FloatUnitRand() + 0.01f),
					p0.Z().AsFloat() + w);

				m_hitables.push_back( new Box(p0, p1, ground) );
			}
		}
		
		m_hitables.push_back( new XZRect(123.0f, 423.0f, 147.0f, 412.0f, 554.0f, light ) );

		Vfloat3 center(400, 400, 200);
		m_hitables.push_back( new MovingSphere(center, center+Vfloat3(30,0,0), 0, 1, 50, movingMat) );
		
		m_hitables.push_back( new Sphere(Vfloat3(260, 150, 45), 50, dielectric) );
		m_hitables.push_back( new Sphere(Vfloat3(0, 150, 145), 50, metal) );
		
		m_coreHitables.push_back( new Sphere(Vfloat3(360, 150, 145), 70, dielectric) );
		m_hitables.push_back( new ConstantMedium(m_coreHitables.back(), 0.2f, isotropic0) );
		
		m_coreHitables.push_back( new Sphere(Vfloat3(0.0f), 5000, dielectric) );
		m_hitables.push_back( new ConstantMedium(m_coreHitables.back(), 0.0001f, isotropic1) );

		
		m_hitables.push_back( new Sphere(Vfloat3(400, 200, 400), 100, imageMat) );
		m_hitables.push_back( new Sphere(Vfloat3(220, 280, 300), 80, noiseMat) );

		const int ballCount = 1000;
		for(int i=0; i<ballCount; ++i)
		{
			m_coreHitables.push_back( new Sphere(165*Vfloat3(FloatUnitRand(), FloatUnitRand(), FloatUnitRand()), 10, white) );
			m_coreHitables.push_back( new RotateY(m_coreHitables.back(), 15*SI::kPi/180.0f) );
			m_hitables.push_back( new Translate(m_coreHitables.back(), Vfloat3(-100, 270, 395)) );
		}
	}
};

class SimpleLightScene : public SceneBase
{
public:
	SimpleLightScene()
	{
		m_textures.push_back( new NoiseTexture(4.0f) );
		m_textures.push_back( new ConstantTexture(Vfloat3(4.0f)) );
		
		m_materials.push_back( new Lambert(m_textures[0]) );
		m_materials.push_back( new DiffuseLight(m_textures[1]) );
		m_materials.push_back( new Metal() );
		
		m_hitables.push_back( new Sphere(Vfloat3(0, -1000, 0), 1000.0f, m_materials[0] ) );
		m_hitables.push_back( new Sphere(Vfloat3(0, 2, 0), 2.0f, m_materials[0] ) );
		m_hitables.push_back( new Sphere(Vfloat3(0, 7, 0), 2.0f, m_materials[1] ) );
		m_hitables.push_back( new XYRect(3.0f, 5.0f, 0.50f, 3.0f, -2.0f, m_materials[1] ) );
	}
};

class TwoPerlinSphere : public SceneBase
{
public:
	TwoPerlinSphere()
	{
		m_textures.push_back( new NoiseTexture(4) );

		char ddsFilePath[260];
		sprintf_s(ddsFilePath, "%stexture\\test_texture_rgba8.dds", SI_PATH_STORAGE().GetAssetDirPath());
		m_textures.push_back( new DdsTexture(ddsFilePath) );

		m_materials.push_back( new Lambert(m_textures[0]) );
		m_materials.push_back( new Lambert(m_textures[1]) );

		m_hitables.push_back( new Sphere(Vfloat3(0, -1000, 0), 1000, m_materials[0]) );
		m_hitables.push_back( new Sphere(Vfloat3(0, 2, 0), 2, m_materials[1]) );
	}
};

class ManySpheres : public SceneBase
{
public:
	ManySpheres(float time0, float time1)
	{
		float timeDif = time1 - time0;
		m_textures.push_back( new ConstantTexture(Vfloat3(0.2f, 0.3f, 0.1f)) );
		m_textures.push_back( new ConstantTexture(Vfloat3(0.9f, 0.9f, 0.9f)) );
		m_textures.push_back( new CheckerTexture(m_textures[0], m_textures[1]) ); // checker
	
		m_textures.push_back( new ConstantTexture(Vfloat3(1.0f, 0.5f, 0.5f)) );
		m_textures.push_back( new ConstantTexture(Vfloat3(0.5f, 0.5f, 0.5f)) );
		m_textures.push_back( new ConstantTexture(Vfloat3(0.2f, 0.2f, 0.9f)) );

		size_t lambertId = m_materials.size();
		m_materials.push_back( new Lambert(m_textures[2]) );
		m_materials.push_back( new Lambert(m_textures[3]) );
		m_materials.push_back( new Lambert(m_textures[4]) );
		m_materials.push_back( new Lambert(m_textures[5]) );
		size_t metalId = m_materials.size();
		m_materials.push_back( new Metal(Vfloat3(1.0f, 1.0f, 1.0f), 0.0f) );
		m_materials.push_back( new Metal(Vfloat3(1.0f, 0.5f, 0.5f), 0.5f) );
		size_t dielectricId = m_materials.size();
		m_materials.push_back( new Dielectric(1.5f) );
		m_materials.push_back( new Dielectric(2.4f) );

		m_hitables.push_back( new Sphere(Vfloat3(0.0f, -1000.f, 0.0f), 1000.0f, m_materials[lambertId]) );
		m_hitables.push_back( new Sphere(Vfloat3(-4.0f, 1.0f, -1.0f), 1.0f, m_materials[dielectricId]) );
		m_hitables.push_back( new Sphere(Vfloat3(0.0f, 1.0f, -1.0f), 1.0f, m_materials[lambertId+1]) );
		m_hitables.push_back( new Sphere(Vfloat3(4.0f, 1.0f, -1.0f), 1.0f, m_materials[metalId]));
		
		int randCount = 6;
		int allRandCount = (2 * randCount + 1) * (2 * randCount + 1);

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
					m_textures.push_back( new ConstantTexture(RandomInUnitSphere()*RandomInUnitSphere()) );
					m = new Lambert(m_textures.back());
					m_materials.push_back(m);
					m_hitables.push_back(new MovingSphere(center, center+Vfloat3(0, 0.5f*timeDif*FloatUnitRand(), 0), time0, time1, 0.2f, m));
				}
				else if(randKey < 0.9f)
				{
					m = new Metal(0.5f * (Vfloat3(1) + RandomInUnitSphere()), 0.5f * FloatUnitRand());
					m_materials.push_back(m);
					m_hitables.push_back(new Sphere(center, 0.2f, m));
				}
				else
				{
					m = new Dielectric(1.5f);
					m_materials.push_back(m);
					m_hitables.push_back(new Sphere(center, 0.2f, m));
				}
			}
		}
	#endif
	}
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

Vfloat3 RayToColor(const Ray& r, const Hitable& world, int bounce = 0)
{
	HitRecord record;
	if(world.Hit(r, 0.001f, FLT_MAX, record))
	{
		Ray scatteredRay;
		Vfloat3 attenuation;
		Vfloat3 emissive = record.material->Emitted(record.u, record.v, record.position);
		if( bounce < 64 && record.material->Scatter(r, record, attenuation, scatteredRay) )
		{
			return emissive + attenuation * RayToColor(scatteredRay, world, bounce+1);
		}
		else
		{
			return emissive;
		}
	}
	else
	{
#if 1
		return Vfloat3(0.0f);
#else
		Vfloat3 unitDir = (r.Dir()).Normalize();

		float t = 0.5f*unitDir.Y() + 0.5f;
		return Lerp(Vfloat3(1.0f), Vfloat3(0.5f, 0.7f, 1.0f), t);
#endif
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
#elif 0
	Vfloat3 cameraPos(13,2,3);
	float vFov = 20;
	Vfloat3 cameraTarget(0.0f, 0.0f, 0.0f);
	float focusDist = 10;
#elif 0
	Vfloat3 cameraPos(4,2,4);
	float vFov = 60;
	Vfloat3 cameraTarget(0.0f, 2.0f, 0.0f);
	float focusDist = (cameraTarget - cameraPos).Length();
#elif 0
	Vfloat3 cameraPos(8,3,2);
	float vFov = 70;
	Vfloat3 cameraTarget(0.0f, 1.0f, -1.0f);
	float focusDist = (cameraTarget - cameraPos).Length();
#elif 0
	Vfloat3 cameraPos(278,278,-800);
	float vFov = 40;
	Vfloat3 cameraTarget(278.0f, 278.0f, 0.0f);
	float focusDist = (cameraTarget - cameraPos).Length();
#else
	Vfloat3 cameraPos(478,278,-800);
	float vFov = 30;
	Vfloat3 cameraTarget(278.0f, 278.0f, 0.0f);
	float focusDist = (cameraTarget - cameraPos).Length();
#endif
	float apature = 0.05f;

	float time0 = 0;
	float time1 = 1;
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
#elif 0
	TwoPerlinSphere world;
#elif 0
	SimpleLightScene world;
#elif 0
	CornellBox world;
#else
	FinalScene world;
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

	{
		auto start2 = std::chrono::system_clock::now();
		SI_SCOPE_EXIT( SI_PRINT("RaytracingTime=%dms\n", (int)std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::system_clock::now() - start2)).count()); );

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
	}

	return std::move(data);
}

} // namespace RayTracingTheNextWeek
} // namespace SI
