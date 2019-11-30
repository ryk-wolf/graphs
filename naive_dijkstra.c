/*Author Alex Vigneron */

#include "graphe.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h> /*DBL_MAX*/

/*********************************MACROS***************************************/
#define CFALSE 0
#define CTRUE 1
#define ERROR -1
#define EMPTY -1


/***********************TYPEDEFS & GLOBAL VARIABLES****************************/
typedef enum {RED, BLUE, GREEN} tCouleur;
tCouleur coloursArray[MAX_SOMMETS];
tNumeroSommet pred[MAX_SOMMETS];
tValeurArc pi[MAX_SOMMETS];
tNumeroSommet path[MAX_SOMMETS];
int checked[MAX_SOMMETS];


/***********************TOOL FUNCTIONS AND PROCEDURES**************************/

/*Colours all nodes in blue */
void colour_all_in_blue(tGraphe g){
  int i, nb_nodes;
  nb_nodes = grapheNbSommets(g);
  for(i=0 ; i < nb_nodes ; i++) {
    coloursArray[i] = BLUE;
  }
}


/*Initialises checked array*/
void init_checked(){
  int i;
  for(i=0; i<MAX_SOMMETS; i++){
    checked[i] = CFALSE;
  }
}


/*Initialises values for pi array, by convention,
all paths should be of infinite length until discovered*/
void init_pi(){
  int i;
  for(i=0; i<MAX_SOMMETS; i++){
    pi[i] = DBL_MAX;
  }
}


/*Initialises values for predecessor's array*/
void init_pred(){
  int i;
  for(i=0; i<MAX_SOMMETS; i++){
    pred[i] = EMPTY;
  }
}


/*Initialises values of path array*/
void init_path(){
  int i;
  for(i=0; i<MAX_SOMMETS; i++){
    path[i] = EMPTY;
  }
}


/*Calls all necessary initialising procedures*/
void init_all(tGraphe g){
  colour_all_in_blue(g);
  init_pi();
  init_pred();
  init_path();
}


/*Returns index of next green node if any, -1 otherwise*/
tNumeroSommet exists_some_green_node(){
  int i;
  for (i = 0; i < MAX_SOMMETS; i++) {
    /*If green and never tried before*/
    if ((coloursArray[i] == GREEN) && (checked[i] == CFALSE)){
      checked[i] = CTRUE; /*Flagging as already tried*/
      return (tNumeroSommet)i;
    }
  }
  return EMPTY;
}


/*Returns index of node with minimum pi among green nodes, -1 if none found*/
tNumeroSommet get_min_pi(){
  tNumeroSommet wanted_index, curr_index;
  tValeurArc min_pi, tmp_pi;
  min_pi = DBL_MAX;
  wanted_index = ERROR; /*default*/

  init_checked();

  while( (curr_index = exists_some_green_node()) != EMPTY ) {
    tmp_pi = pi[curr_index];
    if(tmp_pi <= min_pi){ /*A better way has been found, minimum is updated*/
      min_pi = tmp_pi;
      wanted_index = curr_index; /*Keeping track of best node*/
    }
  } /*By now, we have found the smallest valued path*/
  return wanted_index;
}


void calculate_path(tGraphe g, tNomSommet start_node, tNomSommet end_node){
  int i;
  tNumeroSommet curr_node_index, start_node_index, end_node_index;
  i = MAX_SOMMETS;

  /*Initialising*/
  init_path();
  start_node_index = grapheChercheSommetParNom(g, start_node);
  end_node_index = grapheChercheSommetParNom(g, end_node);
  curr_node_index = end_node_index; /*Start from the end*/

  /*Will execute while we haven't got back to the beginning*/
  while(start_node_index != curr_node_index){
  path[i] = curr_node_index; /*Storing the nodes in the end to access the beginning later*/
  i--;
  /*For next iteration, the node should be the predecessor of the current one*/
  curr_node_index = pred[curr_node_index];
  }
}


