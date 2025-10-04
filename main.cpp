/*
 * Simple test driver for StringList undo()
 * NOTE: This file is for local testing only. Do NOT submit it.
 */

#include "StringList.h"
#include <iostream>
#include <vector>
#include <string>
#include <initializer_list>
using std::cout;
using std::endl;
using std::string;
using std::vector;

// Utility: fill a list from an initializer_list
static void fill(StringList& lst, std::initializer_list<string> items) {
    for (const auto& s : items) {
        lst.insertBack(s);
    }
}

// Utility: convert StringList -> std::vector<string>
static vector<string> toVec(const StringList& lst) {
    vector<string> v;
    for (int i = 0; i < lst.size(); ++i) v.push_back(lst.get(i));
    return v;
}

// Utility: check equality vs expected vector
static bool equals(const StringList& lst, const vector<string>& exp) {
    if (lst.size() != (int)exp.size()) return false;
    for (int i = 0; i < lst.size(); ++i) {
        if (lst.get(i) != exp[i]) return false;
    }
    return true;
}

// Utility: pretty print a vector
static string vecStr(const vector<string>& v) {
    string out = "{";
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) out += ", ";
        out += "\"" + v[i] + "\"";
    }
    out += "}";
    return out;
}

// Utility: run & report a test
static void report(const string& name, bool ok, const StringList* got = nullptr, const vector<string>* exp = nullptr) {
    cout << (ok ? "[PASS] " : "[FAIL] ") << name << endl;
    if (!ok && got && exp) {
        cout << "  got: " << got->toString() << endl;
        cout << "  exp: " << vecStr(*exp) << endl;
    }
}

int main() {
    int passed = 0, total = 0;

    // --- Test 1: insertBefore undo at end ---
    {
        ++total;
        StringList lst;
        fill(lst, {"dog","cat","tree"});
        lst.insertBefore(3, "hat");                     // {"dog","cat","tree","hat"}
        lst.undo();                                     // back to {"dog","cat","tree"}
        vector<string> exp = {"dog","cat","tree"};
        bool ok = equals(lst, exp);
        report("Undo insertBefore at end", ok, &lst, &exp);
        passed += ok;
    }

    // --- Test 2: insertBefore undo in middle ---
    {
        ++total;
        StringList lst;
        fill(lst, {"dog","cat","tree"});
        lst.insertBefore(1, "shoe");                    // {"dog","shoe","cat","tree"}
        lst.undo();                                     // {"dog","cat","tree"}
        vector<string> exp = {"dog","cat","tree"};
        bool ok = equals(lst, exp);
        report("Undo insertBefore in middle", ok, &lst, &exp);
        passed += ok;
    }

    // --- Test 3: set undo ---
    {
        ++total;
        StringList lst;
        fill(lst, {"yellow","green","red","orange"});
        lst.set(2, "cow");                              // {"yellow","green","cow","orange"}
        lst.undo();                                     // {"yellow","green","red","orange"}
        vector<string> exp = {"yellow","green","red","orange"};
        bool ok = equals(lst, exp);
        report("Undo set at index", ok, &lst, &exp);
        passed += ok;
    }

    // --- Test 4: remove undo (middle element) ---
    {
        ++total;
        StringList lst;
        fill(lst, {"dog","cat","tree"});
        lst.remove(1);                                  // {"dog","tree"}
        lst.undo();                                     // {"dog","cat","tree"}
        vector<string> exp = {"dog","cat","tree"};
        bool ok = equals(lst, exp);
        report("Undo remove at index", ok, &lst, &exp);
        passed += ok;
    }

    // --- Test 5: removeAll undo ---
    {
        ++total;
        StringList lst;
        fill(lst, {"a","b","c","d"});
        lst.removeAll();                                // {}
        lst.undo();                                     // {"a","b","c","d"}
        vector<string> exp = {"a","b","c","d"};
        bool ok = equals(lst, exp);
        report("Undo removeAll", ok, &lst, &exp);
        passed += ok;
    }

    // --- Test 6: operator= undo (full restore) ---
    {
        ++total;
        StringList lst1, lst2;
        fill(lst1, {"dog","cat","tree"});
        fill(lst2, {"yellow","green","red","orange"});
        lst1 = lst2;                                    // lst1 -> {"yellow","green","red","orange"}
        lst1.undo();                                    // restore lst1 -> {"dog","cat","tree"}
        vector<string> exp = {"dog","cat","tree"};
        bool ok = equals(lst1, exp) && equals(lst2, {"yellow","green","red","orange"});
        report("Undo operator= (restore whole list)", ok, &lst1, &exp);
        passed += ok;
    }

    // --- Test 7: Multiple operations LIFO ---
    {
        ++total;
        StringList lst;
        fill(lst, {"x","y"});
        lst.insertBack("z");                            // {"x","y","z"}
        lst.set(1, "Y");                                // {"x","Y","z"}
        lst.insertFront("w");                           // {"w","x","Y","z"}
        lst.remove(2);                                  // {"w","x","z"}
        // Now undo 4 times to get back to {"x","y"}
        lst.undo();                                     // undo remove -> {"w","x","Y","z"}
        lst.undo();                                     // undo insertFront -> {"x","Y","z"}
        lst.undo();                                     // undo set -> {"x","y","z"}
        lst.undo();                                     // undo insertBack -> {"x","y"}
        vector<string> exp = {"x","y"};
        bool ok = equals(lst, exp);
        report("LIFO sequence across mixed ops", ok, &lst, &exp);
        passed += ok;
    }

    // --- Test 8: undo on empty stack (no-op) ---
    {
        ++total;
        StringList lst;                      // empty list, no history recorded
        lst.undo();                          // should do nothing, not crash
        vector<string> exp = {};             // still empty
        bool ok = equals(lst, exp);
        report("Undo on empty stack (no-op)", ok, &lst, &exp);
        passed += ok;
    }

    // --- Test 9: copy constructor should NOT copy undo stack ---
    {
        ++total;
        StringList lst;
        fill(lst, {"a","b","c"});
        // Do a couple of undoable operations
        lst.insertBack("d");                            // {"a","b","c","d"}
        lst.set(1, "B");                                // {"a","B","c","d"}

        // Copy now — per spec, undo stack must not be copied
        StringList copy(lst);

        // Undo on the copy should do nothing (its stack is empty),
        // while original still has its history
        vector<string> beforeCopyUndo = toVec(copy);
        copy.undo(); // should be a no-op
        bool copyOk = equals(copy, beforeCopyUndo);

        // The original should still be able to undo twice
        lst.undo(); // undo set -> {"a","b","c","d"}
        lst.undo(); // undo insertBack -> {"a","b","c"}

        bool originalOk = equals(lst, vector<string>{"a","b","c"});
        bool ok = copyOk && originalOk;
        report("Copy ctor does not copy undo stack", ok);
        passed += ok;
    }

    cout << "\nSummary: " << passed << " / " << total << " tests passed.\n";
    return (passed == total) ? 0 : 1;
}
