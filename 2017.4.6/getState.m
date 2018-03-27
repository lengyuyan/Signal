function [deltaZ, x_angle, y_angle] = getState(img_ori, img_p, img_m, deltaZ_p, deltaZ_m, pixel_size)
%get the current state of the initial image including u, v and x, y, z
%============================================
%Author:Chame
%Date:2017.3.27
%============================================
%Input
% ima_ori: the original image in the initial position;
% deltaZ_p: the distance of image moving to the plus direction;
% deltaZ_m: the distance of image moving to the minus direction;
% img_p: the image moved deltaZ_p distance to the plus direction;
% img_m: the image moved deltaZ_m distance to the minus direction;
% pixel_size: the size of CCD pixel;
%--------------------------------------------
%Output
% deltaZ: the distance of the initial image to the focal plane;
% x_offset, y_offset: horizontal(x) offset and vertical(y) offset;
% x_angle, y_angle: horizontal(x) angle and vertical(y) angle;
%============================================
narginchk(6, 6);

%% 计算三幅图像的laplace量
[x_lap_o, y_lap_o, x_pt_o,  y_pt_o] = getAllLaplace(img_ori);
[x_lap_p, y_lap_p] = getAllLaplace(img_p);
[x_lap_m, y_lap_m] = getAllLaplace(img_m);
%% 计算x方向的偏转角度
dx = (deltaZ_p-deltaZ_m)/100;
deltaZ_X = zeros(size(x_lap_o));
for i = 1:length(x_lap_o)
    p = polyfit([deltaZ_m, 0, deltaZ_p], [x_lap_m(i), x_lap_o(i), x_lap_p(i)], 2);
    lap = polyval(p, [deltaZ_m:dx:deltaZ_p]);
    ind = find(lap == max(lap));
    deltaZ_X(i) = deltaZ_m + (ind-1) * dx;
end
x_angle = getangle(deltaZ_X, x_pt_o(:, 1), pixel_size);
%% 计算y方向的偏转角度
dy = (deltaZ_p-deltaZ_m)/100;
deltaZ_Y = zeros(size(x_lap_o));
for i = 1:length(x_lap_o)
    p = polyfit([deltaZ_m, 0, deltaZ_p], [y_lap_m(i), y_lap_o(i), y_lap_p(i)], 2);
    lap = polyval(p, [deltaZ_m:dy:deltaZ_p]);
    ind = find(lap == max(lap));
    deltaZ_Y(i) = deltaZ_m + (ind-1) * dy;
end
y_angle = getangle(deltaZ_Y, y_pt_o(:, 2), pixel_size);
%% 镜头初始位置离准焦面的距离
deltaZ = (deltaZ_X(2) + deltaZ_Y(2)) / 2;

end

%% 计算倾斜角
function angle = getangle(deltaZ, pt, pixel_size)
    x = pt .* pixel_size;
    y = deltaZ;
    y(2) = 2 * y(2);
    p = polyfit(x, y, 1);
    angle = atan(p(1)); 
    angle = angle / pi * 180;
end
