/*
 * By orbyfied (2022)
 * License can be found at https://github.com/orbyfiedistrash/picrun
 */

#include "main.h"

#include "vm.h"
#include "bmp.h"

#include <cstdio>
#include <vector>

struct MainArgs* pr_main_args;

void print_ma_usage() {
    pr_printheader();
    printf("Missing arguments.\n");
    printf("Usage: picrun <file> [--workdir <dir>]\n");
    printf("\n");
}

// main entry point
// it sucks
int main(int argc, char** argv) {
    // get binary path
    char* bin_path = argv[0];

    // check arguments
    if (argc < 2) {
        BITMAPFILEHEADER bfh;
        BITMAPINFOHEADER bih;
        int bcode;
        unsigned long long bdatlen;
        int_col* buf = read_bmp_file("../data.bmp", &bfh, &bih, &bdatlen, &bcode);

        printf("bfsize: %d, bftype: %d, bfpixoff: %d\n", bfh.bfSize, bfh.bfType, bfh.bfOffBits);
        printf("data | bdatlen: %d\n", bdatlen);
        for (int i = 0; i < bdatlen; i++) {
            int_col col = buf[i];
            col_rgb rgb = col.rgb;
            printf("- i: %d, rgb: %d,%d,%d,%d, intf: %d\n", i, rgb.r, rgb.g, rgb.b, rgb.a, col.i);
        }

        print_ma_usage();
        return -1;
    }

    // parse args
    pr_main_args = { };

    // positioned arguments
    std::vector<std::string> positioned_args;
    // argument index
    int argi = 0;
    // next argument macro
    auto next_arg = [&] () {
        return std::string(argv[argi += 1]);
    };
    // for every argument after arg 0
    while (argi < argc) {
        // construct string
        std::string arg = next_arg();

        // check if it is a named argument start
        if (str_starts_with(arg, "--")) {
            // get name
            std::string name = str_sub(arg, 2);

            switch (strhash(name)) {
                // set working directory
                case strhash("workdir"):
                    std::string dir = next_arg();
                    dir = fix_path_slash(dir);
                    pr_main_args->work_dir = dir.c_str();
                    break;
            }
        } else {
            // append to positioned
            positioned_args.push_back(arg);
        }
    }

    // set missing properties
    if (positioned_args.empty()) {
        print_ma_usage();
        return -1;
    }

    pr_main_args->entry_file = positioned_args[0].c_str();
    if (pr_main_args->work_dir == nullptr)
        pr_main_args->work_dir = ".";

    printf("%s\n%s", pr_main_args->entry_file, pr_main_args->work_dir);

    // return
    return 0;
}
