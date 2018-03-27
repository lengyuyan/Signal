clear all 
close all 
clc
%% 设定fix图像
B = imread('fix.jpg');
B = rgb2gray(B);
center = [1800, 1400]; %按照图像坐标存储
left = [200, 1400]; 
right = [3500, 1400]; 
top = [1800,300];
bottom = [1800,2500];
size = 500;
B_block = cell(5,1);
B_block{1} = B(center(2):center(2) + size,center(1):center(1) + size);
B_block{2} = B(left(2):left(2) + size,left(1):left(1) + size);
B_block{3} = B(right(2):right(2) + size,right(1):right(1) + size);
B_block{4} = B(top(2):top(2) + size,top(1):top(1) + size);
B_block{5} = B(bottom(2):bottom(2) + size,bottom(1):bottom(1) + size);
figure(1)
imshow(B)
hold on
rectangle('Position',[center(1),center(2),size,size],'EdgeColor','r');
rectangle('Position',[left(1),left(2),size,size],'EdgeColor','r');
rectangle('Position',[right(1),right(2),size,size],'EdgeColor','r');
rectangle('Position',[top(1),top(2),size,size],'EdgeColor','r');
rectangle('Position',[bottom(1),bottom(2),size,size],'EdgeColor','r');
title('Image-Fixed')
%% 设定unfix
I = imread('out1.jpg');
I = rgb2gray(I);
%delta = 85;
I_block  = cell(5,1);
I_block{1} = I(center(2):center(2) + size,center(1):center(1) + size);  %center
I_block{2} = I(left(2):left(2) + size,left(1):left(1) + size);          %left
I_block{3} = I(right(2):right(2) + size,right(1):right(1) + size);      %right
I_block{4} = I(top(2):top(2) + size,top(1):top(1) + size);              %top
I_block{5} = I(bottom(2):bottom(2) + size,bottom(1):bottom(1) + size);  %bottom
figure(2)
imshow(I)
hold on
rectangle('Position',[center(1),center(2),size,size],'EdgeColor','r');
rectangle('Position',[left(1),left(2),size,size],'EdgeColor','r');
rectangle('Position',[right(1),right(2),size,size],'EdgeColor','r');
rectangle('Position',[top(1),top(2),size,size],'EdgeColor','r');
rectangle('Position',[bottom(1),bottom(2),size,size],'EdgeColor','r');
title('Image-Moved')
%% 计算I的五个质心
for i = 1:5
    %计算x方向
    sum_x = sum(I_block{i});
    max_x = max(sum_x);
    min_x = min(sum_x);
    x_th = (max_x - min_x) * 0.5 + min_x;
    sum_xth = sum_x(sum_x>x_th);
    x = find(sum_x>x_th);
    Ix = sum(sum_xth.*x)/sum(sum_xth);
    %计算y方向
    sum_y = sum(I_block{i}');
    max_y = max(sum_y);
    min_y = min(sum_y);
    y_th = (max_y - min_y) * 0.5 + min_y;
    sum_yth = sum_y(sum_y>y_th);
    y = find(sum_y>y_th);
    Iy = sum(sum_yth.*y)/sum(sum_yth);
    if i == 1
        Ix = Ix + center(1) - 1;
        Iy = Iy + center(2) - 1;
    elseif i == 2
        Ix = Ix + left(1) - 1;
        Iy = Iy + left(2) - 1;
    elseif i == 3
        Ix = Ix + right(1) - 1;
        Iy = Iy + right(2) - 1;
    elseif i == 4
        Ix = Ix + top(1) - 1;
        Iy = Iy + top(2) - 1;
    elseif i == 5
        Ix = Ix + bottom(1) - 1;
        Iy = Iy + bottom(2) - 1;
    end
    I_point(i,:) = [Ix,Iy];
end
%% 计算B的五个质心
for i = 1:5
    %计算x方向
    sum_x = sum(B_block{i});
    max_x = max(sum_x);
    min_x = min(sum_x);
    x_th = (max_x - min_x) * 0.3 + min_x;
    sum_xth = sum_x(sum_x>x_th);
    x = find(sum_x>x_th);
    Ix = sum(sum_xth.*x)/sum(sum_xth);
    %计算y方向
    sum_y = sum(B_block{i}');
    max_y = max(sum_y);
    min_y = min(sum_y);
    y_th = (max_y - min_y) * 0.3 + min_y;
    sum_yth = sum_y(sum_y>y_th);
    y = find(sum_y>y_th);
    Iy = sum(sum_yth.*y)/sum(sum_yth);
    if i == 1
        Ix = Ix + center(1) - 1;
        Iy = Iy + center(2) - 1;
    elseif i == 2
        Ix = Ix + left(1) - 1;
        Iy = Iy + left(2) - 1;
    elseif i == 3
        Ix = Ix + right(1) - 1;
        Iy = Iy + right(2) - 1;
    elseif i == 4
        Ix = Ix + top(1) - 1;
        Iy = Iy + top(2) - 1;
    elseif i == 5
        Ix = Ix + bottom(1) - 1;
        Iy = Iy + bottom(2) - 1;
    end
   B_point(i,:) = [Ix,Iy];
end
%% 基准值设置
Dis = 18;
pixel_size = 0.00112;
f = 3.31;
D = 600
d = 1/(1/f - 1/D);
dis = Dis * d / D;
dis_pixel = dis / pixel_size;
%% 计算uv
centerB = sum(B_point)/5;
centerI = sum(I_point)/5;
delta_x = centerI(1) - centerB(1) - dis_pixel;
delta_y = centerI(2) - centerB(2);
adjust_v = -(delta_x * pixel_size / f) / pi * 180;
adjust_u = (delta_y * pixel_size / f) / pi * 180;
%% 计算w
kb1 = (B_point(2,2) - B_point(3,2))/(B_point(2,1) - B_point(3,1));
kb2 = -(B_point(5,1) - B_point(4,1))/(B_point(5,2) - B_point(4,2));
angleB1 = atan(kb1)/pi*180;
angleB2 = atan(kb2)/pi*180;
kI1 = (I_point(2,2) - I_point(3,2))/(I_point(2,1) - I_point(3,1));
kI2 = -(I_point(5,1) - I_point(4,1))/(I_point(5,2) - I_point(4,2));
angleI1 = atan(kI1)/pi*180;
angleI2 = atan(kI2)/pi*180;
adjust_w = ((angleI1 - angleB1) + (angleI2 - angleB2))/2;