#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "utils.h"

#define WAIT_LENGHT 200

int in(int * tab, int length, int val) {
        int i;
        for (i = 0; i < length; i++)
                if(tab[i] == val)
                        return 1;
        return 0;
}

void printTab(const char * title, int * tab, int length) {
        printf("Title : %s\n", title);
        int i;
        printf("Value : [");
        for (i = 0; i < length; i++)
                printf("%d, ", tab[i]);
        printf("]\n");
}

void readAndInterpreteFile(const char * filename, int nprocess,  int rank, int ** sources, int ** degrees, int ** destinations, int ** weights) {
        FILE * file =  fopen(filename , "r");
        char * aLine = NULL;
        int scanf_nbread, s, d, line_number = 1, local_dest, i; 
        size_t len;
        ssize_t read;
        *sources = calloc(1, sizeof(int));
        *degrees = calloc(1, sizeof(int));
        *destinations = calloc(nprocess, sizeof(int));
        * weights = calloc(nprocess, sizeof(int));
        for (i = 0; i < nprocess; i++) {
                (* weights)[i] = 1;
        }
        assert (file != NULL);
        assert (nprocess > 0);
        assert (rank >= 0);
        **sources = rank; 
        while((read = getline(&aLine , &len, file)) != -1 ){
                scanf_nbread = sscanf(aLine, "%d-%d", &s, &d);
                //printf("line : %s", aLine);
                if (scanf_nbread < 2)
                {
                        printf("Unable to parse line #%d : %s\n", line_number, aLine);
                }
                else {
                        assert( s >= 0 && d >= 0 && s < nprocess && d < nprocess);
                        if ((s == rank || d == rank) && (s != d)) {
                                if(s == rank){
                                        local_dest = d;
                                }
                                else {
                                        local_dest = s;
                                }
                                if (!in( *destinations, **degrees, local_dest)) {
                                        (* destinations)[**degrees] = local_dest;
                                        (**degrees)++; 
                                }
                                
                        }
                }
                line_number ++;
        }
        if (aLine) free(aLine);
        fclose(file);
}


topo_t create_topo(char * filename, MPI_Comm old_comm){
        topo_t res;
        MPI_Comm new_comm;
        int rank, size, *sources, * degrees, * destinations, * weights;
        int *nsources, * ndestinations, * noweights, * niweights, indegree, outdegree, weighted;
        MPI_Comm_rank(old_comm, &rank);
    	MPI_Comm_size(old_comm, &size);
        readAndInterpreteFile(filename, size,  rank, &sources, &degrees, &destinations, &weights);
        MPI_Dist_graph_create(old_comm, 1, sources, degrees, destinations, weights,  MPI_INFO_NULL, 0, &new_comm);
        MPI_Dist_graph_neighbors_count(new_comm, &indegree, &outdegree, &weighted);
        nsources = calloc(indegree, sizeof(int));
        ndestinations = calloc(outdegree, sizeof(int));
        niweights = calloc(size, sizeof(int));
        noweights = calloc(size, sizeof(int));
        MPI_Dist_graph_neighbors(new_comm, indegree, nsources, niweights, size, ndestinations, noweights);
        res.voisins = ndestinations;
        res.lon_v = outdegree;
        res.local_com = new_comm;
        res.rank = rank;
        res.comworld_size = size;
        free(sources);
        free(degrees);
        free(destinations);
        free(weights);
        free(nsources);
        free(niweights);
        free(noweights);
        return res;
} 

void envoi_tous_les_voisins_sauf(topo_t my_topo, const void *buf, int count, MPI_Datatype datatype, int tag, MPI_Request *request, int * sauf, int lon_sauf) {
        for(int i = 0; i<my_topo.lon_v; i++) {
                if (!in(sauf, lon_sauf, my_topo.voisins[i])) {
                        MPI_Isend(buf, count, datatype, my_topo.voisins[i], tag, my_topo.local_com, request);
                }
                
        }
}

int attente(topo_t my_topo, void *buf, int count, MPI_Datatype datatype, MPI_Request *request, MPI_Status *status) {
        int res; 
        for(int i = 0; i<WAIT_LENGHT; i++ ) {
                MPI_Irecv(buf, count, datatype, MPI_ANY_SOURCE, MPI_ANY_TAG, my_topo.local_com, request);
                MPI_Test(request, &res, status);
                if(res)
                        break;
        }
        
        return res;
}






