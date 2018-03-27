function T = calc_geography(points1, points2)

  
    A = zeros(size(points1, 1)*2, 6);
    A(1:2:end,3) = 1;
    A(2:2:end,6) = 1;
    A(1:2:end,1:2) = points1;
    A(2:2:end,4:5) = points1;
   

    Y = zeros(size(points1, 1)*2, 1);
    Y(1:2:end) = points2(:,1);
    Y(2:2:end) = points2(:,2);
    %[~,~,V] = svd(A);
    %h = V(:,9) ./ V(9,9);
    h = A \ Y;
    T = reshape(h,3,2);
    T = T';
end