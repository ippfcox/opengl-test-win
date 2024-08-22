#include <stdio.h>
#include "log.h"

int main()
{
    set_log_level(LOG_LEVEL_DEBUG);
    loginfo("123456");
}