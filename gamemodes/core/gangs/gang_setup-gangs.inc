#include <YSI_Coding\y_hooks>

hook OnMySQLConnected()
{
    static const query[] = "\
        CREATE TABLE IF NOT EXISTS gangs \
        (\
            g_id int(11) NOT NULL AUTO_INCREMENT,\
            g_name varchar(24) NOT NULL,\
            g_owner varchar(24) NOT NULL,\
            g_kills int(11) NOT NULL DEFAULT '0' , \
            g_deaths int(11) NOT NULL DEFAULT '0',\
            PRIMARY KEY (g_id), \
            UNIQUE KEY username (gang_name)\
        )";
    mysql_tquery(MySQL_GetHandle(), query);

    log("setting up 'gangs' table...");
    return 1;
}
