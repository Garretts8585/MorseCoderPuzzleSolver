
// Project Includes
#include "MorseCodeSolver.h"

WordTrieNode::WordTrieNode()
    : CompletedString(false) {
}

WordTrieNode::WordTrieNode(bool InCompleted)
    : CompletedString(InCompleted) {
}

TrieNodeInterface* WordTrieNode::GetNextNode(char InChar) const {
    const char CharToUse = tolower(InChar);
    if (!AssertValue(ValidChar(CharToUse))) {
        return nullptr;
    }

    auto NextNode = NextNodeMap.find(CharToUse);

    if (NextNode != NextNodeMap.end()) {
       return NextNode->second;
    }

   return nullptr;
}

bool WordTrieNode::IsCompletedString() const {
    return CompletedString;
}

void WordTrieNode::SetNextChar(char InChar, TrieNodeInterface* NextNodeInterface) {
    WordTrieNode* NextNode = AssertCast<WordTrieNode, TrieNodeInterface>(NextNodeInterface);

    const char CharToUse = tolower(InChar);

    if (AssertValue(ValidChar(CharToUse))) {
        NextNodeMap[CharToUse] = NextNode;
    }
}

void WordTrieNode::SetNodeData(bool InComplete) {
    CompletedString = InComplete;
}

bool WordTrieNode::ValidChar(char InChar) const {
    const char ToLowerChar = tolower(InChar);

    return ToLowerChar >= 'a' && ToLowerChar <= 'z';
}


MorseCodeTrieNode::MorseCodeTrieNode() {
    NextDash = nullptr;
    NextDot = nullptr;
    Letter = 0;
}

MorseCodeTrieNode::MorseCodeTrieNode(char InLetter) {
    NextDash = nullptr;
    NextDot = nullptr;
    Letter = InLetter;
}

TrieNodeInterface* MorseCodeTrieNode::GetNextNode(char InChar) const {
    if (!AssertValue(ValidChar(InChar))) {
        return nullptr;
    }

    return InChar == '.' ? NextDot : NextDash;
}

bool MorseCodeTrieNode::IsCompletedString() const {
    return Letter != 0;
}

void MorseCodeTrieNode::SetNextChar(char InChar, TrieNodeInterface* NextNode) {
    if (!AssertValue(ValidChar(InChar))) {
        return;
    }

    MorseCodeTrieNode* MorseNode = AssertCast< MorseCodeTrieNode, TrieNodeInterface>(NextNode);

    if (InChar == '.') {
        NextDot = MorseNode;
    }
    else {
        NextDash = MorseNode;
    }
}

bool MorseCodeTrieNode::ValidChar(char InChar) const {
    return InChar == '.' || InChar == '-';
}

void MorseCodeTrieNode::SetNodeData(char InDecodedChar) {
    Letter = InDecodedChar;
}

MorseDecoder::MorseDecoder() {
    // Build the morse code tree from the hard coded conversions 
    MorseTrieTree.AddString(".-", 'a');
    MorseTrieTree.AddString("-...", 'b');
    MorseTrieTree.AddString("-.-.", 'c');
    MorseTrieTree.AddString("-..", 'd');
    MorseTrieTree.AddString(".", 'e');
    MorseTrieTree.AddString("..-.", 'f');
    MorseTrieTree.AddString("--.", 'g');
    MorseTrieTree.AddString("....", 'h');
    MorseTrieTree.AddString("..", 'i');
    MorseTrieTree.AddString(".---", 'j');
    MorseTrieTree.AddString("-.-", 'k');
    MorseTrieTree.AddString(".-..", 'l');
    MorseTrieTree.AddString("--", 'm');
    MorseTrieTree.AddString("-.", 'n');
    MorseTrieTree.AddString("---", 'o');
    MorseTrieTree.AddString(".--.", 'p');
    MorseTrieTree.AddString("--.-", 'q');
    MorseTrieTree.AddString(".-.", 'r');
    MorseTrieTree.AddString("...", 's');
    MorseTrieTree.AddString("-", 't');
    MorseTrieTree.AddString("..-", 'u');
    MorseTrieTree.AddString("...-", 'v');
    MorseTrieTree.AddString(".--", 'w');
    MorseTrieTree.AddString("-..-", 'x');
    MorseTrieTree.AddString("-.--", 'y');
    MorseTrieTree.AddString("--..", 'z');
}

