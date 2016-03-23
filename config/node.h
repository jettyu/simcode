#ifndef __SIMCODE_CONFIG_NODE_H__
#define __SIMCODE_CONFIG_NODE_H__

#include <simcode/base/any.h>
#include <map>
#include <string>
#include <vector>

namespace simcode {
namespace config {

class Node 
{
public:
    typedef void (*FlushValueCallback)(const simex::any& value);
    Node(const std::string& name__, const FlushValueCallback fc=NULL)
        :name(name__),
         root(NULL),
         flush_value_callback_(fc)
    {}
    virtual ~Node(){
        std::map<std::string, Node*>::iterator it;
        for (it=child.begin(); it!=child.end(); ++it)
        {
            if (!it->second->value.empty() && flush_value_callback_)
            {
                flush_value_callback_(it->second->value);
            }
            delete(it->second);
        }
        child.clear();
    }
    const std::string& Name() const{return name;}
    Node *Root() {return root;}
    const std::map<std::string, Node*>& AllChild() const {return child;}
    const Node* Child(const std::string& key) const
    {
        std::map<std::string, Node*>::const_iterator it;
        it = child.find(key);
        if (it != child.end()){return it->second;}
        return NULL;
    }
    const Node* Search(const std::vector<std::string>& keys, int depth=0) const
    {
        if (keys.size() - depth ==1)
        {
            return Child(keys[depth]);
        }
        else if (keys.size() - depth > 1)
        {
            const Node* n = Child(keys[depth]);
            if (n != NULL) return n->Search(keys, ++depth);
        }
        return NULL;
    }
    const simex::any& Value() const
    {
        return value;
    }
    void SetRoot(Node *root__)
    {
        root = root__;
    }
    void SetValue(const simex::any& value__)
    {
        value = value__;
    }
    Node* AddChild(const std::string& key, Node* child__)
    {
        child__->root = this;
        child[key] = child__;
        return child__;
    }
private:
    std::string name;
    simex::any value;
    Node *root;
    std::map<std::string, Node*> child;
    FlushValueCallback flush_value_callback_;
};

}//endof namespace config
}//endof namespace simcode

#endif

