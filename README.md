# c_65_tideman

## DESCRIPTION

For this program, you’ll implement a program that runs a Tideman election, per the below.

```bash
$ ./tideman Alice Bob Charlie
Number of voters: 5
Rank 1: Alice
Rank 2: Charlie
Rank 3: Bob

Rank 1: Alice
Rank 2: Charlie
Rank 3: Bob

Rank 1: Bob
Rank 2: Charlie
Rank 3: Alice

Rank 1: Bob
Rank 2: Charlie
Rank 3: Alice

Rank 1: Charlie
Rank 2: Alice
Rank 3: Bob

Charlie
```

### BACKGROUND

You already know about plurality elections, which follow a very simple algorithm for determining the winner of an election: every voter gets one vote, and the candidate with the most votes wins.

But the plurality vote does have some disadvantages. What happens, for instance, in an election with three candidates, and the ballots below are cast?

![plurality](./image/fptp_ballot_1.png)

A plurality vote would here declare a tie between Alice and Bob, since each has two votes. But is that the right outcome?

There’s another kind of voting system known as a ranked-choice voting system. In a ranked-choice system, voters can vote for more than one candidate. Instead of just voting for their top choice, they can rank the candidates in order of preference. The resulting ballots might therefore look like the below.

![ranked](./image/ranked_ballot_1.png)

Here, each voter, in addition to specifying their first preference candidate, has also indicated their second and third choices. And now, what was previously a tied election could now have a winner. The race was originally tied between Alice and Bob, so Charlie was out of the running. But the voter who chose Charlie preferred Alice over Bob, so Alice could here be declared the winner.

Ranked choice voting can also solve yet another potential drawback of plurality voting. Take a look at the following ballots.

![ranked](./image/ranked_ballot_3.png)

Who should win this election? In a plurality vote where each voter chooses their first preference only, Charlie wins this election with four votes compared to only three for Bob and two for Alice. Note that, if you’re familiar with the instant runoff voting system, Bob wins under that system.

Bob is, in this election, the so-called “Condorcet winner” of the election: the person who would have won any head-to-head matchup against another candidate. If the election had been just Bob and Alice, or just Bob and Charlie, Bob would have won.

The Tideman voting method (also known as “ranked pairs”) is a ranked-choice voting method that’s guaranteed to produce the Condorcet winner of the election if one exists.

Generally speaking, the Tideman method works by constructing a “graph” of candidates, where an arrow (i.e. edge) from candidate A to candidate B indicates that candidate A wins against candidate B in a head-to-head matchup. The graph for the above election, then, would look like the below.

![condorcet graph](./image/condorcet_graph_1.png)

The arrow from Bob to Alice means that more voters prefer Bob to Alice (5 prefer Bob, 4 prefer Alice). Likewise, the other arrows mean that more voters prefer Bob to Charlie, and more voters prefer Alice to Charlie.

Looking at this graph, the Tideman method says the winner of the election should be the “source” of the graph (i.e. the candidate that has no arrow pointing at them). In this case, the source is Bob — Bob is the only one who has no arrow pointing at him, which means nobody is preferred head-to-head over Bob. Bob is thus declared the winner of the election.

It’s possible, however, that when the arrows are drawn, there is no Condorcet winner. Consider the below ballots.

![no winner](./image/no_condorcet_1.png)

Between Alice and Bob, Alice is preferred over Bob by a 7-2 margin. Between Bob and Charlie, Bob is preferred over Charlie by a 5-4 margin. But between Charlie and Alice, Charlie is preferred over Alice by a 6-3 margin. If we draw out the graph, there is no source! We have a cycle of candidates, where Alice beats Bob who beats Charlie who beats Alice (much like a game of rock-paper-scissors). In this case, it looks like there’s no way to pick a winner.

To handle this, the Tideman algorithm must be careful to avoid creating cycles in the candidate graph. How does it do this? The algorithm locks in the strongest edges first, since those are arguably the most significant. In particular, the Tideman algorithm specifies that matchup edges should be “locked in” to the graph one at a time, based on the “strength” of the victory (the more people who prefer a candidate over their opponent, the stronger the victory). So long as the edge can be locked into the graph without creating a cycle, the edge is added; otherwise, the edge is ignored.

How would this work in the case of the votes above? Well, the biggest margin of victory for a pair is Alice beating Bob, since 7 voters prefer Alice over Bob (no other head-to-head matchup has a winner preferred by more than 7 voters). So the Alice-Bob arrow is locked into the graph first. The next biggest margin of victory is Charlie’s 6-3 victory over Alice, so that arrow is locked in next.

