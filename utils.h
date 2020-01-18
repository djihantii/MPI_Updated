#ifndef LOCAL_UTILS_MPI 
#define LOCAL_UTILS_MPI


#include <mpi.h>


int in(int * tab, int length, int val) ;

void printTab(const char * title, int * tab, int length) ;

void readAndInterpreteFile(const char * filename, int nprocess,  int rank, int ** sources, int ** degrees, int ** destinations, int ** weights) ;

typedef struct {
        int * voisins;
        int lon_v;
        MPI_Comm local_com;
        int rank;
        int comworld_size;
} topo_t;

topo_t create_topo(char * filename, MPI_Comm old_comm);

void envoi_tous_les_voisins_sauf(topo_t my_topo, const void *buf, int count, MPI_Datatype datatype, int tag, MPI_Request *request, int * sauf, int lon_sauf) ;

int attente(topo_t my_topo, void *buf, int count, MPI_Datatype datatype, MPI_Request *request, MPI_Status *status);

#endif
