#include"common.h"

int main(int argc, char* argv[]) {

	string folder = string(argv[1]);
	string sceneFn = string(argv[2]);
	string refFn = string(argv[3]);;
	string srcFn = string(argv[4]);
	string dispFns = string(argv[5]);//��׼�Ӳ�
	string dispFn = string(argv[6]);//Ҫ���Ƚ��Ӳ�

	folder = folder + "/" + sceneFn;
	string refVisImFn = folder + "/" + "visibility_" + refFn + ".png";
	Mat refVisIm = imread(refVisImFn, CV_LOAD_IMAGE_GRAYSCALE);//�Ӳ�ɼ���
	string disprFn = folder + "/" + dispFn + ".png"; 
	Mat dispr = imread(disprFn, CV_LOAD_IMAGE_GRAYSCALE);
	string dispsFn = folder + "/" + dispFns + ".png";
	Mat disps = imread(dispsFn, CV_LOAD_IMAGE_GRAYSCALE);

	int rows = disps.rows;
	int cols = disps.cols;

	int errornum = 0;
	int allnum = 0;
	allnum = rows * cols;
	for(int y = 0; y < rows; ++y) {
		for(int x = 0; x < cols; ++x) {
			if(refVisIm.at<uchar>(y, x) == 0) {//�����λ��Ϊ�ɼ��������Ӳ����
				int r = dispr.at<uchar>(y, x);
				int s = disps.at<uchar>(y, x);
				if(abs(dispr.at<uchar>(y, x)-disps.at<uchar>(y, x)) > 10.0) {
					errornum++;
				}
			}
		}
	}
	float badpixels = errornum*1.0 / allnum*1.0;
	cout << dispFn << " badpixels = " << badpixels<< endl;
	return 0;
}