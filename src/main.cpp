#include <iostream>
#include <memory>
#include <vector>

#include "../include/raymath.hpp"
#include "../include/Camera.hpp"
#include "../include/Image.hpp"
#include "../include/Ray.hpp"

#include "../include/Sphere.hpp"
#include "../include/Plane.hpp"

Vector3 renderRay(const Ray &r, const std::vector<RenderObject*> &scene, const Image &world);

int main()
{
	const int width = 1024;
	const int height = 512;

	Image img(width, height, 3);
	int* sampled = new int[width * height * 3];

	Image world("solitude_interior_2k.hdr");

	double aspectRatio = (double)width / height;
	Camera cam(Vector3(0, 4, 5), Vector3(0, 0, -1), aspectRatio, 1.5);
	cam.lookAt(Vector3(0, 0, -5));

	Sphere s(Vector3(1, 0.25, -5), 2);
	Sphere s2(Vector3(-3, -2.5, -7), 2);

	Plane p(Vector3(0, -5, 0), Vector3(0, 1, 0));

	std::vector<RenderObject*> renderObjects;
	renderObjects.push_back(&s);
	renderObjects.push_back(&s2);
	//renderObjects.push_back(&p);

	const int aliasSamples = 10;

	for (int s = 0; s < aliasSamples; s++)
	{
		size_t index = 0;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				Ray r = cam.generateRay(((float)x+getRandom())/width, ((float)y+getRandom())/height);
				Vector3 color = renderRay(r, renderObjects, world);
				sampled[index++] += (int)color.x;
				sampled[index++] += (int)color.y;
				sampled[index++] += (int)color.z;
			}
		}
	}

	for (int i = 0; i < width * height * 3; i++)
	{
		img.buf[i] = (int)((double)sampled[i] / aliasSamples);
	}

	img.save("out.png");
	
	return 0;
}

Vector3 renderRay(const Ray &r, const std::vector<RenderObject*> &scene, const Image &world)
{
	double t = INFINITY;
	size_t hitIndex;

	for (size_t i = 0; i < scene.size(); i++)
	{
		double dist = scene[i]->getIntersection(r);
		if (dist < t && dist > 0)
		{
			t = dist;
			hitIndex = i;
		}
	}

	if (t == INFINITY)
	{
		Vector3 d = -r.direction.normalize();
		double u = 0.5 + std::atan2(d.x, d.z) / (2 * 3.14159);
		double v = 0.5 + std::asin(d.y) / 3.14159;
		Vector3 skyColor = world.getPixel(u, v);
		return skyColor;
	}
	else
	{
		Vector3 hit = r.origin + r.direction * t;
		Vector3 normal = scene[hitIndex]->getNormal(hit);
		Vector3 shiftedNormal = (Vector3(1, 1, 1) + normal) * 0.5 * 255;

		if (hitIndex == 1)
		{
			Vector3 out = r.direction - normal * 2 * dot(normal, r.direction);

			Ray newRay(hit, out);
			Vector3 reflection = shiftedNormal * 0.15 + renderRay(newRay, scene, world) * 0.85;
			return reflection;
		}

		return shiftedNormal;
	}
}
