#include <YSI_Coding\y_hooks>

hook OnMySQLConnected() {
    static const query[] = "\
    CREATE TABLE IF NOT EXISTS player_gang \
    ( \
    	u_id INT(11) NOT NULL, \
    	pGangID INT(11) NOT NULL DEFAULT '0', \
        pGangPos INT(11) NOT NULL DAFAULT '0', \
    	PRIMARY KEY (u_id), \
    	FOREIGN KEY (u_id) REFERENCES players(u_id) \
        ON DELETE CASCADE \
        ON UPDATE CASCADE \
    )";
    mysql_tquery(MySQL_GetHandle(), query);

    log("setting up 'player-gang' table...");
    return 1;
}