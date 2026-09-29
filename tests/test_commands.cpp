#include <functional>
#include <iostream>
#include <string>

#include "Channel.hpp"
#include "Client.hpp"
#include "Command.hpp"
#include "Commands.hpp"
#include "ServerState.hpp"

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

static int g_failed = 0;

static Command makeCmd(const std::string& name, const std::string& p1 = "",
                       const std::string& p2 = "")
{
    Command cmd;
    cmd.name = name;
    if (!p1.empty())
        cmd.params.push_back(p1);
    if (!p2.empty())
        cmd.params.push_back(p2);
    return cmd;
}

static void check(const std::string& label, const std::string& got,
                  const std::string& expected)
{
    if (got == expected)
    {
        std::cout << GREEN "[OK] " RESET << label << std::endl;
        return;
    }
    ++g_failed;
    std::cout << RED "[KO] " RESET << label << std::endl
              << "     attendu : \"" << expected << "\"" << std::endl
              << "     obtenu  : \"" << got << "\"" << std::endl;
}

static void check(const std::string& label, bool got, bool expected)
{
    check(label, std::string(got ? "true" : "false"),
          std::string(expected ? "true" : "false"));
}

static void testPass()
{
    std::cout << "--- PASS ---" << std::endl;
    ServerState state("secret");

    // fd -1 : le destructeur de Client ne ferme aucun vrai fd
    {
        Client c(-1);
        cmdPass(&c, makeCmd("PASS"), state);
        check("PASS sans parametre -> 461", c.getOutBuff(),
              ":irc.42.fr 461 * PASS :Not enough parameters\r\n");
    }
    {
        Client c(-1);
        cmdPass(&c, makeCmd("PASS", "wrong"), state);
        check("PASS mauvais mot de passe -> 464", c.getOutBuff(),
              ":irc.42.fr 464 * :Password incorrect\r\n");
        check("PASS mauvais mot de passe -> hasPass false", c.hasPass(), false);
    }
    {
        Client c(-1);
        cmdPass(&c, makeCmd("PASS", "secret"), state);
        check("PASS correct -> aucune reponse", c.getOutBuff(), "");
        check("PASS correct -> hasPass true", c.hasPass(), true);
        check("PASS seul -> pas encore enregistre", c.isRegistered(), false);
    }
    {
        Client c(-1);
        c.setNickname("rinda");
        c.setUsername("rinda");
        cmdPass(&c, makeCmd("PASS", "secret"), state);
        check("PASS avec NICK/USER deja poses -> 001", c.getOutBuff(),
              ":irc.42.fr 001 rinda :Welcome to IRC rinda\r\n");
        check("PASS avec NICK/USER deja poses -> enregistre",
              c.isRegistered(), true);
    }
    {
        Client c(-1);
        c.setNickname("rinda");
        c.setIsRegistered(true);
        cmdPass(&c, makeCmd("PASS", "secret"), state);
        check("PASS deja enregistre -> 462", c.getOutBuff(),
              ":irc.42.fr 462 rinda :You may not reregister\r\n");
    }
}

