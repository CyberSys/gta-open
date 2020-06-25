#include <YSI_Coding\y_hooks>

hook OnMySQLConnected() {
    static const query[] = "\
    CREATE TABLE IF NOT EXISTS atm \
    ( \
    	atm_id INT(11) NOT NULL, \
    	PosX MEDIUMINT(20) NOT NULL, \
        PosY MEDIUMINT(20) NOT NULL, \
        PosZ MEDIUMINT(20) NOT NULL, \
        RotX MEDIUMINT(20) NOT NULL, \
        RotY MEDIUMINT(20) NOT NULL, \
        RotZ MEDIUMINT(20) NOT NULL, \
        Vwid INT(11) NOT NULL DEFAULT 0, \
        Interior INT(11) NOT NULL DEFAULT 0, \
    	PRIMARY KEY (atm_id), \
    )";
    mysql_tquery(MySQL_GetHandle(), query);
    log("setting up 'atm' table...");
    return 1;
}