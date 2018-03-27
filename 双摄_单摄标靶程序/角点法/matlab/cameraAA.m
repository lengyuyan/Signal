clc
clear all
close all
%% 图像预处理
B = imread('FIX.jpg');

B = rgb2gray(B);
B = im2double(B);
B = B(600:2100,1000:3100);
B = imresize(B,0.2);
I = imread('UNFIX.jpg');

I = rgb2gray(I);
I = im2double(I);
I = I(600:2100,1000:3100);%前面为高
I = imresize(I,0.2);
figure(1); imshow(B); hold on; figure(2); imshow(I); hold on
%% 基准距离设置
Dis = 18;
pixel_size = 0.00112 * 5;
f = 3.31;
D = 600
d = 1/(1/f - 1/D);
dis = Dis * d / D;
dis_pixel = dis / pixel_size;
%% Harris角点检测,右下角角点
HarrisB = detectHarrisFeatures(B);
TempB = HarrisB.Location;
PointB = [];
HarrisI = detectHarrisFeatures(I);
TempI = HarrisI.Location;
PointI = [];
for i = 1:20
    if B(ceil(TempB(i,2))-10,ceil(TempB(i,1))) < 0.15
        PointB = [PointB;TempB(i,:)];
    end
    if I(ceil(TempI(i,2))-10,ceil(TempI(i,1))) < 0.15
        PointI = [PointI;TempI(i,:)];
    end
end
figure(1);plot(PointB(:,1),PointB(:,2),'.');figure(2);plot(PointI(:,1),PointI(:,2),'.')
for i = 1:5
    if PointB(i,1)<160 && PointB(i,2)<100
        Btl= PointB(i,:);
    elseif PointB(i,1)>300 && PointB(i,2)<160
        Btr= PointB(i,:);
    elseif PointB(i,1)<160 && PointB(i,2)>200
        Bbl = PointB(i,:);
    elseif PointB(i,1)>300 && PointB(i,2)>200
        Bbr= PointB(i,:);
    else
        Bc= PointB(i,:);
    end
end
for i = 1:5
    if PointI(i,1)<160 && PointI(i,2)<100
        Itl= PointI(i,:);
    elseif PointI(i,1)>300 && PointI(i,2)<160
        Itr= PointI(i,:);
    elseif PointI(i,1)<160 && PointI(i,2)>200
        Ibl = PointI(i,:);
    elseif PointI(i,1)>300 && PointI(i,2)>200
        Ibr= PointI(i,:);
    else
        Ic= PointI(i,:);
    end
end
%% 计算w
%bw角度
kb1 = (Btl(2)-Btr(2))/(Btl(1)-Btr(1));
kb2 = (Bbl(2)-Bbr(2))/(Bbl(1)-Bbr(1));
kb3 = -(Btr(1)-Bbr(1))/(Btr(2)-Bbr(2));
kb4 = -(Btl(1)-Bbl(1))/(Btl(2)-Bbl(2));
kB = (kb1 + kb2 + kb3 + kb4)/4;
angleB = atan(kB)/pi*180;
%lw角度
kI1 = (Itl(2)-Itr(2))/(Itl(1)-Itr(1));
kI2 = (Ibl(2)-Ibr(2))/(Ibl(1)-Ibr(1));
kI3 = -(Itr(1)-Ibr(1))/(Itr(2)-Ibr(2));
kI4 = -(Itl(1)-Ibl(1))/(Itl(2)-Ibl(2));
kI = (kI1 + kI2 + kI3 + kI4)/4;
angleI = atan(kI)/pi*180;
adjust_w = angleI - angleB;
%% 计算uv
centerB = sum(PointB)/5;
centerI = sum(PointI)/5;
delta_x = centerI(1) - centerB(1) - dis_pixel;
delta_y = centerI(2) - centerB(2);
adjust_v = -(delta_x * pixel_size / f) / pi * 180;
adjust_u = (delta_y * pixel_size / f) / pi * 180;