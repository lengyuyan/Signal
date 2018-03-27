function [horizontal_lap, vertical_lap, horizontal_pt,  vertical_pt] = getAllEdgeLaplace(I, n, m, edge_size, error)
%get all edge laplace
%============================================
%Author:Chame
%Date:2017.2.22
%============================================
%Input
% I: input image
% n: 1/2 size of block to determine the points;
% m: 1/2 size of block to choose points;
% edge_size: 1/2 size of block to determine edge;
% error: error of distance of points
%--------------------------------------------
%Output
% horizontal_lap: laplace of horizontal edges;
% vertical_lap: laplace of vertical edges;
% horizontal_pt: locations of horizontal points;
% vertical_pt: locations of vertical points;
%============================================
narginchk(1, 5);
if nargin == 1
    n = 10;
    m = 50;
    edge_size = 15;
    error = 5;
elseif nargin == 2
    m = 50;
    edge_size = 15;
    error = 5;
elseif nargin == 3
    edge_size = 15;
    error = 5;
elseif nargin == 4
    error = 5;
end
if size(I, 3) == 3
    I = rgb2gray(I);
end

I = imresize(I, [600, 800]);%将图像调整到固定大小
I = I(40:end-40, 40:end-40,:);%将边缘非靶标区域截掉
I = im2double(I);
figure;imshow(I);hold on;

tic;
%% 检测黑色方块角点
Points = detectHarrisFeatures(I);
Loc = Points.Location;
plot(Loc(:,1), Loc(:,2),'.');
Loc_copy = Loc;

%% 筛选黑色方块右上角的角点
small_num = 10 / 255;
for i = 1:size(Loc, 1)
    x = round(Loc(i ,1));
    y = round(Loc(i, 2));
    %l_t = I(y-n:y, x-n:x);
    r_t = I(y-n:y, x:x+n);
    l_b = I(y:y+n, x-n:x);
    %r_b = I(y:y+n, x:x+n);
    temp = r_t - l_b - small_num;
    [num,~] = find(temp > 0);
    if size(num, 1) < 0.8 * size(r_t, 1) * size(r_t, 2)
        Loc(i, 1) = 0;
        Loc(i, 2) = 0;
    end
end
for i=size(Loc, 1):-1:1
    if  Loc(i, 1) == 0
        Loc(i, :) = [];
    end
end

%% 计算中心点
center = mean(Loc);
%{
figure;imshow(I);hold on;
plot(Loc(:,1), Loc(:,2),'.');hold on;
plot(center(1), center(2),'.','Color',[0 1 0]);hold on;
for i = 1:size(Loc, 1)
    rectangle('Position', [Loc(i,1) - n, Loc(i,2)-n, 2*n, 2*n],'EdgeColor','r');
end
%}
%% 筛选上下左右中五个黑色方块
for i = 1:size(Loc, 1)
    if abs(Loc(i, 1) - center(1)) < error
        if abs(Loc(i, 2) - center(2)) < error
            Loc_center = Loc(i, :);
        elseif Loc(i, 2) < center(2) - error
            Loc_top = Loc(i, :);
        elseif Loc(i, 2) > center(2) + error
            Loc_bottom = Loc(i, :);
        end
    elseif Loc(i, 1) < center(1) - error && abs(Loc(i, 2) - center(2)) < error
        Loc_left = Loc(i, :);
    elseif Loc(i, 1) > center(1) + error && abs(Loc(i, 2) - center(2)) < error
        Loc_right = Loc(i, :);
    end
end
%{
figure;imshow(I);hold on;
plot(Loc_center(1), Loc_center(2),'.');
plot(Loc_left(1), Loc_left(2),'.');
plot(Loc_right(1), Loc_right(2),'.');
plot(Loc_top(1), Loc_top(2),'.');
plot(Loc_bottom(1), Loc_bottom(2),'.');
%
%% 确定上下左右中五个区域范围
rectangle('Position', [Loc_center(1) - m, Loc_center(2) - m, 2*m, 2*m], 'EdgeColor','r');
rectangle('Position', [Loc_left(1) - m, Loc_left(2) - m, 2*m, 2*m], 'EdgeColor','r');
rectangle('Position', [Loc_right(1) - m, Loc_right(2) - m, 2*m, 2*m], 'EdgeColor','r');
rectangle('Position', [Loc_top(1) - m, Loc_top(2) - m, 2*m, 2*m], 'EdgeColor','r');
rectangle('Position', [Loc_bottom(1) - m, Loc_bottom(2) - m, 2*m, 2*m], 'EdgeColor','r');
%}
%% 筛选区域范围内的对应角点
for i = 1:size(Loc_copy, 1)
    x = round(Loc_copy(i ,1));
    y = round(Loc_copy(i, 2));
    if x > Loc_center(1) - m && x < Loc_center(1) + m && y > Loc_center(2) - m && y < Loc_center(2) + m
        l_t = I(y-n:y, x-n:x);
        r_b = I(y:y+n, x:x+n);
        temp_1 = l_t - r_b - small_num;
        temp_2 = r_b - l_t - small_num;
        [num_1,~] = find(temp_1 > 0);
        [num_2,~] = find(temp_2 > 0);
        if size(num_1, 1) > 0.8 * size(l_t, 1) * size(l_t, 2)
            Loc_center_left = Loc_copy(i, :);
        elseif size(num_2, 1) > 0.8 * size(l_t, 1) * size(l_t, 2)
            Loc_center_bottom = Loc_copy(i, :);
        end
    elseif x > Loc_left(1) - m && x < Loc_left(1) + m && y > Loc_left(2) - m && y < Loc_left(2) + m
        l_t = I(y-n:y, x-n:x);
        r_b = I(y:y+n, x:x+n);
        temp = l_t - r_b - small_num;
        [num,~] = find(temp > 0);
        if size(num, 1) > 0.8 * size(l_t, 1) * size(l_t, 2)
            Loc_left_left = Loc_copy(i, :);
        end
    elseif x > Loc_right(1) - m && x < Loc_right(1) + m && y > Loc_right(2) - m && y < Loc_right(2) + m
        l_t = I(y-n:y, x-n:x);
        r_b = I(y:y+n, x:x+n);
        temp = l_t - r_b - small_num;
        [num,~] = find(temp > 0);
        if size(num, 1) > 0.8 * size(l_t, 1) * size(l_t, 2)
            Loc_right_left = Loc_copy(i, :);
        end
    elseif x > Loc_top(1) - m && x < Loc_top(1) + m && y > Loc_top(2) - m && y < Loc_top(2) + m
        l_t = I(y-n:y, x-n:x);
        r_b = I(y:y+n, x:x+n);
        temp = r_b - l_t - small_num;
        [num,~] = find(temp > 0);
        if size(num, 1) > 0.8 * size(l_t, 1) * size(l_t, 2)
            Loc_top_bottom = Loc_copy(i, :);
        end
    elseif x > Loc_bottom(1) - m && x < Loc_bottom(1) + m && y > Loc_bottom(2) - m && y < Loc_bottom(2) + m
        l_t = I(y-n:y, x-n:x);
        r_b = I(y:y+n, x:x+n);
        temp = r_b - l_t - small_num;
        [num,~] = find(temp > 0);
        if size(num, 1) > 0.8 * size(l_t, 1) * size(l_t, 2)
            Loc_bottom_bottom = Loc_copy(i, :);
        end
    end
