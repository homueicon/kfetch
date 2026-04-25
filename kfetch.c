#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <sys/statvfs.h>
#include <termios.h>
#include <ctype.h>
#include <png.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <dirent.h>

#define CONFIG_PATH "/.config/kfetch/kfetch.conf"

#define RESET    "\033[0m"
#define BOLD     "\033[1m"
#define RED      "\033[31m"
#define BLUE     "\033[34m"
#define CYAN     "\033[36m"
#define WHITE    "\033[97m"
#define BRED     "\033[91m"
#define BBLUE    "\033[94m"
#define BGREEN   "\033[92m"
#define BYELLOW  "\033[93m"
#define BMAGENTA "\033[95m"
#define BCYAN    "\033[96m"
#define GREEN    "\033[32m"

typedef enum {
    DISTRO_ARCH,
    DISTRO_ARTIX,
    DISTRO_ALPINE,
    DISTRO_BEDROCK,
    DISTRO_CACHYOS,
    DISTRO_FEDORA,
    DISTRO_DEBIAN,
    DISTRO_UBUNTU,
    DISTRO_GENTOO,
    DISTRO_OPENSUSE,
    DISTRO_OPENSUSE_TUMBLEWEED,
    DISTRO_OPENSUSE_LEAP,
    DISTRO_VOID,
    DISTRO_WINDOWS,
    DISTRO_DEVUAN,
    DISTRO_PARABOLA,
    DISTRO_BLACKARCH,
    DISTRO_UNKNOWN
} DistroID;

typedef struct {
    DistroID    id;
    const char *name;
    const char *color1;
    const char *color2;
    const char **lines;
    int          num_lines;
    int          width;
} Distro;

static const char *lines_arch[] = {
    "                  -`",
    "                 .o+`",
    "                `ooo/",
    "               `+oooo:",
    "              `+oooooo:",
    "              -+oooooo+:",
    "            `/:-:++oooo+:",
    "           `/++++/+++++++:",
    "          `/++++++++++++++:",
    "         `/+++ooooooooooooo/`",
    "        ./ooosssso++osssssso+`",
    "       .oossssso-````/ossssss+`",
    "      -osssssso.      :ssssssso.",
    "     :osssssss/        osssso+++.",
    "    /ossssssss/        +ssssooo/-",
    "  `/ossssso+/:-        -:/+osssso+-",
    " `+sso+:-`                 `.-/+oso:",
    "`++:.                           `-/+/",
    ".`                                 `/",
};

static const char *lines_artix[] = {
    "                   '",
    "                  'o'",
    "                 'ooo'",
    "                'ooxoo'",
    "               'ooxxxoo'",
    "              'oookkxxoo'",
    "             'oiioxkkxxoo'",
    "            ':;:iiiioxxxoo'",
    "               `'.;::ioxxoo'",
    "          '-.      `':;jiooo'",
    "         'oooio-..     `'i:io'",
    "        'ooooxxxxoio:,.   `'-;'",
    "       'ooooxxxxxkkxoooIi:-.  `'",
    "      'ooooxxxxxkkkkxoiiiiiji'",
    "     'ooooxxxxxkxxoiiii:'`     .i'",
    "    'ooooxxxxxoi:::'`       .;ioxo'",
    "   'ooooxooi::'`         .:iiixkxxo'",
    "  'ooooi:'`                `'';ioxxo'",
    " 'i:'`                          '':io'",
    "'`                                   `'",
};

static const char *lines_alpine[] = {
    "       .hddddddddddddddddddddddh.",
    "      :dddddddddddddddddddddddddd:",
    "     /dddddddddddddddddddddddddddd/",
    "    +dddddddddddddddddddddddddddddd+",
    "  `sdddddddddddddddddddddddddddddddds`",
    " `ydddddddddddd++hdddddddddddddddddddy`",
    ".hddddddddddd+`  `+ddddh:-sdddddddddddh.",
    "hdddddddddd+`      `+y:    .sddddddddddh",
    "ddddddddh+`   `//`   `.`     -sddddddddd",
    "ddddddh+`   `/hddh/`   `:s-    -sddddddd",
    "ddddh+`   `/+/dddddh/`   `+s-    -sddddd",
    "ddd+`   `/o` :dddddddh/`   `oy-    .yddd",
    "hdddyo+ohddyosdddddddddho+oydddy++ohdddh",
    ".hddddddddddddddddddddddddddddddddddddh.",
    " `yddddddddddddddddddddddddddddddddddy`",
    "  `sdddddddddddddddddddddddddddddddds`",
    "    +dddddddddddddddddddddddddddddd+",
    "     /dddddddddddddddddddddddddddd/",
    "      :dddddddddddddddddddddddddd:",
    "       .hddddddddddddddddddddddh.",
};

static const char *lines_bedrock[] = {
    "--------------------------------------",
    "--------------------------------------",
    "--------------------------------------",
    "---\\\\\\\\\\\\-----------------------",
    "----\\\\\\      \\\\\\----------------------",
    "-----\\\\\\      \\\\\\---------------------",
    "------\\\\\\      \\\\\\\\\\\\\\\\\\\\\\\\------",
    "-------\\\\\\                    \\\\\\-----",
    "--------\\\\\\                    \\\\\\----",
    "---------\\\\\\        ______      \\\\\\---",
    "----------\\\\\\                   ///---",
    "-----------\\\\\\                 ///----",
    "------------\\\\\\               ///-----",
    "-------------\\\\\\////////////////------",
    "--------------------------------------",
    "--------------------------------------",
    "--------------------------------------",
};

static const char *lines_cachyos[] = {
    "           .-------------------------:",
    "          .+=========================.",
    "         :++===++==================-       :++-",
    "        :*++====+++++=============-        .==:",
    "       -*+++=====+***++==========:",
    "      =*++++========------------:",
    "     =*+++++=====-                     ...",
    "   .+*+++++=-===:                    .=+++=:",
    "  :++++=====-==:                     -***/**+",
    " :++========-=.                      .=+**+.",
    ".+==========-.                          .",
    " :+++++++====-                                .--==-.",
    "  :++==========.                             :+++++++:",
    "   .-===========.                            =*****+*+",
    "    .-===========:                           .+*****+:",
    "      -=======++++:::::::::::::::::::::::::-:  .---:",
    "       :======++++====+++******************=.",
    "        :=====+++==========++++++++++++++*-",
    "         .====++==============++++++++++*-",
    "          .===+==================+++++++:",
    "           .-=======================+++:",
    "             ..........................",
};

