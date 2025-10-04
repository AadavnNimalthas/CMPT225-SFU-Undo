/*
 * Aadavn Nimalthas
 * 10/03/25
 * A Class for String List
 */

#pragma once

#include <string>
using std::string;

class StringList
{
public:
	// Default constructor - makes an empty list of capacity 10
	StringList();
	
	// Copy constructor
	StringList(const StringList& other);

	// ***UNDOABLE
	// Overloaded assignment operator
	StringList& operator=(const StringList& other);

	// Destructor
	~StringList();

	// ACCESSORS
	// POST: Returns the number of elements in the list
	int size() const;

	// POST: Returns true if the list is empty, false otherwise
	bool empty() const;

	// Returns the string at the given index
	// PRE: 0 <= i < size()
	// PARAM: i = index to access
	// POST: Returns the element at index i
	string get(int i) const;

	// Returns the index of the first occurrence of the given string
	// PARAM: str = string to search for
	// POST: Returns the index of the first occurrence of s, or -1 if not found
	int index(string str) const;

	// Returns true iff the given string is in the list
	// PARAM: str = string to search for
	// POST: Returns true iff s is in the list
	bool contains(string str) const;

	// Returns true if the two lists are equal, false otherwise.
	// Does *not* consider any undo information when comparing two Stringlists. All
	// that matters is that they have the same strings in the same order.
	// PARAM: sl = list to compare with calling object
	// POST: Returns true iff calling object and sl are equal
	bool operator==(const StringList& sl) const;

	// Returns true if the two lists are not equal, false otherwise
	// Does *not* consider any undo information when comparing two Stringlists.
	// PARAM: sl = list to compare with calling object
	// POST: Returns true iff calling object and sl are *not* equal
	bool operator!=(const StringList& sl) const;

	// POST: Returns a string representation of the list
	string toString() const;

	// MUTATORS
	 
	// ***UNDOABLE
	// Sets the value at the given index
	// PRE: 0 <= i < size()
	// PARAM: i = index to set value at
	//		  str = value to set
	// POST: Sets list[i] to str
	void set(int i, string str);

	// ***UNDOABLE
	// Inserts the given string *before* the string at the given index
	// PRE: 0 <= pos < size()
	// PARAM: pos = index of insertion point
	//		  str = value of inserted string
	// POST: Sets list[pos] to str and moves all other elements up
	void insertBefore(int pos, string str);

	// ***UNDOABLE
	// Inserts the given string at the front of the list
	// PARAM: str = value of inserted string
	// POST: Sets list[0] to str and moves all other elements up
	void insertFront(string str);

	// ***UNDOABLE
	// Inserts the given string at the back of the list
	// PARAM: str = value of inserted string
	// POST: Sets list[size()] to str
	void insertBack(string str);
	
	// ***UNDOABLE
	// Removes the element at the given index
	// PRE: 0 <= pos < size()
	// PARAM:pos = index of element to remove
	// POST: Removes list[pos] and moves all other elements down
	void remove(int pos);

	// ***UNDOABLE
	// Empties the list
	// POST: Removes all strings from the list without changing capacity
	void removeAll();

	// Undoes the last operation that modified the list
	// POST: Reverts the list to its state before the last operation
	void undo();

	// Prints the list
	// POST: Prints the list to the console
	void print() const;

private:
	int n;
	int capacity;
	string* arr;

	// Represents the inverse of a single modifying operation on the list
    // POST: Stores enough information to revert one change (insert, remove, set, or whole-list replace)
    struct Operation {
        // The type of inverse operation using enums
        enum Type { REMOVE_AT, INSERT_AT, SET_AT, SET_LIST } type;

        int index = 0;          // Index where the operation applies (for insert/remove/set)
        string value;           // Stored string used for insert/remove/set inverses
        string* snapshot = nullptr; // Deep copy of the entire list (for SET_LIST)
        int snapLen = 0;        // Length of snapshot (for SET_LIST)
    };

    // Stack implementation for storing operations (array-based, doubles capacity when full)
    // POST: Provides push/pop access to Operation elements, used to support undo()
    class UndoStack {
    public:
        // Default constructor
        // POST: Creates an empty stack with capacity 4
        UndoStack() : size_(0), cap_(4) { data_ = new Operation[cap_]; }

        // Destructor
        // POST: Frees all memory held by the stack, including snapshots
        ~UndoStack() { clear(); delete[] data_; }

        // POST: Returns true if stack is empty, false otherwise
        bool empty() const { return size_ == 0; }

        // PARAM: op = Operation to push
        // POST: Pushes op onto the top of the stack, doubling capacity if needed
        void push(const Operation& op) {
            if (size_ == cap_) grow();
            data_[size_++] = op;
        }

        // PARAM: out = Operation reference to receive top of stack
        // POST: Pops the most recent Operation into out (no-op if empty)
        void pop(Operation& out) {
            if (size_ == 0) return;
            out = data_[--size_];
        }

        // POST: Clears the stack and frees any snapshots owned by its Operations
        void clear() {
            for (int i = 0; i < size_; i++) {
                if (data_[i].type == Operation::SET_LIST && data_[i].snapshot != nullptr) {
                    delete[] data_[i].snapshot;
                    data_[i].snapshot = nullptr;
                    data_[i].snapLen = 0;
                }
            }
            size_ = 0;
        }

    private:
        // POST: Doubles the internal capacity of the stack
        void grow() {
            cap_ *= 2;
            Operation* tmp = new Operation[cap_];
            for (int i = 0; i < size_; i++) tmp[i] = data_[i];
            delete[] data_;
            data_ = tmp;
        }

        Operation* data_; // Dynamic array of Operation elements
        int size_;        // Number of Operations currently stored
        int cap_;         // Current capacity of the stack
    };

    // Undo stack that stores inverse operations
    // POST: Used by undo() to restore list to previous state
    UndoStack undo_;

    // Flag to suppress recording while undo is applying changes
    // POST: Prevents undo() itself from generating new undo records
    bool recordingEnabled_ = true;

	// Helper function for checking index bounds
	// PARAM: i = index to check
	// POST: Throws an out_of_range exception if i is out of bounds
	void checkBounds(int i, string s) const;

	// Helper function to check capacity and double list size if necessary
	// POST: Doubles the capacity of the list if n == capacity
	void checkCapacity();

	// Helper function to copy the contents of another list
	// PARAM: lst = list to copy
	// POST: Makes a deep copy of lst
	void copyList(const StringList& lst);

};

