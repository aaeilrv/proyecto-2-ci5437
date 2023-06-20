// Game of Othello -- Example of main
// Universidad Simon Bolivar, 2012.
// Author: Blai Bonet
// Last Revision: 1/11/16
// Modified by: 

#include <iostream>
#include <limits>
#include <unordered_map>
#include <algorithm>
#include <limits.h>
#include <queue>

#include "othello_cut.h"
#include "utils.h"

using std::queue;
using namespace std;

unsigned expanded = 0;
unsigned generated = 0;
int tt_threshold = 32; // threshold to save entries in TT

int negamax(state_t state, int depth, int color);
int negamax_alpha_beta(state_t state, int depth, int alpha, int beta, int color);
int scout(state_t state, int depth, int color);
int negascout(state_t state, int depth, int alpha, int beta, int color);

// Transposition table (it is not necessary to implement TT)
struct stored_info_t {
    int value_;
    int type_;
    enum { EXACT, LOWER, UPPER };
    stored_info_t(int value = -100, int type = LOWER) : value_(value), type_(type) { }
};

struct hash_function_t {
    size_t operator()(const state_t &state) const {
        return state.hash();
    }
};

class hash_table_t : public unordered_map<state_t, stored_info_t, hash_function_t> {
};

hash_table_t TTable[2];

int main(int argc, const char **argv) {
    state_t pv[128];
    int npv = 0;
    for( int i = 0; PV[i] != -1; ++i ) ++npv;

    int algorithm = 0;
    if( argc > 1 ) algorithm = atoi(argv[1]);
    bool use_tt = argc > 2;

    // Extract principal variation of the game
    state_t state;
    cout << "Extracting principal variation (PV) with " << npv << " plays ... " << flush;
    for( int i = 0; PV[i] != -1; ++i ) {
        bool player = i % 2 == 0; // black moves first!
        int pos = PV[i];
        pv[npv - i] = state;
        state = state.move(player, pos);
    }
    pv[0] = state;
    cout << "done!" << endl;

#if 0
    // print principal variation
    for( int i = 0; i <= npv; ++i )
        cout << pv[npv - i];
#endif

    // Print name of algorithm
    cout << "Algorithm: ";
    if( algorithm == 1 )
        cout << "Negamax (minmax version)";
    else if( algorithm == 2 )
        cout << "Negamax (alpha-beta version)";
    else if( algorithm == 3 )
        cout << "Scout";
    else if( algorithm == 4 )
        cout << "Negascout";
    cout << (use_tt ? " w/ transposition table" : "") << endl;

    // Run algorithm along PV (backwards)
    cout << "Moving along PV:" << endl;
    for( int i = 0; i <= npv; ++i ) {
        //cout << pv[i];
        int value = 0;
        TTable[0].clear();
        TTable[1].clear();
        float start_time = Utils::read_time_in_seconds();
        expanded = 0;
        generated = 0;
        int color = i % 2 == 1 ? 1 : -1;

        try {
            if( algorithm == 1 ) {
                value = negamax(pv[i], i, color);
            } else if( algorithm == 2 ) {
                value = negamax_alpha_beta(pv[i], i, -200, 200, color);
            } else if( algorithm == 3 ) {
                //value = scout(pv[i], i, color);
            } else if( algorithm == 4 ) {
                value = negascout(pv[i], i, -200, 200, color);
            }
        } catch( const bad_alloc &e ) {
            cout << "size TT[0]: size=" << TTable[0].size() << ", #buckets=" << TTable[0].bucket_count() << endl;
            cout << "size TT[1]: size=" << TTable[1].size() << ", #buckets=" << TTable[1].bucket_count() << endl;
            use_tt = false;
        }

        float elapsed_time = Utils::read_time_in_seconds() - start_time;

        cout << npv + 1 - i << ". " << (color == 1 ? "Black" : "White") << " moves: "
             << "value=" << color * value
             << ", #expanded=" << expanded
             << ", #generated=" << generated
             << ", seconds=" << elapsed_time
             << ", #generated/second=" << generated/elapsed_time
             << endl;
    }

    return 0;
}

