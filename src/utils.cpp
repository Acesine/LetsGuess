#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "utils.h"

std::string convertToString(char * s) {
        std::string str = "";
        while (s != NULL && *s != '\0') {
                str.push_back(*s);
                s ++;
        }
        return str;
}
