#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHARS 20

#define MAX_CANDIDATES 9
#define MAX_VOTERS 9
#define MAX_LOOP 10

#define MAX_PREFERENCE_LENGTH (MAX_CANDIDATES * (MAX_CANDIDATES - 1))

struct pair
{
    int Winner;
    int Loser;
};

bool vote(int rank, char name[MAX_CHARS], int ranks[MAX_CANDIDATES], char candidates[MAX_CANDIDATES][MAX_CHARS], int candidate_count);
bool record_preference(int ranks[MAX_CANDIDATES], int preferences[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH], int candidate_count);
int add_pairs(int preferences[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH], int preference_length, int pairs[MAX_PREFERENCE_LENGTH], struct pair pair_ref[MAX_PREFERENCE_LENGTH]);
bool sort_pairs(int pairs[MAX_PREFERENCE_LENGTH], struct pair pair_ref[MAX_PREFERENCE_LENGTH], int pair_count);
bool lock_pairs(struct pair pair_ref[MAX_PREFERENCE_LENGTH], int pair_count, int candidate_count, bool locked[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH]);
bool is_cycle(int winner, int loser, bool locked[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH], int candidate_count);
bool print_winner(bool locked[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH], char candidates[MAX_CANDIDATES][MAX_CHARS], int candidate_count);


int main(int argc, char *argv[])
{
    // Save number of candidates
    const int candidate_count = argc - 1;

    if (candidate_count < 1 || candidate_count > MAX_CANDIDATES)
    {
        printf ("Usage: ./runoff <candidate1> .. <candidate9>\n");
        return EXIT_FAILURE;
    }

    // Array to save names of candidates
    char candidates[MAX_CANDIDATES][MAX_CHARS];

    // Save candidate names into candidates array
    for (int cand = 0; cand < candidate_count; cand++)
    {
        strcpy(candidates[cand], argv[cand + 1]);
    }


    // Get the number of voters
    int number_of_voters;

    do
    {
        number_of_voters = get_int("Number of voters: ");

        if (number_of_voters < 0 || number_of_voters > MAX_VOTERS)
        {
            printf ("Maximum number of voters can be 0 to %i!\n", MAX_VOTERS);
            continue;
        }

        break;
        
    } while (true);
    
    const int voter_count = number_of_voters;

    printf ("\n");

    // ranks[i] is the index of the candidate who is the i-th preference for the voter
    int ranks[MAX_PREFERENCE_LENGTH];

    // preferences[i][j] represents the number of voters who prefer candidate i over candidate j
    int preferences[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH];

    // Clear preferences
    for (int i = 0; i < MAX_PREFERENCE_LENGTH; i++)
    {
        for (int j = 0; j < MAX_PREFERENCE_LENGTH; j++)
        {
            preferences[i][j] = 0;
        }
    }

    // LOOP to ask for votes
    for (int voter = 0; voter < voter_count; voter++)
    {
        for (int rank = 0; rank < candidate_count; rank++)
        {
            bool valid_vote;
            int loop = 0;

            // If there is more then 10 invalid inputs then exit loop and stop the program with an error
            while (loop < MAX_LOOP)
            {
                char name[MAX_CHARS];
                
                strcpy(name, get_string("Rank %i: ", rank + 1));

                // Get vote; valid == true; invalid == false
                valid_vote = vote(rank, name, ranks, candidates, candidate_count);

                // Get the next vote if valid
                if (valid_vote)
                {
                    break;
                }

                // Invalid vote
                printf ("Invalid vote!\n");
                loop++;

                if (loop == MAX_LOOP)
                {
                    printf ("Too much invalid input! Start the program again!\n");
                    return EXIT_FAILURE;
                }
            }
        }

        // Record voters' preferences
        record_preference(ranks, preferences, candidate_count);

        printf ("\n");
    }

    // DEBUG record_preference
    // printf ("\nRecord Preferences\n");
    // for (int i = 0; i < candidate_count; i++)
    // {
    //     for (int j = 0; j < candidate_count; j++)
    //     {
    //         if (preferences[i][j] != 0)
    //         {
    //             printf ("Candidate %i is over %i by %i\n", i, j, preferences[i][j]);   
    //         }
    //     }
    // }


    // Pairs score 
    int pairs[MAX_PREFERENCE_LENGTH];

    // To save index of the winner and loser in a pair
    struct pair pair[MAX_PREFERENCE_LENGTH];

    // Clear pairs array and pair structure
    for (int i = 0; i < MAX_PREFERENCE_LENGTH; i++)
    {
        pairs[i] = 0;

        pair[i].Winner = 0;
        pair[i].Loser = 0;
    }

    // Check for pairs where a candidate wins over the other candidate with higher margin
    // Return how many pairs there are
    const int pair_count = add_pairs(preferences, MAX_PREFERENCE_LENGTH, pairs, pair);

    if (pair_count <= 0)
    {
        return EXIT_FAILURE;
    }

    //DEBUG add_pairs
    // printf ("\nAdd pairs\n");
    // for (int i = 0; i < pair_count; i++)
    // {
    //     printf ("Candidate %i wins over %i by %i\n", pair[i].Winner, pair[i].Loser, pairs[i]);
    // }

    // Sort pairs in decreasing order
    // Most voted candidate will be the first
    bool is_sorted = sort_pairs(pairs, pair, pair_count);

    if (is_sorted == false)
    {
        return EXIT_FAILURE;
    }

    //DEBUG sort_pairs
    // printf ("\nSorted pairs in decreasing order:\n");
    // for (int i = 0; i < pair_count; i++)
    // {
    //     printf ("Candidate %i wins over %i by %i\n", pair[i].Winner, pair[i].Loser, pairs[i]);
    // }


    // Create candidate graph
    bool locked[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH];

    // Set all lock values to false
    for (int i = 0; i < MAX_PREFERENCE_LENGTH; i++)
    {
        for (int j = 0; j < MAX_PREFERENCE_LENGTH; j++)
        {
            locked[i][j] = false;
        }
    }

    // Lock pairs until they don't create a closed cycle
    bool is_locked = lock_pairs(pair, pair_count, candidate_count, locked);

    if (is_locked == false)
    {
        return EXIT_FAILURE;
    }

    // Print winner if there is any
    // Return false if there is no winner
    bool is_winner = print_winner(locked, candidates, candidate_count);

    if (is_winner)
    {
        return EXIT_SUCCESS;
    }
    
    printf ("No winner!\n");
    return EXIT_FAILURE;
}


