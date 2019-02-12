
#define THREAD_X 8
#define THREAD_Y 8

#ifndef MODE
#define MODE 14
#endif

#define SAMPLE_COUNT 16

#define PLACE_COUNT 2
#define PLACE_COUNT_ALL (((2*PLACE_COUNT+1)*(2*PLACE_COUNT+1)) + 4)

#define FLT_MAX 3.40282e+38
#define M_PI 3.14159265359

cbuffer RootConstants : register(b0)
{
	uint width;
	uint height;
};

Texture2D<float4>   inputTexture  : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

#define MATERIAL_LAMBERT       0
#define MATERIAL_METAL         1
#define MATERIAL_DIELECTRICS   2

class Material
{
	int type;
	float3 albedo;
	float fuzz;
	float refIndex;
};

struct HitRecord
{
	float t;
	float3 pos;
	float3 normal;

	Material material;
};

struct Sphere
{
	float3 center;
	float radius;

	Material material;
};

class Ray
{
	float3 pos;
	float3 dir;

	float3 PointAt(float t)
	{
		return pos + t * dir;
	}
};

float3 RayToColor(Ray ray)
{
	float3 unitDir = normalize(ray.dir);

	float t = 0.5f*unitDir.y + 0.5f;
	return lerp(1.0.xxx, float3(0.5f, 0.7f, 1.0f), t);
}

bool HitSphere(Sphere sphere, Ray ray)
{
	float3 oc = ray.pos - sphere.center;
	float a = dot(ray.dir, ray.dir);
	float b = 2 * dot( oc, ray.dir );
	float c = dot(oc,oc) - sphere.radius * sphere.radius;
	float discriminant = b*b - 4*a*c;
	return discriminant > 0;
}

float HitSphere2(Sphere sphere, Ray ray)
{
	float3 oc = ray.pos - sphere.center;
	float a = dot(ray.dir, ray.dir);
	float b = 2 * dot( oc, ray.dir );
	float c = dot(oc,oc) - sphere.radius * sphere.radius;
	float discriminant = b*b - 4*a*c;
	if(discriminant < 0) return -1;

	return (-b - sqrt(discriminant)) / (2 * a);
}

#define HITTABLE_TYPE_SPHERE 1

class Hittable
{
	int type;

	Sphere sphere;

	bool Hit(Ray ray, float2 tMinMax, out HitRecord record)
	{
		record = (HitRecord)0;
		if(type == HITTABLE_TYPE_SPHERE)
		{
			float3 oc = ray.pos - sphere.center;
			float a = dot(ray.dir, ray.dir);
			float b = 2 * dot( oc, ray.dir );
			float c = dot(oc,oc) - sphere.radius * sphere.radius;
			float discriminant = b*b - 4*a*c;
			if(discriminant < 0) return false;
			
			float tmp = (-b - sqrt(discriminant)) / (2 * a);
			if(tMinMax.x < tmp && tmp < tMinMax.y)
			{
				record.t = tmp;
				record.pos = ray.PointAt(tmp);
				record.normal = (record.pos - sphere.center) / sphere.radius;
				record.material = sphere.material;
				return true;
			}
			
			tmp = (-b + sqrt(discriminant)) / (2 * a);
			if(tMinMax.x < tmp && tmp < tMinMax.y)
			{
				record.t = tmp;
				record.pos = ray.PointAt(tmp);
				record.normal = (record.pos - sphere.center) / sphere.radius;
				record.material = sphere.material;
				return true;
			}
		}

		return false;
	}
};

class HittableList
{
	int count;

#if MODE>=14
	Hittable list[PLACE_COUNT_ALL];
#else
	Hittable list[5];
#endif
	
	bool Hit(Ray ray, float2 tMinMax, out HitRecord record)
	{
		bool hitAnything = false;
		record = (HitRecord)0;

		for(int i=0; i<count; ++i)
		{
			HitRecord tmpRecord = (HitRecord)0;
			if(list[i].Hit(ray, tMinMax, tmpRecord))
			{
				hitAnything = true;
				tMinMax.y = tmpRecord.t;
				record = tmpRecord;
			}
		}

		return hitAnything;
	}
};

