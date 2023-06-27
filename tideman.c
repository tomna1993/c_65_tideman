#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHARS 20

#define MAX_CANDIDATES 9
#define MAX_VOTERS 9

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
bool find_cycle(int winner, int loser, bool locked[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH], int candidate_count);
bool print_winner(bool locked[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH], char candidates[MAX_CANDIDATES][MAX_CHARS], int candidate_count);


int main(int argc, char *argv[])
{
    const int candidate_count = argc - 1;

    if (candidate_count < 1 || candidate_count > MAX_CANDIDATES)
    {
        printf ("Usage: ./runoff <candidate1> .. <candidate9>\n");
        return EXIT_FAILURE;
    }

    char candidates[candidate_count][MAX_CHARS];

    // Save candidate names into candidates array
    for (int cand = 0; cand < candidate_count; cand++)
    {
        strcpy(candidates[cand], argv[cand + 1]);
    }

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

            while (true)
            {
                char name[MAX_CHARS];
                
                strcpy(name, get_string("Rank %i: ", rank + 1));

                valid_vote = vote(rank, name, ranks, candidates, candidate_count);

                if (valid_vote)
                {
                    break;
                }

                printf ("Invalid vote!\n");

            }
        }

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

    
    const int pair_count = add_pairs(preferences, MAX_PREFERENCE_LENGTH, pairs, pair);

    //DEBUG add_pairs
    // printf ("\nAdd pairs\n");
    // for (int i = 0; i < pair_count; i++)
    // {
    //     printf ("Candidate %i wins over %i by %i\n", pair[i].Winner, pair[i].Loser, pairs[i]);
    // }

    sort_pairs(pairs, pair, pair_count);

    //DEBUG sort_pairs
    printf ("\nSorted pairs in decreasing order:\n");
    for (int i = 0; i < pair_count; i++)
    {
        printf ("Candidate %i wins over %i by %i\n", pair[i].Winner, pair[i].Loser, pairs[i]);
    }


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

    lock_pairs(pair, pair_count, candidate_count, locked);

    bool is_winner = print_winner(locked, candidates, candidate_count);

    if (is_winner)
    {
        return EXIT_SUCCESS;
    }
    
    return EXIT_FAILURE;
}


bool vote(int rank, char name[MAX_CHARS], int ranks[MAX_CANDIDATES], char candidates[MAX_CANDIDATES][MAX_CHARS], int candidate_count)
{
    for (int cand = 0; cand < candidate_count; cand++)
    {
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
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count] = preferences[i][j];

                pair_ref[pair_count].Winner = i;
                pair_ref[pair_count].Loser = j;

                pair_count++;
            }
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count] = preferences[j][i];

                pair_ref[pair_count].Winner = j;
                pair_ref[pair_count].Loser = i;

                pair_count++;
            }
        }
    }

    return pair_count;
}

bool sort_pairs(int pairs[MAX_PREFERENCE_LENGTH], struct pair pair_ref[MAX_PREFERENCE_LENGTH], int pair_count)
{
    for (int i = 0; i < pair_count - 1; i++)
    {
        bool is_no_change = true; 

        for (int j = 0; j < pair_count - 1; j++)
        {
            if (pairs[j] < pairs[j + 1])
            {
                int temp = pairs[j];
                pairs[j] = pairs[j + 1];
                pairs[j + 1] = temp;

                temp = pair_ref[j].Winner;
                pair_ref[j].Winner = pair_ref[j + 1].Winner;
                pair_ref[j + 1].Winner = temp;

                temp = pair_ref[j].Loser;
                pair_ref[j].Loser = pair_ref[j + 1].Loser;
                pair_ref[j + 1].Loser = temp;

                is_no_change = false;
            }
        }

        if (is_no_change)
        {
            return true;
        }
    }
}

bool lock_pairs(struct pair pair_ref[MAX_PREFERENCE_LENGTH], int pair_count, int candidate_count, bool locked[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH])
{   
    if (pair_count == 0)
    {
        return false;
    }

    int count = 0;

    while (count < pair_count)
    {
        bool is_cycle = find_cycle(pair_ref[count].Winner, pair_ref[count].Loser, locked, candidate_count);

        if (is_cycle)
        {
            count++;
            continue;
        }

        locked[pair_ref[count].Winner][pair_ref[count].Loser] = true;

        printf ("%i --> %i\n", pair_ref[count].Winner, pair_ref[count].Loser);

        count++;
    }
    
    return true;
}

bool find_cycle(int winner, int loser, bool locked[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH], int candidate_count)
{
    while (true)
    {
        bool check_next_edge = false;

        for (int i = 0; i < candidate_count; i++)
        {
            for (int j = 0; j < candidate_count; j++)
            {
                if (locked[i][j] && i == loser)
                {
                    if (j == winner)
                    {
                        return true;
                    } 

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

        if (check_next_edge == false)
        {
            return false;
        }
    }
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
                if (locked[i][j] && j == candidate)
                {
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

        if (check_next_candidate == false)
        {
            printf ("%s\n", candidates[candidate]);
            return true;
        }
    }

    return false;
}

