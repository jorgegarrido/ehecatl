##
## Copyright (C) 2012, All rights reserved.
## Jorge Garrido <jorge.garrido@morelosoft.com>
## Makefile 
##

ERL_ROOT=/usr/local/lib/erlang
EHECATL_DIR_LIB=$(shell pwd)

.PHONY: c_src

all: compile_c compile

compile_c: c_src/ehecatl_api_sc.c c_src/ehecatl_port_driver.c
	@mkdir -p lib
	gcc -o lib/libehecatl.so -I$(ERL_ROOT)/usr/include/ -I/usr/include/PCSC /usr/lib/libpcsclite.so -fpic -rdynamic -shared c_src/ehecatl_api_sc.c c_src/ehecatl_port_driver.c 
	@ldconfig $(EHECATL_DIR_LIB)/

compile: 
	erlc -o ebin/ src/ehecatl_app.erl
	erlc -o ebin/ src/ehecatl_sup.erl
	erlc -o ebin/ src/ehecatl_gen.erl
	erlc -o ebin/ src/ehecatl.erl

clean: ebin lib
	@rm -rf ebin/*.beam
	@rm -rf lib
	@rm -rf log

ehecatl: ebin lib/libehecatl.so
	@mkdir -p log
	erl -pa ebin/ -eval 'application:start(ehecatl)'