/*Prints to standard output the path calculated with calculate_path*/
void display_path(tGraphe g){
  int i;
  tNomSommet curr_node_name;

  for (i = 0; i < MAX_SOMMETS; i++) { /*All nodes...*/
    if(path[i] != EMPTY){ /*... IIF they are part of the best path and not just empty*/
    grapheRecupNomSommet(g, path[i], curr_node_name);
    printf(" %s", curr_node_name);
    }
  }
  printf("\n");
}


/*Prints to standard output the values of the optimal paths and
the corresponding nodes which should be visited*/
void display_dijkstra_results(tGraphe g, tNumeroSommet start_node){
  int i, nb_nodes;
  tNomSommet curr_name, first_name;

  nb_nodes = grapheNbSommets(g);
  grapheRecupNomSommet(g, start_node, first_name);

  for(i = 0 ; i < nb_nodes ; i++){
    grapheRecupNomSommet(g, (tNumeroSommet)i, curr_name);
    calculate_path(g, first_name, curr_name);
    printf("Shortest path from %s to %s = %f \t:", first_name, curr_name, pi[i]);
    display_path(g);
  }
}


/*******************************CORE FUNCTION**********************************/

void dijkstra(tGraphe g, tNumeroSommet node){
  int i, curr_nb_succesor;
  tNumeroSommet curr_node_index, curr_succ_index;
  tValeurArc curr_node_pi, distance_between_nodes, curr_total_distance;

  /*Check whether graph is oriented or not, exit if not */
  if(!(grapheEstOriente(g))){
    fprintf(stdout, "The graph given as argument is not oriented, no topological sort possible\n");
    exit(EXIT_FAILURE);
  } /*Reaching here means we've made sure the graph is oriented*/

  /*Initialising*/
  init_all(g);
  coloursArray[(int)node] = GREEN; /*Colour starting node in green to begin*/
  pi[node] = 0; /*The path from start node to itself should be 0*/

  /*This will execute while there are green nodes left*/
  while((curr_node_index = get_min_pi()) != ERROR){ /*x as described in handout*/
    coloursArray[curr_node_index] = RED;
    /*Checks whether it has already been updated or not,
    if the value is a real updated value it is used, otherwise it's set to 0*/
    curr_node_pi = pi[curr_node_index] >= (DBL_MAX-1)? 0 : pi[curr_node_index];

    curr_nb_succesor = grapheNbSuccesseursSommet(g, curr_node_index);
    for( i = 0; i < curr_nb_succesor; i++){
      curr_succ_index = grapheSuccesseurSommetNumero(g, curr_node_index, i); /*y as described in handout*/
      distance_between_nodes = grapheRecupValeurArc(g, curr_node_index, curr_succ_index);
      curr_total_distance = curr_node_pi + distance_between_nodes;

      /*Either it has never been visited before or it has been visited before but through a longer path*/
      if(coloursArray[curr_succ_index] == BLUE ||
        (coloursArray[curr_succ_index] == GREEN && pi[curr_succ_index] > curr_total_distance )) {
          coloursArray[curr_succ_index] = GREEN;
          pi[curr_succ_index] = curr_total_distance; /*Updating shortest distance*/
          pred[curr_succ_index] = curr_node_index; /*Updating predecessor with shortest path*/
        }
      }
    }
  }



/****************************DRIVER FOR TESTING********************************/

  int main(int argc, char ** argv){
    tGraphe graphe;

    if (argc<3){
      printf("%s\n","Use as :\t./dijkstra [grp file name] [start node name]");
      exit(EXIT_FAILURE);
    }

    printf("\nExecuting Dijkstra's algorithm, starting from %s.\n", argv[2]);

    graphe = grapheAlloue();
    grapheChargeFichier(graphe,argv[1]);
    dijkstra(graphe, grapheChercheSommetParNom(graphe, argv[2]));
    display_dijkstra_results(graphe, grapheChercheSommetParNom(graphe, argv[2]));
    grapheLibere(graphe);

    exit(EXIT_SUCCESS);
  }
