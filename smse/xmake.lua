target( "SMSE" )
    set_kind( "shared" )
    
	add_packages("minhook")

	if is_mode( "Debug" ) then 
    	set_basename("SMSE_dbg")
	else is_mode( "Release" )
    	set_basename("SMSE")
	end

	add_defines( "LUA_BUILD_AS_DLL" )
	
    set_targetdir "../bin"
    set_objectdir "../build/obj"

    add_headerfiles( "**.h", "**.hpp" )
    add_files( "**.cpp" )
target_end()