bool vote(int rank, char name[MAX_CHARS], int ranks[MAX_CANDIDATES], char candidates[MAX_CANDIDATES][MAX_CHARS], int candidate_count)
{
    for (int cand = 0; cand < candidate_count; cand++)
    {
        // If candidate name exists save candidates' index into ranks array
        // The names are case sensitive!
        if (strcmp(name, candidates[cand]) == 0)
        {
            ranks[rank] = cand;
            return true;
        }
    }

    return false;
}

bool record_preference(int ranks[MAX_CANDIDATES], int preferences[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH], int candidate_count)
{
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            // Increment preference if candidate i preferred over candidate j
            preferences[ranks[i]][ranks[j]]++;
        }   
    }

    return true;
}

// The function returns the pair count
int add_pairs(int preferences[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH], int preference_length, int pairs[MAX_PREFERENCE_LENGTH], struct pair pair_ref[MAX_PREFERENCE_LENGTH])
{
    int pair_count = 0;

    for (int i = 0; i < preference_length - 1; i++)
    {
        for (int j = i + 1; j < preference_length; j++)
        {
            // Candidate i has more votes then candidate j
            if (preferences[i][j] > preferences[j][i])
            {
                // Save how much votes candidate i has over candidate j
                pairs[pair_count] = preferences[i][j];

                // Save winner and loser candidates' index
                pair_ref[pair_count].Winner = i;
                pair_ref[pair_count].Loser = j;

                pair_count++;
            }
            // Candidate j has more votes then candidate i
            else if (preferences[i][j] < preferences[j][i])
            {
                // Save how much votes candidate j has over candidate i
                pairs[pair_count] = preferences[j][i];

                // Save winner and loser candidates' index
                pair_ref[pair_count].Winner = j;
                pair_ref[pair_count].Loser = i;

                pair_count++;
            }

            // If candidate i and candidate j has the same votes then ignore that vote
        }
    }

    return pair_count;
}

