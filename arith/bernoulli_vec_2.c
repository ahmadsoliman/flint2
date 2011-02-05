/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2011 Fredrik Johansson

******************************************************************************/

#include <stdio.h>
#include <mpfr.h>
#include <mpir.h>
#include "flint.h"
#include "fmpz.h"
#include "fmpz_poly.h"
#include "fmpq_poly.h"
#include "fmpz_vec.h"
#include "arith.h"

/*
  Recurrence data (characteristic polynomial + initial values) for
  multisectioning the series expansion of $x / (e^x - 1)$ into $2m$
  independent numerator series (half of which are trivial)
  divided by a common denominator series. Here we use $m = 9$.

  Coefficients generated using a Mathematica program
  written by Manuel Kauers.
*/

#define BERN_SECTIONS 9

static const char *BERN_M9_CHARPOLY_0[] =
{
    "40341068970691068873250369779249",
    "-3539368167414875001687358847373613344",
    "7502704469280714345880078228505321482668",
    "-75435569485001553007378071931396671421732",
    "-591042510119480582783689336745870997402",
    "-71206774486953330106554594126836210028",
    "335228074382127548801999211802764834",
    "83207493096236179213563949580676",
    "8263730251874088428584262661",
    "-6482799949471047358968",
    "3750359307037686",
    "-205169364",
    "1",
    NULL
};

static const char *BERN_M9_CHARPOLY_2[] =
{
    "2049538635913786967090909403",
    "-379538337720454837872022161349741692",
    "32927134666325862971385131962182688604232",
    "-39263287784805591099522365879326510518368987",
    "-65310610605614654651260608142195149460827098082",
    "-1502026265820444142890092623460092618691487801",
    "-73640766728007619589627757920467898662868752",
    "-691967051800682522037458729464190320230897",
    "168058421381549461592239700071773808062",
    "62080841008908914971447792438312875",
    "7270554951937357550933028023646",
    "-60628070429392737472676091",
    "-411659094147267884404",
    "308574089329251",
    "-186750615",
    "1",
    NULL
};

static const char *BERN_M9_CHARPOLY_5[] =
{
    "-794033260550112308632187028364958067",
    "147040936425731908517444975695355948647244790",
    "-12758133966068428110667529974815214273439646955707",
    "15340438528090861863137145816079268140663556233338618",
    "25148801319215390526320362854810089442838156833733992084",
    "-255359586260663497636908523508637497192699702422505222568",
    "-4662013612297628549821189033619541830742396405432009479",
    "-260820641253448155579933838200880766900501471805495840",
    "-1363609599654438157603590526054636877856509968909413",
    "13326599058042658348066418471338885598534193182450",
    "-2834118410703579141768539765136319168860186915",
    "-1107877094205894343638694312796302775986950",
    "-133280301571128499961258562333535134105",
    "1115598268398541014998567923009878",
    "7476979415990790644956738386",
    "-6061573126818195238930",
    "3727491921962040",
    "-205057992",
    "1",
    NULL
};


static const char *BERN_M9_INITIAL_DENOM[] =
{
    "3265920",
    "420716289708000",
    "75741628207941793959840",
    "13980827630393894670831845845200",
    "2587079681708018226891969425300762175360",
    "478843906754989274913087275436386477269070250720",
    "88631630407118383608663290935993372328168433536798023776",
    "16405314518533886679174634665827619362699620128510150063984938400",
    "3036550355154853956593930215133004274771291436075454001944943921497669120",
    "562051905008608996508409812964263360163229995972865417036965838714036843889632080",
    "104033296904263196505732112016519358654528982053147668974992492887844343195478328378218976",
    "19256098539703660325938618466008059986785127232904971956981990625309623260966523031177798853092240",
    NULL
};

static const char *BERN_M9_INITIAL_NUMER_0[] =
{
    "362880",
    "46746254412000",
    "8415736467549088217760",
    "1553425292265988296759093982800",
    "287453297967557580765774380588973575040",
    "53204878528332141657009697270709608585452250080",
    "9847958934124264845407032326221485814240937059644224864",
    "1822812724281542964352737185091957706966624458723350007109437600",
    "337394483906094884065992246125889363863476826230606000216104880166407680",
    "62450211667623221834267756996029262240358888441429490781885093190448538209959120",
    "11559255211584799611748012446279928739392109117016407663888054765316038132830925375357664",
    "2139566504411517813993179829556451109642791914767219106331332291701069251218502559019755428121360",
    NULL
};

