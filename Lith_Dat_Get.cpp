// Lith_Dat_Get.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
using namespace std;

typedef unsigned char BYTE;

#define ver "Lith Dat Get 85\nBy: URA\nAlpha Version: 02/26/2012";

short int error = 0;
bool striper = false;
bool b69 = false;
bool debug = false;
bool lightmaps = true;
bool flip = false;
const char *ProjectPath = "C:\\Games\\Game\\";

char buffer[4];
char bs[4];
char test[1];
char b2[2];
int seek = 0x18;
int len = 4;

bool ms3d = false;

unsigned int Sec = 0;

char Texture[256];
string TList[65535];
string TList1[256];
unsigned int Texs = 0;
unsigned long int Ti[65535];
int Tic = 0;

unsigned int Verts = 0;
float x[65535];
float y[65535];
float z[65535];
float u[65535];
float v[65535];
float u1[65535];
float v1[65535];
float nx[65535];
float ny[65535];
float nz[65535];

unsigned int Face = 0;
unsigned int f0[65535];
unsigned int f1[65535];
unsigned int f2[65535];
unsigned int f3[65535];

unsigned int SkyS = 0;

unsigned int OccS = 0;

unsigned int LGS = 0;

////////////////////////////////////////////////////////
ifstream myFile;// ("rs.dat", ios::in | ios::binary);
istream& seekg(streampos pos);

ofstream outfile;//("example.bin", ios::out | ios::binary);
ofstream mtlfile;//("example.bin", ios::out | ios::binary);

const char *filePath = "rs.dat";
const char *out = "noname.dat";
BYTE *fileBuf;			// Pointer to our buffered data
//char *fileBuf;			// Pointer to our buffered data
FILE *file = NULL;		// File pointer
////////////////////////////////////////////////////////

long getFileSize(FILE *file)
{
	long lCurPos, lEndPos;
	lCurPos = ftell(file);
	fseek(file, 0, 2);
	lEndPos = ftell(file);
	fseek(file, lCurPos, 0);
	return lEndPos;
}

int Sections(int Geo, int index)
{
	myFile.seekg(Geo);
	myFile.read(buffer, 4);
	Sec = *reinterpret_cast<unsigned int*>(buffer);
	Geo += 4;

	if(debug)
		cout << "\nSections: " << Sec << " @ " << hex << Geo-4 << endl << "--------------------------" << endl;

	return Geo;
}

