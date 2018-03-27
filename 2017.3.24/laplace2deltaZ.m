function deltaZ = laplace2deltaZ(laplace, pOrm, table)
% convert laplace to deltaZ
%============================================
%Author:Chame
%Date:2017.3.24
%============================================
%Input
% laplace: laplace values;
% pOrm: deltaZ is plus or minus, 1 is plus, 0 is 0(focal plane), -1 is minus;
% table: laplace-deltaZ table, m by 2, the fisrt column is laplace
%        values and the second column is deltaZ;
%--------------------------------------------
%Output
% deltaZ: the distance from focal plane;
%============================================
narhinchk(3, 3);

lap_table = table(:, 1);
z_table = table(:, 2);
%% 先判断deltaZ的正负，再取与输入laplace量最接近的值所对应的deltaZ
if pOrm == -1
    m_z_table = z_table(z_table < 0);
    m_lap_table = lap_table(z_table < 0);
    error = m_lap_table - laplace;
    ind = find(error == min(error));
    deltaZ = m_z_table(ind(1));
elseif pOrm == 0
    deltaZ = 0;
elseif pOrm == 1
    p_z_table = z_table(z_table > 0);
    p_lap_table = lap_table(z_table > 0);
    error = p_lap_table - laplace;
    ind = find(error == min(error));
    deltaZ = p_z_table(ind(1));   
end
end