static const char *BERN_M9_INITIAL_NUMER_1[] =
{
    "2721600",
    "384888437125920",
    "69725460471500036340720",
    "12878343113270238160559369547264",
    "2383218610440983634215548977656532759360",
    "441113867993834676869514272707427543450876885280",
    "81648037149162692988299756259570482209766342679621032544",
    "15112683853979968063466916015425029989046357683396081129643059840",
    "2797290216244233335108453465989460374513396532518343001231297894873750000",
    "517765922490943460216338215578361893602453341376451679366372090820609537809156384",
    "95836159373428723268508118785008559357754887377168990530331545918441676040083579743591440",
    "17738844999504043163204748981020751848216402020576214919551308168845442416588311616423328678316544",
    NULL
};

static const char *BERN_M9_INITIAL_NUMER_2[] =
{
    "-5987520",
    "-1082725674030000",
    "-200292170975897484396096",
    "-37072989911805581808190116000000",
    "-6862044786116036528226335960931952541184",
    "-1270133837557842921119962111796110714152926254800",
    "-235096099644324857691621315305877852985954741680588183456",
    "-43515237869776461691612391391437678081581579742766244901025762720",
    "-8054476147107433532674489418034287297553460259084750148484521593045332320",
    "-1490847555480819396968585079508738754363789482768666488712835534218894234740027528",
    "-275949222902762750581290304746033750287399137394397241805297698708186650039110389030697472",
    "-51076968493991892398740820834932570974049575785846492165427560064680595615633119574062756312767680",
    "-9454118707395428699695226748519293891740923662927493985604201064828021689540385481713578067106704315383904",
    "-1749915141186147138795481290341955968451131567499175064846751452527570068894764564514109291807147935717792848432472",
    "-323901475761790784854049667121983618819124338353902564702456406039538352384524566984813283842121648393341962424147628680832",
    NULL
};

static const char *BERN_M9_INITIAL_NUMER_3[] =
{
    "25945920",
    "5732564274547200",
    "1081514562285057099091200",
    "200576269278407968143857422008192",
    "37132985968409579883740477642253059760960",
    "6873281904324980421614369217398797718619129234432",
    "1272216195385076370839779066732813668489975117776931270912",
    "235481579039260387240325702425475790849982594044475543152802255680",
    "43586589194187045914735580446614047506071748697751757370990010998556685760",
    "8067682973364554316692152977732174672266832781679514453641682974999751102669722880",
    "1493292080297702274417577502768429695609695487430280103059191827033103512852597781485148160",
    "276401693531646279073500003010508581957866372016259369675535944678367704474571737832484539293583552",
    NULL
};

static const char *BERN_M9_INITIAL_NUMER_4[] =
{
    "-155675520",
    "-38330603704261440",
    "-7302430572983028894771360",
    "-1355516410647663980436479162564160",
    "-250970903211323360539243882904979284879616",
    "-46454890437678191832816441772909411625147903137680",
    "-8598616456346445465854772080179467809069463336011803134432",
    "-1591565930847229269025839522557960504254666616035138144157475005040",
    "-294591752234032997147877260479525638571376465728021348395055381719453442560",
    "-54527617542747276954350200445176759346715605536168965916204002563402119609671057840",
    "-10092818434936918525773848408791542264104934437585289901214089895709257192080653558488847744",
    "-1868135607730931195414147044945168569914072816066204455608611764683251483779788978117279624352700064",
    "-345783556029244348154841746690483173381175580226535090911940996356704681311902126279947700294055999453564736",
    NULL
};

