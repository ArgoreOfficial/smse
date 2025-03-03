target( "SMSELoader" )
    set_kind( "binary" )
    
	if is_mode( "Debug" ) then 
    	set_basename("SMSELoader_dbg")
	else is_mode( "Release" )
    	set_basename("SMSELoader")
	end

    set_targetdir "../bin"
    set_objectdir "../build/obj"

    add_headerfiles( "**.h", "**.hpp" )
    add_files( "**.cpp" )
target_end()