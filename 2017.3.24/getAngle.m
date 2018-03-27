function [x_angle, y_angle] = getAngle(x_lap, y_lap, x_pOrm, y_pOrm, table, pixel_size, loc_x, loc_y)
%get horizontal and vertical angle
%============================================
%Author:Chame
%Date:2017.3.24
%============================================
%Input
% x_lap, y_lap: laplace values of horizontal(x) and  vertical(y) direction, 3 by 1 vector;
% x_pOrm, y_pOrm: deltaZ is plus or minus
% table: laplace-deltaZ table, m by 2 by 5 matrix, the fisrt column is laplace
%        values and the second column is deltaZ, every loctions have one
%        table, so we have m by 2 by 5 data
% pixel_size: the size of CCD pixel;
% loc_x, loc_y: locations of horizontal(x) and vertical(y) direction, 3 by 2 matrix;
%--------------------------------------------
%Output
% x_angle: angle of horizontal direction;
% y_angle: angle of vertical direction;
%============================================
narginchk(6, 6);

table_x = table(:,:,1:3);
table_y = table(:,:,4:6);
x_z = zeros(size(x_lap));
y_z = zeros(size(y_lap));
%% 利用查表法将laplace量转化为离焦量
for i = 1:3
    x_z(i) = laplace2deltaZ(x_lap(i), x_pOrm(i), table_x(:,:,i));
end
%% 利用得到的离焦量拟合直线，得到角度
x = loc_x(:, 2) .* pixel_size;
y = x_z;
p = polyfit(x, y, 1);
x_angle = atan2(p(1));
%% 
for i = 1:3
    y_z(i) = laplace2deltaZ(y_lap(i), y_pOrm(i), table_y(:,:,i));
end
x = loc_y(:, 1) .* pixel_size;
y = y_z;
p = polyfit(x, y, 1);
y_angle = atan2(p(1));
end