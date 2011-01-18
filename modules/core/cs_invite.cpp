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

class CommandCSInvite : public Command
{
 public:
	CommandCSInvite() : Command("INVITE", 1, 3)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];

		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Channel *c = ci->c;

		if (!(c = findchan(chan)))
		{
			source.Reply(CHAN_X_NOT_IN_USE, chan.c_str());
			return MOD_CONT;
		}

		ci = c->ci;

		if (!check_access(u, ci, CA_INVITE))
		{
			source.Reply(ACCESS_DENIED);
			return MOD_CONT;
		}

		User *u2;
		if (params.size() == 1)
			u2 = u;
		else
		{
			if (!(u2 = finduser(params[1])))
			{
				source.Reply(NICK_X_NOT_IN_USE, params[1].c_str());
				return MOD_CONT;
			}
		}

		// XXX need a check for override...
		Log(LOG_COMMAND, u, this, ci) << "for " << u2->nick;

		if (c->FindUser(u2))
			source.Reply(CHAN_INVITE_ALREADY_IN, c->name.c_str());
		else
		{
			ircdproto->SendInvite(whosends(ci), chan, u2->nick);
			source.Reply(CHAN_INVITE_OTHER_SUCCESS, u2->nick.c_str(), c->name.c_str());
			u2->SendMessage(whosends(ci), CHAN_INVITE_SUCCESS, c->name.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_HELP_INVITE);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "INVITE", CHAN_INVITE_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_INVITE);
	}
};

class CSInvite : public Module
{
	CommandCSInvite commandcsinvite;

 public:
	CSInvite(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsinvite);
	}
};

MODULE_INIT(CSInvite)