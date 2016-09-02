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

	correcter.initial(folder, transfer, RAlist);

	correcter.Correction(correcter.srcRgbIm, correcter.refRgbIm, sceneFn, folder, srcFn);

	imwrite(folder + "/" + srcFn + "_First_sim.png", correcter.init_sim_corr);
	imwrite(folder + "/" + srcFn + "_First_weight.png", correcter.init_weight_corr);
	cout << "correction done" << endl;

	return 1;
	
}