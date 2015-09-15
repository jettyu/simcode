#include <simcode/mysql/mysql.h>
#include <simcode/mysql/query_sector.h>

using namespace simcode;
using namespace mysql;

int main(void)
{
    MysqlInfo mysql_info;
    mysql_info.host   = "192.168.78.128";
    mysql_info.port   = 3306;
    mysql_info.user   = "root";
    mysql_info.passwd = "Alen";
    mysql_info.db     = "test";

    Mysql mysql(mysql_info);
    MysqlSelect selector(&mysql);
    selector.Select("id, name, age")
            .Table("test")
            .Filter("age", 20)
            .Limit(1);
    MysqlRes mysql_res(selector.TryQuery());
    if (mysql.errcode())
    {
        printf("errmsg=%s\n", mysql.errmsg().c_str());
        return 1;
    }
    int row_num = mysql_res.RowNum();
    for (int i=0; i<row_num; i++)
    {
        MYSQL_ROW row = mysql_res.FetchRow();
        unsigned long* res_length = mysql_res.FetchLengths();
        if (!row) continue;
        if (row[0]) printf("id:%d\n", atoi(row[0]));
        if (row[1]) printf("name:%s\n", row[1]);
        if (row[2]) printf("age:%d\n", atoi(row[2]));
    }

    return 0;
}
