#include "refiner.h"

void MatchesRefiner::readImage(const string& refFn, const string& srcFn)
{
	refIm = imread(refFn, CV_LOAD_IMAGE_UNCHANGED);
	if(refIm.data == nullptr)
	{
		cerr << "failed to open " << refFn << endl;
		return ;
	}

	srcIm = imread(srcFn, CV_LOAD_IMAGE_UNCHANGED);
	if(srcIm.data == nullptr)
	{
		cerr << "failed to open " << srcFn << endl;
		return ;
	}
}

//ȥ�������matches
//����sidxΪkey����ɨһ��reference -> source�Ķ��һ����
//����ridxΪkey����ɨһ��source -> reference�Ķ��һ����
//��֤����������һ��һmatch
void MatchesRefiner::readKeypoints(const string& matchFn)
{
	fstream fin(matchFn.c_str(), ios::in);
	if (fin.is_open() == false)
	{
		cout << "failed to open " << matchFn << endl;
		return ;
	}
	
	int sz;	
	fin >> sz;

	//key - source index;  value - reference index
	for(int i = 0; i < sz; ++i)
	{
		int rx, ry, sx, sy;
		fin >> rx >> ry >> sx >> sy;
		
		KeyPoint rpt = cv::KeyPoint(rx, ry, 2);
		KeyPoint spt = cv::KeyPoint(sx, sy, 2);
		refKeypts.push_back(rpt);
		srcKeypts.push_back(spt);
	}
}

void MatchesRefiner::removeRebundancy(vector<KeyPoint>& refpts, vector<KeyPoint>& srcpts)
{
	vector<KeyPoint> trpts(0), tspts(0);
	copy(refpts.begin(), refpts.end(), back_inserter(trpts));
	copy(srcpts.begin(), srcpts.end(), back_inserter(tspts));

	int sz = trpts.size();
	int width = srcIm.size().width;
	map<int, int> traverse;  //key ��  point index

	//����source -> reference ����һ�Զ��ƥ�䣺��һ��souce point �����˶��
	refpts.clear();
	srcpts.clear();	
	for(int i = 0; i < sz; ++i)
	{
		int ridx = trpts[i].pt.y * width + trpts[i].pt.x;
		int sidx = tspts[i].pt.y * width + tspts[i].pt.x;

		map<int,int>::iterator it = traverse.find(sidx);
		
		if( it == traverse.end() )
		{
			traverse.insert(pair<int,int>(sidx, 1));
		}
		else
		{
			it->second += 1;
		}
	}

	for(int i = 0; i < sz; ++i)
	{
		int ridx = trpts[i].pt.y * width + trpts[i].pt.x;
		int sidx = tspts[i].pt.y * width + tspts[i].pt.x; 

		map<int,int>::iterator it = traverse.find(sidx);

		if(it->second == 1)
		{
			refpts.push_back(trpts[i]);
			srcpts.push_back(tspts[i]);
		}
	}

	cout << "removal repeated matches. first traverse: " << sz  << " -> " << refpts.size() << endl;

	
	//����reference->source һ�Զ��ƥ�䣬 ��ridxΪkey
	trpts.clear();
	tspts.clear();
	copy(refpts.begin(), refpts.end(), back_inserter(trpts));
	copy(srcpts.begin(), srcpts.end(), back_inserter(tspts));

	sz = tspts.size();
	refpts.clear();
	srcpts.clear();
	traverse.clear();
	for(int i = 0; i < sz; ++i)
	{	
		int ridx = trpts[i].pt.y * width + trpts[i].pt.x;
		int sidx = tspts[i].pt.y * width + tspts[i].pt.x; 

		map<int,int>::iterator it = traverse.find(ridx);
		if(it == traverse.end())
		{
			traverse.insert(pair<int,int>(ridx, 1));
		}
		else
		{
			it->second += 1;
		}
	}
	
	
	for(int i = 0; i < sz; ++i)
	{
		int ridx = trpts[i].pt.y * width + trpts[i].pt.x;
		int sidx = tspts[i].pt.y * width + tspts[i].pt.x; 

		map<int,int>::iterator it = traverse.find(ridx);

		if(it->second == 1)
		{
			refpts.push_back(trpts[i]);
			srcpts.push_back(tspts[i]);
		}
	}

	cout << "removal repeated matches. second traverse: " << sz  << " -> " << refpts.size() << endl;

}

