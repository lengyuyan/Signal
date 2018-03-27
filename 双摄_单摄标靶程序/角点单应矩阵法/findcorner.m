
function resultLocs = findcorner(Locs, roi)


[hei, wid] = size(roi);
%% 筛选黑色方块右下角的角点
small_num = 10 / 255;
n = 10;
for i = 1:size(Locs, 1)
    x = round(Locs(i ,1));
    y = round(Locs(i, 2));
    if y-n < 1 || y+n > hei || x-n < 1 || x+n > wid
        Locs(i, 1) = 0;
        Locs(i, 2) = 0;
    else
        l_t = roi(y-n:y, x-n:x);
        %r_t = img(y-n:y, x:x+n);
        %l_b = img(y:y+n, x-n:x);
        r_b = roi(y:y+n, x:x+n);
        temp = r_b - l_t - small_num;
        [num,~] = find(temp > 0);
        if size(num, 1) < 0.8 * n * n
            Locs(i, 1) = 0;
            Locs(i, 2) = 0;
        end
    end
end
for i=size(Locs, 1):-1:1
    if  Locs(i, 1) == 0
        Locs(i, :) = [];
    end
end
resultLocs = Locs;
end