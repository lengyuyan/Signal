close all
clear all
clc

%============================================
fixImg = im2double(imread('FIX.jpg'));
movImg = im2double(imread('UNFIX3.jpg'));
fixImg = rgb2gray(fixImg);
movImg = rgb2gray(movImg);
scale = 0.2;
fixImg = imresize(fixImg, scale);
movImg = imresize(movImg, scale);
%============================================
pixel_size = 0.00112 / scale;
u = 600;
f = 3.31;
L = 280;
W = 200;

diff = 18 * f / u / pixel_size ;
%============================================
[hei, wid, ~] = size(fixImg);
roi_size = 200;
center_y = floor(hei/2) + 1;
center_x = floor(wid/2) + 1;
%=========================================== 
%% ºÏ≤‚πÃ∂®…„œÒÕ∑Ω«µ„
roi_center_fix = fixImg(center_y-roi_size:center_y+roi_size, center_x-roi_size:center_x+roi_size);
fixPoints = detectHarrisFeatures(roi_center_fix);
fixLocs = fixPoints.Location;
fixLocs = findcorner(fixLocs, roi_center_fix);

figure;imshow(roi_center_fix);hold on;
plot(center_x, center_y, '.');
plot(fixLocs(:,1), fixLocs(:,2), '.');

%% ºÏ≤‚“∆∂Ø…„œÒÕ∑Ω«µ„
roi_center_mov = movImg(center_y-roi_size:center_y+roi_size, center_x-roi_size:center_x+roi_size);
movPoints = detectHarrisFeatures(roi_center_mov);
movLocs = movPoints.Location;
movLocs = findcorner(movLocs, roi_center_mov);

figure;imshow(roi_center_mov);hold on;
plot(center_x, center_y, '.');
plot(movLocs(:,1), movLocs(:,2), '.');
%%
T = calc_geography(fixLocs, movLocs);
angle = atan((T(2, 1) - T(1, 2)) / (T(1, 1) + T(2, 2)));
x = T(1, 3);
y = T(2, 3);
s = (T(1, 1) + T(2, 2)) / (4 * cos(angle)) + (T(2, 1) - T(1, 2)) / (4 * sin(angle));

%%
u = ((x - diff) * pixel_size / f) / pi * 180;
v = (y * pixel_size / f) / pi * 180;
w = angle / pi * 180;
adjust_u = v;
adjust_v = -u;
adjust_w = w;