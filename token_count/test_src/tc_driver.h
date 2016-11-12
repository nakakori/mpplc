/* test driver */

#ifndef _TC_DRIVER_H_
#define _TC_DRIVER_H_

#include "../src/token-list.h"
#include "../src/scan.h"
#include "../src/id-list.h"

extern void test_main(void);
void scan_file_driver(void);
void scan_self_driver(void);
extern int get_buf_string(void);

#endif /* _TC_DRIVER_H_ */
