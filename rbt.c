#include <stdlib.h>
#include <string.h>
#include "rbt.h"
#include "mylib.h"

#define IS_BLACK(x) ((NULL == (x) || (BLACK == (x)->colour)))
#define IS_RED(x) ((NULL != (x) && (RED == (x)->colour)))

struct rbt_node {
    char *key;
    int count;
    rbt_colour colour;
    rbt left;
    rbt right;
};

rbt rbt_new() {
    return NULL;
}

rbt rbt_free(rbt r) {
    if (r != NULL) {
        r->left = rbt_free(r->left);
        r->right = rbt_free(r->right);
        free(r->key);
        free(r);
        r = NULL;
    }    

    return NULL;
}

/**
* Performs a left rotation on a rbt.
* Must assign the result to original rbt to have an effect.
* E.g. rbt r = left_rotate(r)
* @param rbt r The rbt to rotate.
* @return The rotated rtb.
*/
static rbt left_rotate(rbt r) {
    rbt temp = r;
    r = r->right;
    temp->right = r->left;
    r->left = temp;
    return r;
}

/**
 * Performs a right rotation on a rbt.
 * Must assign the result to original rbt to have an effect.
 * E.g. rbt r = right_rotate(r)
 * @param rbt r The rbt to rotate.
 * @return The rotated rbt.
 */
static rbt right_rotate(rbt r) {
    rbt temp = r;
    r = r->left;
    temp->left = r->right;
    r->right = temp;
    return r;
}

/**
 * Fixes any issues that violate the properties of a rbt.
 * Must assign the result to original rbt to have an effect.
 * E.g. rbt r = right_rotate(r)
 * @param rbt r The rbt to fix.
 * @return The fixed rbt.
 */
 static rbt rbt_fix(rbt r) {
    if (r == NULL) {
        return NULL;
    }

    if (IS_RED(r->left) && IS_RED(r->left->left)) {
        if (IS_RED(r->right)) {
            r->colour = RED;
            r->left->colour = BLACK;
            r->right->colour = BLACK;
        } else {
            r = right_rotate(r);
            r->colour = BLACK;
            r->right->colour = RED;
        }
    } else if (IS_RED(r->left) && IS_RED(r->left->right)) {
        if (IS_RED(r->right)) {
            r->colour = RED;
            r->left->colour = BLACK;
            r->right->colour = BLACK;
        } else {
            r->left = left_rotate(r->left);
            r = right_rotate(r);
            r->colour = BLACK;
            r->right->colour = RED;
        }
    } else if (IS_RED(r->right) && IS_RED(r->right->left)) {
        if (IS_RED(r->left)) {
            r->colour = RED;
            r->left->colour = BLACK;
            r->right->colour = BLACK;
        } else {
            r->right = right_rotate(r->right);
            r = left_rotate(r);
            r->colour = BLACK;
            r->left->colour = RED;
        }
    } else if (IS_RED(r->right) && IS_RED(r->right->right)) {
        if (IS_RED(r->left)) {
            r->colour = RED;
            r->left->colour = BLACK;
            r->right->colour = BLACK;
        } else {
            r = left_rotate(r);
            r->colour = BLACK;
            r->left->colour = RED;
        }
    }
    
    return r;
}

/**
 * A wrapper for the function rbt_insert.
 * Should only be called by the function rbt_insert so that after the key has
 * been inserted, rbt_insert can then ensure the root node is always black.
 * Must assign the result to original rbt to have an effect.
 * E.g. rbt r = rbt_insert_helper(r, key)
 * @param rbt r The rbt to insert into.
 * @param char *key The key to insert.
 * @return The rbt after the key is inserted.
 */ 
 static rbt rbt_insert_helper(rbt r, char *key) {
    if (r == NULL) {
        r = emalloc(sizeof *r);
        r->key = emalloc(strlen(key) + 1);
        strcpy(r->key, key);
        r->count = 1;
        r->colour = RED;
        r->left = NULL;
        r->right = NULL;
    } else if (strcmp(key, r->key) < 0) {
        r->left = rbt_insert_helper(r->left, key);
    } else if (strcmp(key, r->key) > 0 ) {
        r->right = rbt_insert_helper(r->right, key);
    } else {
        r->count++;
    }
    
    return rbt_fix(r);
}

rbt rbt_insert(rbt r, char *key) {
    r = rbt_insert_helper(r, key);
    /* Ensure that the root node is always coloured black. */
    r->colour = BLACK;

    return r;
}

int rbt_search(rbt r, char *key) {
    int comparison;

    if (r == NULL) {
        return 0;
    }

    comparison = strcmp(key, r->key);

    if (comparison < 0) {
        return rbt_search(r->left, key);
    } 
    if (comparison > 0) {
        return rbt_search(r->right, key);
    }

    return r->count; 
}

rbt rbt_delete(rbt r, char *key) {
    int comparison;
    rbt temp, successor;
    char *swap_key;
    int swap_count;
    
    if (r == NULL) {
        return NULL;
    }

    comparison = strcmp(key, r->key);
    
    if (comparison < 0) {
        r->left = rbt_delete(r->left, key);
    } else if (comparison > 0) {    
        r->right = rbt_delete(r->right, key);
    } else {
        if (r->count > 1) {
            r->count--;
        } else if (r->left && r->right) {
            successor = r->right;
            
            while(successor->left) {
                successor = successor->left;
            }

            swap_key = r->key;
            r->key = successor->key;
            successor->key = swap_key;
            
            swap_count = r->count;
            r->count = successor->count;
            successor->count = swap_count;

            r->right = rbt_delete(r->right, key);
        } else if (r->left || r->right) { 
            temp = r;
            r = (r->left) ? r->left : r->right;
            free(temp->key);
            free(temp);                
        } else {
            free(r->key);
            free(r);
            r = NULL;     
        }        
    } 

    return r;
}

void rbt_inorder(rbt r, void f(char *key)) {
    int i;
    
    if (r == NULL) {
        return;
    }
    
    rbt_inorder(r->left, f);

    for (i = 0; i < r->count; i++) {
        f(r->key);
    }    

    rbt_inorder(r->right, f);
}

void rbt_preorder(rbt r, void f(char *key)) {
    int i;

    if (r == NULL) {
        return;
    }
    
    for (i = 0; i < r->count; i++) {
        f(r->key);
    }
    
    rbt_preorder(r->left, f);
    rbt_preorder(r->right, f);
}

void rbt_postorder(rbt r, void f(char *key)) {
    int i;

    if (r == NULL) {
        return;
    }
        
    rbt_postorder(r->left, f);
    rbt_postorder(r->right, f);

    for (i = 0; i < r->count; i++) {
        f(r->key);
    }
}