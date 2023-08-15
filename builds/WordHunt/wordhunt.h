#pragma once

#include <fstream>
#include <queue>
#include <set>
#include <iostream>

struct LetterNode;
struct Dictionary;
struct Solution;
struct Tile;
struct Seed;
struct SolutionPointerComparator;
struct Solver;

enum WordHuntGamePhase_
{
    WordHuntGamePhase_Selection     = 0,
    WordHuntGamePhase_Generate      = 1,
    WordHuntGamePhase_Play          = 2,
    WordHuntGamePhase_Result        = 3,
};

namespace WordHunt
{
    // Variable Getters and Setters
    int                             GetMinWordLength();
    void                            SetMinWordLength(const int _word_len = 3);
    int                             GetMaxPoints();
    void                            SetMaxPoints(const int _max_points = 0);
    int                             GetPointVal(const int word_length = 0);
    void                            SetDictAsDefault(Dictionary* dict);
    Dictionary*                     GetDefaultDictionary();
    std::vector<Dictionary*>        GetDictionaries();
    void                            SetCurrentSeed(Seed* _seed);
    Seed*                           GetCurrentSeed();

    // Setup Functions
    void                            GenerateGame(char* s, const size_t len, int seed);
    Dictionary*                     AddDictionary(const char* fileName);
    int                             IsValidSeed(char* potential_seed);
    void                            Setup(char* file_name);
    Solver*                         SolveCurrentSeed(char* letters);

    // Internals
    void                            SolveStartHere(Solver* solver, Seed* seed, const int row_i, const int col_i, Tile* head, Tile* last, LetterNode* dictionary_pos, const int depth);
}

struct LetterNode
{
    bool            end_of_word;
    char            letter;
    bool            hasChildren;
    bool*           isChild;
    LetterNode**    children;

    LetterNode(bool _end_of_word, char _letter);
};

struct Dictionary
{
    LetterNode*     head;


    Dictionary()                    { head = new LetterNode(true, '_'); }
    Dictionary(LetterNode* _head)   { head = _head; }
    void AddWord(char* word, size_t word_length);
    bool IsWord(const char* word, size_t word_length);
    bool IsWord(std::string word) { return IsWord(word.data(), word.size()); }
};

struct Tile
{
    int     x;
    int     y;
    char    val;
    Tile*   next;

    Tile(int _x, int _y, char _val)
    {
        x = _x;
        y = _y;
        val = _val;
        next = NULL;
    }
    Tile(int _x, int _y, char _val, Tile* _next)
    {
        x = _x;
        y = _y;
        val = _val;
        next = _next;
    }
};

struct Solution
{
    int     length;
    char*   word;
    Tile*   head;

    Solution(Tile* _head);
    int GetPointValue() { return WordHunt::GetPointVal(length); }
    char* to_string();
};

struct SolutionPointerComparator
{
    bool operator()(const Solution* a, const Solution* b) const
    {
        if (a->length != b->length) return a->length > b->length;
        return strcmp(a->word, b->word) < 0;
    }
};

struct Seed
{
    int             rows;
    int             cols;
    unsigned int    seed_value;
    bool            board[15][15];
    int             time_seconds;

    // Valid Formats:
    //      R##C##>1111111111111111[numericalseed]t##
    //      ^row^col^active squares ^actual srand seed
    //
    //      [numericalseed]
    //      default row = col = 4, time_seconds = 75
    Seed(unsigned int _seed_value);
    Seed(char* _complete_seed);
    char* to_string();
};

struct Solver
{
    std::vector<std::vector<char>> grid;
    std::vector<std::vector<bool>> visited;
    std::set<Solution*, SolutionPointerComparator> sol_list;
    const int dr[3] = { -1, 0 , 1 };
    const int dc[3] = { -1, 0 , 1 };

    Solver(char* letters, Seed* seed);
};
