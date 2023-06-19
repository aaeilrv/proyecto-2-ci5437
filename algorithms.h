/*
    Implementation of algorithms

        - negamax
        - negamax w alpha/betha pruning
        - scout
        - negascout
*/

#include "othello_cut.h"
#include "utils.h"
#include <algorithm>
#include <limits.h>
#include <queue>
using std::queue;

using namespace std;

int negamax(state_t state, int depth, int color, bool use_tt) {
    if (depth == 0 || state.terminal()) return color * state.value();

    queue<int> move = state.get_moves(color);     
    int child, alpha = INT_MIN;
    bool curr_player = color == 1;

    while (!move.empty()) {
        child = move.front();
        move.pop();
        alpha = max(alpha, -negamax(state.move(curr_player, child), depth-1, -color, use_tt));
    }
    return alpha;

}

int negamax_alpha_beta(state_t state, int depth, int alpha, int beta, int color, bool use_tt) {
    if (depth == 0 || state.terminal()) return color * state.value();

    queue<int> move = state.get_moves(color);
    int score = INT_MIN;
    bool curr_player = color == 1;

    while (!move.empty()) {
        int child = move.front();
        move.pop();
        int val = -negamax_alpha_beta(state.move(curr_player, child), depth-1, -alpha, -beta, -color, use_tt);
        score = max(score, val);
        alpha = max(alpha, val);

        if (alpha >= beta) break; 
    }

    return score;
}

/*int TEST(state_t state, int depth, int score, int color, bool cond) {
    if (depth == 0 || state.terminal()) {
        return (cond ? state.value() >= score : state.value() > score);
    }

    queue<int> move = state.get_moves(color);
    bool curr_player = color == 1;
    int n_moves = move.size();

    // for each child of node
    for (int i = 0; i < n_moves; i++) {
        int child = move.front();
        move.pop();

        // no va a funcionar porque child es int y no state_t
        // se podria usar chid = state.move(curr_player, i)
         
        

        if (i == 0) {
            if (curr_player && TEST(child, depth--, score, -color, cond)) {
                return true;
            }

            if (color == 0 && TEST(child, depth--, score, -color, cond)) {
                return false;
            }
        }
    }

    return !(curr_player);
}*/

/*int scout(state_t state, int depth, int color, bool use_tt) {
    if (depth == 0 || state.terminal()) return state.value();
    
    queue<int> moves = state.get_moves(color);
    bool curr_player = color == 1;
    int score, n_moves, child;

    score = 0;
    n_moves = moves.size();

    for (int i = 0; i < n_moves; i++) {
        child = moves.front();
        moves.pop();

        if (i == 0) {
            score = scout(state.move(curr_player, child), depth--, -color, use_tt);
        } 
        else {
            if (curr_player && !TEST) {
                score = scout(state.move(curr_player, child), depth--, -color, use_tt);
            } else  if (color == 0 && !TEST) {
                score = scout(state.move(curr_player, child), depth--, -color, use_tt);   
            }
        }
    }
    return score;
}*/

int negascout(state_t state, int depth, int alpha, int beta, int color, bool use_tt) {
    if (depth == 0 || state.terminal()) return color * state.value();

    queue<int> moves = state.get_moves(color);
    bool curr_player = color == 1;
    int score, n_moves, child;

    score = 0;
    n_moves = moves.size();

    for (int i =0; i<n_moves; i++) {
        child = moves.front();
        moves.pop();

        if (i == 0) {
            score = -negascout(state.move(curr_player, child), depth--, -beta, -alpha, -color, use_tt);
        } else {
            score = -negascout(state.move(curr_player, child), depth--, -(alpha-1), -alpha, -color, use_tt);
            
            if (alpha < score && score < beta) {
                score = -negascout(state.move(curr_player, child), depth--, -beta, -score, -color, use_tt);
            }
        }
        alpha = max(alpha, score);

        if (alpha >= beta) break;
    }

    return alpha;
}