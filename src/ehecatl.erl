%%
%% Copyright (C) 2012, All rights reserved.
%% Jorge Garrido <jorge.garrido@morelosoft.com>
%% ehecatl.erl 
%%

-module(ehecatl).
-export([start/0, stop/0, init/1]).
-export([flush/1]).
-define(libehecatl, "libehecatl").


start() ->
    case erl_ddll:load_driver("lib", ?libehecatl) of
        ok                      ->
            ok;
        {error, already_loaded} ->
            ok;
        _                       ->
            exit({error, could_not_load_driver})
    end,
    spawn(?MODULE, init, [?libehecatl]).

init(Lib) ->
    register(ehecatl_driver, self()),
    Port = open_port({spawn, Lib}, []),
    loop(Port).

stop() ->
    ehecatl_driver ! stop.

flush(write) ->
    call_port({flush, {2, 2}});
flush(read) ->
    call_port({flush, {1, 1}});
flush(_)    ->
    "option not supported".

call_port(Msg) ->
    ehecatl_driver ! {call, self(), Msg},
    receive
        {ehecatl_driver, Res} ->
            Res
    end.

loop(Port) ->
    receive
        {call, Caller, Msg} ->
            Port ! {self(), {command, encode(Msg)}},
            receive
                {Port, {data, Data}} ->
                    Caller ! {ehecatl_driver, decode(Data)}
            end,
            loop(Port);
        stop ->
            Port ! {self(), close},
            receive
                {Port, closed} ->
                    exit(normal)
            end;
        {'EXIT', Port, Reason} ->
            io:format("~p ~n", [Reason]),
            exit(port_terminated)
    end.

encode({flush, {Fun, Arg}}) ->
    [Fun, Arg].

decode([RetVal]) ->
    success_card(RetVal);
decode(RetVal)   ->
    success_card(RetVal).

success_card([ A, B, C, D, 144]) ->
    [A, B, C, D];
success_card(144)                ->
    ok;
success_card(_)                  ->
    "error".
