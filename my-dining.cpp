#ifndef LOCKS_FREE_H
#define LOCKS_FREE_H

#include <atomic>
#include <vector>

struct PetersonLock {
    std::atomic<bool> flag0;
    std::atomic<bool> flag1;
    std::atomic<int> turn;
};

void init_peterson(PetersonLock* lock) {
    lock->flag0 = false;
    lock->flag1 = false;
    lock->turn = 0;
}

void acquire_peterson(PetersonLock* lock, int thread_id) {
    if (thread_id == 0) {
        lock->flag0 = true;
        lock->turn = 1;
        
        while (lock->flag1 == true && lock->turn == 1) {
            //spinning/waiting
        }
    } else if (thread_id == 1) {
        lock->flag1 = true;
        lock->turn = 0;
        
        while (lock->flag0 == true && lock->turn == 0) {
            //waiting
        }
    }
}

//release function that accepts a pointer to lock state
void release_peterson(PetersonLock* lock, int thread_id) {
    if (thread_id == 0) {
        lock->flag0 = false;
    } else if (thread_id == 1) {
        lock->flag1 = false;
    }
}

//part 2
struct TournamentTreeLock {
    std::vector<PetersonLock*> tree;
    int num_leaves;
};

void init_tournament_tree(TournamentTreeLock* tt, int n) {
    //find following power of 2 to handle unused pos when n is not power of 2
    tt->num_leaves = 1;
    while (tt->num_leaves < n) {
        tt->num_leaves = tt->num_leaves * 2;
    }

    //make 1 index array for binary tree
    tt->tree.resize(2 * tt->num_leaves);
    int i = 1;
    // FIXED: Added missing closing parenthesis
    while (i < 2 * tt->num_leaves) 
    {
       tt->tree[i] = new PetersonLock();
       init_peterson(tt->tree[i]);
       i++;
    }
}

//clean up to prevent memory leaks
void cleanup_tournament_tree(TournamentTreeLock* tt) {
    int i = 1;
    while (i < tt->tree.size()) 
    {
       delete tt->tree[i];
       i++;
    }
}

//Thread aquires locks along path from leaf to root
void acquire_tournament(TournamentTreeLock* tt, int thread_id) {
    // FIXED: Removed redundant standalone current_node declaration
    for (int current_node = tt->num_leaves + thread_id; current_node > 1; current_node = current_node / 2) 
    {
        int parent = current_node / 2;
        int side = current_node % 2;
        acquire_peterson(tt->tree[parent], side);
    }
}

//A thread lets locks go in reverse order from root to leaf
void release_tournament(TournamentTreeLock* tt, int thread_id) {
    std::vector<int> path;

    //save the path from leaf to root
    // FIXED: Removed redundant standalone current_node declaration
    for (int current_node = tt->num_leaves + thread_id; current_node > 1; current_node = current_node / 2) 
    {
        path.push_back(current_node);
    }

    //reverse traverse to release locks
    for (int i = path.size() - 1; i >= 0; i--) {
        int node = path[i];
        int parent = node / 2;
        int side = node % 2;
        
        release_peterson(tt->tree[parent], side);
    }
}

#endif