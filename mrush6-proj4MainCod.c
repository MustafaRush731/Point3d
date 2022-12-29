/*
proj4MainCode.c Mustafa Rushdi 5/30/2022
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
int DebugMode;

typedef struct {
  int ID;
  double coordinates[3];
  double luminosity;
}
Vertex;

typedef struct {
  Vertex * vertex;
  int nAllocated;
  int nVertices;
  double luminosity;
  double lower[3], upper[3];
}
VertexSet;

typedef struct vertexNode {
  Vertex * vertex;
  struct vertexNode * next;
}
VertexNode;

typedef struct {
  int ID;
  VertexNode * vertices;
  int nVertices;
  double luminosity;
  double lower[3], upper[3];
}
Polygon;

Vertex * findVertexAddress(VertexSet set, int ID);
int polygonVertexIntersection(Polygon poly, Vertex vertex);
void addVertexToSet(VertexSet * set, Vertex vertices);
void BoundingBoxInfo(VertexSet * set);
void addVertexToPolygon(Polygon * polygon, Vertex * vertices);
void MaxValuePolygon(Polygon * polygon);
void MinValuePolygon(Polygon * polygon);
void DisplayMaxAndMin(Polygon * polygon, int i);
int intersection(VertexSet * set, Vertex * point);
int remove1stVertexFromPolygon(Polygon * polygon);
int isEmpty(Polygon polygon);

int main(int argc, char
  const * argv[]) {

  DebugMode = FALSE;
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-d") == 0) {
      DebugMode = TRUE;
    }
  }

  //required for linked list
  Polygon * polygons = NULL;
  int allocated = 5;
  // create a dynamically allocated array for polygons to hold an arburtary amount of them
  polygons = (Polygon * ) malloc(allocated * sizeof(Polygon));
  polygons -> vertices = NULL;
  polygons -> ID = 0;
  polygons -> nVertices = 0;
  // The reason i am implementing this is because i was running the same error on valgrind indicating it will loop three times and initialize values
  for (int i = 0; i < 3; i++) {
    polygons[i].nVertices = 0;
    polygons[i].vertices = NULL;
  }

  //used for Vertex set
  VertexSet * vertexSet = (VertexSet * ) malloc(sizeof(VertexSet));
  vertexSet -> nAllocated = 100;
  vertexSet -> nVertices = 0;
  vertexSet -> vertex = (Vertex * ) malloc(vertexSet -> nAllocated * sizeof(Vertex));
  Vertex point3d;

  int counter = 0;
  // while loop used to read in values till a negative luminosity is used
  while (1) {
    point3d.ID = counter;
    scanf("%lf", & point3d.coordinates[0]);
    scanf("%lf", & point3d.coordinates[1]);
    scanf("%lf", & point3d.coordinates[2]);
    scanf("%lf", & point3d.luminosity);
    //when a negative value is entered then we break from the while loop
    if (point3d.luminosity < 0) {
      break;
    //else stament runs only when there isnt a negative luminosity
    } else {
      addVertexToSet(vertexSet, point3d);
    }
    counter++;
  }

  BoundingBoxInfo(vertexSet);

  int userInput = 0;
  printf("Enter the Vertices at\n");
  scanf("%d", & userInput);
  //while loop used until a negative input is entered
  while (1) {
    if (userInput < 0) {
      // if a negative input is entered then the if staments checks weather the struct has no vertices if it is empty then break from the while loop
      if (polygons[polygons -> ID].nVertices == 0) {
        break;
      } else {
        // if thre isnt two consecutive negative intergers add polygons ID by 1
        polygons -> ID += 1;
        if(DebugMode == TRUE){
          printf("Did the polygon ID increment correctly %d\n", polygons->ID);
        }
      }
    } else {
      // if thre are more polygons / ID then the amount allocated then double the allocation
      if (allocated <= polygons -> ID) {
        Polygon * temp;
        temp = polygons;
        polygons = (Polygon * ) malloc(2 * allocated * sizeof(Polygon));
        for (int i = 0; i < allocated; i++) {
          polygons[i] = temp[i];
        }
        free(temp);
        temp = NULL;
        allocated = allocated * 2;
      } else {
        // if there is no Vertex ID found within the our vertex set then break from the while loop
        if (findVertexAddress( * vertexSet, userInput) == NULL) {
          break;
        } else {
          addVertexToPolygon(polygons, findVertexAddress( * vertexSet, userInput));
        }
      }
    }
    scanf("%d", & userInput);
  }

  float totalLuminosity = 0;
  double averageLuminosity = 0;
  //loop through the linked list
  for (int i = 0; i < polygons -> ID; i++) {
    VertexNode * pTemp = polygons[i].vertices;
    while (pTemp != NULL) {
      // for every node add the value of luminosity to a variable do this until the end of the linked list
      totalLuminosity += pTemp -> vertex -> luminosity;
      // the average is the total luminosity of the entire linked list then dividing it byt the length of the certain linked list
      averageLuminosity = totalLuminosity / polygons[i].nVertices;
      pTemp = pTemp -> next;
    }
    MinValuePolygon(polygons);
    MaxValuePolygon(polygons);
    printf("For polygon number %d\n", i);
    DisplayMaxAndMin(polygons, i);
    printf("the average for this polygon is %.2lf\n", averageLuminosity);
    totalLuminosity = 0;
  }
  int counter2 = 0;
  printf("Enter a second set of numbers\n");
  //while loop enter numbers until a negative luminosity is enetred the same way for the first part of the project 
  while (1) {
    point3d.ID = counter2;
    scanf("%lf", & point3d.coordinates[0]);
    scanf("%lf", & point3d.coordinates[1]);
    scanf("%lf", & point3d.coordinates[2]);
    scanf("%lf", & point3d.luminosity);
    if (point3d.luminosity < 0) {
      break;
    }
    // looped used to go through all linked lists
    for (int i = 0; i < polygons -> ID; i++) {
      // if the vertex intercsects with the the polygon
      if (polygonVertexIntersection(polygons[i], point3d) == 1) {
        printf("It Intersects with polygon index %d\n", i);
      } else {
        printf("It does not Intersect with polygon index %d\n", i);
      }
    }
    // then if staments used to indicate weather the vertex entered intersects with the vertex set
    if (intersection(vertexSet, & point3d) == 1) {
      printf("Point %.1lf %.1lf %.1lf intersects our set\n", point3d.coordinates[0], point3d.coordinates[1], point3d.coordinates[2]);
    } else {
      printf("Point %.1lf %.1lf %.1lf does not intersect our set\n", point3d.coordinates[0], point3d.coordinates[1], point3d.coordinates[2]);
    }
    counter++;
  }
  // loop through all linked lists and remove the head of the list until the list is NULL
  for (int i = 0; i < polygons -> ID; i++) {
    while ((remove1stVertexFromPolygon( & polygons[i])) != -1) {
      if (isEmpty(polygons[i]) == 1) {
        if(DebugMode == TRUE){
          printf("if you see this then the list was emptied\n");
        }
        break;
      }
    }
  }
  // free memory
  free(vertexSet -> vertex);
  free(vertexSet);
  free(polygons);
  return 0;
}

// required function

// function returns a pointer to
Vertex * findVertexAddress(VertexSet set, int ID) {
  //loop through the how many vertices within the set
  for (int i = 0; i < set.nVertices; i++) {
    //if the Id is evequall to a specific vertex ID then return the address of that vertex
    if (ID == set.vertex[i].ID) {
      return &set.vertex[i];
      break;
    }
  }
  //returns null if no address was found
  return NULL;
}

//required function
//using paramters poly which is the polygons but not modyfing it and vertex of the second set
int polygonVertexIntersection(Polygon poly, Vertex vertex) {
  //nested if statments used to indicated weather values X Y Z from user input intersects box coordinates for each polygon
  if (poly.lower[0] <= vertex.coordinates[0] && poly.upper[0] >= vertex.coordinates[0]) {
    if (poly.lower[1] <= vertex.coordinates[1] && poly.upper[1] >= vertex.coordinates[1]) {
      if (poly.lower[2] <= vertex.coordinates[2] && poly.upper[2] >= vertex.coordinates[2]) {
        //return 1
        return 1;
      }
    }
  } else {
    return 0;
  }
}

// add vertex to set parameters Vertex set is modyfied
void addVertexToSet(VertexSet * set, Vertex vertices) {
  // the ammount of vertices is greater then the amount allocated increase the allocated space by 2
  if (set -> nVertices >= set -> nAllocated) {
    Vertex * temp;
    temp = set -> vertex;
    set -> vertex = (Vertex * ) malloc(2 * set -> nAllocated * sizeof(Vertex));
    for (int i = 0; i < set -> nAllocated; i++) {
      set -> vertex[i] = temp[i];
    }
    free(temp);
    temp = NULL;
    set -> nAllocated = set -> nAllocated * 2;
  }
  //set the array of vertex at a sertain position as the vertices passed by
  set -> vertex[set -> nVertices] = vertices;
  //increase nVertices by 1
  set -> nVertices += 1;
  if(DebugMode == TRUE){
    printf("the vertices we are at within list certain linked list is %d\n", set->nVertices);
  }
}

//Void function to find the upper and lower coordinates and the average luminosity
// by first finding the total luministy of the set
//then dividing it by how many vertices there are in the set
//then using multiples if staments to find the min and max of the set as a whole
void BoundingBoxInfo(VertexSet * set) {
  double maxX = -500;
  double minX = 500;
  double maxY = -500;
  double minY = 500;
  double maxZ = -500;
  double minZ = 500;
  double TotalLuminosity = 0;
  double luminosity = 0;

  for (int i = 0; i < set -> nVertices; i++) {
    TotalLuminosity += set -> vertex[i].luminosity;
    luminosity = (TotalLuminosity / set -> nVertices);
    set -> luminosity = luminosity;
    /* using multiple if staments to find the min and max values which will be the bound */
    if (maxX < set -> vertex[i].coordinates[0]) {
      maxX = set -> vertex[i].coordinates[0];
      set -> upper[0] = maxX;
    }
    if (minX > set -> vertex[i].coordinates[0]) {
      minX = set -> vertex[i].coordinates[0];
      set -> lower[0] = minX;
    }
    if (maxY < set -> vertex[i].coordinates[1]) {
      maxY = set -> vertex[i].coordinates[1];
      set -> upper[1] = maxY;
    }
    if (minY > set -> vertex[i].coordinates[1]) {
      minY = set -> vertex[i].coordinates[1];
      set -> lower[1] = minY;
    }
    if (maxZ < set -> vertex[i].coordinates[2]) {
      maxZ = set -> vertex[i].coordinates[2];
      set -> upper[2] = maxZ;
    }
    if (minZ > set -> vertex[i].coordinates[2]) {
      minZ = set -> vertex[i].coordinates[2];
      set -> lower[2] = minZ;
    }
  }
  //print all info of the bounding box 
  if(DebugMode == TRUE){
    printf("the luminosity average is %.2f\n", luminosity);
    printf("max bound for coordinate X is %.2f the min bound for coordinate X is %.2f\n", set -> upper[0], set -> lower[0]);
    printf("max bound for coordinate Y is %.2f the min bound for coordinate Y is %.2f\n", set -> upper[1], set -> lower[1]);
    printf("max bound for coordinate Z is %.2f the min bound for coordinate Z is %.2f\n", set -> upper[2], set -> lower[2]);
  }
  printf("the luminosity average is %.2f\n", luminosity);
  printf("max bound for coordinate X is %.2f the min bound for coordinate X is %.2f\n", set -> upper[0], set -> lower[0]);
  printf("max bound for coordinate Y is %.2f the min bound for coordinate Y is %.2f\n", set -> upper[1], set -> lower[1]);
  printf("max bound for coordinate Z is %.2f the min bound for coordinate Z is %.2f\n", set -> upper[2], set -> lower[2]);
}

