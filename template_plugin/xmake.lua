local PLUGIN_NAME = "SMSEPlugin_Template"

target( PLUGIN_NAME )
    set_kind( "shared" )
    
	if is_mode( "Debug" ) then 
    	set_basename( PLUGIN_NAME .. "_dbg" )
	else is_mode( "Release" )
    	set_basename( PLUGIN_NAME )
	end

    set_targetdir "../bin"
    set_objectdir "../build/obj"

    add_headerfiles( "**.h", "**.hpp" )
    add_files( "**.cpp" )
target_end()