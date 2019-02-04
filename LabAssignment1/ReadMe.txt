
By Robert Sappington for CIS 531
Univercity of Oregon

Using ISPC Compiler from Intel
https://ispc.github.io/

Swirl implementation based on JH Labs Java Image Processing Implementation
http://www.jhlabs.com/ip/filters/TwirlFilter.html 
 
Image reader and writer provided by The Clmg Library
http://cimg.eu/

Implementation:
I did the twirl implementation in serial and parallel. I also include a grayscale implementation, but it currently is commented out. 
Timing:
Running the same timers, here is what I get:

Time for Serial execuation: 0.16
Saving serial image

Time for Parallel execution: 0.03
Saving parallel image

Output:
They produce the exact same image. I have 2 output files. (output_parallel.jpg and output_serial.jpg)

Why is the ISPC faster?:
This is because I can use multiple cores at the same time to calcuate the new spot in my arrays. Each core runs different indexs into the array, and calculates a new twirl pixel to put in its current index. Since each calculation is independent, as the writing to the array, this can be ran all at once.

Run run:
sh runme.sh

To Clear output:
sh clear.sh
