#include "graphe.h"
#include <stdio.h>
#include <stdlib.h>
#include "my_library.h"


/*********************************MACROS***************************************/
#define ERROR -1
#define EMPTY -1
#define CFALSE 0
#define CTRUE 1


/******************************TYPEDEFS****************************************/
typedef enum {RED, BLUE, GREEN} tCouleur;
typedef tCouleur tTabCouleurs[MAX_SOMMETS];
tTabCouleurs coloursArray;
tNumeroSommet pred[MAX_SOMMETS];
tNumeroSommet best_path[MAX_SOMMETS];



/***********************TOOL FUNCTIONS AND PROCEDURES**************************/

/*Initialises values for predecessor's array*/
void init_pred(){
  int i;
  for(i=0;i<MAX_SOMMETS;i++){
    pred[i] = EMPTY;
  }
}


/*Initialises values for best_path array*/
void init_path_array(){
  int i;
  for(i=0;i<MAX_SOMMETS;i++){
    best_path[i] = EMPTY;
  }
}


/*Colours all nodes in blue */
void colour_all_in_blue(tGraphe g){
  int i, nb_nodes;
  nb_nodes = grapheNbSommets(g);
  for(i=0 ; i < nb_nodes ; i++) {
    coloursArray[i] = BLUE;
  }
}


int found_node(tNumeroSommet target, tNumeroSommet current){
  return target == current? CTRUE : CFALSE;
}


/*Returns the index of a blue neighbour if any, -1 otherwise*/
tNumeroSommet has_blue_neighbour(tGraphe graph, tNumeroSommet node){
  int i, nb_neighbours;
  tNumeroSommet neigh_index;
  nb_neighbours = grapheNbVoisinsSommet(graph, node);

  for(i=0 ; i < nb_neighbours ; i++){ /*For each neighbour*/
    /*Obtaining the neighbours' node index in the graph*/
    neigh_index = grapheVoisinSommetNumero(graph, node, i);
    if(coloursArray[neigh_index] == BLUE){ /*Returns the first blue encountered*/
      return neigh_index;
    }
  }
  return ERROR;
}


void calculate_path(tGraphe g, tNomSommet start_node, tNomSommet end_node){
  int i;
  tNumeroSommet curr_node_index, start_node_index, end_node_index;
  i = MAX_SOMMETS;

  init_path_array();

  start_node_index = grapheChercheSommetParNom(g, start_node);
  end_node_index = grapheChercheSommetParNom(g, end_node);
  curr_node_index = end_node_index; /*Start from the end*/
  while(start_node_index != curr_node_index){ /*While we haven't got back to the beginning*/
  best_path[i] = curr_node_index; /*Storing the nodes in the end to access the beginning later*/
  i--;
  /*For next iteration, the node should be the predecessor of the current one*/
  curr_node_index = pred[curr_node_index];
}
}


/*Prints to standard output the path from beginning node to end node*/
void display_path(tGraphe g, tNomSommet beginning, tNomSommet end){
  int i, nb_nodes;
  tNumeroSommet curr_node_index;
  tNomSommet curr_node_name;

  /*First node*/
  printf("\n%s\n", beginning);

  /*All nodes in the path*/
  for (i = 0; i < MAX_SOMMETS; i++) {
    if(best_path[i] != EMPTY){ /*If it's part of the best path and not just empty*/
    curr_node_index = best_path[i];
    grapheRecupNomSommet(g, best_path[i], curr_node_name);
    printf("%s\n", curr_node_name);
  }
}
/*Last node*/
printf("%s\n\n", end);
}



/*******************************CORE FUNCTION**********************************/

/*Checks whether there is a path between two selected nodes,
returns 0 if so, -1 otherwise*/
int path_test(tGraphe graphe, tNomSommet startNode, tNomSommet endNode){
  int found;
  tPileSommets stack;
  tNumeroSommet curr_node, end_index, start_index;
  tNumeroSommet neigh_index;
  tNomSommet node_name;

  /*Initialising*/
  init_pred(); /*Initialises predecessor's array*/
  colour_all_in_blue(graphe);
  stack = pileSommetsAlloue();
  found = ERROR; /*By default, we consider there is no path between both*/

  /*Getting index of start node*/
  start_index = grapheChercheSommetParNom(graphe, startNode);

  /*Getting index of end node*/
  end_index = grapheChercheSommetParNom(graphe, endNode);

  /*Colouring start node in green...*/
  coloursArray[grapheChercheSommetParNom(graphe, startNode)] = GREEN;
  /*...and stacking it up right away */
  pileSommetsEmpile(stack, start_index);

  while( !pileSommetsEstVide(stack) ){
    /*Peaks at top of stack withouth removing item, since we might need to look at it several times*/
    curr_node = pileSommetsTete(stack);
    /*Checking whether we have found the correct node, i.e. current matches end*/
    found = found_node(end_index, curr_node);
    if(found){return CTRUE;} /*No need to keep on looking*/

    /*Obtaining index of first blue neighbour encountered*/
    neigh_index = has_blue_neighbour(graphe, curr_node);
    if(neigh_index != ERROR){ /*And if there is at least one...*/

      /*This is done in all cases*/
      pred[neigh_index] = curr_node; /*Setting the predecessor*/

      /*Checking whether we have found the correct node*/
      found = found_node(end_index, neigh_index);
      if(found){return CTRUE;} /*No need to keep on looking*/

      coloursArray[neigh_index] = GREEN; /*Colour it in green*/
      pileSommetsEmpile(stack, neigh_index); /*stack it up*/
    }

    else { /*Case where we are stuck and need to fall back*/
      /*We shall never tread this node again since we know it leads to a dead end*/
      coloursArray[curr_node] = RED;
      pileSommetsDepile(stack); /*Handling of this node is over*/
      grapheRecupNomSommet(graphe, curr_node, node_name);
    }
  }
  pileSommetsLibere(stack); /*Freeing memory*/
  return found;
}




int main(int argc, char ** argv){
  tGraphe graphe;
  int res = ERROR;

  if (argc<4){
    printf("%s\n","Use as :\t./existeChaineEntre2Sommets [grp file name] [start node name] [end node name]");
    exit(EXIT_FAILURE);
  }

  printf("\nIs there a path from %s to %s?\n", argv[2], argv[3]);

  graphe = grapheAlloue();
  grapheChargeFichier(graphe,argv[1]);
  res = path_test(graphe, argv[2], argv[3]);
  calculate_path(graphe, argv[2], argv[3]);
  display_path(graphe, argv[2], argv[3]);
  grapheLibere(graphe);

  if(res != CTRUE){   printf("NO, there is no path\n" );  }

  exit(EXIT_SUCCESS);
}
