%拟合六个位置的刃边的拉普拉斯值-离焦量曲线
close all;
clear all;
clc;
num_img = 9;
I = cell(num_img,1);
bound_wid = 1;
bound_hei = 1;
%x = [0.45, 0.5, 0.525, 0.55, 0.554, 0.5595, 0.575, 0.6, 0.65];
x = linspace(1, num_img, num_img)' .* 0.003 + 0.547;
for i = 1:num_img
    num = x(i);
    filename = ['E:\MATLAB\镜头矫正\2017.4.1\uv\11\',num2str(num), '.bmp'];
    I{i} = imread(filename);
    I{i} = rgb2gray(I{i});
    I{i} = imresize(I{i}, [600, 800]);
    I{i} = I{i}(bound_hei:end-bound_hei, bound_wid:end-bound_wid);%将边缘非靶标区域截掉
end
%
lap = cell(num_img,2);
for i = 1:num_img
    [lap{i, 1}, lap{i, 2}] = getAllLaplace(I{i});
end
y = cell(6, 1);
for i = 1:num_img
    y{1}(i) = lap{i, 1}(2);
    y{2}(i) = lap{i, 2}(2);
    y{3}(i) = lap{i, 1}(1);
    y{4}(i) = lap{i, 1}(3);
    y{5}(i) = lap{i, 2}(1);
    y{6}(i) = lap{i, 2}(3);
end
figure;
subplot(2,3,1);
plot(x, y{1});title('center_y');
subplot(2,3,2);
plot(x, y{2});title('center_x');
subplot(2,3,3);
plot(x, y{3});title('left');
subplot(2,3,4);
plot(x, y{4});title('right');
subplot(2,3,5);
plot(x, y{5});title('top');
subplot(2,3,6);
plot(x, y{6});title('buttom');
ind1 = x(y{1} == max(y{1}));
ind2 = x(y{2} == max(y{2}));
ind3 = x(y{3} == max(y{3}));
ind4 = x(y{4} == max(y{4}));
ind5 = x(y{5} == max(y{5}));
ind6 = x(y{6} == max(y{6}));
%}