// Use bubble-sort algorithm to sort the pairs in decreasing order
bool sort_pairs(int pairs[MAX_PREFERENCE_LENGTH], struct pair pair_ref[MAX_PREFERENCE_LENGTH], int pair_count)
{
    for (int i = 0; i < pair_count; i++)
    {
        bool is_no_change = true; 

        for (int j = 0; j < pair_count - 1; j++)
        {
            if (pairs[j] < pairs[j + 1])
            {
                // Swap pairs value
                int temp = pairs[j];
                pairs[j] = pairs[j + 1];
                pairs[j + 1] = temp;

                // Swap winner indexes
                temp = pair_ref[j].Winner;
                pair_ref[j].Winner = pair_ref[j + 1].Winner;
                pair_ref[j + 1].Winner = temp;

                // Swap loser indexes
                temp = pair_ref[j].Loser;
                pair_ref[j].Loser = pair_ref[j + 1].Loser;
                pair_ref[j + 1].Loser = temp;

                // There was a swap, need to do another cycle
                is_no_change = false;
            }
        }

        // Pairs are sorted
        if (is_no_change)
        {
            return true;
        }
    }
}

bool lock_pairs(struct pair pair_ref[MAX_PREFERENCE_LENGTH], int pair_count, int candidate_count, bool locked[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH])
{   
    int count = 0;

    while (count < pair_count)
    {
        // Before locking a pair into the graph, check if it will close the cycle
        // Return true if locking the pair will create a cycle
        bool is_cycle_created = is_cycle(pair_ref[count].Winner, pair_ref[count].Loser, locked, candidate_count);

        // If locking the pair will create a cycle then don't lock the pair
        if (is_cycle_created)
        {
            count++;
            continue;
        }

        // Else lock the pair and continue checking the next pair
        locked[pair_ref[count].Winner][pair_ref[count].Loser] = true;

        // DEBUG LOCKED PAIRS
        // printf ("%i --> %i\n", pair_ref[count].Winner, pair_ref[count].Loser);

        count++;
    }

    return true;
}

bool is_cycle(int winner, int loser, bool locked[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH], int candidate_count)
{
    // winner is the winner we want to lock in
    // loser is the loser we want to lock in

    // We have to check if the loser has any connection to other candidates
    // and if so, is the other candidate the same as the winner we want to lock in
    // If yes, then we would make a closed cycle if we lock the pair in

    int loop = 0;

    const int max_loop = candidate_count * candidate_count;

    while (loop < max_loop)
    {
        // Check next edge if there is a connection from the loser to any other candidate
        bool check_next_edge = false;

        for (int i = 0; i < candidate_count; i++)
        {
            for (int j = 0; j < candidate_count; j++)
            {
                // loser is the winner of other already locked in pair
                if (locked[i][j] && i == loser)
                {
                    // Is the loser of that locked in pair the same as the winner
                    // of the pair we want to lock in
                    if (j == winner)
                    {
                        // Yes, we would create a closed cycle if we lock the pair in
                        return true;
                    } 
                    // Save the loser of locked in pair and check if he is the winner
                    // of other already locked in pair
                    else
                    {
                        loser = j;
                        check_next_edge = true;

                        break;
                    }
                }
            }

            
            if (check_next_edge)
            {
                break;
            }
        }

        // If there is no edge to check then we can lock the pair in without
        // creating a closed cycle
        if (check_next_edge == false)
        {
            return false;
        }

        loop++;

        // If there is too much loop cycle exit the function and disable to 
        // lock in the pair
        if (loop == max_loop)
        {
            return true;
        }
    }

    return true;
}

bool print_winner(bool locked[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH], char candidates[MAX_CANDIDATES][MAX_CHARS], int candidate_count)
{
    // Select the first candidate
    int candidate = 0;

    while (candidate < candidate_count)
    {
        bool check_next_candidate = false;

        for (int i = 0; i < candidate_count; i++)
        {
            for (int j = 0; j < candidate_count; j++)
            {
                // Check if an edge (arrow) pointing to the given candidate
                // If so, the candidate can't be the winner
                if (locked[i][j] && j == candidate)
                {
                    // So check the next candidate
                    candidate++;
                    check_next_candidate = true;

                    break;
                }
            }

            if (check_next_candidate)
            {
                break;
            }
        }

        // If there is no other candidate to check then we found the winner
        if (check_next_candidate == false)
        {
            printf ("%s\n", candidates[candidate]);
            return true;
        }
    }

    // winner hasn't been found
    return false;
}

