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
		track_current_filename = string();
		step = 0;

		m_dwHeight = 0;
		m_dwWidth = 0;
		m_flag = 0;
		m_pBitmap = NULL;

		m_LowThre = 0;
		m_HighThre = 0;
		m_MergeThre = 0;
		m_RSmooth = 0;
		m_Search_Radius = 0;

		currentPicCloud.clear();
		currentPicCloudX_vector.clear();
		currentPicCloudY_vector.clear();
		currentPicCloudPointCount.clear();
		currentPicCloudCount = 0;
		matchMap.clear();
		trackLineVector.clear();
		trackLineOrder.clear();
	}

	Track::Track(string FileName, WORD LowThre, WORD HighThre, WORD MergeThre, WORD RSmooth, WORD Search_Radius)
	{
		isStart = false;
		isHaveFileName = true;
		track_current_filename = FileName;
		step = 1;

		loadImageFromGivenFileName(FileName, m_dwHeight, m_dwWidth, m_flag, m_pBitmap);

		m_LowThre = LowThre; m_HighThre = HighThre; m_MergeThre = MergeThre; m_RSmooth = RSmooth; m_Search_Radius = Search_Radius;

		/*vector<int> belong;*/
		CloudDetect(m_pBitmap, m_dwHeight, m_dwWidth, m_LowThre, m_HighThre, m_MergeThre, m_RSmooth, currentPicCloudX_vector, currentPicCloudY_vector, currentPicCloudPointCount, currentPicCloudCount, currentPicCloud);

		string PicNum = GetFileNum(FileName);
		WritePicCloudToTxtFile(PicNum, currentPicCloudX_vector, currentPicCloudY_vector, currentPicCloudPointCount, currentPicCloudCount, currentPicCloud);

		for (int i = 0; i < currentPicCloudCount; i++)
		{
			TrackUnit unit;
			queue<TrackUnit> tmp;
			unit.secureUnit(PicNum, currentPicCloud[i]);
			tmp.push(unit);
			trackLineVector.push_back(tmp);
			trackLineOrder.push_back(i);

			matchMap.push_back(0);
		}
	}

	Track::~Track()
	{
		delete[] m_pBitmap; m_pBitmap = NULL;
	}

	bool Track::Track_start()
	{
		if (!isHaveFileName) return false;
		
		BYTE * NextBitmap = NULL;
		DWORD NextdwHeight, NextdwWidth;
		WORD Nextflag;
		vector<double *> NextPicCloudX_vector;
		vector<double *> NextPicCloudY_vector;
		vector<int> NextPicCloudPointCount;
		int NextPicCloudCount;
		vector<Features> NextPicCloud;

		GetNextPicFeatures(NextBitmap, NextdwHeight, NextdwWidth, Nextflag, NextPicCloudX_vector, NextPicCloudY_vector, NextPicCloudPointCount, NextPicCloudCount, NextPicCloud);

		GetMatchMap(NextBitmap, NextPicCloudX_vector, NextPicCloudY_vector, NextPicCloudPointCount, NextPicCloudCount, NextPicCloud);

		return true;
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
		if (FileName.empty() || cloudX_vector.empty() || cloudY_vector.empty() || cloudPointCount.empty() || cloudCount == 0 || cloud.empty())
			return false;

		system("md data");
		string prefix = "./data/Pic_";
		string suffix = ".txt";
		string cloudNum = FileName;
		FileName = prefix + cloudNum + suffix;

		ofstream fout;
		fout.open(FileName, ios::out);

		fout << cloudCount << '\n';//写入云团数目

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

	bool Track::ReadPicCloudFromTxtFile(string FileNum, vector<double*>& cloudX_vector, vector<double*>& cloudY_vector, vector<int>& cloudPointCount, int & cloudCount, vector<Features>& cloud)
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

		fin >> cloudCount;//获取云团数目

		while (!fin.eof())
		{
			for (int i = 0; i < cloudCount; i++)
			{
				Features tmp;
				int countTmp;
				fin >> tmp.ID >> tmp.left >> tmp.right >> tmp.top >> tmp.bottom >> tmp.Area >> tmp.centroid.x >> tmp.centroid.y;
				fin >> countTmp;

				cloudPointCount.push_back(countTmp);
				cloud.push_back(tmp);

				double * pdX = new double[countTmp];
				double * pdY = new double[countTmp];

				for (int j = 0; j < countTmp; j++)
				{
					fin >> pdX[j];
				}
				cloudX_vector.push_back(pdX);

				for (int j = 0; j < countTmp; j++)
				{
					fin >> pdY[j];

				}
				cloudY_vector.push_back(pdY);
			}
			fin.close();
		}
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

	bool Track::GetNextPicFeatures(BYTE *& NextBitmap, DWORD & NextdwHeight, DWORD & NextdwWidth, WORD & Nextflag, vector<double *> & NextPicCloudX_vector, vector<double *> & NextPicCloudY_vector, vector<int> & NextPicCloudPointCount, /*vector<int> & belong, */int & NextPicCloudCount, vector<Features> & NextPicCloud)
	{
		if (NextBitmap)
		{
			delete[] NextBitmap; NextBitmap = NULL;
		}
		string FileName = track_current_filename;
		if (FindNextFileName(FileName, step)) return false;

		if (loadImageFromGivenFileName(FileName, NextdwHeight, NextdwWidth, Nextflag, NextBitmap)) return false;

		CloudDetect(NextBitmap, NextdwHeight, NextdwWidth, m_LowThre, m_HighThre, m_MergeThre, m_RSmooth, NextPicCloudX_vector, NextPicCloudY_vector, NextPicCloudPointCount, NextPicCloudCount, NextPicCloud);
		
		string PicNum = GetFileNum(FileName);
		WritePicCloudToTxtFile(PicNum, NextPicCloudX_vector, NextPicCloudY_vector, NextPicCloudPointCount, NextPicCloudCount, NextPicCloud);

		track_current_filename = FileName;

		return true;
	}

	bool Track::GetMatchMap(BYTE * NextBitmap, vector<double*> & NextPicCloudX_vector, vector<double*> & NextPicCloudY_vector, vector<int> & NextPicCloudPointCount, int & NextPicCloudCount, vector<Features> & NextPicCloud)
	{
		if (!NextBitmap) return false;
		if (currentPicCloud.empty()) //地区没有出现风暴的情况暂时没有考虑
		{
			return false;

			//可能添加没有出现风暴的情况
		}
		if (NextPicCloud.empty())
		{
			return false;

			//可能添加没有出现风暴的情况
		}
		
		DWORD dwSize = m_dwHeight * m_dwWidth;

		vector<Features> currentPicRelationCloud;
		correlation(NextBitmap, NextPicCloud, currentPicRelationCloud);

		vector<double*> currentPicRelationCloudX_vector;
		vector<double*> currentPicRelationCloudY_vector;
		vector<int> currentPicRelationCloudPointCount;
		CalcRelationCloudEdge(NextPicCloud, NextPicCloudX_vector, NextPicCloudY_vector, NextPicCloudPointCount, currentPicRelationCloud, currentPicRelationCloudX_vector, currentPicRelationCloudY_vector, currentPicRelationCloudPointCount);

		vector<vector<int> > RelationCloudRectIntersectVector;
		GetCloudRectIntersectVector(currentPicRelationCloud, RelationCloudRectIntersectVector);



		return true;
	}

	bool Track::correlation(BYTE * NextBitmap, vector<Features>& NextPicCloud, vector<Features>& currentPicRelationCloud)
	{
		if (!NextBitmap) return false;
		for (int i = 0; i < NextPicCloud.size(); i++)
		{
			Features tmp;
			int nTop = NextPicCloud[i].top;
			int nLeft = NextPicCloud[i].left;
			int nSizeX = NextPicCloud[i].right - NextPicCloud[i].left + 1;
			int nSizeY = NextPicCloud[i].bottom - NextPicCloud[i].top + 1;

			tmp = Match(NextBitmap, m_pBitmap, m_dwHeight, m_dwWidth, nLeft, nTop, nSizeX, nSizeY, m_Search_Radius);

			if (tmp.left == -1 || tmp.right == -1 || tmp.top == -1 || tmp.bottom == -1) continue; //没有相应的相关云团，将暂时不进行处理。

			tmp.ID = NextPicCloud[i].ID;

			currentPicRelationCloud.push_back(tmp);
		}

		return true;
	}

	bool Track::CalcRelationCloudEdge(vector<Features>& NextPicCloud, vector<double*>& NextPicCloudX_vector, vector<double*>& NextPicCloudY_vector, vector<int> & NextPicCloudPointCount, vector<Features>& currentPicRelationCloud, vector<double*>& currentPicRelationCloudX_vector, vector<double*>& currentPicRelationCloudY_vector, vector<int> & currentPicRelationCloudPointCount)
	{
		if (NextPicCloud.size() != currentPicRelationCloud.size()) return false; //没有相应的相关云团，不进行处理，以后改进

		for (int i = 0; i < NextPicCloud.size(); i++)
		{
			int cloudPointCount = NextPicCloudPointCount[i];

			double offSetY = currentPicRelationCloud[i].top - NextPicCloud[i].top;
			double offSetX = currentPicRelationCloud[i].left - NextPicCloud[i].left;
			double * pdX = new double[NextPicCloudPointCount[i]];
			double * pdY = new double[NextPicCloudPointCount[i]];
			for (int j = 0; j < cloudPointCount; j++)
			{
				pdX[j] = NextPicCloudX_vector[i][j]+ offSetX;
				pdY[j] = NextPicCloudY_vector[i][j] + offSetY;
			}
			currentPicRelationCloudX_vector.push_back(pdX);
			currentPicRelationCloudY_vector.push_back(pdY);
		}
		return true;
	}

	bool Track::GetCloudRectIntersectVector(vector<Features>& currentPicRelationCloud, vector<vector<int> > &IntersectVector)
	{
		if (currentPicRelationCloud.empty()) return false;

		int RelationSize = currentPicRelationCloud.size();

		for (int i = 0; i < RelationSize; i++)
		{
			vector<int> matchVec;
			matchVec.push_back(i);

			double Match_RectangleTempX = (currentPicRelationCloud[i].right + currentPicRelationCloud[i].left) / 2.0;
			double Match_XLength = currentPicRelationCloud[i].right - currentPicRelationCloud[i].left;
			double Match_RectangleTempY = (currentPicRelationCloud[i].top + currentPicRelationCloud[i].bottom) / 2.0;
			double Match_YLength = currentPicRelationCloud[i].top - currentPicRelationCloud[i].bottom;

			for (int j = 0; j < currentPicCloudCount; j++)
			{
				Features His_RectangleTemp = currentPicCloud[j];
				double His_RectangleTempX = (His_RectangleTemp.right + His_RectangleTemp.left) / 2.0;
				double His_XLength = His_RectangleTemp.right - His_RectangleTemp.left;
				double His_RectangleTempY = (His_RectangleTemp.top + His_RectangleTemp.bottom) / 2.0;
				double His_YLength = His_RectangleTemp.top - His_RectangleTemp.bottom;

				bool X_Cross = fabs(Match_RectangleTempX - His_RectangleTempX) < fabs((His_XLength + Match_XLength) / 2.0 /*+ 1.0*/);
				bool Y_Cross = fabs(Match_RectangleTempY - His_RectangleTempY) < fabs((His_YLength + Match_YLength) / 2.0 /*+ 1.0*/);

				if (X_Cross && Y_Cross)
				{
					matchVec.push_back(j);
				}
			}
			IntersectVector.push_back(matchVec);
		}
		return true;
	}

	bool Track::CalcRelationCloudEdgeIntersectVector(vector<vector<int>>& IntersectVector)
	{
		if (IntersectVector.size() == 0) return false;

		int matchNum = IntersectVector.size(); //匹配框的数量 22
		vector<vector<int>> match_his;

		for (int i = 0; i < matchNum; i++)
		{
			int hisNum = IntersectVector[i].size() - 1; //第 i 个Match中匹配到的 His 个数； 0表示当面Match没有匹配到历史云团

			for (int j = 0; j < hisNum; j++)
			{
				vector<int> match_hisVec;

				Features IntersectCloud = currentPicCloud[IntersectVector[i][j + 1]];
				int IntersectCloudPointCount = currentPicCloudPointCount[IntersectVector[i][j + 1]];
	
				//算法部分
				for (int k = 0; k < pointNumMatch; k++)
				{
					double matchPointX = ppdX_Match[i][k];
					double matchPointY = ppdY_Match[i][k];
					int pointNum = pnPtsCnt_His[index_his] - 2;		//match云团边界点个数
																	//判断在里面的点数至少存在 insidePointNum 个
					InsideMargin(pointNum, ppdX_His[index_his], ppdY_His[index_his], matchPointX, matchPointY) ? insidePointNum++ : 0;
				}

				//如果是边界相交或者包含关系 就保存 Match_Rectangle 序号索引 和His_Rectangle 序号索引 是一对前后关系
				if (insidePointNum >= 2)
				{
					match_hisVec.push_back(i);
					match_hisVec.push_back(match_his_queue[i][j + 1]);
					match_his.push_back(match_hisVec);
				}
				match_hisVec.clear();
			}
		}
		return match_his;
	}
	
}
