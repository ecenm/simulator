set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
#set yrange [0:50]
#set y2range [0:10]
#set ytic 0, 5 
#set y2tic 0, 2                        # set ytics automatically
set ytic auto
#set y2tic auto                              # set ytics automatically
set nokey
set grid 
set terminal jpeg large size 800,600
set output outputname

set title mytitle
set xlabel x_label
set ylabel y_label
#set y2label y2_label
set key right bottom

plot datafile using 3:7 axes x1y1 title parameter1 with linespoints pt 7 ps 2
#datafile using 3:10 axes x1y2 title parameter2 with linespoints pt 5 ps 2, \
#datafile using 3:8 axes x1y2 title parameter3 with linespoints pt 7 ps 2 


