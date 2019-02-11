
#include "raytracing_in_one_weekend.h"

#include <si_base/core/core.h>
#include <si_base/math/math.h>
#include <si_base/core/constant.h>
#include <random>
#include <chrono>
#include <si_base/concurency/atomic.h>
#include <si_base/concurency/mutex.h>
//#include <omp.h>

namespace SI
{
namespace APP005
{

Vfloat3 Lerp(const Vfloat3& a, const Vfloat3& b, float t){ return (1-t)*a + t*b; }

class Ray
{
public: 
	Ray() {}
	~Ray(){}
	Ray(const Vfloat3& pos, const Vfloat3& dir)
		: position(pos)
		, direction(dir)
	{}
	
	const Vfloat3& Pos() const{ return position; }
	const Vfloat3& Dir() const{ return direction; }
	Vfloat3 PointAt(float t) const{ return position + t * direction; }

private:
	Vfloat3 position;
	Vfloat3 direction;
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

class Material;

struct HitRecord
{
	HitRecord() : t(0.0f), position(0.0f), normal(0.0f, 1.0f, 0.0f), material(nullptr) {}

	float t;
	Vfloat3 position;
	Vfloat3 normal;
	const Material* material;
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
			if(100 < ++tryCount) return false;
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
	explicit Lambert(const Vfloat3& a = Vfloat3(1.0f)) : albedo(a){}
	virtual ~Lambert(){}

	virtual bool Scatter(const Ray& ray, const HitRecord& record, Vfloat3& outAttenuation, Ray& outRay) const override
	{
		outRay = Ray(record.position, record.normal + RandomInUnitSphere());
		outAttenuation = albedo;
		return true;
	}

private:
	Vfloat3 albedo;
};

class Hitable
{
public:
	Hitable(){}
	virtual ~Hitable(){}

	virtual bool Hit(const Ray& ray, float minT, float maxT, HitRecord& outRecord) const = 0;
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
			return true;
		}
		
		t = (-b + sqrtDiscriminant) / (2.0f * a);
		if(minT < t && t < maxT)
		{
			outRecord.t = t;
			outRecord.position = ray.PointAt(t);
			outRecord.normal = (outRecord.position - Center()) / Radius();
			outRecord.material = material;
			return true;
		}

		return false;
	}

private:
	Vfloat3 center;
	float radius;
	const Material* material;
};

class HitableList : public Hitable
{
public:
	HitableList() : list(nullptr), count(0)
	{
	}

	HitableList(int c) : list(new const Hitable* [c]), count(c)
	{
		Clear();
	}

	virtual ~HitableList()
	{
		delete[] list;
		count = 0;
	}

	void Reserve(int c)
	{
		delete[] list;
		list = new const Hitable* [c];
		Clear();
		count = c;
	}

	void Clear()
	{
		if(0<count && list)
		{
			memset(list, 0, sizeof(const Hitable*)*count);
		}
	}

	bool Set(int id, const Hitable* h)
	{
		if(count<=id) return false;
		if(id<0) return false;

		list[id] = h;
		return true;
	}
	
