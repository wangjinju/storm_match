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
		trackLinevector.clear();
	}

	Track::Track(string FileName, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth, WORD SearchRadius)
	{
		isStart = false;
		isHaveFileName = true;

		loadImageFromGivenFileName(FileName, m_dwHeight, m_dwWidth, m_flag, m_pBitmap);

		m_LowThre = LowThre; m_HighThre = HighThre; m_MergeThre = MergeThre;
		m_RSmooth = RSmooth; m_Search_Radius = SearchRadius;

		vector<double *> cloudX_vector;
		vector<double *> cloudY_vector;
		vector<int> cloudPointCount;
		//vector<int> belong;
		int cloudCount;
		CloudDetect(m_pBitmap, m_dwHeight, m_dwWidth, m_LowThre, m_HighThre, m_MergeThre, m_RSmooth,
			cloudX_vector, cloudY_vector, cloudPointCount, /*belong,*/ cloudCount, prePicCloud);

		string PicNum = GetFileNum(FileName);
		WritePicCloudToTxtFile(PicNum, cloudX_vector, cloudY_vector, cloudPointCount, cloudCount, prePicCloud);
		ReadPicCloudFromTxtFile(PicNum, cloudX_vector, cloudY_vector, cloudPointCount, cloudCount, prePicCloud);//测试调用

		for (int i = 0; i < cloudCount; i++)
		{
			TrackUnit unit;
			queue<TrackUnit> tmp;
			unit.secureUnit(PicNum, prePicCloud[i]);
			tmp.push(unit);
			trackLinevector.push_back(tmp);
		}
		TrackCount = cloudCount;

		for(int i=0; i < cloudCount; i++)
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
		if (FileName.empty())
		{
			isHaveFileName = false;
			return false;
		}
		if (pBitmap != NULL) delete[] pBitmap;

		BYTE* pTmp = NULL;
		LoadVectorFromBMPFile(FileName, pTmp, dwHeight, dwWidth, flag);

		int dwSize;
		dwSize = dwHeight * dwWidth;
		pBitmap = new BYTE[dwSize];
		for (int i = 0; i < dwSize; i++)
		{
			pBitmap[i] = pTmp[i];
		}

		delete[] pTmp;
		return true;
	}

	string Track::GetFileNum(string FileName)
	{
		string StrNum;
		if (FileName.empty()) return string();
		int nStrLength;
		int nNumLength;
		nStrLength = FileName.length();
		int nPositionAhead = FileName.find(" ");
		int nPositionFront = FileName.find(".bmp");
		if (nPositionFront < 0)
			nPositionFront = FileName.find(".BMP");
		nPositionAhead++;
		nPositionFront--;
		nNumLength = nPositionFront - nPositionAhead + 1;

		StrNum = FileName.substr(nPositionAhead, nNumLength);
		
		return StrNum;
	}

	bool Track::WritePicCloudToTxtFile(string FileName, vector<double *>& cloudX_vector, vector<double *>& cloudY_vector, vector<int>& cloudPointCount, 
		int& cloudCount, vector<Features>& cloud)
	{
		if (FileName.empty() || cloudX_vector.empty() || cloudY_vector.empty() || cloudPointCount.empty() || cloudCount == 0 || cloud.empty()) 
			return false;

		system("md data");
		string prefix = "./data/Pic_";
		string suffix = ".txt";
		string cloudNum = FileName;
		FileName = prefix + cloudNum + suffix;

		ofstream fout;
		fout.open(FileName, ios::out);

		for (int i = 0; i < cloudCount; i++)
		{
			if (cloudPointCount[i] <= 0) continue;

			if (i == cloudCount - 1)
			{
				fout << cloud[i].ID << ' ' << cloud[i].left << ' ' << cloud[i].right << ' ' << cloud[i].top << ' ' << cloud[i].bottom 
					<< ' ' << cloud[i].Area << ' ' << cloud[i].centroid.x << ' ' << cloud[i].centroid.y
					<< ' ' << cloudPointCount[i] << '\n';

				for (int j = 0; j < cloudPointCount[i]; j++)
				{
					if (j == cloudPointCount[i] - 1)
						fout << cloudX_vector[i][j];
					else
						fout << cloudX_vector[i][j] << ' ';
				}
				fout << '\n';
				for (int j = 0; j < cloudPointCount[i]; j++)
				{
					if (j == cloudPointCount[i] - 1)
						fout << cloudY_vector[i][j];
					else
						fout << cloudY_vector[i][j] << ' ';
				}
			}

			else
			{
				fout << cloud[i].ID << ' ' << cloud[i].left << ' ' << cloud[i].right << ' ' << cloud[i].top << ' ' << cloud[i].bottom 
					<< ' ' << cloud[i].Area << ' ' << cloud[i].centroid.x << ' ' << cloud[i].centroid.y
					<< ' ' << cloudPointCount[i] << '\n';

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

	bool Track::ReadPicCloudFromTxtFile(string FileNum, vector<double *>& cloudX_vector, vector<double *>& cloudY_vector, vector<int>& cloudPointCount,
		int& cloudCount, vector<Features>& cloud)
	{
		if (FileNum.empty()) return false;

		cloudX_vector.clear();
		cloudY_vector.clear();
		cloudPointCount.clear();
		cloud.clear();

		string prefix = "./data/Pic_";
		string suffix = ".txt";
		string FileName = prefix + FileNum + suffix;

		ifstream fin;
		fin.open(FileName, ios::in);

		//获取行数及云团数
		char c;
		int nLine = 1;
		while (fin.get(c))
		{
			if (c == '\n')
				nLine++;
		}
		if (nLine % 3 == 0)
			cloudCount = nLine / 3;
		else
			return false;

		fin.close();
		fin.open(FileName, ios::in);
		//fin.seekg(0, ios::beg);//回到文件开始位置？

		DWORD IDTmp;
		double topTmp;
		double bottomTmp;
		double leftTmp;
		double rightTmp;
		Point  centroidTmp;
		double AreaTmp;
		int cloudPointCountTmp;
		Features cloudTmp;
		double* pCloudXTmp = NULL;
		double* pCloudYTmp = NULL;

		while (!fin.eof())
		{
			for (int i = 0; i < cloudCount; i++)
			{
				//读取云团特征
				fin >> IDTmp >> leftTmp >> rightTmp >> topTmp >> bottomTmp >> AreaTmp >> centroidTmp.x >> centroidTmp.y >> cloudPointCountTmp;
				cloudTmp.ID = IDTmp;
				cloudTmp.left = leftTmp;
				cloudTmp.right = rightTmp;
				cloudTmp.top = topTmp;
				cloudTmp.bottom = bottomTmp;
				cloudTmp.Area = AreaTmp;
				cloudTmp.centroid.x = centroidTmp.x;
				cloudTmp.centroid.y = centroidTmp.y;
				cloudPointCount.push_back(cloudPointCountTmp);
				cloud.push_back(cloudTmp);

				//读取x,y指针向量
				pCloudXTmp = new double[cloudPointCountTmp * sizeof(double)];
				pCloudYTmp = new double[cloudPointCountTmp * sizeof(double)];
				for (int j = 0; j < cloudPointCountTmp; j++)
				{
					fin >> *pCloudXTmp;
				}
				cloudX_vector.push_back(pCloudXTmp);

				for (int j = 0; j < cloudPointCountTmp; j++)
				{
					fin >> *pCloudYTmp;
				}
				cloudY_vector.push_back(pCloudYTmp);
			}
			delete pCloudXTmp;
			delete pCloudYTmp;
			fin.close();
		}
		return true;
	}

	bool Track::CloudDetect(BYTE * Bitmap, DWORD & dwHeight, DWORD& dwWidth, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth,
		vector<double*>& cloudX_vector, vector<double*>& cloudY_vector, vector<int>& cloudPointCount, /*vector<int>& belong,*/ int & cloudCount, vector<Features> & cloud)
	{
		double** ppdX = NULL;
		double** ppdY = NULL;
		int* pnPtsCnt = NULL;
		int* pnBelong = NULL;//内轮廓从属关系(不用)
		int nCount = 0;
		vector<int> cloudQueue;

		if (!FindContour(Bitmap, ppdX, ppdY, pnPtsCnt, pnBelong, nCount, dwHeight, dwWidth, cloud, LowThre, HighThre, MergeThre, RSmooth)) return false;

		cloudCount = 0;
		for (int i = 0; i < nCount; i++)
		{
			if (pnPtsCnt[i] <= 0)//排除内轮廓
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
