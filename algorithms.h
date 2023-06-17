/*
    Implementation of algorithms

        - negamax
        - negamax w alpha/betha pruning
        - scout
        - negascout
*/

#include "othello_cut.h"
#include <queue>

int negamax(state_t state, int depth, int color) {
    if (depth == 0 || state.terminal()) return color * state.value();

    queue<int> move = state.get_moves(color);     
    int child, alpha = INT_MIN;
    bool curr_player = color == 1;

    while (!moves.empty()) {
        child = moves.front();
        moves.pop();
        alpha = max(aplha, -negamax(state.move(curr_player, child), depth-1, -color));
    }
    return alpha;

}

int negamax_alpha_beta(state_t state, int depth, int alpha, int beta, int color) {
    if (depth == 0 || state.terminal()) return color * state.value();

    queue<int> move = state.get_moves(color);
    int score = INT_MIN_VALUE;
    bool curr_player = color == 1;

    for (!moves.empty()) {
        child = moves.front();
        moves.pop();
        int val = -negamax_alpha_beta(state.move(curr_player, child), depth-1, -alhpa, -beta, -color);
        score = max(score, val);
        alpha = max(alpha, val);

        if (alpha >= beta) break; 
    }

    return score;
}
int scout(state_t state, int depth, int color) {
    if (depth == 0 || state.terminal()) return state.value();
    
    queue<int> moves = state.get_moves(color);
    bool curr_player = color == 1;
    int score, n_moves, child;

    score = 0;
    n_moves = moves.size()

    for (int i = 0; i < n_moves; i++) {
        child = moves.front();
        moves.pop();

        if (i == 0) {
            score = scout(state.move(curr_player, child), depth--1, -color);
        } 
        else {
            if (curr_player && !TEST) {
                score = scout(state.move(curr_player, child), depth-1, -color);
            } else  if (color == 0 && !TEST) {
                score = scout(state.move(curr_player, child), depth-1, -color);   
            }
        }
    }
    return score;
}

int negascout(state_t state, int depth, int alpha, int beta, int color) {
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
            score = -negascout(state.move(curr_player, child), depth-1, -beta, -aplha, -color);
        } else {
            score = -negascout(state.move(curr_player, child), depth-1, -aplha-a, -alpha, -color)
            
            if (alpha < score && score < beta) {
                score = -negascout(state.move(curr_player, child), depth-1, -beta, -score, -color)
            }
        }
        alpha = max(alpha, score)

        if (alpha >= beta) break;
    }

    return alpha;
}