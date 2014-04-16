/* 
 * File:   community-main.cpp
 * Author: charith
 *
 * Created on April 8, 2014, 9:48 AM
 */

#include <cstdlib>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <iostream> 
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <mpi.h>

#include "graph_binary.h"
#include "community.h"
using namespace std;


char *filename = NULL;
char *remoteEdgesFile = NULL;
//char *remoteEdgesFile = NULL;
char *filename_part = NULL;
int type = UNWEIGHTED;
int nb_pass = 0;
double precision = 0.000001;
int display_level = -2;
int k1 = 16;

bool verbose = false;

vector< pair<int, int> > remoteMap(10, make_pair(-1, -1));

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void
usage(char *prog_name, const char *more) {
    cerr << more;
    cerr << "usage: " << prog_name << " input_file [-w weight_file] [-p part_file] [-q epsilon] [-l display_level] [-v] [-h]" << endl << endl;
    cerr << "input_file: file containing the graph to decompose in communities." << endl;
    cerr << "-w file\tread the graph as a weighted one (weights are set to 1 otherwise)." << endl;
    cerr << "-p file\tstart the computation with a given partition instead of the trivial partition." << endl;
    cerr << "\tfile must contain lines \"node community\"." << endl;
    cerr << "-q eps\ta given pass stops when the modularity is increased by less than epsilon." << endl;
    cerr << "-l k\tdisplays the graph of level k rather than the hierachical structure." << endl;
    cerr << "\tif k=-1 then displays the hierarchical structure rather than the graph at a given level." << endl;
    cerr << "-v\tverbose mode: gives computation time, information about the hierarchy and modularity." << endl;
    cerr << "-h\tshow this usage message." << endl;
    exit(0);
}

void
parse_args(int argc, char **argv) {
    if (argc < 2)
        usage(argv[0], "Bad arguments number\n");

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'r':
                    //type = WEIGHTED;
                    remoteEdgesFile = argv[i + 1];
                    i++;
                    break;
                case 'p':
                    filename_part = argv[i + 1];
                    i++;
                    break;
                case 'q':
                    precision = atof(argv[i + 1]);
                    i++;
                    break;
                case 'l':
                    display_level = atoi(argv[i + 1]);
                    i++;
                    break;
                case 'k':
                    k1 = atoi(argv[i + 1]);
                    i++;
                    break;
                case 'v':
                    verbose = true;
                    break;
                default:
                    usage(argv[0], "Unknown option\n");
            }
        } else {
            if (filename == NULL)
                filename = argv[i];
            else if (remoteEdgesFile == NULL)
                remoteEdgesFile = argv[i];
            else
                usage(argv[0], "More than one filename\n");
        }
    }
}

void
display_time(const char *str) {
    time_t rawtime;
    time(&rawtime);
    cerr << str << ": " << ctime(&rawtime);
}

/*
 * 
 */
int main(int argc, char** argv) {

    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    parse_args(argc, argv);

    stringstream rankS;
    rankS << rank;

    string s(filename);
    s += "_" + rankS.str() + ".bin";

    string r(remoteEdgesFile);
    r += "_" + rankS.str() + ".remote";

    vector<pair<int, int> > remoteMap;

    ifstream remoteFileStream(r);
    int remoteEdgeCount = 0;

    
    
    vector<int> rSource;
    vector<int> rSink;
    vector<int> rpart;
   
    for (string line; getline(remoteFileStream, line);) {
        vector<string> parts = split(line, ' ');
        string localV = parts[0];
        string mapping = parts[1];
        int source = atoi(localV);

        vector<string> mappingParts = split(mapping, ',');

        int sink = atoi(mapping[0]);
        int partition = atoi(mapping[1]);


        if (remoteMap.size() <= source) {
            s
            remoteMap.resize(source + 1, make_pair(-1, -1));

        }

        remoteMap[source] = make_pair(sink, partition);
        
        rSource.push_back(source);
        rSink.push_back(sink);
        rpart.push_back(partition);
        
    }








    if (verbose)
        display_time("Begin");
    char* tmp = new char[s.length() + 1];
    strcpy(tmp, s.c_str());
    Community c(tmp, NULL, type, -1, precision);
    if (filename_part != NULL)
        c.init_partition(filename_part);
    Graph g;
    bool improvement = true;
    double mod = c.modularity(), new_mod;
    int level = 0;

    if (verbose) {
        cerr << "level " << level << ":\n";
        display_time("  start computation");
        cerr << "  network size: "
                << c.g.nb_nodes << " nodes, "
                << c.g.nb_links << " links, "
                << c.g.total_weight << " weight." << endl;
    }

    improvement = c.one_level();
    new_mod = c.modularity();
    if (++level == display_level)
        g.display();
    if (display_level == -1)
        c.display_partition();
    g = c.partition2graph_binary();
   // c = Community(g, -1, precision);

    if (verbose)
        cerr << "  modularity increased from " << mod << " to " << new_mod << endl;

    mod = new_mod;
    if (verbose)
        display_time("  end computation from " + rank);
    if (verbose)
        cerr << "Start sending data" << endl;

    MPI_Send(&g.nb_links, 1, MPI_LONG, 0, 1, MPI_COMM_WORLD);
    MPI_Send(&g.nb_nodes, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    MPI_Send(&g.total_weight, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    MPI_Send(&g.links.front(), g.links.size(), MPI_INT, 0, 1, MPI_COMM_WORLD);
    MPI_Send(&g.degrees.front(), g.degrees.size(), MPI_LONG, 0, 1, MPI_COMM_WORLD);
    MPI_Send(&g.weights.front(), g.weights.size(), MPI_FLOAT, 0, 1, MPI_COMM_WORLD);

    MPI_Send(&rSource.front(), rSource.size(), MPI_INT,0,1,MPI_COMM_WORLD);
    MPI_Send(&rSink.front(), rSink.size(), MPI_INT,0,1,MPI_COMM_WORLD);
    MPI_Send(&rpart.front(), rpart.size(), MPI_INT,0,1,MPI_COMM_WORLD);
   
    MPI_Send(&c.n2c_new.front(), c.n2c_new.size(), MPI_INT,0,1,MPI_COMM_WORLD);
    
    if (rank == 0) {

        //get graph parts. construct
        //new graph
        //execute louvan
        
        
        


    }

    MPI_Finalize();

    return 0;
}

