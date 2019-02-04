By Robert Sappington for CIS 531
Univerity of Oregon
 
Using ICC and TBB from Intel
 
Swirl implemntation based on JH Labs Java Image Processing Implementation
http://www.jhlabs.com/ip/filters/TwirlFilter.html 
  
Image reader and writer provided by The Clmg Library
http://cimg.eu/

 

Run run:
sh runme.sh

To Clear output:
sh clear.sh


Implemention:
I did the twirl implementation in serial and parallel. I also include a grayscale implementation, but it currently is commented out. 
Timing:
Running the same timers, here is what I get:


I think the reason the very last output is not faster than the previous is because of the addition overhead from each image that TBB creates trying to run the multiple images at the same time in parallel. So updating the chache and page misses is the most likely cause why the last one is slowest.

Output:
Time to swirl : images/alpaca.jpg 0.24
Time to swirl : images/bee.jpg 0.21
Time to swirl : images/bison.jpg 0.23
Time to swirl : images/dog.jpg 0.23
Time to swirl : images/duck.jpg 0.2
Time to swirl : images/eagle.jpg 0.22
Time to swirl : images/fox.jpg 0.23
Time to swirl : images/giraffe.jpg 0.23
Time to swirl : images/gorilla.jpg 0.22
Time to swirl : images/hedgehog.jpg 0.2
Time to swirl : images/horns.jpg 0.21
Time to swirl : images/hyena.jpg 0.22
Time to swirl : images/koala.jpg 0.22
Time to swirl : images/lizard.jpg 0.22
Time to swirl : images/owl.jpg 0.22
Time to swirl : images/parrot.jpg 0.22
Time to swirl : images/ram.jpg 0.25
Time to swirl : images/tiger.jpg 0.23
Time to swirl : images/turtle.jpg 0.25
Time to swirl : images/white-lion.jpg 0.29
Time for Serial distoration: 9.81

Time to swirl : images/alpaca.jpg 0.0445712
Time to swirl : images/bee.jpg 0.0411057
Time to swirl : images/bison.jpg 0.0458361
Time to swirl : images/dog.jpg 0.0395769
Time to swirl : images/duck.jpg 0.039582
Time to swirl : images/eagle.jpg 0.0436744
Time to swirl : images/fox.jpg 0.0418225
Time to swirl : images/giraffe.jpg 0.0436082
Time to swirl : images/gorilla.jpg 0.0452826
Time to swirl : images/hedgehog.jpg 0.0408212
Time to swirl : images/horns.jpg 0.0484203
Time to swirl : images/hyena.jpg 0.0479427
Time to swirl : images/koala.jpg 0.0489685
Time to swirl : images/lizard.jpg 0.0470299
Time to swirl : images/owl.jpg 0.050689
Time to swirl : images/parrot.jpg 0.0471649
Time to swirl : images/ram.jpg 0.0519878
Time to swirl : images/tiger.jpg 0.050198
Time to swirl : images/turtle.jpg 0.0542558
Time to swirl : images/white-lion.jpg 0.0644954
Total Run Time for Parallel distoration using parallel_for: 8.31975

Time to swirl : images/bee.jpg 0.213613
Time to swirl : images/duck.jpg 0.202964
Time to swirl : images/alpaca.jpg 0.228175
Time to swirl : images/eagle.jpg 0.226816
Time to swirl : images/dog.jpg 0.229219
Time to swirl : images/fox.jpg 0.228794
Time to swirl : images/bison.jpg 0.24003
Time to swirl : images/white-lion.jpg 0.297416
Time to swirl : images/giraffe.jpg 0.22783
Time to swirl : images/hedgehog.jpg 0.197045
Time to swirl : images/gorilla.jpg 0.229803
Time to swirl : images/horns.jpg 0.219119
Time to swirl : images/koala.jpg 0.226565
Time to swirl : images/hyena.jpg 0.232881
Time to swirl : images/lizard.jpg 0.221332
Time to swirl : images/parrot.jpg 0.240037
Time to swirl : images/ram.jpg 0.26238
Time to swirl : images/tiger.jpg 0.273512
Time to swirl : images/owl.jpg 0.330004
Time to swirl : images/turtle.jpg 0.302333
Total Run Time for Tasking Serial execuation: 0.702074

Time to swirl : images/duck.jpg 0.266825
Time to swirl : images/bee.jpg 0.285124
Time to swirl : images/alpaca.jpg 0.305996
Time to swirl : images/eagle.jpg 0.307919
Time to swirl : images/fox.jpg 0.308606
Time to swirl : images/dog.jpg 0.309391
Time to swirl : images/bison.jpg 0.31727
Time to swirl : images/white-lion.jpg 0.412851
Time to swirl : images/giraffe.jpg 0.308194
Time to swirl : images/hedgehog.jpg 0.264688
Time to swirl : images/gorilla.jpg 0.308327
Time to swirl : images/horns.jpg 0.297601
Time to swirl : images/lizard.jpg 0.307095
Time to swirl : images/hyena.jpg 0.328648
Time to swirl : images/koala.jpg 0.330586
Time to swirl : images/owl.jpg 0.178599
Time to swirl : images/ram.jpg 0.196551
Time to swirl : images/parrot.jpg 0.321737
Time to swirl : images/tiger.jpg 0.22405
Time to swirl : images/turtle.jpg 0.166742
Total Run Time for tasking and parallel exection: 0.807863