void MatchesRefiner::transfer2DMatches(const vector<KeyPoint>& refpts, const vector<KeyPoint>& srcpts, vector<DMatch>& dmatches)
{
	dmatches.clear();
	int sz = refpts.size();
	for(int i = 0; i < sz; ++i)
	{
		KeyPoint p1 = refpts[i];
		KeyPoint p2 = srcpts[i];

		float distance = static_cast<float>(sqrt((p1.pt.x-p2.pt.x)*(p1.pt.x-p2.pt.x) + (p1.pt.y - p2.pt.y)*(p1.pt.y - p2.pt.y)));
		DMatch m = DMatch(i,i,distance);

		dmatches.push_back(m);
	}	
}

void MatchesRefiner::saveMatches(const vector<KeyPoint>& refpts, const vector<KeyPoint>& srcpts, const string& fn)
{	
	fstream fout(fn.c_str(), ios::out);
	if (fout.is_open() == false)
	{
		cout << "failed to open " << fn << endl;
		return ;
	}

	int sz = refpts.size();
	fout << sz << endl;
	for(int i = 0; i < sz; ++i)
	{
		fout << refpts[i].pt.x << ' ' << refpts[i].pt.y << ' ' 
			 << srcpts[i].pt.x << ' ' << srcpts[i].pt.y << endl;
	}
	fout.close();	
}

void MatchesRefiner::Initial(const string& refFn, const string& srcFn, const string& matchFn)
{
	refKeypts.resize(0);
	srcKeypts.resize(0);
	initialMatches.resize(0);

	readImage(refFn, srcFn);

	imgwidth = refIm.size().width;
	imgheight = srcIm.size().height;

	readKeypoints(matchFn);	
	//removeRebundancy(refKeypts, srcKeypts);
	transfer2DMatches(refKeypts, srcKeypts, initialMatches);

	cout << "read images done." << endl;
	cout << "read keypoints done." << endl;
	cout << "matches size = " << initialMatches.size() << endl;
}