static const char *lines_debian[] = {
    "        _,met$$$$$$$$$$gg.",
    "     ,g$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$P.",
    "   ,g$$$$P\"\"       \"\"\"Y$$$$.\".",
    "  ,$$$$P'              `$$$$$$.",
    "',$$$$P       ,ggs.     `$$$$b:",
    "`d$$$$'     ,$P\"'   .    $$$$$$",
    " $$$$P      d$'     ,    $$$$P",
    " $$$$:      $$$.   -    ,d$$$$'",
    " $$$$;      Y$b._   _,d$P'",
    " Y$$$$.    `.`\"Y$$$$$$$$P\"'",
    " `$$$$b      \"-.__",
    "  `Y$$$$b",
    "   `Y$$$$.",
    "     `$$$$b.",
    "       `Y$$$$b.",
    "         `\"Y$$b._",
    "             `\"\"\"\"",
};

static const char *lines_ubuntu[] = {
    "       ..;,; .,;,.",
    "    .,lool: .ooooo,",
    "   ;oo;:    .coool.",
    " ....         ''' ,l;",
    ":oooo,            'oo.",
    "looooc            :oo'",
    " '::'             ,oo:",
    "   ,.,       .... co,",
    "    lo:;.   :oooo; .",
    "     ':ooo; cooooc",
    "        '''  ''''",
};

static const char *lines_fedora[] = {
    "             .',;::::;,'.",
    "         .';:cccccccccccc:;,.",
    "      .;cccccccccccccccccccccc;.",
    "    .:cccccccccccccccccccccccccc:.",
    "  .;ccccccccccccc;.:dddl:.;ccccccc;.",
    " .:ccccccccccccc;OWMKOOXMWd;ccccccc:.",
    ".:ccccccccccccc;KMMc;cc;xMMc;ccccccc:.",
    ",cccccccccccccc;MMM.;cc;;WW:;cccccccc,",
    ":cccccccccccccc;MMM.;cccccccccccccccc:",
    ":ccccccc;oxOOOo;MMM000k.;cccccccccccc:",
    "cccccc;MMKxdd:;MMMkddc.;cccccccccccc;",
    "ccccc;XMO';cccc;MMM.;cccccccccccccccc'",
    "ccccc;MMo;ccccc;MMW.;ccccccccccccccc;",
    "ccccc;MNc.ccc.xMMd;ccccccccccccccc;",
    "cccccc;dNMWXXXWM0:;cccccccccccccc:,",
    "cccccccc;.:odl:.;cccccccccccccc:,.",
    "ccccccccccccccccccccccccccccc:'.",
    ":ccccccccccccccccccccccc:;,..",
    " ':cccccccccccccccc::;,.",
};

static const char *lines_gentoo[] = {
    "         -/oyddmdhs+:.",
    "     -odNMMMMMMMMNNmhy+-`",
    "   -yNMMMMMMMMMMMNNNmmdhy+-",
    " `omMMMMMMMMMMMMNmdmmmmddhhy/`",
    " omMMMMMMMMMMMNhhyyyohmdddhhhdo`",
    ".ydMMMMMMMMMMdhs++so/smdddhhhhdm+`",
    " oyhdmNMMMMMMMNdyooydmddddhhhhyhNd.",
    "  :oyhhdNNMMMMMMMNNNmmdddhhhhhyymMh",
    "    .:+sydNMMMMMNNNmmmdddhhhhhhmMmy",
    "       /mMMMMMMNNNmmmdddhhhhhmMNhs:",
    "    `oNMMMMMMMNNNmmmddddhhdmMNhs+`",
    "  `sNMMMMMMMMNNNmmmdddddmNMmhs/.",
    " /NMMMMMMMMNNNNmmmdddmNMNdso:`",
    "+MMMMMMMNNNNNmmmmdmNMNdso/-",
    "yMMNNNNNNNmmmmmNNMmhs+/-`",
    "/hMMNNNNNNNNMNdhs++/-`",
    "`/ohdmmddhys+++/:.`",
    "  `-//////:--.",
};

static const char *lines_opensuse[] = {
    "  _______",
    "__|   __ \\",
    "     / .\\ \\",
    "     \\__/ |",
    "   _______|",
    "   \\_______",
    "__________/",
};

static const char *lines_opensuse_tumbleweed[] = {
    "          ,...,",
    "     .,:lloooooc;.",
    "   ,ool'     oo,;oo:",
    " .lo'        oo.   oo:",
    ".oo.         oo.    oo:",
    ":ol          oo.    'oo",
    ":oo         .oo.    .oo.",
    ".oooooooooooooo.    .oo.",
    " ;oo.               .oo.",
    "  'oo,              .oo.",
    "    \"ooc,',,,,,,,,,,:ooc,,,,,,,,,,,",
    "       ':cooooooooooooooooooooooooool;.",
    "                    .oo.             .oo;",
    "                    .oo.               .oo.",
    "                    .oo.    'oooooooooo:ooo.",
    "                    .oo.    'oo.         col",
    "                    .oo'    'oo          col",
    "                     coo    'oo          oo'",
    "                      coc   'oo        .lo,",
    "                       `oo, 'oo      .:oo",
    "                         'ooooc,, ,:lol",
    "                            `''\"clc\"'",
};

static const char *lines_opensuse_leap[] = {
    "          ====",
    "         ======",
    "       ==== ====+",
    "     +====    +====",
    "   +===+        ====",
    "  ====            ====",
    "+===               +====",
    "====               +====",
    " =====            ====",
    "   +===+        =====",
    "==+  =====    +===+  ===",
    "====   ==== =====  =====",
    "  ====  =======   ===",
    "    ====  ===   ===",
    "     ====+    ===",
    "       ==== ====",
    "         ======",
    "           ==",
};

static const char *lines_void[] = {
    "                __.;=====;.__",
    "            _.=+==++=++=+=+===;.",
    "             -=+++=+===+=+=+++++=_",
    "        .     -=:``     `--==+=++==.",
    "       _vi,    `            --+=++++:",
    "      .uvnvi.       _._       -==+==+.",
    "     .vvnvnI`    .;==|==;.     :|=||=|.",
    "+QmQQmpvvnv; _yYsyQQWUUQQQm #QmQ#:QQQWUV$QQm.",
    " -QQWQWpvvowZ?.wQQQE==<QWWQ/QWQW.QQWW(: jQWQE",
    "  -$QQQQmmU'  jQQQ@+=<QWQQ)mQQQ.mQQQC+;jWQQ@'",
    "   -$WQ8YnI:   QWQQwgQQWV`mWQQ.jQWQQgyyWW@!",
    "     -1vvnvv.     `~+++`        ++|+++",
    "      +vnvnnv,                 `-|===",
    "       +vnvnvns.           .      :=-",
    "        -Invnvvnsi..___..=sv=.     `",
    "          +Invnvnvnnnnnnnnvvnn;.",
    "            ~|Invnvnvvnvvvnnv}+`",
    "               -~|{*l}*|~",
};

