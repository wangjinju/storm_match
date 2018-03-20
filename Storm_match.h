#pragma once

#include "stdafx.h"
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "stormtype.h"
#include "fortest.h"
#include "Storm.h"

using namespace std;

namespace storm
{
	class Track
	{
	public:
		Track();
		Track(string FileName);
		~Track();

	public:
		void track(string FileName);
		bool track_operator();
	protected:
		bool loadImageFromGivenFileName(string FileName, int &dwHeight, int &dwWidth, int &flag, BYTE* &pBitmap);
		bool IdentifyAndMatch(BYTE** pOut, WORD ImgNumber, DWORD dwHeight, DWORD dwWidth, int Search_Radius, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth);
		bool CloudDetect(BYTE* Bitmap, DWORD &dwHeight, DWORD &dwWidth, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth, vector<double *> &cloudX_vector, vector<double *> &cloudY_vector, vector<int> &cloudPointCount, vector<int> belong, int &cloudCount, vector<Features> &cloud);
	//private:
	public:
		list<queue<Features> > trackvector;
		list<pair<int, int> > track_start_end_point_vector;
		BYTE* m_pBitmap;
		int m_dwHeight;
		int m_dwWidth;
		int m_flag;
		 
		bool isStart;
		bool isHaveFileName;
	};

	bool CloudDetect(BYTE* Bitmap, DWORD &dwHeight, DWORD &dwWidth, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth, vector<double *> &cloudX_vector, vector<double *> &cloudY_vector, vector<int> &cloudPointCount, vector<int> belong, int &cloudCount, vector<Features> &cloud);
}