float3 Rand(float2 uv, float3 seed)
{
	return frac(sin(dot(uv, float2(12.9898, 78.233)) + seed) * 43758.5453);
}

float3 RandomInUnitSphere(float2 uvSeed)
{
	float3 p;
	float3 seed = float3(0.552,0.313,0.843);
	do{
		p = Rand(uvSeed, seed);
		seed += float3(0.1332, 0.3313, 0.22221);
	}while(dot(p,p)>=1.0);

	return p;
}

class Camera
{
	float3 lowerLeftCorner;
	float3 horizontal;
	float3 vertical;
	float3 origin;
	
#if MODE>=13
	float lensRadius;
	float3 u,v,w;
	void Setup(float3 lookFrom, float3 lookAt, float3 vUp, float fov, float aspect, float apature, float focus)
	{
		lensRadius = apature/2;
		float theta = fov * (M_PI / 180.0);
		float halfHeight = tan(theta/2);
		float halfWidth = halfHeight * aspect;

		w = normalize(lookFrom - lookAt);
		u = normalize(cross(vUp, w));
		v = cross(w,u);
				
		origin = lookFrom;
		lowerLeftCorner = origin - halfWidth * focus * u - halfHeight * focus * v - focus * w;
		horizontal = 2 * halfWidth * focus * u;
		vertical = 2 * halfHeight * focus * v;
	}

	Ray GetRay(float2 uv)//, float2 seed)
	{
		float2 seed = uv;
		float3 rd = lensRadius * RandomInUnitSphere(seed);
		float3 offset =	u * rd.x + v * rd.y;

		Ray ray = (Ray)0;
		ray.pos = origin + offset;
		ray.dir = lowerLeftCorner + uv.x * horizontal + uv.y * vertical - (origin + offset);

		return ray;
	}
#else
	void Setup(float3 lookFrom, float3 lookAt, float3 vUp, float fov, float aspect)
	{
		float theta = fov * (M_PI / 180.0);
		float halfHeight = tan(theta/2);
		float halfWidth = halfHeight * aspect;

		float3 w = normalize(lookFrom - lookAt);
		float3 u = normalize(cross(vUp, w));
		float3 v = cross(w,u);
				
		origin = lookFrom;
		//lowerLeftCorner = float3(-halfWidth, -halfHeight, -1);
		lowerLeftCorner = origin - halfWidth * u - halfHeight * v - w;
		horizontal = 2 * halfWidth * u;
		vertical = 2 * halfHeight * v;
	}

	Ray GetRay(float2 uv)
	{
		Ray ray = (Ray)0;
		ray.pos = origin;
		ray.dir = lowerLeftCorner + uv.x * horizontal + uv.y * vertical - origin;

		return ray;
	}
#endif
};

float3 CalcColor(Ray ray, HittableList inworld, float2 uv)
{
	HitRecord record = (HitRecord)0;
	if(inworld.Hit(ray, float2(0,FLT_MAX), record))
	{
		return 0.5 * (record.normal + 1.0.xxx);
	}
	else
	{
		return RayToColor(ray);
	}
}
//
//float3 CalcColor2(Ray ray, HittableList inworld, float2 uv)
//{
//	HitRecord record = (HitRecord)0;
//	if(inworld.Hit(ray, float2(0,FLT_MAX), record))
//	{
//		float3 target = record.pos + record.normal + RandomInUnitSphere(uv);
//		Ray newRay;
//		newRay.pos = record.pos;
//		newRay.dir = target - record.pos;
//		return 0.5 * CalcColor(newRay, inworld, uv);
//	}
//	else
//	{
//		return RayToColor(ray);
//	}
//}

float3 CalcColor2(Ray ray, HittableList inworld, float2 uv)
{
	const int maxBounds = 100;
	float scale = 1;
	for(int i=0; i<maxBounds; ++i)
	{
		HitRecord record = (HitRecord)0;
		if(inworld.Hit(ray, float2(0.001,FLT_MAX), record))
		{
			float3 target = record.pos + record.normal + RandomInUnitSphere(uv+float2(0.323,0.421)*(float)i);
			Ray newRay;
			newRay.pos = record.pos;
			newRay.dir = target - record.pos;
			
			ray = newRay;
			scale *= 0.5;
		}
		else
		{
			return scale * RayToColor(ray);
		}
	}

	return 0.0;//scale * RayToColor(ray);
}

