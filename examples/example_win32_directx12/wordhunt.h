#pragma once

#include <fstream>
#include <queue>
#include <vector>
#include <algorithm>

struct LetterNode;
struct Dictionary;
struct Solution;
struct Tile;
struct Seed;

namespace WordHunt
{
    // Setup
    Dictionary* CreateDictionary(const char* fileName);
    bool SetAsDefault(Dictionary* dict);
    Dictionary* GetDefaultDictionary();
    int IsValidSeed(char* potential_seed);

    const static int PointValues[] = { 0,0,100,400,800,1200,1600,2000,2400,2800 };
}

struct LetterNode
{
    bool end_of_word;
    char letter;
    bool hasChildren;

    bool* isChild;
    LetterNode** children;

    LetterNode(bool _end_of_word, char _letter)
    {
        end_of_word = _end_of_word;
        letter = _letter;
        hasChildren = false;
        
        children = nullptr;
        isChild = nullptr;
    }
};

struct Dictionary
{
    LetterNode* head;

    Dictionary() { head = new LetterNode(true, '_'); }
    Dictionary(LetterNode* _head) { head = _head; }

    void AddWord(char* word, size_t word_length)
    {
        LetterNode* current = head;
        for (int index = 0; index < word_length; index++)
        {
            if (islower(word[index]))
            {
                word[index] = toupper(word[index]);
            }
            if (!current->hasChildren)
            {
                current->hasChildren = true;
                current->children = new LetterNode * [26] {0};
                current->isChild = new bool[26] {0};
            }
            if (!current->isChild[word[index] - 'A'])
            {
                current->isChild[word[index] - 'A'] = true;
                current->children[word[index] - 'A'] = new LetterNode(index == word_length - 1, word[index]);
            }
            current = current->children[word[index] - 'A'];
        }
    }

    bool IsWord(const char* word, size_t word_length)
    {
        LetterNode* current = head;
        for (size_t index = 0; index < word_length; index++)
        {
            if (!isalpha(word[index]) || !current->hasChildren)
            {
                return false;
            }
            if (current->isChild[word[index] - 'A'])
            {
                current = current->children[word[index] - 'A'];
            }
            else
            {
                return false;
            }
        }
        return current->end_of_word;
    }

    bool IsWord(std::string word)
    {
        return IsWord(word.data(), word.size());
    }
};

struct Tile
{
    int x;
    int y;
    char val;
    Tile* next;

    Tile(int _x, int _y, char _val)
    {
        x = _x;
        y = _y;
        val = _val;
        next = nullptr;
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
    int length;
    //int maxRows;
    //int maxColumns;
    char* word;

    Tile* head;

    int GetPointValue()
    {
        return WordHunt::PointValues[length];
    }

    Solution(std::queue<Tile*> queue)
    {
        length = queue.size();
        head = queue.front();
        short index = 0;
        Tile* temp = head;
        word[index++] = temp->val;
        queue.pop();
        while (queue.size())
        {
            temp->next = queue.front();
            queue.pop();
            word[index++] = temp->val;
            temp = temp->next;
        }
    }

    bool operator< (const Solution& other) const
    {
        if (this->length != other.length) return this->length > other.length;
        return strcmp(this->word,other.word) < 0;
    }

    bool operator== (const Solution& other) const
    {
        return length == other.length && word == other.word;
    }
};

struct Seed
{
    int rows;
    int cols;
    unsigned int seed_value;
    bool board[15][15];

    // Valid Formats:
    //      R##C##>1111111111111111[numericalseed]
    //      ^row^col^active squares ^actual srand seed
    //
    //      [numericalseed]
    //      default row = col = 4,
    Seed(const int _seed_value = 0, const int _rows = 4, const int _cols = 4)
    {
        seed_value = _seed_value;
        rows = _rows;
        cols = _cols;
        for (int row = 0; row < rows; row++)
        {
            for (int col = 0; col < cols; col++)
            {
                board[row][col] = 1;
            }
        }
    }

    Seed(char* complete_seed)
    {
        int index = 0;
        if (complete_seed[index] == 'R')
        {
            rows = 0; // Get rows
            index++; // skip 'R'
            while (isdigit(complete_seed[index]))
            {
                rows *= 10;
                rows += complete_seed[index++];
            }

            cols = 0; // Get cols
            index++; // skip 'C'
            while (isdigit(complete_seed[index]))
            {
                cols *= 10;
                cols += complete_seed[index++];
            }

            // Get board layout
            index++; // skip '>'
            for (int r = 0; r < rows; r++)
            {
                for (int c = 0; c < cols; c++)
                {
                    board[r][c] = complete_seed[index++] - '0';
                }
            }

            seed_value = 0; // Get seed_value
            index++; // skip '['
            while (isdigit(complete_seed[index]))
            {
                seed_value *= 10;
                seed_value += complete_seed[index++];
            }
        }
        else
        {
            index++;
            seed_value = 0;
            while (isdigit(complete_seed[index]))
            {
                seed_value *= 10;
                seed_value += complete_seed[index++];
            }
            Seed(seed_value);
        }
    }

    char* to_string()
    {
        std::string output = "";

        output.push_back('R');
        output += rows;

        output.push_back('C');
        output += cols;

        output.push_back('>');
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                output.push_back(board[r][c]);
            }
        }

        output.push_back('[');
        output += seed_value;
        output.push_back(']');

        char out[256];
        memcpy(out, output.data(), output.length());
        return out;
    }
};