static const char *lines_windows[] = {
    "lllllll  lllllll",
    "lllllll  lllllll",
    "lllllll  lllllll",
    "",
    "lllllll  lllllll",
    "lllllll  lllllll",
    "lllllll  lllllll",
};

static const char *lines_unknown[] = {
    "      .--.      ",
    "     |o_o |     ",
    "     |:_/ |     ",
    "    //   \\ \\    ",
    "   (|     | )   ",
    "  /'\\_   _/`\\   ",
    "  \\___)=(___/   ",
};

static const char *lines_devuan[] = {
    "   ..,,;;;::;,..",
    "           `':ddd;:,.",
    "                 `'dPPd:,.",
    "                     `:b$$$$b`.",
    "                        'P$$$$$d`",
    "                         .$$$$$$$$$`",
    "                         ;$$$$$$$$$P",
    "                      .:P$$$$$$$$$$$$`",
    "                  .,:b$$$$$$$$$$$$$;'",
    "             .,:dP$$$$$$$$$$$$$$$$b:'",
    "      .,:;db$$$$$$$$$$$$$$$$$$$$Pd'`",
    " ,db$$$$$$$$$$$$$$$$$$$$$$$$$$$$b:'`",
    ":$$$$$$$$$$$$$$$$$$$$$$$$b:'`",
    " `$$$$$$$$$bd:''`",
    "   `'''`",
};

static const char *lines_parabola[] = {
    "                          `.-.    `.",
    "                   `.`  `:++.   `-+o+.",
    "             `` `:+/. `:+/.   `-+oooo+",
    "        ``-::-.:+/. `:+/.   `-+oooooo+",
    "    `.-:///-  ..`   .-.   `-+oooooooo-",
    " `..-..`                 `+ooooooooo:",
    "``                        :oooooooo/",
    "                          `ooooooo:",
    "                          `oooooo:",
    "                          -oooo+.",
    "                          +ooo/`",
    "                         -ooo-",
    "                        `+o/.",
    "                        /+-",
    "                       //`",
    "                      -.",
};

static const char *lines_blackarch[] = {
    BCYAN "                   00" RESET,
    "                   11",
    "                  ====" RESET,
    "                  ." BCYAN "//" RESET,
    "                 `o" BCYAN "//" RESET ":",
    "                `+o" BCYAN "//" RESET "o:",
    "               `+oo" BCYAN "//" RESET "oo:",
    "               -+oo" BCYAN "//" RESET "oo+:",
    "             `/:-:+" BCYAN "//" RESET "ooo+:",
    "            `/+++++" BCYAN "//" RESET "+++++:",
    "           `/++++++" BCYAN "//" RESET "++++++:",
    "          `/+++o" BRED "ooo" BCYAN "//" BRED "ooo" RESET "oooo/`",
    BRED "         " RESET "./" BRED "ooosssso" BCYAN "//" BRED "osssssso" RESET "+`",
    BRED "        .oossssso-`" BCYAN "//" RESET "`/ossssss+`",
    "       -osssssso.  " BCYAN "//" RESET "  :ssssssso.",
    "      :osssssss/   " BCYAN "//" RESET "   osssso+++.",
    "     /ossssssss/   " BCYAN "//" RESET "   +ssssooo/-",
    "   `/ossssso+/:-   " BCYAN "//" RESET "   -:/+osssso+-",
    "  `+sso+:-`        " BCYAN "//" RESET "       `.-/+oso:",
    " `++:.             " BCYAN "//" RESET "            `-/+//",
    " .`                " BCYAN "/" RESET "                `/",
};

static Distro distros[] = {
    { DISTRO_ARCH,               "arch",               BBLUE,    BLUE,    lines_arch,                    19, 37 },
    { DISTRO_ARTIX,              "artix",              BCYAN,    WHITE,   lines_artix,                   20, 39 },
    { DISTRO_ALPINE,             "alpine",             BBLUE,    BLUE,    lines_alpine,                  20, 40 },
    { DISTRO_BEDROCK,            "bedrock",            WHITE,    CYAN,    lines_bedrock,                 17, 38 },
    { DISTRO_CACHYOS,            "cachyos",            BBLUE,    BGREEN,  lines_cachyos,                 22, 54 },
    { DISTRO_FEDORA,             "fedora",             BBLUE,    WHITE,   lines_fedora,                  19, 38 },
    { DISTRO_DEBIAN,             "debian",             BRED,     RED,     lines_debian,                  17, 39 },
    { DISTRO_UBUNTU,             "ubuntu",             BRED,     BYELLOW, lines_ubuntu,                  11, 22 },
    { DISTRO_GENTOO,             "gentoo",             BMAGENTA, WHITE,   lines_gentoo,                  18, 35 },
    { DISTRO_OPENSUSE,           "opensuse",           BGREEN,   WHITE,   lines_opensuse,                 7, 11 },
    { DISTRO_OPENSUSE_TUMBLEWEED,"opensuse-tumbleweed",BGREEN,   GREEN,   lines_opensuse_tumbleweed,     22, 46 },
    { DISTRO_OPENSUSE_LEAP,      "opensuse-leap",      BGREEN,   WHITE,   lines_opensuse_leap,           17, 31 },
    { DISTRO_VOID,               "void",               BGREEN,   GREEN,   lines_void,                    18, 45 },
    { DISTRO_WINDOWS,            "windows",            BCYAN,    BBLUE,   lines_windows,                  7, 16 },
    { DISTRO_DEVUAN,             "devuan",             BRED,     WHITE,   lines_devuan,                  14, 51 },
    { DISTRO_PARABOLA,           "parabola",           BCYAN,    WHITE,   lines_parabola,                16, 57 },
    { DISTRO_BLACKARCH,          "blackarch",          BRED,     BCYAN,   lines_blackarch,               21, 50 },
    { DISTRO_UNKNOWN,            "unknown",            BCYAN,    WHITE,   lines_unknown,                  7, 16 },
};