float3 Reflect(float3 v, float3 n)
{
	return v - 2 * dot(v,n)*n;
}

bool Refract(float3 v, float3 n, float niOverNt, out float3 refracted)
{
	float3 unitV = normalize(v);
	float dt = dot(unitV, n);
	float discriminant = 1 - niOverNt*niOverNt*(1-dt*dt);
	if(0<discriminant)
	{
		refracted = niOverNt*(unitV - n*dt) - n * sqrt(discriminant);
		return true;
	}
	refracted = 0.0.xxx;
	return false;
}

float Schlick(float cosine, float refIndex)
{
	float r0 = (1-refIndex)/(1+refIndex);
	r0 = r0*r0;
	return r0 + (1-r0) * pow(1-cosine, 5);
}

bool MaterialScatter(Ray ray, HitRecord record, out float3 attenuation, out Ray scattered, float2 seed)
{
	attenuation = 0.0.xxx;
	scattered = (Ray)0;
	if(record.material.type == MATERIAL_LAMBERT)
	{
		float3 target = record.pos + record.normal + RandomInUnitSphere(seed);
		scattered.pos = record.pos;
		scattered.dir = target - record.pos;
		attenuation = record.material.albedo;
		return true;
	}
	else if(record.material.type == MATERIAL_METAL)
	{
		float3 reflected = Reflect(normalize(ray.dir), record.normal);
		scattered.pos = record.pos;
		scattered.dir = reflected;
#if MODE>=9
		scattered.dir += record.material.fuzz * RandomInUnitSphere(seed);
#endif
		attenuation = record.material.albedo;

		return (dot(scattered.dir, record.normal) > 0);
	}
	else if(record.material.type == MATERIAL_DIELECTRICS)
	{
		attenuation = 1.0.xxx;

		float3 outwardNormal = 0.0.xxx;
		float niOverNt = 0;
		float cosine = 0;
		if( 0 < dot(ray.dir, record.normal))
		{
			outwardNormal = -record.normal;
			niOverNt = record.material.refIndex;
			cosine = record.material.refIndex * dot(ray.dir, record.normal) / length(ray.dir);
		}
		else
		{
			outwardNormal = record.normal;
			niOverNt = 1.0 / record.material.refIndex;
			cosine = - dot(ray.dir, record.normal) / length(ray.dir);
		}

		float3 refracted = 0.0.xxx;
		float3 reflected = Reflect(normalize(ray.dir), record.normal);
		
#if MODE<11
		if(Refract(ray.dir, outwardNormal, niOverNt, refracted))
		{
			scattered.pos = record.pos;
			scattered.dir = refracted;
		}
		else
		{
			scattered.pos = record.pos;
			scattered.dir = reflected;
		}
#else
		float reflectProb;
		if(Refract(ray.dir, outwardNormal, niOverNt, refracted))
		{
			scattered.pos = record.pos;
			scattered.dir = refracted;
			reflectProb = Schlick(cosine, record.material.refIndex);
		}
		else
		{
			scattered.pos = record.pos;
			scattered.dir = reflected;
			reflectProb = 1;
		}

		if(RandomInUnitSphere(seed).x < reflectProb)
		{
			scattered.pos = record.pos;
			scattered.dir = reflected;
		}
		else
		{
			scattered.pos = record.pos;
			scattered.dir = refracted;
		}
#endif

		return true;
	}

	return false;
}

