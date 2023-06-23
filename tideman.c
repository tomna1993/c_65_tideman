#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHARS 20

#define MAX_CANDIDATES 9
#define MAX_VOTERS 9

struct pair
{
    int winner;
    int loser;
};

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

    // LOOP to ask for votes
    for (int voter = 0; voter < voter_count; voter++)
    {
        for (int rank = 0; rank < candidate_count; rank++)
        {
            bool valid_vote;

            do
            {
                char name[MAX_CHARS];
                
                strcpy(name, get_string("Rank %i: ", rank + 1));

                // valid_vote = vote();

            } while (valid_vote == false);
        }

        printf ("\n");
    }

    return EXIT_FAILURE;
}


bool vote()
{

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

