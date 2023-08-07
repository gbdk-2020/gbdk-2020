
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "common.h"

bool exit_error                 = false;
void set_exit_error(void) {
    exit_error = true;
}

bool get_exit_error(void) {
    return exit_error;
}

