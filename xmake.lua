
set_project( "SMSE" )
set_version( "0.0.1" )

if is_mode("Debug") then
    set_symbols "debug"
    set_optimize "none"

    add_defines( "SMSE_DEBUG" )
elseif is_mode("Release") then 
    set_symbols "debug"
    set_optimize "fast"

    add_defines( "SMSE_RELEASE" )
end

set_languages( "cxx17" )

add_includedirs( "include", "smse" )

add_requires( "minhook" )
add_requires( "luajit", { configs = { shared = false }} )

includes( "loader/" )
includes( "smse/" )
includes( "template_plugin/" )