#define NUM_DISTROS (int)(sizeof(distros) / sizeof(distros[0]))

typedef struct {
    char user[64];
    char host[64];
    int  show_os;
    int  show_kernel;
    int  show_shell;
    int  show_ram;
    int  show_uptime;
    int  show_cpu;
    int  show_disk;
    int  show_packages;
    int  show_de;
    int  show_wm;
    int  show_terminal;
} Config;

static Config cfg = {
    .user        = "user",
    .host        = "host",
    .show_os     = 1,
    .show_kernel = 1,
    .show_shell  = 1,
    .show_ram    = 0,
    .show_uptime = 0,
    .show_cpu    = 0,
    .show_disk   = 0,
    .show_packages = 0,
    .show_de     = 0,
    .show_wm     = 0,
    .show_terminal = 0,
};

static void get_distro_name(char *out, size_t len) {
    FILE *f = fopen("/etc/os-release", "r");
    if (!f) { strncpy(out, "Unknown", len); return; }
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "PRETTY_NAME=", 12) == 0) {
            char *v = line + 12;
            if (*v == '"') v++;
            v[strcspn(v, "\"\n")] = 0;
            strncpy(out, v, len);
            fclose(f);
            return;
        }
    }
    fclose(f);
    strncpy(out, "Unknown", len);
}

static DistroID detect_distro(void) {
    struct utsname u;
    if (uname(&u) == 0) {
        char release[256];
        strncpy(release, u.release, sizeof(release));
        for (int i = 0; i < (int)strlen(release); i++)
            release[i] = tolower((unsigned char)release[i]);
        if (strstr(release, "microsoft") || strstr(release, "windows"))
            return DISTRO_WINDOWS;
    }
    FILE *f = fopen("/etc/os-release", "r");
    if (!f) return DISTRO_UNKNOWN;
    char line[256];
    char id[64] = {0};
    char version_id[64] = {0};
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "ID=", 3) == 0) {
            char *v = line + 3;
            if (*v == '"') v++;
            v[strcspn(v, "\"\n")] = 0;
            strncpy(id, v, sizeof(id));
        } else if (strncmp(line, "VERSION_ID=", 11) == 0) {
            char *v = line + 11;
            if (*v == '"') v++;
            v[strcspn(v, "\"\n")] = 0;
            strncpy(version_id, v, sizeof(version_id));
        }
    }
    fclose(f);
    for (int i = 0; i < (int)strlen(id); i++) id[i] = tolower((unsigned char)id[i]);

    if (strstr(id, "arch"))        return DISTRO_ARCH;
    if (strstr(id, "artix"))       return DISTRO_ARTIX;
    if (strstr(id, "alpine"))      return DISTRO_ALPINE;
    if (strstr(id, "bedrock"))     return DISTRO_BEDROCK;
    if (strstr(id, "cachyos"))     return DISTRO_CACHYOS;
    if (strstr(id, "fedora"))      return DISTRO_FEDORA;
    if (strstr(id, "debian"))      return DISTRO_DEBIAN;
    if (strstr(id, "ubuntu"))      return DISTRO_UBUNTU;
    if (strstr(id, "gentoo"))      return DISTRO_GENTOO;
    if (strstr(id, "void"))        return DISTRO_VOID;
    if (strstr(id, "devuan"))      return DISTRO_DEVUAN;
    if (strstr(id, "parabola"))    return DISTRO_PARABOLA;
    if (strstr(id, "blackarch"))   return DISTRO_BLACKARCH;

    if (strstr(id, "opensuse")) {
        if (strstr(version_id, "tumbleweed") || strstr(version_id, "202"))
            return DISTRO_OPENSUSE_TUMBLEWEED;
        else if (strstr(version_id, "15") || strstr(version_id, "leap"))
            return DISTRO_OPENSUSE_LEAP;
        else
            return DISTRO_OPENSUSE;
    }

    return DISTRO_UNKNOWN;
}

static Distro *get_distro(DistroID id) {
    for (int i = 0; i < NUM_DISTROS; i++)
        if (distros[i].id == id) return &distros[i];
    return &distros[NUM_DISTROS - 1];
}

static void get_kernel(char *out, size_t len) {
    struct utsname u;
    if (uname(&u) == 0) snprintf(out, len, "%s", u.release);
    else strncpy(out, "unknown", len);
}

static void get_shell(char *out, size_t len) {
    char *sh = getenv("SHELL");
    if (!sh) { strncpy(out, "unknown", len); return; }
    char *base = strrchr(sh, '/');
    strncpy(out, base ? base + 1 : sh, len);
}

static void get_ram(char *out, size_t len) {
    struct sysinfo si;
    if (sysinfo(&si) != 0) { strncpy(out, "unknown", len); return; }
    long used  = (si.totalram - si.freeram) * si.mem_unit / 1024 / 1024;
    long total = si.totalram * si.mem_unit / 1024 / 1024;
    snprintf(out, len, "%ldMiB / %ldMiB", used, total);
}

static void get_uptime(char *out, size_t len) {
    struct sysinfo si;
    if (sysinfo(&si) != 0) { strncpy(out, "unknown", len); return; }
    long h = si.uptime / 3600;
    long m = (si.uptime % 3600) / 60;
    snprintf(out, len, "%ldh %ldm", h, m);
}

static void get_cpu(char *out, size_t len) {
    FILE *f = fopen("/proc/cpuinfo", "r");
    if (!f) { strncpy(out, "unknown", len); return; }
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "model name", 10) == 0) {
            char *colon = strchr(line, ':');
            if (colon) {
                char *model = colon + 1;
                while (*model == ' ') model++;
                model[strcspn(model, "\n")] = 0;
                strncpy(out, model, len);
                fclose(f);
                return;
            }
        }
    }
    fclose(f);
    strncpy(out, "unknown", len);
}

static void get_disk(char *out, size_t len) {
    struct statvfs st;
    if (statvfs("/", &st) != 0) { strncpy(out, "unknown", len); return; }
    unsigned long total = st.f_blocks * st.f_frsize / 1024 / 1024;
    unsigned long free  = st.f_bfree  * st.f_frsize / 1024 / 1024;
    unsigned long used  = total - free;
    snprintf(out, len, "%luMiB / %luMiB", used, total);
}