int Textures(int Geo, int index)
{
	// Geo + 1c (texture index 1byte?) | (texture name length 4bytes) + 1 (padding?) + length (ASCII/text) + 12 & repeat
	// u16 +T0, u16 +T1, u8 Shader, u32 texture faces?, u16 +TextureEffect, u32 width, u32 height, u32 data size, (data that we skip)
	Geo += 0x18;
	myFile.seekg(Geo);
	myFile.read(buffer, 4);
	Texs = *reinterpret_cast<unsigned int*>(buffer);
	Geo += 4;

	if (Texs == 0) // hack to skip ahead
	{
		if(debug)
			cout << "Textures (0): " << Geo-4 << endl;

		return Geo;
	}

	if(debug)
		cout << "Textures (" << Texs << ") " << Geo-4 << endl;

	//cout << Texs << " " << Geo << endl;
	unsigned short int Length = 0;
	unsigned int Table = 0;
	unsigned long int TiC = 0;
	Tic = 0;
	//string sTList[256], sTList1[256];
	for (int i = 0; i < Texs; i++)
	{
		// Texture 0
		myFile.seekg(Geo);
		myFile.read(b2, 2);
		Length = *reinterpret_cast<unsigned short int*>(b2);
		Geo += 2;
		if (Length > 0)
		{
			myFile.seekg(Geo);
			myFile.read(Texture, Length);

			//if (!debug)
			TList[i] = string(Texture);
			for (int cc = 0; cc < Length; cc++)
				Texture[cc] = NULL;

			Geo += Length;

			if(debug)
				cout << "\tTexture0: " << TList[i] << " Geo: " << Geo - Length << " | " << Length << " | " << Geo << endl;
		}

		// Texture 1
		myFile.seekg(Geo);
		myFile.read(b2, 2);
		Length = *reinterpret_cast<unsigned short int*>(b2);
		Geo += 2;
		if (Length > 0)
		{
			myFile.seekg(Geo);
			myFile.read(Texture, Length);

			//if (!debug)
			TList1[i] = string(Texture);
			for (int cc = 0; cc < Length; cc++)
				Texture[cc] = NULL;

			Geo += Length;
			
			if(debug)
				cout << "\t\tTexture1: " << TList1[i] << " Geo: " << Geo - Length << " | " << Length << " | " << Geo << endl;
		}

		// skip shader
		Geo += 1;

		// Ti count?
		myFile.seekg(Geo);
		myFile.read(buffer, 4);
		Ti[i] = *reinterpret_cast<unsigned long int*>(buffer);
		Tic += Ti[i];
		Geo += 4;

		// Texture effect
		myFile.seekg(Geo);
		myFile.read(b2, 2);
		Length = *reinterpret_cast<unsigned short int*>(b2);
		Geo += 2;
		if (Length > 0)
		{
			// Skip this data
			Geo += Length;
		}

		// skip lightmap width
		Geo += 4;
		// skip lightmap height
		Geo += 4;
		// lightmap data size (data that we skip)
		myFile.seekg(Geo);
		myFile.read(buffer, 4);
		Table = *reinterpret_cast<unsigned int*>(buffer);
		Geo += Table + 4;

		if(debug)
			cout << "\t\t\tData skip: " << Geo << endl << "--------------------------\n" << endl;

		//cout << TList[i] << " " << Length << endl;
	}
	//cout << TiC << endl;
	/*for (int i = 0; i < Texs; i++)
	{
		int ii = 0;
		for ( ; ii < Ti[i]; ii++)
		{
			TList[Tic] = sTList[i];
			//TList1[ii] = sTList1[i];
			//cout << sTList[i] << " i: " << i << endl;
			Tic += 1;
		}
		//cout << "\t\t" << Tic << " " << ii << " " << sTList[i] << " " << Ti[i] << endl;
		//sTList[i].clear();
	}*/
	return Geo;//0;
}

int VertData(int Geo, int index)
{
	// vertex index (4bytes?) + 3 (padding?) | (group block size 2c) c (verts) | 8 (UV) + 8 (?) + 4 (vert lighting RGBA?) | c (normals)
	/*
	vertex
	vector pos, f u0, fv0, fu1, fv1, u8 u8 u8 u8 color, vector normal

	build 69 extra vertex data
	vector Tangent, vector Binormal
	*/

	unsigned long dGeo = Geo;

	unsigned short int Length = 0;
	myFile.seekg(Geo);//(GetOS(Geo));
	myFile.read(buffer, 4);
	Verts = *reinterpret_cast<unsigned int*>(buffer);
	Geo += 4;

	if(debug)
		cout << "Vert: " << Verts << " " << Geo-4 << endl;

	if (Verts == 0) // hack to skip ahead
	{
		return Geo;
	}

	for (int i = 0; i < Verts; i++)
	{
		myFile.seekg(Geo);
		myFile.read(buffer, 4);
		x[i] = *reinterpret_cast<float*>(buffer);
		myFile.seekg(Geo += 4);
		myFile.read(buffer, 4);
		y[i] = *reinterpret_cast<float*>(buffer);
		myFile.seekg(Geo += 4);
		myFile.read(buffer, 4);
		z[i] = *reinterpret_cast<float*>(buffer);
		myFile.seekg(Geo += 4);
		myFile.read(buffer, 4);
		u[i] = *reinterpret_cast<float*>(buffer);
		myFile.seekg(Geo += 4);
		myFile.read(buffer, 4);
		v[i] = *reinterpret_cast<float*>(buffer);

		myFile.seekg(Geo += 4);
		myFile.read(buffer, 4);
		u1[i] = *reinterpret_cast<float*>(buffer);
		myFile.seekg(Geo += 4);
		myFile.read(buffer, 4);
		v1[i] = *reinterpret_cast<float*>(buffer);

		myFile.seekg(Geo += 8);
		myFile.read(buffer, 4);
		nx[i] = *reinterpret_cast<float*>(buffer);
		myFile.seekg(Geo += 4);
		myFile.read(buffer, 4);
		ny[i] = *reinterpret_cast<float*>(buffer);
		myFile.seekg(Geo += 4);
		myFile.read(buffer, 4);
		nz[i] = *reinterpret_cast<float*>(buffer);
		Geo += 4;

		for (long ii = dGeo; ii < Geo && striper; ii++)
		{
			outfile.put(fileBuf[ii]);
			//cout << i << "\t" ii << " - ";
		}

		// 69'ers
		if (b69)
			Geo += 0x18;

		dGeo = Geo;
		
		//printf("%d %f %f %f\n%f %f\n%f %f %f\n\n", i, x[i], y[i], z[i], u[i], v[i], nx[i], ny[i], nz[i]);
	}
	//cout << Geo << endl;

	if(debug)
		cout << "\t" << Geo << "\n--------------------------\n" << endl;

	return Geo;
}

