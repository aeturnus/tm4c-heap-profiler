#include <stdio.h>t
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <malloc.h>
#include <stdint.h>

#include <Random.h>

#include <btn/vector.h>
#include <btn/tokenizer.h>
#include <btn/bst.h>
#include <btn/map.h>

void benchmark_vector(uint32_t actions)
{
    printf("Pushing %d uint32_t's into a vector\n", actions);
    vector(uint32_t) v;

    vector_ctor(&v, sizeof(uint32_t), NULL, NULL);

    for (uint32_t i = 0; i < actions; ++i) {
        uint32_t val = Random();
        vector_push_back(&v, &val);
    }

    uint32_t * arr = vector_to_array(&v);

    free(arr);
    vector_dtor(&v);
}

void benchmark_fixed(uint32_t size, uint32_t actions)
{
    printf("Allocating %dB objects %d times\n", size, actions);
    void ** ptrs = (void **) malloc(sizeof(void *) * actions);

    for (uint32_t i = 0; i < actions; ++i) {
        ptrs[i] = (void *) malloc(size);
    }

    for (uint32_t i = 0; i < actions; ++i) {
        free(ptrs[i]);
    }

    free(ptrs);
}

const char * lorem = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
// http://www.perseus.tufts.edu/hopper/text?doc=Perseus:text:1999.02.0055
const char * aenied =
"Arma virumque cano, Troiae qui primus ab oris\nItaliam, fato profugus, Laviniaque venit\nlitora, multum ille et terris iactatus et alto\nvi superum saevae memorem Iunonis ob iram;\n5multa quoque et bello passus, dum conderet urbem,\ninferretque deos Latio, genus unde Latinum,\nAlbanique patres, atque altae moenia Romae.\n"
"Musa, mihi causas memora, quo numine laeso,\nquidve dolens, regina deum tot volvere casus\ninsignem pietate virum, tot adire labores\nimpulerit. Tantaene animis caelestibus irae?\n"
"Urbs antiqua fuit, Tyrii tenuere coloni,\nKarthago, Italiam contra Tiberinaque longe\nostia, dives opum studiisque asperrima belli;\nquam Iuno fertur terris magis omnibus unam\nposthabita coluisse Samo; hic illius arma,\nhic currus fuit; hoc regnum dea gentibus esse,\nsi qua fata sinant, iam tum tenditque fovetque.\nProgeniem sed enim Troiano a sanguine duci\naudierat, Tyrias olim quae verteret arces;\nhinc populum late regem belloque superbum\nventurum excidio Libyae: sic volvere Parcas.\nId metuens, veterisque memor Saturnia belli,\nprima quod ad Troiam pro caris gesserat Argis—\nnecdum etiam causae irarum saevique dolores\nexciderant animo: manet alta mente repostum\niudicium Paridis spretaeque iniuria formae,\net genus invisum, et rapti Ganymedis honores.\nHis accensa super, iactatos aequore toto\nTroas, reliquias Danaum atque immitis Achilli,\narcebat longe Latio, multosque per annos\nerrabant, acti fatis, maria omnia circum.\nTantae molis erat Romanam condere gentem!\n"
"Vix e conspectu Siculae telluris in altum\nvela dabant laeti, et spumas salis aere ruebant,\ncum Iuno, aeternum servans sub pectore volnus,\nhaec secum: \"Mene incepto desistere victam,\nnec posse Italia Teucrorum avertere regem?\nQuippe vetor fatis. Pallasne exurere classem\nArgivum atque ipsos potuit submergere ponto,\nunius ob noxam et furias Aiacis Oilei?\nIpsa, Iovis rapidum iaculata e nubibus ignem,\ndisiecitque rates evertitque aequora ventis,\nillum expirantem transfixo pectore flammas\nturbine corripuit scopuloque infixit acuto.\nAst ego, quae divom incedo regina, Iovisque\net soror et coniunx, una cum gente tot annos\nbella gero! Et quisquam numen Iunonis adoret\npraeterea, aut supplex aris imponet honorem?\"\n"
"Talia flammato secum dea corde volutans\nnimborum in patriam, loca feta furentibus austris,\nAeoliam venit. Hic vasto rex Aeolus antro\nluctantes ventos tempestatesque sonoras\nimperio premit ac vinclis et carcere frenat.\nIlli indignantes magno cum murmure montis\ncircum claustra fremunt; celsa sedet Aeolus arce\nsceptra tenens, mollitque animos et temperat iras.\nNi faciat, maria ac terras caelumque profundum\nquippe ferant rapidi secum verrantque per auras.\nSed pater omnipotens speluncis abdidit atris,\nhoc metuens, molemque et montis insuper altos\nimposuit, regemque dedit, qui foedere certo\net premere et laxas sciret dare iussus habenas.\nAd quem tum Iuno supplex his vocibus usa est:\n"
"\"Aeole, namque tibi divom pater atque hominum rex\net mulcere dedit fluctus et tollere vento,\ngens inimica mihi Tyrrhenum navigat aequor,\nIlium in Italiam portans victosque Penates:\nincute vim ventis submersasque obrue puppes,\naut age diversos et disiice corpora ponto.\nSunt mihi bis septem praestanti corpore nymphae,\nquarum quae forma pulcherrima Deiopea,\nconubio iungam stabili propriamque dicabo,\nomnis ut tecum meritis pro talibus annos\nexigat, et pulchra faciat te prole parentem.\"\n"
"Aeolus haec contra: \"Tuus, O regina, quid optes\nexplorare labor; mihi iussa capessere fas est.\nTu mihi, quodcumque hoc regni, tu sceptra Iovemque\nconcilias, tu das epulis accumbere divom,\nnimborumque facis tempestatumque potentem.\"\n"
"Haec ubi dicta, cavum conversa cuspide montem\nimpulit in latus: ac venti, velut agmine facto,\nqua data porta, ruunt et terras turbine perflant.\nIncubuere mari, totumque a sedibus imis\nuna Eurusque Notusque ruunt creberque procellis\nAfricus, et vastos volvunt ad litora fluctus.\nInsequitur clamorque virum stridorque rudentum.\nEripiunt subito nubes caelumque diemque\nTeucrorum ex oculis; ponto nox incubat atra.\nIntonuere poli, et crebris micat ignibus aether,\npraesentemque viris intentant omnia mortem.\nExtemplo Aeneae solvuntur frigore membra:\ningemit, et duplicis tendens ad sidera palmas\ntalia voce refert: \"O terque quaterque beati,\n95quis ante ora patrum Troiae sub moenibus altis\ncontigit oppetere! O Danaum fortissime gentis\nTydide! Mene Iliacis occumbere campis\nnon potuisse, tuaque animam hanc effundere dextra,\nsaevus ubi Aeacidae telo iacet Hector, ubi ingens\nSarpedon, ubi tot Simois correpta sub undis\nscuta virum galeasque et fortia corpora volvit?\"\n"
;
void benchmark_tokenize(void)
{
    puts("Tokenizing first 101 lines of \"The Aenied\" and copying strings to array");
    tokenizer tok;
    tokenizer_ctor(&tok, aenied, " ");
    
    char ** tokens = tokenizer_tokens_copy(&tok);
    size_t num_tok = tokenizer_num_tokens(&tok);
    for (size_t i = 0; i < num_tok; ++i) {
        free(tokens[i]);
    }
    free(tokens);

    tokenizer_dtor(&tok);
}

void benchmark_tokenize_bst(void)
{
    puts("Tokenizing first 101 lines of \"The Aenied\" and adding them to a BST");
    tokenizer tok;
    bst(const char *, uint32_t) map;

    tokenizer_ctor(&tok, aenied, " ");
    bst_ctor(&map, sizeof(char *), sizeof(uint32_t), NULL, NULL, strcmp);
 
    uint32_t n = 0;
    const char * str = NULL;
    while (tokenizer_has_tokens(&tok)) {
        str = tokenizer_next(&tok);
        map_insert(&map, &str, &n);
        ++n;
    }

    bst_dtor(&map);
    tokenizer_dtor(&tok);
}