float3 CalcColor3(Ray ray, HittableList inworld, float2 uv)
{
	const int maxBounds = 50;
	float3 scale = 1.0.xxx;
	for(int i=0; i<maxBounds; ++i)
	{
		HitRecord record = (HitRecord)0;
		if(inworld.Hit(ray, float2(0.001,FLT_MAX), record))
		{
			float3 attenuation = 1.0.xxx;
			float2 uvSeed = uv+float2(0.323,0.421)*(float)i;
			Ray newRay;
			if(MaterialScatter(ray, record, attenuation, newRay, uvSeed))
			{
				ray = newRay;
				scale *= attenuation;
			}
		}
		else
		{
			return scale * RayToColor(ray);
		}
	}

	return 0.0.xxx;
}

groupshared HittableList s_world;

#if MODE < 8
[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(
	uint3 groupId          : SV_GroupID,
	uint3 groupThreadId    : SV_GroupThreadID,
	uint3 dispatchThreadID : SV_DispatchThreadID,
	uint  groupIndex       : SV_GroupIndex)
{
	uint2 index = dispatchThreadID.xy;

	float2 uv = ((float2)index.xy + float2(0.5,0.5)) / float2(width, height);
	uv.y = 1-uv.y;
	float u = uv.x;
	float v = uv.y;

#if MODE==0
	outputTexture[index] = float4(uv,0,1);
	return;
#endif

	Camera cam;
	cam.lowerLeftCorner = float3(-2,-1,-1);
	cam.horizontal = float3(4,0,0);
	cam.vertical = float3(0,2,0);
	cam.origin = 0.0.xxx;

	Ray ray = cam.GetRay(uv);

	float3 backColor = RayToColor(ray);

	float3 color = backColor;
	float3 sphereCenter = float3(0,0,-1);

	Sphere sphere0 = (Sphere)0;
	sphere0.center = float3(0,0,-1);
	sphere0.radius = 0.5;
	Sphere sphere1 = (Sphere)0;
	sphere1.center = float3(0,-100.5,-1);
	sphere1.radius = 100;

#if MODE==2
	if(HitSphere(sphere0, ray))
	{
		color = float3(1,0,0);
	}
#elif MODE==3
	float t = HitSphere2(sphere0, ray);
	if(0<t)
	{
		float3 N = normalize(ray.PointAt(t) - sphereCenter);
		color = 0.5 * (N + 1.0.xxx);
	}
#elif MODE>=4
	HittableList world = (HittableList)0;
	world.count = 2;
	world.list[0].type = HITTABLE_TYPE_SPHERE;
	world.list[0].sphere = sphere0;
	world.list[1].type = HITTABLE_TYPE_SPHERE;
	world.list[1].sphere = sphere1;

#if MODE==4
	HitRecord record = (HitRecord)0;
	if(world.Hit(ray, float2(0,FLT_MAX), record))
	{
		color = 0.5 * (record.normal + 1.0.xxx);
	}
#elif MODE>=5
	color = 0.0.xxx;
	
	float2 baseUv = uv + (0.5/SAMPLE_COUNT - 0.5).xx / float2(width, height);
	for(int x=0; x<SAMPLE_COUNT; ++x)
	{
		for(int y=0; y<SAMPLE_COUNT; ++y)
		{
			float2 newUv = baseUv + (1.0/SAMPLE_COUNT) * float2(x,y) / float2(width, height);
				
			Ray newRay = cam.GetRay(newUv);
#if MODE==5
			color += CalcColor(newRay, world, newUv);
#elif MODE>=6
			color += CalcColor2(newRay, world, newUv);
#endif
		}
	}
	color /= SAMPLE_COUNT*SAMPLE_COUNT;

#if MODE==7
	color = sqrt(color);
#endif

#endif

#endif

	outputTexture[index] = float4(color,1);
}

#else

[numthreads(THREAD_X, THREAD_Y, 1)]
void CSMain(
	uint3 groupId          : SV_GroupID,
	uint3 groupThreadId    : SV_GroupThreadID,
	uint3 dispatchThreadID : SV_DispatchThreadID,
	uint  groupIndex       : SV_GroupIndex)
{
	uint2 index = dispatchThreadID.xy;

	float2 uv = ((float2)index.xy + float2(0.5,0.5)) / float2(width, height);
	uv.y = 1-uv.y;
	float u = uv.x;
	float v = uv.y;

	Camera cam = (Camera)0;
	cam.lowerLeftCorner = float3(-2,-1,-1);
	cam.horizontal = float3(4,0,0);
	cam.vertical = float3(0,2,0);
	cam.origin = 0.0.xxx;

#if MODE==12
	cam.Setup(float3(-2,2,1), float3(0,0,-1), float3(0,1,0), 90, (float)width/(float)height);
#elif MODE==13	
	float3 lookFrom = float3(3,3,2);
	float3 lookAt   = float3(0,0,-1);
	float focus = length(lookFrom - lookAt);
	float apature = 2;
	cam.Setup(lookFrom, lookAt, float3(0,1,0), 20, (float)width/(float)height, apature, focus);
#elif MODE>=14
	float3 lookFrom = float3(4,2,4);
	float3 lookAt   = float3(0,0,-1);
	float focus = length(lookFrom - lookAt);
	float apature = 0.02;
	cam.Setup(lookFrom, lookAt, float3(0,1,0), 60, (float)width/(float)height, apature, focus);
#endif

	Ray ray = cam.GetRay(uv);

	float3 backColor = RayToColor(ray);

	float3 color = backColor;
	float3 sphereCenter = float3(0,0,-1);

	if(groupIndex == 0)
	{
	#if MODE>=14
		s_world = (HittableList)0;
		s_world.count = 0;
		s_world.list[0].type = HITTABLE_TYPE_SPHERE;
		s_world.list[0].sphere.center = float3(0, -1000, 0);
		s_world.list[0].sphere.radius = 1000;
		s_world.list[0].sphere.material.type = MATERIAL_LAMBERT;
		s_world.list[0].sphere.material.albedo = 0.5.xxx;
		s_world.count++;
		const int placeCount = PLACE_COUNT;
		for(int a=-placeCount; a<placeCount; a++)
		{
			for(int b=-placeCount; b<placeCount; b++)
			{
				float3 random3 = RandomInUnitSphere(float2(a,b)+(float2)placeCount.xx);
				float3 center = float3(a+0.9*random3.x, 0.2, b+0.9*random3.y);
				if(length(center - float3(4,0.2,0)) > 0.9)
				{
					float chooseMat = random3.z;
					s_world.list[s_world.count].type = HITTABLE_TYPE_SPHERE;
					s_world.list[s_world.count].sphere.center = center;
					s_world.list[s_world.count].sphere.radius = 0.2;
					if(chooseMat < 0.8)
					{
						s_world.list[s_world.count].sphere.material.type = MATERIAL_LAMBERT;
						s_world.list[s_world.count].sphere.material.albedo = RandomInUnitSphere(random3.xy)*RandomInUnitSphere(random3.zx+0.5.xx);
					}
					else if(chooseMat < 0.95)
					{
						s_world.list[s_world.count].sphere.material.type = MATERIAL_METAL;
						s_world.list[s_world.count].sphere.material.albedo = 0.5*(1 + RandomInUnitSphere(random3.xy));
						s_world.list[s_world.count].sphere.material.fuzz = 0.5 * RandomInUnitSphere(random3.zx).x;
					}
					else
					{
						s_world.list[s_world.count].sphere.material.type = MATERIAL_DIELECTRICS;
						s_world.list[s_world.count].sphere.material.albedo = 0.5*(1 + RandomInUnitSphere(random3.xy));
						s_world.list[s_world.count].sphere.material.refIndex = 1.5;
					}
					s_world.count++;
				}
			}
		}
		s_world.list[s_world.count].type = HITTABLE_TYPE_SPHERE;
		s_world.list[s_world.count].sphere.center = float3(0, 1, 0);
		s_world.list[s_world.count].sphere.radius = 1;
		s_world.list[s_world.count].sphere.material.type = MATERIAL_DIELECTRICS;
		s_world.list[s_world.count].sphere.material.refIndex = 1.5;
		s_world.count++;

		s_world.list[s_world.count].type = HITTABLE_TYPE_SPHERE;
		s_world.list[s_world.count].sphere.center = float3(-4, 1, 0);
		s_world.list[s_world.count].sphere.radius = 1;
		s_world.list[s_world.count].sphere.material.type = MATERIAL_LAMBERT;
		s_world.list[s_world.count].sphere.material.albedo = float3(0.4,0.2,0.1);
		s_world.count++;

		s_world.list[s_world.count].type = HITTABLE_TYPE_SPHERE;
		s_world.list[s_world.count].sphere.center = float3(4, 1, 0);
		s_world.list[s_world.count].sphere.radius = 1;
		s_world.list[s_world.count].sphere.material.type = MATERIAL_METAL;
		s_world.list[s_world.count].sphere.material.albedo = float3(0.7,0.6,0.5);
		s_world.list[s_world.count].sphere.material.fuzz = 0.5;
		s_world.count++;

	#else
		Sphere sphere0 = (Sphere)0;
		sphere0.center = float3(0,0,-1);
		sphere0.radius = 0.5;
		sphere0.material.type = MATERIAL_LAMBERT;
		sphere0.material.albedo = float3(0.8, 0.3, 0.3);
		Sphere sphere1 = (Sphere)0;
		sphere1.center = float3(0,-100.5,-1);
		sphere1.radius = 100;
		sphere1.material.type = MATERIAL_LAMBERT;
		sphere1.material.albedo = float3(0.8, 0.8, 0.0);
		Sphere sphere2 = (Sphere)0;
		sphere2.center = float3(1,-0,-1);
		sphere2.radius = 0.5;
		sphere2.material.type = MATERIAL_METAL;
		sphere2.material.albedo = float3(0.8, 0.6, 0.2);
		sphere2.material.fuzz = 1.0;
		Sphere sphere3 = (Sphere)0;
		sphere3.center = float3(-1,-0,-1);
		sphere3.radius = 0.5;
	#if MODE>=10
		sphere3.material.type = MATERIAL_DIELECTRICS;
		sphere3.material.albedo = float3(0.8, 0.8, 0.8);
		sphere3.material.refIndex = 1.5;
	#else
		sphere3.material.type = MATERIAL_METAL;
		sphere3.material.albedo = float3(0.8, 0.8, 0.8);
		sphere3.material.fuzz = 0.3;
	#endif

		s_world = (HittableList)0;
		s_world.count = 4;
		s_world.list[0].type = HITTABLE_TYPE_SPHERE;
		s_world.list[0].sphere = sphere0;
		s_world.list[1].type = HITTABLE_TYPE_SPHERE;
		s_world.list[1].sphere = sphere1;
		s_world.list[2].type = HITTABLE_TYPE_SPHERE;
		s_world.list[2].sphere = sphere2;
		s_world.list[3].type = HITTABLE_TYPE_SPHERE;
		s_world.list[3].sphere = sphere3;
	
	#if MODE>=11
		s_world.count = 5;
		Sphere sphere4 = (Sphere)0;
		sphere4.center = float3(-1,-0,-1);
		sphere4.radius = -0.45;
		sphere4.material.type = MATERIAL_DIELECTRICS;
		sphere4.material.albedo = float3(0.8, 0.8, 0.8);
		sphere4.material.refIndex = 1.5;
		s_world.list[4].type = HITTABLE_TYPE_SPHERE;
		s_world.list[4].sphere = sphere4;
	#endif

	#endif
	}
	GroupMemoryBarrierWithGroupSync();

	if(any(uint2(width, height) <= index))
	{
		// out of pixels.
		return;
	}

	color = 0.0.xxx;
	
	float2 baseUv = uv + (0.5/SAMPLE_COUNT - 0.5).xx / float2(width, height);
	for(int x=0; x<SAMPLE_COUNT; ++x)
	{
		for(int y=0; y<SAMPLE_COUNT; ++y)
		{
			float2 newUv = baseUv + (1.0/SAMPLE_COUNT) * float2(x,y) / float2(width, height);
				
			Ray newRay = cam.GetRay(newUv);
			color += CalcColor3(newRay, s_world, newUv);
		}
	}
	color /= SAMPLE_COUNT*SAMPLE_COUNT;

	color = sqrt(color);

	outputTexture[index] = float4(color,1);
}

#endif