int Faces(int Geo, int index)
{
	// faces index (2bytes + 2padding?) | (group block size 10) | f0 (4) | f1 (4) | f2 (4) | f3 (4)
	//cout << Geo << endl;
	myFile.seekg(Geo);
	myFile.read(buffer, 4);
	Face = *reinterpret_cast<unsigned int*>(buffer);
	//cout << Face << endl;
	Geo += 4;

	if(debug)
		cout << "Faces: " << Face << " " << Geo-4 << endl;

	if (Face == 0) // hack to skip ahead
	{
		return Geo;
	}

	for (int i = 0; i < Face; i++)
	{
		myFile.seekg(Geo);
		myFile.read(buffer, 4);
		f0/*[index]*/[i] = *reinterpret_cast<unsigned int*>(buffer);
		myFile.seekg(Geo += 4);
		myFile.read(buffer, 4);
		f1/*[index]*/[i] = *reinterpret_cast<unsigned int*>(buffer);
		myFile.seekg(Geo += 4);
		myFile.read(buffer, 4);
		f2/*[index]*/[i] = *reinterpret_cast<unsigned int*>(buffer);
		myFile.seekg(Geo += 4);
		myFile.read(buffer, 4);
		f3/*[index]*/[i] = *reinterpret_cast<unsigned int*>(buffer);
		Geo += 4;

		if(debug)
			cout << Face << " " << Geo << " | " << f0[i] << " " << f1[i] << " " << f2[i] << " " << f3[i] << endl;
	}

		//cout << Face << " " << Geo << " | " << f0[0] << endl;
	if(debug)
		cout << "\t" << Geo << "\n--------------------------\n" << endl;

	return Geo;
}

int Sky(int Geo, int index)
{
	// size (uint32), size (uint8), ...loop... vector..., normals (vector?), dist (?)

	myFile.seekg(Geo);
	myFile.read(buffer, 4);
	SkyS = *reinterpret_cast<unsigned int*>(buffer);
	Geo += 4;

	if(debug)
		cout << "Sky: " << SkyS << " " << Geo-4 << endl;

	//cout << "   SkySize: " << SkyS << endl;

	if (SkyS == 0) // hack to skip ahead
	{
		if(debug)
			cout << "\t" << Geo << "\n--------------------------\n" << endl;

		return Geo;
	}
	/*else
	{
		cout << "SkyS: " << SkyS << " | Geo: " << std::hex << Geo -4 << endl;
		error = 1;
	}*/

	for (int i = 0; i < SkyS; i++)
	{
		Geo += 0x41;
	}

	if(debug)
		cout << "\t" << Geo << "\n--------------------------\n" << endl;

	return Geo;
}

int Occ(int Geo, int index)
{
	// size (uint32), size (uint8), ...loop... vector..., normals, dist, Name
	// u32 size, |for loop - u8 size, |for loop - Vector (u8 size * vector)|, vector normal, float dist, u32 name|

	myFile.seekg(Geo);
	myFile.read(buffer, 4);
	OccS = *reinterpret_cast<unsigned int*>(buffer);
	Geo += 4;

	if(debug)
		cout << "Occ: " << OccS << " " << Geo-4 << endl;

	if (OccS == 0) // hack to skip ahead
	{
		return Geo;
	}

	for (int i = 0; i < OccS; i++)
	{
		myFile.seekg(Geo);
		myFile.read(test, 1);
		unsigned short int Temp = *reinterpret_cast<unsigned short int*>(test);
		//cout << "\t\tTemp: " << Temp << endl;
		Temp *= 12;
		//cout << "\t\tTemp *12: " << Temp << endl;
		Geo += Temp + 1;
		Geo += 12;
		Geo += 4;
		Geo += 4;
	}

	/*else
	{
		cout << "OccS: " << OccS << " | Geo: " << Geo -4 << endl;
		error = 1;
	}*/

	if(debug)
		cout << "\t" << Geo << "\n--------------------------\n" << endl;

	return Geo;
}

