pinsets.py creates finds all of the valid pinsets and corresponding rows. Stores these to two separate binary files.

pinsets.bin, every 14 integers corresponds to a pinset.

rows.bin every 14 integers correponds to a matrix row, every 14 matrix rows corresponds to a pinset at index / 14 in pinsets.bin


pinsets_compact.py creates pinset_mappings.bin where every 14 integers corresponds to the index of the row in unique_rows.bin


Options:

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



Possible future improvements:
Scramble input
Statistics for one scramble
Optimize pinset generation
Optimize pinset storage
Research 12 move pinset coverage (all unique 12 movers? remove dupes?)
Other clock puzzles