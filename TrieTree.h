#pragma once

// System Includes
#include <map>
#include <fstream>
#include <string>
#include <vector>

// Project Includes
#include "Utility.h"

/* Interface for a node to use in a Trie Tree structure */
class TrieNodeInterface {
    /* Returns next node structure for the given character */
    virtual TrieNodeInterface* GetNextNode(char InChar) const = 0;

    /* return true if this node represents a completed string */
    virtual bool IsCompletedString() const = 0;

    /** Sets the next node in the structure for the given character */
    virtual void SetNextChar(char InChar, TrieNodeInterface* NextNode) = 0;

    /* Helper function returns true only if it is a valid character for this Trie tree structure */
    virtual bool ValidChar(char InChar) const = 0;

    /* Getter for character this node represents */
    virtual char GetChar(const char CurrentChar) { return CurrentChar; }
};

/*
 * Data structure for storing strings, possibly with associated data
 * Insertion and searching are O(L) time complexity, with L being the size of the word being queried or inserted
 * Each node represents a character for a string, edges from that each node represent the next letter
 * If an edge is null, the string is not contained in the tree.
 */
template <typename NodeType, typename NodeData>
class TrieTree {
public:
    // Require NodeType to implement interface to be valid to compile
    static_assert(std::is_base_of<TrieNodeInterface, NodeType>::value, "NodeValue must implment TrieNodeInterface");

    /* Default constructor */
    TrieTree() {
        RootNode = AllocateNewNode(false);
    }

    /* Destructor */
    ~TrieTree() {
        // Delete all the nodes we allocated
        for (int i = 0; i < AllocateNodes.size(); ++i) {
            if (AllocateNodes[i]) {
                delete AllocateNodes[i];
            }
        }

        AllocateNodes.clear();
    }

    /* Attempts to load text file of the given name and add every line as a string into the tree */
    void InitTreeFromDictionaryFile(const std::string& DictionaryFileName, const NodeData& DefaultValue) {
        std::ifstream DictionaryFile(DictionaryFileName);

        if (AssertValue(DictionaryFile.is_open())) {
            std::string Line;

            while (getline(DictionaryFile, Line)) {
                AddString(Line, DefaultValue);
            }

            DictionaryFile.close();
        }
    }

    /* Adds the given string to trie tree letter by letter. Sets the final node with the given data */
    void AddString(const std::string& InString, const NodeData& Data) {
        if (!AssertValue(RootNode != nullptr)) {
            return;
        }

        NodeType* LastNode = RootNode;

        for (int i = 0; i < InString.size(); ++i) {
            const char Char = InString[i];

            if (AssertValue(LastNode->ValidChar(Char))) {
                if (!LastNode->GetNextNode(Char)) {
                    NodeType* NextNode = AllocateNewNode(false);

                    LastNode->SetNextChar(Char, NextNode);
                }

                LastNode = AssertCast<NodeType, TrieNodeInterface>(LastNode->GetNextNode(Char));
            }
        }

        if (AssertValue(LastNode)) {
            LastNode->SetNodeData(Data);
        }
    }

    /* Traverse nodes and returns the final node representing the given string, or null if none found
     * Does not need to be a complete string to return a valid node
     */
    const NodeType* GetNode(const std::string& InString) const {
        if (!AssertValue(RootNode != nullptr)) {
            return nullptr;
        }

        NodeType* CurrentNode = RootNode;

        // Traverse the nodes letter by letter in the string and return the result or null if none found
        for (int i = 0; i < InString.size(); ++i) {
            const char CurrentChar = InString[i];

            CurrentNode = AssertCast<NodeType, TrieNodeInterface>(CurrentNode->GetNextNode(CurrentChar));
            if (!CurrentNode) {
                return nullptr;
            }
        }

        return CurrentNode;
    }

    /* Getter for root pointer  */
    const NodeType* GetRoot() const {
        return RootNode;
    }

    /* Returns true if the string can be found in the tree */
    bool ContainsString(const std::string& InString, bool RequiresComplete = true) const {
        const NodeType* FoundNode = GetNode(InString);

        return FoundNode && (!RequiresComplete || FoundNode->IsCompletedString());
    }

    /* Helper function to allocate a new node and store a pointer for later deletion */
    NodeType* AllocateNewNode(bool InCompleted) {
        NodeType* NextNode = new NodeType(InCompleted);
        AllocateNodes.push_back(NextNode);

        return NextNode;
    }

protected:
    // Root tree node
    NodeType* RootNode;

    // Every allocated node in an array to be deleted in the destructor
    std::vector<NodeType*> AllocateNodes;
};