static void get_packages(char *out, size_t len) {
    const char *managers[] = {"pacman", "dpkg", "rpm", "xbps-query", "apk", "emerge"};
    const char *count_cmd[] = {
        "pacman -Qq 2>/dev/null | wc -l",
        "dpkg-query -f '${Package}\n' -W 2>/dev/null | wc -l",
        "rpm -qa 2>/dev/null | wc -l",
        "xbps-query -l 2>/dev/null | wc -l",
        "apk list --installed 2>/dev/null | wc -l",
        "ls -d /var/db/pkg/*/* 2>/dev/null | wc -l"
    };
    for (int i = 0; i < 6; i++) {
        FILE *p = popen(count_cmd[i], "r");
        if (p) {
            char num[32];
            if (fgets(num, sizeof(num), p)) {
                long n = atol(num);
                if (n > 0) {
                    snprintf(out, len, "%ld (%s)", n, managers[i]);
                    pclose(p);
                    return;
                }
            }
            pclose(p);
        }
    }
    strncpy(out, "unknown", len);
}

static void get_de(char *out, size_t len) {
    char *de = getenv("XDG_CURRENT_DESKTOP");
    if (!de) de = getenv("DESKTOP_SESSION");
    if (!de) { strncpy(out, "unknown", len); return; }
    strncpy(out, de, len);
}

static void get_wm(char *out, size_t len) {
    char *wm = getenv("XDG_SESSION_DESKTOP");
    if (!wm) wm = getenv("GDMSESSION");
    if (!wm) wm = getenv("DESKTOP_SESSION");
    if (!wm) { strncpy(out, "unknown", len); return; }
    strncpy(out, wm, len);
}

static void get_terminal(char *out, size_t len) {
    char *term = getenv("TERM_PROGRAM");
    if (!term) term = getenv("TERM");
    if (!term) { strncpy(out, "unknown", len); return; }
    strncpy(out, term, len);
}

static char *config_path(void) {
    static char path[512];
    char *home = getenv("HOME");
    if (!home) home = "/root";
    snprintf(path, sizeof(path), "%s%s", home, CONFIG_PATH);
    return path;
}

static void load_config(void) {
    FILE *f = fopen(config_path(), "r");
    if (!f) return;
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == '#' || line[0] == 0) continue;
        char key[64], val[192];
        if (sscanf(line, "%63[^=]=%191s", key, val) != 2) continue;
        if      (!strcmp(key, "user"))          strncpy(cfg.user, val, sizeof(cfg.user));
        else if (!strcmp(key, "host"))          strncpy(cfg.host, val, sizeof(cfg.host));
        else if (!strcmp(key, "show_os"))       cfg.show_os       = atoi(val);
        else if (!strcmp(key, "show_kernel"))   cfg.show_kernel   = atoi(val);
        else if (!strcmp(key, "show_shell"))    cfg.show_shell    = atoi(val);
        else if (!strcmp(key, "show_ram"))      cfg.show_ram      = atoi(val);
        else if (!strcmp(key, "show_uptime"))   cfg.show_uptime   = atoi(val);
        else if (!strcmp(key, "show_cpu"))      cfg.show_cpu      = atoi(val);
        else if (!strcmp(key, "show_disk"))     cfg.show_disk     = atoi(val);
        else if (!strcmp(key, "show_packages")) cfg.show_packages = atoi(val);
        else if (!strcmp(key, "show_de"))       cfg.show_de       = atoi(val);
        else if (!strcmp(key, "show_wm"))       cfg.show_wm       = atoi(val);
        else if (!strcmp(key, "show_terminal")) cfg.show_terminal = atoi(val);
    }
    fclose(f);
}

static void save_config(void) {
    char dir[512];
    char *home = getenv("HOME");
    if (!home) home = "/root";
    snprintf(dir, sizeof(dir), "%s/.config/kfetch", home);
    char cmd[600];
    snprintf(cmd, sizeof(cmd), "mkdir -p %s", dir);
    system(cmd);

    FILE *f = fopen(config_path(), "w");
    if (!f) { printf("Could not save config!\n"); return; }
    fprintf(f, "# kfetch config\n");
    fprintf(f, "user=%s\n",           cfg.user);
    fprintf(f, "host=%s\n",           cfg.host);
    fprintf(f, "show_os=%d\n",        cfg.show_os);
    fprintf(f, "show_kernel=%d\n",    cfg.show_kernel);
    fprintf(f, "show_shell=%d\n",     cfg.show_shell);
    fprintf(f, "show_ram=%d\n",       cfg.show_ram);
    fprintf(f, "show_uptime=%d\n",    cfg.show_uptime);
    fprintf(f, "show_cpu=%d\n",       cfg.show_cpu);
    fprintf(f, "show_disk=%d\n",      cfg.show_disk);
    fprintf(f, "show_packages=%d\n",  cfg.show_packages);
    fprintf(f, "show_de=%d\n",        cfg.show_de);
    fprintf(f, "show_wm=%d\n",        cfg.show_wm);
    fprintf(f, "show_terminal=%d\n",  cfg.show_terminal);
    fclose(f);
}

static struct termios orig_term;

static void tui_raw(void) {
    tcgetattr(STDIN_FILENO, &orig_term);
    struct termios t = orig_term;
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

static void tui_restore(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);
}

static void tui_clear(void)       { printf("\033[2J\033[H"); }
static void tui_hide_cursor(void) { printf("\033[?25l"); }
static void tui_show_cursor(void) { printf("\033[?25h"); }

#define MAX_INFO_ITEMS 12
static const char *info_labels[MAX_INFO_ITEMS] = {
    "OS/Distro", "Kernel", "Shell", "RAM", "Uptime",
    "CPU", "Disk", "Packages", "Desktop Env", "Window Mgr", "Terminal"
};
static int *info_flags[MAX_INFO_ITEMS] = {
    &cfg.show_os, &cfg.show_kernel, &cfg.show_shell, &cfg.show_ram, &cfg.show_uptime,
    &cfg.show_cpu, &cfg.show_disk, &cfg.show_packages, &cfg.show_de, &cfg.show_wm, &cfg.show_terminal
};
static int num_info_items = 11;

