/* ChanServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandCSDrop : public Command
{
 public:
	CommandCSDrop() : Command("DROP", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_FORBIDDEN);
		this->SetFlag(CFLAG_ALLOW_SUSPENDED);
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];

		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (readonly)
		{
			source.Reply(CHAN_DROP_DISABLED); // XXX: READ_ONLY_MODE?
			return MOD_CONT;
		}

		ci = cs_findchan(chan);

		if (ci->HasFlag(CI_FORBIDDEN) && !u->Account()->HasCommand("chanserv/drop"))
		{
			source.Reply(CHAN_X_FORBIDDEN, chan.c_str());
			return MOD_CONT;
		}

		if (ci->HasFlag(CI_SUSPENDED) && !u->Account()->HasCommand("chanserv/drop"))
		{
			source.Reply(CHAN_X_FORBIDDEN, chan.c_str());
			return MOD_CONT;
		}

		if ((ci->HasFlag(CI_SECUREFOUNDER) ? !IsFounder(u, ci) : !check_access(u, ci, CA_FOUNDER)) && !u->Account()->HasCommand("chanserv/drop"))
		{
			source.Reply(ACCESS_DENIED);
			return MOD_CONT;
		}

		if (ci->c && ModeManager::FindChannelModeByName(CMODE_REGISTERED))
			ci->c->RemoveMode(NULL, CMODE_REGISTERED, "", false);

		if (ircd->chansqline && ci->HasFlag(CI_FORBIDDEN))
		{
			XLine x(ci->name);
			ircdproto->SendSQLineDel(&x);
		}

		bool override = (ci->HasFlag(CI_SECUREFOUNDER) ? !IsFounder(u, ci) : !check_access(u, ci, CA_FOUNDER));
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "founder: " << (ci->founder ? ci->founder->display : "none");
		if (override)
			ircdproto->SendGlobops(ChanServ, "\2%s\2 used DROP on channel \2%s\2", u->nick.c_str(), ci->name.c_str());

		delete ci;

		source.Reply(CHAN_DROPPED, chan.c_str());

		FOREACH_MOD(I_OnChanDrop, OnChanDrop(chan));

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		if (u->Account() && u->Account()->IsServicesOper())
			source.Reply(CHAN_SERVADMIN_HELP_DROP);
		else
			source.Reply(CHAN_HELP_DROP);

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "DROP", CHAN_DROP_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_DROP);
	}
};

class CSDrop : public Module
{
	CommandCSDrop commandcsdrop;

 public:
	CSDrop(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsdrop);
	}
};

MODULE_INIT(CSDrop)