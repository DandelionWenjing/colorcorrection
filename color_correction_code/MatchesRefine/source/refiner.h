#pragma once

#include "common.h"

//#define INITIAL_MATCH 5
//#define INITIAL_REF_PT 0
//#define INITIAL_SRC_PT 1
//#define REFINE_REF_PT 3
//#define REFINE_SRC_PT 4
enum ShowFlags {INITIAL_MATCH = 0, REFINE_MATCH, OUTLIER_MATCH, 
				INITIAL_REF_PT = 0, INITIAL_SRC_PT, REFINE_REF_PT, REFINE_SRC_PT };

class MatchesRefiner
{

private:
	Mat srcIm, refIm;
	int imgwidth, imgheight;
		
	vector<KeyPoint> refKeypts;
	vector<KeyPoint> srcKeypts;
	
	//DMatch��queryIdx��trainIdx��������initial keypoints
	vector<DMatch> initialMatches;
	vector<DMatch> outlierMatches;
	//DMatch��queryIdx, trainIdx������refine keypoints
	vector<DMatch> refineMatches;    
	

	//refine result
	vector<KeyPoint> re_refKeypts;
	vector<KeyPoint> re_srcKeypts;

	//outlier points
	vector<KeyPoint> out_refKeypts;
	vector<KeyPoint> out_srcKeypts;

	Mat F;

protected:
	void readImage(const string& refFn, const string& srcFn);
	void readKeypoints(const string& matchFn);
	void removeRebundancy(vector<KeyPoint>& refpts, vector<KeyPoint>& srcpts);
	void transfer2DMatches(const vector<KeyPoint>& refpts, const vector<KeyPoint>& srcpts, vector<DMatch>& dmatches);

	void saveMatches(const vector<KeyPoint>& refpts, const vector<KeyPoint>& srcpts, const string& fn);
public:
	void Initial(const string& refFn, const string& srcFn, const string& matchFn);
	
	//ʹ��RANSAC��������������F��Ȼ�����Ż�matches
	void Refine();
	//�Ƴ�refine matches�е�����
	void Remove();
	//����disparity��Ϣ��refineMatches, outlierMatches�����Ż�
	void Postprocess(const Mat& disp);

	//�������� �����Ż����matches, �Լ����
	void showMatches(const ShowFlags Flag, const string& sfn = "");
	void showKeypoints(const ShowFlags Flag, const string& sfn = "");

	void saveInitialMatches(const string& sfn);
	void saveRefineResults(const string& sfn);
	void saveOutlierResults(const string& sfn);
	
};