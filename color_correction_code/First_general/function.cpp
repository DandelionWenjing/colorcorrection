#include "function.h"

void calMask(const vector<Point2f>& pts, Mat& msk)
{
	int width = msk.size().width;
	int height = msk.size().height;
	for (int j = 0; j < pts.size();++j)
	{
		if(pts[j].y >= 0 && pts[j].y < height &&
			pts[j].x >= 0 && pts[j].x < width)
			msk.at<uchar>(pts[j].y, pts[j].x) = 255;
		else
			continue;
	}
}

void readImage(const string& fn, Mat& im, int METHOD)
{
	cout << "read " << fn << endl;
	im = imread(fn, METHOD);

	if(im.data == nullptr)
	{
		cerr << "failed to open " << fn << endl;
		return ;
	}
}

void readLabels(const string& fn, int& regionum, vector<int>& labels)
{
	fstream fin(fn.c_str(), ios::in);
	if (fin.is_open() == false)
	{
		cout << "failed to open " << fn << endl;
		return ;
	}

	int width, height;
	fin >> height >> width ;

	int sz = height * width;
	regionum = 0;

	labels.resize(sz);
	for (int i = 0; i < sz; ++ i)
	{
		fin >> labels[i];
		regionum = ( (labels[i] > regionum) ? labels[i] : regionum );		
	}
	regionum ++;

	fin.close();
}

void readMatches(const string& fn, vector<CMatch>& matches, Mat refVisIm, Mat srcVisIm)
{
	fstream fin(fn.c_str(), ios::in);
	if (fin.is_open() == false)
	{
		cout << "failed to open " << fn << endl;
		return ;
	}

	int sz;
	fin >> sz;

	for (int i = 0; i < sz; ++ i)
	{
		int rx, ry, sx, sy;
		fin >> rx >> ry >> sx >> sy;

		if(refVisIm.at<char>(ry, rx) == 0 && srcVisIm.at<char>(sy, sx) == 0) {
			//refPt. srcPt
			CMatch m(Point2f(rx, ry), Point2f(sx, sy));
			matches.push_back(m);	
		}
	}

	cout << matches.size() << " matches." << endl;
}

void saveLabels(const string& fn, const int& width, const int& height, const vector<int>& labels)
{
	fstream fout(fn.c_str(), ios::out);
	if (fout.is_open() == false)
	{
		cout << "failed to write " << fn << endl;
		return ;
	}

	fout << height << ' ' << width << endl;
	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			fout << labels[j*width+i] << ' ';
		}
		fout << endl;
	}
	fout.close();
}

void Filter(const Mat& im, const int& regionum, const vector<vector<cv::Point2f>>& segments, Mat& filteredIm, vector<int> regionflag)
{
	int width = im.size().width;
	int height = im.size().height;
	int sz = width * height;

	//����filteredSrc
	filteredIm = Mat::zeros(height, width, CV_8UC3);
	for(int i = 0; i < regionum; ++i)
	{
		if(regionflag[i] != -1 && regionflag[i] != -2)
		{
		    const vector<Point2f>& pts = segments[i];
			//����ĳ�������ڵ���ɫֵ
		    Mat msk = Mat::zeros(im.size(), CV_8UC1 );
		    calMask(pts, msk);

		    Scalar mean, stddev;
		    cv::meanStdDev(im, mean, stddev, msk);

		    for(int j = 0; j < pts.size(); ++j)
		    {
			    filteredIm.at<Vec3b>(pts[j].y, pts[j].x)[0] = mean.val[0];
			    filteredIm.at<Vec3b>(pts[j].y, pts[j].x)[1] = mean.val[1];
			    filteredIm.at<Vec3b>(pts[j].y, pts[j].x)[2] = mean.val[2];
		    }	
		}
	}
}

