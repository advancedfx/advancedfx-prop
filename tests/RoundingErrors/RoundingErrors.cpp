// RoundingErrors.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <tchar.h>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	int frames = 72510;
	float fps = 60.0f;

	float fTime = 0.0f;
	double dTime = 0.0;

	for(int i=0;i<frames;i++)
	{
		float fDuration = ((i+1)/fps)-(i/fps);
		double dDuration = (1.0/(double)fps);

		fTime += fDuration;
		dTime += dDuration;

		cout << i << ": " << fTime << "/" << dTime << endl;
	}

	return 0;
}

