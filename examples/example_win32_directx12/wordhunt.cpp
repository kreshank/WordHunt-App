#include "wordhunt.h"

static Dictionary* default_dictionary;

Dictionary* WordHunt::CreateDictionary(const char* fileName)
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
    return new_dictionary;
}

bool WordHunt::SetAsDefault(Dictionary* dict)
{
    return default_dictionary = dict;
}

Dictionary* WordHunt::GetDefaultDictionary()
{
    return default_dictionary;
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
            temp_rows += potential_seed[index++];
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
            temp_cols += potential_seed[index++];
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
            temp_seed += potential_seed[index++];
        }
        return (potential_seed[index] == ']') ? 1 : -1;
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
