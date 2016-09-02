function matchc(image1, image2, imgname)%����ͼ��1��ͼ��2�ĵ�ַ���Լ�ͼ���ļ��� 
str1='H:\intrinsic_image_work_record\data\';
%mkdir(str1,imgname);

%fa , fb�д��˹ؼ������꣬ǰ������x,y
imagea = imread(image1);
imageb = imread(image2);
[imgh, imgw, channels] = size(imagea);

%PeakThresh:0-30 edge_thresh:3.5-10 ����0 �� 10 ����࣬����һͷ�ߵ����
imga = single(rgb2gray(imagea));
imgb = single(rgb2gray(imageb));

% fra1 = vl_sift(imga, 'PeakThresh', 10);
% fra2 = vl_sift(imga, 'edgethresh', 7.5);
% [fa1, da1] = vl_sift(imga, 'frames', fra1);
% [fa2, da2] = vl_sift(imga, 'frames', fra2);
% da = [da1 da2];
% fa = [fa1 fa2];
% 
% frb1 = vl_sift(imgb, 'PeakThresh', 10);
% frb2 = vl_sift(imgb, 'edgethresh', 7.5);
% [fb1, db1] = vl_sift(imgb, 'frames', frb1);
% [fb2, db2] = vl_sift(imgb, 'frames', frb2);
% db = [db1 db2];
% fb = [fb1 fb2];

[fa, da] = vl_sift(imga);
[fb, db] = vl_sift(imgb);

[pathstr1, name1, ext1] = fileparts(image1);%�ֱ�õ�·�������ƺͺ�׺��
[pathstr2, name2, ext2] = fileparts(image2);

%�õ�ƥ���
[matches, scores] = vl_ubcmatch(da, db) ;

[drop, perm] = sort(scores, 'descend');
matches = matches(:, perm);
scores = scores(perm);

[height,matchnum] = size(matches);
[feaheight,featurenum1] = size(fa);
[feaheight,featurenum2] = size(fb);
%%-----------
features = '\features_';
match_string = '\matches_';
resultname = '\result_';
txt = '.txt';

%----����ƥ������������洢·��----
fidpath1 = [str1,imgname,features,name1,txt];
fidpath2 = [str1,imgname,features,name2,txt];
matchpath = [str1,imgname,match_string,name2,txt];
resultpath = [str1, imgname, resultname, name2, ext1];

fid1 = fopen(fidpath1,'wt');
fid2 = fopen(fidpath2,'wt');
fid_match = fopen(matchpath,'wt');

%%------------

for i = 1: matchnum
    xa(1,i) = fix( fa(1, matches(1,i)));
    ya(1,i) = fix( fa(2, matches(1,i)));
   
    xb(1,i) = fix( fb(1, matches(2,i)));
    yb(1,i) = fix( fb(2, matches(2,i)));
    
end

%%----��ȥ���Եĵ�----
k = 1;
for i = 1:matchnum
    if(abs(ya(1,i)-yb(1,i))<60 && abs(xa(1,i)-xb(1,i))<1000)
        img1x(1,k) = xa(1,i);
        img1y(1,k) = ya(1,i);
        img2x(1,k) = xb(1,i);
        img2y(1,k) = yb(1,i);
        k=k+1;
    end
end
%----------------------
%-------------show match result---------------
img = [imagea, imageb];
figure;
imshow(img);
hold on
for i = 1:k-1
    plot([img1x(1,i), img2x(1, i) + imgw], [img1y(1,i), img2y(1,i)]);
end
saveas(gcf, resultpath);
hold off
%----------------------
fprintf(fid1,'%d\n',featurenum1);
fprintf(fid2,'%d\n',featurenum2);
fprintf(fid_match,'%d\n',k-1);

for i = 1:featurenum1
    fprintf(fid1,'%d %d\n',fix(fa(1,i)), fix(fa(2,i)));
end

for i = 1:featurenum2
    fprintf(fid2,'%d %d\n',fix(fb(1,i)), fix(fb(2,i)));
end

for i = 1:k-1
    fprintf(fid_match,'%d %d %d %d\n', img1x(1,i), img1y(1,i), img2x(1,i), img2y(1,i));
end

fclose(fid1);
fclose(fid2);
fclose(fid_match);
hold off;



