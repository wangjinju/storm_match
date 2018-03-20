#include "stdafx.h"
#include "Storm_match.h"

namespace storm
{

	bool CloudDetect(BYTE * Bitmap, DWORD & dwHeight, DWORD& dwWidth, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth, vector<double*>& cloudX_vector, vector<double*>& cloudY_vector, vector<int>& cloudPointCount, vector<int> belong, int & cloudCount, vector<Features> & cloud)
	{
		double** ppdX = NULL;
		double** ppdY = NULL;
		int* pnPtsCnt = NULL;
		int* pnBelong = NULL;
		if (!FindContour(Bitmap, ppdX, ppdY, pnPtsCnt, pnBelong, cloudCount, dwHeight, dwWidth, cloud, LowThre, HighThre, MergeThre, RSmooth)) return false;

		for (int i = 0; i < cloudCount; i++)
		{
			cloudX_vector.push_back(ppdX[i]);
			cloudY_vector.push_back(ppdY[i]);
			cloudPointCount.push_back(pnPtsCnt[i]);
			belong.push_back(pnBelong[i]);
		}

		delete [] ppdX;
		delete [] ppdY;
		delete [] pnPtsCnt;
		delete [] pnBelong;
		return true;
	}

	Track::Track()
	{
		isStart = false;
		isHaveFileName = false;
		m_dwHeight = 0;
		m_dwWidth = 0;
		m_flag = 0;
		m_pBitmap = NULL;
	}

	Track::Track(string FileName)
	{
		isStart = false;
		isHaveFileName = true;

		loadImageFromGivenFileName(FileName, m_dwHeight, m_dwWidth, m_flag, m_pBitmap);


	}

	Track::~Track()
	{
		delete[] m_pBitmap; m_pBitmap = NULL;
	}

	bool Track::loadImageFromGivenFileName(string FileName, int & dwHeight, int & dwWidth, int & flag, BYTE *& pBitmap)
	{
		if (FileName.empty()) return false;

		if (pBitmap != NULL) delete[] pBitmap;

		BYTE* pTmp = NULL;
		DWORD height_tmp, width_tmp;
		WORD flag_tmp;
		LoadVectorFromBMPFile(FileName, pTmp, height_tmp, width_tmp, flag_tmp);

		dwHeight = height_tmp;
		dwWidth = width_tmp;
		flag = flag_tmp;
		
		int dwSize;
		dwSize = dwHeight * dwWidth;
		pBitmap = new BYTE[dwSize];
		for (int i = 0; i < dwSize; i++)
			pBitmap[i] = pTmp[i];
		
		delete[] pTmp;
		return true;
	}
}