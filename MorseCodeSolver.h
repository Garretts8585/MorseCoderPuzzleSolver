#pragma once

// System Includes
#include <map>
#include <cassert>
#include <fstream>
#include <string>
#include <vector>

// Project Includes
#include "TrieTree.h"

/*
    @brief: Node for creating a Trie tree containing word strings
*/
class WordTrieNode : public TrieNodeInterface {

public:
    /* Constructors for WordTrieNode */
    WordTrieNode();
    WordTrieNode(bool bInCompleted);

    /* TrieNodeInterface Start */
    virtual TrieNodeInterface* GetNextNode(char InChar) const override;
    virtual bool IsCompletedString() const override;
    virtual void SetNextChar(char InChar, TrieNodeInterface* NextNode) override;
    virtual bool ValidChar(char InChar) const override;
    /* TrieNodeInterface End */

    /* Setter of Completed string */
    void SetNodeData(bool InCompeted);

protected:
    // Maps letter to next node, or none if next letter does not lead to a valid string
    std::map<char, WordTrieNode*> NextNodeMap;

    // Flag for if this is the last letter of a word
    bool CompletedString;
};

/*
    @brief: Node for creating a Trie tree to decode morse code
*/
class MorseCodeTrieNode : public TrieNodeInterface {

public:
    /* Constructors for MorseCodeTrieNode */
    MorseCodeTrieNode();
    MorseCodeTrieNode(char InLetter);

    /* TrieNodeInterface Start */
    virtual TrieNodeInterface* GetNextNode(char InChar) const override;
    virtual bool IsCompletedString() const override;
    virtual bool ValidChar(char InChar) const override;
    virtual char GetChar(const char CurrentChar) override { return Letter; }
    virtual void SetNextChar(char InChar, TrieNodeInterface* NextNode) override;
    /* TrieNodeInterface End */

    /* Setter for character of the node */
    void SetNodeData(char InDecodedChar);

    /* Cosnt getter for letter  */
    char GetLetter() const { return Letter;  }

protected:
    // Next letter if next morse code letter is a dot
    MorseCodeTrieNode* NextDot;

    // Next letter if next morse code letter is a dash
    MorseCodeTrieNode* NextDash;

    // Letter representing the morse code string traversed in the tree so far
    char Letter;
};


/*
    @brief: Data structure for decoding morsing code strings
*/
class MorseDecoder {

public:
    /* MorseDecoder default constructor */
    MorseDecoder();

    /* MorseDecoder constructor that populates DictionaryTree from a text file of valid words */
    MorseDecoder(const std::string& InDictionaryFileName);

    /* Converts the morse code word with spaces between letters to an alphabet word */
    std::string Decode(const std::string& InString);

    /* return possible words a morse code string could represent with no spaces differentiating letters */
    std::vector<std::string> MultiDecode(const std::string& InString, unsigned int MaxReturnSize);

protected:

    /* 
     * @brief Recursive function to decode a string with multiple possible answers in ComboDecode 
     * 
     * @param InString: String we're trying to find possible decodings for 
     * @param CurrentString: Current prefix of possible word from the morse code being search for
     * @param StringStart: Index we are building CurrentString from
     * @param StringEnd: Last index of the InString
     * @param CurrentNode: Current node in DictionaryTree of our CurrentString
     * @param ReturnString: List of possible solutions found so far
     * @param MaxSize: Max number of possible solutions to return. if zero or less, there will be no limit
     
     */
    void MultiDecode_Req(const std::string& InString, std::string& CurrentString, unsigned int StringStart, unsigned int StringEnd, 
        const WordTrieNode* CurrentNode, std::vector<std::string>& ReturnString, unsigned int MaxReturnSize);

    // Tree for finding alphabet letters from morse code strings
    TrieTree<MorseCodeTrieNode, char> MorseTrieTree;

    // Tree for finding valid alphabet words
    TrieTree<WordTrieNode, bool> DictionaryTree;
};
