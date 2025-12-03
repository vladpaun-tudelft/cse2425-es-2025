#ifndef GEST_RUN_H
#define GEST_RUN_H

#include "gest.h"

void register_tests();

int main(int argc, char *argv[]) {
    register_tests();
    run_suite();
	show_results();
    unregister_tests();
    
    return 0;
}

#endif // GEST_RUN_H
