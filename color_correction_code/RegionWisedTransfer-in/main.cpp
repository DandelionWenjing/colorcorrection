#include "transfer.h"
#include "correction.h"

int main(int argc, char* argv[])
{
	//H:\RQing_code\data_cc\Art Art view1 view5E2
	if(argc != 5)
	{
		//�ļ���·���� �����������ƣ� �ο�ͼ�����ƣ� ԭͼ������
		//../../data_cc Art view1 view5E2
		cout << "Usage: RegionWisedTransfer.exe folder sceneFn refFn srcFn" << endl; 
		exit(-1);
	}

	string folder  = string(argv[1]);//·��
	string sceneFn = string(argv[2]);//ͼ������
	string refFn   = string(argv[3]);//�ο�ͼ����
	string srcFn   = string(argv[4]);//Ŀ��ͼ����
	folder = folder + "/" + sceneFn;

	string refrFn = refFn+"-r";//�ο�ͼreflectance����
	string refsFn = refFn+"-s";//�ο�ͼshading����
	string srcrFn = srcFn+"-r";//Ŀ��ͼreflectance����
	string srcsFn = srcFn+"-s";//Ŀ��ͼshading����

	RWTransfer  transfer;

	transfer.initial(folder, sceneFn, refFn, srcFn);//��ʼ��ͼ���Ӳ�ɼ��ԡ��ָ�����ƥ����������ƽ����ɫ

	transfer.FirstSrcCombine(folder, srcFn);//��ĳһ��ref�����Ӧ�˶��src�����Ҷ�Ӧ���������1����Ҫ�������ںϣ�ʹ��ÿ��ref������һ��ȷ����src����

   	transfer.FuseSrcNopoint(folder, srcFn);//���src��ĳ����û��ƥ��㣬���Ҷ��������ڸ�����ɫ�;���������������ں�

	transfer.Matches(folder, srcFn, refFn);//src��Ӧref,�������һ��src�����Ӧ���ref�������ں�

	transfer.showSameColor(folder, "b");

	transfer.FuseRef(folder, refFn);

	transfer.showSameColor(folder, "a");

	vector<vector<bool>> RAlist;
	transfer.getrefRAList(RAlist);

	CCorrecter correcter;

	correcter.initial(folder, srcsFn, refsFn, srcrFn, refrFn, transfer, RAlist);
	/*correcter.Showlayers(correcter.refrIm, "r", folder, "refrIm");
	correcter.Showlayers(correcter.refsIm, "s", folder, "refsIm");
	correcter.Showlayers(correcter.srcrIm, "r", folder, "srcrIm");
	correcter.Showlayers(correcter.srcsIm, "s", folder, "srcsIm");*/

	string mode;
	mode = "i";
	correcter.Correction(correcter.srcRgbIm, correcter.refRgbIm, sceneFn, folder, srcFn, mode);
	//mode = "r";
	//correcter.Correction(correcter.srcrIm, correcter.refrIm, sceneFn, folder, srcFn, mode);
	//mode = "s";
	//correcter.Correction(correcter.srcsIm, correcter.refsIm, sceneFn, folder, srcFn, mode);

	//string srcSimResultFn = "src_sim_IN";
	//string srcWeightResultFn = "src_weight_IN";

	//
	////correcter.Combine(correcter.reflect_sim_corr, correcter.shading_sim_corr, srcSimResultFn, folder);
	//correcter.Combine(correcter.reflect_weight_corr, correcter.shading_weight_corr, srcWeightResultFn, folder);

	//mode = "l";
	//cout << "l" <<endl;
	//correcter.Correction(correcter.srcinresult, correcter.refRgbIm, sceneFn, folder, srcFn, mode);
	//imwrite(folder + "/last_sim_IN.png", correcter.last_in_sim_corr);
	//imwrite(folder + "/" + srcFn+ "_IN.png", correcter.last_in_weight_corr);
	imwrite(folder + "/src_sim_GE.png", correcter.init_sim_corr);
	imwrite(folder + "/" + srcFn + "_IRW.png", correcter.init_weight_corr);

	return 1;
	
}