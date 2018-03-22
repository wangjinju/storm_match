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

			void secureUnit(string PicNum, Features cloud);
		};

	public:
		Track();
		Track(string FileName, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth, WORD Search_Radius);
		~Track();

	protected:
		string GetFileNum(string FileName);
		bool loadImageFromGivenFileName(string FileName, DWORD &dwHeight, DWORD &dwWidth, WORD &flag, BYTE* &pBitmap);
		bool WritePicCloudToTxtFile(string FileName, vector<double *>& cloudX_vector, vector<double *>& cloudY_vector,
			vector<int>& cloudPointCount, int& cloudCount, vector<Features>& cloud);
		bool ReadPicCloudFromTxtFile(string FileNum, vector<double *>& cloudX_vector, vector<double *>& cloudY_vector,
			vector<int>& cloudPointCount, int& cloudCount, vector<Features>& cloud);
		bool CloudDetect(BYTE* Bitmap, DWORD &dwHeight, DWORD &dwWidth, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth, vector<double *> &cloudX_vector, vector<double *> &cloudY_vector, vector<int> &cloudPointCount, /*vector<int>& belong,*/ int &cloudCount, vector<Features> &cloud);

	public:
		list<queue<TrackUnit>> trackLinevector;
		int TrackCount;

		vector<Features> prePicCloud;
		BYTE* m_pBitmap;
		DWORD m_dwHeight;
		DWORD m_dwWidth;
		WORD m_flag;

		WORD m_LowThre;
		WORD m_HighThre;
		WORD m_MergeThre;
		WORD m_RSmooth;
		WORD m_Search_Radius;

		bool isStart;
		bool isHaveFileName;
	};
}