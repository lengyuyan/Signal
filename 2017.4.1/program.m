%读取三张图来拟合二次曲线
clear all;
close all;
clc;

ori_num = 0.562;%读取的图像所在的位置
p_num = 0.622;
m_num = 0.502;
deltaZ_p = ori_num - p_num;
deltaZ_m = ori_num - m_num;
pixel_size = 0.00112;%像元大小（mm）
std_error = 3e-2;
bound_wid = 1;%裁剪的图像边缘像素数
bound_hei = 1;
tic;
img_ori = imread(['E:\MATLAB\镜头矫正\2017.4.1\uv\9\',num2str(ori_num),'.bmp']);
img_p = imread(['E:\MATLAB\镜头矫正\2017.4.1\uv\9\',num2str(p_num),'.bmp']);
img_m = imread(['E:\MATLAB\镜头矫正\2017.4.1\uv\9\',num2str(m_num),'.bmp']);
img_ori = rgb2gray(img_ori);
img_ori = im2double(img_ori);
img_ori = imresize(img_ori, [600, 800]);
img_ori = img_ori(bound_hei:end-bound_hei, bound_wid:end-bound_wid);
%
img_p  = rgb2gray(img_p);
img_p  = im2double(img_p );
img_p  = imresize(img_p , [600, 800]);
img_p  = img_p (bound_hei:end-bound_hei, bound_wid:end-bound_wid);

img_m  = rgb2gray(img_m);
img_m  = im2double(img_m);
img_m  = imresize(img_m , [600, 800]);
img_m  = img_m (bound_hei:end-bound_hei, bound_wid:end-bound_wid);
    
[deltaZ, x_angle, y_angle] = getState(img_ori, img_p, img_m, deltaZ_p, deltaZ_m, pixel_size);
%}
%[x, y] = getOffcenter(img_ori, pixel_size);
adjust_z = deltaZ;
%adjust_x = -x;
%adjust_y = -y;
adjust_u = -x_angle;
adjust_v = -y_angle;
toc;
%{
while x_std && y_std && z_std && u_std && v_std
    img_ori = imread('original image.bmp');
    img_p = imread('img_p.bmp');
    img_m = imread('img_m.bmp');
    
    [deltaZ, x, y, x_angle, y_angle] = getState(img_ori, img_p, img_m, deltaZ_p, deltaZ_m, pixel_size);
    adjust_z = deltaZ;
    adjust_x = -x;
    adjust_y = -y;
    adjust_u = -x_angle;
    adjust_v = -y_angle;
    x_std = adjust_x - last_x > std_error;
    y_std = adjust_y - last_y > std_error;
    z_std = adjust_z - last_z > std_error;
    u_std = adjust_u - last_u > std_error;
    v_std = adjust_v - last_v > std_error;
end
%}
%
[x_lap_o, y_lap_o, x_pt_o,  y_pt_o] = getAllLaplace(img_ori);
[x_lap_p, y_lap_p, x_pt_p,  y_pt_p] = getAllLaplace(img_p);
[x_lap_m, y_lap_m, x_pt_m,  y_pt_m] = getAllLaplace(img_m);
%% 显示x方向的拟合曲线
dx = (deltaZ_p-deltaZ_m)/100;
deltaZ_X = zeros(size(x_lap_o));
figure;
for i = 1:length(x_lap_o)
    p = polyfit([deltaZ_m, 0, deltaZ_p], [x_lap_m(i), x_lap_o(i), x_lap_p(i)], 2);
    lap = polyval(p, [deltaZ_m:dx:deltaZ_p]);
    ind = find(lap == max(lap));
    deltaZ_X(i) = deltaZ_m + (ind-1) * dx;
    subplot(1,3,i);
    plot([deltaZ_m:dx:deltaZ_p],lap);hold on;
    plot([deltaZ_m, 0, deltaZ_p],[x_lap_m(i), x_lap_o(i), x_lap_p(i)],'.','color',[1,0,0]);
end
%% 显示y方向的拟合曲线
dy = (deltaZ_p-deltaZ_m)/100;
deltaZ_Y = zeros(size(x_lap_o));
figure;
for i = 1:length(x_lap_o)
    p = polyfit([deltaZ_m, 0, deltaZ_p], [y_lap_m(i), y_lap_o(i), y_lap_p(i)], 2);
    lap = polyval(p, [deltaZ_m:dy:deltaZ_p]);
    ind = find(lap == max(lap));
    deltaZ_Y(i) = deltaZ_m + (ind-1) * dy;
    subplot(1,3,i);
    plot([deltaZ_m:dx:deltaZ_p],lap);hold on;
    plot([deltaZ_m, 0, deltaZ_p],[y_lap_m(i), y_lap_o(i), y_lap_p(i)],'.','color',[1,0,0]);
end
%}

