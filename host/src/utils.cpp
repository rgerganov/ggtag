#include "utils.h"
#include <strings.h>

struct FaIcon {
    const char *name;
    int codepoint;
};

static FaIcon icons[] = {
    {"ad", 0xf641},
    {"address-book", 0xf2b9},
    {"address-card", 0xf2bb},
    {"adjust", 0xf042},
    {"air-freshener", 0xf5d0},
    {"align-center", 0xf037},
    {"align-justify", 0xf039},
    {"align-left", 0xf036},
    {"align-right", 0xf038},
    {"allergies", 0xf461},
    {"ambulance", 0xf0f9},
    {"american-sign-language-interpreting", 0xf2a3},
    {"anchor", 0xf13d},
    {"angle-double-down", 0xf103},
    {"angle-double-left", 0xf100},
    {"angle-double-right", 0xf101},
    {"angle-double-up", 0xf102},
    {"angle-down", 0xf107},
    {"angle-left", 0xf104},
    {"angle-right", 0xf105},
    {"angle-up", 0xf106},
    {"angry", 0xf556},
    {"ankh", 0xf644},
    {"apple-alt", 0xf5d1},
    {"archive", 0xf187},
    {"archway", 0xf557},
    {"arrow-alt-circle-down", 0xf358},
    {"arrow-alt-circle-left", 0xf359},
    {"arrow-alt-circle-right", 0xf35a},
    {"arrow-alt-circle-up", 0xf35b},
    {"arrow-circle-down", 0xf0ab},
    {"arrow-circle-left", 0xf0a8},
    {"arrow-circle-right", 0xf0a9},
    {"arrow-circle-up", 0xf0aa},
    {"arrow-down", 0xf063},
    {"arrow-left", 0xf060},
    {"arrow-right", 0xf061},
    {"arrow-up", 0xf062},
    {"arrows-alt", 0xf0b2},
    {"arrows-alt-h", 0xf337},
    {"arrows-alt-v", 0xf338},
    {"assistive-listening-systems", 0xf2a2},
    {"asterisk", 0xf069},
    {"at", 0xf1fa},
    {"atlas", 0xf558},
    {"atom", 0xf5d2},
    {"audio-description", 0xf29e},
    {"award", 0xf559},
    {"baby", 0xf77c},
    {"baby-carriage", 0xf77d},
    {"backspace", 0xf55a},
    {"backward", 0xf04a},
    {"bacon", 0xf7e5},
    {"bacteria", 0xe059},
    {"bacterium", 0xe05a},
    {"bahai", 0xf666},
    {"balance-scale", 0xf24e},
    {"balance-scale-left", 0xf515},
    {"balance-scale-right", 0xf516},
    {"ban", 0xf05e},
    {"band-aid", 0xf462},
    {"barcode", 0xf02a},
    {"bars", 0xf0c9},
    {"baseball-ball", 0xf433},
    {"basketball-ball", 0xf434},
    {"bath", 0xf2cd},
    {"battery-empty", 0xf244},
    {"battery-full", 0xf240},
    {"battery-half", 0xf242},
    {"battery-quarter", 0xf243},
    {"battery-three-quarters", 0xf241},
    {"bed", 0xf236},
    {"beer", 0xf0fc},
    {"bell", 0xf0f3},
    {"bell-slash", 0xf1f6},
    {"bezier-curve", 0xf55b},
    {"bible", 0xf647},
    {"bicycle", 0xf206},
    {"biking", 0xf84a},
    {"binoculars", 0xf1e5},
    {"biohazard", 0xf780},
    {"birthday-cake", 0xf1fd},
    {"blender", 0xf517},
    {"blender-phone", 0xf6b6},
    {"blind", 0xf29d},
    {"blog", 0xf781},
    {"bold", 0xf032},
    {"bolt", 0xf0e7},
    {"bomb", 0xf1e2},
    {"bone", 0xf5d7},
    {"bong", 0xf55c},
    {"book", 0xf02d},
    {"book-dead", 0xf6b7},
    {"book-medical", 0xf7e6},
    {"book-open", 0xf518},
    {"book-reader", 0xf5da},
    {"bookmark", 0xf02e},
    {"border-all", 0xf84c},
    {"border-none", 0xf850},
    {"border-style", 0xf853},
    {"bowling-ball", 0xf436},
    {"box", 0xf466},
    {"box-open", 0xf49e},
    {"box-tissue", 0xe05b},
    {"boxes", 0xf468},
    {"braille", 0xf2a1},
    {"brain", 0xf5dc},
    {"bread-slice", 0xf7ec},
    {"briefcase", 0xf0b1},
    {"briefcase-medical", 0xf469},
    {"broadcast-tower", 0xf519},
    {"broom", 0xf51a},
    {"brush", 0xf55d},
    {"bug", 0xf188},
    {"building", 0xf1ad},
    {"bullhorn", 0xf0a1},
    {"bullseye", 0xf140},
    {"burn", 0xf46a},
    {"bus", 0xf207},
    {"bus-alt", 0xf55e},
    {"business-time", 0xf64a},
    {"calculator", 0xf1ec},
    {"calendar", 0xf133},
    {"calendar-alt", 0xf073},
    {"calendar-check", 0xf274},
    {"calendar-day", 0xf783},
    {"calendar-minus", 0xf272},
    {"calendar-plus", 0xf271},
    {"calendar-times", 0xf273},
    {"calendar-week", 0xf784},
    {"camera", 0xf030},
    {"camera-retro", 0xf083},
    {"campground", 0xf6bb},
    {"candy-cane", 0xf786},
    {"cannabis", 0xf55f},
    {"capsules", 0xf46b},
    {"car", 0xf1b9},
    {"car-alt", 0xf5de},
    {"car-battery", 0xf5df},
    {"car-crash", 0xf5e1},
    {"car-side", 0xf5e4},
    {"caravan", 0xf8ff},
    {"caret-down", 0xf0d7},
    {"caret-left", 0xf0d9},
    {"caret-right", 0xf0da},
    {"caret-square-down", 0xf150},
    {"caret-square-left", 0xf191},
    {"caret-square-right", 0xf152},
    {"caret-square-up", 0xf151},
    {"caret-up", 0xf0d8},
    {"carrot", 0xf787},
    {"cart-arrow-down", 0xf218},
    {"cart-plus", 0xf217},
    {"cash-register", 0xf788},
    {"cat", 0xf6be},
    {"certificate", 0xf0a3},
    {"chair", 0xf6c0},
    {"chalkboard", 0xf51b},
    {"chalkboard-teacher", 0xf51c},
    {"charging-station", 0xf5e7},
    {"chart-area", 0xf1fe},
    {"chart-bar", 0xf080},
    {"chart-line", 0xf201},
    {"chart-pie", 0xf200},
    {"check", 0xf00c},
    {"check-circle", 0xf058},
    {"check-double", 0xf560},
    {"check-square", 0xf14a},
    {"cheese", 0xf7ef},
    {"chess", 0xf439},
    {"chess-bishop", 0xf43a},
    {"chess-board", 0xf43c},
    {"chess-king", 0xf43f},
    {"chess-knight", 0xf441},
    {"chess-pawn", 0xf443},
    {"chess-queen", 0xf445},
    {"chess-rook", 0xf447},
    {"chevron-circle-down", 0xf13a},
    {"chevron-circle-left", 0xf137},
    {"chevron-circle-right", 0xf138},
    {"chevron-circle-up", 0xf139},
    {"chevron-down", 0xf078},
    {"chevron-left", 0xf053},
    {"chevron-right", 0xf054},
    {"chevron-up", 0xf077},
    {"child", 0xf1ae},
    {"church", 0xf51d},
    {"circle", 0xf111},
    {"circle-notch", 0xf1ce},
    {"city", 0xf64f},
    {"clinic-medical", 0xf7f2},
    {"clipboard", 0xf328},
    {"clipboard-check", 0xf46c},
    {"clipboard-list", 0xf46d},
    {"clock", 0xf017},
    {"clone", 0xf24d},
    {"closed-captioning", 0xf20a},
    {"cloud", 0xf0c2},
    {"cloud-download-alt", 0xf381},
    {"cloud-meatball", 0xf73b},
    {"cloud-moon", 0xf6c3},
    {"cloud-moon-rain", 0xf73c},
    {"cloud-rain", 0xf73d},
    {"cloud-showers-heavy", 0xf740},
    {"cloud-sun", 0xf6c4},
    {"cloud-sun-rain", 0xf743},
    {"cloud-upload-alt", 0xf382},
    {"cocktail", 0xf561},
    {"code", 0xf121},
    {"code-branch", 0xf126},
    {"coffee", 0xf0f4},
    {"cog", 0xf013},
    {"cogs", 0xf085},
    {"coins", 0xf51e},
    {"columns", 0xf0db},
    {"comment", 0xf075},
    {"comment-alt", 0xf27a},
    {"comment-dollar", 0xf651},
    {"comment-dots", 0xf4ad},
    {"comment-medical", 0xf7f5},
    {"comment-slash", 0xf4b3},
    {"comments", 0xf086},
    {"comments-dollar", 0xf653},
    {"compact-disc", 0xf51f},
    {"compass", 0xf14e},
    {"compress", 0xf066},
    {"compress-alt", 0xf422},
    {"compress-arrows-alt", 0xf78c},
    {"concierge-bell", 0xf562},
    {"cookie", 0xf563},
    {"cookie-bite", 0xf564},
    {"copy", 0xf0c5},
    {"copyright", 0xf1f9},
    {"couch", 0xf4b8},
    {"credit-card", 0xf09d},
    {"crop", 0xf125},
    {"crop-alt", 0xf565},
    {"cross", 0xf654},
    {"crosshairs", 0xf05b},
    {"crow", 0xf520},
    {"crown", 0xf521},
    {"crutch", 0xf7f7},
    {"cube", 0xf1b2},
    {"cubes", 0xf1b3},
    {"cut", 0xf0c4},
    {"database", 0xf1c0},
    {"deaf", 0xf2a4},
    {"democrat", 0xf747},
    {"desktop", 0xf108},
    {"dharmachakra", 0xf655},
    {"diagnoses", 0xf470},
    {"dice", 0xf522},
    {"dice-d20", 0xf6cf},
    {"dice-d6", 0xf6d1},
    {"dice-five", 0xf523},
    {"dice-four", 0xf524},
    {"dice-one", 0xf525},
    {"dice-six", 0xf526},
    {"dice-three", 0xf527},
    {"dice-two", 0xf528},
    {"digital-tachograph", 0xf566},
    {"directions", 0xf5eb},
    {"disease", 0xf7fa},
    {"divide", 0xf529},
    {"dizzy", 0xf567},
    {"dna", 0xf471},
    {"dog", 0xf6d3},
    {"dollar-sign", 0xf155},
    {"dolly", 0xf472},
    {"dolly-flatbed", 0xf474},
    {"donate", 0xf4b9},
    {"door-closed", 0xf52a},
    {"door-open", 0xf52b},
    {"dot-circle", 0xf192},
    {"dove", 0xf4ba},
    {"download", 0xf019},
    {"drafting-compass", 0xf568},
    {"dragon", 0xf6d5},
    {"draw-polygon", 0xf5ee},
    {"drum", 0xf569},
    {"drum-steelpan", 0xf56a},
    {"drumstick-bite", 0xf6d7},
    {"dumbbell", 0xf44b},
    {"dumpster", 0xf793},
    {"dumpster-fire", 0xf794},
    {"dungeon", 0xf6d9},
    {"edit", 0xf044},
    {"egg", 0xf7fb},
    {"eject", 0xf052},
    {"ellipsis-h", 0xf141},
    {"ellipsis-v", 0xf142},
    {"envelope", 0xf0e0},
    {"envelope-open", 0xf2b6},
    {"envelope-open-text", 0xf658},
    {"envelope-square", 0xf199},
    {"equals", 0xf52c},
    {"eraser", 0xf12d},
    {"ethernet", 0xf796},
    {"euro-sign", 0xf153},
    {"exchange-alt", 0xf362},
    {"exclamation", 0xf12a},
    {"exclamation-circle", 0xf06a},
    {"exclamation-triangle", 0xf071},
    {"expand", 0xf065},
    {"expand-alt", 0xf424},
    {"expand-arrows-alt", 0xf31e},
    {"external-link-alt", 0xf35d},
    {"external-link-square-alt", 0xf360},
    {"eye", 0xf06e},
    {"eye-dropper", 0xf1fb},
    {"eye-slash", 0xf070},
    {"fan", 0xf863},
    {"fast-backward", 0xf049},
    {"fast-forward", 0xf050},
    {"faucet", 0xe005},
    {"fax", 0xf1ac},
    {"feather", 0xf52d},
    {"feather-alt", 0xf56b},
    {"female", 0xf182},
    {"fighter-jet", 0xf0fb},
    {"file", 0xf15b},
    {"file-alt", 0xf15c},
    {"file-archive", 0xf1c6},
    {"file-audio", 0xf1c7},
    {"file-code", 0xf1c9},
    {"file-contract", 0xf56c},
    {"file-csv", 0xf6dd},
    {"file-download", 0xf56d},
    {"file-excel", 0xf1c3},
    {"file-export", 0xf56e},
    {"file-image", 0xf1c5},
    {"file-import", 0xf56f},
    {"file-invoice", 0xf570},
    {"file-invoice-dollar", 0xf571},
    {"file-medical", 0xf477},
    {"file-medical-alt", 0xf478},
    {"file-pdf", 0xf1c1},
    {"file-powerpoint", 0xf1c4},
    {"file-prescription", 0xf572},
    {"file-signature", 0xf573},
    {"file-upload", 0xf574},
    {"file-video", 0xf1c8},
    {"file-word", 0xf1c2},
    {"fill", 0xf575},
    {"fill-drip", 0xf576},
    {"film", 0xf008},
    {"filter", 0xf0b0},
    {"fingerprint", 0xf577},
    {"fire", 0xf06d},
    {"fire-alt", 0xf7e4},
    {"fire-extinguisher", 0xf134},
    {"first-aid", 0xf479},
    {"fish", 0xf578},
    {"fist-raised", 0xf6de},
    {"flag", 0xf024},
    {"flag-checkered", 0xf11e},
    {"flag-usa", 0xf74d},
    {"flask", 0xf0c3},
    {"flushed", 0xf579},
    {"folder", 0xf07b},
    {"folder-minus", 0xf65d},
    {"folder-open", 0xf07c},
    {"folder-plus", 0xf65e},
    {"font", 0xf031},
    {"football-ball", 0xf44e},
    {"forward", 0xf04e},
    {"frog", 0xf52e},
    {"frown", 0xf119},
    {"frown-open", 0xf57a},
    {"funnel-dollar", 0xf662},
    {"futbol", 0xf1e3},
    {"gamepad", 0xf11b},
    {"gas-pump", 0xf52f},
    {"gavel", 0xf0e3},
    {"gem", 0xf3a5},
    {"genderless", 0xf22d},
    {"ghost", 0xf6e2},
    {"gift", 0xf06b},
    {"gifts", 0xf79c},
    {"glass-cheers", 0xf79f},
    {"glass-martini", 0xf000},
    {"glass-martini-alt", 0xf57b},
    {"glass-whiskey", 0xf7a0},
    {"glasses", 0xf530},
    {"globe", 0xf0ac},
    {"globe-africa", 0xf57c},
    {"globe-americas", 0xf57d},
    {"globe-asia", 0xf57e},
    {"globe-europe", 0xf7a2},
    {"golf-ball", 0xf450},
    {"gopuram", 0xf664},
    {"graduation-cap", 0xf19d},
    {"greater-than", 0xf531},
    {"greater-than-equal", 0xf532},
    {"grimace", 0xf57f},
    {"grin", 0xf580},
    {"grin-alt", 0xf581},
    {"grin-beam", 0xf582},
    {"grin-beam-sweat", 0xf583},
    {"grin-hearts", 0xf584},
    {"grin-squint", 0xf585},
    {"grin-squint-tears", 0xf586},
    {"grin-stars", 0xf587},
    {"grin-tears", 0xf588},
    {"grin-tongue", 0xf589},
    {"grin-tongue-squint", 0xf58a},
    {"grin-tongue-wink", 0xf58b},
    {"grin-wink", 0xf58c},
    {"grip-horizontal", 0xf58d},
    {"grip-lines", 0xf7a4},
    {"grip-lines-vertical", 0xf7a5},
    {"grip-vertical", 0xf58e},
    {"guitar", 0xf7a6},
    {"h-square", 0xf0fd},
    {"hamburger", 0xf805},
    {"hammer", 0xf6e3},
    {"hamsa", 0xf665},
    {"hand-holding", 0xf4bd},
    {"hand-holding-heart", 0xf4be},
    {"hand-holding-medical", 0xe05c},
    {"hand-holding-usd", 0xf4c0},
    {"hand-holding-water", 0xf4c1},
    {"hand-lizard", 0xf258},
    {"hand-middle-finger", 0xf806},
    {"hand-paper", 0xf256},
    {"hand-peace", 0xf25b},
    {"hand-point-down", 0xf0a7},
    {"hand-point-left", 0xf0a5},
    {"hand-point-right", 0xf0a4},
    {"hand-point-up", 0xf0a6},
    {"hand-pointer", 0xf25a},
    {"hand-rock", 0xf255},
    {"hand-scissors", 0xf257},
    {"hand-sparkles", 0xe05d},
    {"hand-spock", 0xf259},
    {"hands", 0xf4c2},
    {"hands-helping", 0xf4c4},
    {"hands-wash", 0xe05e},
    {"handshake", 0xf2b5},
    {"handshake-alt-slash", 0xe05f},
    {"handshake-slash", 0xe060},
    {"hanukiah", 0xf6e6},
    {"hard-hat", 0xf807},
    {"hashtag", 0xf292},
    {"hat-cowboy", 0xf8c0},
    {"hat-cowboy-side", 0xf8c1},
    {"hat-wizard", 0xf6e8},
    {"hdd", 0xf0a0},
    {"head-side-cough", 0xe061},
    {"head-side-cough-slash", 0xe062},
    {"head-side-mask", 0xe063},
    {"head-side-virus", 0xe064},
    {"heading", 0xf1dc},
    {"headphones", 0xf025},
    {"headphones-alt", 0xf58f},
    {"headset", 0xf590},
    {"heart", 0xf004},
    {"heart-broken", 0xf7a9},
    {"heartbeat", 0xf21e},
    {"helicopter", 0xf533},
    {"highlighter", 0xf591},
    {"hiking", 0xf6ec},
    {"hippo", 0xf6ed},
    {"history", 0xf1da},
    {"hockey-puck", 0xf453},
    {"holly-berry", 0xf7aa},
    {"home", 0xf015},
    {"horse", 0xf6f0},
    {"horse-head", 0xf7ab},
    {"hospital", 0xf0f8},
    {"hospital-alt", 0xf47d},
    {"hospital-symbol", 0xf47e},
    {"hospital-user", 0xf80d},
    {"hot-tub", 0xf593},
    {"hotdog", 0xf80f},
    {"hotel", 0xf594},
    {"hourglass", 0xf254},
    {"hourglass-end", 0xf253},
    {"hourglass-half", 0xf252},
    {"hourglass-start", 0xf251},
    {"house-damage", 0xf6f1},
    {"house-user", 0xe065},
    {"hryvnia", 0xf6f2},
    {"i-cursor", 0xf246},
    {"ice-cream", 0xf810},
    {"icicles", 0xf7ad},
    {"icons", 0xf86d},
    {"id-badge", 0xf2c1},
    {"id-card", 0xf2c2},
    {"id-card-alt", 0xf47f},
    {"igloo", 0xf7ae},
    {"image", 0xf03e},
    {"images", 0xf302},
    {"inbox", 0xf01c},
    {"indent", 0xf03c},
    {"industry", 0xf275},
    {"infinity", 0xf534},
    {"info", 0xf129},
    {"info-circle", 0xf05a},
    {"italic", 0xf033},
    {"jedi", 0xf669},
    {"joint", 0xf595},
    {"journal-whills", 0xf66a},
    {"kaaba", 0xf66b},
    {"key", 0xf084},
    {"keyboard", 0xf11c},
    {"khanda", 0xf66d},
    {"kiss", 0xf596},
    {"kiss-beam", 0xf597},
    {"kiss-wink-heart", 0xf598},
    {"kiwi-bird", 0xf535},
    {"landmark", 0xf66f},
    {"language", 0xf1ab},
    {"laptop", 0xf109},
    {"laptop-code", 0xf5fc},
    {"laptop-house", 0xe066},
    {"laptop-medical", 0xf812},
    {"laugh", 0xf599},
    {"laugh-beam", 0xf59a},
    {"laugh-squint", 0xf59b},
    {"laugh-wink", 0xf59c},
    {"layer-group", 0xf5fd},
    {"leaf", 0xf06c},
    {"lemon", 0xf094},
    {"less-than", 0xf536},
    {"less-than-equal", 0xf537},
    {"level-down-alt", 0xf3be},
    {"level-up-alt", 0xf3bf},
    {"life-ring", 0xf1cd},
    {"lightbulb", 0xf0eb},
    {"link", 0xf0c1},
    {"lira-sign", 0xf195},
    {"list", 0xf03a},
    {"list-alt", 0xf022},
    {"list-ol", 0xf0cb},
    {"list-ul", 0xf0ca},
    {"location-arrow", 0xf124},
    {"lock", 0xf023},
    {"lock-open", 0xf3c1},
    {"long-arrow-alt-down", 0xf309},
    {"long-arrow-alt-left", 0xf30a},
    {"long-arrow-alt-right", 0xf30b},
    {"long-arrow-alt-up", 0xf30c},
    {"low-vision", 0xf2a8},
    {"luggage-cart", 0xf59d},
    {"lungs", 0xf604},
    {"lungs-virus", 0xe067},
    {"magic", 0xf0d0},
    {"magnet", 0xf076},
    {"mail-bulk", 0xf674},
    {"male", 0xf183},
    {"map", 0xf279},
    {"map-marked", 0xf59f},
    {"map-marked-alt", 0xf5a0},
    {"map-marker", 0xf041},
    {"map-marker-alt", 0xf3c5},
    {"map-pin", 0xf276},
    {"map-signs", 0xf277},
    {"marker", 0xf5a1},
    {"mars", 0xf222},
    {"mars-double", 0xf227},
    {"mars-stroke", 0xf229},
    {"mars-stroke-h", 0xf22b},
    {"mars-stroke-v", 0xf22a},
    {"mask", 0xf6fa},
    {"medal", 0xf5a2},
    {"medkit", 0xf0fa},
    {"meh", 0xf11a},
    {"meh-blank", 0xf5a4},
    {"meh-rolling-eyes", 0xf5a5},
    {"memory", 0xf538},
    {"menorah", 0xf676},
    {"mercury", 0xf223},
    {"meteor", 0xf753},
    {"microchip", 0xf2db},
    {"microphone", 0xf130},
    {"microphone-alt", 0xf3c9},
    {"microphone-alt-slash", 0xf539},
    {"microphone-slash", 0xf131},
    {"microscope", 0xf610},
    {"minus", 0xf068},
    {"minus-circle", 0xf056},
    {"minus-square", 0xf146},
    {"mitten", 0xf7b5},
    {"mobile", 0xf10b},
    {"mobile-alt", 0xf3cd},
    {"money-bill", 0xf0d6},
    {"money-bill-alt", 0xf3d1},
    {"money-bill-wave", 0xf53a},
    {"money-bill-wave-alt", 0xf53b},
    {"money-check", 0xf53c},
    {"money-check-alt", 0xf53d},
    {"monument", 0xf5a6},
    {"moon", 0xf186},
    {"mortar-pestle", 0xf5a7},
    {"mosque", 0xf678},
    {"motorcycle", 0xf21c},
    {"mountain", 0xf6fc},
    {"mouse", 0xf8cc},
    {"mouse-pointer", 0xf245},
    {"mug-hot", 0xf7b6},
    {"music", 0xf001},
    {"network-wired", 0xf6ff},
    {"neuter", 0xf22c},
    {"newspaper", 0xf1ea},
    {"not-equal", 0xf53e},
    {"notes-medical", 0xf481},
    {"object-group", 0xf247},
    {"object-ungroup", 0xf248},
    {"oil-can", 0xf613},
    {"om", 0xf679},
    {"otter", 0xf700},
    {"outdent", 0xf03b},
    {"pager", 0xf815},
    {"paint-brush", 0xf1fc},
    {"paint-roller", 0xf5aa},
    {"palette", 0xf53f},
    {"pallet", 0xf482},
    {"paper-plane", 0xf1d8},
    {"paperclip", 0xf0c6},
    {"parachute-box", 0xf4cd},
    {"paragraph", 0xf1dd},
    {"parking", 0xf540},
    {"passport", 0xf5ab},
    {"pastafarianism", 0xf67b},
    {"paste", 0xf0ea},
    {"pause", 0xf04c},
    {"pause-circle", 0xf28b},
    {"paw", 0xf1b0},
    {"peace", 0xf67c},
    {"pen", 0xf304},
    {"pen-alt", 0xf305},
    {"pen-fancy", 0xf5ac},
    {"pen-nib", 0xf5ad},
    {"pen-square", 0xf14b},
    {"pencil-alt", 0xf303},
    {"pencil-ruler", 0xf5ae},
    {"people-arrows", 0xe068},
    {"people-carry", 0xf4ce},
    {"pepper-hot", 0xf816},
    {"percent", 0xf295},
    {"percentage", 0xf541},
    {"person-booth", 0xf756},
    {"phone", 0xf095},
    {"phone-alt", 0xf879},
    {"phone-slash", 0xf3dd},
    {"phone-square", 0xf098},
    {"phone-square-alt", 0xf87b},
    {"phone-volume", 0xf2a0},
    {"photo-video", 0xf87c},
    {"piggy-bank", 0xf4d3},
    {"pills", 0xf484},
    {"pizza-slice", 0xf818},
    {"place-of-worship", 0xf67f},
    {"plane", 0xf072},
    {"plane-arrival", 0xf5af},
    {"plane-departure", 0xf5b0},
    {"plane-slash", 0xe069},
    {"play", 0xf04b},
    {"play-circle", 0xf144},
    {"plug", 0xf1e6},
    {"plus", 0xf067},
    {"plus-circle", 0xf055},
    {"plus-square", 0xf0fe},
    {"podcast", 0xf2ce},
    {"poll", 0xf681},
    {"poll-h", 0xf682},
    {"poo", 0xf2fe},
    {"poo-storm", 0xf75a},
    {"poop", 0xf619},
    {"portrait", 0xf3e0},
    {"pound-sign", 0xf154},
    {"power-off", 0xf011},
    {"pray", 0xf683},
    {"praying-hands", 0xf684},
    {"prescription", 0xf5b1},
    {"prescription-bottle", 0xf485},
    {"prescription-bottle-alt", 0xf486},
    {"print", 0xf02f},
    {"procedures", 0xf487},
    {"project-diagram", 0xf542},
    {"pump-medical", 0xe06a},
    {"pump-soap", 0xe06b},
    {"puzzle-piece", 0xf12e},
    {"qrcode", 0xf029},
    {"question", 0xf128},
    {"question-circle", 0xf059},
    {"quidditch", 0xf458},
    {"quote-left", 0xf10d},
    {"quote-right", 0xf10e},
    {"quran", 0xf687},
    {"radiation", 0xf7b9},
    {"radiation-alt", 0xf7ba},
    {"rainbow", 0xf75b},
    {"random", 0xf074},
    {"receipt", 0xf543},
    {"record-vinyl", 0xf8d9},
    {"recycle", 0xf1b8},
    {"redo", 0xf01e},
    {"redo-alt", 0xf2f9},
    {"registered", 0xf25d},
    {"remove-format", 0xf87d},
    {"reply", 0xf3e5},
    {"reply-all", 0xf122},
    {"republican", 0xf75e},
    {"restroom", 0xf7bd},
    {"retweet", 0xf079},
    {"ribbon", 0xf4d6},
    {"ring", 0xf70b},
    {"road", 0xf018},
    {"robot", 0xf544},
    {"rocket", 0xf135},
    {"route", 0xf4d7},
    {"rss", 0xf09e},
    {"rss-square", 0xf143},
    {"ruble-sign", 0xf158},
    {"ruler", 0xf545},
    {"ruler-combined", 0xf546},
    {"ruler-horizontal", 0xf547},
    {"ruler-vertical", 0xf548},
    {"running", 0xf70c},
    {"rupee-sign", 0xf156},
    {"sad-cry", 0xf5b3},
    {"sad-tear", 0xf5b4},
    {"satellite", 0xf7bf},
    {"satellite-dish", 0xf7c0},
    {"save", 0xf0c7},
    {"school", 0xf549},
    {"screwdriver", 0xf54a},
    {"scroll", 0xf70e},
    {"sd-card", 0xf7c2},
    {"search", 0xf002},
    {"search-dollar", 0xf688},
    {"search-location", 0xf689},
    {"search-minus", 0xf010},
    {"search-plus", 0xf00e},
    {"seedling", 0xf4d8},
    {"server", 0xf233},
    {"shapes", 0xf61f},
    {"share", 0xf064},
    {"share-alt", 0xf1e0},
    {"share-alt-square", 0xf1e1},
    {"share-square", 0xf14d},
    {"shekel-sign", 0xf20b},
    {"shield-alt", 0xf3ed},
    {"shield-virus", 0xe06c},
    {"ship", 0xf21a},
    {"shipping-fast", 0xf48b},
    {"shoe-prints", 0xf54b},
    {"shopping-bag", 0xf290},
    {"shopping-basket", 0xf291},
    {"shopping-cart", 0xf07a},
    {"shower", 0xf2cc},
    {"shuttle-van", 0xf5b6},
    {"sign", 0xf4d9},
    {"sign-in-alt", 0xf2f6},
    {"sign-language", 0xf2a7},
    {"sign-out-alt", 0xf2f5},
    {"signal", 0xf012},
    {"signature", 0xf5b7},
    {"sim-card", 0xf7c4},
    {"sink", 0xe06d},
    {"sitemap", 0xf0e8},
    {"skating", 0xf7c5},
    {"skiing", 0xf7c9},
    {"skiing-nordic", 0xf7ca},
    {"skull", 0xf54c},
    {"skull-crossbones", 0xf714},
    {"slash", 0xf715},
    {"sleigh", 0xf7cc},
    {"sliders-h", 0xf1de},
    {"smile", 0xf118},
    {"smile-beam", 0xf5b8},
    {"smile-wink", 0xf4da},
    {"smog", 0xf75f},
    {"smoking", 0xf48d},
    {"smoking-ban", 0xf54d},
    {"sms", 0xf7cd},
    {"snowboarding", 0xf7ce},
    {"snowflake", 0xf2dc},
    {"snowman", 0xf7d0},
    {"snowplow", 0xf7d2},
    {"soap", 0xe06e},
    {"socks", 0xf696},
    {"solar-panel", 0xf5ba},
    {"sort", 0xf0dc},
    {"sort-alpha-down", 0xf15d},
    {"sort-alpha-down-alt", 0xf881},
    {"sort-alpha-up", 0xf15e},
    {"sort-alpha-up-alt", 0xf882},
    {"sort-amount-down", 0xf160},
    {"sort-amount-down-alt", 0xf884},
    {"sort-amount-up", 0xf161},
    {"sort-amount-up-alt", 0xf885},
    {"sort-down", 0xf0dd},
    {"sort-numeric-down", 0xf162},
    {"sort-numeric-down-alt", 0xf886},
    {"sort-numeric-up", 0xf163},
    {"sort-numeric-up-alt", 0xf887},
    {"sort-up", 0xf0de},
    {"spa", 0xf5bb},
    {"space-shuttle", 0xf197},
    {"spell-check", 0xf891},
    {"spider", 0xf717},
    {"spinner", 0xf110},
    {"splotch", 0xf5bc},
    {"spray-can", 0xf5bd},
    {"square", 0xf0c8},
    {"square-full", 0xf45c},
    {"square-root-alt", 0xf698},
    {"stamp", 0xf5bf},
    {"star", 0xf005},
    {"star-and-crescent", 0xf699},
    {"star-half", 0xf089},
    {"star-half-alt", 0xf5c0},
    {"star-of-david", 0xf69a},
    {"star-of-life", 0xf621},
    {"step-backward", 0xf048},
    {"step-forward", 0xf051},
    {"stethoscope", 0xf0f1},
    {"sticky-note", 0xf249},
    {"stop", 0xf04d},
    {"stop-circle", 0xf28d},
    {"stopwatch", 0xf2f2},
    {"stopwatch-20", 0xe06f},
    {"store", 0xf54e},
    {"store-alt", 0xf54f},
    {"store-alt-slash", 0xe070},
    {"store-slash", 0xe071},
    {"stream", 0xf550},
    {"street-view", 0xf21d},
    {"strikethrough", 0xf0cc},
    {"stroopwafel", 0xf551},
    {"subscript", 0xf12c},
    {"subway", 0xf239},
    {"suitcase", 0xf0f2},
    {"suitcase-rolling", 0xf5c1},
    {"sun", 0xf185},
    {"superscript", 0xf12b},
    {"surprise", 0xf5c2},
    {"swatchbook", 0xf5c3},
    {"swimmer", 0xf5c4},
    {"swimming-pool", 0xf5c5},
    {"synagogue", 0xf69b},
    {"sync", 0xf021},
    {"sync-alt", 0xf2f1},
    {"syringe", 0xf48e},
    {"table", 0xf0ce},
    {"table-tennis", 0xf45d},
    {"tablet", 0xf10a},
    {"tablet-alt", 0xf3fa},
    {"tablets", 0xf490},
    {"tachometer-alt", 0xf3fd},
    {"tag", 0xf02b},
    {"tags", 0xf02c},
    {"tape", 0xf4db},
    {"tasks", 0xf0ae},
    {"taxi", 0xf1ba},
    {"teeth", 0xf62e},
    {"teeth-open", 0xf62f},
    {"temperature-high", 0xf769},
    {"temperature-low", 0xf76b},
    {"tenge", 0xf7d7},
    {"terminal", 0xf120},
    {"text-height", 0xf034},
    {"text-width", 0xf035},
    {"th", 0xf00a},
    {"th-large", 0xf009},
    {"th-list", 0xf00b},
    {"theater-masks", 0xf630},
    {"thermometer", 0xf491},
    {"thermometer-empty", 0xf2cb},
    {"thermometer-full", 0xf2c7},
    {"thermometer-half", 0xf2c9},
    {"thermometer-quarter", 0xf2ca},
    {"thermometer-three-quarters", 0xf2c8},
    {"thumbs-down", 0xf165},
    {"thumbs-up", 0xf164},
    {"thumbtack", 0xf08d},
    {"ticket-alt", 0xf3ff},
    {"times", 0xf00d},
    {"times-circle", 0xf057},
    {"tint", 0xf043},
    {"tint-slash", 0xf5c7},
    {"tired", 0xf5c8},
    {"toggle-off", 0xf204},
    {"toggle-on", 0xf205},
    {"toilet", 0xf7d8},
    {"toilet-paper", 0xf71e},
    {"toilet-paper-slash", 0xe072},
    {"toolbox", 0xf552},
    {"tools", 0xf7d9},
    {"tooth", 0xf5c9},
    {"torah", 0xf6a0},
    {"torii-gate", 0xf6a1},
    {"tractor", 0xf722},
    {"trademark", 0xf25c},
    {"traffic-light", 0xf637},
    {"trailer", 0xe041},
    {"train", 0xf238},
    {"tram", 0xf7da},
    {"transgender", 0xf224},
    {"transgender-alt", 0xf225},
    {"trash", 0xf1f8},
    {"trash-alt", 0xf2ed},
    {"trash-restore", 0xf829},
    {"trash-restore-alt", 0xf82a},
    {"tree", 0xf1bb},
    {"trophy", 0xf091},
    {"truck", 0xf0d1},
    {"truck-loading", 0xf4de},
    {"truck-monster", 0xf63b},
    {"truck-moving", 0xf4df},
    {"truck-pickup", 0xf63c},
    {"tshirt", 0xf553},
    {"tty", 0xf1e4},
    {"tv", 0xf26c},
    {"umbrella", 0xf0e9},
    {"umbrella-beach", 0xf5ca},
    {"underline", 0xf0cd},
    {"undo", 0xf0e2},
    {"undo-alt", 0xf2ea},
    {"universal-access", 0xf29a},
    {"university", 0xf19c},
    {"unlink", 0xf127},
    {"unlock", 0xf09c},
    {"unlock-alt", 0xf13e},
    {"upload", 0xf093},
    {"user", 0xf007},
    {"user-alt", 0xf406},
    {"user-alt-slash", 0xf4fa},
    {"user-astronaut", 0xf4fb},
    {"user-check", 0xf4fc},
    {"user-circle", 0xf2bd},
    {"user-clock", 0xf4fd},
    {"user-cog", 0xf4fe},
    {"user-edit", 0xf4ff},
    {"user-friends", 0xf500},
    {"user-graduate", 0xf501},
    {"user-injured", 0xf728},
    {"user-lock", 0xf502},
    {"user-md", 0xf0f0},
    {"user-minus", 0xf503},
    {"user-ninja", 0xf504},
    {"user-nurse", 0xf82f},
    {"user-plus", 0xf234},
    {"user-secret", 0xf21b},
    {"user-shield", 0xf505},
    {"user-slash", 0xf506},
    {"user-tag", 0xf507},
    {"user-tie", 0xf508},
    {"user-times", 0xf235},
    {"users", 0xf0c0},
    {"users-cog", 0xf509},
    {"users-slash", 0xe073},
    {"utensil-spoon", 0xf2e5},
    {"utensils", 0xf2e7},
    {"vector-square", 0xf5cb},
    {"venus", 0xf221},
    {"venus-double", 0xf226},
    {"venus-mars", 0xf228},
    {"vest", 0xe085},
    {"vest-patches", 0xe086},
    {"vial", 0xf492},
    {"vials", 0xf493},
    {"video", 0xf03d},
    {"video-slash", 0xf4e2},
    {"vihara", 0xf6a7},
    {"virus", 0xe074},
    {"virus-slash", 0xe075},
    {"viruses", 0xe076},
    {"voicemail", 0xf897},
    {"volleyball-ball", 0xf45f},
    {"volume-down", 0xf027},
    {"volume-mute", 0xf6a9},
    {"volume-off", 0xf026},
    {"volume-up", 0xf028},
    {"vote-yea", 0xf772},
    {"vr-cardboard", 0xf729},
    {"walking", 0xf554},
    {"wallet", 0xf555},
    {"warehouse", 0xf494},
    {"water", 0xf773},
    {"wave-square", 0xf83e},
    {"weight", 0xf496},
    {"weight-hanging", 0xf5cd},
    {"wheelchair", 0xf193},
    {"wifi", 0xf1eb},
    {"wind", 0xf72e},
    {"window-close", 0xf410},
    {"window-maximize", 0xf2d0},
    {"window-minimize", 0xf2d1},
    {"window-restore", 0xf2d2},
    {"wine-bottle", 0xf72f},
    {"wine-glass", 0xf4e3},
    {"wine-glass-alt", 0xf5ce},
    {"won-sign", 0xf159},
    {"wrench", 0xf0ad},
    {"x-ray", 0xf497},
    {"yen-sign", 0xf157},
    {"yin-yang", 0xf6ad},
};

static const uint8_t b64chars[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void b64_decode(const uint8_t *in, int in_len, uint8_t *out)
{
    uint8_t inv[256] = {0};
    for (int i = 0; i < (int)sizeof(b64chars)-1; i++) {
        inv[b64chars[i]] = (uint8_t) i;
    }
    uint8_t block[4];
    for (int i = 0; i < in_len; i += 4) {
        block[0] = inv[in[i]];
        block[1] = inv[in[i + 1]];
        block[2] = inv[in[i + 2]];
        block[3] = inv[in[i + 3]];

        *out++ = (block[0] << 2) | (block[1] >> 4);
        *out++ = (block[1] << 4) | (block[2] >> 2);
        *out++ = (block[2] << 6) | block[3];
    }
}

int get_codepoint(const char *name, int len)
{
    if (len == 0) {
        return -1;
    }
    for (int i = 0; i < (int)sizeof(icons)/(int)sizeof(icons[0]); i++) {
        if (strncasecmp(icons[i].name, name, len) == 0) {
            return icons[i].codepoint;
        }
    }
    return -1;
}
