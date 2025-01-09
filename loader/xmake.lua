target( "smse_loader" )
    set_kind( "binary" )
    
	if is_mode( "Debug" ) then 
    	set_basename("smse_loader_dbg")
	else is_mode( "Release" )
    	set_basename("smse_loader")
	end

    set_targetdir "../bin"
    set_objectdir "../build/obj"

    add_headerfiles( "**.h", "**.hpp" )
    add_files( "**.cpp" )
target_end()