export void convert_grayscale_parallel(uniform int R[],uniform int G[],uniform int B[], uniform int W, uniform int H){ 
  
  foreach (index=0 ... W*H){
        float avg = (R[index]*77 + G[index]*151 + B[index]*28) / 3;
        R[index] = G[index] = B[index]  = avg;
  }

}

export void swirl_parallel(uniform int R2[], uniform int G2[], uniform int B2[],uniform int R[], uniform int G[], uniform int B[], uniform int W, uniform int H ){
        float centerx = (W)/2;
        float centery = (H)/2;
        float radius = 0;
        int temp_index = 0;
        if ( centerx > centery)
            radius = centerx;
        else
            radius = centery;

        float radius2 = radius * radius;

        foreach (index = 0 ... (W*H)) {
               float y = index/W;
               float x  = index%W;     
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
                        R[index] =R2[temp_index];
                        G[index] =G2[temp_index];
                        B[index] =B2[temp_index];
                    }
              }
       }
}