int LightGroup(int Geo, int index)
{
	// size (uint32), ...loop..., vector...
	// light groups
	// u32 count, u16 name, vector, u32 size, u8 * size, u32 size, u32 * size

	myFile.seekg(Geo);
	myFile.read(buffer, 4);
	LGS = *reinterpret_cast<unsigned int*>(buffer);
	Geo += 4;

	if(debug)
		cout << "LGs: " << LGS << " " << Geo-4 << endl;

	if (LGS == 0) // hack to skip ahead
	{
		return Geo;
	}

	for (int i = 0; i < LGS; i++)
	{
		myFile.seekg(Geo);
		myFile.read(b2, 2);
		unsigned short int Temp = *reinterpret_cast<unsigned short int*>(b2);
		if(debug)cout << "--Temp-- " << Temp << endl;
		Geo += 2 + Temp + 12;

		myFile.seekg(Geo);
		myFile.read(buffer, 4);
		unsigned long int Temp32 = *reinterpret_cast<unsigned long int*>(buffer);
		if(debug)cout << "--Temp32-- " << Temp32 << endl;
		Geo += 4 + Temp32;

		/*
		-- LightGroups --
		u32 blocks
			u32 sizes
			if u32 > 0
				u32 blocks
				u32, u32, u32, u32, u32 size (data skip)
			else skip forward
		*/
		myFile.seekg(Geo);
		myFile.read(buffer, 4);
		Temp32 = *reinterpret_cast<unsigned long int*>(buffer);
		if(debug)cout << "--Temp32-- " << Temp32 << endl;
		Geo += 4;

		for (int j = 0; Temp32 > j; j++) // blocks
		{
			myFile.seekg(Geo);
			myFile.read(buffer, 4);
			unsigned long int jj = *reinterpret_cast<unsigned long int*>(buffer);
			Geo += 4;
			if (jj > 0) // sizes
			{
				for (int xj = 0; jj > xj; xj++)
				{
					Geo += 16;
					myFile.seekg(Geo);
					myFile.read(buffer, 4);
					unsigned long int xx = *reinterpret_cast<unsigned long int*>(buffer);
					Geo += 4 + xx;
				}
			}
		}

		//Temp32 *= 4;
		
		if(debug)cout << "---Temp32-- " << Temp32 << endl;
		//Geo += 4 + Temp32;

		if(debug)cout << "----- " << Geo << endl;
	}
	/*else
	{
		cout << "LGS: " << LGS << " | Geo: " << Geo -4 << endl;
		error = 1;
	}*/

	if(debug)
		cout << "\t" << Geo << "\n--------------------------\n" << endl;

	return Geo;
}

int Children(int Geo, int index)
{
	// uint8 - flags
	// ...loop... uint32 - indices

	// don't know how to read this yet... so we skip += 9
	Geo += 9;

	if(debug)
		cout << "Ch " << Geo-9 << " " << Geo << "\n--------------------------\n" << endl;

	return Geo;
}

float /*Q_rsqrt*/Q( float number )
{
        long i;
        float x2, y;
        const float threehalfs = 1.5F;
 
        x2 = number * 0.5F;
        y  = number;
        i  = * ( long * ) &y;                       // evil floating point bit level hacking
        i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
        y  = * ( float * ) &i;
        y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//      y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
 
        return y;
}

float fX (float x, bool v)
{
	if (flip || v)
	{
		//cout << x << " | ";
		if (x<0)
		{
			//float tx = x;
			x = x - x - x;
			//cout << "x<0 " << x;
		}
		else if (x>0)
		{
			x = x - x - x;
			//cout << "\tx>0 " << x;
		}
		//cout << endl;
	}
	return x;
}

