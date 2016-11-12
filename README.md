# genetic-algorithm-traveling-salesman
My custom solution to the Traveling Salesman Problem using a basic genetic algorithm

Chris Dean
November 1, 2016
CS 541 Artificial Intelligence
Portland State University

This program is entirely contained in Main.cpp. To use this program in a Linux terminal, simply
compile and run with this pair of commands:

g++ Main.cpp
./a.out <filename>

Where <filename> is a text file with the following format:

<number of cities>
<city number> <x-coordinate> <y-coordinate>
<city number> <x-coordinate> <y-coordinate>
[...]
<start city number>


If you do not specify a filename when you run it, a random city layout will be generated.