void MatchesRefiner::Refine()
{
	//1. ����F
	//2. ����������ֵ��Сֵ��С��3����Сֵ����Ϊgood matches

	vector<Point2f> refPts(0), srcPts(0);
	Mat fundamental;
	
	int sz = initialMatches.size();
	for(int i = 0; i < sz; ++i)
	{	
		int rIdx = initialMatches[i].queryIdx;
		int sIdx = initialMatches[i].trainIdx;

		float x = refKeypts[rIdx].pt.x;
		float y = refKeypts[rIdx].pt.y;
		refPts.push_back(Point2f(x,y));

		x = srcKeypts[sIdx].pt.x;
		y = srcKeypts[sIdx].pt.y;
		srcPts.push_back(Point2f(x,y));
	}

	vector<uchar> inliers(sz,0);
	double confidence = 0.99 ;
	double distance = 3.0;
	if(refPts.size() > 0 && srcPts.size() > 0)
	{
		fundamental = cv::findFundamentalMat(cv::Mat(refPts), cv::Mat(srcPts), inliers, CV_FM_RANSAC, distance, confidence);
	}
	else
	{
		cerr << "fail in calculate fundamental matrix using RANSAC." << endl;
	}
	
	//������Щ��������match
	//������Щ��������������
	int cnt = 0;
	refineMatches.clear();
	outlierMatches.clear();
	vector<DMatch> rMatches = refineMatches; 
	for(int i = 0; i < sz; ++i)
	{			
		//������
		//refKeypts[rIdx].pt.x < imgwidth && refKeypts[rIdx].pt.y < imgheight && 
			        //srcKeypts[sIdx].pt.x < imgwidth && srcKeypts[sIdx].pt.y < imgheight)
		if(inliers[i] == true )
		{
			refineMatches.push_back(initialMatches[i]);
			int rIdx = refineMatches[cnt].queryIdx;
		    int sIdx = refineMatches[cnt].trainIdx;
			if(refKeypts[rIdx].pt.x < imgwidth && refKeypts[rIdx].pt.y < imgheight && 
			        srcKeypts[sIdx].pt.x < imgwidth && srcKeypts[sIdx].pt.y < imgheight)
			{
			    re_refKeypts.push_back(refKeypts[rIdx]);
			    re_srcKeypts.push_back(srcKeypts[sIdx]);
			    cnt ++;
			}
			else
			{
				refineMatches.erase(refineMatches.begin()+cnt);
				cout<< refKeypts[rIdx].pt.x << " " << refKeypts[rIdx].pt.y << " " << srcKeypts[sIdx].pt.x << " " << srcKeypts[sIdx].pt.y<<endl;
                outlierMatches.push_back(initialMatches[i]);
			}

		}
		else
		{
			outlierMatches.push_back(initialMatches[i]);			
		}
	
	}
	cout << "refine: " << sz << "->" << cnt << endl;
	cout << "outlier size = " << outlierMatches.size() << endl;

	//refine twice
	bool refineF = true;
	if(refineF)
	{
		refPts.clear();
		srcPts.clear();
		sz = refineMatches.size();
		for(int i = 0; i < sz; ++i)
		{
			int rIdx = refineMatches[i].queryIdx;
			int sIdx = refineMatches[i].trainIdx;

			float x = refKeypts[rIdx].pt.x;
			float y = refKeypts[rIdx].pt.y;
			refPts.push_back(Point2f(x,y));

			x = srcKeypts[sIdx].pt.x;
			y = srcKeypts[sIdx].pt.y;
			srcPts.push_back(Point2f(x,y));
		}
		
		if(refPts.size() > 0 && srcPts.size() > 0)
		{
			fundamental = cv::findFundamentalMat(cv::Mat(refPts), cv::Mat(srcPts), CV_FM_8POINT);
		}
		else
		{
			cerr << "fail in calculate fundamental matrix using 8 points." << endl;
		}	
	}

	F = fundamental.clone();
	cout << "F = " << F << endl;

	//refineMatches ��Ӧ re_refKeypts, re_srcKeypts
	refineMatches.clear();
	transfer2DMatches(re_refKeypts, re_srcKeypts, refineMatches);

	//��outliers���ҵ�ĳЩ�õ�
	sz = outlierMatches.size();
	out_refKeypts.clear();
	out_srcKeypts.clear();
	for(int i = 0; i < sz; ++i)
	{
		//float dis = outlierMatches[i].distance;
		int queryIdx = outlierMatches[i].queryIdx;
		int trainIdx = outlierMatches[i].trainIdx;

		int deltay = abs(refKeypts[queryIdx].pt.y - srcKeypts[trainIdx].pt.y);
		int deltax = refKeypts[queryIdx].pt.x - srcKeypts[trainIdx].pt.x;
		//if(deltay > 5 || (deltax < 0 || deltax > 230))
		if(true)
		{
			out_refKeypts.push_back(refKeypts[queryIdx]);
			out_srcKeypts.push_back(srcKeypts[trainIdx]);
		}
	}

	//outlierMatches ��Ӧ out_refKeypts, out_srcKeypts;
	outlierMatches.clear();
	transfer2DMatches(out_refKeypts, out_srcKeypts, outlierMatches);
	cout << "select from outlier matches.." << sz << "->" << outlierMatches.size() << endl; 

}

void MatchesRefiner::Remove()
{
	//removeRebundancy(re_refKeypts, re_srcKeypts);
	//transfer2DMatches(re_refKeypts, re_srcKeypts, refineMatches);
	//ȥ��refineMatches�е��ظ���
	int sz = refineMatches.size();
	removeRebundancy(re_refKeypts, re_srcKeypts);
	cout << "Remove: " << sz << "->" << re_refKeypts.size() << endl;
	transfer2DMatches(re_refKeypts, re_srcKeypts, refineMatches);
}

