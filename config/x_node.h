#ifndef __SIMCODE_CONFIG_XNODE_H__
#define __SIMCODE_CONFIG_XNODE_H__
#include <simcode/config/parser.h>
/*
<root>
   <node>
		<n1>
		k1 = 1
		k2 = b
		</n1>
		<n2>
		k1 = 1
		k2 = b
		</n2>
   </node>
</root>
 */
namespace simcode {
namespace config {
class XNodeInterface
{
public:
    virtual ~XNodeInterface(){}
    virtual int Parse(const std::string& buf) = 0;
    virtual XNodeInterface* assign() = 0;
};

void flush_xnode_interface(const simex::any& value)
{
    if (!value.empty())
    {
        delete simex::any_cast<XNodeInterface*>(value);
    }
}

class XNodeParserI : public ParseInterface
{
public:
    XNodeParserI(XNodeInterface * xi__):xi(xi__)
    {
    }
    virtual ~XNodeParserI()
    {
        delete xi;
    }
    virtual bool IsKey(const std::string& str, std::string* key, bool* isEnd)
    {
        bool ok = false;
        if (str.size() >2 && str[0]=='<' && str[str.size()-1]=='>')
        {
            ok = true;
            if (str[1]=='/')
            {
                *key = str.substr(2, str.size()-2-1);
                *isEnd = true;
            }
            else
            {
                *key = str.substr(1, str.size()-2);
                *isEnd = false;
            }
        } 
        return ok;
    }
    virtual int Parse(Node* node, const std::string& buf)
    {
        if (node->Value().empty())
        {
            node->SetValue(xi->assign());
        }
        return (simex::any_cast<XNodeInterface*>(node->Value()))->Parse(buf);
    }
private:
    XNodeInterface *xi;
};

class XNodeParser : public Parser
{
public:
    XNodeParser(XNodeInterface * xi):
        Parser(new XNodeParserI(xi), flush_xnode_interface)
    {
    }
    virtual ~XNodeParser(){}
};

}//endof namespace config
}//endof namespace simcode

#endif
