#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <assert.h>
#include <unistd.h>
#include "utils.h"

#define LOCAL_WAIT_TIME 500
#define TAG_EVEIL 1

typedef struct {
  int etat;
  topo_t topo;
} proc_data_t;

proc_data_t initialize(const char * filename){
  proc_data_t res;
  res.etat = 0;
  res.topo = create_topo(filename, MPI_COMM_WORLD);
  return res;
}

void traitement_initiateur(proc_data_t* pdata) {
  MPI_Request req;
  envoi_tous_les_voisins_sauf(pdata->topo, &(pdata->topo.rank), 1, MPI_INT, TAG_EVEIL, &req, NULL, 0);
  pdata->etat = 1;
}

void traitement_tous(proc_data_t* pdata) {
  int i = 0, rdata, sdata;
  printf("[%f]En ecoute\n", MPI_Wtime() );
  while (i < LOCAL_WAIT_TIME) {
    MPI_Request req;
    MPI_Status status;
    int f = recevoir(pdata->topo, &rdata, 1, MPI_INT, &req, &status);
    i++;
    if(f) {
      printf("Interpretation\n");
      switch (status.MPI_TAG) {
        case TAG_EVEIL:
          printf("Le signal est de type << eveil >>, recu de la part de %d\n", status.MPI_SOURCE);
          if(rdata != status.MPI_SOURCE){
                printf("!!!!donnee incoherente\n");
          }
          if(pdata->etat) {
            printf("Pas d'action a faire\n" );
          }
          else {
            MPI_Request mreq;
            printf("[%f]Propagation du signal\n", MPI_Wtime() );
            sdata = pdata->topo.rank;
            int sauf = status.MPI_SOURCE;
            envoi_tous_les_voisins_sauf(pdata->topo, &sdata, 1, MPI_INT, TAG_EVEIL, &mreq, &sauf, 1);
            pdata->etat = 1;
            printf("[%f]En ecoute\n", MPI_Wtime());
          }
          break;
        default :
          printf("Signal ignore [tag non reconnu]\n" );
      }
      i = 0;
    }
  }
  printf("[%f]Arret de l'ecoute [terminaison] (etat d'eveil: %d)\n", MPI_Wtime(), pdata->etat);
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int * initiateurs, l_initiateur, est_initiateur, size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  proc_data_t pdata = initialize("fileNetwork.txt");
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
  printTab("Voisins", pdata.topo.voisins, pdata.topo.lon_v);
  printf("Je suis le procs %d\n", pdata.topo.rank);
  est_initiateur = in(initiateurs, l_initiateur, pdata.topo.rank);
  if (est_initiateur) {
    traitement_initiateur(&pdata);
  }
  traitement_tous(&pdata);
  MPI_Finalize();
  return 0;
}
