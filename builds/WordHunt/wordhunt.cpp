// Wordhunt main code

// [SECTION] STRUCT CONSTRUCTORS
//           - [STRUCT] LABEL
// [SECTION] WORDHUNT FUNCTIONS

#include "wordhunt.h"
#include <random>

//-------------------------------------------------------------------------
// [SECTION] STRUCT CONSTRUCTOR
//-------------------------------------------------------------------------

// [STRUCT] LetterNode
LetterNode::LetterNode(bool _end_of_word, char _letter)
{
    end_of_word = _end_of_word;
    letter = _letter;
    hasChildren = false;
    children = nullptr;
    isChild = nullptr;
}

// [STRUCT] Dictionary
void Dictionary::AddWord(char* word, size_t word_length)
{
    LetterNode* current = head;
    for (int index = 0; index < word_length; index++)
    {
        if (islower(word[index]))
        {
            word[index] = (char)toupper(word[index]);
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

bool Dictionary::IsWord(const char* word, size_t word_length)
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

// [STRUCT] Solution
Solution::Solution(Tile* _head)
{
    head = _head;
    Tile* temp = head;
    word = new char[16];
    length = 0;
    while (temp)
    {
        word[length++] = temp->val;
        temp = temp->next;
    }
    word[length] = 0;
}

char* Solution::to_string()
{
    char out[16];
    for (int i = 0; i < length; i++)
    {
        out[i] = word[i];
    }
    out[length] = 0;
    return _strdup(out);
}

// [STRUCT] Seed
Seed::Seed(unsigned int _seed_value)
{
    seed_value = _seed_value;
    rows = 4;
    cols = 4;
    time_seconds = 75;
    memset(board, 0, sizeof(board));
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            board[row][col] = 1;
        }
    }
}

Seed::Seed(char* _complete_seed)
{
    memset(board, 0, sizeof(board));

    int index = 0;
    if (_complete_seed[index] == 'R')
    {
        rows = 0; // Get rows
        index++; // skip 'R'
        while (isdigit(_complete_seed[index]))
        {
            rows *= 10;
            rows += _complete_seed[index++] - '0';
        }

        cols = 0; // Get cols
        index++; // skip 'C'
        while (isdigit(_complete_seed[index]))
        {
            cols *= 10;
            cols += _complete_seed[index++] - '0';
        }

        // Get board layout
        index++; // skip '>'
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                board[r][c] = _complete_seed[index++] - '0';
            }
        }

        seed_value = 0; // Get seed_value
        index++; // skip '['
        while (isdigit(_complete_seed[index]))
        {
            seed_value *= 10;
            seed_value += _complete_seed[index++] - '0';
        }
        index++; // skip ']'
        index++; // skip 't'
        time_seconds = 0;
        while (isdigit(_complete_seed[index]))
        {
            time_seconds *= 10;
            time_seconds += _complete_seed[index++] - '0';
        }
    }
    else
    {
        index++;
        seed_value = 0;
        while (isdigit(_complete_seed[index]))
        {
            seed_value *= 10;
            seed_value += _complete_seed[index++] - '0';
        }
        rows = 4;
        cols = 4;
        index++;
        time_seconds = 75;

        for (int row = 0; row < rows; row++)
        {
            for (int col = 0; col < cols; col++)
            {
                board[row][col] = 1;
            }
        }
    }
}

char* Seed::to_string()
{
    std::string output = "";

    output.push_back('R');
    output.push_back(char(rows + '0'));

    output.push_back('C');
    output.push_back(char(cols + '0'));

    output.push_back('>');
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            output.push_back(char(board[r][c] + '0'));
        }
    }

    output.push_back('[');

    char temp[10] = { 0 };
    int start = 9;
    unsigned int temp_seed_value = seed_value;
    while (temp_seed_value && start >= 0)
    {
        temp[start] = "0123456789"[temp_seed_value % 10];
        start--;
        temp_seed_value /= 10;
    }
    while (start < 9)
    {
        output.push_back(temp[++start]);
    }

    output.push_back(']');
    output.push_back('t');
    int temp_time_seconds = time_seconds;
    start = 9;
    while (temp_time_seconds && start >= 0)
    {
        temp[start] = "0123456789"[temp_time_seconds % 10];
        start--;
        temp_time_seconds /= 10;
    }
    while (start < 9)
    {
        output.push_back(temp[++start]);
    }


    static char out[256] = { 0 };
    for (int i = 0; i < output.size(); i++)
    {
        out[i] = output[i];
    }
    out[output.size()] = 0;
    return out;
}

// [STRUCT] Solver
Solver::Solver(char* letters, Seed* seed)
{
    for (int row = 0; row < seed->rows; row++)
    {
        grid.push_back(std::vector<char>());
        for (int col = 0; col < seed->cols; col++)
        {
            grid[row].push_back(letters[row * seed->rows + col]);
        }
    }
    visited = std::vector<std::vector<bool>>(seed->rows, std::vector<bool>(seed->cols, false));
};

//-------------------------------------------------------------------------
// [SECTION] WORDHUNT CONSTRUCTORS
//-------------------------------------------------------------------------


