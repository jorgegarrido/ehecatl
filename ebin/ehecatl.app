%%
%% Copyright (C) 2012, All rights reserved.
%% Jorge Garrido <jorge.garrido@morelosoft.com>
%% ehecatl.app
%%

{application, ehecatl,[{description, "An erlang driver for write/read smart cards"},
		      {vsn, "0.1"},
  		      {registered, []},
  		      {applications, [ kernel,
                  		       stdlib
                 		     ]},
  		      {mod, { ehecatl_app, []}}
 		     ]}.
