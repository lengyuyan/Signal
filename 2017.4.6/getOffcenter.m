function [x_offset, y_offset] = getOffcenter(img, standard_img, pixel_size)
%get off-center distance
%============================================
%Author:Chame
%Date:2017.3.24
%============================================
%Input
% img: input image
% standard_img: standard image;
% pixel_size: the size of CCD pixel
%--------------------------------------------
%Output
% x_offset, y_offset: horizontal(x) offset and vertical(y) offset
%============================================
narginchk(5,5);
%% 检测角点
fixedPoints = detectHarrisFeatures(standard_img);
movingPoints = detectHarrisFeatures(img);

fixedLocs = fixedPoints.Location;
movingLocs = movingPoints.Location;
%% 计算实测图像与标准图像的像素偏移
x_distance = mean(movingLocs(:,1) - fixedLocs(:,1));
y_distance = mean(movingLocs(:,2) - fixedLocs(:,2));
%% 转换为物空间移动距离
x_offset = x_distance * pixel_size;
y_offset = y_distance * pixel_size;
end