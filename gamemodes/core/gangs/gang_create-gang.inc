#include <YSI_Coding\y_hooks>
#include <YSI_Coding\y_iterate>

#define MAX_GANGS  (100)

//statements
static
    Statement: stmt_InsertGANG,
    Statement: stmt_LoadGANG,
    Statement: stmt_ExistGANG,
    Statement: stmt_RemoveGANG,
    Statement: stmt_UpdateGANG,
    Statement: stmt_UpdateGANGOWNER;

//variables
static
    Iterator: Gang_Iter<MAX_GANGS>,

    GangID[MAX_GANGS],
    GangName[MAX_GANGS],
    GangOwner[MAX_GANGS],
    GangKills[MAX_GANGS],
    GangDeaths[MAX_GANGS];

//player variables
static
    PlayerGangID[MAX_PLAYERS],
    PlayerGangPos[MAX_PLAYERS]
;

hook OnMySQLConnected()
{
    static const insertQuery[] = "\
	INSERT INTO \
		gangs (g_id, g_name, g_owner) \
	VALUES \
		(?,?,?) \
	";
	stmt_InsertGANG = MySQL_PrepareStatement(MySQL_GetHandle(), insertQuery);

    static const loadQuery[] = "\
	SELECT \
		g_id, \
		g_name, \
		g_owner, \
		g_kills, \
		g_deaths \
	FROM \
		gangs \
	";
	stmt_LoadGANG = MySQL_PrepareStatement(MySQL_GetHandle(), loadQuery);

    static const checkQuery[] = "\
	SELECT \
		g_id \
	FROM \
		gangs \
	WHERE \
		g_id = ? \
	";
	stmt_ExistGANG = MySQL_PrepareStatement(MySQL_GetHandle(), checkQuery);

    static const deleteQuery[] = "\
    DELETE FROM \
        gangs \
    WHERE \
        g_id = ? \
    ";
    stmt_RemoveGANG = MySQL_PrepareStatement(MySQL_GetHandle(), deleteQuery);

    static const updateQuery[] = "\
    UPDATE \
        gangs \
    SET \
        g_kills = ?, \
        g_deaths = ? \
    WHERE \
        g_id = ?";
    stmt_UpdateGANG = MySQL_PrepareStatement(MySQL_GetHandle(), updateQuery);

    static const updateOwner[] = "\
    UPDATE \
        gangs \
    SET \
        g_owner = ? \
    WHERE \
        g_id = ?";
    stmt_UpdateGANGOWNER = MySQL_PrepareStatement(MySQL_GetHandle(), updateOwner);

    return 1;
}

Gang_Load() {

    inline const OnGANGLoad() {
        new
            g_id,
            g_name,
            g_owner,
            g_kills,
            g_deaths;

        MySQL_BindResultInt(stmt_LoadGANG, 0, g_id);
        MySQL_BindResult(stmt_LoadGANG, 1, g_name);
        MySQL_BindResult(stmt_LoadGANG, 2, g_owner);
        MySQL_BindResultInt(stmt_LoadGANG, 3, g_kills);
        MySQL_BindResultInt(stmt_LoadGANG, 4, g_deaths);

        while(MySQL_Statement_FetchRow(stmt_LoadGANG)) {
            GangLoad(g_id, g_name, g_owner, g_kills, g_deaths);
        }
    }

    MySQL_ExecuteThreaded_Inline(stmt_LoadGANG, using inline OnGANGLoad);
}

stock Gang_InsertToDatabase(GangID)
{
	MySQL_BindInt(stmt_InsertGANG, 0, GangID);
    MySQL_Bind(stmt_InsertGANG, 1, GangName[GangID]);
    MySQL_Bind(stmt_InsertGANG, 2, GangOwner[GangID]);
    MySQL_ExecuteThreaded(stmt_InsertGANG);
}

stock Gang_UpdateKD(GangID)
{
    MySQL_BindInt(stmt_UpdateGANG, 0, GangKills[GangID]);
    MySQL_BindInt(stmt_UpdateGANG, 1, GangDeaths[GangID]);
    MySQL_BindInt(stmt_UpdateGANG, 2, GangID);
    MySQL_ExecuteThreaded(stmt_UpdateGANG);
}

