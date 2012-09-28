%%
%% Copyright (C) 2012, All rights reserved.
%% Jorge Garrido <jorge.garrido@morelosoft.com>
%% ehecatl_app.erl
%%

-module(ehecatl_app).

-behaviour(application).

%% Application callbacks
-export([start/2, stop/1]).

%% ===================================================================
%% Application callbacks
%% ===================================================================

start(_StartType, _StartArgs) ->
    _ref = ehecatl:start(),
    ehecatl_sup:start_link().

stop(_State) ->
    ok.