end
%
figure;imshow(I);hold on;
plot(Loc_center(1), Loc_center(2),'.');
plot(Loc_left(1), Loc_left(2),'.');
plot(Loc_right(1), Loc_right(2),'.');
plot(Loc_top(1), Loc_top(2),'.');
plot(Loc_bottom(1), Loc_bottom(2),'.');
plot(Loc_center_left(1), Loc_center_left(2),'.');
plot(Loc_center_bottom(1), Loc_center_bottom(2),'.');
plot(Loc_left_left(1), Loc_left_left(2),'.');
plot(Loc_right_left(1), Loc_right_left(2),'.');
plot(Loc_top_bottom(1), Loc_top_bottom(2),'.');
plot(Loc_bottom_bottom(1), Loc_bottom_bottom(2),'.');
%}
horizontal_pt = zeros(3, 2);
horizontal_pt(1, :) = (Loc_left + Loc_left_left) / 2; 
horizontal_pt(2, :) = (Loc_center + Loc_center_left) / 2; 
horizontal_pt(3, :) = (Loc_right + Loc_right_left) / 2; 
vertical_pt = zeros(3, 2);
vertical_pt(1, :) = (Loc_top + Loc_top_bottom) / 2;
vertical_pt(2, :) = (Loc_center + Loc_center_bottom) / 2;
vertical_pt(3, :) = (Loc_bottom + Loc_bottom_bottom) / 2;
plot(horizontal_pt(:, 1), horizontal_pt(:, 2),'.', 'Color', [0 1 0]);
plot(vertical_pt(:, 1), vertical_pt(:, 2),'.','Color', [0 1 0]);
%% 得到刃边区域
for i = 1:3
    rectangle('Position', [horizontal_pt(i, 1) - edge_size, horizontal_pt(i, 2) - edge_size, 2*edge_size, 2*edge_size], 'EdgeColor','r');
    rectangle('Position', [vertical_pt(i, 1) - edge_size, vertical_pt(i, 2) - edge_size, 2*edge_size, 2*edge_size], 'EdgeColor','r');
end
horizontal_edge = cell(3, 1);
vertical_edge = cell(3, 1);
for i = 1:3
    horizontal_edge{i} = I(horizontal_pt(i, 2) - edge_size:horizontal_pt(i, 2) + edge_size, horizontal_pt(i, 1) - edge_size:horizontal_pt(i, 1) + edge_size);
    vertical_edge{i} = I(vertical_pt(i, 2) - edge_size:vertical_pt(i, 2) + edge_size, vertical_pt(i, 1) - edge_size:vertical_pt(i, 1) + edge_size);
end
%% 计算横向与纵向刃边拉普拉斯值
horizontal_lap = zeros(3, 1);
vertical_lap = zeros(3, 1);
for k = 1:3
    temp_h = horizontal_edge{k};
    temp_v = vertical_edge{k};
    sum_lap_h = 0;
    sum_lap_v = 0;
    for i = 2:2*edge_size-1
        for j = 2:2*edge_size-1
            sum_lap_h = sum_lap_h + (9*temp_h(i,j) - sum(sum(temp_h(i-1:i+1, j-1:j+1)))) .^ 2;
            sum_lap_v = sum_lap_v + (9*temp_v(i,j) - sum(sum(temp_v(i-1:i+1, j-1:j+1)))) .^ 2;
        end
    end
    horizontal_lap(k) = sum_lap_h / (2*edge_size-2).^2;
    vertical_lap(k) = sum_lap_v / (2*edge_size-2).^2;
end
toc;
end