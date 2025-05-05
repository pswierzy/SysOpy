CC = gcc
CFLAGS = -Wall -g

.PHONY: all clean

all: integral fifo_integral_sender fifo_integral_calc

integral: integral.c
	$(CC) $(CFLAGS) integral.c -o integral

fifo_integral_sender: fifo_integral_sender.c
	$(CC) $(CFLAGS) fifo_integral_sender.c -o fifo_integral_sender

fifo_integral_calc: fifo_integral_calc.c
	$(CC) $(CFLAGS) fifo_integral_calc.c -o fifo_integral_calc

clean: 
	rm integral fifo_integral_sender fifo_integral_calc /tmp/przedzial /tmp/wynik