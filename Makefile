all:	clean compex run extract

compex:
	#g++ packet.cpp queue.cpp node.cpp factory.cpp event.cpp main.cpp -lrt -o main
	g++ -std=c++0x *.cpp -lrt -o main

run:
	#./main 
	./main 1 config.txt >> log.txt 

extract:
	python extract_log.py 	
	cp results/queue_0.log dummy/64/
	cd dummy/64/ && sh run.sh
draw:
	gnuplot script.plt
	evince plot.ps

rclean:
	rm -f *_es.jpg *_eventsim.txt

clean:
	rm -f main file.txt log.txt *.ps *.jpg log.txt output
	rm -f *.o
	rm -rf results
