#include <discord-connector>
#include <discord_channels>
#include <dcc>

DCCMD:ann(DCC_Channel:channel, DCC_User:user, const params[])
{
    new
        string[1050],
        text[1050],
        bool:hasdevrole;
    DCC_HasGuildMemberRole(gtoguild, DCC_User:user, developer_role, hasdevrole);
    if(!hasdevrole || channel != cmdexecute_Discord_Chat) {
        return 0;
    }
    if(sscanf(params, "s[1050]", text)) return DCC_SendChannelMessage(cmdexecute_Discord_Chat, "```!ann (text)```");
    format(string, sizeof(string), "@everyone %s", text);
    DCC_SendChannelMessage(cmdann_Discord_Chat, string);
    DCC_SendChannelMessage(cmdann_Discord_Chat, "Executed");
    return 1;
}