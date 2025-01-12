target( "SMSE" )
    set_kind( "shared" )
    
	add_packages( "minhook", "luajit" )
	
	if is_mode( "Debug" ) then 
    	set_basename("SMSE_debug")
	else is_mode( "Release" )
    	set_basename("SMSE")
	end

	-- add_defines( "LUA_BUILD_AS_DLL" )
	
    set_targetdir "../bin/$(mode)/SMSE/"
    set_objectdir "../build/obj"

    add_headerfiles( "**.h", "**.hpp" )
    add_files( "**.cpp" )

	add_defines( "SMSE_CORE" )
target_end()