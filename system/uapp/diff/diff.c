// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <magenta/syscalls.h>

#define BUF_SIZE   8192

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: diff <file 1> <file 2>\n");
        return -1;
    }

    int fd1 = open(argv[1], O_RDONLY);
    if (fd1 < 0) {
        fprintf(stderr, "could not open file %s\n", argv[1]);
        return -1;
    }
    int fd2 = open(argv[2], O_RDONLY);
    if (fd2 < 0) {
        fprintf(stderr, "could not open file %s\n", argv[2]);
        return -1;
    }
    
    char buffer1[BUF_SIZE];
    char buffer2[BUF_SIZE];
    int ret = 0;

    mx_time_t start = mx_time_get(MX_CLOCK_MONOTONIC);

    while (1) {
        int count1 = read(fd1, buffer1, BUF_SIZE);
        int count2 = read(fd2, buffer2, BUF_SIZE);
        if (count1 != count2) {
            fprintf(stderr, "read mismatch (files not same size?) %d %d\n", count1, count2);
            ret = -1;
            break;
        }
        if (count1 <= 0) break;
    
        if (memcmp(buffer1, buffer2, count1)) {
            fprintf(stderr, "files do not match\n");
            ret = -1;
            break;
        }
    }

    mx_time_t end = mx_time_get(MX_CLOCK_MONOTONIC);
    uint64_t usec = (end - start) / 1000;
    double seconds = ((double)usec / 1000000.0);

    printf("compare time: %lf seconds\n", seconds);

    close(fd1);
    close(fd2);

    return ret;
}