static Dictionary*              default_dictionary;
static std::vector<Dictionary*> dictionaries;
static int                      min_word_length;
static int                      max_points;
static const int                point_values[] = { 0,0,0,100,400,800,1200,1600,2000,2400,2800 };
static std::set<Solution*>      solution_list;
static Seed*                    seed;

// Variable Getters and Setters
int WordHunt::GetMinWordLength()
{
    return min_word_length;
}

void WordHunt::SetMinWordLength(const int _word_len)
{
    min_word_length = _word_len;
}

int WordHunt::GetMaxPoints()
{
    return max_points;
}

void WordHunt::SetMaxPoints(const int _max_points)
{
    max_points = _max_points;
}

int WordHunt::GetPointVal(const int word_length)
{
    return point_values[word_length];
}

void WordHunt::SetDictAsDefault(Dictionary* dict)
{
    default_dictionary = dict;
}

Dictionary* WordHunt::GetDefaultDictionary()
{
    return default_dictionary;
}

std::vector<Dictionary*> WordHunt::GetDictionaries()
{
    return dictionaries;
}

void WordHunt::SetCurrentSeed(Seed* _seed)
{
    seed = _seed;
}

Seed* WordHunt::GetCurrentSeed()
{
    return seed;
}

// Setup function
void WordHunt::GenerateGame(char* s, const size_t len, int seed)
{
    std::mt19937 rng(seed);
    const static char* characterSet = "EEEEEEEEEEEEAAAAAAAAAIIIIIIIIIOOOOOOOONNNNNNRRRRRRTTTTTTLLLLSSSSUUUUDDDDGGGBBCCMMPPFFHHVVWWYYKJXQZ";
    for (size_t i = 0; i < len * 2; i += 2) {
        s[i] = characterSet[std::uniform_int_distribution<int>(0, 97)(rng)];
        s[i + 1] = 0;
    }
    std::cout << "\n";
    s[len * 2] = 0;
}

Dictionary* WordHunt::AddDictionary(const char* fileName)
{
    std::ifstream fin(fileName);

    LetterNode* head = new LetterNode(true, '_');
    Dictionary* new_dictionary = new Dictionary(head);

    char buffer[32];
    size_t word_size = 0;

    while (fin >> buffer)
    {
        while (buffer[word_size] && buffer[word_size] != '\n')
        {
            word_size++;
        }
        new_dictionary->AddWord(buffer, word_size);
        word_size = 0;
    }
    dictionaries.push_back(new_dictionary);
    return new_dictionary;
}

int WordHunt::IsValidSeed(char* potential_seed)
{
    int index = 0;
    if (potential_seed[index] == 'R')
    {
        // Check rows
        int temp_rows = 0;
        index++;
        while (isdigit(potential_seed[index]) && temp_rows < 20)
        {
            temp_rows *= 10;
            temp_rows += potential_seed[index++] - '0';
        }

        if (temp_rows > 15 || temp_rows == 0)
        {
            return -1;
        }

        // Check rows
        int temp_cols = 0;
        if (potential_seed[index++] != 'C')
        {
            return -1;
        }
        while (isdigit(potential_seed[index]) && temp_cols < 20)
        {
            temp_cols *= 10;
            temp_cols += potential_seed[index++] - '0';
        }

        if (temp_cols > 15 || temp_cols == 0)
        {
            return -1;
        }

        if (potential_seed[index++] != '>')
        {
            return -1;
        }
        for (int r = 0; r < temp_rows; r++)
        {
            for (int c = 0; c < temp_cols; c++)
            {
                if (potential_seed[index] != '0' && potential_seed[index++] != '1')
                {
                    return -1;
                }
            }
        }

        if (potential_seed[index++] != '[')
        {
            return -1;
        }
        long long temp_seed = 0;
        while (isdigit(potential_seed[index]) && temp_seed < INT_MAX)
        {
            temp_seed *= 10;
            temp_seed += potential_seed[index++] - '0';
        }
        if (potential_seed[index++] != ']')
        {
            return -1;
        }
        if (potential_seed[index++] != 't')
        {
            return -1;
        }
        int counter = 0;
        while (potential_seed[index++] != 0)
        {
            counter++;
        }
        return (counter && counter < 5) ? 1 : -1;
    }
    else if (potential_seed[index] == '[')
    {
        index++;
        long long hold = 0;
        while (isdigit(potential_seed[index]) && hold < INT_MAX)
        {
            hold *= 10;
            hold += potential_seed[index] - '0';
        }
        if (isdigit(potential_seed[index]))
        {
            return -1;
        }
        else {
            return 0;
        }
    }
    else
    {
        return -1;
    }
}

void WordHunt::Setup(char* file_name)
{
    SetMinWordLength(3);
    SetMaxPoints(0);
    SetDictAsDefault(AddDictionary(file_name));
}

/*
static Dictionary*              default_dictionary;
static std::vector<Dictionary*> dictionaries;
static int                      min_word_length;
static int                      max_points;
static const int                point_values[] = { 0,0,0,100,400,800,1200,1600,2000,2400,2800 };
static std::set<Solution*>      solution_list;
static Seed*                    seed;
*/

void WordHunt::SolveCurrentSeed(char* letters)
{
    Solver* solve = new Solver(letters, WordHunt::GetCurrentSeed());
}