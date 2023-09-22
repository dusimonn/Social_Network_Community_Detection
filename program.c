/* A simplified community detection algorithm:
 *
 * Skeleton code written by Jianzhong Qi, May 2023
 * Edited by: [Du-Simon Nguyen 1352062]
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STAGE_NUM_ONE   1 /* stage numbers */
#define STAGE_NUM_TWO   2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR  4
#define STAGE_HEADER    "Stage %d\n==========\n" /* stage header format string */
#define MAX_HASH        10
#define MAX_HASH_LENGTH 20
#define MAX_USERS       50
#define MAX_SPACES      9
#define NOT_CLOSE       -1
#define CORE_USER       1
#define NOT_CORE_USER   0
#define HASH_PER_LINE   5

typedef struct {
    /* add your user_t struct definition */
    int id;

    int year;

    char hashtags[MAX_HASH][MAX_HASH_LENGTH + 1];

    int num_hashtags;

    int core_user;

    int friendship[MAX_USERS];

    float soc[MAX_USERS];

    int close_friends[MAX_USERS];

} user_t;

typedef char data_t[MAX_HASH_LENGTH + 1]; /* to be modified for Stage 4 */

/* linked list type definitions below, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
*/
typedef struct node node_t;

struct node {
    data_t data;
    node_t *next;
};

typedef struct {
    node_t *head;
    node_t *foot;
} list_t;

/****************************************************************/

/* function prototypes */
list_t *make_empty_list(void);
void free_list(list_t *list);
void print_list(list_t *list);
list_t *insert_unique_in_order(list_t *list, data_t value);

void print_stage_header(int stage_num);

int stage_one(user_t users[], int n);
void stage_two(user_t users[], int n);
void stage_three(user_t users[], int n);
void stage_four(user_t users[], int n);

/* add your own function prototypes here */

int readusers(user_t users[], int n);
void store_input_into_array(int userid, int useryear, char allhashes[],
                            user_t users[]);
user_t find_largest(user_t users[], int num_users);

void read_fship_m(user_t users[], int num_users);
float s_o_c(user_t users[], int num_users, int u0, int u1);
int fset(user_t users[], int num_users, int u, int f[]);
int find_union(int f0_union_f1[], int f0[], int f1[], int len1, int len2,
               int num_users);
int find_inter(int f0_inter_f1[], int f0[], int f1[], int len1, int len2,
               int num_users);

void read_ths_thc(float *ths, int *thc);
int is_close(int i, int j, float ths, user_t users[MAX_USERS],
             int close_friend_index);
void is_core(int num_close_friends, int thc, user_t users[MAX_USERS], int i);
void print_four(user_t users[], int row_len, list_t *mylist, int i);

/****************************************************************/

/* main function controls all the action; modify if needed */
int
main(int argc, char *argv[]) {
    /* add variables to hold the input data */

    user_t users[MAX_USERS];

    /* stage 1: read user profiles */
    int num_users = stage_one(users, MAX_USERS);

    /* stage 2: compute the strength of connection between u0 and u1 */
    stage_two(users, num_users);

    /* stage 3: compute the strength of connection for all user pairs */
    stage_three(users, num_users);

    /* stage 4: detect communities and topics of interest */
    stage_four(users, num_users);

    /* algorithms are fun */

    /* all done; take some rest */
    return 0;
}

/****************************************************************/

/* add your code below; you can also modify the function return type
   and parameter list
*/

/* stage 1: read user profiles */
int
stage_one(user_t users[], int n) {
    /* add code for stage 1 */

    /* store input */
    int num_users = readusers(users, n);

    /* find largest user with greatest hashtags */
    user_t largest = find_largest(users, num_users);

    /* print stage header */
    print_stage_header(STAGE_NUM_ONE);
    printf("Number of users: %d\n", num_users);
    printf("u%d has the largest number of hashtags:\n", largest.id);
    for (int i = 0; i < largest.num_hashtags; i++) {
        if (i == largest.num_hashtags - 1) {
            printf("#%s", largest.hashtags[i]);
        } else {
            printf("#%s ", largest.hashtags[i]);
        }
    }

    printf("\n");
    return num_users;
}

/* reads in the user input and returns user with largest number of
 * hashtags*/
