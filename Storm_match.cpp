#include "stdafx.h"
#include "Storm_match.h"

namespace storm
{
	void Track::TrackUnit::secureUnit(string PicNum, Features cloud)
	{
		PicNumth = stoi(PicNum);
		cloudID = cloud.ID;
		Position = cloud.centroid;
	}

	Track::Track()
	{
		isStart = false;
		isHaveFileName = false;

		m_dwHeight = 0;
		m_dwWidth = 0;
		m_flag = 0;
		m_pBitmap = NULL;

		int TrackCount = 0;

		m_LowThre = 0;
		m_HighThre = 0;
		m_MergeThre = 0;
		m_RSmooth = 0;
		m_Search_Radius = 0;

		prePicCloud.clear();
		trackLineVector.clear();
	}

	Track::Track(string FileName, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth, WORD Search_Radius)
	{
		isStart = false;
		isHaveFileName = true;

		loadImageFromGivenFileName(FileName, m_dwHeight, m_dwWidth, m_flag, m_pBitmap);

		m_LowThre = LowThre; m_HighThre = HighThre; m_MergeThre = MergeThre; m_RSmooth = RSmooth; m_Search_Radius = Search_Radius;

		vector<double *> cloudX_vector;
		vector<double *> cloudY_vector;
		vector<int> cloudPointCount;
		/*vector<int> belong;*/
		int cloudCount;
		CloudDetect(m_pBitmap, m_dwHeight, m_dwWidth, m_LowThre, m_HighThre, m_MergeThre, m_RSmooth, cloudX_vector, cloudY_vector, cloudPointCount, cloudCount, prePicCloud);

		string PicNum = GetFileNum(FileName);
		WritePicCloudToTxtFile(PicNum, cloudX_vector, cloudY_vector, cloudPointCount, cloudCount, prePicCloud);

		for (int i = 0; i < cloudCount; i++)
		{
			TrackUnit unit;
			queue<TrackUnit> tmp;
			unit.secureUnit(PicNum, prePicCloud[i]);
			tmp.push(unit);
			trackLineVector.push_back(tmp);
		}
		TrackCount = cloudCount;

		for (int i = 0; i < cloudCount; i++)
		{
			delete cloudX_vector[i];
			delete cloudY_vector[i];
		}
	}

	Track::~Track()
	{
		delete[] m_pBitmap; m_pBitmap = NULL;
	}

	bool Track::loadImageFromGivenFileName(string FileName, DWORD & dwHeight, DWORD & dwWidth, WORD & flag, BYTE *& pBitmap)
	{
		if (FileName.empty()) return false;

		if (pBitmap != NULL) delete[] pBitmap;

		BYTE* pTmp = NULL;
		LoadVectorFromBMPFile(FileName, pTmp, dwHeight, dwWidth, flag);
		
		int dwSize;
		dwSize = dwHeight * dwWidth;
		pBitmap = new BYTE[dwSize];
		for (int i = 0; i < dwSize; i++)
			pBitmap[i] = pTmp[i];
		
		delete[] pTmp;
		return true;
	}

	string Track::GetFileNum(string FileName)
	{
		if (FileName.empty()) return string();

		string StrNum;
		int Num;
		int nStrLength;	
		int nNumLength;	
		nStrLength = FileName.length();
		int nPositionAhead = FileName.find(' ');
		int nPositionFront = FileName.find(".bmp");	
		if (nPositionFront < 0)
			nPositionFront = FileName.find(".BMP");
		if (nPositionAhead == -1)
		{
			return string();
		}
		if (nPositionFront == -1)
		{
			return string();
		}
		nPositionAhead++;
		nPositionFront--;
		nNumLength = nPositionFront - nPositionAhead + 1;

		StrNum = FileName.substr(nPositionAhead, nNumLength);

		return StrNum;
	}

	bool Track::WritePicCloudToTxtFile(string FileName, vector<double*>& cloudX_vector, vector<double*>& cloudY_vector, vector<int>& cloudPointCount, int & cloudCount, vector<Features>& cloud)
	{
		if (FileName.empty() || cloudX_vector.empty() || cloudY_vector.empty() || cloudPointCount.empty() || cloudCount == 0 || cloud.empty()) return false;

		string prefix = "./data/Pic_";
		string suffix = ".txt";
		system("md data");

		FileName = prefix + FileName + suffix;

		ofstream fout;
		fout.open(FileName);

		for (int i = 0; i < cloudCount; i++)
		{
			if (cloudPointCount[i] <= 0) continue;

			if (i == cloudCount - 1)
			{
				fout << cloud[i].ID << ' ' << cloud[i].left << ' ' << cloud[i].right << ' ' << cloud[i].top << ' ' << cloud[i].bottom << ' ' << cloud[i].Area << ' ' << cloud[i].centroid.x << ' ' << cloud[i].centroid.y << '\n';

				for (int j = 0; j < cloudPointCount[i]; j++)
				{
					if (j == cloudPointCount[i] - 1) fout << cloudX_vector[i][j];
					else fout << cloudX_vector[i][j] << ' ';
				}
				fout << '\n';
				for (int j = 0; j < cloudPointCount[i]; j++)
				{
					if (j == cloudPointCount[i] - 1) fout << cloudY_vector[i][j];
					else fout << cloudY_vector[i][j] << ' ';
				}
			}
			else
			{
				fout << cloud[i].ID << ' ' << cloud[i].left << ' ' << cloud[i].right << ' ' << cloud[i].top << ' ' << cloud[i].bottom << ' ' << cloud[i].Area << ' ' << cloud[i].centroid.x << ' ' << cloud[i].centroid.y << '\n';

				for (int j = 0; j < cloudPointCount[i]; j++)
				{
					if (j == cloudPointCount[i] - 1) fout << cloudX_vector[i][j];
					else fout << cloudX_vector[i][j] << ' ';
				}
				fout << '\n';
				for (int j = 0; j < cloudPointCount[i]; j++)
				{
					if (j == cloudPointCount[i] - 1) fout << cloudY_vector[i][j];
					else fout << cloudY_vector[i][j] << ' ';
				}
				fout << '\n';
			}
		}
		fout.close();
		return true;
	}

	bool Track::CloudDetect(BYTE * Bitmap, DWORD & dwHeight, DWORD & dwWidth, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth, vector<double*>& cloudX_vector, vector<double*>& cloudY_vector, vector<int>& cloudPointCount, /*vector<int> belong, */int & cloudCount, vector<Features>& cloud)
	{
		double** ppdX = NULL;
		double** ppdY = NULL;
		int* pnPtsCnt = NULL;
		int* pnBelong = NULL;
		int nCount = 0;
		if (!FindContour(Bitmap, ppdX, ppdY, pnPtsCnt, pnBelong, nCount, dwHeight, dwWidth, cloud, LowThre, HighThre, MergeThre, RSmooth)) return false;

		cloudCount = 0;
		for (int i = 0; i < nCount; i++)
		{
			if (pnPtsCnt[i] <= 0)
			{
				delete[] ppdX[i];
				delete[] ppdY[i];

				continue;
			}
			cloudCount++;
			cloudX_vector.push_back(ppdX[i]);
			cloudY_vector.push_back(ppdY[i]);
			cloudPointCount.push_back(pnPtsCnt[i]);
			//belong.push_back(pnBelong[i]);
		}

		delete[] ppdX;
		delete[] ppdY;
		delete[] pnPtsCnt;
		delete[] pnBelong;
		return true;
	}
	
}
