
extern "C"
{
#include <m_pd.h>
#include <g_canvas.h>
#include <m_imp.h>
#include <s_stuff.h>
}

#include <utility>
#include <vector>

#include "PdLibrary.h"

struct _canvasenvironment
{
    t_symbol* ce_dir;    /* directory patch lives in */
    int ce_argc;         /* number of "$" arguments */
    t_atom* ce_argv;     /* array of "$" arguments */
    int ce_dollarzero;   /* value of "$0" */
    t_namelist* ce_path; /* search path */
};

namespace pd
{


// Iterative function to insert a key into a Trie
void Trie::insert(const std::string& key)
{
    // Names with spaces not supported yet by the suggestor
    if (std::count(key.begin(), key.end(), ' ')) return;

    // start from the root node
    Trie* curr = this;
    for (char i : key)
    {
        // create a new node if the path doesn't exist
        if (curr->character[i] == nullptr)
        {
            curr->character[i] = new Trie();
        }

        // go to the next node
        curr = curr->character[i];
    }

    // mark the current node as a leaf
    curr->isLeaf = true;
}

// Iterative function to search a key in a Trie. It returns true
// if the key is found in the Trie; otherwise, it returns false
bool Trie::search(const std::string& key)
{
    Trie* curr = this;
    for (char i : key)
    {
        // go to the next node
        curr = curr->character[i];

        // if the string is invalid (reached end of a path in the Trie)
        if (curr == nullptr)
        {
            return false;
        }
    }

    // return true if the current node is a leaf and the
    // end of the string is reached
    return curr->isLeaf;
}

// Returns true if a given node has any children
bool Trie::hasChildren()
{
    for (int i = 0; i < CHAR_SIZE; i++)
    {
        if (character[i])
        {
            return true;  // child found
        }
    }

    return false;
}

// Recursive function to delete a key in the Trie
bool Trie::deletion(Trie*& curr, std::string key)
{
    // return if Trie is empty
    if (curr == nullptr)
    {
        return false;
    }

    // if the end of the key is not reached
    if (key.length())
    {
        // recur for the node corresponding to the next character in the key
        // and if it returns true, delete the current node (if it is non-leaf)

        if (curr != nullptr && curr->character[key[0]] != nullptr && deletion(curr->character[key[0]], key.substr(1)) && !curr->isLeaf)
        {
            if (!curr->hasChildren())
            {
                delete curr;
                curr = nullptr;
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    // if the end of the key is reached
    if (key.length() == 0 && curr->isLeaf)
    {
        // if the current node is a leaf node and doesn't have any children
        if (!curr->hasChildren())
        {
            // delete the current node
            delete curr;
            curr = nullptr;

            // delete the non-leaf parent nodes
            return true;
        }

        // if the current node is a leaf node and has children
        else
        {
            // mark the current node as a non-leaf node (DON'T DELETE IT)
            curr->isLeaf = false;

            // don't delete its parent nodes
            return false;
        }
    }

    return false;
}

void Trie::suggestionsRec(std::string currPrefix, Suggestions& result)
{
    // found aString in Trie with the given prefix
    if (isLeaf)
    {
        result.push_back({currPrefix, true});
    }

    // All children struct node pointers are NULL
    if (!hasChildren()) return;

    for (int i = 0; i < CHAR_SIZE; i++)
    {
        if (character[i])
        {
            // append current character to currPrefixString
            currPrefix.push_back(i);

            // recur over the rest
            character[i]->suggestionsRec(currPrefix, result);

            // remove last character
            currPrefix.pop_back();
        }
    }
}

// print suggestions for given query prefix.
int Trie::autocomplete(std::string query, Suggestions& result)
{
    auto* pCrawl = this;

    // Check if prefix is present and find the
    // the node (of last level) with last character
    // of givenString.
    int level;
    int n = query.length();
    for (level = 0; level < n; level++)
    {
        int index = CHAR_TO_INDEX(query[level]);

        // noString in the Trie has this prefix
        if (!pCrawl->character[index]) return 0;

        pCrawl = pCrawl->character[index];
    }

    // If prefix is present as a word.
    bool isWord = pCrawl->isLeaf;

    // If prefix is last node of tree (has no
    // children)
    bool isLast = !pCrawl->hasChildren();

    // If prefix is present as a word, but
    // there is no subtree below the last
    // matching node.
    if (isWord && isLast)
    {
        result.push_back({query, true});
        return -1;
    }

    // If there are are nodes below last
    // matching character.
    if (!isLast)
    {
        const std::string& prefix = query;
        pCrawl->suggestionsRec(prefix, result);
        return 1;
    }
    return 0;
}

void Library::initialiseLibrary()
{
    appDataDir = File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory).getChildFile("PlugData");

    lastAppDirModificationTime = appDataDir.getLastModificationTime();

    updateLibrary();

    auto pddocPath = appDataDir.getChildFile("Documentation").getChildFile("pddoc").getFullPathName();

    parseDocumentation(pddocPath);

    startTimer(3000);
}

void Library::updateLibrary()
{
    auto settingsTree = ValueTree::fromXml(appDataDir.getChildFile("Settings.xml").loadFileAsString());

    auto pathTree = settingsTree.getChildWithName("Paths");

    searchTree = std::make_unique<Trie>();

    int i;
    t_class* o = pd_objectmaker;

    t_methodentry *mlist, *m;

#if PDINSTANCE
    mlist = o->c_methods[pd_this->pd_instanceno];
#else
    mlist = o->c_methods;
#endif
    
    for (i = o->c_nmethod, m = mlist; i--; m++)
    {
        String name(m->me_name->s_name);

        searchTree->insert(m->me_name->s_name);
    }

    searchTree->insert("graph");

    for (auto path : pathTree)
    {
        auto filePath = File(path.getProperty("Path").toString());

        for (auto& iter : RangedDirectoryIterator(filePath, false))
        {
            auto file = iter.getFile();
            if (file.getFileExtension() == ".pd") searchTree->insert(file.getFileNameWithoutExtension().toStdString());
        }
    }
}

void Library::parseDocumentation(const String& path)
{
    /* temp disable
    for (auto& iter : RangedDirectoryIterator(path, true))
    {
        auto file = iter.getFile();

        if (file.hasFileExtension("pddoc"))
        {
            XmlDocument doc(file);

            auto elt = doc.getDocumentElement();
            // elt->setTagName(file.getFileNameWithoutExtension());

            auto object = elt->getChildByName("object");
            auto name = object->getStringAttribute("name");

            auto meta = object->getChildByName("meta");

            auto keywords = meta->getChildElementAllSubText("keywords", "");
            auto description = meta->getChildElementAllSubText("description", "");
            
            
            auto outlets = IODescription();
            auto inlets = IODescription();
            
            auto inletsTree = object->getChildByName("inlets");
            auto outletsTree = object->getChildByName("outlets");
            
            if(name == "metro") {
                std::cout << "T" << std::endl;
            }
            
            for(auto* inlet : inletsTree->getChildIterator())
            {
                bool repeating = inlet->getNumAttributes() == 1 && inlet->getAttributeName(0) == "repeating";
                String totalDescription;
                
                for(auto* trigger : inlet->getChildIterator())
                {
                    totalDescription += "(" + trigger->getStringAttribute("on") + ") " +  trigger->getAllSubText() + "\n";
                }
            
                inlets.add({totalDescription, repeating});

            }
            
            for(auto* outlet : outletsTree->getChildIterator())
            {
                String type = outlet->getStringAttribute("out");
                String description = outlet->getAllSubText();
                if(!type.isEmpty()) description = "(" + type + ") " + description;
                bool repeating = outlet->getNumAttributes() == 1 && outlet->getAttributeName(0) == "repeating";
                outlets.add({description, repeating});
            }
            
            outletDescriptions[name] = outlets;
            inletDescriptions[name] = inlets;

            objectDescriptions[name] = description;
            objectKeywords[name] = StringArray::fromTokens(keywords, false);
        }
    } */
}

Suggestions Library::autocomplete(std::string query)
{
    Suggestions result;
    searchTree->autocomplete(std::move(query), result);
    return result;
}

String Library::getInletOutletTooltip(String boxname, int idx, int total, bool isInlet)
{
    auto name = boxname.upToFirstOccurrenceOf(" ", false, false);
    auto args = StringArray::fromTokens(boxname.fromFirstOccurrenceOf(" ", false, false), true);
    
    auto findInfo = [&name, &args, &total, &idx](IODescriptionMap& map){
        if(map.count(name)){
            auto descriptions = map.at(name);
            
            // if the amount of inlets is not equal to the amount in the spec, look for repeating inlets
            if(descriptions.size() < total) {
                for(int i = 0; i < descriptions.size(); i++) {
                    if(descriptions[i].second) { // repeating inlet found
                        for(int j = 0; j < total - descriptions.size(); j++){
                            descriptions.insert(i, descriptions[i]);
                        }
                    }
                }
            }
            
            auto result = isPositiveAndBelow(idx, descriptions.size()) ? descriptions[idx].first : String();
            result = result.replace("$mth", String(idx));
            result = result.replace("$nth", String(idx + 1));
            result = result.replace("$arg", args[idx]);
            
            return result;
        }
        
        return String();
    };
    
    return isInlet ? findInfo(inletDescriptions) : findInfo(outletDescriptions);
}

void Library::timerCallback()
{
    if (lastAppDirModificationTime < appDataDir.getLastModificationTime())
    {
        appDirChanged();
        updateLibrary();
        lastAppDirModificationTime = appDataDir.getLastModificationTime();
    }
}

}  // namespace pd
