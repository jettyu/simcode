#ifndef __SIMCODE_CONFIG_PARSER_H__
#define __SIMCODE_CONFIG_PARSER_H__
#include <simcode/config/node.h>
#include <string>
class ParseInterface
{
public:
    virtual ~ParseInterface(){}
    virtual bool IsKey(const std::string& buf, std::string* key, bool* isEnd) = 0;
    virtual int Parse(Node* node, const std::string& buf) = 0;
};

class Parser
{
public:
    Parser(ParseInterface *pi__, const Node::FlushValueCallback fc=NULL): 
        pi(pi__),
        node_flush_callback_(fc)
    {
        node = new Node("", fc);
    }    
    virtual ~Parser()
    {
        delete pi;
        delete node;
    }
    Node *getNode()
    {
        return node;
    }
    int ParseString(const std::string& buf);
    int LoadFile(const std::string& filename);
    private:
    Node* node;
    ParseInterface* pi;
    Node::FlushValueCallback node_flush_callback_;
};

#endif