static void draw_tui(int step, int sel, int scroll, char *input, int input_len) {
    (void)input_len;
    tui_clear();
    printf(BOLD RED   "  ██╗  ██╗███████╗███████╗████████╗ ██████╗██╗  ██╗\n" RESET);
    printf(BOLD RED   "  ██║ ██╔╝██╔════╝██╔════╝╚══██╔══╝██╔════╝██║  ██║\n" RESET);
    printf(BOLD BLUE  "  █████╔╝ █████╗  █████╗     ██║   ██║     ███████║\n" RESET);
    printf(BOLD BLUE  "  ██╔═██╗ ██╔══╝  ██╔══╝     ██║   ██║     ██╔══██║\n" RESET);
    printf(BOLD RED   "  ██║  ██╗██║     ███████╗   ██║   ╚██████╗██║  ██║\n" RESET);
    printf(BOLD RED   "  ╚═╝  ╚═╝╚═╝     ╚══════╝   ╚═╝    ╚═════╝╚═╝  ╚═╝\n" RESET);
    printf(BOLD WHITE "              lightweight system fetch\n\n" RESET);

    if (step == 0) {
        printf(BOLD WHITE "  Enter your " RESET BOLD RED "username" RESET BOLD WHITE ":\n" RESET);
        printf("  > %s_\n\n", input);
        printf(BOLD WHITE "  (Press " RESET CYAN "Enter" RESET BOLD WHITE " to confirm)\n" RESET);
    } else if (step == 1) {
        printf(BOLD WHITE "  Enter your " RESET BOLD BLUE "hostname" RESET BOLD WHITE ":\n" RESET);
        printf("  > %s_\n\n", input);
        printf(BOLD WHITE "  (Press " RESET CYAN "Enter" RESET BOLD WHITE " to confirm)\n" RESET);
    } else if (step == 2) {
        printf(BOLD WHITE "  Select info to display " RESET CYAN "(↑↓ move, Space toggle, Enter done)" RESET "\n\n");
        int visible = 10;
        int start = scroll;
        int end = start + visible;
        if (end > num_info_items) end = num_info_items;
        for (int i = start; i < end; i++) {
            printf(i == sel ? BOLD RED "  ▶ " RESET : "    ");
            printf("[%s] %s\n", *info_flags[i] ? BOLD BGREEN "✓" RESET : " ", info_labels[i]);
        }
        if (num_info_items > visible) {
            printf("\n  " CYAN "scroll: %d/%d" RESET, sel+1, num_info_items);
        }
        printf("\n");
    } else if (step == 3) {
        printf(BOLD BGREEN "  ✓ Setup complete!\n\n" RESET);
        printf("  user:   " BOLD RED  "%s" RESET "\n", cfg.user);
        printf("  host:   " BOLD BLUE "%s" RESET "\n", cfg.host);
        printf("  Config saved to " CYAN "~/.config/kfetch/kfetch.conf" RESET "\n\n");
        printf(BOLD WHITE "  Press any key to launch kfetch...\n" RESET);
    }
}

static void run_tui(void) {
    tui_raw();
    tui_hide_cursor();

    char input[64];
    int  input_len = 0;
    int  step      = 0;
    int  sel       = 0;
    int  scroll    = 0;

    strncpy(input, cfg.user, sizeof(input));
    input_len = strlen(input);

    while (1) {
        draw_tui(step, sel, scroll, input, input_len);

        char c;
        read(STDIN_FILENO, &c, 1);

        if (step == 0 || step == 1) {
            if (c == '\n' || c == '\r') {
                input[input_len] = 0;
                if (step == 0) {
                    strncpy(cfg.user, input, sizeof(cfg.user));
                    strncpy(input, cfg.host, sizeof(input));
                    input_len = strlen(input);
                } else {
                    strncpy(cfg.host, input, sizeof(cfg.host));
                    input_len = 0;
                    input[0]  = 0;
                }
                step++;
            } else if ((c == 127 || c == '\b') && input_len > 0) {
                input[--input_len] = 0;
            } else if (input_len < 62 && c >= 32 && c < 127) {
                input[input_len++] = c;
                input[input_len]   = 0;
            }
        } else if (step == 2) {
            if (c == '\033') {
                char seq[2];
                read(STDIN_FILENO, seq, 1);
                read(STDIN_FILENO, seq + 1, 1);
                if (seq[0] == '[') {
                    if (seq[1] == 'A' && sel > 0) {
                        sel--;
                        if (sel < scroll) scroll = sel;
                    }
                    if (seq[1] == 'B' && sel < num_info_items - 1) {
                        sel++;
                        if (sel >= scroll + 10) scroll = sel - 9;
                    }
                }
            } else if (c == ' ') {
                *info_flags[sel] ^= 1;
            } else if (c == '\n' || c == '\r') {
                step++;
            }
        } else if (step == 3) {
            break;
        }
    }

    tui_show_cursor();
    tui_restore();
    tui_clear();
    save_config();
}

#define ASCII_CHARS " .,:;i1tfLCG08@"
#define ASCII_LEVELS (int)(sizeof(ASCII_CHARS) - 1)

typedef struct {
    unsigned char *pixels; 
    int            w;
    int            h;
} GrayImage;

static void gray_free(GrayImage *img) {
    free(img->pixels);
    img->pixels = NULL;
}

static const char *expand_path(const char *path, char *buf, size_t buflen) {
    if (path[0] == '~' && path[1] == '/') {
        char *home = getenv("HOME");
        if (!home) home = "/root";
        snprintf(buf, buflen, "%s/%s", home, path + 2);
        return buf;
    }
    return path;
}

static int load_png(const char *path, GrayImage *out) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) { fclose(f); return 0; }
    png_infop info = png_create_info_struct(png);
    if (!info) { png_destroy_read_struct(&png, NULL, NULL); fclose(f); return 0; }
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(f);
        return 0;
    }

    png_init_io(png, f);
    png_read_info(png, info);

    int w        = png_get_image_width(png, info);
    int h        = png_get_image_height(png, info);
    int color    = png_get_color_type(png, info);
    int bitdepth = png_get_bit_depth(png, info);

    if (bitdepth == 16) png_set_strip_16(png);
    if (color == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (color == PNG_COLOR_TYPE_GRAY && bitdepth < 8) png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
    if (color == PNG_COLOR_TYPE_RGB  ||
        color == PNG_COLOR_TYPE_GRAY ||
        color == PNG_COLOR_TYPE_PALETTE) png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    if (color == PNG_COLOR_TYPE_GRAY ||
        color == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png);
    png_read_update_info(png, info);

    png_bytep *rows = malloc(h * sizeof(png_bytep));
    for (int y = 0; y < h; y++)
        rows[y] = malloc(png_get_rowbytes(png, info));
    png_read_image(png, rows);

    out->w = w; out->h = h;
    out->pixels = malloc(w * h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = x * 4;
            unsigned char r = rows[y][idx];
            unsigned char g = rows[y][idx + 1];
            unsigned char b = rows[y][idx + 2];
            out->pixels[y * w + x] = (unsigned char)(0.299f*r + 0.587f*g + 0.114f*b);
        }
        free(rows[y]);
    }
    free(rows);
    png_destroy_read_struct(&png, &info, NULL);
    fclose(f);
    return 1;
}