//adding the vertex to the polygon by first creating an instance of Vertex Node
//if the list is empty then add that new Vertex Node to the Head of the list making the previous head pointer point to the newly allocated VertexNode
//if the list isnt empty i create a VertexNode name temp use that vertex Node point that temp node to the previous node created and have the head of the list point to the newly alloacted new Node
void addVertexToPolygon(Polygon * polygon, Vertex * newVertex) {
  VertexNode * newNode = (VertexNode * ) malloc(sizeof(VertexNode));
  VertexNode * tempNode = NULL;
  polygon[polygon -> ID].nVertices += 1;
  if(DebugMode == TRUE){
    printf("the vertices we are curaintly at within this linked list is %d",polygon[polygon -> ID].nVertices );
  }
  if (polygon[polygon -> ID].vertices == NULL) {
    newNode -> vertex = newVertex;
    newNode -> next = NULL;
    polygon[polygon -> ID].vertices = newNode;
  } else if (polygon[polygon -> ID].vertices != NULL) {
    tempNode = polygon[polygon -> ID].vertices;
    newNode -> vertex = newVertex;
    //having the new node point to the previous allocated Node
    newNode -> next = polygon[polygon -> ID].vertices;
    polygon[polygon -> ID].vertices = newNode;
  }
};

//similarly to how we found the max values of each polygon
void MaxValuePolygon(Polygon * polygon) {
  // loop through how many polygons there are
  for (int i = 0; i < polygon -> ID; i++) {
    VertexNode * pTemp = polygon[i].vertices;
    double maxX = -500;
    double maxY = -500;
    double maxZ = -500;
    // loop until Linked list is NULL
    // using if statments to see weather the values is greater then a certain coorrdinate
    while (pTemp != NULL) {
      if (maxX < pTemp -> vertex -> coordinates[0]) {
        maxX = pTemp -> vertex -> coordinates[0];
        polygon[i].upper[0] = maxX;
      }
      if (maxY < pTemp -> vertex -> coordinates[1]) {
        maxY = pTemp -> vertex -> coordinates[1];
        polygon[i].upper[1] = maxY;
      }
      if (maxZ < pTemp -> vertex -> coordinates[2]) {
        maxZ = pTemp -> vertex -> coordinates[2];
        polygon[i].upper[2] = maxZ;
      }
      // going to the next node
      pTemp = pTemp -> next;
    }
  }
}

