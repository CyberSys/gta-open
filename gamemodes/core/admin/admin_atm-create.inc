#include <YSI_Coding\y_hooks>

#define MAX_ATM_MACHINES		(50)

static
    myobject
;

enum E_ATM_DATA
{
	bool:atm_created
}
static
	atm_data[MAX_ATM_MACHINES][E_ATM_DATA],
	i_atm = 0;

forward Float:GetXYInFrontOfPlayer2(playerid, &Float:q, &Float:w, Float:distance);

Float:GetXYInFrontOfPlayer2(playerid, &Float:q, &Float:w, Float:distance)
{
    new Float:a;
    GetPlayerPos(playerid, q, w, a);
    if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER) GetVehicleZAngle(GetPlayerVehicleID(playerid), a);
    else GetPlayerFacingAngle(playerid, a);
    q += (distance * floatsin(-a, degrees));
    w += (distance * floatcos(-a, degrees));
    return a;
}

static stock CreateAtm(Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, atm_vwid = 0, atm_interior = 0) {
	new atmid = GetAtmFreeID();

	if(atmid == -1) {
		print("ERROR: CreateATM - MAX_ATM_MACHINES reached, increase the limit size.");
		return -1;
	}

    CreateDynamicObject(2942, x, y, z, rx, ry, rz, atm_vwid, atm_interior, -1, 40.0);  


	atm_data[atmid][atm_created] = true;   

    new string[256];

    static const query[] = "\
    INSERT INTO \
        atm(atm_id, PosX, PosY, PosX, RotX, RotY, RotZ, Vwid, Interior) \
    VALUES \
        (%i, %f, %f, %f, %f, %f, %f, %i, %i) \
    ON DUPLICATE KEY UPDATE \
        PosX = %f, \
        PosY = %f, \
        PosZ = %f, \
        RotX = %f, \
        RotY = %f, \
        RotZ = %f, \
        Vwid = %i, \
        Interior = %i \
    ";

    mysql_format(MySQL_GetHandle(), string, sizeof(string), query, atmid, x, y, z, rx, ry, rz, atm_vwid, atm_interior, x, y, z, rx, ry, rz, atm_vwid, atm_interior);
    mysql_tquery(MySQL_GetHandle(), string);

	return (i_atm ++);
}

static stock GetAtmFreeID()
{
	for(new i = 0; i < MAX_ATM_MACHINES; i++)  {
		if(!atm_data[i][atm_created]) {
			return i;
		}
	}
	return -1;
}

ACMD:[5]createatm(playerid, params[])
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    GetXYInFrontOfPlayer2(playerid, x, y, 5.0);
    myobject = CreateDynamicObject(2942, x, y, z, 0, 0, 0, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), -1, 40.0); 
    EditDynamicObject(playerid, myobject);
    return 1;
}

hook OnPlayerEditDynObj(playerid, obj, response, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz)
{
    SetObjectPos(obj, x, y, z);		          
	SetObjectRot(obj, rx, ry, rz);
    if(response == EDIT_RESPONSE_FINAL)
	{
		CreateAtm(x, y, z, rx, ry, rx, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid));
        DestroyObject(myobject);
    }
    if(response == EDIT_RESPONSE_CANCEL)
    {
        DestroyObject(myobject);
    }
    return 1;
}