int
readusers(user_t users[], int n) {

    user_t user;

    int num_users = 0;

    /* the longest #string is = num spaces + (max num # * max hashlength) */
    char allhashes[MAX_HASH * MAX_HASH_LENGTH + MAX_SPACES];

    /* scans in the entire line, stores all the hashes into 1 string, then break
     * down in store_input_into_array() */
    while (scanf("u%d %d %[^\n]%*c", &user.id, &user.year, allhashes) == 3) {
        store_input_into_array(user.id, user.year, allhashes, users);
        num_users++;
    }

    return num_users;
}

/* adds the input row into the array of user structures */
void
store_input_into_array(int userid, int useryear, char allhashes[],
                       user_t users[]) {

    users[userid].id = userid;
    users[userid].year = useryear;

    int hashcount = 0;

    /* split the entire string into individual hashtags and store */
    for (int i = 0; allhashes[i] != '\0'; i++) {

        if (allhashes[i] == '#') {
            hashcount++;
            for (int j = i + 1; allhashes[j] != ' ' && allhashes[j] != '\0';
                 j++) {
                users[userid].hashtags[hashcount - 1][j - i - 1] = allhashes[j];
            }
            /* terminate the hashtag array after the last hashtag */
        }
    }

    users[userid].num_hashtags = hashcount;
}

/* finds the user with the greatest number of hashtags */
user_t
find_largest(user_t users[], int num_users) {

    user_t largest_user;

    int max_num_hash = 0;

    for (int i = 0; i < num_users; i++) {
        if (users[i].num_hashtags > max_num_hash) {
            max_num_hash = users[i].num_hashtags;
            largest_user = users[i];
        }
    }

    return largest_user;
}

/* stage 2: compute the strength of connection between u0 and u1 */
void
stage_two(user_t users[], int num_users) {
    /* add code for stage 2 */

    read_fship_m(users, num_users);

    int u0 = 0;
    int u1 = 1;

    /* determine the entire soc matrix */
    for (int i = 0; i < num_users; i++) {
        for (int j = 0; j < num_users; j++) {
            users[i].soc[j] = s_o_c(users, num_users, i, j);
        }
    }

    printf("\n");

    /* print stage header */
    print_stage_header(STAGE_NUM_TWO);
    /* just print the first element of the soc matrix */
    printf("Strength of connection between u0 and u1: %4.2f\n",
           users[u0].soc[u1]);

    printf("\n");
}

/* read in square matrix for step 2 and 3 */
void
read_fship_m(user_t users[], int num_users) {

    for (int i = 0; i < num_users; i++) {
        for (int j = 0; j < num_users; j++) {
            scanf("%d", &users[i].friendship[j]);
        }
    }
}

/* determines strength of connection between u0 and u1 */
float
s_o_c(user_t users[], int num_users, int u0, int u1) {

    float soc;
    int f0[num_users];
    int f1[num_users];
    int f0_union_f1[num_users];
    int f0_inter_f1[num_users];

    /* case where u0 and u1 are not in a friendship */
    if ((users[u0].friendship[u1] == 0) || (users[u1].friendship[u0] == 0)) {
        soc = 0;
    } else {
        /* determine the union and intersection set to determine soc value */
        int len1 = fset(users, num_users, u0, f0);
        int len2 = fset(users, num_users, u1, f1);

        float unionnum = find_union(f0_union_f1, f0, f1, len1, len2, num_users);
        float intnum = find_inter(f0_inter_f1, f0, f1, len1, len2, num_users);

        soc = intnum / unionnum;
    }

    return soc;
}

/* finds sets/num of users who are in a friendship with the input user */
int
fset(user_t users[], int num_users, int u, int f[]) {

    int index = 0;
    for (int i = 0; i < num_users; i++) {
        if (users[u].friendship[i] == 1) {
            f[index] = i;
            index++;
        }
    }

    return index;
}

/* finds union set between F(u0) and F(u1) */
int
find_union(int f0_union_f1[], int f0[], int f1[], int len1, int len2,
           int num_users) {

    /* copy f0 into union array */
    int i;
    for (i = 0; i < len1; i++) {
        f0_union_f1[i] = f0[i];
    }

    int index = i;

    /* add any extra elements in f1 into union array */
    for (int i = 0; i < len2; i++) {
        int is_duplicate = 0;
        for (int j = 0; j < len1; j++) {
            if (f1[i] == f0[j]) {
                is_duplicate = 1;
                break;
            }
        }
        if (!is_duplicate) {
            f0_union_f1[index] = f1[i];
            index++;
        }
    }

    return index;
}

