/*
 * InspIRCd -- Internet Relay Chat Daemon
 *
 *   Copyright (C) 2011 Adam <Adam@anope.org>
 *
 * This file is part of InspIRCd.  InspIRCd is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "inspircd.h"
#include "socket.h"
#include "xline.h"
#include "socketengine.h"

#include "main.h"
#include "utils.h"
#include "treeserver.h"
#include "treesocket.h"

bool TreeSocket::Whois(const std::string &prefix, parameterlist &params)
{
	if (params.size() < 1)
		return true;
	User* u = ServerInstance->FindNick(prefix);
	if (u)
	{
		// an incoming request
		if (params.size() == 1)
		{
			User* x = ServerInstance->FindNick(params[0]);
			if ((x) && (IS_LOCAL(x)))
			{
				LocalUser* lu = IS_LOCAL(x);
				long idle = abs((long)((lu->idle_lastmsg) - ServerInstance->Time()));
				parameterlist par;
				par.push_back(prefix);
				par.push_back(ConvToStr(x->signon));
				par.push_back(ConvToStr(idle));
				// ours, we're done, pass it BACK
				Utils->DoOneToOne(params[0], "IDLE", par, u->server);
			}
			else
			{
				// not ours pass it on
				if (x)
					Utils->DoOneToOne(prefix, "IDLE", params, x->server);
			}
		}
		else if (params.size() == 3)
		{
			std::string who_did_the_whois = params[0];
			User* who_to_send_to = ServerInstance->FindNick(who_did_the_whois);
			if ((who_to_send_to) && (IS_LOCAL(who_to_send_to)))
			{
				// an incoming reply to a whois we sent out
				ServerInstance->Parser->CallHandler("WHOIS", params, u);
			}
			else
			{
				// not ours, pass it on
				if (who_to_send_to)
					Utils->DoOneToOne(prefix, "IDLE", params, who_to_send_to->server);
			}
		}
	}
	return true;
}


