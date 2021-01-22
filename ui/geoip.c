#include "mtr.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_ERROR_H
#include <error.h>
#else
#include "portability/error.h"
#endif

#define MAX_GEOIP_VAL 128
void
geoip_lookup(struct mtr_ctl *ctl,char* ip,char** country, char** city) {
   int gai_error, mmdb_error;
   static char cc[MAX_GEOIP_VAL];
   static char cty[MAX_GEOIP_VAL];
   MMDB_lookup_result_s r;
   MMDB_entry_data_list_s *entry_data_list = NULL;

   *city = *country = "Unknown";

   r = MMDB_lookup_string(&(ctl->GeoMMDB), ip, &gai_error, &mmdb_error);

   if (gai_error != 0) {
      error(0,0,"mmdb getadderinfo : %s",gai_strerror(gai_error));
      return;
   }

   if (mmdb_error != MMDB_SUCCESS) {
      error(0,0,"MMDB_lookup_string : %s",MMDB_strerror(mmdb_error));
      return;
   }

#define min(a,b) ((a) > (b) ? (b):(a))
   if (r.found_entry) {
            MMDB_entry_data_s ed;
            mmdb_error = MMDB_get_value(&r.entry, &ed,"country","names","en",NULL);
            if(mmdb_error == MMDB_SUCCESS  && ed.has_data &&
               ed.type == MMDB_DATA_TYPE_UTF8_STRING ) {

               strncpy(cc,ed.utf8_string,min(MAX_GEOIP_VAL -1,ed.data_size));
               cc[min(MAX_GEOIP_VAL,ed.data_size+1)] = '\0';
               *country = cc;
            }
            mmdb_error = MMDB_get_value(&r.entry, &ed,"city","names","en",NULL);
            if(mmdb_error == MMDB_SUCCESS  && ed.has_data &&
               ed.type == MMDB_DATA_TYPE_UTF8_STRING ) {
               strncpy(cty,ed.utf8_string,min(MAX_GEOIP_VAL -1,ed.data_size));
               cty[min(MAX_GEOIP_VAL,ed.data_size+1)] = '\0';
               *city = cty;
            }
   } else {
     //Not Found
   }
   if (entry_data_list) {
      MMDB_free_entry_data_list(entry_data_list);
   }
   return;
}

int
geoip_init(struct mtr_ctl *ctl) {
    if (MMDB_open(ctl->GeoIPFile, MMDB_MODE_MMAP, &(ctl->GeoMMDB)) != MMDB_SUCCESS) {
        error(0, 0, "unable to open %s", ctl->GeoIPFile);
        return -1;
    }
   return 0;
}

