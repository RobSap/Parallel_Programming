#include <iostream>
#include <fstream>
#include <time.h>
#include <ctime>
#include <ratio>
#include <chrono>
#include <string>
#include <stdio.h>
#include <string.h>
#include <cstdio>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <mpi.h>

using namespace std;
using namespace std::chrono;


int main(int argc, char *argv[])
{

//*******************
//
// Declare clock times, and start program clock
//
//******************
    high_resolution_clock::time_point t1;// = high_resolution_clock::now();
    high_resolution_clock::time_point t2;
    high_resolution_clock::time_point total = high_resolution_clock::now();
    high_resolution_clock::time_point roundStart;// = high_resolution_clock::now();
    high_resolution_clock::time_point roundEnd;
    high_resolution_clock::time_point timeWriteStart;
    high_resolution_clock::time_point timeWriteEnd;
    high_resolution_clock::time_point startReadTimes = high_resolution_clock::now();
    high_resolution_clock::time_point endReadTimes;// = high_resolution_clock::now();
 

//*******************
//
// Start MPI declarations, and start MPI
//
//******************
   
    int numtasks, taskid, rc, dest, offset, i, tag1, tag2, source, chunksize;
    
    //Status of MPI
    MPI_Status status;
    //Init number of processes
    MPI_Init(&argc, &argv);
    //This tells you the number of tasks (processes) we have total, for our world
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    //This gives each process a rank, so each process has a taskid = their id .. exampe: process zero taskid = 0, process one taskid = 1
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

    char *graphFile, * partitionFile, *output_file;


//*******************
//
// Start reading in files, declare mmap read in 
//
//******************
    graphFile = argv[1];
    partitionFile = argv[2];
    int rounds = strtol(argv[3], NULL, 10); //argv[3];
    char* partitions = argv[4];

    char *f;
    int size;
    struct stat s;

    string readInFile = graphFile;
    string readInFile2 = partitionFile;


    int fd = open (argv[1], O_RDONLY);



    /*   Get the size of the file. */


    int statusRead = fstat (fd, &s);
    size = s.st_size;

    f = (char *) mmap (0, size, PROT_READ, MAP_PRIVATE, fd, 0);

    int j = 0;

    char buffer[32];
    char buffer2[32];
    int input;
    int input2;

    int largest = 0;
    int lines = 0;

//*******************
//
// Find the largest node size in the file
//
//******************
    for (long long i = 0; i < size; i++) {

        if(f[i] == '\n')
        {
            buffer[j] = '\0';
            input  = atoi(strtok(buffer,"\t"));
            input2  = atoi(strtok(NULL,"\t"));

            if(input > largest)
                largest=input;
            if(input2 > largest)
                largest=input2;

            //cout << "C: " << input  << " b: " << input2 << endl;
            j=0;
            lines+=2;
        }

        buffer[j] = f[i];
        j++;

    }
 
//*******************
//
// Declare the array size, and allocate mem for all processes
//
//******************


    int * edges = new int[lines]();
    int * degrees = new int[largest+1]();
    double * creditsArray = new double[(largest+1)*(rounds+1)]();	
    double * partitionsArray = new double[largest+1]();
    double * creditsArray2 = new double[(largest+1)*(rounds+1)](); 

//*******************
//
// Now read in the file 
//
//****************** 


    j = 0;
    int k = 0;
    for (long long i = 0; i < size; i++) {

        if(f[i] == '\n')
        {
            buffer[j] = '\0';
            input  = atoi(strtok(buffer,"\t"));
            input2  = atoi(strtok(NULL,"\t"));


            //cout << "input: " << input  << " input2: " << input2 << endl;

            edges[k] = input;
            k++;
            //degrees[input]+=1;

            edges[k] = input2;
            k++;
            //cout << "input2: " << input2  << " input1: " << input << endl;
            j=0;
            //lines++;
        }


        buffer[j] = f[i];
        j++;

    }


//*******************
//
// Set up for partition file
//
//******************
    int node;
    int degree;
    int belongToPartition;

    char *f2;
    int size2;
    struct stat s2;

    int fd2 = open (argv[2], O_RDONLY);

//*******************
//
//  Read in partition file
//
//******************
    int status2 = fstat (fd2, &s2);
    size2 = s2.st_size;

    f2 = (char *) mmap (0, size2, PROT_READ, MAP_PRIVATE, fd2, 0);

    j = 0;

    for (long long i = 0; i < size2; i++) {

        if(f2[i] == '\n')
        {
            buffer2[j] = '\0';
            node  = atoi(strtok(buffer2,"\t"));
            degree  = atoi(strtok(NULL,"\t"));
            belongToPartition  = atoi(strtok(NULL,"\t"));

            //cout << "**********node " << node << " degree " << degree << " belongToPartition " << belongToPartition <<endl;
            degrees[node] = degree;
	    partitionsArray[node]=belongToPartition;


            j=0;
        }

        buffer2[j] = f2[i];
        j++;

    }


//*******************
//
// Initialize round 0 to score of 1
//
//******************


        //cout << "it 1" <<endl;
        for(int i = 1; i<largest+1; i++)
        {
                //creditsOld[i]=1;
		creditsArray[i]=1;

        }

//Print out to ensure set up is correct
/*	for(int i = 1; i<((rounds+1)*largest+1); i++)
        {       
                //creditsOld[i]=1;
               cout << creditsArray[i] << endl;
        
        }
*/


	//Reading files and initial array set ups are now done, print time
        endReadTimes = high_resolution_clock::now();
        duration<float> time_span = duration_cast<duration<float> >( endReadTimes-startReadTimes);


    cout << "time to read input files, partition " << taskid<<" = " << time_span.count() << " sec"<<endl;
	
	int tag = 0;

    
	for(int z = 1; z <rounds+1; z++)
    	{
            //Let all processes get to this point, and start round clock and calcuations
	    MPI_Barrier(MPI_COMM_WORLD);
            roundStart = high_resolution_clock::now();
            t1= high_resolution_clock::now();
             //t1 = high_resolution_clock::now();
            int countSends =0;
            //t1 = high_resolution_clock::now();
            
         
        		for(int i = 0; i< lines-1; i+=2)
        		{   
        			//The two iffs let me only calcuate for the nodes the proces owns
   				if(taskid==partitionsArray[(int)edges[i]])
				{

					 //cout << (z*largest)+edges[i] << endl; 
  //                           		cout << "stored in spot: " << (z)*largest+edges[i] << "  " << creditsArray[(z)*largest+edges[i]]<< " + (" << (float) (creditsArray[(((z-1)*largest)+edges[i+1] )]) << " /  " << (float)(degrees[edges[i+1]]) << " )" << endl;

					//cout << "before : " << creditsArray[(z)*(largest)+edges[i]] <<endl;
					creditsArray[(z)*(largest)+edges[i]]= creditsArray[(z)*(largest)+edges[i]]  + (double) (creditsArray[(((z-1)*(largest))+edges[i+1] )]) / (double)(degrees[edges[i+1]]);
					//cout <<  "after: " << creditsArray[(z)*(largest)+edges[i]] << endl;
				}

				if( taskid==partitionsArray[(int)edges[i+1]])
 				{

					//cout << (z*largest)+edges[i+1] << endl;
				
				//	cout << "before : " << creditsArray[(1+z)*(largest)+edges[i+1]] << endl;
//					cout << "store in spot : " << (z)*largest+edges[i+1] << " " <<  creditsArray[(z)*largest+edges[i+1]] <<" + ( " << (float) (creditsArray[((z-1)*largest)+edges[i]]) << " / " << (float)(degrees[edges[i]]) << " )" << endl;
				

					 creditsArray[(z)*(largest)+edges[i+1]]= creditsArray[(z)*(largest)+edges[i+1]]  + (double) (creditsArray[((z-1)*largest)+edges[i]]) / (double)(degrees[edges[i]]);
					//cout <<  creditsArray[(1+z)*(largest)+edges[i+1]] <<endl;
			//
			//		cout << "after " << creditsArray[(1+z)*(largest)+edges[i+1]] <<endl;
				}
                	}
			
			  //Used for troubleshooting
		          //cout <<"finished the ones i know " <<endl; 
//			for(int i = 0; i< (1+rounds)*largest+1; i++)
//                        {
//				 cout << creditsArray[i] <<endl;
//			  }           


		//Only do this task 
  		if(numtasks>1)
  		{
        		for(int i = 1; i<(largest)+1  ; i++)
        		{	
        		    //cout << i << " ";
				creditsArray2[i+(largest)*(z)]=creditsArray[i+(largest)*(z)];
		    
                //	   	cout << "a Node " << i+(largest)*(z) << " round " <<z << " credits "  <<creditsArray[i+(largest)*(z)];// <<endl;  
                //		cout <<endl;
        		}
 
			MPI_Barrier(MPI_COMM_WORLD);	
			MPI_Allreduce(creditsArray2, creditsArray, (largest+1)*(rounds+1), MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);	
			MPI_Barrier(MPI_COMM_WORLD);
   		 
        		t2 = high_resolution_clock::now();
        		time_span = duration_cast<duration<float> >(t2 - t1);
        
        
        		cout << "---total time for round " << (z) << ", partition " <<taskid<<" = " << time_span.count() << " sec"<<endl;
        
		}
            
          
            MPI_Barrier(MPI_COMM_WORLD);
            roundEnd = high_resolution_clock::now();
            time_span = duration_cast<duration<float> >(roundEnd - roundStart);
            if(taskid==0)
            {
                cout << "total time for round " << (z) << " = " << time_span.count() << " sec"<<endl;
            }

    } //End rounds

//********************
//
// Have each process generate a file to write to
//
//*******************
    ofstream myOutputFile;
    string temp9 = to_string(taskid);
    string message = "Partition" + (temp9 + ".txt");
    myOutputFile.open (message);
    //myOutputFile << "Writing this to a file.\n";


    timeWriteStart = high_resolution_clock::now();
    

//********************
//
// Print out section, this is optimized for most likely rounds, (5) and works for all number of rounds
//
//*******************

    //cout <<"made it here " <<endl;
    for(int i = 1; i<(largest)+1  ; i++)
    {

        
        string temp10 = ""; //to_string(taskid);

        if(taskid==partitionsArray[i])
        {
            myOutputFile << i << " " << degrees[i] << " ";
                      		//cout << "Node " << i << " " ;
        }
                        //**************************
        if(rounds==5)
        {
            if(taskid==partitionsArray[i])
            {
                           
                myOutputFile << (creditsArray[i+(largest*1)]) <<  " "<< (creditsArray[i+(largest*2)]) <<  " "<< (creditsArray[i+(largest*3)]) <<  " "<< (creditsArray[i+(largest*4)]) <<  " "<< (creditsArray[i+(largest*5)]) <<  " ";
            }

        }
        else if(rounds==4)
        {
            if(taskid==partitionsArray[i])
            {
                            
                myOutputFile << (creditsArray[i+(largest*1)]) <<  " "<< (creditsArray[i+(largest*2)]) <<  " "<< (creditsArray[i+(largest*3)]) <<  " "<< (creditsArray[i+(largest*4)]) <<  " ";
            }
        }
        else if(rounds==3)
        {
            if(taskid==partitionsArray[i])
            {
                            
                myOutputFile << (creditsArray[i+(largest*1)]) <<  " "<< (creditsArray[i+(largest*2)]) <<  " "<< (creditsArray[i+(largest*3)]) <<  " ";

            }
        }
        else if(rounds==2)
        {
            if(taskid==partitionsArray[i])
            {
                            
                myOutputFile << (creditsArray[i+(largest*1)]) <<  " "<< (creditsArray[i+(largest*2)]);
            }
        }
        else
        {
       	    //For unknown size , all other 
                        //*************************
            for( int j = 1; j < rounds+1 ; j++)
            {

                if(taskid==partitionsArray[i])
                {
                        	     //cout<<creditsArray[i+(largest*j)]<< " ";
                            	 myOutputFile << (creditsArray[i+(largest*j)]) <<  " "; 
                            	//cout << "Node " << i << " round " <<j << " credits "  <<creditsArray[i+(largest*j)]<< " "; // <<endl;  
                }
            }
        }
        if(taskid==partitionsArray[i])
        {
                        //cout <<endl;
                     	myOutputFile<<endl;
        }
        
        
        
    }

    myOutputFile.close();
    timeWriteEnd = high_resolution_clock::now();
    time_span = duration_cast<duration<float> >(timeWriteEnd-timeWriteStart);
    cout << "write file for partition : " <<taskid<< " " << time_span.count() << " sec "<<endl;
    

 	

//********************
//
// Free all memery, get the final time and print
//
//*******************


    delete(edges);
    delete(degrees);
    
    delete(creditsArray);
    delete(partitionsArray);
    delete(creditsArray2);

	MPI_Barrier(MPI_COMM_WORLD);

	if(taskid==0)
        	{
			t2 = high_resolution_clock::now();
        		time_span = duration_cast<duration<float> >(t2 - total);
        		cout << "total time for all processes : " << " " << time_span.count() << " sec"<<endl;
		}

	
	MPI_Finalize();

	//Check total time for all process each
	//t2 = high_resolution_clock::now();
        //time_span = duration_cast<duration<float> >(t2 - total);
        //cout << "total time  for process : "<< taskid << " " << time_span.count() << " sec"<<endl;


return 0;


}
