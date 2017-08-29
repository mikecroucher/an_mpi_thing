#include <stdio.h>
#include <mpi.h>
#include <unistd.h>

void calc_foo(int input)
/* Does some calculation that depends on the value of input*/
{
   printf("I am calc_foo and I work on %d\n",input);
   /*sleep(input);*/
   printf("work done\n");
}

void request_work()
{
   int message=1;
   MPI_Send(&message, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
}

int receive_work()
{
    int work;
    MPI_Status status;
    MPI_Recv(&work, 1, MPI_INT, 0 , 0, MPI_COMM_WORLD,&status);
    return(work);
}

int listen_for_work_request(){
   MPI_Status status;
  
   int process_requesting_work,is_there_work;
   MPI_Recv(&is_there_work, 1, MPI_INT, MPI_ANY_SOURCE , 0, MPI_COMM_WORLD,&status);
   process_requesting_work = status.MPI_SOURCE;
   return(process_requesting_work);
}


void send_work(int work,int destination_rank)
{
   MPI_Send(&work, 1, MPI_INT, destination_rank, 0, MPI_COMM_WORLD);
}

void no_more_work(int size)
{
  int worker;
  for(worker=1; worker < size; worker++)
  {
     send_work(-1,worker);
  }
}

int main (argc, argv)
     int argc;
     char *argv[];
{
  int rank, size,number_to_send,number_received,destination_rank,message;
  int worker_wanting_work;
  int work_item;
  int work;
  MPI_Init (&argc, &argv);                      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */
  int numbers_to_send = 10;
  message = 1;

  if (rank == 0) {
         /* send initial work to each worker */
         for(work_item = 0;work_item < size-1;work_item++){
             destination_rank = work_item + 1;
             printf("I am master and I am about to send %d\n",work_item);
             send_work(work_item,destination_rank);
         }
         
         while(work_item < numbers_to_send){
           work_item++;
           printf("Listening for work\n");
           worker_wanting_work = listen_for_work_request();
           printf("I am in the main loop and about to send %d\n",work_item);
           send_work(work_item,worker_wanting_work);
         }

         /*Tell all workers that there's no more work*/
         no_more_work(size);
         printf("Time to die\n");
     
      } else  
        {
         while(work >= 0){
            request_work();
            work = receive_work();
            printf("I am %d and I got %d\n",rank,work);
            calc_foo(work);
            }
        }


  MPI_Finalize();
  return 0;
}
