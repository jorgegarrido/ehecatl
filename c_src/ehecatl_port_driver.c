/*
* 
* Copyright (C) 2012, All rights reserved.
* Jorge Garrido <jorge.garrido@morelosoft.com>
* ehecatl_port_driver.c 
*
*/


/* include libraries */
#include <stdio.h>
#include <string.h>
#include "erl_driver.h"
#include "winscard.h"
#include "sys/time.h"
#include "stdlib.h"

// erl_port struct, connecting to erl system 
typedef struct {
    ErlDrvPort port;
} erl_port;

// card struct, stores the card response
typedef struct {
    char *response;
} card;

// function ehecatl_driver_start, starts the erlang port-driver
static ErlDrvData ehecatl_driver_start(ErlDrvPort port, char *buff)
{
    erl_port* to_erl_port = (erl_port*)driver_alloc(sizeof(port));
    to_erl_port->port = port;
    return (ErlDrvData)to_erl_port;
}

// function ehecatl_driver_stop, stops the erlang port-driver
static void ehecatl_driver_stop(ErlDrvData handle)
{
    driver_free((char*)handle);
}

// function ehecatl_driver_output, hanldes the ouput to erlang, when receives a 
// request from it (erlang request)
static void ehecatl_driver_output(ErlDrvData handle, char *buff, int bufflen)
{
    erl_port* to_erl_port = (erl_port*)handle;
    char fun = buff[0], arg = buff[1], res;

    if (fun == 1) {
      char* ehecatl_api = (char*)ehecatl_api_sc(arg);    
      driver_output(to_erl_port->port, ehecatl_api, strlen(ehecatl_api));
    } else {
      char* ehecatl_api = (char*)ehecatl_api_sc(arg);
      driver_output(to_erl_port->port, ehecatl_api, strlen(ehecatl_api));
    }
}

// configure the driver, functions to use by it
ErlDrvEntry ehecatl_driver_entry = {
    NULL,                       /* F_PTR init, N/A */
    ehecatl_driver_start,           /* L_PTR start, called when port is opened */
    ehecatl_driver_stop,            /* F_PTR stop, called when port is closed */
    ehecatl_driver_output,          /* F_PTR output, called when erlang has sent */
    NULL,                       /* F_PTR ready_input, called when input descriptor ready */
    NULL,                       /* F_PTR ready_output, called when output descriptor ready */
    "libehecatl",                  /* char *driver_name, the argument to open_port */
    NULL,                       /* F_PTR finish, called when unloaded */
    NULL,                       /* F_PTR control, port_command callback */
    NULL,                       /* F_PTR timeout, reserved */
    NULL                        /* F_PTR outputv, reserved */
};

// DRIVER INIT, driver initialization
DRIVER_INIT(libehecatl) /* must match name in driver_entry */
{
    return &ehecatl_driver_entry;
}