//Mat mergeCols(Mat A, Mat B)
//{
//    int totalCols = A.cols + B.cols;
// 
//    Mat mergedDescriptors(totalCols, A.rows, A.type());
//    Mat submat = mergedDescriptors.rowRange(0, A.cols);//rowRange
//    A.copyTo(submat);
//    submat = mergedDescriptors.rowRange(A.rows, totalCols);
//    B.copyTo(submat);
//    return mergedDescriptors;
//}

//GetColorForRegion(Im, srcPixels, refPixels, ref2srcmatch, refcorrsrc);
void GetColorForRegion(const Mat& im, vector<vector<Point2f>> srcPixels, vector<vector<Point2f>> refPixels, vector<vector<int>> lasts2r, Mat& markref, Mat& marksrc)
{
	int width = im.size().width;
	int height = im.size().height;
	int sz = width * height;

	//��ʾ��Ǻ��Դͼ��Ͳο�ͼ��
	marksrc = Mat::zeros(height, width, CV_8UC3);
	markref = Mat::zeros(height, width, CV_8UC3);

	int regionum = lasts2r.size();
	int refr, srcr;
	Scalar color;
	CvRNG rng;  
	for(int i = 0; i < regionum; ++i)
	{
		if(lasts2r[i].size() == 0)
		{
			continue;//��ref���ںϣ����п���ĳЩ�������٣���ʱ���������src��������������ֻ����û�ж�Ӧ������
		}
		srcr = i;
		refr = lasts2r[i][0];//ref�����Ӧ��src�������п���û�еģ���������û�йؼ��㣿����֮ǰ���ں��ˣ�Ӧ��ÿ�������йؼ����

		//vector<Point2f>& ptr = refPixels[refr];//����
		//vector<Point2f>& pts = srcPixels[srcr];
		
		//���������ɫ
		
        rng= cvRNG(cvGetTickCount());  
		for(int j = 0; j < 3; ++j)
		{
			color.val[j] = cvRandInt(&rng)%256;//Ϊÿһͨ������һ�������ɫ��Ϊ0-255֮���������
		}
		//�ø������ɫ����Ӧ����
		for(int j = 0; j < refPixels[refr].size(); ++j)
		{
			markref.at<Vec3b>(refPixels[refr][j].y, refPixels[refr][j].x)[0] = color.val[0];
			markref.at<Vec3b>(refPixels[refr][j].y, refPixels[refr][j].x)[1] = color.val[1];
			markref.at<Vec3b>(refPixels[refr][j].y, refPixels[refr][j].x)[2] = color.val[2];
		}
		for(int j = 0; j < srcPixels[srcr].size(); ++j)
		{
			marksrc.at<Vec3b>(srcPixels[srcr][j].y, srcPixels[srcr][j].x)[0] = color.val[0];
			marksrc.at<Vec3b>(srcPixels[srcr][j].y, srcPixels[srcr][j].x)[1] = color.val[1];
			marksrc.at<Vec3b>(srcPixels[srcr][j].y, srcPixels[srcr][j].x)[2] = color.val[2];
		}
	}
}


void meanshiftFilter(const Mat& im, const int& regionum, const vector<vector<cv::Point2f>>& segments, Mat& filteredIm)
{
	int width = im.size().width;
	int height = im.size().height;
	int sz = width * height;

	//����filteredSrc
	filteredIm = Mat::zeros(height, width, CV_8UC3);
	for(int i = 0; i < regionum; ++i)
	{
		const vector<Point2f>& pts = segments[i];
		
		//����ĳ�������ڵ���ɫֵ
		Mat msk = Mat::zeros(im.size(), CV_8UC1 );
		calMask(pts, msk);

		Scalar mean, stddev;
		cv::meanStdDev(im, mean, stddev, msk);

		for(int j = 0; j < pts.size(); ++j)
		{
			filteredIm.at<Vec3b>(pts[j].y, pts[j].x)[0] = mean.val[0];
			filteredIm.at<Vec3b>(pts[j].y, pts[j].x)[1] = mean.val[1];
			filteredIm.at<Vec3b>(pts[j].y, pts[j].x)[2] = mean.val[2];
		}
	}
}