//similarly to how we found the min values of each polygon
void MinValuePolygon(Polygon * polygon) {
  for (int i = 0; i < polygon -> ID; i++) {
    VertexNode * pTemp = polygon[i].vertices;
    double minX = 500;
    double minY = 500;
    double minZ = 500;
    // loop until Linked list is NULL
    // using if statments to see weather the values is less then a certain coorrdinate
    while (pTemp != NULL) {
      if (minX > pTemp -> vertex -> coordinates[0]) {
        minX = pTemp -> vertex -> coordinates[0];
        polygon[i].lower[0] = minX;
      }
      if (minY > pTemp -> vertex -> coordinates[1]) {
        minY = pTemp -> vertex -> coordinates[1];
        polygon[i].lower[1] = minY;
      }
      if (minZ > pTemp -> vertex -> coordinates[2]) {
        minZ = pTemp -> vertex -> coordinates[2];
        polygon[i].lower[2] = minZ;
      }
      // going to the next node
      pTemp = pTemp -> next;
    }
  }
}

// displaying all Max and Min values in polygon
void DisplayMaxAndMin(Polygon * polygon, int i) {
  printf("max and min are %.0f %.0f\n", polygon[i].upper[0], polygon[i].lower[0]);
  printf("max and min are %.0f %.0f\n", polygon[i].upper[1], polygon[i].lower[1]);
  printf("max and min are %.0f %.0f\n", polygon[i].upper[2], polygon[i].lower[2]);
}

