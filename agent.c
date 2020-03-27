/*******************************************************************************
 *  Nine-Board Tic-Tac-Toe
 *  COMP3411/9414/9814 Artificial Intelligence
 *  Alan Blair, CSE, UNSW
    This is based on Alan Blair's randt.c code.
    Code By Aven Au (z5208734)
 *******************************************************************************/


/*******************************************************************************
            QUESTION

---------------Algorithm and Data Structures-----------------------------------

So this program basically use a basic minimax algorithm with alphabeta pruning.
The bestMove() function is ran everytime a move in need is called. It basically
iterates through every move within the subboard, calling the minimax() function
to find the most optimal move. The minimax function is a recursive function
which simulates the game, returning a heuristic evaluation after a certain
value threshold or depth. The heuristic function basically adds up all the
valutations in heuristic_evaluation() function which checks its adjacencies
and points are reward based on the positioning.

Also the program includes an array of most optimal moves in each subboard.
The moveOrder[][] array contains a list of positions that requires the most moves
for each subboard in decreasing order. This greatly optimises the program
and also complements with the alpha beta pruning very well, which improved the
time complexity by 50% (0.8 seconds at depth 7).

---------------------Design Decisions-------------------------------------------

Intially when starting the assignment I did not use alpha beta pruning which
affected the time complexity by a whole lot. Before alpha beta pruning it took
the program 1 minute and 16 seconds per turn at depth 7. After implementing alpha beta
pruning the program was significantly optimised with it only taking 1.714 seconds.

I also considered using symmetrics as it would also signficantly cut the amount
of moves needed. However, I later realised that it would not work due to it
being a 9 board tictactoe, so it does not have symmetrics as it would affect
the different subboards differently.

I also decided to iterate through positions that require more moves first as
it significantly complements with alpha beta pruning. Since most of the numbers
are already iterated through in the first move iteration, the alpha beta Pruning
is activated a lot earlier for every position afterwards.
********************************************************************************/

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

int board[10][10];
int move[MAX_MOVE+1];
int player;
int m;
int minimax(int depth, int alpha, int beta, int prev_move, int p1, int dummy[10][10]);
int bestMove(int prev_move);
//This is the optimal move order for each subboard
int moveOrder[10][10] = { {0}, {0,5,3,1,7,9,6,8,4,2}, {0,5,1,9,7,3,6,8,4,2},
                        {0,5,1,7,9,8,6,4,2,3}, {0,5,1,9,3,7,8,2,6,4},
                        {0,5,1,3,7,9,6,8,2,4}, {0,5,1,7,9,3,8,2,4,6},
                        {0,1,5,3,9,6,8,2,4,7}, {0,5,1,3,9,7,6,4,2,8},
                        {0,5,1,3,7,8,2,6,4,9}};

/*********************************************************//*
   Print usage information and exit
*/
void usage( char argv0[] )
{
  printf("Usage: %s\n",argv0);
  printf("       [-p port]\n"); // tcp port
  printf("       [-h host]\n"); // tcp host
  exit(1);
}

/*********************************************************//*
   Parse command-line arguments
*/

void agent_parse_args( int argc, char *argv[] )
{
  int i=1;
  while( i < argc ) {
    if( strcmp( argv[i], "-p" ) == 0 ) {
      if( i+1 >= argc ) {
        usage( argv[0] );
      }
      port = atoi(argv[i+1]);
      i += 2;
    }
    else if( strcmp( argv[i], "-h" ) == 0 ) {
      if( i+1 >= argc ) {
        usage( argv[0] );
      }
      host = argv[i+1];
      i += 2;
    }
    else {
      usage( argv[0] );
    }
  }
}

/*********************************************************//*
   Called at the beginning of a series of games
*/
void agent_init()
{
  struct timeval tp;

  // generate a new random seed each time
  gettimeofday( &tp, NULL );
  srandom(( unsigned int )( tp.tv_usec ));
}

/*********************************************************//*
   Called at the beginning of each game
*/
void agent_start( int this_player )
{
  reset_board( board );
  m = 0;
  move[m] = 0;
  player = this_player;
}

/*********************************************************//*
   Choose second move and return it
*/
int agent_second_move( int board_num, int prev_move )
{
  int this_move;
  move[0] = board_num;
  move[1] = prev_move;
  board[board_num][prev_move] = !player;
  m = 2;

//-----------------------------------------------------------

 this_move = bestMove(prev_move);

//-----------------------------------------------------------

  move[m] = this_move;
  board[prev_move][this_move] = player;
  return( this_move );
}

/*********************************************************//*
   Choose third move and return it
*/
int agent_third_move(
                     int board_num,
                     int first_move,
                     int prev_move
                    )
{
  int this_move;
  move[0] = board_num;
  move[1] = first_move;
  move[2] = prev_move;
  board[board_num][first_move] =  player;
  board[first_move][prev_move] = !player;
  m=3;

  //-------------------------------------------------------------
 this_move = bestMove(prev_move);

//---------------------------------------------------------------

  move[m] = this_move;
  board[move[m-1]][this_move] = player;
  return( this_move );
}

/*********************************************************//*
   Choose next move and return it
*/
int agent_next_move( int prev_move )
{
  int this_move;
  m++;
  move[m] = prev_move;
  board[move[m-1]][move[m]] = !player;
  m++;

//---------------------------------------------------------

 this_move = bestMove(prev_move);

//---------------------------------------------------------

  move[m] = this_move;
  board[move[m-1]][this_move] = player;
  return( this_move );
}

/*********************************************************//*
   Receive last move and mark it on the board
*/
void agent_last_move( int prev_move )
{
  m++;
  move[m] = prev_move;
  board[move[m-1]][move[m]] = !player;
}

/*********************************************************//*
   Called after each game
*/
void agent_gameover(
                    int result,// WIN, LOSS or DRAW
                    int cause  // TRIPLE, ILLEGAL_MOVE, TIMEOUT or FULL_BOARD
                   )
{
   /* if (result == 2){
        printf("Agent just won!\n");
    } else if (result == 3){
        printf("Agent just lost!\n");
    } else if (result == 4){
        printf("Agent just drew!\n");
    } else {
        printf("Something else happened\n");
    }*/
}

/*********************************************************//*
   Called after the series of games
*/
void agent_cleanup()
{
  // nothing to do here
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
//Iterates through all Empty tiles, calling the minimax function to evaluate
//its value
int bestMove(int prev_move){
    int i = 0;
    int position = 0;
    int max = 0;

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
        minimaxResult = minimax(0, MIN, MAX, moveOrder[prev_move][i], !player, dummy);

        //Maintain the highest valued move
        if(position != 0) {
            if(minimaxResult > max) {
                position = moveOrder[prev_move][i];
                max = minimaxResult;
            }
        } else {
            position = moveOrder[prev_move][i];
            max = minimaxResult;
        }
    }
    return position;
}

//Basically A minimax algorithm with alpha beta pruning, which returns the value
//of the position of the board
int minimax(int depth, int alpha, int beta, int prev_move, int p1, int dummy[10][10]) {

    //If it is a draw return 0
    if (full_board(dummy[prev_move])){
        return 0;
    }

    int totalvalue = 0;
    //Getting an evaluation of the whole board
    for(int i = 1; i < 10; i++) {
        totalvalue = totalvalue + heuristic(dummy[i]);
    }

    //Returning valuation if the depth is 9 (Most reasonable timewise), or if there was a win
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

