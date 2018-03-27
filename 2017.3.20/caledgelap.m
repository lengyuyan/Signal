close all;
clear all;
clc;

I = cell(8,1);
I{1} = imread('2017.2.21采图\-0.05.jpg');
I{2} = imread('2017.2.21采图\-0.01.jpg');
I{3} = imread('2017.2.21采图\0.jpg');
I{4} = imread('2017.2.21采图\0.004.jpg');
I{5} = imread('2017.2.21采图\0.005.jpg');
I{6} = imread('2017.2.21采图\0.01.jpg');
I{7} = imread('2017.2.21采图\0.05.jpg');
I{8} = imread('2017.2.21采图\0.1.jpg');

lap = cell(8,2);
for i = 1:8
    [lap{i, 1}, lap{i, 2}] = getAllEdgeLaplace(I{i});
end
x = [-0.05, -0.01, 0, 0.004, 0.005, 0.01, 0.05, 0.1];
y_1 = zeros(1, 8);
y_2 = zeros(1, 8);
for i = 1:8
    y_1(i) = lap{i, 1}(2);
    y_2(i) = lap{i, 2}(2);
end
figure;
plot(x, y_1);
figure;
plot(x, y_2);