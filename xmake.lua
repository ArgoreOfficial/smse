
set_project( "smse" )
set_version( "0.0.1" )

if is_mode("Debug") then
    set_symbols "debug"
    set_optimize "none"
    set_strip "none"
elseif is_mode("Release") then 
    set_symbols "debug"
    set_optimize "fast"
    set_strip "debug"
end

set_languages( "cxx17" )

add_includedirs( "include", "smse" )

add_requires( "minhook" )

includes( "loader/" )
includes( "smse/" )