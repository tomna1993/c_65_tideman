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
    int winner;
    int loser;
};

bool vote(int rank, char name[MAX_CHARS], int ranks[MAX_CANDIDATES], char candidates[MAX_CANDIDATES][MAX_CHARS], int candidate_count);
bool record_preference(int ranks[MAX_CANDIDATES], int preferences[MAX_PREFERENCE_LENGTH][MAX_PREFERENCE_LENGTH], int candidate_count);


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
    int ranks[candidate_count];

    // Calculate how much should be the array for preferences
    const int preferences_length = candidate_count * (candidate_count - 1);

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

        // DEBUG record_preference
        for (int i = 0; i < candidate_count; i++)
        {
            for (int j = 0; j < candidate_count; j++)
            {
                printf ("Candidate %i is over %i by %i\n", i, j, preferences[i][j]);
            }
        }

        printf ("\n");
    }

    // Add pairs into 

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

bool add_pairs()
{

}

bool sort_pairs()
{

}

bool lock_pairs()
{

}

bool print_winner()
{

}

