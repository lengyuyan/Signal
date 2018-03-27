function [x_edge, y_edge] = getEdgePosition(img, block_size, thr)
%get all edge laplace
%============================================
%Author:Chame
%Date:2017.3.28
%============================================
%Input
% img: input image
% block_size: 1/2 size of block to determine edge;
% thr: if value > thr, it will be edge;
%--------------------------------------------
%Output
% x_edge:the positions of horizontal edges
% y_edge:the positions of vertical edges
%============================================
narginchk(1, 3);
if nargin == 1
    block_size = 19;
    thr = (2*block_size+1)^2 / 2 * 0.2;
elseif nargin == 2
    thr = (2*block_size+1)^2 / 2 * 0.2;
end
[m, n] = size(img);
h_x = ones(block_size * 2 + 1);
h_y = ones(block_size * 2 + 1);
h_x(:, 1:block_size) = -1;
h_y(block_size+2:end,:) = -1;
value_x = imfilter(img, h_x, 'symmetric');
value_y = imfilter(img, h_y, 'symmetric');

result_x = value_x > thr;
result_y = value_y > thr;
[indy, indx] = find(result_x);
for i = 1:size(indy, 1)
    if indy(i)-block_size < 1 || indy(i)+block_size > m || indx(i)-block_size < 1 || indx(i)+block_size > n
        result_x(indy(i), indx(i)) = 0;
    else
        temp = value_x(indy(i)-block_size:indy(i)+block_size, indx(i)-block_size:indx(i)+block_size);
        [max_y, max_x] = find(temp == max(temp(:)));
        max_y = max_y(1) + indy(i)-block_size - 1;
        max_x = max_x(1) + indx(i)-block_size - 1;
        if indy(i) ~=  max_y || indx(i) ~= max_x
            result_x(indy(i), indx(i)) = 0;           
        end
    end
end
[indy, indx] = find(result_y);
for i = 1:size(indy, 1)
    if indy(i)-block_size < 1 || indy(i)+block_size > m || indx(i)-block_size < 1 || indx(i)+block_size > n
        result_y(indy(i), indx(i)) = 0;
    else
        temp = value_y(indy(i)-block_size:indy(i)+block_size, indx(i)-block_size:indx(i)+block_size);
        [max_y, max_x] = find(temp == max(temp(:)));
        max_y = max_y(1) + indy(i)-block_size - 1;
        max_x = max_x(1) + indx(i)-block_size - 1;
        if indy(i) ~= max_y(1) || indx(i) ~= max_x(1)
            result_y(indy(i), indx(i)) = 0;           
        end
    end
end

[indy,indx] = find(result_x);
x_edge(:, 1) = indx;
x_edge(:, 2) = indy;
[indy,indx] = find(result_y);
y_edge(:, 1) = indx;
y_edge(:, 2) = indy;

figure;
imshow(img, []);
hold on;
plot(x_edge(:, 1), x_edge(:, 2), '.', 'color', 'y');
for i = 1:size(x_edge, 1)
    rectangle('Position', [x_edge(i, 1) - block_size, x_edge(i, 2) - block_size, 2*block_size+1, 2*block_size+1], 'EdgeColor','r');
end
plot(y_edge(:, 1), y_edge(:, 2), '.', 'color', 'r');
for i = 1:size(y_edge, 1)
    rectangle('Position', [y_edge(i, 1) - block_size, y_edge(i, 2) - block_size, 2*block_size+1, 2*block_size+1], 'EdgeColor','r');
end

end