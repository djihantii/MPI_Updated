#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <assert.h>
#include "utils.h"



int main(int argc, char *argv[]) {
        //printf("Je suis executé par le root \n");
        //FILE * file;
        //MPI_Comm new_comm;
        //int maxLines = MAX_NEIGHBOURS*(MAX_NEIGHBOURS-1)/2;
        //int rank, size, *sources, * degrees, * destinations, * weights, * initiateurs, l_initiateur;
        

        MPI_Init(&argc, &argv);

        /*MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    	MPI_Comm_size(MPI_COMM_WORLD,&size);
        readAndInterpreteFile("fileNetwork.txt", size,  rank, &sources, &degrees, &destinations, &weights);
        MPI_Dist_graph_create(MPI_COMM_WORLD, 1, sources, degrees, destinations, weights,  MPI_INFO_NULL, 0, &new_comm);
        
        int *nsources, * ndestinations, * noweights, * niweights, indegree, outdegree, weighted, est_initiateur ;
        
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
        
        printTab("Les initiateurs", initiateurs, l_initiateur);
        est_initiateur = in(initiateurs, l_initiateur, rank);
        printf("Statut initiateur : %d\n", est_initiateur);
        
        
        MPI_Dist_graph_neighbors_count(new_comm, &indegree, &outdegree, &weighted);
        //printf("Je suis le process %d, indegree = %d, outdegree = %d\n", rank, indegree, outdegree);
        nsources = calloc(indegree, sizeof(int));
        ndestinations = calloc(outdegree, sizeof(int));
        niweights = calloc(size, sizeof(int));
        noweights = calloc(size, sizeof(int));
        
        MPI_Dist_graph_neighbors(new_comm, indegree, nsources, niweights, size, ndestinations, noweights);*/
        
        topo_t res = create_topo("fileNetwork.txt", MPI_COMM_WORLD);
        printTab("Voisins", res.voisins, res.lon_v);

        MPI_Finalize();
        return 0;
}
