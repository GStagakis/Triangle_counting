# Triangle_counting
Linux Execution:

./triangle_counting matrixmarketfile.mtx > file.txt   
#to print the COO output and the elapsed time in a .txt file  


./cilk_triangle_counting matrixmarketfile.mtx > file.txt   
#to print the COO output and the elapsed time in a .txt file


./openmp_triangle_counting matrixmarketfile.mtx > file.txt   
#to print the COO output and the elapsed time in a .txt file


./pthreads_triangle_counting matrixmarketfile.mtx (number of threads created) > file.txt   
#to print the COO output and the elapsed time in a .txt file (default number of threads                                                                                          created is 8)

#triangle_cnt.m is the Matlab file used to check results
