#pragma once

#include "basic.h"

class CMatch
{
public:
	cv::Point2f srcPt, refPt; 

	CMatch(Point2f rpt, Point2f spt)
	{
		srcPt = spt;
		refPt = rpt;
	}

};

class RWTransfer
{
friend class CCorrecter;

private:
	string sceneFn;

	int invalidrefregion;
	int level;//��ʾlevel������Ѱ��������ɫ

	Mat srcRAWIm, refRAWIm;		//��CV_LOAD_IMAGE_UNCHANGED��������
	Mat srcIm, refIm;			//BGR��ͨ��ͼ��
	Mat srcVisIm, refVisIm;		//�ɼ���ͼ��
	Mat srcDispIm, refDispIm;   //�Ӳ�ͼ��
	Mat srcDispMat, refDispMat; //�Ӳ�ֵ
	
	int srcregionum;
	int refregionum;
	int regionum;
	
	Mat srcSegmentsIm;
	vector<int> srcLabels;                 //�ָ��ǩ
	vector<vector<Point2f>> srcPixels;     //�ָ�����������
	vector<Point2f> srcCenters;            //src�ָ��������������
	vector<Point2f> refCenters;            //ref�ָ�������������
	vector<CMatch> matches;                //ȫ��ƥ��㣨����ȷ��
	vector<CMatch> matchesmore;            //�ָ�ƥ��㣨���ࣩ

	vector<vector<int>> src2refmap;

	//��Ҫ���
	vector<Mat> homographys;
	vector<int> isMatched;     //0,1,2  

	vector<int> refLabels;
	vector<vector<Point2f>> refPixels;	
	Mat refSegmentsIm;

	vector<vector<Point2f>> back_srcPixels;
	vector<int> back_srcLabels;
	Mat back_srcSegmentsIm;

	vector<int> labelmap;//�洢����src��ref�Ķ�Ӧ��ǩ

	vector<Scalar> srcregioncolor;//src��ref��ÿ�������ƽ����ɫ
	vector<Scalar> refregioncolor;

	vector<vector<bool>> srcRAlist;//ͼ���ڲ��������ڽӱ�
	vector<vector<bool>> refRAlist;
	vector<vector<bool>> multisrcRAlist;//�������
	vector<vector<bool>> multirefRAlist;

	vector<int> srcregionflag;//srcͼ������Щ����δ��ƥ�䣬��ƥ�������1��û��ƥ�������-1�����򱻺ϲ���-2
	vector<int> refregionflag;
protected:
	//void labels2pixels(const vector<int>& labels, const int& regionum, vector<vector<Point2f>>& pixels);
	void labels2pixels(const vector<int>& labels, const int& regionum, vector<vector<Point2f>>& pixels);
	void pixels2labels(const vector<vector<Point2f>>& pixels, vector<int>& labels, int regionum);
	
	void calDispMat();

public:

	void initial(const string& folder,  const string& sceneFn,  const string& refFn, const string& srcFn);
	void readMatfromfile(string ImFn, Mat& RAWIm, int rows, int cols, int channels);
	void calRColors(vector<Scalar>& regioncolor, int regionum, vector<vector<Point2f>> Pixels, Mat Im);
	void dealRefNopoint(const string folder, const string refFn);
	void dealSrcNopoint(const string folder, const string refFn, vector<int> regionflag);

	void Recombine(vector<vector<Point2f>>& Pixels, vector<int>& labels, Mat Im, int& rnum);
	void getrefRAList(vector<vector<bool>>& RAList);

	void FirstSrcCombine(const string folder, const string srcFn);
	void FuseSrcNopoint(const string folder, const string srcFn);
	void FuseRefNopoint(const string folder, const string refFn);
	void Matches(const string folder, const string srcFn, const string refFn);
	void matchregions(const string folder, const string srcFn, const string refFn);

	void FuseRegion(vector<int>& regionflag, vector<vector<Point2f>>& Pixels, int regionum, vector<vector<bool>> RAlist, 
	vector<Point2f> Centers, vector<Scalar> regioncolor, vector<int>& Labels);

	int InvalidRegion(vector<int> regionflag);
	void calSegmentsIm(const string sfn, Mat Im, int regionum, vector<vector<Point2f>> Pixels, Mat SegmentsIm, vector<int> regionflag);

	void showSameColor(string sfn, string flag);

	void calRCenters(const string& fn, vector<Point2f>& Centers, int regionum, vector<vector<Point2f>> Pixels, Mat SegmentsIm);
	
	void estimateHomography();
	void project();
	void dealUnMatchedRegion(const int srcLabel, const vector<Point2f>& srcPts, vector<Point2f>& refPts);

	void calRefSegmentsIm(const string sfn = "");
	void saveRefLabels(const string& sfn);

	// saving regions in src and ref
	void saveRegions();
		
	// calculate regions adajcent list
	void calRAList(vector<vector<bool>>& ralist, Mat Im, int regionum, vector<vector<Point2f>> Pixels, string str, vector<int> Labels);

	void calmultilevelRA(vector<vector<bool>> ralist, vector<vector<bool>>& multiralist, int multinum);

	void backProject();
	void calBackSrcSegmentsIm(const string sfn = "");
	void calCorrespondenceError(const string sfn = "");
	void saveBackSrcLabels(const string sfn = "");

	//using disparity information
	void projectD();
	bool backProjectD();	

	void FuseRef(const string folder, const string refFn);
	void SaveRegionFlag(string sfn, Mat& mark, Mat Im, vector<vector<Point2f>> Pixels, vector<int> regionflag);
};