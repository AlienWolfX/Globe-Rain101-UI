#include "carrier.h"

#include <stddef.h>
#include <string.h>

static const struct {
    const char *code;
    const char *name;
} codes[] = {
    /* Australia (MCC 505) */
    {"50501", "Telstra"},
    {"50502", "Optus"},
    {"50503", "Vodafone"},

    /* Canada (MCC 302) */
    {"302220", "Telus"},
    {"302370", "Fido"},
    {"302490", "Freedom"},
    {"302610", "Bell"},
    {"302720", "Rogers"},

    /* China (MCC 460) */
    {"46000", "China Mobile"},
    {"46001", "China Unicom"},
    {"46002", "China Mobile"},
    {"46003", "China Telecom"},
    {"46007", "China Mobile"},
    {"46011", "China Telecom"},

    /* Germany (MCC 262) */
    {"26201", "Telekom"},
    {"26202", "Vodafone"},
    {"26203", "O2"},

    /* Hong Kong (MCC 454) */
    {"45400", "CSL"},
    {"45403", "3"},
    {"45406", "SmarTone"},
    {"45412", "CMHK"},

    /* India (MCC 404, 405) */
    {"40410", "Airtel"},
    {"40411", "Vi"},
    {"40445", "Airtel"},
    {"405840", "Jio"},
    {"405854", "Jio"},
    {"405872", "Jio"},

    /* Indonesia (MCC 510) */
    {"51001", "Indosat"},
    {"51010", "Telkomsel"},
    {"51011", "XL"},
    {"51089", "3"},

    /* Japan (MCC 440) */
    {"44010", "NTT Docomo"},
    {"44020", "SoftBank"},
    {"44050", "au (KDDI)"},
    {"44051", "au (KDDI)"},
    {"44011", "Rakuten"},

    /* Malaysia (MCC 502) */
    {"50212", "Maxis"},
    {"50213", "Celcom"},
    {"50216", "DiGi"},
    {"50218", "U Mobile"},
    {"50219", "Celcom"},

    /* Philippines (MCC 515) */
    {"51501", "Globe"},
    {"51502", "Globe"},
    {"51503", "Smart"},
    {"51505", "Sun"},
    {"51511", "PLDT"},
    {"51518", "Cure"},
    {"51566", "DITO"},
    {"51588", "Next"},

    /* Singapore (MCC 525) */
    {"52501", "Singtel"},
    {"52502", "Singtel"},
    {"52503", "M1"},
    {"52505", "StarHub"},
    {"52510", "SIMBA"},

    /* South Korea (MCC 450) */
    {"45005", "SK Telecom"},
    {"45006", "LG U+"},
    {"45008", "KT"},

    /* Taiwan (MCC 466) */
    {"46601", "FarEasTone"},
    {"46692", "Chunghwa"},
    {"46697", "Taiwan Mobile"},

    /* Thailand (MCC 520) */
    {"52000", "TrueMove H"},
    {"52001", "AIS"},
    {"52003", "AIS"},
    {"52004", "TrueMove H"},
    {"52005", "dtac"},
    {"52018", "dtac"},

    /* United Arab Emirates (MCC 424) */
    {"42402", "Etisalat"},
    {"42403", "du"},

    /* United Kingdom (MCC 234) */
    {"23410", "O2"},
    {"23415", "Vodafone"},
    {"23420", "3"},
    {"23430", "EE"},

    /* United States (MCC 310, 311, 312) */
    {"310120", "Sprint"},
    {"310260", "T-Mobile"},
    {"310410", "AT&T"},
    {"311480", "Verizon"},
    {"312530", "Sprint"}
};

#define CODE_COUNT (sizeof(codes) / sizeof(codes[0]))

const char *carrier_for_plmn(const char *plmn) {
    size_t length;

    if (plmn == NULL || plmn[0] == '\0') {
        return "";
    }
    length = strlen(plmn);

    for (size_t i = 0; i < CODE_COUNT; ++i) {
        if (strcmp(plmn, codes[i].code) == 0) {
            return codes[i].name;
        }
    }

    /* Modems differ on 2- vs 3-digit MNCs (31026 vs 310260, 505010 vs 50501):
     * accept a code that is a prefix of the PLMN or the other way round. */
    if (length >= 5) {
        for (size_t i = 0; i < CODE_COUNT; ++i) {
            size_t code_length = strlen(codes[i].code);
            size_t common = code_length < length ? code_length : length;
            if (code_length != length && strncmp(plmn, codes[i].code, common) == 0) {
                return codes[i].name;
            }
        }
    }
    return "";
}