int negamax(state_t state, int depth, int color) {
    if (!depth || state.terminal()) {
        expanded++;
        return color * state.value();
    }

    queue<int> move = state.get_moves(color);     
    int child, alpha = INT_MIN;
    bool curr_player = color == 1;

    if (!move.size()) {
        generated++;
        alpha = -negamax(state, depth, -color);
    }

    while (!move.empty()) {
        generated++;

        child = move.front();
        move.pop();

        int value = -negamax(state.move(curr_player, child), depth-1, -color);
        alpha = max(alpha, value);
    }
    return alpha;
}

int negamax_alpha_beta(state_t state, int depth, int alpha, int beta, int color) {
    if (!depth || state.terminal()) {
        expanded++;
        return color * state.value();
    }

    queue<int> move = state.get_moves(color);
    int score = INT_MIN;
    bool curr_player = color == 1;

    if (!move.size()) {
        generated++;
        score = -negamax_alpha_beta(state, depth, -beta, -alpha, -color);
    }

    while (!move.empty()) {
        generated++;

        int child = move.front();
        move.pop();

        int value = -negamax_alpha_beta(state.move(curr_player, child), depth-1, -beta, -alpha, -color);
        score = max(score, value);
        alpha = max(alpha, value);

        if (alpha >= beta) break; 
    }
    return score;
}

int test(state_t state, int depth, int score, int color, bool cond) {
    if (depth == 0 || state.terminal()) {
        return (cond ? state.value() >= score : state.value() > score);
    }

    queue<int> move = state.get_moves(color);
    bool curr_player = color == 1;
    int n_moves = move.size();

    if (move.size() == 0)
        return test(state, depth, score, -color, cond);

    for (int i = 0; i < n_moves; i++) {
        int child = move.front();
        move.pop();

        if (i == 0) {
            if (curr_player && test(state.move(curr_player, child), depth - 1, score, -color, cond)) {
                return true;
            }

            if (!curr_player && !test(state.move(curr_player, child), depth - 1, score, -color, cond)) {
                return false;
            }
        }
    }
    return !(curr_player);
}

int scout(state_t state, int depth, int color) {
    if (depth == 0 || state.terminal()) {
        expanded++;
        return state.value();
    }
    
    queue<int> moves = state.get_moves(color);
    bool curr_player = color == 1;
    int score, n_moves, child;

    score = 0;
    n_moves = moves.size();

    if (moves.size() == 0) {
        generated++;
        return scout(state, depth, -color);
    }

    for (int i = 0; i < n_moves; i++) {
        child = moves.front();
        moves.pop();

        if (i == 0) {
            score = scout(state.move(curr_player, child), depth - 1, -color);
        } 
        else {
            if (curr_player && test(state.move(curr_player, child), depth - 1, -color, score, 0)) {
                score = scout(state.move(curr_player, child), depth - 1, -color);
            } else  if (color == 0 && !test(state.move(curr_player, child), depth - 1, -color, score, 1)) {
                score = scout(state.move(curr_player, child), depth - 1, -color);   
            }
        }
    }
    return score;
}

int negascout(state_t state, int depth, int alpha, int beta, int color) {
    if (depth == 0 || state.terminal()) {
        expanded++;
        return color * state.value();
    }

    queue<int> moves = state.get_moves(color);
    bool curr_player = color == 1;
    int score, n_moves, child;

    n_moves = moves.size();

    if (!moves.size()) {
        return -negascout(state, depth, -beta, -alpha, -color);
    }

    for (int i = 0; i < n_moves; i++) {
        generated++;

        child = moves.front();
        moves.pop();

        if (i == 0) {
            score = -negascout(state.move(curr_player, child), depth - 1, -beta, -alpha, -color);
        } else {
            generated++;
            score = -negascout(state.move(curr_player, child), depth - 1, -alpha - 1, -alpha, -color);
            
            if (alpha < score && score < beta) {
                score = -negascout(state.move(curr_player, child), depth - 1, -beta, -score, -color);
            }
        }
        alpha = max(alpha, score);

        if (alpha >= beta) break;
    }

    return alpha;
}