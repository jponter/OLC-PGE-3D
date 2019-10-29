#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <strstream>
#include <fstream>

using namespace std;

struct vec3d
{
	float x, y, z;
};

struct triangle
{
	vec3d p[3];
	int luminance = 0;

};

struct mesh
{
	vector<triangle> tris;

	bool LoadFromObjectFile(string sFilename)
	{
		ifstream f(sFilename);
		if (!f.is_open())
			return false;

		// Local cache of verts
		vector<vec3d> verts;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			strstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				vec3d v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}

			if (line[0] == 'f')
			{
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}

		return true;
	}


};

struct mat4x4
{
	float m[4][4] = { 0 };
};




// Override base class with your custom functionality
class olcEngine3D : public olc::PixelGameEngine
{

private:
	mesh meshCube;
	mat4x4 matProj;

	vec3d vCamera;

	float fTheta;


	void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m)
	{
		o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
		o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
		o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
		float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

		if (w != 0.0f)
		{
			o.x /= w; o.y /= w; o.z /= w;
		}
	}


public:
	olcEngine3D()
	{
		sAppName = "olcEngine 3D - Part 1";
	}
public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		
		////make a mesh cube
		//meshCube.tris = {

		//	// SOUTH
		//	{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		//	{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		//	// EAST                                                      
		//	{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		//	{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		//	// NORTH                                                     
		//	{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		//	{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

		//	// WEST                                                      
		//	{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		//	{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

		//	// TOP                                                       
		//	{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		//	{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		//	// BOTTOM                                                    
		//	{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		//	{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		//};

		meshCube.LoadFromObjectFile("teapot.obj");

		// Projection Matrix
		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
		float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

		matProj.m[0][0] = fAspectRatio * fFovRad;
		matProj.m[1][1] = fFovRad;
		matProj.m[2][2] = fFar / (fFar - fNear);
		matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;


		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame, draws random coloured pixels
		Clear(olc::BLACK);

		// Set up rotation matrices
		mat4x4 matRotZ, matRotX;
		fTheta += 1.0f * fElapsedTime;

		// Rotation Z
		matRotZ.m[0][0] = cosf(fTheta);
		matRotZ.m[0][1] = sinf(fTheta);
		matRotZ.m[1][0] = -sinf(fTheta);
		matRotZ.m[1][1] = cosf(fTheta);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;

		// Rotation X
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fTheta * 0.5f);
		matRotX.m[1][2] = sinf(fTheta * 0.5f);
		matRotX.m[2][1] = -sinf(fTheta * 0.5f);
		matRotX.m[2][2] = cosf(fTheta * 0.5f);
		matRotX.m[3][3] = 1;



		vector<triangle> vecTrianglesToRaster;


		// draw triangles
		for (auto tri : meshCube.tris)
		{
			triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

			// Rotate in Z-Axis
			MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
			MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
			MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

			// Rotate in X-Axis
			MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

			//translate
			triTranslated = triRotatedZX;
			triTranslated.p[0].z = triRotatedZX.p[0].z + 6.0f;
			triTranslated.p[1].z = triRotatedZX.p[1].z + 6.0f;
			triTranslated.p[2].z = triRotatedZX.p[2].z + 6.0f;


			//calculate the cross product
			vec3d normal, line1, line2;

			line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
			line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
			line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

			line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
			line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
			line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

			normal.x = line1.y * line2.z - line1.z * line2.y;
			normal.y = line1.z * line2.x - line1.x * line2.z;
			normal.z = line1.x * line2.y - line1.y * line2.x;

			//normalse
			float l = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
			normal.x /= l; normal.y /= l; normal.z /= l;


			//project
			//if (normal.z < 0)
			if (normal.x * (triTranslated.p[0].x - vCamera.x) + 
				normal.y * (triTranslated.p[0].y - vCamera.y) +
				normal.z * (triTranslated.p[0].z - vCamera.z) < 0.0)
			{
				//illumintation
				vec3d light_direction = { 0.0f, 0.0f, - 1.0f };
				float l = sqrt(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
				light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

				

				// How similar is normal to light direction
				float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;
				//cout << dp << endl;
				//dp has a value between 0 and 1 to represent how lit the face is to the light direction

				//translate the luminance to greyscale in range 0-255
				float luminance = dp * 255.0f;

				MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
				MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
				MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

				//scale
				triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
				triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
				triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

				triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
				triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
				triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
				triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
				triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
				triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

				triProjected.luminance = (unsigned int)luminance % 255 ;
				//cout << "Luminance " << triProjected.luminance << endl;

				vecTrianglesToRaster.push_back(triProjected);

				//draw

				/*FillTriangle(triProjected.p[0].x, triProjected.p[0].y,
					triProjected.p[1].x, triProjected.p[1].y,
					triProjected.p[2].x, triProjected.p[2].y,
					olc::Pixel(int(luminance),int(luminance),int(luminance)));*/

				//debug show the wireframe
				/*DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
					triProjected.p[1].x, triProjected.p[1].y,
					triProjected.p[2].x, triProjected.p[2].y,olc::BLACK);*/


			}
		}

		// Sort triangles from back to front
		sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2)
		{
			float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
			float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
			return z1 > z2;
		});

		for (auto& triProjected : vecTrianglesToRaster)
		{
			FillTriangle(triProjected.p[0].x, triProjected.p[0].y,
				triProjected.p[1].x, triProjected.p[1].y,
				triProjected.p[2].x, triProjected.p[2].y,
				olc::Pixel(triProjected.luminance, triProjected.luminance, triProjected.luminance));
		}



		return true;
	}
};
int main()
{
	olcEngine3D demo;
	if (demo.Construct(1024, 768, 1, 1))
		demo.Start();
	return 0;
}