static int load_jpg(const char *path, GrayImage *out) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, f);
    jpeg_read_header(&cinfo, TRUE);
    cinfo.out_color_space = JCS_RGB;
    jpeg_start_decompress(&cinfo);

    int w = cinfo.output_width;
    int h = cinfo.output_height;
    int ch = cinfo.output_components;
    out->w = w; out->h = h;
    out->pixels = malloc(w * h);

    unsigned char *row = malloc(w * ch);
    int y = 0;
    while ((int)cinfo.output_scanline < h) {
        jpeg_read_scanlines(&cinfo, &row, 1);
        for (int x = 0; x < w; x++) {
            unsigned char r = row[x * ch];
            unsigned char g = row[x * ch + 1];
            unsigned char b = row[x * ch + 2];
            out->pixels[y * w + x] = (unsigned char)(0.299f*r + 0.587f*g + 0.114f*b);
        }
        y++;
    }
    free(row);
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(f);
    return 1;
}

static char sample_char(GrayImage *img, int x0, int y0, int x1, int y1) {
    long sum = 0, count = 0;
    for (int y = y0; y < y1 && y < img->h; y++)
        for (int x = x0; x < x1 && x < img->w; x++) {
            sum += img->pixels[y * img->w + x];
            count++;
        }
    if (!count) return ' ';
    int avg = (int)(sum / count);
    int idx = avg * (ASCII_LEVELS - 1) / 255;
    return ASCII_CHARS[idx];
}

static char **image_to_ascii(GrayImage *img, int cols, int rows) {
    char **out = malloc(rows * sizeof(char *));
    for (int row = 0; row < rows; row++) {
        out[row] = malloc(cols + 1);
        for (int col = 0; col < cols; col++) {
            int x0 = col     * img->w / cols;
            int x1 = (col+1) * img->w / cols;
            int y0 = row     * img->h / rows;
            int y1 = (row+1) * img->h / rows;
            out[row][col] = sample_char(img, x0, y0, x1, y1);
        }
        out[row][cols] = '\0';
    }
    return out;
}

static void free_ascii(char **ascii, int rows) {
    for (int i = 0; i < rows; i++) free(ascii[i]);
    free(ascii);
}

static void build_info(char info[][256], int *ni, Distro *d) {
    char os[128], kernel[128], shell[64], ram[64], uptime[64];
    char cpu[128], disk[64], packages[64], de[64], wm[64], terminal[64];
    char separator[64];

    get_distro_name(os, sizeof(os));
    get_kernel(kernel, sizeof(kernel));
    get_shell(shell, sizeof(shell));
    get_ram(ram, sizeof(ram));
    get_uptime(uptime, sizeof(uptime));
    get_cpu(cpu, sizeof(cpu));
    get_disk(disk, sizeof(disk));
    get_packages(packages, sizeof(packages));
    get_de(de, sizeof(de));
    get_wm(wm, sizeof(wm));
    get_terminal(terminal, sizeof(terminal));

    snprintf(info[(*ni)++], 256,
        BOLD RED "%s" RESET "@" BOLD BLUE "%s" RESET, cfg.user, cfg.host);

    int seplen = strlen(cfg.user) + 1 + strlen(cfg.host);
    if (seplen > 63) seplen = 63;
    memset(separator, '-', seplen);
    separator[seplen] = 0;
    snprintf(info[(*ni)++], 256, "%s", separator);

    if (cfg.show_os)       snprintf(info[(*ni)++], 256, BOLD "%s" RESET "OS"       RESET ": %s", d->color1, os);
    if (cfg.show_kernel)   snprintf(info[(*ni)++], 256, BOLD "%s" RESET "Kernel"   RESET ": %s", d->color1, kernel);
    if (cfg.show_shell)    snprintf(info[(*ni)++], 256, BOLD "%s" RESET "Shell"    RESET ": %s", d->color1, shell);
    if (cfg.show_ram)      snprintf(info[(*ni)++], 256, BOLD "%s" RESET "RAM"      RESET ": %s", d->color1, ram);
    if (cfg.show_uptime)   snprintf(info[(*ni)++], 256, BOLD "%s" RESET "Uptime"   RESET ": %s", d->color1, uptime);
    if (cfg.show_cpu)      snprintf(info[(*ni)++], 256, BOLD "%s" RESET "CPU"      RESET ": %s", d->color1, cpu);
    if (cfg.show_disk)     snprintf(info[(*ni)++], 256, BOLD "%s" RESET "Disk"     RESET ": %s", d->color1, disk);
    if (cfg.show_packages) snprintf(info[(*ni)++], 256, BOLD "%s" RESET "Packages" RESET ": %s", d->color1, packages);
    if (cfg.show_de)       snprintf(info[(*ni)++], 256, BOLD "%s" RESET "DE"       RESET ": %s", d->color1, de);
    if (cfg.show_wm)       snprintf(info[(*ni)++], 256, BOLD "%s" RESET "WM"       RESET ": %s", d->color1, wm);
    if (cfg.show_terminal) snprintf(info[(*ni)++], 256, BOLD "%s" RESET "Terminal" RESET ": %s", d->color1, terminal);
}

static void render(Distro *d) {
    char info[20][256];
    int  ni = 0;
    build_info(info, &ni, d);

    int max_w = 0;
    for (int i = 0; i < d->num_lines; i++) {
        int w = strlen(d->lines[i]);
        if (w > max_w) max_w = w;
    }

    int total      = d->num_lines > ni ? d->num_lines : ni;
    int info_start = (d->num_lines - ni) / 2;
    if (info_start < 0) info_start = 0;

    printf("\n");
    for (int i = 0; i < total; i++) {
        if (i < d->num_lines) {
            int w = strlen(d->lines[i]);
            printf("  %s%s%s%*s", i < 2 ? d->color1 : d->color2, d->lines[i], RESET, max_w - w, "");
        } else {
            printf("  %*s", max_w, "");
        }

        int ii = i - info_start;
        if (ii >= 0 && ii < ni)
            printf("  %s", info[ii]);
        printf("\n");
    }
    printf("\n");
}