static void testNick()
{
    std::cout << "--- NICK ---" << std::endl;
    ServerState state("secret");

    {
        Client c(-1);
        cmdNick(&c, makeCmd("NICK"), state);
        check("NICK sans parametre -> 431", c.getOutBuff(),
              ":irc.42.fr 431 * :No nickname given\r\n");
    }
    {
        Client c(-1);
        cmdNick(&c, makeCmd("NICK", "1rinda"), state);
        check("NICK invalide -> 432", c.getOutBuff(),
              ":irc.42.fr 432 * 1rinda :Erroneous nickname\r\n");
        check("NICK invalide -> nick inchange", c.getNickname(), "");
    }
    {
        Client c(-1);
        cmdNick(&c, makeCmd("NICK", "rinda"), state);
        check("NICK libre -> aucune reponse", c.getOutBuff(), "");
        check("NICK libre -> nick pose", c.getNickname(), "rinda");
    }
    {
        // Les Client sont sur la pile : ServerState ne les delete pas
        ServerState st("secret");
        Client owner(-1);
        owner.setNickname("rinda");
        st.addClient(10, &owner);

        Client c(-1);
        st.addClient(11, &c);
        cmdNick(&c, makeCmd("NICK", "rinda"), st);
        check("NICK deja pris -> 433", c.getOutBuff(),
              ":irc.42.fr 433 * rinda :Nickname already used\r\n");
        check("NICK deja pris -> nick inchange", c.getNickname(), "");
    }
    {
        ServerState st("secret");
        Client c(-1);
        c.setNickname("rinda");
        st.addClient(10, &c);
        cmdNick(&c, makeCmd("NICK", "rinda"), st);
        check("NICK identique au sien -> pas de 433, aucune reponse",
              c.getOutBuff(), "");
    }
    {
        Client c(-1);
        c.setNickname("rinda");
        c.setUsername("rinda");
        c.setIsRegistered(true);
        cmdNick(&c, makeCmd("NICK", "rinda2"), state);
        check("NICK apres enregistrement -> message NICK", c.getOutBuff(),
              ":rinda NICK :rinda2\r\n");
        check("NICK apres enregistrement -> nick change", c.getNickname(),
              "rinda2");
    }
    {
        Client c(-1);
        c.setHasPass(true);
        c.setUsername("rinda");
        cmdNick(&c, makeCmd("NICK", "rinda"), state);
        check("NICK en dernier (PASS + USER deja faits) -> 001", c.getOutBuff(),
              ":irc.42.fr 001 rinda :Welcome to IRC rinda\r\n");
        check("NICK en dernier -> enregistre", c.isRegistered(), true);
    }
    {
        ServerState st("secret");
        Client owner(-1);
        owner.setNickname("rinda");
        st.addClient(10, &owner);

        Client c(-1);
        st.addClient(11, &c);
        cmdNick(&c, makeCmd("NICK", "RiNdA"), st);
        check("NICK pris avec une autre casse -> 433", c.getOutBuff(),
              ":irc.42.fr 433 * RiNdA :Nickname already used\r\n");
    }
    {
        ServerState st("secret");
        Client c(-1);
        c.setNickname("rinda");
        c.setUsername("rinda");
        c.setIsRegistered(true);
        st.addClient(10, &c);
        cmdNick(&c, makeCmd("NICK", "Rinda"), st);
        check("NICK changer la casse de son nick -> message NICK",
              c.getOutBuff(), ":rinda NICK :Rinda\r\n");
        check("NICK changer la casse de son nick -> nick change",
              c.getNickname(), "Rinda");
    }
    {
        Client c(-1);
        c.setUsername("rinda");
        cmdNick(&c, makeCmd("NICK", "rinda"), state);
        check("NICK + USER sans PASS -> pas enregistre", c.isRegistered(), false);
        check("NICK + USER sans PASS -> aucune reponse", c.getOutBuff(), "");
    }
}

// Client enregistre, pret a envoyer JOIN (username = nick)
static void setRegistered(Client& c, const std::string& nick = "rinda")
{
    c.setNickname(nick);
    c.setUsername(nick);
    c.setIsRegistered(true);
}

// Joue "JOIN <p1> [p2]" pour un client enregistre et renvoie sa sortie
static std::string joinOutput(const std::string& p1, const std::string& p2 = "")
{
    ServerState state("secret");
    Client c(-1);
    setRegistered(c);
    cmdJoin(&c, makeCmd("JOIN", p1, p2), state);
    return c.getOutBuff();
}

// Ce que le client recoit depuis la position "from" de son buffer de sortie
static std::string outputSince(const Client& c, std::string::size_type from)
{
    return c.getOutBuff().substr(from);
}

static std::string err403(const std::string& name)
{
    return ":irc.42.fr 403 rinda " + name + " :No such channel\r\n";
}

static std::string joinMsg(const std::string& nick, const std::string& chan)
{
    return ":" + nick + "!" + nick + "@localhost JOIN " + chan + "\r\n";
}