MorseDecoder::MorseDecoder(const std::string& InDictionaryFileName)
    : MorseDecoder() {
    DictionaryTree.InitTreeFromDictionaryFile(InDictionaryFileName, true);
}

std::string MorseDecoder::Decode(const std::string& InString) {

    // Convert the given string to a list of space seperated morse code chunks
    std::vector<std::string> ParsedString;

    std::string LetterToken;
    for (int i = 0; i < InString.size(); ++i) {
        if (InString[i] == ' ') {
            if (LetterToken.size() > 0) {
                ParsedString.push_back(LetterToken);
            }
            
            LetterToken.clear();
        }
        else if (InString[i] == '.' || InString[i] == '-') {
            LetterToken.push_back(InString[i]);
        }
        else {
            // Skip any other characters
        }
    }

    if (LetterToken.size() > 0) {
        ParsedString.push_back(LetterToken);
    }

    std::string ReturnString;

    // Decode the morse code chunks
    for (int i = 0; i < ParsedString.size(); ++i) {
        const MorseCodeTrieNode* FoundNode = MorseTrieTree.GetNode(ParsedString[i]);

        if (!FoundNode) {
            return "";
        }
        
        ReturnString.push_back(FoundNode->GetLetter());
    }

    return ReturnString;
}

std::vector<std::string> MorseDecoder::MultiDecode(const std::string& InString, unsigned int MaxReturnSize) {
    std::vector<std::string> ReturnString;

    std::string CurrentString;

    MultiDecode_Req(InString, CurrentString, 0, static_cast<unsigned int>(InString.length() - 1), DictionaryTree.GetRoot(), ReturnString, MaxReturnSize);

    return ReturnString;
}

void MorseDecoder::MultiDecode_Req(const std::string& InString, std::string& CurrentString, unsigned int StringStart, unsigned int StringEnd, 
        const WordTrieNode* CurrentNode, std::vector<std::string>& ReturnStrings, unsigned int MaxReturnSize) {

    // Early out if at the end of our tree
    if (CurrentNode == nullptr) {
        return;
    }

    // Early out if max size already reached
    if (ReturnStrings.size() >= MaxReturnSize && MaxReturnSize > 0) {
        return;
    }

    unsigned int CurrentIndex = StringStart;
    const MorseCodeTrieNode* CurrentMCNode = MorseTrieTree.GetRoot();

    while (CurrentMCNode && CurrentIndex <= StringEnd && CurrentString.size() < MaxReturnSize)
    {
        // Traverse word tree reqursively trying each morse code letter possible from our current index
        CurrentMCNode = static_cast<const MorseCodeTrieNode*>(CurrentMCNode->GetNextNode(InString[CurrentIndex]));

        if (CurrentMCNode && CurrentMCNode->GetLetter() != 0) {
            const WordTrieNode* NextWordNode = static_cast<const WordTrieNode*>(CurrentNode->GetNextNode(CurrentMCNode->GetLetter()));

            // A null word indictates no valid words can be formed using this particular word prefix. No further reqursion nessessary 
            if (NextWordNode) {
                CurrentString += CurrentMCNode->GetLetter();

                if (CurrentIndex == StringEnd) {
                    if (NextWordNode->IsCompletedString()) {
                        ReturnStrings.push_back(CurrentString);
                    }
                }
                else {
                    MultiDecode_Req(InString, CurrentString, CurrentIndex+1, StringEnd, NextWordNode, ReturnStrings, MaxReturnSize);
                }

                CurrentString.pop_back();
            }
        }
        else {
            return;
        }

        ++CurrentIndex;
    }
}