clc;

Data = importdata("./belgium_osm.mat");
A = Data.A; 

C = triangle_counting(A);
[I, J, V] = find(C);
diary on;
for i = 1:nnz(C)
    fprintf("(%d, %d)\t\t%d\n", I(i)-1, J(i)-1, V(i));
end
diary off;


e = ones(size(A , 1), 1);
c = C * e / 2;

function C = triangle_counting(A)
    C = A .* (A * A);
end