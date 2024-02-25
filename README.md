How to run:

1. Download and extract the zip
2. Open a terminal in the main folder
3. Compile with mpicc main.c netherFilters.c endFilters.c fileProcessing.c libcubiomes.a -fwrapv -O3 -Wall -Wextra -Wpedantic -lm
4. Run with mpiexec -n 3 ./a.out
   -> Replace 3 with how many cores you want to use]
5. Results can be found in the folder "Output"


If you'd like to change the range of seeds being tested, adjust the values in "seedRange.txt". The first value is your starting seed and the second value is your ending seed