///////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	cout << ver;// << endl;
	int nArg;
	if (__argc > 1)
	{
		for( nArg = 1; nArg < __argc; nArg++ )
		{
			if( stricmp( __argv[nArg], "-in" ) == 0 )
			{
				filePath = __argv[nArg + 1];
				nArg++;
			}
			if( stricmp( __argv[nArg], "-out" ) == 0 )
			{
				out = __argv[nArg + 1];
				nArg++;
			}
			if( stricmp( __argv[nArg], "-pp" ) == 0 )
			{
				ProjectPath = __argv[nArg + 1];
				nArg++;
			}
			if( stricmp( __argv[nArg], "-69" ) == 0 )
			{
				b69 = true;
				//b69 = bool(__argv[nArg + 1]);
				//nArg++;
			}
			if( stricmp( __argv[nArg], "-debug" ) == 0 )
			{
				debug = true;
				//debug = bool(__argv[nArg + 1]);
				//nArg++;
			}
			if( stricmp( __argv[nArg], "-strip" ) == 0 )
			{
				striper = true;
				//striper = bool(__argv[nArg + 1]);
				//nArg++;
			}
			if( stricmp( __argv[nArg], "-ms" ) == 0 )
			{
				ms3d = true;
				//striper = bool(__argv[nArg + 1]);
				//nArg++;
			}
			if( stricmp( __argv[nArg], "-no" ) == 0 )
			{
				lightmaps = false;
			}
			if( stricmp( __argv[nArg], "-flip" ) == 0 )
			{
				flip = true;
			}
		}
	}
	else
	{
		cout << "\n-in (input file name)\n-out (output file)\n-69 (a fix for newer maps)\n-no (skip lightmaps)\n-flip\n-strip" << endl;
		/////////////////////////
		

		return 0;
	}

	if (striper) outfile.open(out, ios::out | ios::binary);
	else 
	{
		//outfile.open(/*out*/"test.obj"); mtlfile.open("test.mtl");
		if (!ms3d) 
			{
				//outfile.close(); 
				//mtlfile.close(); 
				char fn[64]; 
				sprintf(fn, "%s.obj", out); 
				outfile.open(fn);
				sprintf(fn, "%s.mtl", out);
				mtlfile.open(fn);
			}
	}

	// Open the file in binary mode using the "rb" format string
	// This also checks if the file exists and/or can be opened for reading correctly
	if ((file = fopen(filePath, "rb")) == NULL)
	{cout << "Could not open specified file" << endl;}
	/*else
		cout << "File opened successfully" << endl;*/

	// Get the size of the file in bytes
	long fileSize = getFileSize(file);

	// Allocate space in the buffer for the whole file
	fileBuf = new BYTE[fileSize];

	// Read the file in to the buffer
	fread(fileBuf, fileSize, 1, file);
	// Now that we have the entire file buffered, we can take a look at some binary infomation
	// Lets take a look in hexadecimal
	/*for (int i = 24; i < 28; i++)
	{
		printf("%x ", fileBuf[i]);
		/*if (myfile.is_open()) 
		{
			//myfile.write(fileBuf[i], 1);
			outfile.put(fileBuf[i]);
		}
	}

	//cin.get();
	delete[]fileBuf;
    fclose(file);   // Almost forgot this */

	myFile.open(filePath, ios::in | ios::binary);
	if (!myFile) 
	{
        // An error occurred!
        // myFile.gcount() returns the number of bytes read.
        // calling myFile.clear() will reset the stream state
        // so it is usable again.
		printf("\n\nerror!");
		return -1;
	}
	else
	{		
		//GetOS(seek);
		myFile.seekg(seek);//(GetOS(seek));
		myFile.read(buffer, len);
		len = 4; // reset len to 4

		long Geo = *reinterpret_cast<long*>(buffer);
		long dGeo;

		Geo = Sections(Geo, 1); // Read in the amount of sections

		if (ms3d) cout << "\/\/ MilkShape 3D ASCII" << endl
			<< endl << "Frames: 30" << endl
			<< "Frame: 1" << endl << endl
			<< "Meshes: " << Sec << endl;

		// for 69'er .dat scrubber
		// write out the all the data up to the RenderData block
		for (int i = 0; i < Geo && striper; i++)
		{
			outfile.put(fileBuf[i]);
		}

		unsigned int tVert = 0;
		unsigned int tFaces = 1;
		//int temp99 = 0;

		for (int i = 0; i < Sec /*&& !debug*/; i++)
		{
			if (ms3d) cout << "\"Section" << i << "\" 0 -1" << endl; // ms3d
			//int i = 0;
			//cout << "Text: " << Geo << endl;

			dGeo = Geo;
			Geo = Textures(Geo, i); // Texs - Tlist[]
			for (long ii = dGeo; ii < Geo && striper; ii++)
			{
				outfile.put(fileBuf[ii]);
			}

			//cout << "\tVert: " << Geo << endl;
			dGeo = Geo;
			Geo = VertData(Geo, i); // Verts - x y z u v nx ny nz[]
			tVert += Verts;
			//cout << tVert << endl;

			//cout << "\tFace: " << Geo << endl;
			dGeo = Geo;
			Geo = Faces(Geo, i);    // Face - f0 f1 f2 f3[]
			for (long ii = dGeo; ii < Geo && striper; ii++)
			{
				outfile.put(fileBuf[ii]);
			}

			//cout << "\tSky: " << Geo << endl;
			dGeo = Geo;
			Geo = Sky(Geo, i);
			for (long ii = dGeo; ii < Geo && striper; ii++)
			{
				outfile.put(fileBuf[ii]);
			}

			//cout << "\tOcc: " << Geo << endl;
			dGeo = Geo;
			Geo = Occ(Geo, i);
			for (long ii = dGeo; ii < Geo && striper; ii++)
			{
				outfile.put(fileBuf[ii]);
			}

			//cout << "\tLG: " << Geo << endl;
			dGeo = Geo;
			Geo = LightGroup(Geo, i);
			for (long ii = dGeo; ii < Geo && striper; ii++)
			{
				outfile.put(fileBuf[ii]);
			}

			//cout << "\tCh: " << Geo << endl;
			dGeo = Geo;
			Geo = Children(Geo, i);
			for (long ii = dGeo; ii < Geo && striper; ii++)
			{
				outfile.put(fileBuf[ii]);
			}

			//cout << Geo << endl;
			if (error == 1)
			{	return -1;	}
			//-------------------// ^- ints

			if (!ms3d) /*cout*/outfile << "## Lith_Dat_Get Alpha ##" << endl /*<< "g Section" << i << endl*/ << "mtllib " << out << ".mtl" << endl;
			if (Verts > 0)
			{
				//if (!ms3d) /*cout*/outfile << "## Lith_Dat_Get Alpha ##" << endl /*<< "g Section" << i << endl*/ << "mtllib " << out << i << ".mtl" << endl;
				// file -import -type "OBJ" -rpr "Map1" -options "mo=1" "//WVPC/c/cpp/Lith_Dat_Get/Debug/Map1.obj";
				//cout << "file -import -type \"OBJ\" -rpr \"" << out << i << "\" -options \"mo=1\" \""<< out << i << ".obj\";" << endl;

				if (!ms3d)
				for (int i = 0; i < Texs; i++)
				{
					//printf("%s\n", TList[i]);
					//temp99 += Ti[i];
					//for (int cc = 0; cc < Ti[i]; cc++)
					//char TTemp[512];
					//sprintf(TTemp, "newmtl %s\nKa 1.000 1.000 1.000\nKd 1.000 1.000 1.000\nKs 0.000 0.000 0.000\nd 1.0\nillum 2\nmap_Ka %s\n\n", TList[i], TList[i]);
					mtlfile << "newmtl " << TList[i] << endl << "Ka 1.000 1.000 1.000" << endl << "Kd 1.000 1.000 1.000" << endl << "Ks 0.000 0.000 0.000" << endl << "d 1.0" << endl << "illum 2" << endl << /*"map_Ka " << /*ProjectPath << TList[i] << endl <<*/ "map_Kd \"" << /*ProjectPath <<*/ TList[i] << "\"" << endl << endl;

					//cout << Tic << ": " << i << " | "<< TList[i] << " " << Ti[i] << endl;
					//TList[i].clear();
				}

				if (ms3d) cout << Verts << endl; // MS3D
				for (int i = 0; i < Verts; i++)
				{
					//printf("%f %f %f\n%f %f\n%f %f %f\n", x[i], y[i], z[i], u[i], v[i], nx[i], ny[i], nz[i]);
					if (ms3d) {printf("0 %f %f %f %f %f -1\n", x[i], y[i], z[i], u[i], v[i]);} // Milkshape ASCII
					else {/*printf("v %f %f %f\n", x[i], y[i], z[i]);*/ char TEMP[64]; sprintf(TEMP, "v %f %f %f\n", fX(x[i], false), y[i], z[i]); outfile << TEMP;}
				}
				if (ms3d) cout << Verts << endl; // MS3D
				for (int i = 0; i < Verts; i++)
				{
					if (ms3d) {printf("%f %f %f\n", nx[i], ny[i], nz[i]); }// ms3d
					else {char TEMP[64]; sprintf(TEMP, "vt %f %f\n", u[i], fX(v[i], true)); outfile << TEMP;} // obj]
				}

				if (!ms3d)
				{
					for (int i = 0; i < Verts; i++)
					{
						//printf("vn %f %f %f\n", nx[i], ny[i], nz[i]); // obj
						char Temp[64];
						sprintf(Temp, "vn %f %f %f\n", fX(nx[i], false), ny[i], nz[i]); // obj
						outfile << Temp;
					}
				}

				if (ms3d) {cout << Face << endl;} // MS3D
				int TiTemp = 0;
				int zTi = 0;
				for (int i = 0; i < Face; i++)
				{
					//printf("%d %d %d %d\n", f0[i], f1[i], f2[i], f3[i]);
					if (ms3d) {printf("0 %d %d %d %d %d %d 0\n", f0[i], f1[i], f2[i], f3[i], f3[i], f3[i]);} // MS3D
					//else {printf("f %d//%d %d//%d %d//%d\n", f0[i]+1, f3[i]+1, f1[i]+1, f3[i]+1, f2[i]+1, f3[i]+1);} // Obj
					else 
					{
						if (lightmaps)
						{
							if (TiTemp <= 0)
							{
								TiTemp = Ti[zTi];
								outfile << "g " << TList[zTi] << endl;
								outfile << "usemtl " << TList[zTi] << endl;
								zTi++;
								i--;
							}
							else
							{
								char TEMP[128]; 
								if (!flip){sprintf(TEMP, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", f0[i]+tFaces, f0[i]+tFaces, f0[i]+tFaces, f1[i]+tFaces, f1[i]+tFaces, f1[i]+tFaces, f2[i]+tFaces, f2[i]+tFaces, f2[i]+tFaces);}
								else{sprintf(TEMP, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", f2[i]+tFaces, f2[i]+tFaces, f2[i]+tFaces, f1[i]+tFaces, f1[i]+tFaces, f1[i]+tFaces, f0[i]+tFaces, f0[i]+tFaces, f0[i]+tFaces);}
								//sprintf(TEMP, "f %d %d %d\n", f0[i]+1, f1[i]+1, f2[i]+1); 
								outfile << TEMP;
								TiTemp--;
							}
						}
						else
						{
							if (TiTemp <= 0)
							{
								TiTemp = Ti[zTi];
								if (TList[zTi].compare(0, 14, "LightAnim_BASE") > 0)
								{
									outfile << "g " << TList[zTi] << endl;
									outfile << "usemtl " << TList[zTi] << endl;

									//cout << "g " << TList[zTi] << endl;
								}
								zTi++;
								i--;
							}
							else
							{
								if (TList[zTi -1].compare(0, 14, "LightAnim_BASE") > 0)
								{
									char TEMP[128]; 
									if (!flip){sprintf(TEMP, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", f0[i]+tFaces, f0[i]+tFaces, f0[i]+tFaces, f1[i]+tFaces, f1[i]+tFaces, f1[i]+tFaces, f2[i]+tFaces, f2[i]+tFaces, f2[i]+tFaces);}
									else{sprintf(TEMP, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", f2[i]+tFaces, f2[i]+tFaces, f2[i]+tFaces, f1[i]+tFaces, f1[i]+tFaces, f1[i]+tFaces, f0[i]+tFaces, f0[i]+tFaces, f0[i]+tFaces);}
									//sprintf(TEMP, "f %d %d %d\n", f2[i]+tFaces, f1[i]+tFaces, f0[i]+tFaces); 
									outfile << TEMP;
									TiTemp--;
								}
								else
								{
									TiTemp--;
								}
							}
						}
					}
				}/**/
			}
			tFaces += Verts;
		}

		for (long ii = Geo; ii < fileSize && striper; ii++)
		{
			outfile.put(fileBuf[ii]);
		}
	} 

	return 0;
}