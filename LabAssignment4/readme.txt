
Lab4 Assignment

I broke it up into 3 seperate files.
One for each process/grid size.

Run with:

sh runme.sh

Results of runtime.

Ran on 1,000 iterations. All results are in seconds.
______________________________________
|table  |  p = 1 | p=4     | p=9     |
|_______|________|_________|_________|
|n=100  |0.180218| 0.14795 | 0.26308 |
|_______|________|_________|_________|
|n=1000 |17.1393 | 5.527   |11.875   |
|_______|________|_________|_________|
|n=5000 |427.729 | 139.399 | 253.591 |
|_______|________|_________|_________|
|n=1000 | >840   | 559.81  | 837.005 |
|_______|________|_________|_________|


Summary:
As we can see splitting up the work with 4 process works best in all cases. Using 1 process is the slowest.
And using 9, the communication overhead is enough where we lose the benedit of using MPI.
