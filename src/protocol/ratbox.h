/* Ratbox IRCD functions
 *
 * (C) 2003-2008 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#define UMODE_a 0x00000001
#define UMODE_C 0x00000002
#define UMODE_i 0x00000004
#define UMODE_o 0x00000008
#define UMODE_z 0x00000010
#define UMODE_w 0x00000020
#define UMODE_s 0x00000040
#define UMODE_c 0x00000080
#define UMODE_r 0x00000100
#define UMODE_k 0x00000200
#define UMODE_f 0x00000400
#define UMODE_y 0x00000800
#define UMODE_d 0x00001000
#define UMODE_n 0x00002000
#define UMODE_x 0x00004000
#define UMODE_u 0x00008000
#define UMODE_b 0x00010000
#define UMODE_l 0x00020000
#define UMODE_g 0x00040000
#define UMODE_Z 0x00080000

#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040
#define CMODE_l 0x00000080


#define DEFAULT_MLOCK CMODE_n | CMODE_t


class RatboxProto : public IRCDProto {
	public:
		void SendAkillDel(const char *, const char *);
		void cmd_topic(const char *, const char *, const char *, const char *, time_t);
		void SendAkill(const char *, const char *, const char *, time_t, time_t, const char *);
		void SendSVSKill(const char *, const char *, const char *);
		void SendSVSMode(User *, int, const char **);
		void SendMode(const char *, const char *, const char *);
		void SendClientIntroduction(const char *, const char *, const char *, const char *, const char *);
		void SendKick(const char *, const char *, const char *, const char *);
		void SendNoticeChanops(const char *, const char *, const char *);
		void cmd_message(const char *, const char *, const char *);
		void cmd_notice(const char *, const char *, const char *);
		void cmd_privmsg(const char *, const char *, const char *);
		void SendBotOp(const char *, const char *);
		void SendQuit(const char *, const char *);
		void SendPong(const char *, const char *);
		void SendJoin(const char *, const char *, time_t);
		void SendSQLineDel(const char *);
		void SendInvite(const char *, const char *, const char *);
		void SendPart(const char *, const char *, const char *);
		void SendGlobops(const char *, const char *);
		void SendSQLine(const char *, const char *);
		void SendForceNickChange(const char *, const char *, time_t) { } // Ratbox doesn't have an SVSNICK command
		void SendConnect();
		void SendSGLineDel(const char *);
		void SendSGLine(const char *, const char *);
		void cmd_server(const char *, int, const char *);
		void set_umode(User *, int, const char **);
		int valid_nick(const char *);
		void cmd_numeric(const char *, int, const char *, const char *);
} ircd_proto;
