close all
clear all
clc
%============================================
fixImg = im2double(imread('FIX.jpg'));
movImg = im2double(imread('UNFIX.jpg'));
fixImg = rgb2gray(fixImg);
movImg = rgb2gray(movImg);
scale = 0.2;
fixImg = imresize(fixImg, scale);
movImg = imresize(movImg, scale);
%============================================
%============================================
pixel_size = 0.00112 / scale;
u = 600;
f = 3.31;
L = 280;
W = 200;

diff = round(18 * f / u / pixel_size) ;
%============================================
%=========================================== 
%% 计算方块所在的区域
v = u * f / (u - f);
pl = round(L * v / u / pixel_size);
pw = round(W * v / u / pixel_size);
[m, n] = size(fixImg);
roi_size = 80;
roi_fix = cell(5, 1);
roi_mov = cell(5, 1);

center = [m/2, n/2];
center_roi_hei = round([m/2-roi_size:m/2+roi_size]);
center_roi_wid = round([n/2-roi_size:n/2+roi_size]);

roi_fix{1} = fixImg(center_roi_hei, center_roi_wid); 
roi_fix{2} = fixImg(center_roi_hei, center_roi_wid - pl); 
roi_fix{3} = fixImg(center_roi_hei, center_roi_wid + pl); 
roi_fix{4} = fixImg(center_roi_hei - pw, center_roi_wid); 
roi_fix{5} = fixImg(center_roi_hei + pw, center_roi_wid); 

roi_mov{1} = movImg(center_roi_hei, center_roi_wid + diff); 
roi_mov{2} = movImg(center_roi_hei, center_roi_wid - pl + diff); 
roi_mov{3} = movImg(center_roi_hei, center_roi_wid + pl + diff); 
roi_mov{4} = movImg(center_roi_hei - pw, center_roi_wid + diff); 
roi_mov{5} = movImg(center_roi_hei + pw, center_roi_wid + diff); 

for i = 1:5
    roi_fix{i} = padarray(roi_fix{i}, [roi_size,roi_size], 'replicate');
    %roi_mov{i} = 1 - padarray(roi_mov{i}, [roi_size,roi_size], 'replicate');
    %roi_fix{i} = roi_fix{i} - 1/2 * (max(max(roi_fix{i})) - min(min(roi_fix{i})));
    %roi_mov{i} = roi_mov{i} - 1/2 * (max(max(roi_mov{i})) - min(min(roi_mov{i})));
end
for i = 1:5
    %figure;imshow([roi_fix{i},roi_mov{i}]);
end

%% 
Corr = cell(5, 1);
for i = 1:5
    %Corr{i} = conv2(roi_fix{i}, roi_mov{i});
    fix = roi_fix{i};
    mov = roi_mov{i};
    corr = zeros(size(mov));
    for x = 1:2*roi_size+1
        for y = 1:2*roi_size+1
            corr(x, y) = sum(sum(abs(fix(x:x+2*roi_size, y:y+2*roi_size) - mov)));
        end
    end
    Corr{i} = corr ./ max(max(corr));
    [indy(i), indx(i)] = find(Corr{i} == min(min(Corr{i})));
    indy(i) = indy(i) - roi_size - 1;
    indx(i) = indx(i) - roi_size - 1;
    figure;imshow(Corr{i});
end