static void render_image(GrayImage *img, int img_cols, int img_rows, Distro *d) {
    char **ascii = image_to_ascii(img, img_cols, img_rows);

    char info[20][256];
    int  ni = 0;
    build_info(info, &ni, d);

    int total      = img_rows > ni ? img_rows : ni;
    int info_start = (img_rows - ni) / 2;
    if (info_start < 0) info_start = 0;

    printf("\n");
    for (int i = 0; i < total; i++) {
        if (i < img_rows)
            printf("  %s", ascii[i]);
        else
            printf("  %*s", img_cols, "");

        int ii = i - info_start;
        if (ii >= 0 && ii < ni)
            printf("  %s", info[ii]);
        printf("\n");
    }
    printf("\n");

    free_ascii(ascii, img_rows);
}

static Distro *get_distro_by_name(const char *name) {
    char lower[64];
    strncpy(lower, name, sizeof(lower) - 1);
    lower[sizeof(lower) - 1] = 0;
    for (int i = 0; i < (int)strlen(lower); i++)
        lower[i] = tolower((unsigned char)lower[i]);
    for (int i = 0; i < NUM_DISTROS; i++)
        if (strcmp(distros[i].name, lower) == 0) return &distros[i];
    return NULL;
}

static void get_term_size(int *cols, int *rows) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
    } else {
        *cols = 80;
        *rows = 24;
    }
}

static void open_config(void) {
    const char *path = config_path();
    if (access(path, F_OK) != 0) {
        load_config();
        save_config();
    }
    const char *editors[] = {
        getenv("VISUAL"),
        getenv("EDITOR"),
        "nano", "vim", "vi", "notepad"
    };
    for (int i = 0; i < 6; i++) {
        if (!editors[i]) continue;
        char cmd[640];
        snprintf(cmd, sizeof(cmd), "%s %s", editors[i], path);
        int ret = system(cmd);
        if (ret == 0 || ret == 1) return;
    }
    fprintf(stderr, "No editor found. Set $EDITOR or $VISUAL.\n");
}

static void print_help(void) {
    printf(BOLD "kfetch" RESET " - lightweight system fetch\n\n");
    printf(BOLD "USAGE\n" RESET);
    printf("  kfetch [options]\n\n");
    printf(BOLD "OPTIONS\n" RESET);
    printf("  " BOLD "--logo" RESET " <distro>      Force a specific distro logo\n");
    printf("  " BOLD "--logo" RESET " <path>        Render a JPG or PNG as ASCII art logo\n");
    printf("  " BOLD "--size" RESET " <WxH>         Set ASCII image size in terminal columns x rows\n");
    printf("  " BOLD "--setup" RESET "               Re-run the interactive setup wizard\n");
    printf("  " BOLD "--config" RESET "              Open the config file in $EDITOR\n");
    printf("  " BOLD "--help" RESET "                Show this message\n\n");
    printf(BOLD "DISTRO LOGOS\n" RESET);
    printf("  arch artix alpine bedrock cachyos fedora debian ubuntu gentoo opensuse opensuse-tumbleweed opensuse-leap void windows devuan parabola blackarch\n\n");
    printf(BOLD "EXAMPLES\n" RESET);
    printf("  kfetch\n");
    printf("  kfetch --logo arch\n");
    printf("  kfetch --logo opensuse-tumbleweed\n");
    printf("  kfetch --logo ~/pictures/avatar.png --size 40x20\n");
    printf("  kfetch --setup\n");
    printf("  kfetch --config\n");
}

int main(int argc, char *argv[]) {
    int         do_setup   = 0;
    int         do_config  = 0;
    int         do_help    = 0;
    Distro     *forced     = NULL;
    const char *img_path   = NULL;
    int         img_cols   = 0;   
    int         img_rows   = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--setup") == 0) {
            do_setup = 1;
        } else if (strcmp(argv[i], "--config") == 0) {
            do_config = 1;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            do_help = 1;
        } else if (strcmp(argv[i], "--size") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "kfetch: --size requires an argument (e.g. 40x20)\n");
                return 1;
            }
            if (sscanf(argv[++i], "%dx%d", &img_cols, &img_rows) != 2
                || img_cols <= 0 || img_rows <= 0) {
                fprintf(stderr, "kfetch: invalid size '%s', expected WxH (e.g. 40x20)\n", argv[i]);
                return 1;
            }
        } else if (strcmp(argv[i], "--logo") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "kfetch: --logo requires an argument\n");
                fprintf(stderr, "  distros: arch, artix, alpine, bedrock, cachyos, fedora, debian, ubuntu, gentoo, opensuse, opensuse-tumbleweed, opensuse-leap, void, windows, devuan, parabola, blackarch\n");
                fprintf(stderr, "  or a path to a .jpg/.png file\n");
                return 1;
            }
            const char *arg = argv[++i];
            forced = get_distro_by_name(arg);
            if (!forced) img_path = arg;ten in C. It displays a distribution ASCII logo alongside key system details directly in your terminal.

        }
    }

    if (do_help) { print_help(); return 0; }
    if (do_config) { open_config(); return 0; }

    if (!do_setup && access(config_path(), F_OK) != 0) do_setup = 1;
    if (do_setup) run_tui();

    load_config();

    if (img_path) {
        char pathbuf[512];
        const char *real = expand_path(img_path, pathbuf, sizeof(pathbuf));
        GrayImage img = {0};
        int ok = 0;
        const char *ext = strrchr(real, '.');
        if (ext) {
            char extl[8] = {0};
            strncpy(extl, ext + 1, 7);
            for (int j = 0; j < 7; j++) extl[j] = tolower((unsigned char)extl[j]);
            if (!strcmp(extl, "png"))
                ok = load_png(real, &img);
            else if (!strcmp(extl, "jpg") || !strcmp(extl, "jpeg"))
                ok = load_jpg(real, &img);
            else {
                ok = load_png(real, &img);
                if (!ok) ok = load_jpg(real, &img);
            }
        } else {
            ok = load_png(real, &img);
            if (!ok) ok = load_jpg(real, &img);
        }
        if (!ok) {
            fprintf(stderr, "kfetch: could not load image: %s\n", real);
            return 1;
        }
        if (img_cols == 0 || img_rows == 0) {
            int tcols, trows;
            get_term_size(&tcols, &trows);
            img_cols = tcols * 2 / 5;
            img_rows = img_cols * img.h / img.w / 2;
            if (img_rows < 4) img_rows = 4;
        }
        Distro *d = forced ? forced : get_distro(detect_distro());
        render_image(&img, img_cols, img_rows, d);
        gray_free(&img);
        return 0;
    }

    Distro *d = forced ? forced : get_distro(detect_distro());
    render(d);
    return 0;
}