// check weather the vertex entered in the second set intersects the vertex Set
int intersection(VertexSet * set, Vertex * point) {
  //nested if staments used to see weather all corrdinated fit within themin and max of the Vertex Set if so return 1 else if atleast one doesnt meet the requirments then return 0
  if (point -> coordinates[0] >= set -> lower[0] && set -> upper[0] >= point -> coordinates[0]) {
    if (point -> coordinates[1] >= set -> lower[1] && set -> upper[1] >= point -> coordinates[1]) {
      if (point -> coordinates[2] >= set -> lower[2] && set -> upper[2] >= point -> coordinates[2]) {
        return 1;
      }
    }
  }
  return 0;
}
//checks weather list is empty in terms how many vertices are within the linked list
int isEmpty(Polygon polygon) {
  if (polygon.vertices == NULL) {
    polygon.nVertices = 1;
  } else if (polygon.vertices != NULL) {
    polygon.nVertices = 0;
  }
  return polygon.nVertices;
};
// removes the head of the list
int remove1stVertexFromPolygon(Polygon * polygon) {
  VertexNode * pTemp = NULL;
  int ID;
  //checks weather the list is empty if so returns -1;
  if (isEmpty( * polygon) == 1) {
    return -1;

  } else {
    //ptemp points to the head
    pTemp = polygon -> vertices;
    // set the head to the next node
    polygon -> vertices = pTemp -> next;
    //the pTemp pointer does not link to anything
    pTemp -> next = NULL;
    //free the head of the list
    free(pTemp);
    //return 1 when done right
    return 1;
  }
};
