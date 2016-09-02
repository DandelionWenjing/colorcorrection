#pragma once

#include "basic.h"
#include "transfer.h"


class RWTransfer;

class CCorrecter
{
private:
	bool isGRAY;                  //��ʼ������ͼ��ͨ���Ƿ�Ϊ1
	Mat srcIm, refIm;
	Mat srcVisIm, refVisIm;
	Mat srcValidIm, refValidIm;   // ��Ч����srcVisIm, refVisIm�ķ�

	int regionum;
	vector<vector<Point2f>> srcPixels, refPixels;
	vector<int> srcLabels, refLabels;
	vector<vector<bool>> RAList;   //�ڽ������
	vector<Point2f> srcCenters;
	vector<int> labelmap;

	//��Ҫ��������
	//src��ֵ, ref��ֵ��ref����/src����
	Scalar srcMeanG, refMeanG, srcStddevG, refStddevG, rsFactorG; 
	vector<Scalar> srcMeans, refMeans;
	vector<Scalar> srcStddevs, refStddevs;
	vector<Scalar> rsFactors;
	vector<Scalar> IMmap;
	vector<Scalar> srcregioncolor;//src��ref��ÿ�������ƽ����ɫ
	vector<Scalar> refregioncolor;

public: 
	Mat srcinresult;
	Mat srcrIm, refrIm;
	Mat srcsIm, refsIm;
	Mat srcLabIm, refLabIm;
	Mat srcRgbIm, refRgbIm;
	Mat correctedSrcIm, wcorrectedSrcIm;
	Mat reflect_sim_corr, shading_sim_corr, init_sim_corr, reflect_weight_corr, shading_weight_corr, init_weight_corr, last_in_sim_corr, last_in_weight_corr;
	Mat srcrLabIm, refrLabIm, srcsLabIm, refsLabIm;       // Lab�ռ��µ�ͼ��
	Mat final_correctedSrcIm, final_wcorrectedSrcIm;    //����ʼͼ��Ϊ1������Ҫ����ת��
	
public:
	void initial(const string folder, const string srcsFn, const string refsFn, const string srcrFn, const string refrFn, const RWTransfer& rwtranfer, const vector<vector<bool>>& ralist);
	void calGlobalParameters(Mat srcIm, Mat refIm, string mode);
	void calLocalParameters(Mat srcLabIm, Mat refLabIm, string mode);

	//global color transfer algorithims
	//��srcIm������Ч������msk��ʹ��srcMean, refMean, rsFactor������ɫУ��
	void correct(const Mat& msk, const Scalar& srcMean, const Scalar& refMean, const Scalar& rsFactor, Mat& sLabIm, Mat srcLabIm, string mode);

	void simpleCorrection(Mat srcLabIm, string mode);
	void weightedCorrection(Mat srcLabIm, string mode);
	double calMinDist(const Point2f pt, const vector<Point2f>& pixels);

	void saveWeightedMap(const string& sfn, Mat srcLabIm);
	void showCorrectedResult();
	void pixels2labels(const vector<vector<Point2f>>& pixels, vector<int>& labels);
	void Correction(Mat srcLabIm, Mat refLabIm, string sceneFn, string folder, string srcFn, string mode);
	void readMatfromfile(string ImFn, Mat& RAWIm, int rows, int cols, int channels);
	void Combine(Mat reflectance, Mat shading, string resultFn, string folder);
	vector<double> getIMmap(int y, int x, Mat srcIm, string mode);
	void Showlayers(Mat layer, string mode, string folder, string resultFn);
};
