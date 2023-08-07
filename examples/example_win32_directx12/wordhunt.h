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
struct Board;

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
    inline int          GetMinWordLength();
    inline void         SetMinWordLength(const int _word_len = 3);
    inline int          GetMaxPoints();
    inline void         SetMaxPoints(const int _max_points = 0);
    inline static int   GetPointVal(const int word_length);
    inline Dictionary* GetDefaultDictionary();
    inline Board        GetCurrentBoard();

    const static int        PointValues[] = { 0,0,0,100,400,800,1200,1600,2000,2400,2800 };
    std::set<Solution*>     solution_list;
    Board                   board;

    // Setup Functions
    Dictionary*         CreateDictionary(const char* fileName);
    inline void         SetDictAsDefault(Dictionary* dict);
    inline Dictionary*  GetDefaultDictionary();
    inline Board        GetCurrentBoard();
    void                Setup();
    void                LoadBoard();
    void                SetupSolver();
    int                 IsValidSeed(char* potential_seed);
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
    int GetPointValue() { return WordHunt::PointValues[length]; }
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

    // Valid Formats:
    //      R##C##>1111111111111111[numericalseed]
    //      ^row^col^active squares ^actual srand seed
    //
    //      [numericalseed]
    //      default row = col = 4,
    Seed(unsigned int _seed_value);
    Seed(char* _complete_seed);
    char* to_string();
};

struct Board
{
    int numRows;
    int numCows;
    Seed seed;

    Board();
    Board(Seed _seed);
};
