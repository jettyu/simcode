#ifndef __SIMCODE_CONFIG_PARSER_H__
#define __SIMCODE_CONFIG_PARSER_H__
#include <simcode/config/node.h>
#include <string>
namespace simcode {
namespace config {

static std::string TrimLeft(const std::string& str)
{
    if (str.empty()) return str;
    int i = 0;
    for (i=0; i<str.size(); ++i)
    {
        if (str[i] == ' ' || str[i] == '\t' || str[i] == '\r' || str[i] == '\n' || isspace(str[i]))
        continue;
        break;
    }
    if (i == str.size()) return str;
    return str.substr(i);
}

static std::string TrimRight(const std::string& str)
{
    if (str.empty()) return str;
    int i = 0;
    for (i=0; i<str.size(); ++i)
    {
        char c = str[str.size()-i-1];
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || isspace(c))
        continue;
        break;
    }
    if (i == str.size()) return str;
    return str.substr(0, str.size()-i);
}



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
}//endof namespace config
}//endof namespace simcode
#endif
