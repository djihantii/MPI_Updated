#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <assert.h>
#include "utils.h"

#define L 50000000
/*HT; i++ ) {
                MPI_Irecv(buf, count, datatype, MPI_ANY_SOURCE, MPI_ANY_TAG, my_topo.local_com, request);
                MPI_Test(request, &res, status);
                if(res)
                        break;
        }

        return res;
}


        Attente prolongée

int attente2(topo_t my_topo, void *buf, int count, MPI_Datatype datatype, MPI_Request *request, MPI_Status *status)
{
        int i = 0;
        while(i < L){
                int f = recevoir(res, buf, count, datatype, request, status);
                printf("Rec flag = %d; Status : MPI_SOURCE = %d; MPI_TAG = %d; int MPI_ERROR = %d;\n", f, status.MPI_SOURCE, status.MPI_TAG, status.MPI_ERROR);
                if(f){
                        i =0;
                        Traitement
                }
                else
                        i++;
        }
}
*/
int main(int argc, char *argv[]) {

        MPI_Init(&argc, &argv);
        int * initiateurs, l_initiateur, est_initiateur, size;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        topo_t res = create_topo("fileNetwork.txt", MPI_COMM_WORLD);
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
        printTab("Voisins", res.voisins, res.lon_v);
        printf("Je suis le procs %d\n", res.rank);
        est_initiateur = in(initiateurs, l_initiateur, res.rank);
        int sdata = 1, rdata = 0;
        MPI_Request sreq, rreq;
        MPI_Status status;
        if (est_initiateur) {
                /*envoi_tous_les_voisins_sauf(res, &sdata, 1, MPI_INT, 0, &sreq, NULL, 0);
                printf("J'envoie %d\n", sdata);*/

                /*traitement initiateur*/
        }

        /*attente2...*/


        /*else {
                int f = recevoir(res, &rdata, 1, MPI_INT, &rreq, &status);
                printf("Rec flag = %d; Status : MPI_SOURCE = %d; MPI_TAG = %d; int MPI_ERROR = %d;\n", f, status.MPI_SOURCE, status.MPI_TAG, status.MPI_ERROR);
                if(f)
                        printf("Je recois %d\n", rdata);
                else
                        printf("Je n'ai rien recu\n");
        }*/



        MPI_Finalize();
        return 0;
}
