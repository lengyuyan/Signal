function [horizontal_lap, vertical_lap, horizontal_pt,  vertical_pt] = getAllLaplace(I, edge_size, error)
%get all edge laplace
%============================================
%Author:Chame
%Date:2017.3.28
%============================================
%Input
% I: input image
% edge_size: 1/2 size of block to determine edge;
% error: error of distance of points
%--------------------------------------------
%Output
% horizontal_lap: laplace of horizontal edges;
% vertical_lap: laplace of vertical edges;
% horizontal_pt: locations of horizontal points;
% vertical_pt: locations of vertical points;
%============================================
narginchk(1, 3);
if nargin == 1
    edge_size = 15;
    error = 40;
elseif nargin == 2
    error = 40;
end
if size(I, 3) == 3
    I = rgb2gray(I);
    %I = I(:,:,2);
end
if max(I(:) > 1)
    I = im2double(I);
end

%% 检测黑色方块角点
[x_edge, y_edge] = getEdgePosition(I);

%% 计算中心点
center_x = mean(x_edge);
center_y = mean(y_edge);
%% 筛选上下左右中五个黑色方块
for i = 1:size(x_edge, 1)
    if abs(x_edge(i, 1) - center_x(1)) < error && abs(x_edge(i, 2) - center_x(2)) < error
        Loc_center_x = x_edge(i, :);
    elseif x_edge(i, 1) < center_x(1) - error && abs(x_edge(i, 2) - center_x(2)) < error
        Loc_left = x_edge(i, :);
    elseif x_edge(i, 1) > center_x(1) - error && abs(x_edge(i, 2) - center_x(2)) < error
        Loc_right = x_edge(i, :);
    end
end
for i = 1:size(y_edge, 1)
    if abs(y_edge(i, 2) - center_y(2)) < error && abs(y_edge(i, 1) - center_y(1)) < error
        Loc_center_y = y_edge(i, :);
    elseif y_edge(i, 2) < center_y(2) - error && abs(y_edge(i, 1) - center_y(1)) < error
        Loc_top = y_edge(i, :);
    elseif y_edge(i, 2) > center_y(2) - error && abs(y_edge(i, 1) - center_y(1)) < error
        Loc_bottom = y_edge(i, :);
    end
end
%%
horizontal_pt = zeros(3, 2);
horizontal_pt(1, :) = Loc_left; 
horizontal_pt(2, :) = Loc_center_x; 
horizontal_pt(3, :) = Loc_right;
vertical_pt = zeros(3, 2);
vertical_pt(1, :) = Loc_top;
vertical_pt(2, :) = Loc_center_y;
vertical_pt(3, :) = Loc_bottom;
figure;imshow(I);hold on;
plot(horizontal_pt(:, 1), horizontal_pt(:, 2),'.', 'Color', [0 1 0]);
plot(vertical_pt(:, 1), vertical_pt(:, 2),'.','Color', [0 1 0]);
%% 得到刃边区域
for i = 1:3
    rectangle('Position', [horizontal_pt(i, 1) - edge_size, horizontal_pt(i, 2) - edge_size, 2*edge_size+1, 2*edge_size+1], 'EdgeColor','r');
    rectangle('Position', [vertical_pt(i, 1) - edge_size, vertical_pt(i, 2) - edge_size, 2*edge_size+1, 2*edge_size+1], 'EdgeColor','r');
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
    for i = 2:2*edge_size
        for j = 2:2*edge_size
            sum_lap_h = sum_lap_h + (9*temp_h(i,j) - sum(sum(temp_h(i-1:i+1, j-1:j+1)))) .^ 2;
            sum_lap_v = sum_lap_v + (9*temp_v(i,j) - sum(sum(temp_v(i-1:i+1, j-1:j+1)))) .^ 2;
        end
    end
    horizontal_lap(k) = sum_lap_h / (2*edge_size-1).^2;
    vertical_lap(k) = sum_lap_v / (2*edge_size-1).^2;
end

end