/* finds intersection set between F(u0) and F(u1) */
int
find_inter(int f0_inter_f1[], int f0[], int f1[], int len1, int len2,
           int num_users) {

    int index = 0;

    for (int i = 0; i < len1; i++) {
        for (int j = 0; j < len2; j++) {
            if (f0[i] == f1[j]) {
                f0_inter_f1[index] = f0[i];
                index++;
            }
        }
    }

    return index;
}

/* stage 3: compute the strength of connection for all user pairs */
void
stage_three(user_t users[], int num_users) {
    /* add code for stage 3 */

    /* print stage header */
    print_stage_header(STAGE_NUM_THREE);
    /* just print the soc matrix that was determined in stage 2 */
    for (int i = 0; i < num_users; i++) {
        for (int j = 0; j < num_users; j++) {
            if (j == num_users - 1) {
                printf("%4.2f", users[i].soc[j]);
            } else {
                printf("%4.2f ", users[i].soc[j]);
            }
        }
        printf("\n");
    }

    printf("\n");
}

/* stage 4: detect communities and topics of interest */
void
stage_four(user_t users[], int num_users) {
    /* add code for stage 4 */

    /* stage 4.1 */
    float ths;
    int thc;

    /* read in ths and thc values */
    read_ths_thc(&ths, &thc);

    /* determine the close friends of each user and whether that user is core */
    for (int i = 0; i < num_users; i++) {
        int close_friend_index = 0;
        int is_close_friend = 0; /* flag for closeness between u[i] and u[j] */
        int num_close_friends = 0; /* total number of close friends for u[i] */
        for (int j = 0; j < num_users; j++) {
            /* first determine whether ui and uj are close */
            is_close_friend = is_close(i, j, ths, users, close_friend_index);
            num_close_friends += is_close_friend;
            close_friend_index++;
        }
        /* next determine whether ui is a core user */
        is_core(num_close_friends, thc, users, i);
    }

    /* print stage header */
    print_stage_header(STAGE_NUM_FOUR);
    for (int i = 0; i < num_users; i++) {
        /* ouput stage 4.1, add to lists and output 4.2 */
        if (users[i].core_user == 1) {
            list_t *mylist = make_empty_list();
            print_four(users, num_users, mylist, i);
        }
    }
}

/* read in ths and thc from input */
void
read_ths_thc(float *ths, int *thc) {

    scanf("%f %d", ths, thc);
}

/* determines whether user[i] and user[j] are close */
int
is_close(int i, int j, float ths, user_t users[MAX_USERS],
         int close_friend_index) {

    int close = 0;

    /* stores closeness info in an array for each user */
    if (users[i].soc[j] > ths) {
        users[i].close_friends[close_friend_index] = j;
        close = 1;
    } else {
        users[i].close_friends[close_friend_index] = NOT_CLOSE;
        close = 0;
    }

    return close;
}

/* determines whether user[i] is a core user */
void
is_core(int num_close_friends, int thc, user_t users[MAX_USERS], int i) {

    /* stores an int (1 or 0) in the user array for coreness */
    if (num_close_friends > thc) {
        users[i].core_user = CORE_USER;
    } else {
        users[i].core_user = NOT_CORE_USER;
    }
}

/* prints output for stage 4.1, 4.2 and creates the # lists */
void
print_four(user_t users[], int row_len, list_t *mylist, int i) {

    /* 4.2 insert core user u[i] hashtags into list */
    for (int k = 0; k < users[i].num_hashtags; k++) {
        mylist = insert_unique_in_order(mylist, users[i].hashtags[k]);
    }

    /* 4.1 output */
    printf("Stage 4.1. Core user: u%d; close friends:", i);
    for (int j = 0; j < row_len; j++) {
        if (users[i].close_friends[j] != NOT_CLOSE) {
            printf(" u%d", users[i].close_friends[j]);
            /* 4.2. if u[i] is close with u[j], insert u[j] hashtags */
            for (int k = 0; k < users[j].num_hashtags; k++) {
                mylist = insert_unique_in_order(mylist, users[j].hashtags[k]);
            }
        }
    }

    /* 4.2 output */
    printf("\nStage 4.2. Hashtags:\n");
    print_list(mylist);

    /* free memory for list for next core user */
    free_list(mylist);
}

