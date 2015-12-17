#include <simcode/base/string_utils.h>
#include <simcode/config/parser.h>
#include <fstream>
#include <stdlib.h>
using namespace std;
namespace simcode {
namespace config {
int Parser::ParseString(const std::string& buf)
{
    std::string key;
    bool isEnd = false;
    if (pi->IsKey(buf, &key, &isEnd))
    {
        if (!isEnd)
        {
            if (key.empty())
            {
               return 1;
            }
            if (node == NULL)
            {
                node = new Node(key, node_flush_callback_);
            }
            else
            {
                node = node->AddChild(key, new Node(key));
            }
        }
        else
        {
            if (node==NULL || node->Name() != key)
            {
                return 2;
            }
            else
            {
                node = node->Root();
            }
        }
    }
    else
    {
        return pi->Parse(node, buf);
    }
    return 0;
}
int Parser::LoadFile(const std::string& filename)
{
    std::ifstream fi(filename);
    if (!fi.is_open()) return 12;
    std::string line;
    int ret = 0;
    while (getline(fi, line)) 
    {
        line = StringTrimLeft(line, ' ');
        line = StringTrimLeft(line, '\t');
        line = StringTrimLeft(line, '\r');
        line = StringTrimLeft(line, '\n');
        line = StringTrimRight(line, ' ');
        line = StringTrimRight(line, '\t');
        line = StringTrimRight(line, '\r');
        line = StringTrimRight(line, '\n');
        
        if (line.empty() || line[0] == '#') continue;
        ret = ParseString(line);
        if (ret != 0) break;
    }
    return ret;
}
}//endof namespace config
}//endof namespace simcode
