Optimal clock solver written in C! This was done for a class project, but I improved upon it a bit to make it a little more user friendly with a few more options.

### How to use
This was made for linux, if you are using windows I would reccommend installing WSL, idk about mac.

1. Clone the repository
2. Download data files from [here](https://drive.google.com/drive/folders/1CRzm2Bv9G5M9UZfZvoGMNmrzKGm5a-yr?usp=drive_link) and put them in a folder named "data" within the project folder.
3. alternatively you generate them by running pinsets.py then pinsets_compact.py, warning this takes a while
4. Run `make`
5. Run `./clocktimal [OPTIONS]`
6. input state as 14 integers separated by spaces, with each number representing which hour that clock is at (these can be negative). These are mapped to the clock as shown below. Then press enter, type "q" then enter to quit

Front
```
1 2 3
4 5 6
7 8 9
```
Back (y2)
```
xx 10 xx 
11 12 13
xx 14 xx
```

### Supports the following metrics
Move optimal:
- Each dial turn is a move
Tick optimal:
- Each tick is a move
Simul optimal:
- If two moves can be done at the same time it counts as a single move
Simtick optimal:
- If two ticks can be done at the same time it counts as a single move

### Additional features
- Multithreading capibilities to improve speed!
- Batch solving from file, use -i "input file" and -o "output file" with the input file formatted as one state per line and it will solve all of the states and store them in the output file.
- Movecount stats, run a simulation on a bunch of scrambles to see the movecount distribution

Uses the "Matrix Clock" approach, here is my class presentation which gives some more details if you're interested: [Here](https://docs.google.com/presentation/d/1cnDSZdiyNajLK2JMcuDps2WB92j1-44i0ACI5HuyWyY/edit?usp=sharing)




### Performace compared to other programs
|    Program                | Time (100 scrambles) | Time per Solve |
|---------------------------|----------------------|----------------|
|   Optclock                |       89.812         |     0.898      |
| Matrix Clock              |       70.000         |     0.700      |
|  Clocktimal               |       27.915         |     0.279      |
| Clocktimal (12 threads)   |       5.460          |     0.055      |




### Data file info

pinsets.py creates finds all of the valid pinsets and corresponding rows. Stores these to two separate binary files.

pinsets.bin, every 14 integers corresponds to a pinset.

rows.bin every 14 integers correponds to a matrix row, every 14 matrix rows corresponds to a pinset at index / 14 in pinsets.bin


pinsets_compact.py creates pinset_mappings.bin where every 14 integers corresponds to the index of the row in unique_rows.bin


### Options
```
-t N
    Use N CPU threads
-i filename
    Input filename that contains scrambles for batch solving
-o filename
    Output filename that will contain the results from the batch solving (default out.txt)
-m
    Print move optimal solution (defaults to printing all solutions), optimized for movecount only if the only one selected
-k
    Print tick optimal solution (defaults to printing all solutions), optimized for tickcount only if the only one selected
-s
    Print simul optimal solution (defaults to printing all solutions)
-c
    Print simtick optimal solution (defaults to printing all solutions)
-v N
    Prints a progress update every N scrambles when in batch mode, doesn't print any if 0 or not specified
```

### Options for movecountstats
```
-t N
    Use N CPU threads
-n N
    Computes stats for N scrambles, defaults to 100
-m
    Move optimal solutions, cannot select multiple metrics at once
-k
    Tick optimal solutions, cannot select multiple metrics at once
-s
    Simul optimal solutions, cannot select multiple metrics at once
-c
    Simtick optimal solutions, cannot select multiple metrics at once
-v N
    Prints a progress update every N scrambles, deafults to every scramble if 0 or unspecified



```


Possible future improvements:
- Scramble input
- Statistics for one scramble
- Optimize pinset generation
- Optimize pinset storage
- Research 12 move pinset coverage (all unique 12 movers? remove dupes?)
- Other clock puzzles