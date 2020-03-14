#set(COMPONENT_SRCS "main.c")

set(COMPONENT_ADD_INCLUDEDIRS "include")

idf_component_register(
	SRCS "main.cpp" "app_oled.cpp"
	REQUIRES ssd1306
	INCLUDE_DIRS "include")
                    
                    
#register_component()                    