static const char *BERN_M9_INITIAL_NUMER_5[] =
{
    "1202947200",
    "299276521055487360",
    "56907219749360734456466400",
    "10560915236494378504705783273368768",
    "1955284090309375603199093269431307310720400",
    "361923611364440910083816982072591945051772089845136",
    "66990613284180951993948261835443325491227735053860365721000",
    "12399666375479959476607658431386541678906368661252017439832098245504",
    "2295122913605329323597678465591271786747747769439018359158093230190575798240",
    "424817000033408643032707315565442296016084610533953392036544344453636593335305992480",
    "78631729067486651670037942584355807890525898437756081032074927303615376750973110727390666768",
    "14554381802769394285925252780128047939509661768715101954230872018013778677361775089819728995740021888",
    "2693951057268586236051456784136786548499165797213201178056464776296535848141245260693989738348915012254590568",
    "498638306819882538938573909986759690910892530891027900352409405837349047166798952493169125763047106367581280080527440",
    "92295723174809503057781789758464695652888994873898024140473583199249456721987629716587168797576981181066531407336717057783520",
    "17083526074618112758071028805687015799376416707299837149987009986436903846151397453118640448645055002986921243476854399811669477961728",
    "3162084364292741334710733016675015305760853949969680063462532750106442133549084406615022254390742922470445080209524280034148079628326003869120",
    "585287690798232469095630444143378919078586052259668062623619453355362911508599118586975133372195755737336929506829890762525950417845750857891111380320",
    "108334137086360637552494699294482462003510176533322677708533995402232933039902543159435918893220139080617434984466804982576292673347498937143288178373372461904",
    NULL
};

static const char *BERN_M9_INITIAL_NUMER_6[] =
{
    "-11570325120",
    "-2667902039859528000",
    "-502597501561389215830907520",
    "-93187703394606233139413254916457600",
    "-17251540486435878647155490421197706513229056",
    "-3193236410167036809098740641184878996919738424520000",
    "-591054771638258217387286102638474248224608648940330295869440",
    "-109401615861674467368617773906716362367590882247328272263347673111040",
    "-20249750726136873724710610648792296346947252668867427509222529187727368454976",
    "-3748138409752146350078806035586802649897115004741923197239275120426293978416309273280",
    "-693763676818734932462620912469911727137225993922495995172796089954470279642039437780130776960",
    "-128412557538482340516867492961138118085326136782797967416953653322733488189557304360262214083073799936",
    NULL
};

static const char *BERN_M9_INITIAL_NUMER_7[] =
{
    "135377827200",
    "27448087237645314240",
    "5117253380897086222319458800",
    "947846275532367408364866169677662880",
    "175454300448799135432231038150961647675969120",
    "32476030428167121103379778859774689846781938747579264",
    "6011172247445381053050275329952148333583261553528593669944000",
    "1112641205889243489395320626810834798235624323609341255462237298043200",
    "205944918410614386306247298125087314769643182879009209921201552267892383688080",
    "38119484466824746372398164136570482485609611917811607773983236541008611453787785305664",
    "7055746295513007618979859551305873399717879738197213055779436799932478351104340709443883544720",
    "1305987121285813236888681828449530543817362480650277268346223171152779197266294968950744595909144883840",
    NULL
};

static const char *BERN_M9_INITIAL_NUMER_8[] =
{
    "-1892809550016",
    "-351445830266181924000",
    "-65051756300336643217182836880",
    "-12040787817092642667785073708838732800",
    "-2228696034085553726593386957079933204714473600",
    "-412521680318863990063305249107362756991656361164833360",
    "-76355920292484325778353608478051128825620909650181845652490240",
    "-14133139764206730736262360481380110191711118719885472367769697275670800",
    "-2615981037612640769258369334434058050905116020221736691257749425490523908754304",
    "-484206404473564946898732261464524015447596425096123695896023115397931432624353995707160",
    "-89624442517818598703770402573195614862626398569039651061129713133683393544174723794567507280064",
    "-16589083957621018033348551137767133772183926681730075383620794824829861625658926321106470464228645848760",
    "-3070565337109749351040944159418563265868545906071888704294054721747284369053620304209705346198480756783118482176",
    "-568347927682198441648061492409473989947231252024079201196364184004088681155819411742546975449853579425376740354086106880",
    "-105198662603512606753854067072225138866989789498497068998788778390659416921721487205997750431536919720911525626491564132994025760",
    NULL
};

