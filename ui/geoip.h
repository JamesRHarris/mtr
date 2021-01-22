#ifndef GEOIP_H
#define GEOIP_H
struct mtr_ctl;
void geoip_lookup(struct mtr_ctl*, char*, char**, char**);
int geoip_init(struct mtr_ctl*);
#endif
