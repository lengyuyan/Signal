function [x_offset, y_offset] = getOffcenter(img, pixel_size)
%get off-center distance
%============================================
%Author:Chame
%Date:2017.3.24
%============================================
%Input
% img: input image
% pixel_size: the size of CCD pixel
%--------------------------------------------
%Output
% x_offset, y_offset: horizontal(x) offset and vertical(y) offset
%============================================
narginchk(2,2);
if size(img, 3) == 3
    img = rgab2gray(img);
end
if max(img(:)) > 1
    img = im2double(img);
end

[y, x] = size(img);
center_y = round(y / 2);
center_x = round(x / 2);
%% 检测角点
Points = detectHarrisFeatures(img);
Locs = Points.Location;
if size(Locs, 1) ~= 52;
   %error('get wrong number of points, the central point will be wrong !');
   Locs = [center_y, center_x];
end
%% 计算实测图像与标准图像的像素偏移
center_locs = mean(Locs);
x_distance = center_locs(2) - center_x;
y_distance = center_locs(1) - center_y;
%% 转换为物空间移动距离
x_offset = x_distance * pixel_size;
y_offset = y_distance * pixel_size;
end