void MatchesRefiner::Postprocess(const Mat& disp)
{
	//outlierMatches
	vector<KeyPoint> trefKeypoints(0);
	vector<KeyPoint> tsrcKeypoints(0);

	int sz = outlierMatches.size();
	for(int i = 0; i < sz; ++i)
	{
		cv:KeyPoint rpt = out_refKeypts[i];
		cv::KeyPoint spt = out_srcKeypts[i];
		if(abs(rpt.pt.y - spt.pt.y) > 5 || abs(rpt.pt.x - spt.pt.x) > 225 )
		{
			trefKeypoints.push_back(rpt);
			tsrcKeypoints.push_back(spt);
		}
		else
		{
			re_refKeypts.push_back(rpt);
			re_srcKeypts.push_back(spt);
		}
	}
	 cout << "Postprocess: outliers : " << sz << "->" << trefKeypoints.size() << endl;
	 out_refKeypts.clear();
	 out_srcKeypts.clear();
	 copy(trefKeypoints.begin(), trefKeypoints.end(), back_inserter(out_refKeypts));
	 copy(tsrcKeypoints.begin(), tsrcKeypoints.end(), back_inserter(out_srcKeypts));
	 outlierMatches.clear();
	 transfer2DMatches(out_refKeypts, out_srcKeypts, outlierMatches);

	 //refineMatches
	 cout << "Postprocess: refine : " << refineMatches.size() << "->" << re_refKeypts.size() << endl;
	 refineMatches.clear();
	 transfer2DMatches(re_refKeypts, re_srcKeypts, refineMatches);
	
	 sz = refineMatches.size();
	 trefKeypoints.clear();
	 tsrcKeypoints.clear();
	 for(int i = 0; i < sz; ++i)
	 {
		 cv::KeyPoint rpt = re_refKeypts[i];
		 cv::KeyPoint spt = re_srcKeypts[i];

		 int rx = rpt.pt.x;
		 int ry = rpt.pt.y;
		 int d = disp.at<uchar>(ry, rx);
		 int sx = rx - d;
		 int sy = ry;

		 spt.pt.x = sx;
		 spt.pt.y = sy;

		 trefKeypoints.push_back(rpt);
		 tsrcKeypoints.push_back(spt);
	 }
	 re_refKeypts.clear();
	 re_srcKeypts.clear();
	 copy(trefKeypoints.begin(), trefKeypoints.end(), back_inserter(re_refKeypts));
	 copy(tsrcKeypoints.begin(), tsrcKeypoints.end(), back_inserter(re_srcKeypts));
	 refineMatches.clear();
	 transfer2DMatches(re_refKeypts, re_srcKeypts, refineMatches);
}

void MatchesRefiner::showMatches(const ShowFlags Flag, const string& sfn)
{
	Mat matchIm;

	if(Flag == INITIAL_MATCH)
	{
		drawMatches(refIm, refKeypts, srcIm, srcKeypts, initialMatches, matchIm, Scalar::all(-1), Scalar::all(-1),
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
		cout << "initial matches..." << endl;
//		imshow("initial matches", matchIm);
//		waitKey(0);
//		destroyWindow("initial matches");
	}
	else if(Flag == REFINE_MATCH)
	{
		drawMatches(refIm, re_refKeypts, srcIm, re_srcKeypts, refineMatches, matchIm, Scalar::all(-1), Scalar::all(-1),
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
		cout << "refine matches..." << endl;
//		imshow("refine matches", matchIm);
//		waitKey(0);
//		destroyWindow("refine matches");
	}
	else if(Flag == OUTLIER_MATCH)
	{
//		drawMatches(refIm, refKeypts, srcIm, srcKeypts, outlierMatches, matchIm, Scalar::all(-1), Scalar::all(-1),
//			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
		drawMatches(refIm, out_refKeypts, srcIm, out_srcKeypts, outlierMatches, matchIm, Scalar::all(-1), Scalar::all(-1), 
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
		cout << "outlier matches.." << endl;
	}

	if(sfn != "")
		imwrite(sfn ,matchIm);
}

void MatchesRefiner::showKeypoints(const ShowFlags Flag, const string& sfn)
{
	Mat ptsIm;
	switch(Flag)
	{
	case INITIAL_REF_PT: cv::drawKeypoints(refIm, refKeypts, ptsIm); break;
	case INITIAL_SRC_PT: cv::drawKeypoints(srcIm, srcKeypts, ptsIm); break;
	case REFINE_REF_PT : cv::drawKeypoints(refIm, re_refKeypts, ptsIm); break;
	case REFINE_SRC_PT : cv::drawKeypoints(srcIm, re_srcKeypts, ptsIm); break;
	default: break;
	}

//	imshow("keypoints", ptsIm);
//	waitKey(0);
//	destroyWindow("keypoints");

	if(sfn!="")
		imwrite(sfn, ptsIm);
}

void MatchesRefiner::saveInitialMatches(const string& sfn)
{
	saveMatches(refKeypts,srcKeypts, sfn);
	cout << "save " << sfn << endl;
}

void MatchesRefiner::saveRefineResults(const string& sfn)
{
	saveMatches(re_refKeypts, re_srcKeypts, sfn);
	cout << "save " << sfn << endl;
}

void MatchesRefiner::saveOutlierResults(const string& sfn)
{
	saveMatches(out_refKeypts, out_srcKeypts, sfn);
	cout << "save " << sfn << endl;
}