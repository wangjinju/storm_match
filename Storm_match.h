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

		Track();
		Track(string FileName, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth, WORD Search_Radius);
		~Track();

	public:
		bool Track_start();
	protected:
		bool loadImageFromGivenFileName(string FileName, DWORD &dwHeight, DWORD &dwWidth, WORD &flag, BYTE* &pBitmap);
		string GetFileNum(string FileName);
		bool WritePicCloudToTxtFile(string FileName, vector<double *>& cloudX_vector, vector<double *>& cloudY_vector, vector<int>& cloudPointCount, int& cloudCount, vector<Features>& cloud);
		bool ReadPicCloudFromTxtFile(string FileNum, vector<double *>& cloudX_vector, vector<double *>& cloudY_vector, vector<int>& cloudPointCount, int& cloudCount, vector<Features>& cloud);
		bool CloudDetect(BYTE* Bitmap, DWORD &dwHeight, DWORD &dwWidth, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth, vector<double *> &cloudX_vector, vector<double *> &cloudY_vector, vector<int> &cloudPointCount, /*vector<int> belong, */int &cloudCount, vector<Features> &cloud);

		bool GetNextPicFeatures(string & FileName, string & PicNum, BYTE* &Bitmap, DWORD & dwHeight, DWORD & dwWidth, WORD & flag, vector<double *> & NextPicCloudX_vector, vector<double *> & NextPicCloudY_vector, vector<int> & NextPicCloudPointCount, /*vector<int> & belong, */int & NextPicCloudCount, vector<Features> & NextPicCloud);
		bool GetMatchMap(BYTE * Bitmap, vector<double*> & NextPicCloudX_vector, vector<double*> & NextPicCloudY_vector, vector<int> & NextPicCloudPointCount, int & NextPicCloudCount, vector<Features> & NextPicCloud);
		bool correlation(BYTE* Bitmap, vector<Features> & NextPicCloud, vector<Features> & currentPicRelationCloud);
		bool CalcRelationCloudEdge(vector<Features> &NextPicCloud, vector<double *> &NextPicCloudX_vector, vector<double *> &NextPicCloudY_vector, vector<int> & NextPicCloudPointCount, vector<Features> &currentPicRelationCloud, vector<double *> &currentPicRelationCloudX_vector, vector<double *> &currentPicRelationCloudY_vector, vector<int> & currentPicRelationCloudPointCount);
		bool GetCloudIntersectVector(vector<double *> & currentPicRelationCloudX_vector, vector<double *> & currentPicRelationCloudY_vector, vector<int> & currentPicRelationCloudPointCloud, vector<Features> & currentPicRelationCloud, vector<vector<int> > &IntersectVector);
		bool CheckCloudEdgeIntersect(int currentPicRelationCloudNum, int currentPicCloudNum, vector<double *> & currentPicRelationCloudX_vector, vector<double *> & currentPicRelationCloudY_vector, vector<int> & currentPicRelationCloudPointCloud, vector<Features> & currentPicRelationCloud);

		bool AnalyseMatchMap(string & FileName, string & PicNum, vector<Features> &NextPicCloud);
		bool WriteTrackLineToTxtFile(int lineOrder, vector<TrackUnit> & trackLine, vector<int> & mergeLineOrder, vector<int> & splitLineOrder = vector<int>());
		bool ReadTrackLineFromTxtFile(int & lineOrder, vector<TrackUnit> & trackLine, vector<int> & mergeLineOrder, vector<int> & splitLineOder);
	private:
		list<vector<TrackUnit> > trackLineVector;
		list<vector<int> > lineMergeOrder;
		list<int> trackLineOrder;
		int trackLineCount;

		vector<vector<int> > matchMap;
		vector<Features> currentPicCloud; //现阶段没有考虑出现环状的云团
		vector<double *> currentPicCloudX_vector;
		vector<double *> currentPicCloudY_vector;
		vector<int> currentPicCloudPointCount;
		int currentPicCloudCount;
		BYTE* m_pBitmap;
		DWORD m_dwHeight;
		DWORD m_dwWidth;
		WORD m_flag;

		bool isStart;
		bool isHaveFileName;

		string track_current_filename;
		string current_PicNum;
		int step;
		
		WORD m_LowThre;
		WORD m_HighThre;
		WORD m_MergeThre;
		WORD m_RSmooth;
		WORD m_Search_Radius;
	};

}