Next up is Bob’s 5-4 victory over Charlie. But notice: if we were to add an arrow from Bob to Charlie now, we would create a cycle! Since the graph can’t allow cycles, we should skip this edge, and not add it to the graph at all. If there were more arrows to consider, we would look to those next, but that was the last arrow, so the graph is complete.

This step-by-step process is shown below, with the final graph at right.

![arrow lock](./image/lockin.png)

Based on the resulting graph, Charlie is the source (there’s no arrow pointing towards Charlie), so Charlie is declared the winner of this election.

Put more formally, the Tideman voting method consists of three parts:

- `Tally`: Once all of the voters have indicated all of their preferences, determine, for each pair of candidates, who the preferred candidate is and by what margin they are preferred.

- `Sort`: Sort the pairs of candidates in decreasing order of strength of victory, where strength of victory is defined to be the number of voters who prefer the preferred candidate.

- `Lock`: Starting with the strongest pair, go through the pairs of candidates in order and “lock in” each pair to the candidate graph, so long as locking in that pair does not create a cycle in the graph.
Once the graph is complete, the source of the graph (the one with no edges pointing towards it) is the winner!

### IMPLEMENTATION DETAILS

- create `tideman.c`

- Define `MAX_CANDIDATES` for the maximum number of candidates in the election, and `MAX_VOTERS` for the maximum number of voters in the election.

- The candidates themselves are stored in the array `candidates`, which is an array of `strings` representing the names of each of the candidates.
- Declare `candidate_count` variable, representing the number of candidates in the `candidates` array.

- Next, the program loops over all of the voters and collects their preferences in an array called `ranks` (via a call to `vote` function), where `ranks[i]` is the index of the candidate who is the `i`th preference for the voter.
  - `vote` takes arguments `rank`, `name` and `ranks`. If `name` is a match for the name of a valid candidate, then you should update the `ranks` array to indicate that the voter `voter` has that candidate as their `rank` preference (where `0` is the first preference, `1` is the second preference, etc.).
  - The function should return `true` if the rank was successfully recorded, and `false` otherwise (if, for instance, name is not the name of one of the candidates).
  - You may assume that no two candidates will have the same name.

- Create a two-dimensional array `preferences`. The integer `preferences[i][j]` will represent the number of voters who prefer candidate `i` over candidate `j`.

- The function `record_preference` is called once for each voter, and takes as argument the `ranks` array, (recall that `ranks[i]` is the voter’s `i`th preference, where `ranks[0]` is the first preference).
  - The function should update the `preferences` array to add the current voter’s preferences.
  - You may assume that every voter will rank each of the candidates.

- An array of `pairs` will represent all of the pairs of candidates (for which one is preferred over the other) in the election.
- Declare `pair_count` variable, representing the number of pairs in the arrays `pairs`.

- Once all of the votes are in, the pairs of candidates are added to the `pairs` array via a call to `add_pairs`.
  - The function should add all pairs of candidates where one candidate is preferred to the `pairs` array. A pair of candidates who are tied (one is not preferred over the other) should not be added to the array.
  - The function should update the variable `pair_count` to be the number of pairs of candidates. (The pairs should thus all be stored between `pairs[0]` and `pairs[pair_count - 1]`, inclusive).

- Define a `struct` called `pair`. This structure will be used to represent a pair of candidates: each pair includes the `winner`’s candidate index and the `loser`’s candidate index.

- The `sort_pairs` function should sort the `pairs` array in decreasing order of strength of victory, where strength of victory is defined to be the number of voters who prefer the preferred candidate. If multiple pairs have the same strength of victory, you may assume that the order does not matter.

- Define another two-dimensional array, called `locked`, which will represent the candidate graph. `locked` is a boolean array, so `locked[i][j]` being `true` represents the existence of an edge pointing from candidate `i` to candidate `j`; `false` means there is no edge. (If curious, this representation of a graph is known as an “adjacency matrix”).

- Loop through the `locked` graph and initially set all of the values to `false`, which means our initial graph will have no edges in it.

- The `lock_pairs` function should create the locked graph, adding all edges in decreasing order of victory strength so long as the edge would not create a cycle.

Finally, the `print_winner` function should print out the name of the candidate who is the source of the graph. You may assume there will not be more than one source.

Your program should behave per the examples below.

``` bash
./tideman Alice Bob Charlie
Number of voters: 5
Rank 1: Alice
Rank 2: Charlie
Rank 3: Bob

Rank 1: Alice
Rank 2: Charlie
Rank 3: Bob

Rank 1: Bob
Rank 2: Charlie
Rank 3: Alice

Rank 1: Bob
Rank 2: Charlie
Rank 3: Alice

Rank 1: Charlie
Rank 2: Alice
Rank 3: Bob

Charlie
```
