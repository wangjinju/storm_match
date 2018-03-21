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
		struct TrackUnit
		{
			int PicNumth;
			int cloudID;
			Point Position;
		};

		Track();
		Track(string FileName, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth);
		~Track();

	public:
		void track(string FileName);
		bool track_operator();
	protected:
		bool loadImageFromGivenFileName(string FileName, DWORD &dwHeight, DWORD &dwWidth, WORD &flag, BYTE* &pBitmap);
		bool IdentifyAndMatch(BYTE** pOut, WORD ImgNumber, DWORD dwHeight, DWORD dwWidth, int Search_Radius, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth);
		bool CloudDetect(BYTE* Bitmap, DWORD &dwHeight, DWORD &dwWidth, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth, vector<double *> &cloudX_vector, vector<double *> &cloudY_vector, vector<int> &cloudPointCount, vector<int> belong, int &cloudCount, vector<Features> &cloud);
	private:
		list<queue<TrackUnit> > trackLineVector;
		int TrackCount;

		vector<Features> prePicCloud;
		BYTE* m_pBitmap;
		DWORD m_dwHeight;
		DWORD m_dwWidth;
		WORD m_flag;

		bool isStart;
		bool isHaveFileName;
		
		WORD m_LowThre;
		WORD m_HighThre;
		WORD m_MergeThre;
		WORD m_RSmooth;
	};

}