local PLUGIN_NAME = "SMSEPlugin_Template"

target( PLUGIN_NAME )
    set_kind( "shared" )
    add_packages( "luajit" )
    
	set_basename( PLUGIN_NAME )

    set_targetdir "../bin/$(mode)/SMSE/Plugins/"
    set_objectdir "../build/obj"

    add_headerfiles( "**.h", "**.hpp" )
    add_files( "**.cpp" )
target_end()