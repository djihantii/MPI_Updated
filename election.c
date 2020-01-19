#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "utils.h"

#define LOCAL_WAIT 500

typedef struct{
  topo_t topology;
  int nombre_diffusion;
  int * voisins;
  int maxId;
}state_election;

int maxId(int a, int b){
  if (a>b) {
    return a;
  }
  return b;
}

int min(int * tableau, int taille){
  int i, min = tableau[0];
  for(i=0; i<taille; i++){
    if(min < tableau[i])
      min = tableau[i];
  }
  return min;
}


int recoverIndex(int rank, state_election *process){
  for (int i = 0; i < process->topology.lon_v; i++) {
    if (process->topology.voisins[i] == rank) {
      return i;
    }
  }
  return -1;
}


state_election initialiser_attributs(char * filename){
  state_election process;
  process.topology = create_topo(filename, MPI_COMM_WORLD);
  process.nombre_diffusion=0;
  process.voisins = malloc(process.topology.lon_v*sizeof(int));
  for (int i = 0; i < process.topology.lon_v; i++) {
    process.voisins[i]=0;
  }
  process.maxId = process.topology.rank;

  return process;
}

void diffusion(state_election * process, int *elu){
  process->nombre_diffusion +=1;
  MPI_Request sreq;

  envoi_tous_les_voisins_sauf(process->topology, &process->maxId, 1, MPI_INT, 0, &sreq, NULL, 0);
}
//Si initiateur alors lancer la diffusion
void premiere_diffusion(state_election *process, int * list_initiateurs , int number_init, int *elu){
  if(!in(list_initiateurs, number_init, process->topology.rank)){
    process->maxId = -1;
  }
  if(in(list_initiateurs, number_init, process->topology.rank)){
    diffusion(process, elu);
  }
}

//

void fin(state_election * process , int *elu){
  if(*elu == -1){
    *elu = process->maxId;
    // MPI_Bcast(&elu, 1, MPI_INT, process.topology.rank, process.local_com);
  }
}


void reception(state_election * process, int diametre, int *elu){
  int accumulateur=0;
  int rdata, recv_rank, index_rank, min_voisins;
  while(accumulateur < LOCAL_WAIT){
    MPI_Request req;
    MPI_Status status;
    int f = recevoir(process->topology, &rdata, 1, MPI_INT, &req, &status);
    accumulateur++;
    if(f){
      printf("Interprétation \n");
      process->maxId = maxId(rdata, process->maxId);
      recv_rank = status.MPI_SOURCE;
      index_rank = recoverIndex(recv_rank, process );
      process -> voisins[index_rank] +=1;

      min_voisins = min(process->voisins , process->topology.lon_v);
      if(min_voisins >= process->nombre_diffusion && process->nombre_diffusion < diametre){
        diffusion(process, elu);
      }

      if(min_voisins >= diametre){
        fin(process, elu);
      }

      accumulateur=0;
    }

  }
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int * initiateurs, l_initiateur, size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  state_election process = initialiser_attributs("fileNetwork.txt");
  initiateurs = calloc(size, sizeof(int));
  l_initiateur = 0;
  for( int i = 1; i < argc; i++) {
          int to_add = atoi( argv[i] );
          if (to_add<0 || to_add>=size)
          {
                  printf("Initiateur incorrect : %d (parsed from '%s')\n", to_add, argv[i]);
          }
          else if (!in(initiateurs, l_initiateur, to_add)){
                  initiateurs[l_initiateur] = to_add;
                  l_initiateur++;
          }
  }

  int elu = -1;
  int diametre = 4;

  premiere_diffusion(&process, initiateurs, l_initiateur, &elu);
  reception(&process, diametre, &elu);

  MPI_Bcast(&elu, 1, MPI_INT, elu, process.topology.local_com);
  printf("l'elu est %d \n", elu);
  MPI_Finalize();
  return 0;

}
