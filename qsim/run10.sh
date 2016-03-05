
# Running the python scripts to generate results
python qsim.py -n 10 -p 64 -q 2000 -a 3 -s 2 -o 10a3s2_qsim.txt
python qsim.py -n 10 -p 64 -q 2000 -a 2 -s 3 -o 10a2s3_qsim.txt
python qsim.py -n 10 -p 64 -q 2000 -a 3 -s 3 -o 10a3s3_qsim.txt
python qsim.py -n 10 -p 64 -q 2000 -a 3 -s '3.001' -o 10a3s3p_qsim.txt
python qsim.py -n 10 -p 64 -q 2000 -a '3.001' -s 3 -o 10a3ps3_qsim.txt

# Plotting the results with gnuplot
gnuplot -e "datafile='10a2s3_qsim.txt'; mytitle = 'Queue Length Vs Time (Arrival Rate = 2 pkts/sec, Schedule Rate = 3 pkts/sec)';outputname='10a2s3.jpg'" script.plt 
gnuplot -e "datafile='10a3s2_qsim.txt'; mytitle = 'Queue Length Vs Time (Arrival Rate = 3 pkts/sec, Schedule Rate = 2 pkts/sec)';outputname='10a3s2.jpg'" script.plt 
gnuplot -e "datafile='10a3s3_qsim.txt'; mytitle = 'Queue Length Vs Time (Arrival Rate = 3 pkts/sec, Schedule Rate = 3 pkts/sec)';outputname='10a3s3.jpg'" script.plt
gnuplot -e "datafile='10a3s3p_qsim.txt'; mytitle = 'Queue Length Vs Time (Arrival Rate = 3 pkts/sec, Schedule Rate = 3.001 pkts/sec)';outputname='10a3s3p.jpg'" script.plt
gnuplot -e "datafile='10a3ps3_qsim.txt'; mytitle = 'Queue Length Vs Time (Arrival Rate = 3.001 pkts/sec, Schedule Rate = 3 pkts/sec)';outputname='10a3ps3.jpg'" script.plt
