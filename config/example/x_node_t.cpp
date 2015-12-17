#include <simcode/config/x_node.h>
#include <simcode/base/string_utils.h>
#include <simcode/base/command.h>
#include <iostream>
using namespace simcode;
using namespace config;
using namespace std;

typedef struct XNode {
    std::string key;
    std::string value;
}XNode;

class XNodes : public XNodeInterface
{
public:
    virtual int Parse(const std::string& buf)
    {
        cout<<"value="<<buf<<endl;
        int offset = buf.find('=');
        if (offset == std::string::npos) return 1;
        XNode xnode;
        xnode.key = buf.substr(0, offset);
        if (offset != buf.size()-1) xnode.value = buf.substr(offset+1);
        xnode.key = StringTrimRight(xnode.key, ' ');
        xnode.key = StringTrimRight(xnode.key, '\t');
        xnode.value = StringTrimLeft(xnode.value, ' ');
        xnode.value = StringTrimLeft(xnode.value, '\t');
        nodes_.push_back(xnode);
        return 0;
    }
    virtual XNodeInterface* assign()
    {
        return new XNodes;
    }
    const std::vector<XNode>& getNodes() const
    {
        return nodes_;
    }
private:
    std::vector<XNode> nodes_;
};

int main()
{
    XNodeParser xNodeParser(new XNodes);
    int ret = 0;
    ret = xNodeParser.LoadFile("xn.conf");
    cout<<"ret="<<ret<<endl;
    const Node * node = xNodeParser.getNode();
    node = node->Search(command("root")("node"));
    const std::map<std::string, Node*>& allchild = node->AllChild();
    std::map<std::string, Node*>::const_iterator mit;
    for (mit=allchild.begin(); mit!=allchild.end(); ++mit)
    {
        const Node* child = mit->second;
        const simex::any& value = child->Value();
        if (!value.empty())
        {
            const XNodeInterface *xn = simex::any_cast<XNodeInterface*>(value);
            const XNodes* xnodes = dynamic_cast<const XNodes*>(xn);
            const std::vector<XNode>& xnode_vec = xnodes->getNodes();
            std::vector<XNode>::const_iterator it;
            for (it=xnode_vec.begin(); it!=xnode_vec.end(); ++it)
                cout<<"key="<<it->key<<"|" <<"value="<<it->value<<endl;
        }
        else
        {
            cout<<"value is empty!"<<endl;
        }
    }
    return 0;
}