	bool Hit(const Ray& ray, float minT, float maxT, HitRecord& outRecord) const override
	{
		bool hitAnything = false;
		float closestT = maxT;
		HitRecord tmpRecord;

		for(int i=0; i<count; ++i)
		{
			const Hitable* h = list[i];
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

public:
	const Hitable** list;
	int count;
};

class Camera
{
public:
	Camera(uint32_t width=1920, uint32_t height=1080)
	{
		float aspect = (float)width / (float)height;
	
		origin = Vfloat3(0.0f);
		topLeft = Vfloat3(-aspect, 1, -1);
		horizon = Vfloat3(2.0f*aspect, 0.0f, 0.0f);
		vertical = Vfloat3(0.0f, -2.0f, 0.0f);
	}

	Ray CalcRay(float u, float v)
	{
		Vfloat3 dir = topLeft + u * horizon + v * vertical;
		Ray r(origin, dir);
		return r;
	}

public:
	Vfloat3 origin;
	Vfloat3 topLeft;
	Vfloat3 horizon;
	Vfloat3 vertical;
};

class CameraEx
{
public:
	CameraEx(Vfloat3 camPos, Vfloat3 target, Vfloat3 vUp, float vFov, float aspect, float aparture, float focusDist)
	{
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

	~CameraEx(){}

	Ray CalcRay(float s, float t)
	{
		Vfloat3 rd = lensRadius * RandomInUnitDisk();
		Vfloat3 offset = u * rd.X() + v * rd.Y();
		return Ray(origin + offset, lowerLeftCorner + s*horizontal + t*vertical - (origin + offset));
	}

private:
	Vfloat3 origin;
	Vfloat3 lowerLeftCorner;
	Vfloat3 horizontal;
	Vfloat3 vertical;
	Vfloat3 u,v,w;
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

	Lambert lamberts[] =
	{
		Lambert(Vfloat3(0.8f, 0.8f, 0.8f)),
		Lambert(Vfloat3(1.0f, 0.5f, 0.5f)),
		Lambert(Vfloat3(0.2f, 0.2f, 0.9f)),
		Lambert(Vfloat3(0.7f, 0.2f, 0.5f)),
	};

	Metal metals[] =
	{
		Metal(Vfloat3(1.0f, 1.0f, 1.0f), 0.0f),
		Metal(Vfloat3(1.0f, 0.5f, 0.5f), 0.5f),
	};

	Dielectric dielectrics[] = 
	{
		Dielectric(1.5f),
		Dielectric(2.4f),
	};

	Vfloat3 cameraPos(4,2,4);
	Vfloat3 cameraTarget(0.0f, 0.0f, -1.0f);
	float focusDist = (cameraTarget - cameraPos).Length();
	float apature = 0.05f;

	Camera camera3(width, height);
	CameraEx camera(
		cameraPos,
		cameraTarget,
		Vfloat3(0.0f, 1.0f ,0.0f),  // vup
		90.0f,
		(float)width/(float)height,
		apature, // aparture
		focusDist);

	Sphere spheres[] = 
	{
		Sphere(Vfloat3(0.0f, -10000.f, 0.0f), 10000.0f, &lamberts[0]),
		Sphere(Vfloat3(-4.0f, 1.0f, -1.0f), 1.0f, &dielectrics[0]),
		Sphere(Vfloat3(0.0f, 1.0f, -1.0f), 1.0f, &lamberts[1]),
		Sphere(Vfloat3(4.0f, 1.0f, -1.0f), 1.0f, &metals[0]),
		//Sphere(Vfloat3(2.0f, 0.20f, -1.0f), 0.5f, &lamberts[2]),
		//Sphere(Vfloat3(-2.0f, 0.20f, -1.0f), 0.5f, &metals[1]),
	};
	int sphereCount = (int)(sizeof(spheres)/sizeof(spheres[0]));
		
	std::vector<Material*> randMaterials;
	std::vector<Sphere*> randSpheres;
	int randCount = 8;
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
				m = new Lambert(RandomInUnitSphere()*RandomInUnitSphere());
			}
			else if(randKey < 0.9f)
			{
				m = new Metal(0.5f * (Vfloat3(1) + RandomInUnitSphere()), 0.5f * FloatUnitRand());
			}
			else
			{
				m = new Dielectric(1.5f);
			}

			randMaterials.push_back(m);
			randSpheres.push_back(new Sphere(center, 0.2f, m));
		}
	}
#endif

	HitableList hitables(sphereCount + (int)randSpheres.size());
	for(int i=0; i<sphereCount; ++i)
	{
		hitables.Set(i, &spheres[i]);
	}
	for(size_t i=0; i<randSpheres.size(); ++i)
	{
		Sphere* s = randSpheres[i];
		hitables.Set((int)i + sphereCount, s);
	}

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
				Vfloat3 sampleColor = RayToColor(ray, hitables);

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

	for(Sphere* s : randSpheres)
	{
		delete s;
	}
	randSpheres.clear();
		
	for(Material* m : randMaterials)
	{
		delete m;
	}
	randMaterials.clear();

	return std::move(data);
}

} // namespace APP005
} // namespace SI