stock Gang_UpdateOwner(GangID)
{
    MySQL_Bind(stmt_UpdateGANGOWNER, 0, GangOwner[GangID])
    MySQL_BindInt(stmt_UpdateGANGOWNER, 1, GangID);
    MySQL_ExecuteThreaded(stmt_UpdateGANGOWNER);
}

GangLoad(GID, GName, GOwner, GKills, GDeaths)
{
    GangID[GID] = GID;
    GangName[GID] = GName;
    GangOwner[GID] = GOwner;
    GangKills[GID] = GKills;
    GangDeaths[GID] = GDeaths;
    Iter_Add(Gang_Iter, GID);
}

GangCreate(playerid, const gNAME[], bool:save = false)
{
    new 
        gID = Iter_Alloc(Gang_Iter);
    new gOwnerName[MAX_PLAYER_NAME];
    if (gID == ITER_NONE) {
		return -1;
	}
    GetPlayerName(playerid, gOwnerName, sizeof(gOwnerName));
    GangID[gID] = gID;
    GangName[gID] = gNAME;
    GangOwner[gID] = gOwnerName;

    Player_SetGangID(playerid, gID);
    Player_SetGangPos(playerid, 10);

    Iter_Add(Gang_Iter, gID);

    if(save) {
        Gang_InsertToDatabase(gID);
    }

    return gID;
}

GangDelete(playerid, gID) 
{
    inline OnGangRemove()
    {
        if(MySQL_Statement_FetchRow(stmt_ExistGANG))
        {
            MySQL_BindInt(stmt_RemoveGANG, 0, gID);
            MySQL_ExecuteThreaded(stmt_RemoveGANG);

            if(playerid != INVALID_PLAYER_ID) {
                SendServerMsgF(playerid, "Gang %s has beed removed.", GangName[gID]);
                SendMsgAllF(COLOR_MODERATION, "[GANG REMOVAL]: Gang %s has been removed.", GangName[gID]);
            }

            Iter_Remove(Gang_Iter, gID);
        }
        else
        {
            if (playerid != INVALID_PLAYER_ID) {
                SendErrorMsgF(playerid, "%d is not a valid Gang!", gID);
            }
        }
    }
    MySQL_BindInt(stmt_ExistGANG, 0, gID);
    MySQL_ExecuteThreaded_Inline(stmt_ExistGANG, using inline OnGangRemove);
}

stock Player_SetGangPos(playerid, level)
{
    PlayerGangPos[playerid] = level;
    new
        string[256];
    static const query[] = "\
    INSERT INTO \
        player_gang(u_id, pGangPos) \
    VALUES \
        (%i, %i) \
    ON DUPLICATE KEY UPDATE \
        pGangPos = %i \
    ";   
    mysql_format(MySQL_GetHandle(), string, sizeof(string), query, Player_GetAccountID(playerid), PlayerGangPos[playerid], PlayerGangPos[playerid]);
    mysql_tquery(MySQL_GetHandle(), string);
}

stock Player_SetGangID(playerid, level)
{
    PlayerGangID[playerid] = level;
    new
        string[256];
    static const query[] = "\
    INSERT INTO \
        player_gang(u_id, pGangID) \
    VALUES \
        (%i, %i) \
    ON DUPLICATE KEY UPDATE \
        pGangID = %i \
    ";   
    mysql_format(MySQL_GetHandle(), string, sizeof(string), query, Player_GetAccountID(playerid), PlayerGangID[playerid], PlayerGangID[playerid]);
    mysql_tquery(MySQL_GetHandle(), string);
}

ACMD:[5]cgang(playerid, params[])
{
    new target, namegang[128];
    if(sscanf(params, "us[128]", target, namegang)) {
        return SendSyntaxMsg(playerid, "/cgang (name/id (Gang Owner)) (Gang Name)");
    }
    if(!IsPlayerConnected(target)) {
        return SendErrorMsg(playerid, "Player is not connected");
    }
    GangCreate(target, namegang, true);
    SendMsgAllF(COLOR_MODERATION, "%p (%d) Has been accepted and created the Gang %s", namegang);
    return 1;
}

ACMD:[5]dgang(playerid, params[])
{
    new gtarget;
    if(sscanf(params, "i", gtarget)) {
        return SendSyntaxMsg(playerid, "/dgang (gang id)");
    }
    GangDelete(playerid, gtarget);
    return 1;
}