/*********************************************************
 *  Nine-Board Tic-Tac-Toe
 *  COMP3411/9414/9814 Artificial Intelligence
 *  Alan Blair, CSE, UNSW
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "common.h"
#include "agent.h"
#include "game.h"

#define MAX_MOVE 81
#define MAX 2147483647
#define MIN -2147483648

int board[10][10] = {{2}};
int move[MAX_MOVE+1];
int player;
int m;
int minimax(int depth, int alpha, int beta, int prev_move, int p1, int dummy[10][10]);
int bestMove(int prev_move);
int noMoves;
int height = 0;
int moveOrder[10][10] = { {0}, {0,5,3,1,7,9,6,8,4,2}, {0,5,1,9,7,3,6,8,4,2},
                        {0,5,1,7,9,8,6,4,2,3}, {0,5,1,9,3,7,8,2,6,4},
                        {0,5,1,3,7,9,6,8,2,4}, {0,5,1,7,9,3,8,2,4,6},
                        {0,1,5,3,9,6,8,2,4,7}, {0,5,1,3,9,7,6,4,2,8},
                        {0,5,1,3,7,8,2,6,4,9}};

int main(){
    for (int x = 0; x < 10; x++){
        for (int y = 0; y < 10; y++){
            board[x][y] = 2;
        }
    }
    for (int i = 1; i < 10; i++){
        printf("\nSubboard %d----------------------------\n\n", i);
        bestMove(i);
        printf("\n\nDEPTH: %d", height);
        height = 0;
    }
    return 0;
}

//Evaluating the each case within the subboard
int heuristic_evaluation(int first, int second, int third){
    int player1 = 0;
    int player2 = 0;

    //First Square
    if(first == 0){
        player1++;
    } else if(first == 1){
        player2++;
    }

    //Second Square
    if(second == 0){
        player1++;
    } else if(second == 1){
        player2++;
    }

    //Third Square
    if(third == 0){
        player1++;
    } else if(third == 1){
        player2++;
    }

    //When opponent wins
    if(player1 == 0 && player2 == 3){
        return -1000000;
    //When oppenent only need 1 tile left to win
    } else if(player1 == 0 && player2 == 2){
        return -10000;
    //When oppenent has possession of one of the tiles while the other 2 is empty
    } else if(player1 == 0 && player2 == 1){
        return -100;
    //When player has possession of one of the tiles while the other 2 is empty
    } else if(player1 == 1 && player2 == 0){
        return  100;
    //When player only need 1 tile left to win
    } else if(player1 == 2 && player2 == 0){
        return 10000;
    //When player wins
    } else if(player1 == 3 && player2 == 0){
        return 1000000;

    } else {
        return 0;
    }
}

//Find the value of the subboard
int heuristic(int singleboard[10]){
    int total = 0;


    total = total + heuristic_evaluation(singleboard[1],singleboard[2],singleboard[3]);
    total = total + heuristic_evaluation(singleboard[4],singleboard[5],singleboard[6]);
    total = total + heuristic_evaluation(singleboard[7],singleboard[8],singleboard[9]);


    total = total + heuristic_evaluation(singleboard[1],singleboard[4],singleboard[7]);
    total = total + heuristic_evaluation(singleboard[2],singleboard[5],singleboard[8]);
    total = total + heuristic_evaluation(singleboard[3],singleboard[6],singleboard[9]);

    total = total + heuristic_evaluation(singleboard[1],singleboard[5],singleboard[9]);
    total = total + heuristic_evaluation(singleboard[3],singleboard[5],singleboard[7]);

    return total;
}

void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// A function to implement bubble sort
void bubbleSort(int arr[],int positions[], int n)
{
   int i, j;
   for (i = 1; i < n-1; i++){
       // Last i elements are already in place
       for (j = 1; j < n-i-1; j++){
           if (arr[j] > arr[j+1]){
              swap(&arr[j], &arr[j+1]);
              swap(&positions[j], &positions[j+1]);
          }
      }
  }
}
//Iterates through all Empty tiles, calling the minimax function to evaluate
//its value
int bestMove(int prev_move){
    int i = 0;
    int position = 0;
    int max = 0;
    int moveData[10];
    int positions[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (int x = 0; x < 10; x++){
        moveData[x] = 0;
    }

    for (i = 1; i < 10; i++){
        if (board[prev_move][moveOrder[prev_move][i]] != EMPTY){
            continue;
        }
        int dummy[10][10];
        int minimaxResult ;
        //copying board into dummy
        for(int a = 1 ; a < 10 ; a++) {
            for(int b = 1 ; b < 10 ; b++) {
                dummy[a][b] = board[a][b];
            }
        }
        //making the move into the board
        dummy[prev_move][moveOrder[prev_move][i]] = player;
        //Calling the minimax function to get an evaluation on the tile
        noMoves = 0;
        minimaxResult = minimax(0, MIN, MAX, moveOrder[prev_move][i], !player, dummy);
        moveData[i] = noMoves;
        //printf("Position %d: %d moves\n", positions[i], moveData[i]);
        //printf("Position %d: %d moves\n", i, noMoves);
        noMoves = 0;
    }

    bubbleSort(moveData, positions, 11);
    for (i = 1; i < 10; i++){
        printf("Position %d: %d moves\n", positions[i], moveData[i]);
    }
    return position;
}
//changes
//Basically A minimax algorithm with alpha beta pruning, which returns the value
//of the position of the board
int minimax(int depth, int alpha, int beta, int prev_move, int p1, int dummy[10][10]) {
    noMoves++;
    if (depth > height){
        height = depth;
    }
    //If it is a draw return 0
    if (full_board(dummy[prev_move])){
        return 0;
    }

    int totalvalue = 0;
    //Getting an evaluation of the whole board
    for(int i = 1; i < 10; i++) {
        totalvalue = totalvalue + heuristic(dummy[moveOrder[prev_move][i]]);
    }

    //Returning valuation if the depth is 6 (Most reasonable time), or if there was a win
    //or a loss. (Made it +-800000 to give it some leeway, just in case there were a 2 adjacent evluation)
    if (depth == 9 || totalvalue >= 800000  || totalvalue <= -800000 ) {
        return totalvalue;
    }

    //If it is the player, goal is to maximise
    if (p1 == player) {
        for ( int i = 1; i < 10; i++) {
            //Skipping used tiles to improve run time
            if (dummy[prev_move][moveOrder[prev_move][i]] != EMPTY){
                continue;
            }
            int dummydummy[10][10];
            //Copying boards
            for (int a = 1; a < 10; a++){
                for (int b = 1; b < 10; b++){
                    dummydummy[a][b] = dummy[a][b];
                }
            }
            dummydummy[prev_move][moveOrder[prev_move][i]] = player;
            //Recursing the minimax to run through future turns
            int currentScore = minimax(depth + 1, alpha, beta, moveOrder[prev_move][i], !player, dummydummy);
            if (currentScore > alpha){
                alpha = currentScore;
            }

            //Alpha Beta Pruning
            if(alpha >= beta){
                break;
            }
        }
        return alpha;
    //This is if it is the opponent. Goal is to minimise
    } else {

        for ( int i = 1; i < 10; i++) {

            if (dummy[prev_move][moveOrder[prev_move][i]] != EMPTY){
                continue;
            }

            int dummydummy[10][10];
            for (int a = 1; a < 10; a++){
                for (int b = 1; b < 10; b++){
                    dummydummy[a][b] = dummy[a][b];
                }
            }
            dummydummy[prev_move][moveOrder[prev_move][i]] = !player;

            int currentScore = minimax(depth + 1, alpha, beta, moveOrder[prev_move][i], player, dummydummy);


            if (currentScore < beta){
                beta = currentScore;
            }


            if(alpha >= beta){
                break;
            }
        }
        return beta;
    }

}