static const char ** BERN_M9_CHARPOLY[] =
{
    BERN_M9_CHARPOLY_0,
    BERN_M9_CHARPOLY_0,
    BERN_M9_CHARPOLY_2,
    BERN_M9_CHARPOLY_0,
    BERN_M9_CHARPOLY_0,
    BERN_M9_CHARPOLY_5,
    BERN_M9_CHARPOLY_0,
    BERN_M9_CHARPOLY_0,
    BERN_M9_CHARPOLY_2,
    BERN_M9_CHARPOLY_0,
};

static const char ** BERN_M9_INITIAL[] =
{
    BERN_M9_INITIAL_NUMER_0,
    BERN_M9_INITIAL_NUMER_1,
    BERN_M9_INITIAL_NUMER_2,
    BERN_M9_INITIAL_NUMER_3,
    BERN_M9_INITIAL_NUMER_4,
    BERN_M9_INITIAL_NUMER_5,
    BERN_M9_INITIAL_NUMER_6,
    BERN_M9_INITIAL_NUMER_7,
    BERN_M9_INITIAL_NUMER_8,
    BERN_M9_INITIAL_DENOM,
};

/*
With k = m, gives n nonzero coefficients of denominator series
With k < m, gives n nonzero coefficients of k:th nontrivial numerator series
*/
void bernoulli_series_section(fmpq_poly_t A, long n, long k)
{
    const long m = BERN_SECTIONS;
    long i, j, deg;
    fmpz_t t, u;
    fmpz * charpoly;

    fmpz_init(t);
    fmpz_init(u);

    for (deg = 0; BERN_M9_CHARPOLY[k][deg] != NULL; deg++);
    deg -= 1;

    charpoly = _fmpz_vec_init(deg + 1);

    fmpq_poly_fit_length(A, n);
    _fmpq_poly_set_length(A, n);
    fmpz_set_ui(fmpq_poly_denref(A), 1UL);

    /* Initial values */
    for (i = 0; i < deg && i < n; i++)
    {
        fmpz_set_str(A->coeffs + i, (char *) BERN_M9_INITIAL[k][i], 10);
        fmpz_set_str(charpoly + i, (char *) BERN_M9_CHARPOLY[k][i], 10);
    }

    /* C-finite recurrence */
    for (i = deg; i < n; i++)
    {
        fmpz_zero(A->coeffs + i);
        for (j = 0; j < deg; j++)
            fmpz_submul(A->coeffs + i, A->coeffs + i - deg + j, charpoly + j);
    }

    /* Factorials */
    fmpz_set_ui(t, 1UL);
    for (i = n - 2; i >= 0; i--)
    {
        fmpz_set_ui(u, 1UL);
        if (k == m) /* Denominator */
            for (j = 0; j < 2*m; j++)
                fmpz_mul_ui(u, u, 2*i*m + 3*m + 1 - j - 1);
        else        /* Numerator */
            for (j = 0; j < 2*m; j++)
                fmpz_mul_ui(u, u, 2*i*m + 3*m + 2*k - j - 1);

        fmpz_mul(t, t, u);
        fmpz_mul(A->coeffs + i, A->coeffs + i, t);
    }

    fmpz_fac_ui(u, (k == m) ? m : (m + 2*k - 1));
    fmpz_mul(t, t, u);
    fmpz_set(fmpq_poly_denref(A), t);

    _fmpz_vec_clear(charpoly, deg + 1);
    fmpz_clear(t);
    fmpz_clear(u);
}

static __inline__ void _fmpz_get_mpfr(mpfr_t f, const fmpz_t x)
{
    fmpz v = *x;
    if (COEFF_IS_MPZ(v))
        mpfr_set_z(f, COEFF_TO_PTR(v), GMP_RNDD);
    else
        mpfr_set_si(f, v, GMP_RNDD);
}

static __inline__ void _mpfr_mul_fmpz(mpfr_t f, mpfr_t g, const fmpz_t x)
{
    fmpz v = *x;
    if (COEFF_IS_MPZ(v))
        mpfr_mul_z(f, g, COEFF_TO_PTR(v), GMP_RNDD);
    else
        mpfr_mul_si(f, g, v, GMP_RNDD);
}