static std::string namesReply(const std::string& nick, const std::string& chan,
                              const std::string& names)
{
    return ":irc.42.fr 353 " + nick + " = " + chan + " :" + names + "\r\n"
         + ":irc.42.fr 366 " + nick + " " + chan + " :End of /NAMES list\r\n";
}

// Sortie complete d'un JOIN reussi, sans topic
static std::string joinOk(const std::string& nick, const std::string& chan,
                          const std::string& names)
{
    return joinMsg(nick, chan) + namesReply(nick, chan, names);
}

// Liste 353 de deux membres : Channel les stocke dans une std::map<Client*, bool>,
// donc l'ordre suit celui des adresses
static std::string twoNames(Client* a, const std::string& nameA,
                            Client* b, const std::string& nameB)
{
    if (std::less<Client*>()(a, b))
        return nameA + " " + nameB;
    return nameB + " " + nameA;
}

static void testJoin()
{
    std::cout << "--- JOIN ---" << std::endl;
    ServerState state("secret");

    // Preconditions
    {
        Client c(-1);
        cmdJoin(&c, makeCmd("JOIN", "#general"), state);
        check("JOIN non enregistre -> 451", c.getOutBuff(),
              ":irc.42.fr 451 * :You have not registered\r\n");
    }
    {
        Client c(-1);
        setRegistered(c);
        cmdJoin(&c, makeCmd("JOIN"), state);
        check("JOIN sans parametre -> 461", c.getOutBuff(),
              ":irc.42.fr 461 rinda JOIN :Not enough parameters\r\n");
    }

    // Validation du nom (canal seul)
    const std::string name50 = "#" + std::string(49, 'a');
    const std::string name51 = "#" + std::string(50, 'a');

    check("\"#general\" valide -> JOIN + 353/366",
          joinOutput("#general"), joinOk("rinda", "#general", "@rinda"));
    check("\"#a\" valide (2 caracteres) -> JOIN + 353/366",
          joinOutput("#a"), joinOk("rinda", "#a", "@rinda"));
    check("nom de 50 caracteres valide -> JOIN + 353/366",
          joinOutput(name50), joinOk("rinda", name50, "@rinda"));
    check("nom de 51 caracteres -> 403", joinOutput(name51), err403(name51));
    check("\"general\" sans '#' -> 403", joinOutput("general"), err403("general"));
    check("\"&general\" prefixe '&' -> 403", joinOutput("&general"),
          err403("&general"));
    check("\"#\" seul -> 403", joinOutput("#"), err403("#"));
    check("\"#a:b\" (':') -> 403", joinOutput("#a:b"), err403("#a:b"));
    check("\"#a\\x07b\" (BEL) -> 403", joinOutput("#a\x07" "b"),
          err403("#a\x07" "b"));

    // Decoupage sur ',' : chaque canal est traite independamment, dans l'ordre
    const std::string okA = joinOk("rinda", "#a", "@rinda");
    const std::string okB = joinOk("rinda", "#b", "@rinda");
    const std::string okC = joinOk("rinda", "#c", "@rinda");

    check("\"#a,#b,#c\" -> trois JOIN dans l'ordre",
          joinOutput("#a,#b,#c"), okA + okB + okC);
    check("\"#a,bad,#c\" -> 403 pour bad, #a et #c rejoints",
          joinOutput("#a,bad,#c"), okA + err403("bad") + okC);
    check("\"bad1,#a,bad2\" -> les deux erreurs autour du JOIN",
          joinOutput("bad1,#a,bad2"), err403("bad1") + okA + err403("bad2"));
    check("\"#a,,#b\" -> 403 pour l'element vide",
          joinOutput("#a,,#b"), okA + err403("") + okB);
    check("\"#a,\" virgule finale -> 403 pour l'element vide",
          joinOutput("#a,"), okA + err403(""));
    check("\"#a,,#b,#\" -> 403 pour \"\" puis \"#\"",
          joinOutput("#a,,#b,#"), okA + err403("") + okB + err403("#"));
    check("\"#a,#a\" -> le second JOIN est ignore",
          joinOutput("#a,#a"), okA);

    // Cles : ne doivent pas perturber le traitement
    check("\"#a,bad\" \"k1\" (moins de cles) -> #a rejoint, 403 pour bad",
          joinOutput("#a,bad", "k1"), okA + err403("bad"));
    check("\"#a\" \"k1,k2,k3\" (cles en trop ignorees) -> #a rejoint",
          joinOutput("#a", "k1,k2,k3"), okA);

    // Etat du canal et du client apres un JOIN
    {
        ServerState st("secret");
        Client rinda(-1);
        setRegistered(rinda);
        cmdJoin(&rinda, makeCmd("JOIN", "#a,#b"), st);

        Channel* a = st.getChannel("#a");
        check("JOIN cree le canal", a != NULL, true);
        if (a)
        {
            check("createur membre du canal", a->isMember(&rinda), true);
            check("createur operateur du canal", a->isOperator(&rinda), true);
            check("canal ajoute a la liste du client",
                  rinda.getChannels().count(a) == 1, true);
        }
        check("\"#a,#b\" -> client dans 2 canaux",
              rinda.getChannels().size() == 2, true);
    }

    // Plusieurs clients sur le meme canal
    {
        ServerState st("secret");
        Client rinda(-1);
        Client bob(-1);
        setRegistered(rinda, "rinda");
        setRegistered(bob, "bob");

        cmdJoin(&rinda, makeCmd("JOIN", "#a"), st);
        std::string::size_type rindaFrom = rinda.getOutBuff().size();

        cmdJoin(&bob, makeCmd("JOIN", "#a"), st);
        check("bob rejoint #a -> JOIN + liste des deux membres", bob.getOutBuff(),
              joinOk("bob", "#a", twoNames(&rinda, "@rinda", &bob, "bob")));
        check("bob rejoint #a -> rinda recoit le JOIN de bob",
              outputSince(rinda, rindaFrom), joinMsg("bob", "#a"));

        Channel* a = st.getChannel("#a");
        if (a)
        {
            check("bob membre de #a", a->isMember(&bob), true);
            check("bob pas operateur de #a", a->isOperator(&bob), false);
            check("rinda toujours operatrice de #a", a->isOperator(&rinda), true);
        }

        rindaFrom = rinda.getOutBuff().size();
        std::string::size_type bobFrom = bob.getOutBuff().size();
        cmdJoin(&rinda, makeCmd("JOIN", "#a"), st);
        check("re-JOIN de rinda -> aucune reponse pour rinda",
              outputSince(rinda, rindaFrom), "");
        check("re-JOIN de rinda -> rien pour bob", outputSince(bob, bobFrom), "");
        if (a)
            check("re-JOIN de rinda -> toujours operatrice",
                  a->isOperator(&rinda), true);
    }

    // Topic envoye au client qui rejoint
    {
        ServerState st("secret");
        Client rinda(-1);
        Client bob(-1);
        setRegistered(rinda, "rinda");
        setRegistered(bob, "bob");

        cmdJoin(&rinda, makeCmd("JOIN", "#t"), st);
        Channel* t = st.getChannel("#t");
        if (t)
            t->setTopic("Bienvenue sur #t");

        cmdJoin(&bob, makeCmd("JOIN", "#t"), st);
        check("canal avec topic -> JOIN, 332, puis 353/366", bob.getOutBuff(),
              joinMsg("bob", "#t")
              + ":irc.42.fr 332 bob #t :Bienvenue sur #t\r\n"
              + namesReply("bob", "#t", twoNames(&rinda, "@rinda", &bob, "bob")));
    }
}

int main()
{
    testPass();
    testNick();
    testJoin();

    std::cout << std::endl;
    if (g_failed == 0)
        std::cout << GREEN "Tous les tests passent" RESET << std::endl;
    else
        std::cout << RED << g_failed << " test(s) en echec" RESET << std::endl;
    return g_failed != 0;
}
