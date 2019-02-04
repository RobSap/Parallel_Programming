
Lab3 Assignment

Run with:

sh runme.sh

Results of runtime.

Ran on 1,000 iterations.(To many people were running and taking up resources, so I ran 1,000 instead of 10,000)
__________________________________________________________
|table  |  p = 1 |   p =2   | p=4     | p=8     | p=16    |
|_______|________|__________|_________|_________|_________|
|n=100  | 0.0325 | 0.0177   | 0.00880 | 0.00610 | 0.019   |
|_______|________|__________|_________|_________|_________|
|n=1000 | 2.448  | 1.245    | 0.6158  | 0.3550  | 0.4509  |
|_______|________|__________|_________|_________|_________|
|n=1e4  | 248.4  | 126.725  | 64.924  | 36.2802 | 37.3513 |
|_______|________|__________|_________|_________|_________|

In this I can see using 8 processes is the fastest for all different sizes of N.
This is likely because the overhead pass the amount it helped. And it would not go any faster. 
And likely causes more page misses by missing the loading in memory.
