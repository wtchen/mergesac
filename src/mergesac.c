/*
mergesac.c
Merges multiple SAC files, given as command line arguments (in chronological order).
-------------------------------
Copyright (c) 2014 William Chen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "sachead64.h"

struct SAChead *nh;

float **datasets;
float **infill;

long *infill_npts;

time_t *start_time;
time_t *end_time;

void merge_sac(int, char**);
void decode_sac(int, char *);
void encode_sac();
time_t getUnixTime(int, int, int, int, int);
int arswap(unsigned char*, int);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("File name required! Aborting.\n");
        exit(1);
    }
    merge_sac(argc, argv);
}

void merge_sac(int argc, char *argv[])
{
    long gap_npts = 0;
    nh = malloc(sizeof(struct SAChead)*(argc-1));
    datasets = malloc(sizeof(float*)*(argc-1));
    infill = malloc(sizeof(float*)*(argc-1));
    infill_npts = malloc(sizeof(long)*(argc-1));
    start_time = malloc(sizeof(time_t)*(argc-1));
    end_time = malloc(sizeof(time_t)*(argc-1));
    decode_sac(0, argv[1]);
    int arg_iter = 0;
    while (argc >= 3 && arg_iter < (argc-2))
    {
        decode_sac(1+arg_iter, argv[2+arg_iter]);
        // since decode_sac gives the time of the last data point, the gap between the two datasets
        // is 1 less than the difference between the starting time of the second dataset and the
        // ending time of the first dataset; i.e. if the first dataset ends at t=99s and the second
        // dataset starts at t=101s, then there is a gap of only one
        long gap = labs((long)(start_time[1+arg_iter]-end_time[0+arg_iter]-1));
        if (nh[0+arg_iter].delta != nh[1+arg_iter].delta)
        {
            printf("Data frequencies differ between files! Aborting.\n");
            exit(1);
        }
        gap_npts = (long)(gap / nh[0+arg_iter].delta);
        float slope = datasets[1+arg_iter][0] - datasets[0+arg_iter][nh[0+arg_iter].npts-1];
        float dy_gap = slope/gap_npts;
        // fill in gap using simple linear interpolation
        int i = 0;
        infill[arg_iter] = malloc(sizeof(float)*gap_npts);
        float cur_gap_val = datasets[0+arg_iter][nh[0+arg_iter].npts-1];
        while (i < gap_npts)
        {
            cur_gap_val+=dy_gap;
            infill[arg_iter][i] = cur_gap_val;
            i++;
        }
        infill_npts[arg_iter] = gap_npts;
        arg_iter++;
    }
    encode_sac(argc-1);
    free(datasets);
    free(infill);
    free(infill_npts);
    free(start_time);
    free(end_time);
}

void decode_sac(int index, char *arg)
{
    float *data;
    unsigned char *array;
    float dt;
    int bitrev, nd;

    FILE *fildes = fopen(arg, "rb");

    if (fildes < 0) exit(1);

    printf("Processing %s ...", arg);

    fread(&nh[index], SACHEADERSIZE, 1, fildes);

    dt = nh[index].delta;
    bitrev = 0;
    if (dt < 0.00001 || dt > 1.0e6) bitrev=1;
    if (bitrev != 0)
    {
        array= (unsigned char *)&nh[index];
        arswap(array, SACHEADERSIZE/4);
    }

    nd = nh[index].npts;
    dt = nh[index].delta;

    data = malloc(sizeof(float)*nh[index].npts);
    fread(data, 4*nh[index].npts, 1, fildes);
    if (bitrev != 0)
    {
        array= (unsigned char *)data;
        arswap(array,nh[index].npts);
    }
    time_t startTime;
    startTime = getUnixTime(nh[index].nzyear, nh[index].nzjday, nh[index].nzhour, nh[index].nzmin, nh[index].nzsec);

    int i = 0;
    datasets[index] = malloc(sizeof(float)*nh[index].npts);
    while (i <= nh[index].npts)
    {
        datasets[index][i] = data[i];
        i++;
    }
    fclose(fildes);
    start_time[index] = (long)startTime;
    free(data);
    // saves the end time (time of the last sample) ... note the fence post rule!
    // i.e. if you have 100 data points sampled at 1 Hz starting at t=0s end time would be t=99s
    end_time[index] = startTime + (int)nh[index].delta*(nh[index].npts - 1);
    printf(" done!\n");
}

void encode_sac(int num_files)
{
    char *station = strtok(nh[0].kstnm, " ");
    char *channel = strtok(nh[0].kcmpnm, " ");
    char *network = strtok(NULL, " ");
    struct tm *t_start = gmtime(&start_time[0]);
    char new_sac_file_name[256];
    snprintf(new_sac_file_name, sizeof(new_sac_file_name),
             "%4d.%3d.%02d.%02d.%02d.0000_", t_start->tm_year + 1900,
             t_start->tm_yday + 1, t_start->tm_hour, t_start->tm_min,
             t_start->tm_sec);
    end_time[num_files-1]++;
    struct tm *t_end = gmtime(&(end_time[num_files-1]));
    snprintf(new_sac_file_name, sizeof(new_sac_file_name),
             "%s%4d.%3d.%02d.%02d.%02d.0000.%s.%s..%s.M.SAC", new_sac_file_name,
             t_end->tm_year + 1900, t_end->tm_yday + 1, t_end->tm_hour,
             t_end->tm_min, t_end->tm_sec, network, station, channel);

    int fil = creat(new_sac_file_name, 0644);
    long total_data_length = 0;
    int i = 0;
    for (i = 0; i < num_files; i++)
    {
        total_data_length+=nh[i].npts;
        if (i < (num_files-1))
        {
            total_data_length+=infill_npts[i];
        }
    }
    float *data = malloc(sizeof(float)*(total_data_length));
    memcpy(data, datasets[0], nh[0].npts*sizeof(float));
    float *data_ptr = data + nh[0].npts;
    for (i = 1; i < num_files; i++)
    {
        memcpy(data_ptr, infill[i-1], infill_npts[i-1]*sizeof(float));
        memcpy(data_ptr+infill_npts[i-1], datasets[i], nh[i].npts*sizeof(float));
        data_ptr = data_ptr + nh[i].npts + infill_npts[i-1];
    }
    nh[0].npts = (int)total_data_length;
    write(fil, &nh[0], SACHEADERSIZE);
    write(fil, data, 4*(nh[0].npts));
    close(fil);
    printf("New SAC file at %s\n", new_sac_file_name);
}

time_t getUnixTime(int year, int jday, int hour, int min, int sec)
{
    year-=1900;
    // yday is where the first day of the year is considered day 0 of the year
    int yday = jday - 1;
    // POSIX standard time formula (official specification of seconds since epoch)
    return sec + min*60 + hour*3600 + yday*86400 + (year-70)*31536000 +
           ((year-69)/4)*86400 - ((year-1)/100)*86400 + ((year+299)/400)*86400;
}

/* byte swap subroutine for different byte-order machine
Just in case the data were generated by the other (Big endian vs. Little endian)
byte order, this routine is attached.
If you decoded data by a Mac machine (INTEL-based), the binary data are in
little endian. Then you can read by a program compiled by Mac WITHOUT USING THIS
ROUTINE.
*/
int arswap(unsigned char *array, int nps)
{
    int i;
    unsigned int n;
    unsigned char a;

    for (i = 0; i < nps; i++)
    {
        n = i * 4;
        a = array[n];
        array[n] = array[n+3];
        array[n+3] = a;
        a = array[n+1];
        array[n+1] = array[n+2];
        array[n+2] = a;
    }
    return 1;
}
