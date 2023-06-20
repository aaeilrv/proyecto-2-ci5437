// Game of Othello -- Example of main
// Universidad Simon Bolivar, 2012.
// Author: Blai Bonet
// Last Revision: 1/11/16
// Modified by: 

#include <iostream>
#include <limits>
#include "algorithms.h"

#include <unordered_map>

using namespace std;

unsigned expanded = 0;
unsigned generated = 0;
int tt_threshold = 32; // threshold to save entries in TT

int negamax(state_t state, int depth, int color);
int negamax_alpha_beta(state_t state, int depth, int alpha, int beta, int color, bool use_tt);
int negascout(state_t state, int depth, int alpha, int beta, int color, bool use_tt);

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
                value = negamax_alpha_beta(pv[i], 0, -200, 200, color, use_tt);
            } else if( algorithm == 3 ) {
                //value = scout(pv[i], 0, color, use_tt);
            } else if( algorithm == 4 ) {
                value = negascout(pv[i], 0, -200, 200, color, use_tt);
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
    if (depth == 0 || state.terminal()) {
        expanded++;
        return color * state.value();
    }

    queue<int> move = state.get_moves(color);     
    int child, alpha = INT_MIN;
    bool curr_player = color == 1;

    if (move.size() == 0) {
        generated++;
        alpha = -negamax(state, depth, -color);
    }

    while (!move.empty()) {
        generated++;
        child = move.front();
        move.pop();
        alpha = max(alpha, -negamax(state.move(curr_player, child), depth-1, -color));
    }
    return alpha;
}

int negamax_alpha_beta(state_t state, int depth, int alpha, int beta, int color, bool use_tt) {
    generated++;
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
    expanded++;
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
    generated++;
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
    expanded++;
    return score;
}*/

int negascout(state_t state, int depth, int alpha, int beta, int color, bool use_tt) {
    generated++;
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

    expanded++;
    return alpha;
}