static __inline__ void _fmpz_set_mpfr(fmpz_t x, mpfr_t f)
{
    mpz_t t;
    mpz_init(t);
    mpfr_get_z(t, f, GMP_RNDN);
    fmpz_set_mpz(x, t);
    mpz_clear(t);
}


void bernoulli_vec_series_multisectioned(fmpz * num, fmpz * den, long len)
{
    const long m = BERN_SECTIONS;
    long n, i, j, k;
    fmpq_poly_t section;
    fmpq_poly_t numpoly;
    fmpq_poly_t denpoly;
    fmpz_t fac, u;

    if (!len)
        return;

    n = (len + (2*m) - 1) / (2*m);

    fmpq_poly_init2(section, n);
    fmpq_poly_init2(numpoly, n);
    fmpq_poly_init2(denpoly, n);

    bernoulli_series_section(denpoly, n, m);
    fmpq_poly_inv_series(denpoly, denpoly, n);

    fmpz_init(fac);
    fmpz_init(u);

    /* Trivial values */
    if (len > 1)
    {
        fmpz_set_si(num + 1, -1L);
        fmpz_set_ui(den + 1, 2UL);
    }
    for (k = 3; k < len; k++)
    {
        fmpz_zero(num + k);
        fmpz_set_ui(den + k, 1UL);
    }

    for (k = 0; k < m; k++)
    {
        mpfr_t section_den_inv;
        long maxprec;

        bernoulli_series_section(numpoly, n, k);

        /* Important: avoid canonicalising here, which is slow */
        _fmpq_poly_mullow(section->coeffs, section->den,
                          numpoly->coeffs, numpoly->den, n,
                          denpoly->coeffs, denpoly->den, n, n);
        _fmpq_poly_set_length(section, n);

        /* Estimate size of largest numerator (this is too high...) */
        maxprec = fmpz_bits(section->coeffs + (n - 1)) + 8;

        mpfr_init2(section_den_inv, maxprec);
        _fmpz_get_mpfr(section_den_inv, section->den);
        mpfr_ui_div(section_den_inv, 1UL, section_den_inv, GMP_RNDD);

        fmpz_fac_ui(fac, 2*k);
        for (j = 0; j < n; j++)
        {
            mpfr_t xx, yy;
            long prec;
            long r = 2*k + 2*m*j;

            if (r >= len)
                break;

            fmpz_bernoulli_denom(den + r, r);

            /* Knowing the denominator, we can compute the numerator
               more quickly than by performing a GCD and a divexact:
               we use floating-point arithmetic with just enough
               precision to represent the numerator (plus a few guard
               bits) and round to the nearest integer. */

            /* Size of numerator of B_r, plus guard bits */
            prec = fmpz_bits(section->coeffs + j) + fmpz_bits(fac) - \
                fmpz_bits(section->den) + fmpz_bits(den + r) + 8;

            mpfr_init2(xx, prec);
            mpfr_init2(yy, prec);
            _fmpz_get_mpfr(xx, section->coeffs + j);
            _mpfr_mul_fmpz(xx, xx, den + r);
            /* --- mpfr_mul(xx, xx, section_den_inv, GMP_RNDD); */
            mpfr_set(yy, section_den_inv, GMP_RNDD);
            mpfr_mul(xx, xx, yy, GMP_RNDD);
            /* --- */
            _fmpz_get_mpfr(yy, fac);
            mpfr_mul(xx, xx, yy, GMP_RNDD);
            mpfr_round(xx, xx);
            _fmpz_set_mpfr(num + r, xx);
            mpfr_clear(xx);
            mpfr_clear(yy);

            /* Next factorial */
            fmpz_set_ui(u, 1UL);
            for (i = r + 1; i <= r + 2*m; i++)
                fmpz_mul_ui(u, u, i);
            fmpz_mul(fac, fac, u);
        }

        mpfr_clear(section_den_inv);
    }

    fmpz_clear(fac);
    fmpz_clear(u);

    fmpq_poly_clear(section);
    fmpq_poly_clear(numpoly);
    fmpq_poly_clear(denpoly);
}

void fmpz_bernoulli_vec_2(fmpz * num, fmpz * den, long len)
{
    bernoulli_vec_series_multisectioned(num, den, len);
}
