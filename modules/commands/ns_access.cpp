/* NickServ core functions
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

class CommandNSAccess : public Command
{
 private:
	void DoServAdminList(CommandSource &source, const std::vector<Anope::string> &params, NickCore *nc)
	{
		Anope::string mask = params.size() > 2 ? params[2] : "";
		unsigned i, end;

		if (nc->access.empty())
		{
			source.Reply(_("Access list for \002%s\002 is empty."), nc->display.c_str());
			return;
		}

		if (nc->HasFlag(NI_SUSPENDED))
		{
			source.Reply(NICK_X_SUSPENDED, nc->display.c_str());
			return;
		}

		source.Reply(_("Access list for \002%s\002:"), params[1].c_str());
		for (i = 0, end = nc->access.size(); i < end; ++i)
		{
			Anope::string access = nc->GetAccess(i);
			if (!mask.empty() && !Anope::Match(access, mask))
				continue;
			source.Reply("    %s", access.c_str());
		}

		return;
	}

	void DoAdd(CommandSource &source, NickCore *nc, const Anope::string &mask)
	{

		if (mask.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		if (nc->access.size() >= Config->NSAccessMax)
		{
			source.Reply(_("Sorry, you can only have %d access entries for a nickname."), Config->NSAccessMax);
			return;
		}

		if (nc->FindAccess(mask))
		{
			source.Reply(_("Mask \002%s\002 already present on your access list."), mask.c_str());
			return;
		}

		nc->AddAccess(mask);
		source.Reply(_("\002%s\002 added to your access list."), mask.c_str());

		return;
	}

	void DoDel(CommandSource &source, NickCore *nc, const Anope::string &mask)
	{
		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return;
		}

		if (!nc->FindAccess(mask))
		{
			source.Reply(_("\002%s\002 not found on your access list."), mask.c_str());
			return;
		}

		source.Reply(_("\002%s\002 deleted from your access list."), mask.c_str());
		nc->EraseAccess(mask);

		return;
	}

	void DoList(CommandSource &source, NickCore *nc, const Anope::string &mask)
	{
		User *u = source.u;
		unsigned i, end;

		if (nc->access.empty())
		{
			source.Reply(_("Your access list is empty."), u->nick.c_str());
			return;
		}

		source.Reply(_("Access list:"));
		for (i = 0, end = nc->access.size(); i < end; ++i)
		{
			Anope::string access = nc->GetAccess(i);
			if (!mask.empty() && !Anope::Match(access, mask))
				continue;
			source.Reply("    %s", access.c_str());
		}

		return;
	}
 public:
	CommandNSAccess(Module *creator) : Command(creator, "nickserv/access", 1, 3)
	{
		this->SetDesc(_("Modify the list of authorized addresses"));
		this->SetSyntax(_("ADD \037mask\037"));
		this->SetSyntax(_("DEL \037mask\037"));
		this->SetSyntax(_("LIST"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &cmd = params[0];
		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		NickAlias *na;
		if (cmd.equals_ci("LIST") && u->IsServicesOper() && !mask.empty() && (na = findnick(params[1])))
			return this->DoServAdminList(source, params, na->nc);

		if (!mask.empty() && mask.find('@') == Anope::string::npos)
		{
			source.Reply(BAD_USERHOST_MASK);
			source.Reply(MORE_INFO, Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str(), this->name.c_str());
		}
		else if (u->Account()->HasFlag(NI_SUSPENDED))
			source.Reply(NICK_X_SUSPENDED, u->Account()->display.c_str());
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, u->Account(), mask);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, u->Account(), mask);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, u->Account(), mask);
		else
			this->OnSyntaxError(source, "");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Modifies or displays the access list for your nick.  This\n"
					"is the list of addresses which will be automatically\n"
					"recognized by %s as allowed to use the nick.  If\n"
					"you want to use the nick from a different address, you\n"
					"need to send an \002IDENTIFY\002 command to make %s\n"
					"recognize you.\n"
					" \n"
					"Examples:\n"
					" \n"
					"    \002ACCESS ADD anyone@*.bepeg.com\002\n"
					"        Allows access to user \002anyone\002 from any machine in\n"
					"        the \002bepeg.com\002 domain.\n"
					" \n"
					"    \002ACCESS DEL anyone@*.bepeg.com\002\n"
					"        Reverses the previous command.\n"
					" \n"
					"    \002ACCESS LIST\002\n"
					"        Displays the current access list."), Config->NickServ.c_str(), Config->NickServ.c_str());
		return true;
	}
};

class NSAccess : public Module
{
	CommandNSAccess commandnsaccess;

 public:
	NSAccess(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnsaccess(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandnsaccess);
	}
};

MODULE_INIT(NSAccess)