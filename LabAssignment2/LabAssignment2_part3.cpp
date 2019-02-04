#include <cstdio>
#include <ctime>
#include <array>
#include <iostream>
#define cimg_use_jpeg
#include "CImg.h"
#include <X11/Xlib.h>
#include <math.h>
#include <fstream>
#include <string>
#include "tbb/tick_count.h"
#include <unistd.h>
//For part 1
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"

//For part2
#include "tbb/task.h"
#include "tbb/task_scheduler_init.h"

//#include <tbb/tbb.h>
//#include "tbb/task_group.h"



/* 
 * By Robert Sappington for CIS 531
 * Univerity of Oregon
 *
 * Using ICC and TBB from Intel
 *
 * Swirl implemntation based on JH Labs Java Image Processing Implementation
 * http://www.jhlabs.com/ip/filters/TwirlFilter.html 
 * 
 * Image reader and writer provided by The Clmg Library
 * http://cimg.eu/
 *
 */
using namespace tbb;

class swirl_class {
    int *R2;
    int *G2;
    int *B2;
    int *R;
    int *G;
    int *B;
    int W;
    int H;
    float centerx;
    float centery;
    int radius;
    int radius2;
    public:
      void operator() (tbb::blocked_range<std::size_t> &r) const {
      int temp_index = 0;
      for (std::size_t i = r.begin(); i !=r.end(); i++) {
               float y = i/W;
               float x  = i%W;
               float x2 = x - centerx;
               float y2 = y - centery;
               float distance = (x2*x2) + (y2*y2);

               if (distance <= radius2)
               {

                   float real_distance = sqrt(distance);
                   float angle = 3;
                   float temp = atan2(y2,x2) + angle * (radius-real_distance)/radius;
                   int new_x =centerx + real_distance*cos(temp);
                   int new_y = centery+ real_distance*sin(temp);
             
                   if ((new_x)>=0 && new_y >= 0 && (new_x) < W && new_y < H)
                    {   //y*W+x
                        temp_index = new_y*W+new_x;
                        R[i] =R2[temp_index];
                        G[i] =G2[temp_index];
                        B[i] =B2[temp_index];


                   }
              }
       }


    }
    swirl_class(int *r2,int *g2,int *b2,int*r,int *g,int *b, int w, int h, float centerX, float centerY, int radiuS, int radiuS2 ):
    R2(r2),G2(g2),B2(b2),R(r),G(g),B(b),W(w),H(h), centerx(centerX),centery(centerY), radius(radiuS), radius2(radiuS2){}

};

class swirlTask: public tbb::task {

public:
    swirlTask(std::string tempString_):
    tempString(tempString_){}
    std::string tempString;
    task* execute() {      // Overrides virtual function task::execute
        
        cimg_library::CImg<unsigned char> input_img(tempString.c_str());
        cimg_library::CImg<unsigned char> output_img(input_img.width(), input_img.height(), 1, 3);
        int W = input_img.width();
        int H = input_img.height();
        int *R = new int [W*H];
        int *G = new int [W*H];
        int *B = new int [W*H];

        for (int c = 0; c < input_img.width(); ++c) {
          for (int r = 0; r < input_img.height(); ++r) {
            R[r*W+c] = input_img(c, r, 0);
            G[r*W+c] = input_img(c, r, 1);
            B[r*W+c] = input_img(c, r, 2);
          }

        }

        auto R2=R;
        auto G2=G;
        auto B2=B;

        int tempDimension = input_img.width() * input_img.height();
        float centerx = (W)/2;
        float centery = (H)/2;
        float radius = 0;
        if ( centerx > centery)
            radius = centerx;
        else
            radius = centery;

        float radius2 = radius * radius;
        
        tbb::tick_count temp_time = tbb::tick_count::now();
        tbb::parallel_for(tbb::blocked_range<std::size_t>(0,tempDimension),swirl_class(R2,G2,B2,R,G,B,W,H,centerx, centery, radius,radius2));
        double duration = ( tbb::tick_count::now()-temp_time).seconds(); 
        std::cout<<"\nTime to swirl : "<< tempString << " " << duration ;
 
        for (int c = 0; c < input_img.width(); ++c) {
          for (int r = 0; r < input_img.height(); ++r) {
              output_img(c, r, 0)=R[r*W+c];
              output_img(c, r, 1)=G[r*W+c];
              output_img(c, r, 2)=B[r*W+c];
          }
      }
        std::string tempString2 = tempString + "_task_parallel.jpg";
        output_img.save_jpeg(tempString2.c_str());
      return 0;
   }        

};


int main (int argc, char *argv[]) {

  std::ifstream file(argv[1]);
  std::string tempString; 
  //unsigned char tempString; 
  std::vector<std::string> tasks;

  while (std::getline(file, tempString))
  {
    tasks.push_back(tempString);
  }

  tbb::task_scheduler_init init;
  tbb::tick_count total_end = tbb::tick_count::now();
  int count = 0;

  for(int i = 0; i < tasks.size(); i++)
  {
      swirlTask& a = *new(task::allocate_root()) swirlTask(tasks.at(i));

      if (count < tasks.size()-1 )
          {
           task::spawn(a);
          }
      else{
         task::spawn_root_and_wait(a);
     }
      count = count +1;
   }
  //usleep(1000000);
  double duration2 = ( tbb::tick_count::now()-total_end).seconds(); // (double) CLOCKS_PER_SEC;

  usleep(1000000);
  std::cout<<"\nTotal Run Time for tasking and parallel exection: "<< duration2 ;
  std::cout<<std::endl; 
  return 0;


}