/****************************************************************/
/* functions provided, adapt them as appropriate */

/* print stage header given stage number */
void
print_stage_header(int stage_num) {
    printf(STAGE_HEADER, stage_num);
}

/****************************************************************/
/* linked list implementation below, adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
*/

/* create an empty list */
list_t *
make_empty_list(void) {
    list_t *list;

    list = (list_t *) malloc(sizeof(*list));
    assert(list != NULL);
    list->head = list->foot = NULL;

    return list;
}

/* free the memory allocated for a list (and its nodes) */
void
free_list(list_t *list) {
    node_t *curr, *prev;

    assert(list != NULL);
    curr = list->head;
    while (curr) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }

    free(list);
}

/* insert a new data element into a linked list, keeping the
        data elements in the list unique and in alphabetical order
        adapted from Ed Ex10.x2 Linked Lists and listops.c
*/
list_t *
insert_unique_in_order(list_t *list, data_t value) {
    node_t *new, *prev, *curr;

    /* create node to store the hashtag string */
    new = (node_t *) malloc(sizeof(*new));
    assert(list != NULL && new != NULL);
    strcpy(new->data, value);
    new->next = NULL;

    /* if list is empty, then point head and foot to new node */
    if (list->foot == NULL) {
        list->head = new;
        list->foot = new;
    } else {
        /* list is not empty, so traverse and compare curr and prev string */
        prev = NULL;
        curr = list->head;
        while (curr) {
            int cmp_result = strcmp(curr->data, value);
            if (cmp_result == 0) {
                /* not unique value, so don't update list */
                free(new);
                return list;
            } else if (cmp_result > 0) {
                /* found location to insert node */
                break;
            }
            prev = curr;
            curr = curr->next;
        }

        /* insert string into the location */
        if (prev == NULL) {
            /* nothing before the new node, so insert at head */
            new->next = list->head;
            list->head = new;
        } else {
            new->next = prev->next;
            prev->next = new;
        }

        /* point foot to new node if it is at end of list */
        if (new->next == NULL) {
            list->foot = new;
        }
    }
    return list;
}

/* print the data contents of a list */
void
print_list(list_t *list) {
    /* add code to print list */
    assert(list != NULL);
    node_t *curr;
    curr = list->head;
    int hash_count = 0;

    while (curr) {
        if (hash_count == HASH_PER_LINE - 1 || curr->next == NULL) {
            printf("#%s\n", curr->data);
            hash_count = 0;
        } else {
            printf("#%s ", curr->data);
            hash_count++;
        }
        curr = curr->next;
    }
}

/****************************************************************/
/*
        Write your time complexity analysis below for Stage 4.2,
        assuming U users, C core users, H hashtags per user,
        and a maximum length of T characters per hashtag:

        For all the users, we check if user is core O(1), previously determined
        in stage_four, then make a list for that user O(1), then run the code
        for 4.2 in print_four(). (Thus no C in the time complexity analysis)
        -> O(U * print_four())

        In print_four(), we first insert hashtags for that input core
        user then insert hashtags for the close friends of the core user.
        Then we print the associated linked list for that user,
        which is given by num_nodes =  num_hashtags * strlen = O(H*T).
        -> O(print_four()) = O(insert core) + O(insert close) + O(print)
        -> O(print_four()) = O(H * insert) + O(U * H * insert) + O(H*T)
        -> The (insert close) dominates so O(print_four()) =  O(U * H * insert).

        In our linked list insert implementation, we first copy
        the hashtag (O(T)), then traverse the linked list to insert
        in correct location, on average the linked list is H hashtags long.
        So we need to run strcmp at each node, hence O(H*T).
        -> O(insert) = O(T) + O(H*T)
        -> The latter dominates so O(H * T).

        Thus, O(stage 4.2) = O(U * print_four())
        => O(U * U * H * insert)
        => O(U * U * H * H * T)
        => O(UUHHT)

*/