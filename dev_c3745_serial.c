/*  
 * Cisco C3745 (Predator) simulation platform.
 * Copyright (c) 2006 Christophe Fillot (cf@utc.fr)
 *
 * Serial Network Modules.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

#include "utils.h"
#include "net.h"
#include "net_io.h"
#include "ptask.h"
#include "dev_mueslix.h"
#include "dev_c3745.h"

/* ====================================================================== */
/* NM-4T                                                                  */
/* ====================================================================== */

/*
 * dev_c3745_nm_4t_init()
 *
 * Add a NM-4T network module into specified slot.
 */
int dev_c3745_nm_4t_init(c3745_t *router,char *name,u_int nm_bay)
{
   struct mueslix_data *data;

   /* Set the EEPROM */
   c3745_nm_set_eeprom(router,nm_bay,cisco_eeprom_find_nm("NM-4T"));

   /* Create the Mueslix chip */
   data = dev_mueslix_init(router->vm,name,0,
                           router->nm_bay[nm_bay].pci_map,0,
                           C3745_NETIO_IRQ);

   if (!data) return(-1);

   /* Store device info into the router structure */
   return(c3745_nm_set_drvinfo(router,nm_bay,data));
}

/* Remove a NM-4T from the specified slot */
int dev_c3745_nm_4t_shutdown(c3745_t *router,u_int nm_bay) 
{
   struct c3745_nm_bay *bay;

   if (!(bay = c3745_nm_get_info(router,nm_bay)))
      return(-1);

   c3745_nm_unset_eeprom(router,nm_bay);
   dev_mueslix_remove(bay->drv_info);
   return(0);
}

/* Bind a Network IO descriptor to a specific port */
int dev_c3745_nm_4t_set_nio(c3745_t *router,u_int nm_bay,u_int port_id,
                            netio_desc_t *nio)
{
   struct mueslix_data *data;

   if ((port_id >= MUESLIX_NR_CHANNELS) || 
       !(data = c3745_nm_get_drvinfo(router,nm_bay)))
      return(-1);

   return(dev_mueslix_set_nio(data,port_id,nio));
}

/* Unbind a Network IO descriptor to a specific port */
int dev_c3745_nm_4t_unset_nio(c3745_t *router,u_int nm_bay,u_int port_id)
{
   struct mueslix_data *d;

   if ((port_id >= MUESLIX_NR_CHANNELS) || 
       !(d = c3745_nm_get_drvinfo(router,nm_bay)))
      return(-1);
   
   return(dev_mueslix_unset_nio(d,port_id));
}

/* NM-4T driver */
struct c3745_nm_driver dev_c3745_nm_4t_driver = {
   "NM-4T", 1, 0,
   dev_c3745_nm_4t_init, 
   dev_c3745_nm_4t_shutdown,
   dev_c3745_nm_4t_set_nio,
   dev_c3745_nm_4t_unset_nio,
   NULL,
};